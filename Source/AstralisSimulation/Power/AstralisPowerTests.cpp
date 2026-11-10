#include "Misc/AutomationTest.h"
#include "AstralisPowerSystem.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FSolarArrayAtOneAUTest,
	"Astralis.Power.SolarArrayAt1AU",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)

bool FSolarArrayAtOneAUTest::RunTest(const FString& Parameters)
{
	FSolarArraySpec Spec;
	Spec.Area_m2 = 2.0;
	Spec.Efficiency = 0.30;
	Spec.DegradationPerYear = 0.0; // No degradation for this test

	// Normal incidence at 1 AU: P = 1361 * 2.0 * 0.30 = 816.6 W
	const double Power = FAstralisPowerSystem::SolarArrayPower(Spec, 1.0, 0.0, 0.0);
	const double Expected = 1361.0 * 2.0 * 0.30;

	TestTrue(
		FString::Printf(TEXT("Power %.2f should be %.2f"), Power, Expected),
		FMath::Abs(Power - Expected) < 0.01);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FSolarArrayDistanceTest,
	"Astralis.Power.SolarArrayInverseSquare",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)

bool FSolarArrayDistanceTest::RunTest(const FString& Parameters)
{
	FSolarArraySpec Spec;
	Spec.Area_m2 = 2.0;
	Spec.Efficiency = 0.30;
	Spec.DegradationPerYear = 0.0;

	const double P1 = FAstralisPowerSystem::SolarArrayPower(Spec, 1.0, 0.0, 0.0);
	const double P_Mars = FAstralisPowerSystem::SolarArrayPower(Spec, 1.524, 0.0, 0.0);

	// At Mars (~1.524 AU): should be P1 / 1.524^2
	const double Expected = P1 / (1.524 * 1.524);
	const double Error = FMath::Abs(P_Mars - Expected) / Expected;

	TestTrue(
		FString::Printf(TEXT("Mars power error %.4f should be < 1e-6"), Error),
		Error < 1e-6);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FEclipseSurvivalTest,
	"Astralis.Power.EclipseSurvival",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)

bool FEclipseSurvivalTest::RunTest(const FString& Parameters)
{
	FBatterySpec Battery;
	Battery.Capacity_Wh = 100.0;
	Battery.MaxDoD = 0.80;
	Battery.Efficiency = 0.95;

	// 80Wh usable, 50W load for 30 minutes = 25Wh / 0.95 = 26.3Wh needed
	TestTrue(TEXT("Should survive short eclipse"),
		FAstralisPowerSystem::CanSurviveEclipse(Battery, 100.0, 50.0, 1800.0));

	// 50W load for 3 hours = 150Wh / 0.95 = 157.9Wh needed — exceeds 80Wh usable
	TestFalse(TEXT("Should not survive long eclipse"),
		FAstralisPowerSystem::CanSurviveEclipse(Battery, 100.0, 50.0, 10800.0));

	return true;
}
