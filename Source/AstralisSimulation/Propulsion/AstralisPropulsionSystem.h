#pragma once

#include "CoreMinimal.h"
#include "AstralisPropulsionSystem.generated.h"

UENUM(BlueprintType)
enum class EPropulsionType : uint8
{
	Chemical       UMETA(DisplayName = "Chemical Bipropellant"),
	Monopropellant UMETA(DisplayName = "Monopropellant"),
	ColdGas        UMETA(DisplayName = "Cold Gas"),
	IonElectric    UMETA(DisplayName = "Ion Electric"),
	HallEffect     UMETA(DisplayName = "Hall-Effect Thruster")
};

USTRUCT(BlueprintType)
struct ASTRALISSIMULATION_API FEngineSpec
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FName EngineID;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	EPropulsionType Type = EPropulsionType::Chemical;

	// Specific impulse (seconds)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (Units = "Seconds"))
	double Isp_s = 300.0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (Units = "Newtons"))
	double MaxThrust_N = 500.0;

	// Minimum throttle fraction [0, 1]
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	double MinThrottle = 1.0;

	// Electrical power required during operation (Watts) — relevant for electric propulsion
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (ToolTip = "Electrical power required (Watts)"))
	double PowerRequired_W = 0.0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (Units = "Kilograms"))
	double Mass_kg = 5.0;
};

// Propulsion system state and calculations.
// All SI units, double precision.
class ASTRALISSIMULATION_API FAstralisPropulsionSystem
{
public:
	static constexpr double G0 = 9.80665; // m/s^2

	// Exhaust velocity from Isp
	static double ExhaustVelocity(double Isp_s);

	// Tsiolkovsky delta-v: Isp * g0 * ln(m0 / mf)
	static double RocketEquationDeltaV(double Isp_s, double WetMass_kg, double DryMass_kg);

	// Propellant required for a given delta-v
	static double PropellantForDeltaV(double DeltaV_mps, double Isp_s, double DryMass_kg);

	// Mass flow rate at given thrust: mdot = F / (Isp * g0)
	static double MassFlowRate(double Thrust_N, double Isp_s);

	// Burn duration for given delta-v at constant thrust
	// Accounts for changing mass during burn
	static double BurnDuration(double DeltaV_mps, double Thrust_N, double Isp_s, double InitialMass_kg);

	// Simulate a finite burn: returns final mass and achieved delta-v
	// Steps through changing mass over the burn
	struct FBurnResult
	{
		double AchievedDeltaV_mps = 0.0;
		double PropellantConsumed_kg = 0.0;
		double FinalMass_kg = 0.0;
		double BurnDuration_s = 0.0;
		bool bPropellantExhausted = false;
	};

	static FBurnResult SimulateBurn(
		double RequestedDeltaV_mps,
		double Thrust_N,
		double Isp_s,
		double InitialMass_kg,
		double AvailablePropellant_kg,
		double TimeStep_s = 0.1);
};
