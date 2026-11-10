#include "Misc/AutomationTest.h"
#include "AstralisTwoBodyPropagator.h"

// Test A: Circular orbit maintains radius
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FCircularOrbitStabilityTest,
	"Astralis.Orbital.CircularOrbitStability",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)

bool FCircularOrbitStabilityTest::RunTest(const FString& Parameters)
{
	const double Mu = FAstralisTwoBodyPropagator::Mu_Earth;
	const double R = 6.778e6; // ~400km LEO
	const double V = FAstralisTwoBodyPropagator::CircularVelocity(R, Mu);

	FSpacecraftOrbitalState Initial;
	Initial.Position_m = FVector3d(R, 0.0, 0.0);
	Initial.Velocity_mps = FVector3d(0.0, V, 0.0);

	const double Period = FAstralisTwoBodyPropagator::OrbitalPeriod(R, Mu);
	const FSpacecraftOrbitalState Final =
		FAstralisTwoBodyPropagator::Propagate(Initial, Mu, Period, 10000);

	const double FinalR = Final.Position_m.Length();
	const double Error = FMath::Abs(FinalR - R) / R;

	TestTrue(
		FString::Printf(TEXT("Radius error %.2e should be < 1e-6"), Error),
		Error < 1e-6
	);

	return true;
}

// Test B: Orbital period matches analytical
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FOrbitalPeriodTest,
	"Astralis.Orbital.PeriodAccuracy",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)

bool FOrbitalPeriodTest::RunTest(const FString& Parameters)
{
	const double Mu = FAstralisTwoBodyPropagator::Mu_Earth;
	const double a = 7.0e6;
	const double AnalyticalPeriod = FAstralisTwoBodyPropagator::OrbitalPeriod(a, Mu);

	// Expected: T = 2*pi*sqrt(a^3/mu)
	const double Expected = 2.0 * UE_DOUBLE_PI * FMath::Sqrt(a * a * a / Mu);
	const double Error = FMath::Abs(AnalyticalPeriod - Expected);

	TestTrue(
		FString::Printf(TEXT("Period error %.2e s should be < 1e-6 s"), Error),
		Error < 1e-6
	);

	return true;
}

// Test C: Energy conservation over one orbit
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FEnergyConservationTest,
	"Astralis.Orbital.EnergyConservation",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)

bool FEnergyConservationTest::RunTest(const FString& Parameters)
{
	const double Mu = FAstralisTwoBodyPropagator::Mu_Earth;
	const double R = 7.0e6;
	const double V = FAstralisTwoBodyPropagator::CircularVelocity(R, Mu);

	FSpacecraftOrbitalState State;
	State.Position_m = FVector3d(R, 0.0, 0.0);
	State.Velocity_mps = FVector3d(0.0, V, 0.0);

	const double E0 = FAstralisTwoBodyPropagator::SpecificOrbitalEnergy(
		State.Position_m, State.Velocity_mps, Mu);

	const double Period = FAstralisTwoBodyPropagator::OrbitalPeriod(R, Mu);
	const FSpacecraftOrbitalState Final =
		FAstralisTwoBodyPropagator::Propagate(State, Mu, Period, 10000);

	const double Ef = FAstralisTwoBodyPropagator::SpecificOrbitalEnergy(
		Final.Position_m, Final.Velocity_mps, Mu);

	const double RelError = FMath::Abs(Ef - E0) / FMath::Abs(E0);

	TestTrue(
		FString::Printf(TEXT("Energy relative error %.2e should be < 1e-8"), RelError),
		RelError < 1e-8
	);

	return true;
}

// Test D: Angular momentum conservation
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FAngularMomentumTest,
	"Astralis.Orbital.AngularMomentumConservation",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)

bool FAngularMomentumTest::RunTest(const FString& Parameters)
{
	const double Mu = FAstralisTwoBodyPropagator::Mu_Earth;
	const double R = 7.0e6;
	const double V = FAstralisTwoBodyPropagator::CircularVelocity(R, Mu);

	FSpacecraftOrbitalState State;
	State.Position_m = FVector3d(R, 0.0, 0.0);
	State.Velocity_mps = FVector3d(0.0, V, 0.0);

	const FVector3d h0 = FAstralisTwoBodyPropagator::SpecificAngularMomentum(
		State.Position_m, State.Velocity_mps);

	const double Period = FAstralisTwoBodyPropagator::OrbitalPeriod(R, Mu);
	const FSpacecraftOrbitalState Final =
		FAstralisTwoBodyPropagator::Propagate(State, Mu, Period, 10000);

	const FVector3d hf = FAstralisTwoBodyPropagator::SpecificAngularMomentum(
		Final.Position_m, Final.Velocity_mps);

	const double RelError = (hf - h0).Length() / h0.Length();

	TestTrue(
		FString::Printf(TEXT("Angular momentum error %.2e should be < 1e-8"), RelError),
		RelError < 1e-8
	);

	return true;
}
