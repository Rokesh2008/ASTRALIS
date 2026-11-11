#pragma once

#include "CoreMinimal.h"
#include "AstralisSimulation/SimCore/AstralisSimState.h"

// Two-body orbital propagator using RK4 integration.
// All calculations in SI units, double precision.
// mu = GM of central body in m^3/s^2.
class ASTRALISSIMULATION_API FAstralisTwoBodyPropagator
{
public:
	// Standard gravitational parameters (m^3/s^2)
	static constexpr double Mu_Earth = 3.986004418e14;
	static constexpr double Mu_Moon  = 4.9048695e12;
	static constexpr double Mu_Sun   = 1.32712440018e20;
	static constexpr double Mu_Mars  = 4.282837e13;

	// Propagate orbital state forward by DeltaTime seconds.
	// Uses RK4 with fixed step; NumSteps controls accuracy vs cost.
	static FSpacecraftOrbitalState Propagate(
		const FSpacecraftOrbitalState& State,
		double Mu,
		double DeltaTime_s,
		int32 NumSteps = 100
	);

	// Compute gravitational acceleration at Position.
	static FVector3d GravitationalAcceleration(const FVector3d& Position_m, double Mu);

	// Compute circular orbit velocity for given radius.
	static double CircularVelocity(double Radius_m, double Mu);

	// Compute orbital period for given semi-major axis.
	static double OrbitalPeriod(double SemiMajorAxis_m, double Mu);

	// Compute specific orbital energy.
	static double SpecificOrbitalEnergy(const FVector3d& Position_m, const FVector3d& Velocity_mps, double Mu);

	// Compute specific angular momentum vector.
	static FVector3d SpecificAngularMomentum(const FVector3d& Position_m, const FVector3d& Velocity_mps);
};
