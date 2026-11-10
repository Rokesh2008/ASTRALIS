#include "AstralisTwoBodyPropagator.h"

struct FStateVector6
{
	FVector3d R;
	FVector3d V;
};

static FStateVector6 Derivative(const FStateVector6& S, double Mu)
{
	const double r = S.R.Length();
	const double r3 = r * r * r;
	return { S.V, -Mu / r3 * S.R };
}

static FStateVector6 operator+(const FStateVector6& A, const FStateVector6& B)
{
	return { A.R + B.R, A.V + B.V };
}

static FStateVector6 operator*(double s, const FStateVector6& A)
{
	return { s * A.R, s * A.V };
}

FSpacecraftOrbitalState FAstralisTwoBodyPropagator::Propagate(
	const FSpacecraftOrbitalState& State,
	double Mu,
	double DeltaTime_s,
	int32 NumSteps)
{
	const double h = DeltaTime_s / static_cast<double>(NumSteps);
	FStateVector6 S = { State.Position_m, State.Velocity_mps };

	for (int32 i = 0; i < NumSteps; ++i)
	{
		const FStateVector6 k1 = Derivative(S, Mu);
		const FStateVector6 k2 = Derivative(S + 0.5 * h * k1, Mu);
		const FStateVector6 k3 = Derivative(S + 0.5 * h * k2, Mu);
		const FStateVector6 k4 = Derivative(S + h * k3, Mu);

		S = S + (h / 6.0) * (k1 + 2.0 * k2 + 2.0 * k3 + k4);
	}

	FSpacecraftOrbitalState Result;
	Result.Position_m = S.R;
	Result.Velocity_mps = S.V;
	Result.Epoch_s = State.Epoch_s + DeltaTime_s;
	return Result;
}

FVector3d FAstralisTwoBodyPropagator::GravitationalAcceleration(const FVector3d& Position_m, double Mu)
{
	const double r = Position_m.Length();
	return -Mu / (r * r * r) * Position_m;
}

double FAstralisTwoBodyPropagator::CircularVelocity(double Radius_m, double Mu)
{
	return FMath::Sqrt(Mu / Radius_m);
}

double FAstralisTwoBodyPropagator::OrbitalPeriod(double SemiMajorAxis_m, double Mu)
{
	return 2.0 * UE_DOUBLE_PI * FMath::Sqrt(
		SemiMajorAxis_m * SemiMajorAxis_m * SemiMajorAxis_m / Mu
	);
}

double FAstralisTwoBodyPropagator::SpecificOrbitalEnergy(
	const FVector3d& Position_m,
	const FVector3d& Velocity_mps,
	double Mu)
{
	return 0.5 * Velocity_mps.SquaredLength() - Mu / Position_m.Length();
}

FVector3d FAstralisTwoBodyPropagator::SpecificAngularMomentum(
	const FVector3d& Position_m,
	const FVector3d& Velocity_mps)
{
	return FVector3d::CrossProduct(Position_m, Velocity_mps);
}
