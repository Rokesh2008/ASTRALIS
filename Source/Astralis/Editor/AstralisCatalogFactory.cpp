#include "AstralisCatalogFactory.h"
#include "AstralisData/ComponentCatalog/AstralisComponentTypes.h"
#include "UObject/SavePackage.h"
#include "UObject/Package.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "Misc/Paths.h"
#include "HAL/PlatformFileManager.h"

DEFINE_LOG_CATEGORY_STATIC(LogAstralisCatalog, Log, All);

FAutoConsoleCommand UAstralisCatalogFactory::CreateSampleComponentsCmd(
	TEXT("astralis.CreateSampleComponents"),
	TEXT("Create 7 illustrative spacecraft component data assets under /Game/Data/Components/"),
	FConsoleCommandDelegate::CreateStatic(&UAstralisCatalogFactory::CreateSampleComponents)
);

static UAstralisComponentData* CreateAndSave(
	const FString& AssetName,
	const FName& CompID,
	const FText& DisplayName,
	EComponentCategory Category,
	double Mass_kg,
	double StandbyW, double AvgW, double PeakW,
	double ThermalDissW,
	double OpMinC, double OpMaxC,
	const FVector& Envelope_m,
	double DataRate_Mbps,
	double Cost)
{
	const FString PackagePath = FString::Printf(TEXT("/Game/Data/Components/%s"), *AssetName);
	const FString FilePath = FPackageName::LongPackageNameToFilename(
		PackagePath, FPackageName::GetAssetPackageExtension());

	// Duplicate prevention: skip if file already exists on disk
	if (FPlatformFileManager::Get().GetPlatformFile().FileExists(*FilePath))
	{
		UE_LOG(LogAstralisCatalog, Log, TEXT("  SKIP (exists): %s"), *AssetName);
		return nullptr;
	}

	// Ensure directory exists
	const FString Dir = FPaths::GetPath(FilePath);
	FPlatformFileManager::Get().GetPlatformFile().CreateDirectoryTree(*Dir);

	UPackage* Package = CreatePackage(*PackagePath);
	Package->FullyLoad();

	UAstralisComponentData* Asset = NewObject<UAstralisComponentData>(
		Package, *AssetName, RF_Public | RF_Standalone);

	Asset->ComponentID = CompID;
	Asset->DisplayName = DisplayName;
	Asset->Category = Category;
	Asset->Mass_kg = Mass_kg;

	Asset->Power.Standby_W = StandbyW;
	Asset->Power.Average_W = AvgW;
	Asset->Power.Peak_W = PeakW;

	Asset->Thermal.Dissipation_W = ThermalDissW;
	Asset->Thermal.OperatingMin_C = OpMinC;
	Asset->Thermal.OperatingMax_C = OpMaxC;

	Asset->Geometry.Envelope_m = Envelope_m;
	Asset->DataRate_Mbps = DataRate_Mbps;
	Asset->Cost = Cost;

	Asset->Metadata.SpecType = ESpecificationType::Illustrative;
	Asset->Metadata.Source = TEXT("ASTRALIS sample catalog — illustrative, not flight-qualified");
	Asset->Metadata.Revision = 1;

	FAssetRegistryModule::AssetCreated(Asset);
	Asset->MarkPackageDirty();

	FSavePackageArgs SaveArgs;
	SaveArgs.TopLevelFlags = RF_Public | RF_Standalone;
	bool bSaved = UPackage::SavePackage(Package, Asset, *FilePath, SaveArgs);

	if (bSaved)
	{
		UE_LOG(LogAstralisCatalog, Log, TEXT("  CREATED: %s  [%.1f kg, %s]"),
			*AssetName, Mass_kg, *DisplayName.ToString());
	}
	else
	{
		UE_LOG(LogAstralisCatalog, Error, TEXT("  FAILED to save: %s"), *AssetName);
	}

	return Asset;
}

void UAstralisCatalogFactory::CreateSampleComponents()
{
	UE_LOG(LogAstralisCatalog, Log, TEXT("=== ASTRALIS Component Catalog Factory ==="));
	UE_LOG(LogAstralisCatalog, Log, TEXT("All specs are ILLUSTRATIVE — game-balanced, not flight-qualified."));

	int32 Created = 0;
	int32 Skipped = 0;

	auto Track = [&](UAstralisComponentData* Result)
	{
		if (Result) ++Created; else ++Skipped;
	};

	// 1. Spacecraft Bus Structure
	Track(CreateAndSave(
		TEXT("SC_Bus_Medium"),
		FName("BUS_MED_001"),
		FText::FromString(TEXT("Medium Bus Structure")),
		EComponentCategory::Structure,
		85.0,
		2.0, 5.0, 8.0,
		5.0,
		-40.0, 60.0,
		FVector(1.0, 1.0, 0.8),
		0.0,
		50000.0
	));

	// 2. Solar Array Panel
	Track(CreateAndSave(
		TEXT("SC_SolarArray_4m2"),
		FName("SOL_4M2_001"),
		FText::FromString(TEXT("Solar Array (4 m²)")),
		EComponentCategory::Power,
		8.5,
		0.0, 0.0, 0.0,
		0.5,
		-100.0, 120.0,
		FVector(2.0, 1.0, 0.02),
		0.0,
		35000.0
	));

	// 3. Li-Ion Battery Pack
	Track(CreateAndSave(
		TEXT("SC_Battery_200Wh"),
		FName("BAT_200WH_001"),
		FText::FromString(TEXT("Battery Pack (200 Wh)")),
		EComponentCategory::Power,
		3.2,
		0.5, 0.5, 0.5,
		0.5,
		-10.0, 45.0,
		FVector(0.2, 0.15, 0.1),
		0.0,
		18000.0
	));

	// 4. Reaction Wheel Assembly
	Track(CreateAndSave(
		TEXT("SC_ReactionWheel"),
		FName("RW_001"),
		FText::FromString(TEXT("Reaction Wheel Assembly")),
		EComponentCategory::Avionics,
		4.5,
		2.0, 12.0, 28.0,
		10.0,
		-20.0, 50.0,
		FVector(0.2, 0.2, 0.15),
		0.0,
		22000.0
	));

	// 5. S-Band Antenna
	Track(CreateAndSave(
		TEXT("SC_Antenna_SBand"),
		FName("ANT_SBAND_001"),
		FText::FromString(TEXT("S-Band Patch Antenna")),
		EComponentCategory::Communications,
		0.8,
		0.5, 5.0, 8.0,
		4.0,
		-40.0, 70.0,
		FVector(0.15, 0.15, 0.03),
		2.0,
		12000.0
	));

	// 6. Propellant Tank
	Track(CreateAndSave(
		TEXT("SC_PropTank_50kg"),
		FName("TANK_50KG_001"),
		FText::FromString(TEXT("Propellant Tank (50 kg capacity)")),
		EComponentCategory::Propulsion,
		6.0,
		0.0, 0.0, 0.0,
		0.0,
		-20.0, 50.0,
		FVector(0.35, 0.35, 0.5),
		0.0,
		15000.0
	));

	// 7. SAR Instrument (illustrative)
	Track(CreateAndSave(
		TEXT("SC_SAR_Instrument"),
		FName("SAR_001"),
		FText::FromString(TEXT("SAR Imaging Instrument")),
		EComponentCategory::Instrument,
		12.0,
		4.0, 55.0, 80.0,
		50.0,
		-20.0, 45.0,
		FVector(0.6, 0.4, 0.3),
		100.0,
		120000.0
	));

	UE_LOG(LogAstralisCatalog, Log, TEXT("=== Done: %d created, %d skipped (already exist) ==="), Created, Skipped);
}
