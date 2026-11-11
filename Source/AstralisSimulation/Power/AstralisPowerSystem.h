#pragma once

#include "CoreMinimal.h"
#include "AstralisPowerSystem.generated.h"

USTRUCT(BlueprintType)
struct ASTRALISSIMULATION_API FSolarArraySpec
{
	GENERATED_BODY()

	// Active area (m^2)
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	double Area_m2 = 2.0;

	// Conversion efficiency [0, 1]
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	double Efficiency = 0.30;

	// Annual degradation rate [0, 1]
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	double DegradationPerYear = 0.02;
};

USTRUCT(BlueprintType)
struct ASTRALISSIMULATION_API FBatterySpec
{
	GENERATED_BODY()

	// Capacity in watt-hours
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (ToolTip = "Battery capacity (Watt-hours)"))
	double Capacity_Wh = 100.0;

	// Maximum depth of discharge [0, 1]
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	double MaxDoD = 0.80;

	// Round-trip efficiency [0, 1]
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	double Efficiency = 0.95;

	// Mass (kg)
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	double Mass_kg = 3.0;
};

USTRUCT(BlueprintType)
struct ASTRALISSIMULATION_API FPowerLoadEntry
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FName SubsystemID;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (ToolTip = "Current power draw (Watts)"))
	double CurrentDraw_W = 0.0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bIsActive = true;
};

// Power system calculations — all SI units.
class ASTRALISSIMULATION_API FAstralisPowerSystem
{
public:
	// Solar irradiance at 1 AU (W/m^2)
	static constexpr double S0 = 1361.0;

	// Solar array output at given heliocentric distance and incidence angle.
	// P = S0 * (1AU/r)^2 * A * eta * cos(theta) * degradation
	static double SolarArrayPower(
		const FSolarArraySpec& Spec,
		double HeliocentricDistance_AU,
		double IncidenceAngle_rad,
		double MissionYears);

	// Battery state of charge after a time step.
	// Positive NetPower = charging, negative = discharging.
	// Returns new energy stored in Wh.
	static double BatteryUpdate(
		const FBatterySpec& Spec,
		double CurrentEnergy_Wh,
		double NetPower_W,
		double DeltaTime_s);

	// Usable battery energy considering max depth of discharge
	static double UsableEnergy_Wh(const FBatterySpec& Spec, double CurrentEnergy_Wh);

	// Total power consumption from all active loads
	static double TotalLoad(const TArray<FPowerLoadEntry>& Loads);

	// Eclipse duration for circular orbit (seconds)
	// Simplified: assumes cylindrical shadow
	static double EclipseDuration(double OrbitRadius_m, double BodyRadius_m, double OrbitalPeriod_s);

	// Check if battery can survive an eclipse at given load
	static bool CanSurviveEclipse(
		const FBatterySpec& Spec,
		double CurrentEnergy_Wh,
		double EclipseLoad_W,
		double EclipseDuration_s);
};
