#pragma once

#include "CoreMinimal.h"
#include "AstralisThermalSystem.generated.h"

USTRUCT(BlueprintType)
struct ASTRALISSIMULATION_API FThermalNode
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FName NodeID;

	// Current temperature (Kelvin)
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	double Temperature_K = 293.15;

	// Heat capacity (J/K)
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	double HeatCapacity_JK = 500.0;

	// Radiator area (m^2)
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	double RadiatorArea_m2 = 0.0;

	// Emissivity [0, 1]
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	double Emissivity = 0.85;

	// Internal heat dissipation (Watts)
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	double InternalHeat_W = 0.0;

	// Solar absorptivity [0, 1]
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	double SolarAbsorptivity = 0.3;

	// Projected area exposed to Sun (m^2)
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	double SolarArea_m2 = 0.0;

	// Operating limits (Kelvin)
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	double MinTemp_K = 233.15; // -40C

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	double MaxTemp_K = 333.15; // +60C
};

// Thermal calculations — simplified lumped-node model.
class ASTRALISSIMULATION_API FAstralisThermalSystem
{
public:
	// Stefan-Boltzmann constant (W/m^2/K^4)
	static constexpr double Sigma = 5.670374419e-8;

	// Radiative heat rejection: Q = epsilon * sigma * A * (T^4 - Tenv^4)
	static double RadiativeHeatRejection(double Emissivity, double Area_m2,
		double Temp_K, double EnvironmentTemp_K);

	// Solar heat input: Q = alpha * S * A_projected
	// S is local solar flux (W/m^2)
	static double SolarHeatInput(double SolarAbsorptivity, double SolarFlux_Wm2,
		double ProjectedArea_m2);

	// Update a single thermal node for one time step.
	// Returns new temperature in Kelvin.
	static double UpdateNode(FThermalNode& Node, double SolarFlux_Wm2,
		double EnvironmentTemp_K, double DeltaTime_s);

	// Equilibrium temperature for a simple body (no internal heat, steady state)
	// T_eq = (alpha * S * A_solar / (epsilon * sigma * A_rad))^(1/4)
	static double EquilibriumTemperature(double SolarAbsorptivity, double SolarFlux_Wm2,
		double SolarArea_m2, double Emissivity, double RadiatorArea_m2);

	// Check if node is within operating limits
	static bool IsWithinLimits(const FThermalNode& Node);
};
