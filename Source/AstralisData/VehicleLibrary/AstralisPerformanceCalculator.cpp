#include "AstralisPerformanceCalculator.h"

double FAstralisPerformanceCalculator::CalculateStageDeltaV(
	const FStageDefinition& Stage, double PayloadMass_kg)
{
	const double Isp = (Stage.EngineSpec.IspVacuum_s > 0.0)
		? Stage.EngineSpec.IspVacuum_s
		: Stage.EngineSpec.IspSeaLevel_s;

	if (Isp <= 0.0) return 0.0;

	const double M0 = PayloadMass_kg + Stage.DryMass_kg + Stage.PropellantCapacity_kg;
	const double M1 = PayloadMass_kg + Stage.DryMass_kg;

	if (M0 <= 0.0 || M1 <= 0.0 || M0 <= M1) return 0.0;

	return Isp * g0 * FMath::Loge(M0 / M1);
}

double FAstralisPerformanceCalculator::CalculateVehicleDeltaV(
	const FLaunchVehicleDefinition& Vehicle)
{
	double TotalDV = 0.0;
	double PayloadAbove = 0.0;

	// Process from top stage down
	for (int32 i = Vehicle.Stages.Num() - 1; i >= 0; --i)
	{
		const double StageDV = CalculateStageDeltaV(Vehicle.Stages[i], PayloadAbove);
		TotalDV += StageDV;

		// This stage becomes payload for the stage below it
		PayloadAbove += Vehicle.Stages[i].DryMass_kg + Vehicle.Stages[i].PropellantCapacity_kg;
	}

	return TotalDV;
}

double FAstralisPerformanceCalculator::CalculateTWR(
	const FStageDefinition& Stage, double UpperMass_kg)
{
	const double Thrust = Stage.EngineSpec.ThrustSeaLevel_N * Stage.NumEngines;
	if (Thrust <= 0.0) return 0.0;

	const double TotalMass = UpperMass_kg + Stage.DryMass_kg + Stage.PropellantCapacity_kg;
	if (TotalMass <= 0.0) return 0.0;

	return Thrust / (TotalMass * g0);
}

double FAstralisPerformanceCalculator::AtmosphericDensity(double Altitude_m)
{
	// Exponential atmosphere model — sea-level density 1.225 kg/m^3, scale height 8500 m
	const double Rho0 = 1.225;
	const double H = 8500.0;
	return Rho0 * FMath::Exp(-Altitude_m / H);
}

double FAstralisPerformanceCalculator::CalculateMaxQ_Pa(
	double Velocity_mps, double Altitude_m)
{
	const double Rho = AtmosphericDensity(Altitude_m);
	return 0.5 * Rho * Velocity_mps * Velocity_mps;
}

FString FAstralisPerformanceCalculator::ValidateVehicle(
	const FLaunchVehicleDefinition& Vehicle)
{
	if (Vehicle.Stages.Num() == 0)
	{
		return TEXT("Vehicle has no stages defined.");
	}

	// Check first stage TWR (must exceed 1.0 for liftoff)
	{
		double MassAboveFirstStage = 0.0;
		for (int32 i = 1; i < Vehicle.Stages.Num(); ++i)
		{
			MassAboveFirstStage += Vehicle.Stages[i].DryMass_kg
				+ Vehicle.Stages[i].PropellantCapacity_kg;
		}

		const double TWR = CalculateTWR(Vehicle.Stages[0], MassAboveFirstStage);
		if (TWR < 1.0)
		{
			return FString::Printf(
				TEXT("First stage TWR is %.2f — must exceed 1.0 for liftoff."), TWR);
		}
	}

	// Check propellant/engine compatibility per stage
	for (int32 i = 0; i < Vehicle.Stages.Num(); ++i)
	{
		const FStageDefinition& S = Vehicle.Stages[i];
		if (S.EngineSpec.PropellantType != S.PropellantType)
		{
			return FString::Printf(
				TEXT("Stage %d: engine requires different propellant than the stage supplies."),
				S.StageNumber);
		}

		// Solid motors should not appear with liquid propellant
		if (S.EngineSpec.EngineType == EEngineType::SolidMotor &&
			S.PropellantType != EPropellantType::SolidHTPB)
		{
			return FString::Printf(
				TEXT("Stage %d: solid motor specified with non-solid propellant."),
				S.StageNumber);
		}
	}

	// Check interface compatibility between adjacent stages
	for (int32 i = 0; i < Vehicle.Stages.Num() - 1; ++i)
	{
		const FStageDefinition& Lower = Vehicle.Stages[i];
		const FStageDefinition& Upper = Vehicle.Stages[i + 1];

		if (Lower.UpperInterface != EAttachmentInterface::Custom &&
			Upper.LowerInterface != EAttachmentInterface::Custom &&
			Lower.UpperInterface != Upper.LowerInterface)
		{
			return FString::Printf(
				TEXT("Interface mismatch between Stage %d (upper) and Stage %d (lower)."),
				Lower.StageNumber, Upper.StageNumber);
		}
	}

	// Check total delta-v sufficient for LEO (~9400 m/s including gravity/drag losses)
	const double DV = CalculateVehicleDeltaV(Vehicle);
	if (DV < 9400.0)
	{
		return FString::Printf(
			TEXT("Total delta-v is %.0f m/s — approximately 9400 m/s required for LEO (including losses)."),
			DV);
	}

	return FString(); // valid
}
