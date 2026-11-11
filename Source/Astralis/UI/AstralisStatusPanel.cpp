#include "AstralisStatusPanel.h"
#include "Astralis/Spacecraft/AstralisSpacecraftActor.h"
#include "Astralis/Spacecraft/AstralisComponentActor.h"
#include "Astralis/Core/AstralisAssemblyGameMode.h"
#include "AstralisData/ComponentCatalog/AstralisComponentTypes.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Layout/SScrollBox.h"
#include "Widgets/Text/STextBlock.h"

static const FLinearColor HeaderColor(0.7f, 0.85f, 1.0f);
static const FLinearColor LabelColor(0.6f, 0.6f, 0.65f);
static const FLinearColor ValueColor(0.95f, 0.95f, 0.95f);
static const FLinearColor GoodColor(0.3f, 0.9f, 0.4f);
static const FLinearColor WarnColor(0.95f, 0.75f, 0.2f);
static const FLinearColor BadColor(0.95f, 0.3f, 0.3f);

TSharedRef<SWidget> UAstralisStatusPanel::MakeSectionHeader(const FString& Title)
{
	return SNew(SBox)
		.Padding(FMargin(0.0f, 8.0f, 0.0f, 2.0f))
		[
			SNew(STextBlock)
			.Text(FText::FromString(Title))
			.Font(FCoreStyle::GetDefaultFontStyle("Bold", 11))
			.ColorAndOpacity(FSlateColor(HeaderColor))
		];
}

TSharedRef<SWidget> UAstralisStatusPanel::MakeValueRow(const FString& Label, TSharedPtr<STextBlock>& OutValue)
{
	return SNew(SHorizontalBox)
		+ SHorizontalBox::Slot().AutoWidth().Padding(8.0f, 1.0f, 4.0f, 1.0f)
		[
			SNew(SBox).WidthOverride(110.0f)
			[
				SNew(STextBlock)
				.Text(FText::FromString(Label))
				.Font(FCoreStyle::GetDefaultFontStyle("Regular", 10))
				.ColorAndOpacity(FSlateColor(LabelColor))
			]
		]
		+ SHorizontalBox::Slot().FillWidth(1.0f).Padding(0.0f, 1.0f)
		[
			SAssignNew(OutValue, STextBlock)
			.Font(FCoreStyle::GetDefaultFontStyle("Regular", 10))
			.ColorAndOpacity(FSlateColor(ValueColor))
		];
}

TSharedRef<SWidget> UAstralisStatusPanel::RebuildWidget()
{
	TSharedRef<SVerticalBox> Content = SNew(SVerticalBox);

	// Title
	Content->AddSlot().AutoHeight().Padding(4.0f)
	[
		SNew(STextBlock)
		.Text(FText::FromString(TEXT("ENGINEERING BUDGET")))
		.Font(FCoreStyle::GetDefaultFontStyle("Bold", 14))
		.ColorAndOpacity(FSlateColor(FLinearColor(0.8f, 0.9f, 1.0f)))
	];

	// Mass section
	Content->AddSlot().AutoHeight() [ MakeSectionHeader(TEXT("MASS")) ];
	Content->AddSlot().AutoHeight() [ MakeValueRow(TEXT("Components"), ComponentCountText) ];
	Content->AddSlot().AutoHeight() [ MakeValueRow(TEXT("Total Mass"), TotalMassText) ];
	Content->AddSlot().AutoHeight() [ MakeValueRow(TEXT("Dry Mass"), DryMassText) ];
	Content->AddSlot().AutoHeight() [ MakeValueRow(TEXT("CoM (m)"), CoMText) ];

	// Power section
	Content->AddSlot().AutoHeight() [ MakeSectionHeader(TEXT("POWER")) ];
	Content->AddSlot().AutoHeight() [ MakeValueRow(TEXT("Generation"), PowerGenerationText) ];
	Content->AddSlot().AutoHeight() [ MakeValueRow(TEXT("Standby"), PowerStandbyText) ];
	Content->AddSlot().AutoHeight() [ MakeValueRow(TEXT("Average"), PowerAverageText) ];
	Content->AddSlot().AutoHeight() [ MakeValueRow(TEXT("Peak"), PowerPeakText) ];
	Content->AddSlot().AutoHeight() [ MakeValueRow(TEXT("Margin"), PowerMarginText) ];

	// Thermal section
	Content->AddSlot().AutoHeight() [ MakeSectionHeader(TEXT("THERMAL")) ];
	Content->AddSlot().AutoHeight() [ MakeValueRow(TEXT("Op. Min"), ThermalMinText) ];
	Content->AddSlot().AutoHeight() [ MakeValueRow(TEXT("Op. Max"), ThermalMaxText) ];
	Content->AddSlot().AutoHeight() [ MakeValueRow(TEXT("Dissipation"), ThermalDissipationText) ];

	// Data & cost
	Content->AddSlot().AutoHeight() [ MakeSectionHeader(TEXT("DATA / COST")) ];
	Content->AddSlot().AutoHeight() [ MakeValueRow(TEXT("Data Rate"), DataRateText) ];
	Content->AddSlot().AutoHeight() [ MakeValueRow(TEXT("Total Cost"), CostText) ];

	return SNew(SBox)
		.WidthOverride(300.0f)
		.Padding(FMargin(4.0f))
		[
			SNew(SScrollBox)
			+ SScrollBox::Slot()
			[
				Content
			]
		];
}

void UAstralisStatusPanel::SetSpacecraft(AAstralisSpacecraftActor* Spacecraft)
{
	TrackedSpacecraft = Spacecraft;
	RefreshDisplay();
}

void UAstralisStatusPanel::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	RefreshDisplay();
}

void UAstralisStatusPanel::RefreshDisplay()
{
	if (!TrackedSpacecraft.IsValid())
	{
		if (TotalMassText.IsValid())
		{
			TotalMassText->SetText(FText::FromString(TEXT("No spacecraft")));
		}
		return;
	}

	// Gather installed component data from game mode
	double TotalPowerGen_W = 0.0;
	double TotalStandby_W = 0.0;
	double TotalAverage_W = 0.0;
	double TotalPeak_W = 0.0;
	double TotalDissipation_W = 0.0;
	double TotalDataRate_Mbps = 0.0;
	double TotalCost = 0.0;
	double ThermalMinOverall_C = 1000.0;
	double ThermalMaxOverall_C = -1000.0;
	int32 CompCount = 0;

	AAstralisAssemblyGameMode* GM = GetWorld() ?
		GetWorld()->GetAuthGameMode<AAstralisAssemblyGameMode>() : nullptr;

	if (GM)
	{
		for (const auto& Comp : GM->InstalledComponents)
		{
			if (!Comp || !Comp->ComponentData) continue;

			const UAstralisComponentData* D = Comp->ComponentData;
			CompCount++;

			if (D->Category == EComponentCategory::Power && D->Power.Average_W < 0.0)
			{
				TotalPowerGen_W += FMath::Abs(D->Power.Average_W);
			}
			else
			{
				TotalStandby_W += D->Power.Standby_W;
				TotalAverage_W += D->Power.Average_W;
				TotalPeak_W += D->Power.Peak_W;
			}

			TotalDissipation_W += D->Thermal.Dissipation_W;
			TotalDataRate_Mbps += D->DataRate_Mbps;
			TotalCost += D->Cost;

			if (D->Thermal.OperatingMin_C < ThermalMinOverall_C)
				ThermalMinOverall_C = D->Thermal.OperatingMin_C;
			if (D->Thermal.OperatingMax_C > ThermalMaxOverall_C)
				ThermalMaxOverall_C = D->Thermal.OperatingMax_C;
		}
	}

	// If power components use positive generation convention, sum them separately
	// Solar arrays typically have Average_W > 0 as generation
	if (TotalPowerGen_W == 0.0 && GM)
	{
		for (const auto& Comp : GM->InstalledComponents)
		{
			if (!Comp || !Comp->ComponentData) continue;
			if (Comp->ComponentData->Category == EComponentCategory::Power)
			{
				TotalPowerGen_W += Comp->ComponentData->Power.Average_W;
			}
		}
		// Subtract power components from consumption totals
		for (const auto& Comp : GM->InstalledComponents)
		{
			if (!Comp || !Comp->ComponentData) continue;
			if (Comp->ComponentData->Category == EComponentCategory::Power)
			{
				TotalStandby_W -= Comp->ComponentData->Power.Standby_W;
				TotalAverage_W -= Comp->ComponentData->Power.Average_W;
				TotalPeak_W -= Comp->ComponentData->Power.Peak_W;
			}
		}
	}

	const FSpacecraftMassState& MS = TrackedSpacecraft->MassState;

	// Mass
	if (ComponentCountText.IsValid())
		ComponentCountText->SetText(FText::FromString(FString::Printf(TEXT("%d"), CompCount)));

	if (TotalMassText.IsValid())
		TotalMassText->SetText(FText::FromString(FString::Printf(TEXT("%.2f kg"), MS.TotalMass_kg)));

	if (DryMassText.IsValid())
		DryMassText->SetText(FText::FromString(FString::Printf(TEXT("%.2f kg"), MS.DryMass_kg)));

	if (CoMText.IsValid())
	{
		CoMText->SetText(FText::FromString(FString::Printf(TEXT("(%.3f, %.3f, %.3f)"),
			MS.CenterOfMass_m.X, MS.CenterOfMass_m.Y, MS.CenterOfMass_m.Z)));
	}

	// Power
	if (PowerGenerationText.IsValid())
		PowerGenerationText->SetText(FText::FromString(FString::Printf(TEXT("%.1f W"), TotalPowerGen_W)));

	if (PowerStandbyText.IsValid())
		PowerStandbyText->SetText(FText::FromString(FString::Printf(TEXT("%.1f W"), TotalStandby_W)));

	if (PowerAverageText.IsValid())
		PowerAverageText->SetText(FText::FromString(FString::Printf(TEXT("%.1f W"), TotalAverage_W)));

	if (PowerPeakText.IsValid())
		PowerPeakText->SetText(FText::FromString(FString::Printf(TEXT("%.1f W"), TotalPeak_W)));

	if (PowerMarginText.IsValid())
	{
		double Margin = (TotalPowerGen_W > 0.0) ?
			((TotalPowerGen_W - TotalAverage_W) / TotalPowerGen_W) * 100.0 : 0.0;
		FString MarginStr = FString::Printf(TEXT("%.1f%%"), Margin);
		PowerMarginText->SetText(FText::FromString(MarginStr));

		FLinearColor MarginColor = GoodColor;
		if (Margin < 10.0) MarginColor = BadColor;
		else if (Margin < 25.0) MarginColor = WarnColor;
		PowerMarginText->SetColorAndOpacity(FSlateColor(MarginColor));
	}

	// Thermal
	if (ThermalMinText.IsValid())
	{
		FString Val = (ThermalMinOverall_C < 999.0) ?
			FString::Printf(TEXT("%.0f C"), ThermalMinOverall_C) : TEXT("--");
		ThermalMinText->SetText(FText::FromString(Val));
	}

	if (ThermalMaxText.IsValid())
	{
		FString Val = (ThermalMaxOverall_C > -999.0) ?
			FString::Printf(TEXT("%.0f C"), ThermalMaxOverall_C) : TEXT("--");
		ThermalMaxText->SetText(FText::FromString(Val));
	}

	if (ThermalDissipationText.IsValid())
		ThermalDissipationText->SetText(FText::FromString(FString::Printf(TEXT("%.1f W"), TotalDissipation_W)));

	// Data & cost
	if (DataRateText.IsValid())
		DataRateText->SetText(FText::FromString(FString::Printf(TEXT("%.2f Mbps"), TotalDataRate_Mbps)));

	if (CostText.IsValid())
		CostText->SetText(FText::FromString(FString::Printf(TEXT("$%.0f"), TotalCost)));
}
