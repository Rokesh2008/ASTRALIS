#include "AstralisSimEngine.h"

FAstralisSimEngine::FAstralisSimEngine()
{
	State.SimTime_s = 0.0;
}

void FAstralisSimEngine::SetOrbitalState(const FSpacecraftOrbitalState& OrbState)
{
	State.Orbital = OrbState;
}

void FAstralisSimEngine::SetPropellantMass(double Mass_kg)
{
	State.Mass.PropellantMass_kg = Mass_kg;
}

void FAstralisSimEngine::RequestManeuver(const FVector3d& DeltaV_mps)
{
	PendingDeltaV = DeltaV_mps;
	bManeuverPending = true;
}

void FAstralisSimEngine::Step(double DeltaTime_s)
{
	if (DeltaTime_s <= 0.0)
	{
		return;
	}

	// Dependency order (Section 4.1 of GDD):
	// 1. Mass properties
	// 2. Propulsion (may change mass and velocity)
	// 3. Orbital propagation
	// 4. Power (depends on orbital position for eclipse/solar angle)
	// 5. Thermal (depends on power dissipation and orbital geometry)
	// 6. GNC (depends on mass/inertia for slew calculations)
	// 7. Communications (depends on orbital position for range/visibility)

	UpdateMassProperties();
	UpdatePropulsion(DeltaTime_s);
	UpdateOrbit(DeltaTime_s);
	UpdatePower(DeltaTime_s);
	UpdateThermal(DeltaTime_s);
	UpdateGNC(DeltaTime_s);
	UpdateCommunications();

	State.SimTime_s += DeltaTime_s;
}

void FAstralisSimEngine::UpdateMassProperties()
{
	State.Mass.DryMass_kg = FAstralisMassCalculator::TotalMass(Components);
	State.Mass.TotalMass_kg = State.Mass.DryMass_kg + State.Mass.PropellantMass_kg;
	State.Mass.CenterOfMass_m = FAstralisMassCalculator::CenterOfMass(Components);
	FAstralisMassCalculator::InertiaTensor(
		Components, State.Mass.CenterOfMass_m, State.Mass.InertiaTensor);
}

void FAstralisSimEngine::UpdatePropulsion(double DeltaTime_s)
{
	if (!bManeuverPending || State.Mass.PropellantMass_kg <= 0.0)
	{
		return;
	}

	const double RequestedDv = PendingDeltaV.Length();
	if (RequestedDv <= 0.0)
	{
		bManeuverPending = false;
		return;
	}

	auto BurnResult = FAstralisPropulsionSystem::SimulateBurn(
		RequestedDv,
		Engine.MaxThrust_N,
		Engine.Isp_s,
		State.Mass.TotalMass_kg,
		State.Mass.PropellantMass_kg,
		FMath::Min(DeltaTime_s, 0.1));

	// Apply delta-v to orbital velocity
	const FVector3d DvDirection = PendingDeltaV.GetSafeNormal();
	State.Orbital.Velocity_mps += DvDirection * BurnResult.AchievedDeltaV_mps;

	// Update propellant
	State.Mass.PropellantMass_kg -= BurnResult.PropellantConsumed_kg;
	State.Mass.PropellantMass_kg = FMath::Max(0.0, State.Mass.PropellantMass_kg);
	State.Mass.TotalMass_kg = State.Mass.DryMass_kg + State.Mass.PropellantMass_kg;

	bManeuverPending = false;
	PendingDeltaV = FVector3d::ZeroVector;
}

void FAstralisSimEngine::UpdateOrbit(double DeltaTime_s)
{
	// Adaptive step count based on time step
	const int32 Steps = FMath::Max(10, static_cast<int32>(DeltaTime_s * 10.0));

	State.Orbital = FAstralisTwoBodyPropagator::Propagate(
		State.Orbital, GravParam, DeltaTime_s, Steps);
}

void FAstralisSimEngine::UpdatePower(double DeltaTime_s)
{
	// Determine eclipse state (simplified: check if behind body)
	const double R = State.Orbital.Position_m.Length();
	State.Power.bInEclipse = false; // Simplified — full shadow model needed later

	// Solar generation
	const double IncidenceAngle = 0.0; // Simplified — needs attitude integration
	const double MissionYears = State.SimTime_s / (365.25 * 86400.0);
	State.Power.SolarGeneration_W = State.Power.bInEclipse ? 0.0 :
		FAstralisPowerSystem::SolarArrayPower(SolarArray, HelioDistance_AU, IncidenceAngle, MissionYears);

	// Total consumption
	State.Power.TotalConsumption_W = FAstralisPowerSystem::TotalLoad(PowerLoads);

	// Add GNC power
	State.Power.TotalConsumption_W += FAstralisGNCSystem::GNCPowerDraw(AttitudeMode, ReactionWheel);

	// Battery update
	const double NetPower = State.Power.SolarGeneration_W - State.Power.TotalConsumption_W;
	const double CurrentEnergy_Wh = State.Power.BatteryEnergy_J / 3600.0;
	const double NewEnergy_Wh = FAstralisPowerSystem::BatteryUpdate(
		Battery, CurrentEnergy_Wh, NetPower, DeltaTime_s);
	State.Power.BatteryEnergy_J = NewEnergy_Wh * 3600.0;
	State.Power.BatteryCapacity_J = Battery.Capacity_Wh * 3600.0;
}

void FAstralisSimEngine::UpdateThermal(double DeltaTime_s)
{
	const double SolarFlux = State.Power.bInEclipse ? 0.0 :
		FAstralisPowerSystem::S0 / (HelioDistance_AU * HelioDistance_AU);
	const double SpaceTemp = 2.7; // CMB

	State.Thermal.bAnyOvertemp = false;
	State.Thermal.bAnyUndertemp = false;
	State.Thermal.MinTemperature_C = 1000.0;
	State.Thermal.MaxTemperature_C = -1000.0;

	for (auto& Node : ThermalNodes)
	{
		FAstralisThermalSystem::UpdateNode(Node, SolarFlux, SpaceTemp, DeltaTime_s);

		const double TempC = Node.Temperature_K - 273.15;
		State.Thermal.MinTemperature_C = FMath::Min(State.Thermal.MinTemperature_C, TempC);
		State.Thermal.MaxTemperature_C = FMath::Max(State.Thermal.MaxTemperature_C, TempC);

		if (!FAstralisThermalSystem::IsWithinLimits(Node))
		{
			if (Node.Temperature_K > Node.MaxTemp_K) State.Thermal.bAnyOvertemp = true;
			if (Node.Temperature_K < Node.MinTemp_K) State.Thermal.bAnyUndertemp = true;
		}
	}

	if (ThermalNodes.Num() == 0)
	{
		State.Thermal.MinTemperature_C = 0.0;
		State.Thermal.MaxTemperature_C = 0.0;
	}
}

void FAstralisSimEngine::UpdateGNC(double /*DeltaTime_s*/)
{
	// GNC state updates — simplified for M3
	// Full attitude propagation will be added in M5
}

void FAstralisSimEngine::UpdateCommunications()
{
	const double Range = State.Orbital.Position_m.Length();

	auto LinkResult = FAstralisCommsSystem::ComputeLinkBudget(
		Antenna, GroundStation, Range, 10.0);

	State.Comms.bGroundStationVisible = true; // Simplified — needs visibility calc
	State.Comms.LinkMargin_dB = LinkResult.LinkMargin_dB;
	State.Comms.AchievableDataRate_bps = LinkResult.AchievableDataRate_bps;
}
