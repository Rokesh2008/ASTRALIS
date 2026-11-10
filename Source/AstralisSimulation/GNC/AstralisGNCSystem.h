#pragma once

#include "CoreMinimal.h"
#include "AstralisGNCSystem.generated.h"

UENUM(BlueprintType)
enum class EAttitudeMode : uint8
{
	Idle           UMETA(DisplayName = "Idle"),
	SunPointing    UMETA(DisplayName = "Sun Pointing"),
	NadirPointing  UMETA(DisplayName = "Nadir Pointing"),
	TargetPointing UMETA(DisplayName = "Target Pointing"),
	InertialHold   UMETA(DisplayName = "Inertial Hold"),
	Slewing        UMETA(DisplayName = "Slewing")
};

USTRUCT(BlueprintType)
struct ASTRALISSIMULATION_API FReactionWheelSpec
{
	GENERATED_BODY()

	// Maximum torque (N*m)
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	double MaxTorque_Nm = 0.01;

	// Maximum angular momentum storage (N*m*s)
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	double MaxMomentum_Nms = 0.5;

	// Current stored momentum fraction [0, 1]
	double StoredMomentumFraction = 0.0;

	// Power draw (Watts)
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	double Power_W = 5.0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	double Mass_kg = 1.0;
};

// Simplified GNC calculations.
class ASTRALISSIMULATION_API FAstralisGNCSystem
{
public:
	// Slew time estimate for a given angle (simplified constant-torque model)
	// Assumes bang-bang profile: accelerate half, decelerate half
	// t = 2 * sqrt(angle / alpha) where alpha = torque / inertia
	static double SlewTime(double Angle_rad, double Torque_Nm, double Inertia_kgm2);

	// Check if reaction wheels are saturated
	static bool IsWheelSaturated(const FReactionWheelSpec& Wheel);

	// Power required for current GNC mode
	static double GNCPowerDraw(EAttitudeMode Mode, const FReactionWheelSpec& Wheel);

	// Pointing error for a given sensor noise and control bandwidth (simplified)
	// Returns 3-sigma pointing error in radians
	static double PointingError3Sigma(double SensorNoise_rad, double ControlBandwidth_Hz);
};
