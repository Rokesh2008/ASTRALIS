#include "AstralisPropulsionSystem.h"

double FAstralisPropulsionSystem::ExhaustVelocity(double Isp_s)
{
	return Isp_s * G0;
}

double FAstralisPropulsionSystem::RocketEquationDeltaV(double Isp_s, double WetMass_kg, double DryMass_kg)
{
	if (DryMass_kg <= 0.0 || WetMass_kg <= DryMass_kg)
	{
		return 0.0;
	}
	return Isp_s * G0 * FMath::Loge(WetMass_kg / DryMass_kg);
}

double FAstralisPropulsionSystem::PropellantForDeltaV(double DeltaV_mps, double Isp_s, double DryMass_kg)
{
	if (Isp_s <= 0.0 || DryMass_kg <= 0.0)
	{
		return 0.0;
	}
	const double Ve = Isp_s * G0;
	// m0 = mf * exp(dv/ve), propellant = m0 - mf
	return DryMass_kg * (FMath::Exp(DeltaV_mps / Ve) - 1.0);
}

double FAstralisPropulsionSystem::MassFlowRate(double Thrust_N, double Isp_s)
{
	if (Isp_s <= 0.0)
	{
		return 0.0;
	}
	return Thrust_N / (Isp_s * G0);
}

double FAstralisPropulsionSystem::BurnDuration(
	double DeltaV_mps, double Thrust_N, double Isp_s, double InitialMass_kg)
{
	if (Thrust_N <= 0.0 || Isp_s <= 0.0 || InitialMass_kg <= 0.0)
	{
		return 0.0;
	}
	const double Ve = Isp_s * G0;
	const double Mdot = Thrust_N / Ve;
	// t = (m0/mdot) * (1 - exp(-dv/ve))
	return (InitialMass_kg / Mdot) * (1.0 - FMath::Exp(-DeltaV_mps / Ve));
}

FAstralisPropulsionSystem::FBurnResult FAstralisPropulsionSystem::SimulateBurn(
	double RequestedDeltaV_mps,
	double Thrust_N,
	double Isp_s,
	double InitialMass_kg,
	double AvailablePropellant_kg,
	double TimeStep_s)
{
	FBurnResult Result;
	Result.FinalMass_kg = InitialMass_kg;

	if (Thrust_N <= 0.0 || Isp_s <= 0.0 || InitialMass_kg <= 0.0)
	{
		return Result;
	}

	const double Ve = Isp_s * G0;
	const double Mdot = Thrust_N / Ve;
	double AccumulatedDv = 0.0;
	double CurrentMass = InitialMass_kg;
	double PropUsed = 0.0;
	double Time = 0.0;

	while (AccumulatedDv < RequestedDeltaV_mps)
	{
		const double RemainingProp = AvailablePropellant_kg - PropUsed;
		if (RemainingProp <= 0.0)
		{
			Result.bPropellantExhausted = true;
			break;
		}

		const double dt = FMath::Min(TimeStep_s, RemainingProp / Mdot);
		const double dm = Mdot * dt;
		const double dv = Thrust_N * dt / (CurrentMass - 0.5 * dm);

		AccumulatedDv += dv;
		PropUsed += dm;
		CurrentMass -= dm;
		Time += dt;

		if (AccumulatedDv >= RequestedDeltaV_mps)
		{
			break;
		}
	}

	Result.AchievedDeltaV_mps = AccumulatedDv;
	Result.PropellantConsumed_kg = PropUsed;
	Result.FinalMass_kg = CurrentMass;
	Result.BurnDuration_s = Time;
	return Result;
}
