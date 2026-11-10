#include "Misc/AutomationTest.h"
#include "AstralisSimEngine.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FSARActivationChainTest,
	"Astralis.Integration.SARActivationChain",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)

bool FSARActivationChainTest::RunTest(const FString& Parameters)
{
	FAstralisSimEngine Engine;

	// Configure LEO orbit: 400km altitude
	const double R = 6.778e6;
	const double V = FAstralisTwoBodyPropagator::CircularVelocity(R, FAstralisTwoBodyPropagator::Mu_Earth);

	FSpacecraftOrbitalState Orbit;
	Orbit.Position_m = FVector3d(R, 0.0, 0.0);
	Orbit.Velocity_mps = FVector3d(0.0, V, 0.0);
	Engine.SetOrbitalState(Orbit);

	// Configure subsystems
	FSolarArraySpec Solar;
	Solar.Area_m2 = 4.0;
	Solar.Efficiency = 0.30;
	Engine.SetSolarArray(Solar);

	FBatterySpec Batt;
	Batt.Capacity_Wh = 200.0;
	Batt.MaxDoD = 0.80;
	Batt.Efficiency = 0.95;
	Engine.SetBattery(Batt);

	FEngineSpec Prop;
	Prop.Isp_s = 220.0;
	Prop.MaxThrust_N = 10.0;
	Engine.SetEngine(Prop);
	Engine.SetPropellantMass(5.0);

	// Add spacecraft bus component
	FComponentMassEntry Bus;
	Bus.Mass_kg = 50.0;
	Bus.Position_m = FVector3d::ZeroVector;
	Engine.GetComponents().Add(Bus);

	// Add SAR instrument (illustrative spec, NOT a real flight instrument)
	FComponentMassEntry SAR;
	SAR.Mass_kg = 12.0;
	SAR.Position_m = FVector3d(0.5, 0.0, 0.0);
	Engine.GetComponents().Add(SAR);

	// Add thermal node for SAR
	FThermalNode SARThermal;
	SARThermal.NodeID = FName("SAR");
	SARThermal.Temperature_K = 293.15;
	SARThermal.HeatCapacity_JK = 600.0;
	SARThermal.InternalHeat_W = 0.0; // Will change when activated
	SARThermal.RadiatorArea_m2 = 0.05;
	SARThermal.Emissivity = 0.85;
	SARThermal.MinTemp_K = 253.15; // -20C
	SARThermal.MaxTemp_K = 318.15; // +45C
	Engine.GetThermalNodes().Add(SARThermal);

	// Initial state: SAR off
	FPowerLoadEntry SARPower;
	SARPower.SubsystemID = FName("SAR");
	SARPower.CurrentDraw_W = 4.0; // Standby
	SARPower.bIsActive = true;
	Engine.GetPowerLoads().Add(SARPower);

	// Initialize battery to full
	// State is internal — step once to initialize
	Engine.Step(1.0);

	const auto& StateBefore = Engine.GetState();
	const double PowerBefore = StateBefore.Power.TotalConsumption_W;

	// ACTIVATE SAR: increase power draw and thermal dissipation
	Engine.GetPowerLoads()[0].CurrentDraw_W = 55.0; // Average operating
	Engine.GetThermalNodes()[0].InternalHeat_W = 55.0;

	// Step for 60 seconds
	for (int i = 0; i < 60; ++i)
	{
		Engine.Step(1.0);
	}

	const auto& StateAfter = Engine.GetState();

	// 1. Power consumption should have increased
	TestTrue(TEXT("Power consumption increased after SAR activation"),
		StateAfter.Power.TotalConsumption_W > PowerBefore);

	// 2. Thermal state should show heating
	TestTrue(TEXT("SAR thermal node should be warming"),
		Engine.GetThermalNodes()[0].Temperature_K > 293.15);

	// 3. Mass properties should include SAR
	TestTrue(TEXT("Total mass should include SAR"),
		StateAfter.Mass.DryMass_kg >= 62.0); // Bus + SAR

	// 4. CoM should shift toward SAR position
	TestTrue(TEXT("CoM should be offset from origin"),
		StateAfter.Mass.CenterOfMass_m.X > 0.0);

	// 5. Communications should still work (LEO range)
	TestTrue(TEXT("Communications should have positive data rate"),
		StateAfter.Comms.AchievableDataRate_bps > 0.0);

	// 6. Orbit should still be propagating
	TestTrue(TEXT("Orbital position should have changed"),
		StateAfter.Orbital.Position_m != Orbit.Position_m);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FManeuverTest,
	"Astralis.Integration.OrbitalManeuver",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)

bool FManeuverTest::RunTest(const FString& Parameters)
{
	FAstralisSimEngine Engine;

	const double R = 6.778e6;
	const double V = FAstralisTwoBodyPropagator::CircularVelocity(R, FAstralisTwoBodyPropagator::Mu_Earth);

	FSpacecraftOrbitalState Orbit;
	Orbit.Position_m = FVector3d(R, 0.0, 0.0);
	Orbit.Velocity_mps = FVector3d(0.0, V, 0.0);
	Engine.SetOrbitalState(Orbit);

	FEngineSpec Prop;
	Prop.Isp_s = 300.0;
	Prop.MaxThrust_N = 500.0;
	Engine.SetEngine(Prop);
	Engine.SetPropellantMass(50.0);

	FComponentMassEntry Bus;
	Bus.Mass_kg = 200.0;
	Engine.GetComponents().Add(Bus);

	// Request a prograde maneuver of 100 m/s
	Engine.RequestManeuver(FVector3d(0.0, 100.0, 0.0));
	Engine.Step(1.0);

	const auto& State = Engine.GetState();

	// Velocity should have increased
	TestTrue(TEXT("Velocity should increase after prograde burn"),
		State.Orbital.Velocity_mps.Length() > V);

	// Propellant should have decreased
	TestTrue(TEXT("Propellant should decrease after burn"),
		State.Mass.PropellantMass_kg < 50.0);

	return true;
}
