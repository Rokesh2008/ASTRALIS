#include "AstralisOrbitTelemetryPanel.h"
#include "Astralis/Orbit/AstralisOrbitSceneActor.h"
#include "AstralisSimulation/OrbitalMechanics/AstralisTwoBodyPropagator.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Layout/SScrollBox.h"
#include "Widgets/Text/STextBlock.h"

static const FLinearColor THeaderColor(0.4f, 0.8f, 1.0f);
static const FLinearColor TLabelColor(0.55f, 0.6f, 0.65f);
static const FLinearColor TValueColor(0.95f, 0.95f, 0.95f);
static const FLinearColor TEclipseColor(0.9f, 0.3f, 0.3f);
static const FLinearColor TSunlitColor(0.3f, 0.9f, 0.4f);

TSharedRef<SWidget> UAstralisOrbitTelemetryPanel::MakeRow(
	const FString& Label, TSharedPtr<STextBlock>& OutValue)
{
	return SNew(SHorizontalBox)
		+ SHorizontalBox::Slot().AutoWidth().Padding(6.0f, 1.0f, 4.0f, 1.0f)
		[
			SNew(SBox).WidthOverride(100.0f)
			[
				SNew(STextBlock)
				.Text(FText::FromString(Label))
				.Font(FCoreStyle::GetDefaultFontStyle("Regular", 10))
				.ColorAndOpacity(FSlateColor(TLabelColor))
			]
		]
		+ SHorizontalBox::Slot().FillWidth(1.0f).Padding(0.0f, 1.0f)
		[
			SAssignNew(OutValue, STextBlock)
			.Font(FCoreStyle::GetDefaultFontStyle("Regular", 10))
			.ColorAndOpacity(FSlateColor(TValueColor))
		];
}

static TSharedRef<SWidget> MakeHeader(const FString& Title)
{
	return SNew(SBox)
		.Padding(FMargin(0.0f, 6.0f, 0.0f, 2.0f))
		[
			SNew(STextBlock)
			.Text(FText::FromString(Title))
			.Font(FCoreStyle::GetDefaultFontStyle("Bold", 11))
			.ColorAndOpacity(FSlateColor(THeaderColor))
		];
}

TSharedRef<SWidget> UAstralisOrbitTelemetryPanel::RebuildWidget()
{
	TSharedRef<SVerticalBox> Content = SNew(SVerticalBox);

	Content->AddSlot().AutoHeight().Padding(4.0f)
	[
		SNew(STextBlock)
		.Text(FText::FromString(TEXT("ORBIT TELEMETRY")))
		.Font(FCoreStyle::GetDefaultFontStyle("Bold", 14))
		.ColorAndOpacity(FSlateColor(FLinearColor(0.4f, 0.85f, 1.0f)))
	];

	// Time section
	Content->AddSlot().AutoHeight() [ MakeHeader(TEXT("TIME")) ];
	Content->AddSlot().AutoHeight() [ MakeRow(TEXT("MET"), METText) ];
	Content->AddSlot().AutoHeight() [ MakeRow(TEXT("Time Warp"), TimeWarpText) ];

	// Orbit section
	Content->AddSlot().AutoHeight() [ MakeHeader(TEXT("ORBIT")) ];
	Content->AddSlot().AutoHeight() [ MakeRow(TEXT("Altitude"), AltitudeText) ];
	Content->AddSlot().AutoHeight() [ MakeRow(TEXT("Velocity"), VelocityText) ];
	Content->AddSlot().AutoHeight() [ MakeRow(TEXT("Period"), PeriodText) ];
	Content->AddSlot().AutoHeight() [ MakeRow(TEXT("Eclipse"), EclipseText) ];

	// Power section
	Content->AddSlot().AutoHeight() [ MakeHeader(TEXT("POWER")) ];
	Content->AddSlot().AutoHeight() [ MakeRow(TEXT("Solar"), SolarGenText) ];
	Content->AddSlot().AutoHeight() [ MakeRow(TEXT("Draw"), PowerDrawText) ];
	Content->AddSlot().AutoHeight() [ MakeRow(TEXT("Battery"), BatteryText) ];

	// Comms section
	Content->AddSlot().AutoHeight() [ MakeHeader(TEXT("COMMS")) ];
	Content->AddSlot().AutoHeight() [ MakeRow(TEXT("Link Margin"), LinkMarginText) ];
	Content->AddSlot().AutoHeight() [ MakeRow(TEXT("Data Rate"), DataRateText) ];
	Content->AddSlot().AutoHeight() [ MakeRow(TEXT("Ground Vis"), GroundVisText) ];

	Content->AddSlot().AutoHeight().Padding(4.0f, 10.0f, 4.0f, 2.0f)
	[
		SNew(STextBlock)
		.Text(FText::FromString(TEXT("[Period/Comma] Time Warp  [Space] Pause  [Esc] Assembly")))
		.Font(FCoreStyle::GetDefaultFontStyle("Italic", 9))
		.ColorAndOpacity(FSlateColor(FLinearColor(0.4f, 0.4f, 0.45f)))
	];

	return SNew(SBox)
		.WidthOverride(280.0f)
		.Padding(FMargin(4.0f))
		[
			SNew(SScrollBox)
			+ SScrollBox::Slot() [ Content ]
		];
}

void UAstralisOrbitTelemetryPanel::SetOrbitScene(AAstralisOrbitSceneActor* InScene)
{
	OrbitScene = InScene;
}

void UAstralisOrbitTelemetryPanel::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	RefreshDisplay();
}

static FString FormatMET(double Seconds)
{
	const int32 Days = static_cast<int32>(Seconds / 86400.0);
	const int32 Hours = static_cast<int32>(FMath::Fmod(Seconds, 86400.0) / 3600.0);
	const int32 Mins = static_cast<int32>(FMath::Fmod(Seconds, 3600.0) / 60.0);
	const int32 Secs = static_cast<int32>(FMath::Fmod(Seconds, 60.0));

	if (Days > 0)
		return FString::Printf(TEXT("%dd %02dh %02dm %02ds"), Days, Hours, Mins, Secs);
	if (Hours > 0)
		return FString::Printf(TEXT("%02dh %02dm %02ds"), Hours, Mins, Secs);
	return FString::Printf(TEXT("%02dm %02ds"), Mins, Secs);
}

void UAstralisOrbitTelemetryPanel::RefreshDisplay()
{
	if (!OrbitScene.IsValid()) return;

	const FSpacecraftState& S = OrbitScene->GetSimEngine().GetState();
	const double EarthR = 6.371e6;

	// Time
	if (METText.IsValid())
		METText->SetText(FText::FromString(FormatMET(
			OrbitScene->GetSimEngine().GetClock().GetMissionElapsedTime())));

	if (TimeWarpText.IsValid())
		TimeWarpText->SetText(FText::FromString(OrbitScene->GetTimeWarpLabel()));

	// Orbit
	const double R = S.Orbital.Position_m.Length();
	const double Alt_km = (R - EarthR) / 1000.0;
	const double V_mps = S.Orbital.Velocity_mps.Length();

	if (AltitudeText.IsValid())
		AltitudeText->SetText(FText::FromString(FString::Printf(TEXT("%.1f km"), Alt_km)));

	if (VelocityText.IsValid())
		VelocityText->SetText(FText::FromString(FString::Printf(TEXT("%.1f m/s"), V_mps)));

	if (PeriodText.IsValid())
	{
		const double Energy = FAstralisTwoBodyPropagator::SpecificOrbitalEnergy(
			S.Orbital.Position_m, S.Orbital.Velocity_mps, FAstralisTwoBodyPropagator::Mu_Earth);
		if (Energy < 0.0)
		{
			const double SMA = -FAstralisTwoBodyPropagator::Mu_Earth / (2.0 * Energy);
			const double Period_s = FAstralisTwoBodyPropagator::OrbitalPeriod(SMA, FAstralisTwoBodyPropagator::Mu_Earth);
			const double Period_min = Period_s / 60.0;
			PeriodText->SetText(FText::FromString(FString::Printf(TEXT("%.1f min"), Period_min)));
		}
		else
		{
			PeriodText->SetText(FText::FromString(TEXT("Hyperbolic")));
		}
	}

	if (EclipseText.IsValid())
	{
		if (S.Power.bInEclipse)
		{
			EclipseText->SetText(FText::FromString(TEXT("ECLIPSE")));
			EclipseText->SetColorAndOpacity(FSlateColor(TEclipseColor));
		}
		else
		{
			EclipseText->SetText(FText::FromString(TEXT("Sunlit")));
			EclipseText->SetColorAndOpacity(FSlateColor(TSunlitColor));
		}
	}

	// Power
	if (SolarGenText.IsValid())
		SolarGenText->SetText(FText::FromString(FString::Printf(TEXT("%.1f W"), S.Power.SolarGeneration_W)));

	if (PowerDrawText.IsValid())
		PowerDrawText->SetText(FText::FromString(FString::Printf(TEXT("%.1f W"), S.Power.TotalConsumption_W)));

	if (BatteryText.IsValid())
	{
		const double Energy_Wh = S.Power.BatteryEnergy_J / 3600.0;
		const double Cap_Wh = S.Power.BatteryCapacity_J / 3600.0;
		const double Pct = (Cap_Wh > 0.0) ? (Energy_Wh / Cap_Wh) * 100.0 : 0.0;
		BatteryText->SetText(FText::FromString(
			FString::Printf(TEXT("%.0f / %.0f Wh (%.0f%%)"), Energy_Wh, Cap_Wh, Pct)));
	}

	// Comms
	if (LinkMarginText.IsValid())
		LinkMarginText->SetText(FText::FromString(FString::Printf(TEXT("%.1f dB"), S.Comms.LinkMargin_dB)));

	if (DataRateText.IsValid())
	{
		const double Rate_kbps = S.Comms.AchievableDataRate_bps / 1000.0;
		if (Rate_kbps >= 1000.0)
			DataRateText->SetText(FText::FromString(FString::Printf(TEXT("%.1f Mbps"), Rate_kbps / 1000.0)));
		else
			DataRateText->SetText(FText::FromString(FString::Printf(TEXT("%.1f kbps"), Rate_kbps)));
	}

	if (GroundVisText.IsValid())
	{
		GroundVisText->SetText(FText::FromString(
			S.Comms.bGroundStationVisible ? TEXT("IN VIEW") : TEXT("No Contact")));
		GroundVisText->SetColorAndOpacity(FSlateColor(
			S.Comms.bGroundStationVisible ? TSunlitColor : TEclipseColor));
	}
}
