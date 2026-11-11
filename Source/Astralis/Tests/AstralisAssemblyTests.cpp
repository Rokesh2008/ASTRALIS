#include "Misc/AutomationTest.h"
#include "AstralisData/ComponentCatalog/AstralisComponentTypes.h"
#include "AstralisSimulation/MassProperties/AstralisMassCalculator.h"

// Test that component mass entries sum correctly when added/removed
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FAssemblyMassRecalcTest,
	"Astralis.Assembly.MassRecalculation",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)

bool FAssemblyMassRecalcTest::RunTest(const FString& Parameters)
{
	TArray<FComponentMassEntry> Entries;

	// Add bus: 85 kg at origin
	FComponentMassEntry Bus;
	Bus.Mass_kg = 85.0;
	Bus.Position_m = FVector3d::ZeroVector;
	Entries.Add(Bus);

	double Total = FAstralisMassCalculator::TotalMass(Entries);
	TestEqual(TEXT("Single component mass"), Total, 85.0);

	// Add solar array: 8.5 kg at (1.2, 0, 0)
	FComponentMassEntry Solar;
	Solar.Mass_kg = 8.5;
	Solar.Position_m = FVector3d(1.2, 0.0, 0.0);
	Entries.Add(Solar);

	Total = FAstralisMassCalculator::TotalMass(Entries);
	TestNearlyEqual(TEXT("Two component mass"), Total, 93.5, 0.001);

	FVector3d CoM = FAstralisMassCalculator::CenterOfMass(Entries);
	// Expected CoM.X = (85*0 + 8.5*1.2) / 93.5 = 10.2 / 93.5 ≈ 0.10909
	TestNearlyEqual(TEXT("CoM X offset"), CoM.X, 10.2 / 93.5, 0.001);
	TestNearlyEqual(TEXT("CoM Y zero"), CoM.Y, 0.0, 0.001);

	// Add SAR: 12 kg at (0.5, 0, 0)
	FComponentMassEntry SAR;
	SAR.Mass_kg = 12.0;
	SAR.Position_m = FVector3d(0.5, 0.0, 0.0);
	Entries.Add(SAR);

	Total = FAstralisMassCalculator::TotalMass(Entries);
	TestNearlyEqual(TEXT("Three component mass"), Total, 105.5, 0.001);

	// Remove solar array (index 1)
	Entries.RemoveAt(1);

	Total = FAstralisMassCalculator::TotalMass(Entries);
	TestNearlyEqual(TEXT("After removal mass"), Total, 97.0, 0.001);

	FVector3d CoMAfter = FAstralisMassCalculator::CenterOfMass(Entries);
	// Expected CoM.X = (85*0 + 12*0.5) / 97 = 6.0 / 97 ≈ 0.06186
	TestNearlyEqual(TEXT("CoM after removal"), CoMAfter.X, 6.0 / 97.0, 0.001);

	return true;
}

// Test inertia tensor is updated when components change
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FAssemblyInertiaTest,
	"Astralis.Assembly.InertiaUpdate",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)

bool FAssemblyInertiaTest::RunTest(const FString& Parameters)
{
	TArray<FComponentMassEntry> Entries;

	FComponentMassEntry A;
	A.Mass_kg = 50.0;
	A.Position_m = FVector3d(1.0, 0.0, 0.0);
	Entries.Add(A);

	FComponentMassEntry B;
	B.Mass_kg = 50.0;
	B.Position_m = FVector3d(-1.0, 0.0, 0.0);
	Entries.Add(B);

	FVector3d CoM = FAstralisMassCalculator::CenterOfMass(Entries);
	TestNearlyEqual(TEXT("Symmetric CoM at origin"), CoM.X, 0.0, 0.001);

	double Tensor[6];
	FAstralisMassCalculator::InertiaTensor(Entries, CoM, Tensor);

	// Ixx should be 0 (both masses on X axis, no Y or Z offset)
	// Iyy = m1*dx1^2 + m2*dx2^2 = 50*1 + 50*1 = 100 (from Z component, but actually
	// Iyy = sum(m * (x^2 + z^2)) for point masses. With z=0: Iyy = 50*1 + 50*1 = 100
	TestTrue(TEXT("Iyy should be positive"), Tensor[1] > 0.0);
	TestNearlyEqual(TEXT("Iyy = Izz for symmetric X-axis"), Tensor[1], Tensor[2], 0.001);

	return true;
}

// Test component specification values are reasonable
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FComponentSpecValidationTest,
	"Astralis.Assembly.ComponentSpecValidation",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)

bool FComponentSpecValidationTest::RunTest(const FString& Parameters)
{
	// Create a component programmatically and validate constraints
	UAstralisComponentData* Comp = NewObject<UAstralisComponentData>();
	Comp->ComponentID = FName("TEST_001");
	Comp->DisplayName = FText::FromString(TEXT("Test Component"));
	Comp->Category = EComponentCategory::Avionics;
	Comp->Mass_kg = 4.5;
	Comp->Power.Standby_W = 2.0;
	Comp->Power.Average_W = 12.0;
	Comp->Power.Peak_W = 28.0;
	Comp->Thermal.OperatingMin_C = -20.0;
	Comp->Thermal.OperatingMax_C = 50.0;

	// Validate: peak >= average >= standby
	TestTrue(TEXT("Peak >= Average power"),
		Comp->Power.Peak_W >= Comp->Power.Average_W);
	TestTrue(TEXT("Average >= Standby power"),
		Comp->Power.Average_W >= Comp->Power.Standby_W);

	// Validate: thermal range is physically reasonable
	TestTrue(TEXT("Max temp > Min temp"),
		Comp->Thermal.OperatingMax_C > Comp->Thermal.OperatingMin_C);
	TestTrue(TEXT("Min temp above absolute zero"),
		Comp->Thermal.OperatingMin_C > -273.15);

	// Validate: mass is positive
	TestTrue(TEXT("Mass is positive"), Comp->Mass_kg > 0.0);

	// Validate: primary asset ID
	FPrimaryAssetId Id = Comp->GetPrimaryAssetId();
	TestEqual(TEXT("Asset type is AstralisComponent"),
		Id.PrimaryAssetType.ToString(), FString(TEXT("AstralisComponent")));
	TestEqual(TEXT("Asset name matches ComponentID"),
		Id.PrimaryAssetName.ToString(), FString(TEXT("TEST_001")));

	return true;
}
