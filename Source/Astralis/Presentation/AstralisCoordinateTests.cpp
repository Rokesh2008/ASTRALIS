#include "Misc/AutomationTest.h"
#include "AstralisCoordinateService.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FCoordinateRoundTripTest,
	"Astralis.Coordinates.SIToUERoundTrip",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)

bool FCoordinateRoundTripTest::RunTest(const FString& Parameters)
{
	FAstralisCoordinateService::SetRenderingOrigin(FVector3d::ZeroVector);

	const FVector3d Original_m(1.5, -3.7, 0.25);
	const FVector UE = FAstralisCoordinateService::SIToUE(Original_m);

	// 1.5m should be 150cm
	TestTrue(TEXT("X: 1.5m -> 150cm"),
		FMath::Abs(UE.X - 150.0) < 0.01);
	TestTrue(TEXT("Y: -3.7m -> -370cm"),
		FMath::Abs(UE.Y - (-370.0)) < 0.01);
	TestTrue(TEXT("Z: 0.25m -> 25cm"),
		FMath::Abs(UE.Z - 25.0) < 0.01);

	// Round-trip
	const FVector3d Back = FAstralisCoordinateService::UEToSI(UE);
	const double Error = (Back - Original_m).Length();
	TestTrue(
		FString::Printf(TEXT("Round-trip error %.2e should be < 1e-10"), Error),
		Error < 1e-10);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FCoordinateOriginRebaseTest,
	"Astralis.Coordinates.OriginRebase",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)

bool FCoordinateOriginRebaseTest::RunTest(const FString& Parameters)
{
	// Simulate an object at orbital distance
	const FVector3d OrbitalPos(6.778e6, 0.0, 0.0);
	FAstralisCoordinateService::SetRenderingOrigin(OrbitalPos);

	// An object 100m away from the rendering origin
	const FVector3d NearbyObject = OrbitalPos + FVector3d(100.0, 0.0, 0.0);
	const FVector UE = FAstralisCoordinateService::SIToUE(NearbyObject);

	// Should be 10000cm (100m) from UE origin
	TestTrue(TEXT("Rebased X should be 10000cm"),
		FMath::Abs(UE.X - 10000.0) < 0.01);

	// Round-trip back
	const FVector3d Back = FAstralisCoordinateService::UEToSI(UE);
	const double Error = (Back - NearbyObject).Length();
	TestTrue(
		FString::Printf(TEXT("Origin-rebased round-trip error %.2e < 1e-6"), Error),
		Error < 1e-6);

	// Reset
	FAstralisCoordinateService::SetRenderingOrigin(FVector3d::ZeroVector);
	return true;
}
