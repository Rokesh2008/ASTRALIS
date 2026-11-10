#pragma once

#include "CoreMinimal.h"
#include "AstralisSimState.generated.h"

USTRUCT()
struct ASTRALISSIMULATION_API FSpacecraftOrbitalState
{
	GENERATED_BODY()

	// Position in inertial frame (metres)
	FVector3d Position_m = FVector3d::ZeroVector;

	// Velocity in inertial frame (m/s)
	FVector3d Velocity_mps = FVector3d::ZeroVector;

	// Simulation epoch (seconds since J2000 or mission start)
	double Epoch_s = 0.0;
};

USTRUCT()
struct ASTRALISSIMULATION_API FSpacecraftAttitudeState
{
	GENERATED_BODY()

	FQuat4d Attitude = FQuat4d::Identity;
	FVector3d AngularVelocity_radps = FVector3d::ZeroVector;
};

USTRUCT()
struct ASTRALISSIMULATION_API FSpacecraftMassState
{
	GENERATED_BODY()

	double DryMass_kg = 0.0;
	double PropellantMass_kg = 0.0;
	double TotalMass_kg = 0.0;
	FVector3d CenterOfMass_m = FVector3d::ZeroVector;

	// Inertia tensor (kg*m^2) — symmetric, stored as 6 unique elements
	// [Ixx, Iyy, Izz, Ixy, Ixz, Iyz]
	double InertiaTensor[6] = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
};

USTRUCT()
struct ASTRALISSIMULATION_API FSpacecraftPowerState
{
	GENERATED_BODY()

	double SolarGeneration_W = 0.0;
	double TotalConsumption_W = 0.0;
	double BatteryEnergy_J = 0.0;
	double BatteryCapacity_J = 0.0;
	bool bInEclipse = false;
};

USTRUCT()
struct ASTRALISSIMULATION_API FSpacecraftThermalState
{
	GENERATED_BODY()

	// Per-node temperatures stored separately; this is the summary
	double MinTemperature_C = 0.0;
	double MaxTemperature_C = 0.0;
	bool bAnyOvertemp = false;
	bool bAnyUndertemp = false;
};

USTRUCT()
struct ASTRALISSIMULATION_API FSpacecraftCommsState
{
	GENERATED_BODY()

	bool bGroundStationVisible = false;
	double LinkMargin_dB = 0.0;
	double AchievableDataRate_bps = 0.0;
	double DataBacklog_bytes = 0.0;
};

USTRUCT()
struct ASTRALISSIMULATION_API FSpacecraftState
{
	GENERATED_BODY()

	double SimTime_s = 0.0;

	FSpacecraftOrbitalState Orbital;
	FSpacecraftAttitudeState Attitude;
	FSpacecraftMassState Mass;
	FSpacecraftPowerState Power;
	FSpacecraftThermalState Thermal;
	FSpacecraftCommsState Comms;
};
