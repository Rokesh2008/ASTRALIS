#include "AstralisVehicleInspector.h"
#include "Astralis/LaunchVehicle/AstralisLaunchVehicleActor.h"
#include "Astralis/LaunchVehicle/AstralisStageActor.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Layout/SBorder.h"
#include "Widgets/Layout/SScrollBox.h"
#include "Widgets/Text/STextBlock.h"

static const FLinearColor VIHeaderColor(0.9f, 0.7f, 0.3f);
static const FLinearColor VILabelColor(0.6f, 0.6f, 0.65f);
static const FLinearColor VIValueColor(0.95f, 0.95f, 0.95f);
static const FLinearColor VIGoodColor(0.3f, 0.9f, 0.4f);
static const FLinearColor VIWarnColor(0.95f, 0.75f, 0.2f);
static const FLinearColor VIBadColor(0.95f, 0.3f, 0.3f);
static const FLinearColor VIStageTitleColor(0.5f, 0.8f, 1.0f);

TSharedRef<SWidget> UAstralisVehicleInspector::MakeHeader(const FString& Title)
{
	return SNew(SBox)
		.Padding(FMargin(0.0f, 8.0f, 0.0f, 2.0f))
		[
			SNew(STextBlock)
			.Text(FText::FromString(Title))
			.Font(FCoreStyle::GetDefaultFontStyle("Bold", 11))
			.ColorAndOpacity(FSlateColor(VIHeaderColor))
		];
}

TSharedRef<SWidget> UAstralisVehicleInspector::MakeRow(const FString& Label, TSharedPtr<STextBlock>& OutValue)
{
	return SNew(SHorizontalBox)
		+ SHorizontalBox::Slot().AutoWidth().Padding(8.0f, 1.0f, 4.0f, 1.0f)
		[
			SNew(SBox).WidthOverride(120.0f)
			[
				SNew(STextBlock)
				.Text(FText::FromString(Label))
				.Font(FCoreStyle::GetDefaultFontStyle("Regular", 10))
				.ColorAndOpacity(FSlateColor(VILabelColor))
			]
		]
		+ SHorizontalBox::Slot().FillWidth(1.0f).Padding(0.0f, 1.0f)
		[
			SAssignNew(OutValue, STextBlock)
			.Font(FCoreStyle::GetDefaultFontStyle("Regular", 10))
			.ColorAndOpacity(FSlateColor(VIValueColor))
		];
}

TSharedRef<SWidget> UAstralisVehicleInspector::RebuildWidget()
{
	TSharedRef<SVerticalBox> Content = SNew(SVerticalBox);

	// Title
	Content->AddSlot().AutoHeight().Padding(4.0f)
	[
		SNew(STextBlock)
		.Text(FText::FromString(TEXT("LAUNCH VEHICLE")))
		.Font(FCoreStyle::GetDefaultFontStyle("Bold", 14))
		.ColorAndOpacity(FSlateColor(FLinearColor(0.9f, 0.75f, 0.3f)))
	];

	// Vehicle overview
	Content->AddSlot().AutoHeight() [ MakeHeader(TEXT("VEHICLE OVERVIEW")) ];
	Content->AddSlot().AutoHeight() [ MakeRow(TEXT("Vehicle"), VehicleNameText) ];
	Content->AddSlot().AutoHeight() [ MakeRow(TEXT("Total Mass"), TotalMassText) ];
	Content->AddSlot().AutoHeight() [ MakeRow(TEXT("Total Delta-V"), TotalDeltaVText) ];
	Content->AddSlot().AutoHeight() [ MakeRow(TEXT("Payload"), PayloadMassText) ];
	Content->AddSlot().AutoHeight() [ MakeRow(TEXT("Status"), StatusText) ];

	// Stage 1
	Content->AddSlot().AutoHeight()
	[
		SNew(SBox).Padding(FMargin(0, 10, 0, 2))
		[
			SNew(STextBlock)
			.Text(FText::FromString(TEXT("STAGE 1 — FIRST STAGE")))
			.Font(FCoreStyle::GetDefaultFontStyle("Bold", 11))
			.ColorAndOpacity(FSlateColor(VIStageTitleColor))
		]
	];
	Content->AddSlot().AutoHeight() [ MakeRow(TEXT("Dry Mass"), Stage1MassText) ];
	Content->AddSlot().AutoHeight() [ MakeRow(TEXT("Propellant"), Stage1PropText) ];
	Content->AddSlot().AutoHeight() [ MakeRow(TEXT("Thrust"), Stage1ThrustText) ];
	Content->AddSlot().AutoHeight() [ MakeRow(TEXT("Isp"), Stage1IspText) ];
	Content->AddSlot().AutoHeight() [ MakeRow(TEXT("TWR"), Stage1TWRText) ];
	Content->AddSlot().AutoHeight() [ MakeRow(TEXT("Delta-V"), Stage1DVText) ];

	// Stage 2
	Content->AddSlot().AutoHeight()
	[
		SNew(SBox).Padding(FMargin(0, 10, 0, 2))
		[
			SNew(STextBlock)
			.Text(FText::FromString(TEXT("STAGE 2 — UPPER STAGE")))
			.Font(FCoreStyle::GetDefaultFontStyle("Bold", 11))
			.ColorAndOpacity(FSlateColor(VIStageTitleColor))
		]
	];
	Content->AddSlot().AutoHeight() [ MakeRow(TEXT("Dry Mass"), Stage2MassText) ];
	Content->AddSlot().AutoHeight() [ MakeRow(TEXT("Propellant"), Stage2PropText) ];
	Content->AddSlot().AutoHeight() [ MakeRow(TEXT("Thrust"), Stage2ThrustText) ];
	Content->AddSlot().AutoHeight() [ MakeRow(TEXT("Isp"), Stage2IspText) ];
	Content->AddSlot().AutoHeight() [ MakeRow(TEXT("TWR"), Stage2TWRText) ];
	Content->AddSlot().AutoHeight() [ MakeRow(TEXT("Delta-V"), Stage2DVText) ];

	// Validation
	Content->AddSlot().AutoHeight() [ MakeHeader(TEXT("VALIDATION")) ];
	Content->AddSlot().AutoHeight()
	[
		SNew(SBox).Padding(FMargin(8, 2))
		[
			SAssignNew(ValidationText, STextBlock)
			.Font(FCoreStyle::GetDefaultFontStyle("Bold", 10))
			.ColorAndOpacity(FSlateColor(VIGoodColor))
			.AutoWrapText(true)
		]
	];

	// Instructions
	Content->AddSlot().AutoHeight()
	[
		SNew(SBox).Padding(FMargin(8, 12, 8, 4))
		[
			SNew(STextBlock)
			.Text(FText::FromString(TEXT("[F5] Launch  [Esc] Return")))
			.Font(FCoreStyle::GetDefaultFontStyle("Regular", 9))
			.ColorAndOpacity(FSlateColor(FLinearColor(0.4f, 0.4f, 0.5f)))
		]
	];

	// Wrap in dark background and scrollbox
	return SNew(SBox)
		.WidthOverride(320.0f)
		.MaxDesiredHeight(700.0f)
		[
			SNew(SBorder)
			.BorderBackgroundColor(FLinearColor(0.02f, 0.02f, 0.04f, 0.92f))
			.Padding(FMargin(8.0f))
			[
				SNew(SScrollBox)
				+ SScrollBox::Slot()
				[
					Content
				]
			]
		];
}

void UAstralisVehicleInspector::SetLaunchVehicle(AAstralisLaunchVehicleActor* Vehicle)
{
	TrackedVehicle = Vehicle;
	UpdateAllValues();
}

void UAstralisVehicleInspector::RefreshDisplay()
{
	UpdateAllValues();
}

void UAstralisVehicleInspector::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	UpdateAllValues();
}

void UAstralisVehicleInspector::UpdateAllValues()
{
	AAstralisLaunchVehicleActor* Vehicle = TrackedVehicle.Get();
	if (!Vehicle) return;

	// Vehicle overview
	if (VehicleNameText)
		VehicleNameText->SetText(FText::FromString(TEXT("Representative 2-Stage Vehicle")));

	if (TotalMassText)
	{
		const double Mass = Vehicle->GetTotalMass_kg();
		TotalMassText->SetText(FText::FromString(
			FString::Printf(TEXT("%.0f kg (%.1f t)"), Mass, Mass / 1000.0)));
	}

	if (TotalDeltaVText)
	{
		const double DV = Vehicle->GetTotalDeltaV_mps();
		TotalDeltaVText->SetText(FText::FromString(
			FString::Printf(TEXT("%.0f m/s"), DV)));

		// Color code: green if sufficient for LEO (~9400 m/s)
		if (DV >= 9400.0)
			TotalDeltaVText->SetColorAndOpacity(FSlateColor(VIGoodColor));
		else if (DV >= 7000.0)
			TotalDeltaVText->SetColorAndOpacity(FSlateColor(VIWarnColor));
		else
			TotalDeltaVText->SetColorAndOpacity(FSlateColor(VIBadColor));
	}

	if (PayloadMassText)
		PayloadMassText->SetText(FText::FromString(TEXT("2,623 kg (Landsat 8)")));

	if (StatusText)
	{
		StatusText->SetText(FText::FromString(Vehicle->GetMissionStateString()));
	}

	// Stage data helper lambda
	auto UpdateStage = [&](int32 Idx,
		TSharedPtr<STextBlock>& MassT, TSharedPtr<STextBlock>& PropT,
		TSharedPtr<STextBlock>& ThrustT, TSharedPtr<STextBlock>& IspT,
		TSharedPtr<STextBlock>& TWRT, TSharedPtr<STextBlock>& DVT)
	{
		if (Idx >= Vehicle->Stages.Num() || !Vehicle->Stages[Idx]) return;

		const AAstralisStageActor* Stage = Vehicle->Stages[Idx];
		const FStageSpec& S = Stage->Spec;

		if (MassT)
			MassT->SetText(FText::FromString(
				FString::Printf(TEXT("%.0f kg"), S.DryMass_kg)));

		if (PropT)
		{
			// Show current/max during flight
			PropT->SetText(FText::FromString(
				FString::Printf(TEXT("%.0f / %.0f kg"), Stage->CurrentPropellant_kg, S.PropellantCapacity_kg)));
		}

		if (ThrustT)
			ThrustT->SetText(FText::FromString(
				FString::Printf(TEXT("%.0f kN"), S.Thrust_N / 1000.0)));

		if (IspT)
			IspT->SetText(FText::FromString(
				FString::Printf(TEXT("%.0f s"), S.Isp_s)));

		// TWR at stage ignition (payload above this stage)
		if (TWRT)
		{
			double UpperMass = 0.0;
			if (Vehicle->Payload)
			{
				UpperMass = FMath::Max(Vehicle->Payload->GetTotalMass(), 2623.0);
			}
			for (int32 j = Idx + 1; j < Vehicle->Stages.Num(); ++j)
			{
				if (Vehicle->Stages[j])
					UpperMass += Vehicle->Stages[j]->GetTotalMass_kg();
			}

			const double StageTotalMass = Stage->GetTotalMass_kg() + UpperMass;
			const double TWR = (StageTotalMass > 0) ? S.Thrust_N / (StageTotalMass * 9.80665) : 0.0;
			TWRT->SetText(FText::FromString(FString::Printf(TEXT("%.2f"), TWR)));

			if (TWR >= 1.2)
				TWRT->SetColorAndOpacity(FSlateColor(VIGoodColor));
			else if (TWR >= 1.0)
				TWRT->SetColorAndOpacity(FSlateColor(VIWarnColor));
			else
				TWRT->SetColorAndOpacity(FSlateColor(VIBadColor));
		}

		// Stage delta-v
		if (DVT)
		{
			double UpperMass = 0.0;
			if (Vehicle->Payload)
				UpperMass = FMath::Max(Vehicle->Payload->GetTotalMass(), 2623.0);
			for (int32 j = Idx + 1; j < Vehicle->Stages.Num(); ++j)
			{
				if (Vehicle->Stages[j])
					UpperMass += Vehicle->Stages[j]->GetTotalMass_kg();
			}

			const double M0 = UpperMass + S.DryMass_kg + S.PropellantCapacity_kg;
			const double M1 = UpperMass + S.DryMass_kg;
			const double DV = (M0 > 0 && M1 > 0 && S.Isp_s > 0)
				? S.Isp_s * 9.80665 * FMath::Loge(M0 / M1)
				: 0.0;
			DVT->SetText(FText::FromString(FString::Printf(TEXT("%.0f m/s"), DV)));
		}
	};

	UpdateStage(0, Stage1MassText, Stage1PropText, Stage1ThrustText, Stage1IspText, Stage1TWRText, Stage1DVText);
	UpdateStage(1, Stage2MassText, Stage2PropText, Stage2ThrustText, Stage2IspText, Stage2TWRText, Stage2DVText);

	// Validation
	if (ValidationText)
	{
		const double TotalDV = Vehicle->GetTotalDeltaV_mps();
		const double TWR = Vehicle->GetThrustToWeightRatio();

		FString ValidationMsg;

		if (TWR < 1.0 && Vehicle->MissionState == ELaunchVehicleState::OnPad)
		{
			ValidationMsg = TEXT("FAIL: TWR < 1.0 — insufficient thrust for liftoff");
			ValidationText->SetColorAndOpacity(FSlateColor(VIBadColor));
		}
		else if (TWR < 1.2 && Vehicle->MissionState == ELaunchVehicleState::OnPad)
		{
			ValidationMsg = TEXT("WARN: TWR < 1.2 — marginal liftoff performance");
			ValidationText->SetColorAndOpacity(FSlateColor(VIWarnColor));
		}
		else if (TotalDV < 9400.0 && Vehicle->MissionState == ELaunchVehicleState::OnPad)
		{
			ValidationMsg = FString::Printf(
				TEXT("WARN: Delta-V %.0f m/s — may be insufficient for LEO (need ~9,400 m/s)"), TotalDV);
			ValidationText->SetColorAndOpacity(FSlateColor(VIWarnColor));
		}
		else if (Vehicle->MissionState == ELaunchVehicleState::Complete)
		{
			ValidationMsg = TEXT("MISSION COMPLETE — Payload deployed to orbit");
			ValidationText->SetColorAndOpacity(FSlateColor(VIGoodColor));
		}
		else if (Vehicle->MissionState == ELaunchVehicleState::OnPad)
		{
			ValidationMsg = TEXT("PASS: Vehicle configuration validated");
			ValidationText->SetColorAndOpacity(FSlateColor(VIGoodColor));
		}
		else
		{
			ValidationMsg = Vehicle->GetMissionStateString();
			ValidationText->SetColorAndOpacity(FSlateColor(VIValueColor));
		}

		ValidationText->SetText(FText::FromString(ValidationMsg));
	}
}
