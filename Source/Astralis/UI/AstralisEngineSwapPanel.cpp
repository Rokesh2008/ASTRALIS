#include "AstralisEngineSwapPanel.h"
#include "Astralis/LaunchVehicle/AstralisLaunchVehicleActor.h"
#include "Astralis/LaunchVehicle/AstralisStageActor.h"
#include "AstralisData/VehicleLibrary/AstralisPerformanceCalculator.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Layout/SBorder.h"
#include "Widgets/Layout/SScrollBox.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/SNullWidget.h"

static const FLinearColor ESHeaderColor(0.4f, 0.85f, 1.0f);
static const FLinearColor ESLabelColor(0.6f, 0.6f, 0.65f);
static const FLinearColor ESValueColor(0.95f, 0.95f, 0.95f);
static const FLinearColor ESGoodColor(0.3f, 0.9f, 0.4f);
static const FLinearColor ESWarnColor(0.95f, 0.75f, 0.2f);
static const FLinearColor ESBadColor(0.95f, 0.3f, 0.3f);

TSharedRef<SWidget> UAstralisEngineSwapPanel::MakeHeader(const FString& Title)
{
	return SNew(SBox)
		.Padding(FMargin(0.0f, 8.0f, 0.0f, 2.0f))
		[
			SNew(STextBlock)
			.Text(FText::FromString(Title))
			.Font(FCoreStyle::GetDefaultFontStyle("Bold", 11))
			.ColorAndOpacity(FSlateColor(ESHeaderColor))
		];
}

TSharedRef<SWidget> UAstralisEngineSwapPanel::MakeEngineButton(int32 EngineIndex)
{
	if (!EngineCatalog.IsValidIndex(EngineIndex)) return SNullWidget::NullWidget;

	const FEngineSpec_Vehicle& E = EngineCatalog[EngineIndex];

	FString Label = FString::Printf(TEXT("%s  [%s]"),
		*E.Name.ToString(),
		*StaticEnum<EEngineType>()->GetDisplayNameTextByValue(static_cast<int64>(E.EngineType)).ToString());

	FString Detail = FString::Printf(TEXT("%.0f kN SL / %.0f kN Vac  |  Isp: %.0f / %.0f s  |  %.0f kg"),
		E.ThrustSeaLevel_N / 1000.0, E.ThrustVacuum_N / 1000.0,
		E.IspSeaLevel_s, E.IspVacuum_s, E.Mass_kg);

	FString PropLabel;
	switch (E.PropellantType)
	{
	case EPropellantType::LOX_RP1:     PropLabel = TEXT("LOX/RP-1"); break;
	case EPropellantType::LOX_LH2:     PropLabel = TEXT("LOX/LH2"); break;
	case EPropellantType::NTO_UDMH:    PropLabel = TEXT("NTO/UDMH"); break;
	case EPropellantType::SolidHTPB:   PropLabel = TEXT("Solid (HTPB)"); break;
	case EPropellantType::LOX_Methane: PropLabel = TEXT("LOX/CH4"); break;
	}

	return SNew(SBox)
		.Padding(FMargin(4.0f, 2.0f))
		[
			SNew(SButton)
			.OnClicked_Lambda([this, EngineIndex]()
			{
				ApplyEngine(EngineIndex);
				return FReply::Handled();
			})
			.ButtonColorAndOpacity(FLinearColor(0.08f, 0.08f, 0.12f, 0.95f))
			[
				SNew(SVerticalBox)
				+ SVerticalBox::Slot().AutoHeight().Padding(2.0f, 1.0f)
				[
					SNew(STextBlock)
					.Text(FText::FromString(Label))
					.Font(FCoreStyle::GetDefaultFontStyle("Bold", 10))
					.ColorAndOpacity(FSlateColor(ESValueColor))
				]
				+ SVerticalBox::Slot().AutoHeight().Padding(2.0f, 0.0f)
				[
					SNew(STextBlock)
					.Text(FText::FromString(Detail))
					.Font(FCoreStyle::GetDefaultFontStyle("Regular", 8))
					.ColorAndOpacity(FSlateColor(ESLabelColor))
				]
				+ SVerticalBox::Slot().AutoHeight().Padding(2.0f, 0.0f, 2.0f, 1.0f)
				[
					SNew(STextBlock)
					.Text(FText::FromString(PropLabel))
					.Font(FCoreStyle::GetDefaultFontStyle("Regular", 8))
					.ColorAndOpacity(FSlateColor(FLinearColor(0.5f, 0.8f, 1.0f)))
				]
			]
		];
}

TSharedRef<SWidget> UAstralisEngineSwapPanel::RebuildWidget()
{
	EngineCatalog = UAstralisEngineCatalog::GetDefaultCatalog();

	TSharedRef<SVerticalBox> Content = SNew(SVerticalBox);

	// Title
	Content->AddSlot().AutoHeight().Padding(4.0f)
	[
		SNew(STextBlock)
		.Text(FText::FromString(TEXT("ENGINE SWAP")))
		.Font(FCoreStyle::GetDefaultFontStyle("Bold", 14))
		.ColorAndOpacity(FSlateColor(FLinearColor(0.4f, 0.85f, 1.0f)))
	];

	// Stage selector buttons
	Content->AddSlot().AutoHeight().Padding(4.0f, 2.0f)
	[
		SNew(SHorizontalBox)
		+ SHorizontalBox::Slot().AutoWidth().Padding(2.0f)
		[
			SNew(SButton)
			.OnClicked_Lambda([this]() { SelectStage(0); return FReply::Handled(); })
			.ButtonColorAndOpacity(FLinearColor(0.15f, 0.15f, 0.2f))
			[
				SNew(STextBlock)
				.Text(FText::FromString(TEXT("Stage 1")))
				.Font(FCoreStyle::GetDefaultFontStyle("Bold", 10))
				.ColorAndOpacity(FSlateColor(ESValueColor))
			]
		]
		+ SHorizontalBox::Slot().AutoWidth().Padding(2.0f)
		[
			SNew(SButton)
			.OnClicked_Lambda([this]() { SelectStage(1); return FReply::Handled(); })
			.ButtonColorAndOpacity(FLinearColor(0.15f, 0.15f, 0.2f))
			[
				SNew(STextBlock)
				.Text(FText::FromString(TEXT("Stage 2")))
				.Font(FCoreStyle::GetDefaultFontStyle("Bold", 10))
				.ColorAndOpacity(FSlateColor(ESValueColor))
			]
		]
	];

	// Current engine info
	Content->AddSlot().AutoHeight() [ MakeHeader(TEXT("CURRENT ENGINE")) ];
	Content->AddSlot().AutoHeight().Padding(8.0f, 2.0f)
	[
		SAssignNew(StageNameText, STextBlock)
		.Font(FCoreStyle::GetDefaultFontStyle("Regular", 10))
		.ColorAndOpacity(FSlateColor(ESLabelColor))
	];
	Content->AddSlot().AutoHeight().Padding(8.0f, 2.0f)
	[
		SAssignNew(CurrentEngineText, STextBlock)
		.Font(FCoreStyle::GetDefaultFontStyle("Bold", 10))
		.ColorAndOpacity(FSlateColor(ESValueColor))
	];

	// Preview after swap
	Content->AddSlot().AutoHeight() [ MakeHeader(TEXT("IMPACT PREVIEW")) ];
	Content->AddSlot().AutoHeight().Padding(8.0f, 2.0f)
	[
		SNew(SHorizontalBox)
		+ SHorizontalBox::Slot().AutoWidth()
		[
			SNew(SBox).WidthOverride(80.0f)
			[
				SNew(STextBlock).Text(FText::FromString(TEXT("Thrust:")))
				.Font(FCoreStyle::GetDefaultFontStyle("Regular", 9))
				.ColorAndOpacity(FSlateColor(ESLabelColor))
			]
		]
		+ SHorizontalBox::Slot().FillWidth(1.0f)
		[
			SAssignNew(PreviewThrustText, STextBlock)
			.Font(FCoreStyle::GetDefaultFontStyle("Regular", 9))
			.ColorAndOpacity(FSlateColor(ESValueColor))
		]
	];
	Content->AddSlot().AutoHeight().Padding(8.0f, 2.0f)
	[
		SNew(SHorizontalBox)
		+ SHorizontalBox::Slot().AutoWidth()
		[
			SNew(SBox).WidthOverride(80.0f)
			[
				SNew(STextBlock).Text(FText::FromString(TEXT("Isp:")))
				.Font(FCoreStyle::GetDefaultFontStyle("Regular", 9))
				.ColorAndOpacity(FSlateColor(ESLabelColor))
			]
		]
		+ SHorizontalBox::Slot().FillWidth(1.0f)
		[
			SAssignNew(PreviewIspText, STextBlock)
			.Font(FCoreStyle::GetDefaultFontStyle("Regular", 9))
			.ColorAndOpacity(FSlateColor(ESValueColor))
		]
	];
	Content->AddSlot().AutoHeight().Padding(8.0f, 2.0f)
	[
		SNew(SHorizontalBox)
		+ SHorizontalBox::Slot().AutoWidth()
		[
			SNew(SBox).WidthOverride(80.0f)
			[
				SNew(STextBlock).Text(FText::FromString(TEXT("TWR:")))
				.Font(FCoreStyle::GetDefaultFontStyle("Regular", 9))
				.ColorAndOpacity(FSlateColor(ESLabelColor))
			]
		]
		+ SHorizontalBox::Slot().FillWidth(1.0f)
		[
			SAssignNew(PreviewTWRText, STextBlock)
			.Font(FCoreStyle::GetDefaultFontStyle("Regular", 9))
			.ColorAndOpacity(FSlateColor(ESValueColor))
		]
	];
	Content->AddSlot().AutoHeight().Padding(8.0f, 2.0f)
	[
		SNew(SHorizontalBox)
		+ SHorizontalBox::Slot().AutoWidth()
		[
			SNew(SBox).WidthOverride(80.0f)
			[
				SNew(STextBlock).Text(FText::FromString(TEXT("Delta-V:")))
				.Font(FCoreStyle::GetDefaultFontStyle("Regular", 9))
				.ColorAndOpacity(FSlateColor(ESLabelColor))
			]
		]
		+ SHorizontalBox::Slot().FillWidth(1.0f)
		[
			SAssignNew(PreviewDVText, STextBlock)
			.Font(FCoreStyle::GetDefaultFontStyle("Regular", 9))
			.ColorAndOpacity(FSlateColor(ESValueColor))
		]
	];

	// Compatibility
	Content->AddSlot().AutoHeight().Padding(8.0f, 4.0f)
	[
		SAssignNew(CompatibilityText, STextBlock)
		.Font(FCoreStyle::GetDefaultFontStyle("Bold", 9))
		.ColorAndOpacity(FSlateColor(ESGoodColor))
		.AutoWrapText(true)
	];

	// Engine catalogue list
	Content->AddSlot().AutoHeight() [ MakeHeader(TEXT("ENGINE CATALOGUE")) ];
	Content->AddSlot().FillHeight(1.0f)
	[
		SNew(SScrollBox)
		+ SScrollBox::Slot()
		[
			SAssignNew(EngineListBox, SVerticalBox)
		]
	];

	// Populate engine buttons
	for (int32 i = 0; i < EngineCatalog.Num(); ++i)
	{
		EngineListBox->AddSlot().AutoHeight()
		[
			MakeEngineButton(i)
		];
	}

	// Instructions
	Content->AddSlot().AutoHeight().Padding(8.0f, 6.0f, 8.0f, 4.0f)
	[
		SNew(STextBlock)
		.Text(FText::FromString(TEXT("Click engine to swap. Calculations update live.")))
		.Font(FCoreStyle::GetDefaultFontStyle("Regular", 8))
		.ColorAndOpacity(FSlateColor(FLinearColor(0.4f, 0.4f, 0.5f)))
	];

	return SNew(SBox)
		.WidthOverride(340.0f)
		.MaxDesiredHeight(700.0f)
		[
			SNew(SBorder)
			.BorderBackgroundColor(FLinearColor(0.02f, 0.02f, 0.04f, 0.92f))
			.Padding(FMargin(8.0f))
			[
				Content
			]
		];
}

void UAstralisEngineSwapPanel::SetLaunchVehicle(AAstralisLaunchVehicleActor* Vehicle)
{
	TrackedVehicle = Vehicle;
	UpdatePreview();
}

void UAstralisEngineSwapPanel::SelectStage(int32 StageIndex)
{
	SelectedStageIndex = StageIndex;
	UpdatePreview();
}

void UAstralisEngineSwapPanel::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	UpdatePreview();
}

void UAstralisEngineSwapPanel::ApplyEngine(int32 EngineIndex)
{
	AAstralisLaunchVehicleActor* Vehicle = TrackedVehicle.Get();
	if (!Vehicle) return;
	if (!EngineCatalog.IsValidIndex(EngineIndex)) return;
	if (!Vehicle->Stages.IsValidIndex(SelectedStageIndex)) return;

	AAstralisStageActor* Stage = Vehicle->Stages[SelectedStageIndex];
	if (!Stage) return;

	const FEngineSpec_Vehicle& NewEngine = EngineCatalog[EngineIndex];

	// Apply engine specs to stage
	Stage->Spec.Thrust_N = (NewEngine.ThrustSeaLevel_N > 0)
		? NewEngine.ThrustSeaLevel_N : NewEngine.ThrustVacuum_N;
	Stage->Spec.Isp_s = (NewEngine.IspSeaLevel_s > 0)
		? NewEngine.IspSeaLevel_s : NewEngine.IspVacuum_s;

	UE_LOG(LogTemp, Warning, TEXT("ENGINE SWAP: Stage %d -> %s (%.0f kN, %.0f s Isp)"),
		SelectedStageIndex + 1, *NewEngine.Name.ToString(),
		Stage->Spec.Thrust_N / 1000.0, Stage->Spec.Isp_s);

	OnEngineSwapped.Broadcast(SelectedStageIndex, NewEngine.Name);
	UpdatePreview();
}

void UAstralisEngineSwapPanel::UpdatePreview()
{
	AAstralisLaunchVehicleActor* Vehicle = TrackedVehicle.Get();
	if (!Vehicle) return;

	if (!Vehicle->Stages.IsValidIndex(SelectedStageIndex)) return;
	const AAstralisStageActor* Stage = Vehicle->Stages[SelectedStageIndex];
	if (!Stage) return;

	if (StageNameText)
		StageNameText->SetText(FText::FromString(
			FString::Printf(TEXT("Stage %d: %s"), SelectedStageIndex + 1,
				*Stage->Spec.StageName.ToString())));

	if (CurrentEngineText)
		CurrentEngineText->SetText(FText::FromString(
			FString::Printf(TEXT("%.0f kN / %.0f s Isp"),
				Stage->Spec.Thrust_N / 1000.0, Stage->Spec.Isp_s)));

	// Compute current vehicle performance for display
	double UpperMass = 0.0;
	if (Vehicle->Payload)
		UpperMass = FMath::Max(Vehicle->Payload->GetTotalMass(), 2623.0);

	for (int32 j = SelectedStageIndex + 1; j < Vehicle->Stages.Num(); ++j)
	{
		if (Vehicle->Stages[j])
			UpperMass += Vehicle->Stages[j]->GetTotalMass_kg();
	}

	const double StageTotalMass = Stage->GetTotalMass_kg() + UpperMass;
	const double TWR = (StageTotalMass > 0) ? Stage->Spec.Thrust_N / (StageTotalMass * 9.80665) : 0.0;

	const double M0 = UpperMass + Stage->Spec.DryMass_kg + Stage->Spec.PropellantCapacity_kg;
	const double M1 = UpperMass + Stage->Spec.DryMass_kg;
	const double DV = (M0 > 0 && M1 > 0 && Stage->Spec.Isp_s > 0)
		? Stage->Spec.Isp_s * 9.80665 * FMath::Loge(M0 / M1)
		: 0.0;

	if (PreviewThrustText)
		PreviewThrustText->SetText(FText::FromString(
			FString::Printf(TEXT("%.0f kN"), Stage->Spec.Thrust_N / 1000.0)));

	if (PreviewIspText)
		PreviewIspText->SetText(FText::FromString(
			FString::Printf(TEXT("%.0f s"), Stage->Spec.Isp_s)));

	if (PreviewTWRText)
	{
		PreviewTWRText->SetText(FText::FromString(FString::Printf(TEXT("%.2f"), TWR)));
		if (TWR >= 1.2)
			PreviewTWRText->SetColorAndOpacity(FSlateColor(ESGoodColor));
		else if (TWR >= 1.0)
			PreviewTWRText->SetColorAndOpacity(FSlateColor(ESWarnColor));
		else
			PreviewTWRText->SetColorAndOpacity(FSlateColor(ESBadColor));
	}

	if (PreviewDVText)
	{
		PreviewDVText->SetText(FText::FromString(FString::Printf(TEXT("%.0f m/s"), DV)));
		const double TotalDV = Vehicle->GetTotalDeltaV_mps();
		if (TotalDV >= 9400.0)
			PreviewDVText->SetColorAndOpacity(FSlateColor(ESGoodColor));
		else if (TotalDV >= 7000.0)
			PreviewDVText->SetColorAndOpacity(FSlateColor(ESWarnColor));
		else
			PreviewDVText->SetColorAndOpacity(FSlateColor(ESBadColor));
	}

	if (CompatibilityText)
	{
		if (TWR < 1.0 && SelectedStageIndex == 0)
		{
			CompatibilityText->SetText(FText::FromString(TEXT("WARNING: TWR < 1.0 — Cannot liftoff!")));
			CompatibilityText->SetColorAndOpacity(FSlateColor(ESBadColor));
		}
		else
		{
			CompatibilityText->SetText(FText::FromString(TEXT("Configuration valid")));
			CompatibilityText->SetColorAndOpacity(FSlateColor(ESGoodColor));
		}
	}
}
