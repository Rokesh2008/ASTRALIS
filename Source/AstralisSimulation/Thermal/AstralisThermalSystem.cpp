#include "AstralisThermalSystem.h"

double FAstralisThermalSystem::RadiativeHeatRejection(
	double Emissivity, double Area_m2, double Temp_K, double EnvironmentTemp_K)
{
	if (Area_m2 <= 0.0 || Temp_K <= 0.0)
	{
		return 0.0;
	}
	const double T4 = Temp_K * Temp_K * Temp_K * Temp_K;
	const double Tenv4 = EnvironmentTemp_K * EnvironmentTemp_K * EnvironmentTemp_K * EnvironmentTemp_K;
	return Emissivity * Sigma * Area_m2 * (T4 - Tenv4);
}

double FAstralisThermalSystem::SolarHeatInput(
	double SolarAbsorptivity, double SolarFlux_Wm2, double ProjectedArea_m2)
{
	return SolarAbsorptivity * SolarFlux_Wm2 * ProjectedArea_m2;
}

double FAstralisThermalSystem::UpdateNode(
	FThermalNode& Node, double SolarFlux_Wm2,
	double EnvironmentTemp_K, double DeltaTime_s)
{
	if (Node.HeatCapacity_JK <= 0.0 || DeltaTime_s <= 0.0)
	{
		return Node.Temperature_K;
	}

	const double QSolar = SolarHeatInput(Node.SolarAbsorptivity, SolarFlux_Wm2, Node.SolarArea_m2);
	const double QRad = RadiativeHeatRejection(Node.Emissivity, Node.RadiatorArea_m2,
		Node.Temperature_K, EnvironmentTemp_K);
	const double QNet = Node.InternalHeat_W + QSolar - QRad;

	Node.Temperature_K += (QNet / Node.HeatCapacity_JK) * DeltaTime_s;

	if (Node.Temperature_K < 0.0)
	{
		Node.Temperature_K = 0.0;
	}

	return Node.Temperature_K;
}

double FAstralisThermalSystem::EquilibriumTemperature(
	double SolarAbsorptivity, double SolarFlux_Wm2, double SolarArea_m2,
	double Emissivity, double RadiatorArea_m2)
{
	if (Emissivity <= 0.0 || RadiatorArea_m2 <= 0.0)
	{
		return 0.0;
	}

	const double Numerator = SolarAbsorptivity * SolarFlux_Wm2 * SolarArea_m2;
	const double Denominator = Emissivity * Sigma * RadiatorArea_m2;

	return FMath::Pow(Numerator / Denominator, 0.25);
}

bool FAstralisThermalSystem::IsWithinLimits(const FThermalNode& Node)
{
	return Node.Temperature_K >= Node.MinTemp_K && Node.Temperature_K <= Node.MaxTemp_K;
}
