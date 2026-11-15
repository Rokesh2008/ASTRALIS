#pragma once

#include "CoreMinimal.h"
#include "AstralisVehicleTypes.h"

class ASTRALISDATA_API FAstralisPerformanceCalculator
{
public:
	static double CalculateStageDeltaV(const FStageDefinition& Stage, double PayloadMass_kg);

	static double CalculateVehicleDeltaV(const FLaunchVehicleDefinition& Vehicle);

	static double CalculateTWR(const FStageDefinition& Stage, double UpperMass_kg);

	static double CalculateMaxQ_Pa(double Velocity_mps, double Altitude_m);

	// Returns empty string if vehicle is valid. Otherwise the first issue found.
	static FString ValidateVehicle(const FLaunchVehicleDefinition& Vehicle);

private:
	static constexpr double g0 = 9.80665;

	static double AtmosphericDensity(double Altitude_m);
};
