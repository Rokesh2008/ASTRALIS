#include "Misc/AutomationTest.h"
#include "AstralisMassCalculator.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FMassCenterOfMassTest,
	"Astralis.Mass.CenterOfMass",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)

bool FMassCenterOfMassTest::RunTest(const FString& Parameters)
{
	TArray<FComponentMassEntry> Components;

	// Two equal masses at symmetric positions -> CoM at origin
	FComponentMassEntry A;
	A.Mass_kg = 10.0;
	A.Position_m = FVector3d(1.0, 0.0, 0.0);
	Components.Add(A);

	FComponentMassEntry B;
	B.Mass_kg = 10.0;
	B.Position_m = FVector3d(-1.0, 0.0, 0.0);
	Components.Add(B);

	const FVector3d CoM = FAstralisMassCalculator::CenterOfMass(Components);
	TestTrue(TEXT("CoM X near zero"), FMath::Abs(CoM.X) < 1e-12);
	TestTrue(TEXT("CoM Y near zero"), FMath::Abs(CoM.Y) < 1e-12);
	TestTrue(TEXT("CoM Z near zero"), FMath::Abs(CoM.Z) < 1e-12);

	// Asymmetric: 10kg at x=0, 30kg at x=4 -> CoM at x=3
	Components.Empty();
	FComponentMassEntry C;
	C.Mass_kg = 10.0;
	C.Position_m = FVector3d(0.0, 0.0, 0.0);
	Components.Add(C);

	FComponentMassEntry D;
	D.Mass_kg = 30.0;
	D.Position_m = FVector3d(4.0, 0.0, 0.0);
	Components.Add(D);

	const FVector3d CoM2 = FAstralisMassCalculator::CenterOfMass(Components);
	TestTrue(
		FString::Printf(TEXT("CoM X=%.6f should be 3.0"), CoM2.X),
		FMath::Abs(CoM2.X - 3.0) < 1e-12
	);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FTotalMassTest,
	"Astralis.Mass.TotalMass",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)

bool FTotalMassTest::RunTest(const FString& Parameters)
{
	TArray<FComponentMassEntry> Components;

	FComponentMassEntry A;
	A.Mass_kg = 12.5;
	Components.Add(A);

	FComponentMassEntry B;
	B.Mass_kg = 7.3;
	Components.Add(B);

	FComponentMassEntry C;
	C.Mass_kg = 4.2;
	Components.Add(C);

	const double Total = FAstralisMassCalculator::TotalMass(Components);
	TestTrue(
		FString::Printf(TEXT("Total mass %.1f should be 24.0"), Total),
		FMath::Abs(Total - 24.0) < 1e-12
	);

	return true;
}
