#include "Misc/AutomationTest.h"
#include "AstralisData/ComponentCatalog/AstralisComponentTypes.h"
#include "Engine/AssetManager.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FCatalogDiscoveryTest,
	"Astralis.Assembly.CatalogDiscovery",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)

bool FCatalogDiscoveryTest::RunTest(const FString& Parameters)
{
	UAssetManager& AM = UAssetManager::Get();
	TArray<FPrimaryAssetId> AssetIds;
	AM.GetPrimaryAssetIdList(FPrimaryAssetType("AstralisComponent"), AssetIds);

	if (AssetIds.Num() == 0)
	{
		AddWarning(TEXT("No AstralisComponent assets discovered by Asset Manager — may need editor restart or astralis.CreateSampleComponents"));
		return true;
	}

	TestEqual(TEXT("Expected 7 component assets"), AssetIds.Num(), 7);

	// Load one and verify properties
	for (const FPrimaryAssetId& Id : AssetIds)
	{
		FSoftObjectPath Path = AM.GetPrimaryAssetPath(Id);
		UObject* Loaded = Path.TryLoad();
		TestNotNull(TEXT("Asset loads successfully"), Loaded);

		if (UAstralisComponentData* Comp = Cast<UAstralisComponentData>(Loaded))
		{
			TestTrue(TEXT("Mass is positive"), Comp->Mass_kg > 0.0);
			TestTrue(TEXT("Has display name"), !Comp->DisplayName.IsEmpty());
			TestTrue(TEXT("Has component ID"), !Comp->ComponentID.IsNone());
			TestTrue(TEXT("Peak >= Average power"),
				Comp->Power.Peak_W >= Comp->Power.Average_W);
			TestTrue(TEXT("Max temp > Min temp"),
				Comp->Thermal.OperatingMax_C > Comp->Thermal.OperatingMin_C);

			// Verify provenance marking
			TestEqual(TEXT("Spec type is Illustrative"),
				static_cast<uint8>(Comp->Metadata.SpecType),
				static_cast<uint8>(ESpecificationType::Illustrative));
		}
	}

	return true;
}

// Test specific component values match what the factory creates
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FCatalogSpecVerificationTest,
	"Astralis.Assembly.CatalogSpecVerification",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)

bool FCatalogSpecVerificationTest::RunTest(const FString& Parameters)
{
	UAssetManager& AM = UAssetManager::Get();

	// Load bus by known primary asset ID
	FPrimaryAssetId BusId(FPrimaryAssetType("AstralisComponent"), FName("BUS_MED_001"));
	FSoftObjectPath BusPath = AM.GetPrimaryAssetPath(BusId);

	if (!BusPath.IsValid())
	{
		AddWarning(TEXT("Bus asset not found — run astralis.CreateSampleComponents first"));
		return true;
	}

	UAstralisComponentData* Bus = Cast<UAstralisComponentData>(BusPath.TryLoad());
	if (!Bus)
	{
		AddError(TEXT("Bus asset failed to load"));
		return false;
	}

	TestNearlyEqual(TEXT("Bus mass = 85 kg"), Bus->Mass_kg, 85.0, 0.01);
	TestNearlyEqual(TEXT("Bus standby power = 2 W"), Bus->Power.Standby_W, 2.0, 0.01);
	TestNearlyEqual(TEXT("Bus average power = 5 W"), Bus->Power.Average_W, 5.0, 0.01);
	TestNearlyEqual(TEXT("Bus peak power = 8 W"), Bus->Power.Peak_W, 8.0, 0.01);
	TestNearlyEqual(TEXT("Bus thermal dissipation = 5 W"), Bus->Thermal.Dissipation_W, 5.0, 0.01);
	TestNearlyEqual(TEXT("Bus operating min = -40 C"), Bus->Thermal.OperatingMin_C, -40.0, 0.01);
	TestNearlyEqual(TEXT("Bus operating max = 60 C"), Bus->Thermal.OperatingMax_C, 60.0, 0.01);
	TestEqual(TEXT("Bus category = Structure"),
		static_cast<uint8>(Bus->Category),
		static_cast<uint8>(EComponentCategory::Structure));

	// Load SAR and check
	FPrimaryAssetId SarId(FPrimaryAssetType("AstralisComponent"), FName("SAR_001"));
	FSoftObjectPath SarPath = AM.GetPrimaryAssetPath(SarId);

	if (SarPath.IsValid())
	{
		UAstralisComponentData* SAR = Cast<UAstralisComponentData>(SarPath.TryLoad());
		if (SAR)
		{
			TestNearlyEqual(TEXT("SAR mass = 12 kg"), SAR->Mass_kg, 12.0, 0.01);
			TestNearlyEqual(TEXT("SAR avg power = 55 W"), SAR->Power.Average_W, 55.0, 0.01);
			TestNearlyEqual(TEXT("SAR peak power = 80 W"), SAR->Power.Peak_W, 80.0, 0.01);
			TestNearlyEqual(TEXT("SAR data rate = 100 Mbps"), SAR->DataRate_Mbps, 100.0, 0.01);
			TestEqual(TEXT("SAR category = Instrument"),
				static_cast<uint8>(SAR->Category),
				static_cast<uint8>(EComponentCategory::Instrument));
		}
	}

	return true;
}
