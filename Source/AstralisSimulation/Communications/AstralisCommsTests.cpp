#include "Misc/AutomationTest.h"
#include "AstralisCommsSystem.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FFSPLTest,
	"Astralis.Communications.FreeSpacePathLoss",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)

bool FFSPLTest::RunTest(const FString& Parameters)
{
	// FSPL at 1000km, 2.2GHz
	// lambda = 3e8 / 2.2e9 = 0.1364m
	// FSPL = 20*log10(4*pi*1e6/0.1364) = 20*log10(9.21e7) = 159.3 dB
	const double FSPL = FAstralisCommsSystem::FreeSpacePathLoss_dB(1.0e6, 2.2e9);
	const double Lambda = 3.0e8 / 2.2e9;
	const double Expected = 20.0 * FMath::Log2(4.0 * UE_DOUBLE_PI * 1.0e6 / Lambda) / FMath::Log2(10.0);

	TestTrue(
		FString::Printf(TEXT("FSPL %.2f dB should match expected %.2f dB"), FSPL, Expected),
		FMath::Abs(FSPL - Expected) < 0.01);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FLinkBudgetTest,
	"Astralis.Communications.LinkBudget",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)

bool FLinkBudgetTest::RunTest(const FString& Parameters)
{
	FAntennaSpec SCAntenna;
	SCAntenna.TransmitPower_W = 5.0;
	SCAntenna.Gain_dBi = 6.0;
	SCAntenna.Frequency_Hz = 2.2e9;
	SCAntenna.PointingLoss_dB = 1.0;

	FGroundStationSpec GS;
	GS.ReceiveGain_dBi = 40.0;
	GS.SystemNoiseTemp_K = 150.0;

	// LEO range ~700km
	auto Result = FAstralisCommsSystem::ComputeLinkBudget(SCAntenna, GS, 7.0e5, 10.0);

	// At 700km with these specs, link should close
	TestTrue(TEXT("LEO link should close"), Result.bLinkClosed);
	TestTrue(TEXT("Link margin should be positive"), Result.LinkMargin_dB > 0.0);
	TestTrue(TEXT("Data rate should be positive"), Result.AchievableDataRate_bps > 0.0);

	// Deep space range ~1 AU — link with small antenna should fail
	auto DeepResult = FAstralisCommsSystem::ComputeLinkBudget(SCAntenna, GS, 1.496e11, 10.0);
	TestFalse(TEXT("Deep space link with small antenna should fail"), DeepResult.bLinkClosed);

	return true;
}
