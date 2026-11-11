#include "AstralisComponentPalette.h"
#include "AstralisData/ComponentCatalog/AstralisComponentTypes.h"
#include "Engine/AssetManager.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "Widgets/Layout/SScrollBox.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/Input/SButton.h"

static FString CategoryLabel(EComponentCategory Cat)
{
	switch (Cat)
	{
	case EComponentCategory::Structure:      return TEXT("STRUCT");
	case EComponentCategory::Power:          return TEXT("POWER");
	case EComponentCategory::Propulsion:     return TEXT("PROP");
	case EComponentCategory::Avionics:       return TEXT("AVION");
	case EComponentCategory::Communications: return TEXT("COMMS");
	case EComponentCategory::Thermal:        return TEXT("THERM");
	case EComponentCategory::Instrument:     return TEXT("INSTR");
	default:                                 return TEXT("OTHER");
	}
}

TSharedRef<SWidget> UAstralisComponentPalette::RebuildWidget()
{
	ListBox = SNew(SVerticalBox);

	// Defer loading slightly — widget builds before asset registry finishes in some cases
	LoadComponents();
	BuildEntries();

	return SNew(SBox)
		.WidthOverride(300.0f)
		.Padding(FMargin(4.0f))
		[
			SNew(SVerticalBox)
			+ SVerticalBox::Slot().AutoHeight().Padding(4.0f)
			[
				SNew(STextBlock)
				.Text(FText::FromString(TEXT("COMPONENT CATALOG")))
				.Font(FCoreStyle::GetDefaultFontStyle("Bold", 16))
				.ColorAndOpacity(FSlateColor(FLinearColor(0.7f, 0.85f, 1.0f)))
			]
			+ SVerticalBox::Slot().FillHeight(1.0f)
			[
				SNew(SScrollBox)
				+ SScrollBox::Slot()
				[
					ListBox.ToSharedRef()
				]
			]
			+ SVerticalBox::Slot().AutoHeight().Padding(4.0f, 8.0f, 4.0f, 4.0f)
			[
				SNew(SButton)
				.OnClicked_Lambda([this]() -> FReply
				{
					RefreshList();
					return FReply::Handled();
				})
				[
					SNew(STextBlock)
					.Text(FText::FromString(TEXT("Refresh Catalog")))
					.Font(FCoreStyle::GetDefaultFontStyle("Regular", 11))
					.Justification(ETextJustify::Center)
				]
			]
		];
}

void UAstralisComponentPalette::RefreshList()
{
	LoadComponents();
	BuildEntries();
}

void UAstralisComponentPalette::LoadComponents()
{
	CachedComponents.Empty();

	// Primary method: Asset Manager scan
	UAssetManager& AM = UAssetManager::Get();
	TArray<FPrimaryAssetId> AssetIds;
	AM.GetPrimaryAssetIdList(FPrimaryAssetType("AstralisComponent"), AssetIds);

	for (const FPrimaryAssetId& Id : AssetIds)
	{
		FSoftObjectPath Path = AM.GetPrimaryAssetPath(Id);
		if (UObject* Loaded = Path.TryLoad())
		{
			if (UAstralisComponentData* Comp = Cast<UAstralisComponentData>(Loaded))
			{
				CachedComponents.Add(Comp);
			}
		}
	}

	// Fallback: direct asset registry scan if Asset Manager found nothing
	if (CachedComponents.Num() == 0)
	{
		IAssetRegistry& AR = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry").Get();
		TArray<FAssetData> AssetDataList;
		AR.GetAssetsByClass(UAstralisComponentData::StaticClass()->GetClassPathName(), AssetDataList);

		for (const FAssetData& AssetData : AssetDataList)
		{
			if (UObject* Loaded = AssetData.GetAsset())
			{
				if (UAstralisComponentData* Comp = Cast<UAstralisComponentData>(Loaded))
				{
					CachedComponents.Add(Comp);
				}
			}
		}
	}

	// Sort by category then name
	CachedComponents.Sort([](const UAstralisComponentData& A, const UAstralisComponentData& B)
	{
		if (A.Category != B.Category)
			return static_cast<uint8>(A.Category) < static_cast<uint8>(B.Category);
		return A.DisplayName.CompareTo(B.DisplayName) < 0;
	});
}

void UAstralisComponentPalette::BuildEntries()
{
	if (!ListBox.IsValid())
	{
		return;
	}

	ListBox->ClearChildren();

	EComponentCategory LastCategory = static_cast<EComponentCategory>(255);

	for (UAstralisComponentData* Data : CachedComponents)
	{
		if (!Data) continue;

		// Category header
		if (Data->Category != LastCategory)
		{
			LastCategory = Data->Category;
			ListBox->AddSlot()
				.AutoHeight()
				.Padding(2.0f, 8.0f, 2.0f, 2.0f)
				[
					SNew(STextBlock)
					.Text(FText::FromString(FString::Printf(TEXT("— %s —"), *CategoryLabel(Data->Category))))
					.Font(FCoreStyle::GetDefaultFontStyle("Bold", 11))
					.ColorAndOpacity(FSlateColor(FLinearColor(0.5f, 0.6f, 0.7f)))
				];
		}

		TWeakObjectPtr<UAstralisComponentData> WeakData = Data;
		TWeakObjectPtr<UAstralisComponentPalette> WeakSelf = this;

		const FString Line1 = Data->DisplayName.ToString();
		const FString Line2 = FString::Printf(TEXT("%.1f kg  |  %.0f W  |  %s"),
			Data->Mass_kg,
			Data->Power.Average_W,
			*CategoryLabel(Data->Category));

		ListBox->AddSlot()
			.AutoHeight()
			.Padding(2.0f, 1.0f)
			[
				SNew(SButton)
				.OnClicked_Lambda([WeakSelf, WeakData]() -> FReply
				{
					if (WeakSelf.IsValid() && WeakData.IsValid())
					{
						WeakSelf->OnComponentSelected.Broadcast(WeakData.Get());
					}
					return FReply::Handled();
				})
				[
					SNew(SBox)
					.Padding(FMargin(6.0f, 4.0f))
					[
						SNew(SVerticalBox)
						+ SVerticalBox::Slot().AutoHeight()
						[
							SNew(STextBlock)
							.Text(FText::FromString(Line1))
							.Font(FCoreStyle::GetDefaultFontStyle("Bold", 12))
							.ColorAndOpacity(FSlateColor(FLinearColor::White))
						]
						+ SVerticalBox::Slot().AutoHeight()
						[
							SNew(STextBlock)
							.Text(FText::FromString(Line2))
							.Font(FCoreStyle::GetDefaultFontStyle("Regular", 10))
							.ColorAndOpacity(FSlateColor(FLinearColor(0.6f, 0.6f, 0.6f)))
						]
					]
				]
			];
	}

	if (CachedComponents.Num() == 0)
	{
		ListBox->AddSlot()
			.AutoHeight()
			.Padding(8.0f)
			[
				SNew(STextBlock)
				.Text(FText::FromString(TEXT("No components found.\nRun: astralis.CreateSampleComponents\nThen click Refresh.")))
				.Font(FCoreStyle::GetDefaultFontStyle("Italic", 11))
				.ColorAndOpacity(FSlateColor(FLinearColor(0.5f, 0.5f, 0.5f)))
			];
	}
}
