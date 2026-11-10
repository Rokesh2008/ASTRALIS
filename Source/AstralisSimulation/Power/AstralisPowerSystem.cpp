#include "AstralisPowerSystem.h"

double FAstralisPowerSystem::SolarArrayPower(
	const FSolarArraySpec& Spec,
	double HeliocentricDistance_AU,
	double IncidenceAngle_rad,
	double MissionYears)
{
	if (HeliocentricDistance_AU <= 0.0 || Spec.Area_m2 <= 0.0)
	{
		return 0.0;
	}

	const double CosTheta = FMath::Cos(IncidenceAngle_rad);
	if (CosTheta <= 0.0)
	{
		return 0.0; // Array facing away from Sun
	}

	const double InverseSquare = 1.0 / (HeliocentricDistance_AU * HeliocentricDistance_AU);
	const double Degradation = FMath::Pow(1.0 - Spec.DegradationPerYear, MissionYears);

	return S0 * InverseSquare * Spec.Area_m2 * Spec.Efficiency * CosTheta * Degradation;
}

double FAstralisPowerSystem::BatteryUpdate(
	const FBatterySpec& Spec,
	double CurrentEnergy_Wh,
	double NetPower_W,
	double DeltaTime_s)
{
	const double DeltaTime_h = DeltaTime_s / 3600.0;
	double DeltaEnergy_Wh;

	if (NetPower_W >= 0.0)
	{
		// Charging — apply efficiency loss
		DeltaEnergy_Wh = NetPower_W * DeltaTime_h * Spec.Efficiency;
	}
	else
	{
		// Discharging — draw more from battery due to efficiency
		DeltaEnergy_Wh = NetPower_W * DeltaTime_h / Spec.Efficiency;
	}

	double NewEnergy = CurrentEnergy_Wh + DeltaEnergy_Wh;

	// Clamp to [0, Capacity]
	NewEnergy = FMath::Clamp(NewEnergy, 0.0, Spec.Capacity_Wh);

	return NewEnergy;
}

double FAstralisPowerSystem::UsableEnergy_Wh(const FBatterySpec& Spec, double CurrentEnergy_Wh)
{
	const double MinEnergy = Spec.Capacity_Wh * (1.0 - Spec.MaxDoD);
	return FMath::Max(0.0, CurrentEnergy_Wh - MinEnergy);
}

double FAstralisPowerSystem::TotalLoad(const TArray<FPowerLoadEntry>& Loads)
{
	double Total = 0.0;
	for (const auto& L : Loads)
	{
		if (L.bIsActive)
		{
			Total += L.CurrentDraw_W;
		}
	}
	return Total;
}

double FAstralisPowerSystem::EclipseDuration(
	double OrbitRadius_m, double BodyRadius_m, double OrbitalPeriod_s)
{
	if (OrbitRadius_m <= BodyRadius_m)
	{
		return OrbitalPeriod_s; // Inside body — perpetual eclipse
	}

	// Shadow half-angle for cylindrical approximation
	const double SinAlpha = BodyRadius_m / OrbitRadius_m;
	const double Alpha = FMath::Asin(SinAlpha);
	return OrbitalPeriod_s * (2.0 * Alpha) / (2.0 * UE_DOUBLE_PI);
}

bool FAstralisPowerSystem::CanSurviveEclipse(
	const FBatterySpec& Spec,
	double CurrentEnergy_Wh,
	double EclipseLoad_W,
	double EclipseDuration_s)
{
	const double EclipseEnergy_Wh = EclipseLoad_W * (EclipseDuration_s / 3600.0) / Spec.Efficiency;
	const double Usable = UsableEnergy_Wh(Spec, CurrentEnergy_Wh);
	return Usable >= EclipseEnergy_Wh;
}
