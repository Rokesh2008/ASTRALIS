#include "Misc/AutomationTest.h"
#include "AstralisPropulsionSystem.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRocketEquationTest,
	"Astralis.Propulsion.RocketEquation",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)

bool FRocketEquationTest::RunTest(const FString& Parameters)
{
	// Known case: Isp=300s, wet=1000kg, dry=400kg
	// dv = 300 * 9.80665 * ln(1000/400) = 2694.8 m/s
	const double Dv = FAstralisPropulsionSystem::RocketEquationDeltaV(300.0, 1000.0, 400.0);
	const double Expected = 300.0 * 9.80665 * FMath::Loge(1000.0 / 400.0);
	const double Error = FMath::Abs(Dv - Expected);

	TestTrue(
		FString::Printf(TEXT("Delta-v error %.6f should be < 1e-6"), Error),
		Error < 1e-6);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FPropellantCalculationTest,
	"Astralis.Propulsion.PropellantForDeltaV",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)

bool FPropellantCalculationTest::RunTest(const FString& Parameters)
{
	// Round trip: compute propellant for a delta-v, then verify rocket equation matches
	const double Isp = 320.0;
	const double DryMass = 500.0;
	const double TargetDv = 2000.0;

	const double Propellant = FAstralisPropulsionSystem::PropellantForDeltaV(TargetDv, Isp, DryMass);
	const double VerifyDv = FAstralisPropulsionSystem::RocketEquationDeltaV(
		Isp, DryMass + Propellant, DryMass);

	TestTrue(
		FString::Printf(TEXT("Round-trip dv error %.6f < 1e-6"), FMath::Abs(VerifyDv - TargetDv)),
		FMath::Abs(VerifyDv - TargetDv) < 1e-6);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FBurnSimulationTest,
	"Astralis.Propulsion.BurnSimulation",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)

bool FBurnSimulationTest::RunTest(const FString& Parameters)
{
	const double Isp = 300.0;
	const double Thrust = 500.0;
	const double InitMass = 1000.0;
	const double Propellant = 600.0;
	const double RequestedDv = 2000.0;

	auto Result = FAstralisPropulsionSystem::SimulateBurn(
		RequestedDv, Thrust, Isp, InitMass, Propellant, 0.01);

	// Analytical delta-v available
	const double MaxDv = FAstralisPropulsionSystem::RocketEquationDeltaV(
		Isp, InitMass, InitMass - Propellant);

	if (RequestedDv <= MaxDv)
	{
		TestTrue(TEXT("Should achieve requested dv"),
			FMath::Abs(Result.AchievedDeltaV_mps - RequestedDv) / RequestedDv < 0.01);
		TestFalse(TEXT("Should not exhaust propellant"), Result.bPropellantExhausted);
	}

	// Verify mass conservation
	const double MassCheck = Result.FinalMass_kg + Result.PropellantConsumed_kg;
	TestTrue(TEXT("Mass conservation"),
		FMath::Abs(MassCheck - InitMass) < 0.1);

	return true;
}
