#pragma once

#include "CoreMinimal.h"
#include "AstralisSimState.h"
#include "AstralisSimClock.h"
#include "AstralisSimulation/OrbitalMechanics/AstralisTwoBodyPropagator.h"
#include "AstralisSimulation/Propulsion/AstralisPropulsionSystem.h"
#include "AstralisSimulation/Power/AstralisPowerSystem.h"
#include "AstralisSimulation/Thermal/AstralisThermalSystem.h"
#include "AstralisSimulation/Communications/AstralisCommsSystem.h"
#include "AstralisSimulation/GNC/AstralisGNCSystem.h"
#include "AstralisSimulation/MassProperties/AstralisMassCalculator.h"

// Central simulation engine. Evaluates all subsystems in dependency order.
// Owns the authoritative spacecraft state. No UE rendering dependencies.
class ASTRALISSIMULATION_API FAstralisSimEngine
{
public:
	FAstralisSimEngine();

	// Configuration
	void SetGravitationalParameter(double Mu) { GravParam = Mu; }
	void SetCentralBodyRadius(double Radius_m) { BodyRadius_m = Radius_m; }
	void SetHeliocentricDistance(double Distance_AU) { HelioDistance_AU = Distance_AU; }

	// Solar array and battery configuration
	void SetSolarArray(const FSolarArraySpec& Spec) { SolarArray = Spec; }
	void SetBattery(const FBatterySpec& Spec) { Battery = Spec; }

	// Propulsion configuration
	void SetEngine(const FEngineSpec& Spec) { Engine = Spec; }
	void SetPropellantMass(double Mass_kg);

	// GNC
	void SetAttitudeMode(EAttitudeMode Mode) { AttitudeMode = Mode; }
	void SetReactionWheel(const FReactionWheelSpec& Spec) { ReactionWheel = Spec; }

	// Communications
	void SetAntenna(const FAntennaSpec& Spec) { Antenna = Spec; }
	void SetGroundStation(const FGroundStationSpec& Spec) { GroundStation = Spec; }

	// Thermal nodes
	TArray<FThermalNode>& GetThermalNodes() { return ThermalNodes; }

	// Power loads
	TArray<FPowerLoadEntry>& GetPowerLoads() { return PowerLoads; }

	// Component mass entries
	TArray<FComponentMassEntry>& GetComponents() { return Components; }

	// Set initial orbital state
	void SetOrbitalState(const FSpacecraftOrbitalState& State);

	// Main simulation step — evaluates all subsystems in dependency order
	void Step(double DeltaTime_s);

	// Get current state (read-only)
	const FSpacecraftState& GetState() const { return State; }
	FAstralisSimClock& GetClock() { return Clock; }
	const FAstralisSimClock& GetClock() const { return Clock; }

	// Pending maneuver
	void RequestManeuver(const FVector3d& DeltaV_mps);
	bool HasPendingManeuver() const { return bManeuverPending; }

private:
	FSpacecraftState State;
	FAstralisSimClock Clock;

	// Central body
	double GravParam = FAstralisTwoBodyPropagator::Mu_Earth;
	double BodyRadius_m = 6.371e6;
	double HelioDistance_AU = 1.0;

	// Subsystem specs
	FSolarArraySpec SolarArray;
	FBatterySpec Battery;
	FEngineSpec Engine;
	FAntennaSpec Antenna;
	FGroundStationSpec GroundStation;
	FReactionWheelSpec ReactionWheel;
	EAttitudeMode AttitudeMode = EAttitudeMode::Idle;

	// Component data
	TArray<FComponentMassEntry> Components;
	TArray<FThermalNode> ThermalNodes;
	TArray<FPowerLoadEntry> PowerLoads;

	// Maneuver
	bool bManeuverPending = false;
	FVector3d PendingDeltaV = FVector3d::ZeroVector;

	// Subsystem update methods (called in dependency order)
	void UpdateMassProperties();
	void UpdatePropulsion(double DeltaTime_s);
	void UpdateOrbit(double DeltaTime_s);
	void UpdatePower(double DeltaTime_s);
	void UpdateThermal(double DeltaTime_s);
	void UpdateGNC(double DeltaTime_s);
	void UpdateCommunications();
};
