#include "AstralisGNCSystem.h"

double FAstralisGNCSystem::SlewTime(double Angle_rad, double Torque_Nm, double Inertia_kgm2)
{
	if (Torque_Nm <= 0.0 || Inertia_kgm2 <= 0.0 || Angle_rad <= 0.0)
	{
		return 0.0;
	}
	const double Alpha = Torque_Nm / Inertia_kgm2;
	return 2.0 * FMath::Sqrt(Angle_rad / Alpha);
}

bool FAstralisGNCSystem::IsWheelSaturated(const FReactionWheelSpec& Wheel)
{
	return Wheel.StoredMomentumFraction >= 0.95;
}

double FAstralisGNCSystem::GNCPowerDraw(EAttitudeMode Mode, const FReactionWheelSpec& Wheel)
{
	switch (Mode)
	{
	case EAttitudeMode::Idle:
		return Wheel.Power_W * 0.1;
	case EAttitudeMode::Slewing:
		return Wheel.Power_W * 1.5;
	default:
		return Wheel.Power_W;
	}
}

double FAstralisGNCSystem::PointingError3Sigma(double SensorNoise_rad, double ControlBandwidth_Hz)
{
	if (ControlBandwidth_Hz <= 0.0)
	{
		return UE_DOUBLE_PI;
	}
	// Simplified: error scales with noise and inversely with bandwidth
	return 3.0 * SensorNoise_rad / FMath::Sqrt(ControlBandwidth_Hz);
}
