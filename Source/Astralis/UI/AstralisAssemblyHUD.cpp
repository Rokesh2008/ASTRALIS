#include "AstralisAssemblyHUD.h"
#include "Engine/Canvas.h"
#include "Engine/Engine.h"
#include "Astralis/Core/AstralisAssemblyGameMode.h"
#include "Astralis/Core/AstralisAssemblyPC.h"
#include "Astralis/Core/AstralisAssemblyPawn.h"
#include "Astralis/LaunchVehicle/AstralisLaunchVehicleActor.h"
#include "Astralis/LaunchVehicle/AstralisStageActor.h"
#include "Astralis/Orbit/AstralisOrbitSceneActor.h"
#include "AstralisData/VehicleLibrary/AstralisVehicleTypes.h"

void AAstralisAssemblyHUD::BeginPlay()
{
	Super::BeginPlay();
}

void AAstralisAssemblyHUD::OnComponentChosen(UAstralisComponentData* Data)
{
	if (AAstralisAssemblyPC* PC = Cast<AAstralisAssemblyPC>(GetOwningPlayerController()))
	{
		PC->SetPlacementData(Data);
	}
}

void AAstralisAssemblyHUD::OnPhaseChanged(EAstralisGamePhase NewPhase)
{
	CurrentPhase = NewPhase;
}

// ─────────────────────────────────────────────────────────────────────────────
// Helper: panel background with accent line
// ─────────────────────────────────────────────────────────────────────────────
void AAstralisAssemblyHUD::DrawPanel(float X, float Y, float PW, float PH,
	const FString& Title, const FLinearColor& Accent)
{
	// Outer glow border (subtle bloom effect)
	const float GlowSize = 2.0f;
	FLinearColor GlowCol(Accent.R, Accent.G, Accent.B, 0.12f);
	FCanvasTileItem GlowT(FVector2D(X - GlowSize, Y - GlowSize), FVector2D(PW + GlowSize * 2, GlowSize), GlowCol);
	GlowT.BlendMode = SE_BLEND_Translucent; Canvas->DrawItem(GlowT);
	FCanvasTileItem GlowB(FVector2D(X - GlowSize, Y + PH), FVector2D(PW + GlowSize * 2, GlowSize), GlowCol);
	GlowB.BlendMode = SE_BLEND_Translucent; Canvas->DrawItem(GlowB);
	FCanvasTileItem GlowL(FVector2D(X - GlowSize, Y), FVector2D(GlowSize, PH), GlowCol);
	GlowL.BlendMode = SE_BLEND_Translucent; Canvas->DrawItem(GlowL);
	FCanvasTileItem GlowR(FVector2D(X + PW, Y), FVector2D(GlowSize, PH), GlowCol);
	GlowR.BlendMode = SE_BLEND_Translucent; Canvas->DrawItem(GlowR);

	// Main background — gradient effect (darker at bottom)
	const float HalfH = PH * 0.5f;
	FCanvasTileItem BGTop(FVector2D(X, Y), FVector2D(PW, HalfH),
		FLinearColor(0.03f, 0.05f, 0.10f, 0.90f));
	BGTop.BlendMode = SE_BLEND_Translucent;
	Canvas->DrawItem(BGTop);
	FCanvasTileItem BGBot(FVector2D(X, Y + HalfH), FVector2D(PW, PH - HalfH),
		FLinearColor(0.015f, 0.03f, 0.06f, 0.92f));
	BGBot.BlendMode = SE_BLEND_Translucent;
	Canvas->DrawItem(BGBot);

	// 1px border
	FLinearColor BorderCol(Accent.R * 0.5f, Accent.G * 0.5f, Accent.B * 0.5f, 0.4f);
	FCanvasTileItem BT(FVector2D(X, Y), FVector2D(PW, 1.0f), BorderCol);
	BT.BlendMode = SE_BLEND_Translucent; Canvas->DrawItem(BT);
	FCanvasTileItem BB(FVector2D(X, Y + PH - 1.0f), FVector2D(PW, 1.0f), BorderCol);
	BB.BlendMode = SE_BLEND_Translucent; Canvas->DrawItem(BB);
	FCanvasTileItem BL(FVector2D(X, Y), FVector2D(1.0f, PH), BorderCol);
	BL.BlendMode = SE_BLEND_Translucent; Canvas->DrawItem(BL);
	FCanvasTileItem BR(FVector2D(X + PW - 1.0f, Y), FVector2D(1.0f, PH), BorderCol);
	BR.BlendMode = SE_BLEND_Translucent; Canvas->DrawItem(BR);

	// Top accent line (brighter, thicker)
	FCanvasTileItem AccLine(FVector2D(X, Y), FVector2D(PW, 3.0f), Accent);
	AccLine.BlendMode = SE_BLEND_Translucent;
	Canvas->DrawItem(AccLine);

	// Title with subtle shadow
	if (!Title.IsEmpty())
	{
		UFont* SmFont = GEngine->GetSmallFont();
		FCanvasTextItem Shadow(FVector2D(X + 11.0f, Y + 9.0f),
			FText::FromString(Title), SmFont, FLinearColor(0.0f, 0.0f, 0.0f, 0.5f));
		Shadow.Scale = FVector2D(1.1f, 1.1f);
		Canvas->DrawItem(Shadow);

		FCanvasTextItem TItem(FVector2D(X + 10.0f, Y + 8.0f),
			FText::FromString(Title), SmFont, Accent);
		TItem.Scale = FVector2D(1.1f, 1.1f);
		Canvas->DrawItem(TItem);
	}
}

// ─────────────────────────────────────────────────────────────────────────────
// Helper: horizontal progress bar with label
// ─────────────────────────────────────────────────────────────────────────────
void AAstralisAssemblyHUD::DrawProgressBar(float X, float Y, float PW, float PH,
	double Value, double Max, const FLinearColor& Color, const FString& Label)
{
	// Background
	FCanvasTileItem BG(FVector2D(X, Y), FVector2D(PW, PH),
		FLinearColor(0.08f, 0.08f, 0.12f, 0.9f));
	BG.BlendMode = SE_BLEND_Translucent;
	Canvas->DrawItem(BG);

	// Fill
	const float Frac = (Max > 0.0) ? static_cast<float>(FMath::Clamp(Value / Max, 0.0, 1.0)) : 0.0f;
	if (Frac > 0.0f)
	{
		FCanvasTileItem Fill(FVector2D(X, Y), FVector2D(PW * Frac, PH), Color);
		Fill.BlendMode = SE_BLEND_Translucent;
		Canvas->DrawItem(Fill);
	}

	// Label
	if (!Label.IsEmpty())
	{
		UFont* Fn = GEngine->GetSmallFont();
		FCanvasTextItem TI(FVector2D(X + 4.0f, Y + 1.0f),
			FText::FromString(Label), Fn, FLinearColor::White);
		Canvas->DrawItem(TI);
	}
}

// ─────────────────────────────────────────────────────────────────────────────
// Helper: vertical gauge (altitude/velocity style)
// ─────────────────────────────────────────────────────────────────────────────
void AAstralisAssemblyHUD::DrawVerticalGauge(float X, float Y, float GW, float GH,
	double Value, double Min, double Max,
	const FString& Label, const FString& Unit,
	const FLinearColor& Color)
{
	// Background
	FCanvasTileItem BG(FVector2D(X, Y), FVector2D(GW, GH),
		FLinearColor(0.02f, 0.03f, 0.06f, 0.85f));
	BG.BlendMode = SE_BLEND_Translucent;
	Canvas->DrawItem(BG);

	// Border
	FCanvasTileItem Border(FVector2D(X, Y), FVector2D(2.0f, GH), FLinearColor(0.15f, 0.25f, 0.4f, 0.6f));
	Border.BlendMode = SE_BLEND_Translucent;
	Canvas->DrawItem(Border);

	// Fill from bottom
	const double Range = Max - Min;
	const float Frac = (Range > 0.0) ? static_cast<float>(FMath::Clamp((Value - Min) / Range, 0.0, 1.0)) : 0.0f;
	const float FillH = GH * Frac;
	if (FillH > 0.0f)
	{
		FLinearColor FillColor = Color;
		FillColor.A = 0.5f;
		FCanvasTileItem Fill(FVector2D(X + 2.0f, Y + GH - FillH),
			FVector2D(GW - 2.0f, FillH), FillColor);
		Fill.BlendMode = SE_BLEND_Translucent;
		Canvas->DrawItem(Fill);
	}

	// Tick marks (5 evenly spaced)
	for (int32 i = 0; i <= 4; ++i)
	{
		const float TY = Y + GH - (GH * i / 4.0f);
		FCanvasTileItem Tick(FVector2D(X, TY), FVector2D(6.0f, 1.0f),
			FLinearColor(0.4f, 0.5f, 0.6f, 0.6f));
		Tick.BlendMode = SE_BLEND_Translucent;
		Canvas->DrawItem(Tick);

		const double TickVal = Min + Range * i / 4.0;
		FString TickStr = (TickVal >= 1000.0) ?
			FString::Printf(TEXT("%.0fk"), TickVal / 1000.0) :
			FString::Printf(TEXT("%.0f"), TickVal);
		UFont* Fn = GEngine->GetSmallFont();
		FCanvasTextItem TTI(FVector2D(X + 8.0f, TY - 6.0f),
			FText::FromString(TickStr), Fn, FLinearColor(0.5f, 0.6f, 0.7f, 0.8f));
		TTI.Scale = FVector2D(0.85f, 0.85f);
		Canvas->DrawItem(TTI);
	}

	// Current value - big text
	UFont* BigFont = GEngine->GetLargeFont();
	FString ValStr = FString::Printf(TEXT("%.1f"), Value);
	FCanvasTextItem VI(FVector2D(X + 4.0f, Y + GH + 4.0f),
		FText::FromString(ValStr), BigFont, Color);
	Canvas->DrawItem(VI);

	// Unit
	UFont* SmFont = GEngine->GetSmallFont();
	FCanvasTextItem UI(FVector2D(X + 4.0f, Y + GH + 22.0f),
		FText::FromString(Unit), SmFont, FLinearColor(0.6f, 0.7f, 0.8f, 0.8f));
	Canvas->DrawItem(UI);

	// Label at top
	FCanvasTextItem LI(FVector2D(X + 4.0f, Y - 16.0f),
		FText::FromString(Label), SmFont, FLinearColor(0.7f, 0.8f, 1.0f, 0.9f));
	Canvas->DrawItem(LI);
}

// ─────────────────────────────────────────────────────────────────────────────
// Telemetry graph — altitude + velocity curves
// ─────────────────────────────────────────────────────────────────────────────
void AAstralisAssemblyHUD::DrawTelemetryGraph(float X, float Y, float GW, float GH,
	AAstralisLaunchVehicleActor* V)
{
	if (!V || V->TelemAltitude_km.Num() < 2) return;

	DrawPanel(X, Y, GW, GH, TEXT(""), FLinearColor(0.0f, 0.5f, 0.8f, 0.7f));

	const float MarginL = 40.0f;
	const float MarginR = 40.0f;
	const float MarginT = 6.0f;
	const float MarginB = 18.0f;
	const float PlotX = X + MarginL;
	const float PlotY = Y + MarginT;
	const float PlotW = GW - MarginL - MarginR;
	const float PlotH = GH - MarginT - MarginB;

	UFont* Fn = GEngine->GetSmallFont();

	double MaxAlt = 1.0;
	double MaxVel = 1.0;
	for (int32 i = 0; i < V->TelemAltitude_km.Num(); ++i)
	{
		MaxAlt = FMath::Max(MaxAlt, V->TelemAltitude_km[i]);
		MaxVel = FMath::Max(MaxVel, V->TelemVelocity_mps[i]);
	}
	MaxAlt = FMath::CeilToDouble(MaxAlt / 50.0) * 50.0;
	MaxVel = FMath::CeilToDouble(MaxVel / 500.0) * 500.0;

	const int32 N = V->TelemAltitude_km.Num();

	// Grid lines (horizontal — altitude scale)
	for (int32 g = 1; g <= 3; ++g)
	{
		float Frac = g / 4.0f;
		float GY = PlotY + PlotH * (1.0f - Frac);
		FCanvasTileItem Grid(FVector2D(PlotX, GY), FVector2D(PlotW, 1.0f),
			FLinearColor(0.15f, 0.2f, 0.3f, 0.3f));
		Grid.BlendMode = SE_BLEND_Translucent;
		Canvas->DrawItem(Grid);

		FCanvasTextItem ALabel(FVector2D(X + 2.0f, GY - 5.0f),
			FText::FromString(FString::Printf(TEXT("%.0f"), MaxAlt * Frac)),
			Fn, FLinearColor(0.3f, 0.5f, 0.8f, 0.5f));
		ALabel.Scale = FVector2D(0.6f, 0.6f);
		Canvas->DrawItem(ALabel);

		FCanvasTextItem VLabel(FVector2D(X + GW - MarginR + 4.0f, GY - 5.0f),
			FText::FromString(FString::Printf(TEXT("%.0f"), MaxVel * Frac)),
			Fn, FLinearColor(0.3f, 0.8f, 0.5f, 0.5f));
		VLabel.Scale = FVector2D(0.6f, 0.6f);
		Canvas->DrawItem(VLabel);
	}

	// Target altitude reference line
	const double TargetAlt = V->GetTargetAltitude_m() / 1000.0;
	if (TargetAlt > 0 && TargetAlt <= MaxAlt)
	{
		float TgtY = PlotY + PlotH - PlotH * static_cast<float>(TargetAlt / MaxAlt);
		for (float dx = 0; dx < PlotW; dx += 8.0f)
		{
			FCanvasTileItem Dash(FVector2D(PlotX + dx, TgtY), FVector2D(4.0f, 1.0f),
				FLinearColor(1.0f, 0.8f, 0.2f, 0.4f));
			Dash.BlendMode = SE_BLEND_Translucent;
			Canvas->DrawItem(Dash);
		}
	}

	// Altitude curve (blue) with fill
	for (int32 i = 1; i < N; ++i)
	{
		const float X0 = PlotX + PlotW * (i - 1) / (N - 1);
		const float X1 = PlotX + PlotW * i / (N - 1);
		const float Y0 = PlotY + PlotH - PlotH * static_cast<float>(V->TelemAltitude_km[i - 1] / MaxAlt);
		const float Y1 = PlotY + PlotH - PlotH * static_cast<float>(V->TelemAltitude_km[i] / MaxAlt);

		// Fill below curve
		float FillTop = FMath::Min(Y0, Y1);
		float FillBot = PlotY + PlotH;
		FCanvasTileItem Fill(FVector2D(X0, FillTop), FVector2D(X1 - X0 + 1.0f, FillBot - FillTop),
			FLinearColor(0.1f, 0.3f, 0.6f, 0.15f));
		Fill.BlendMode = SE_BLEND_Translucent;
		Canvas->DrawItem(Fill);

		const float DX = X1 - X0;
		const float DY = Y1 - Y0;
		if (FMath::Sqrt(DX * DX + DY * DY) < 0.5f) continue;

		FCanvasTileItem Seg(FVector2D(FMath::Min(X0, X1), FMath::Min(Y0, Y1)),
			FVector2D(FMath::Abs(DX) + 2.0f, FMath::Max(2.0f, FMath::Abs(DY) + 1.0f)),
			FLinearColor(0.2f, 0.6f, 1.0f, 0.9f));
		Seg.BlendMode = SE_BLEND_Translucent;
		Canvas->DrawItem(Seg);
	}

	// Velocity curve (green)
	for (int32 i = 1; i < N; ++i)
	{
		const float X0 = PlotX + PlotW * (i - 1) / (N - 1);
		const float X1 = PlotX + PlotW * i / (N - 1);
		const float Y0 = PlotY + PlotH - PlotH * static_cast<float>(V->TelemVelocity_mps[i - 1] / MaxVel);
		const float Y1 = PlotY + PlotH - PlotH * static_cast<float>(V->TelemVelocity_mps[i] / MaxVel);
		const float DX = X1 - X0;
		const float DY = Y1 - Y0;
		FCanvasTileItem Seg(FVector2D(FMath::Min(X0, X1), FMath::Min(Y0, Y1)),
			FVector2D(FMath::Abs(DX) + 2.0f, FMath::Max(2.0f, FMath::Abs(DY) + 1.0f)),
			FLinearColor(0.2f, 1.0f, 0.4f, 0.9f));
		Seg.BlendMode = SE_BLEND_Translucent;
		Canvas->DrawItem(Seg);
	}

	// Current value dot markers at end of curves
	if (N > 0)
	{
		float EndX = PlotX + PlotW;
		float AltEndY = PlotY + PlotH - PlotH * static_cast<float>(V->TelemAltitude_km.Last() / MaxAlt);
		float VelEndY = PlotY + PlotH - PlotH * static_cast<float>(V->TelemVelocity_mps.Last() / MaxVel);

		FCanvasTileItem ADot(FVector2D(EndX - 3.0f, AltEndY - 3.0f), FVector2D(6.0f, 6.0f),
			FLinearColor(0.3f, 0.7f, 1.0f, 1.0f));
		ADot.BlendMode = SE_BLEND_Translucent;
		Canvas->DrawItem(ADot);

		FCanvasTileItem VDot(FVector2D(EndX - 3.0f, VelEndY - 3.0f), FVector2D(6.0f, 6.0f),
			FLinearColor(0.3f, 1.0f, 0.5f, 1.0f));
		VDot.BlendMode = SE_BLEND_Translucent;
		Canvas->DrawItem(VDot);
	}

	// Legend with axis units
	FCanvasTextItem AltAxis(FVector2D(X + 2.0f, Y + GH - MarginB + 2.0f),
		FText::FromString(TEXT("km")), Fn, FLinearColor(0.3f, 0.5f, 0.8f, 0.5f));
	AltAxis.Scale = FVector2D(0.6f, 0.6f);
	Canvas->DrawItem(AltAxis);

	FCanvasTextItem AltLbl(FVector2D(PlotX + 4.0f, Y + GH - MarginB + 2.0f),
		FText::FromString(FString::Printf(TEXT("ALT %.0f km"), V->Altitude_m / 1000.0)),
		Fn, FLinearColor(0.3f, 0.7f, 1.0f, 1.0f));
	AltLbl.Scale = FVector2D(0.85f, 0.85f);
	Canvas->DrawItem(AltLbl);

	FCanvasTextItem VelLbl(FVector2D(PlotX + PlotW * 0.45f, Y + GH - MarginB + 2.0f),
		FText::FromString(FString::Printf(TEXT("VEL %.0f m/s"), V->Velocity_mps)),
		Fn, FLinearColor(0.3f, 1.0f, 0.5f, 1.0f));
	VelLbl.Scale = FVector2D(0.85f, 0.85f);
	Canvas->DrawItem(VelLbl);

	FCanvasTextItem VelAxis(FVector2D(X + GW - MarginR + 4.0f, Y + GH - MarginB + 2.0f),
		FText::FromString(TEXT("m/s")), Fn, FLinearColor(0.3f, 0.8f, 0.5f, 0.5f));
	VelAxis.Scale = FVector2D(0.6f, 0.6f);
	Canvas->DrawItem(VelAxis);
}

// ─────────────────────────────────────────────────────────────────────────────
// Mission timeline
// ─────────────────────────────────────────────────────────────────────────────
void AAstralisAssemblyHUD::DrawMissionTimeline(float X, float Y, float TW, float TH,
	AAstralisLaunchVehicleActor* V)
{
	if (!V) return;

	DrawPanel(X, Y, TW, TH, TEXT(""), FLinearColor(0.0f, 0.4f, 0.7f, 0.8f));

	struct FMilestone { FString Label; double MET; bool bReached; };
	TArray<FMilestone> Milestones;
	Milestones.Add({TEXT("LIFTOFF"), 0.0, V->MissionElapsedTime_s >= 0.0 &&
		V->MissionState != ELaunchVehicleState::OnPad && V->MissionState != ELaunchVehicleState::Countdown});
	Milestones.Add({TEXT("MAX-Q"), 70.0, V->MissionElapsedTime_s > 70.0 && Milestones[0].bReached});
	Milestones.Add({TEXT("MECO"), 162.0, V->CurrentStageIndex >= 1});
	Milestones.Add({TEXT("SEP"), 165.0, V->CurrentStageIndex >= 1});
	Milestones.Add({TEXT("S2 IGN"), 170.0, V->CurrentStageIndex >= 1 &&
		(V->MissionState == ELaunchVehicleState::Ascent || V->MissionState == ELaunchVehicleState::Complete)});
	Milestones.Add({TEXT("FAIRING"), 200.0, V->Altitude_m > 110.0e3});
	Milestones.Add({TEXT("ORBIT"), 500.0, V->bOrbitAchieved});

	const float BarY = Y + 8.0f;
	const float BarH = 4.0f;
	const float MarginX = 30.0f;
	const float BarW = TW - 2.0f * MarginX;

	// Track line
	FCanvasTileItem Track(FVector2D(X + MarginX, BarY), FVector2D(BarW, BarH),
		FLinearColor(0.15f, 0.2f, 0.3f, 0.8f));
	Track.BlendMode = SE_BLEND_Translucent;
	Canvas->DrawItem(Track);

	// Progress fill
	const double TotalT = 500.0;
	const float ProgFrac = static_cast<float>(FMath::Clamp(V->MissionElapsedTime_s / TotalT, 0.0, 1.0));
	if (ProgFrac > 0.0f)
	{
		FCanvasTileItem Prog(FVector2D(X + MarginX, BarY), FVector2D(BarW * ProgFrac, BarH),
			FLinearColor(0.0f, 0.7f, 1.0f, 0.9f));
		Prog.BlendMode = SE_BLEND_Translucent;
		Canvas->DrawItem(Prog);
	}

	// Milestone markers
	UFont* Fn = GEngine->GetSmallFont();
	for (int32 i = 0; i < Milestones.Num(); ++i)
	{
		const float Frac = static_cast<float>(FMath::Clamp(Milestones[i].MET / TotalT, 0.0, 1.0));
		const float MX = X + MarginX + BarW * Frac;

		FLinearColor DotColor = Milestones[i].bReached
			? FLinearColor(0.1f, 1.0f, 0.4f, 1.0f)
			: FLinearColor(0.4f, 0.5f, 0.6f, 0.7f);

		FCanvasTileItem Dot(FVector2D(MX - 3.0f, BarY - 2.0f), FVector2D(6.0f, BarH + 4.0f), DotColor);
		Dot.BlendMode = SE_BLEND_Translucent;
		Canvas->DrawItem(Dot);

		float LblW, LblH;
		Canvas->TextSize(Fn, Milestones[i].Label, LblW, LblH);
		FCanvasTextItem LblItem(FVector2D(MX - LblW * 0.4f, BarY + BarH + 4.0f),
			FText::FromString(Milestones[i].Label), Fn, DotColor);
		LblItem.Scale = FVector2D(0.8f, 0.8f);
		Canvas->DrawItem(LblItem);
	}
}

// ─────────────────────────────────────────────────────────────────────────────
// Title and bottom bars
// ─────────────────────────────────────────────────────────────────────────────
void AAstralisAssemblyHUD::DrawTitleBar(float W, float /*H*/)
{
	// Gradient background — darker at top
	FCanvasTileItem BarTop(FVector2D(0, 0), FVector2D(W, 20.0f),
		FLinearColor(0.005f, 0.01f, 0.04f, 0.95f));
	BarTop.BlendMode = SE_BLEND_Translucent;
	Canvas->DrawItem(BarTop);
	FCanvasTileItem BarBot(FVector2D(0, 20.0f), FVector2D(W, 24.0f),
		FLinearColor(0.01f, 0.025f, 0.07f, 0.90f));
	BarBot.BlendMode = SE_BLEND_Translucent;
	Canvas->DrawItem(BarBot);

	// Bottom accent — glowing blue line
	FCanvasTileItem AccGlow(FVector2D(0, 42.0f), FVector2D(W, 3.0f),
		FLinearColor(0.0f, 0.4f, 0.9f, 0.3f));
	AccGlow.BlendMode = SE_BLEND_Translucent;
	Canvas->DrawItem(AccGlow);
	FCanvasTileItem AccLine(FVector2D(0, 43.0f), FVector2D(W, 1.5f),
		FLinearColor(0.0f, 0.55f, 1.0f, 1.0f));
	AccLine.BlendMode = SE_BLEND_Translucent;
	Canvas->DrawItem(AccLine);

	UFont* BigFont = GEngine->GetLargeFont();
	UFont* SmFont = GEngine->GetSmallFont();

	// Title with drop shadow
	FCanvasTextItem TitleShadow(FVector2D(17.0f, 9.0f),
		FText::FromString(TEXT("ASTRALIS")), BigFont, FLinearColor(0.0f, 0.0f, 0.0f, 0.6f));
	Canvas->DrawItem(TitleShadow);

	FCanvasTextItem Title(FVector2D(16.0f, 8.0f),
		FText::FromString(TEXT("ASTRALIS")), BigFont, FLinearColor::White);
	Canvas->DrawItem(Title);

	float TW_text, TH_text;
	Canvas->TextSize(BigFont, TEXT("ASTRALIS"), TW_text, TH_text);

	// Vertical separator
	const float SepX = 20.0f + TW_text + 6.0f;
	FCanvasTileItem Sep(FVector2D(SepX, 8.0f), FVector2D(1.0f, 28.0f),
		FLinearColor(0.2f, 0.4f, 0.8f, 0.4f));
	Sep.BlendMode = SE_BLEND_Translucent;
	Canvas->DrawItem(Sep);

	FCanvasTextItem Sub(FVector2D(SepX + 10.0f, 10.0f),
		FText::FromString(TEXT("SPACEFLIGHT ENGINEERING")), SmFont,
		FLinearColor(0.5f, 0.7f, 1.0f, 0.85f));
	Sub.Scale = FVector2D(1.0f, 1.0f);
	Canvas->DrawItem(Sub);

	FCanvasTextItem Sub2(FVector2D(SepX + 10.0f, 24.0f),
		FText::FromString(TEXT("SIMULATOR")), SmFont,
		FLinearColor(0.35f, 0.55f, 0.85f, 0.65f));
	Sub2.Scale = FVector2D(0.9f, 0.9f);
	Canvas->DrawItem(Sub2);

	// Phase on right — with background pill
	FString PhaseStr;
	FLinearColor PhaseColor = FLinearColor::White;
	switch (CurrentPhase)
	{
	case EAstralisGamePhase::Assembly:
		PhaseStr = TEXT("ASSEMBLY"); PhaseColor = FLinearColor(0.2f, 0.85f, 0.4f, 1.0f); break;
	case EAstralisGamePhase::Launch:
		PhaseStr = TEXT("LAUNCH"); PhaseColor = FLinearColor(1.0f, 0.5f, 0.1f, 1.0f); break;
	case EAstralisGamePhase::Orbit:
		PhaseStr = TEXT("ORBIT"); PhaseColor = FLinearColor(0.3f, 0.7f, 1.0f, 1.0f); break;
	case EAstralisGamePhase::Reentry:
		PhaseStr = TEXT("REENTRY"); PhaseColor = FLinearColor(1.0f, 0.3f, 0.1f, 1.0f); break;
	}
	float PW, PH;
	Canvas->TextSize(BigFont, PhaseStr, PW, PH);

	// Phase background pill
	const float PillPad = 12.0f;
	FCanvasTileItem PhaseBG(FVector2D(W - PW - 18.0f - PillPad, 5.0f),
		FVector2D(PW + PillPad * 2, 34.0f),
		FLinearColor(PhaseColor.R * 0.15f, PhaseColor.G * 0.15f, PhaseColor.B * 0.15f, 0.5f));
	PhaseBG.BlendMode = SE_BLEND_Translucent;
	Canvas->DrawItem(PhaseBG);

	FCanvasTextItem PhaseItem(FVector2D(W - PW - 18.0f, 8.0f),
		FText::FromString(PhaseStr), BigFont, PhaseColor);
	Canvas->DrawItem(PhaseItem);

	// Mission name and clock — center of title bar
	{
		AAstralisAssemblyGameMode* TitleGM = Cast<AAstralisAssemblyGameMode>(GetWorld()->GetAuthGameMode());
		if (TitleGM && TitleGM->GetMissions().Num() > 0)
		{
			const FMissionDefinition& TitleMission = TitleGM->GetSelectedMission();
			FString MissionStr = TitleMission.Name.ToUpper();
			float MW_t, MH_t;
			Canvas->TextSize(SmFont, MissionStr, MW_t, MH_t);
			FCanvasTextItem MsnI(FVector2D((W - MW_t) / 2.0f, 6.0f),
				FText::FromString(MissionStr), SmFont,
				FLinearColor(0.7f, 0.8f, 0.9f, 0.7f));
			MsnI.Scale = FVector2D(0.9f, 0.9f);
			Canvas->DrawItem(MsnI);

			FString ClockStr;
			if (CurrentPhase == EAstralisGamePhase::Launch && TitleGM->LaunchVehicle)
			{
				const double MET = TitleGM->LaunchVehicle->MissionElapsedTime_s;
				ClockStr = FString::Printf(TEXT("T+ %02d:%02d:%02d"),
					static_cast<int32>(MET / 3600.0),
					static_cast<int32>(FMath::Fmod(MET, 3600.0) / 60.0),
					static_cast<int32>(FMath::Fmod(MET, 60.0)));
			}
			else if (CurrentPhase == EAstralisGamePhase::Orbit)
			{
				const double OT = TitleGM->SCResources.MissionTime_s;
				const int32 D = static_cast<int32>(OT / 86400.0);
				const int32 Hr = static_cast<int32>(FMath::Fmod(OT, 86400.0) / 3600.0);
				const int32 Mn = static_cast<int32>(FMath::Fmod(OT, 3600.0) / 60.0);
				ClockStr = FString::Printf(TEXT("MET %dd %02d:%02d"), D, Hr, Mn);
			}
			else if (CurrentPhase == EAstralisGamePhase::Reentry)
			{
				ClockStr = FString::Printf(TEXT("REENTRY T+ %.0fs"), TitleGM->ReentryTimer);
			}

			if (!ClockStr.IsEmpty())
			{
				float CW_t, CH_t;
				Canvas->TextSize(SmFont, ClockStr, CW_t, CH_t);
				FCanvasTextItem ClkI(FVector2D((W - CW_t) / 2.0f, 22.0f),
					FText::FromString(ClockStr), SmFont,
					FLinearColor(0.4f, 0.6f, 0.8f, 0.6f));
				ClkI.Scale = FVector2D(0.85f, 0.85f);
				Canvas->DrawItem(ClkI);
			}
		}
	}

	// Camera mode indicator during Launch
	if (CurrentPhase == EAstralisGamePhase::Launch || CurrentPhase == EAstralisGamePhase::Orbit)
	{
		APlayerController* PC = GetOwningPlayerController();
		if (PC)
		{
			if (AAstralisAssemblyPawn* Pawn = Cast<AAstralisAssemblyPawn>(PC->GetPawn()))
			{
				FString CamStr = FString::Printf(TEXT("[C] %s"), *Pawn->GetCameraModeString());
				float CW, CH;
				Canvas->TextSize(SmFont, CamStr, CW, CH);
				FCanvasTextItem CamItem(FVector2D(W - PW - PillPad - CW - 30.0f, 14.0f),
					FText::FromString(CamStr), SmFont, FLinearColor(0.5f, 0.7f, 0.9f, 0.8f));
				Canvas->DrawItem(CamItem);
			}
		}
	}
}

void AAstralisAssemblyHUD::DrawBottomBar(float W, float H)
{
	const float BarH = 32.0f;
	const float BarY = H - BarH;

	// Background gradient (darker center, slightly lighter edges)
	FCanvasTileItem Bar(FVector2D(0, BarY), FVector2D(W, BarH),
		FLinearColor(0.008f, 0.015f, 0.035f, 0.92f));
	Bar.BlendMode = SE_BLEND_Translucent;
	Canvas->DrawItem(Bar);

	// Top edge glow line
	FCanvasTileItem TopLine(FVector2D(0, BarY), FVector2D(W, 1.0f),
		FLinearColor(0.15f, 0.35f, 0.7f, 0.5f));
	TopLine.BlendMode = SE_BLEND_Translucent;
	Canvas->DrawItem(TopLine);

	UFont* Fn = GEngine->GetSmallFont();
	const float TxtY = BarY + 8.0f;

	// Left: timestamp with LIVE indicator
	FDateTime Now = FDateTime::Now();
	FString TimeStr = FString::Printf(TEXT("UTC %04d-%02d-%02d  %02d:%02d:%02d"),
		Now.GetYear(), Now.GetMonth(), Now.GetDay(), Now.GetHour(), Now.GetMinute(), Now.GetSecond());

	// Pulsing LIVE dot
	const float Pulse = 0.5f + 0.5f * FMath::Sin(GetWorld()->GetTimeSeconds() * 3.0f);
	FCanvasTileItem LiveDot(FVector2D(12.0f, TxtY + 3.0f), FVector2D(6.0f, 6.0f),
		FLinearColor(1.0f, 0.15f, 0.1f, 0.6f + Pulse * 0.4f));
	LiveDot.BlendMode = SE_BLEND_Translucent;
	Canvas->DrawItem(LiveDot);

	FCanvasTextItem LiveTxt(FVector2D(22.0f, TxtY),
		FText::FromString(TEXT("LIVE")), Fn, FLinearColor(1.0f, 0.3f, 0.2f, 0.9f));
	LiveTxt.Scale = FVector2D(0.9f, 0.9f);
	Canvas->DrawItem(LiveTxt);

	// Separator
	FCanvasTileItem Sep1(FVector2D(60.0f, BarY + 6.0f), FVector2D(1.0f, BarH - 12.0f),
		FLinearColor(0.3f, 0.4f, 0.6f, 0.3f));
	Sep1.BlendMode = SE_BLEND_Translucent;
	Canvas->DrawItem(Sep1);

	FCanvasTextItem TimeItem(FVector2D(68.0f, TxtY),
		FText::FromString(TimeStr), Fn, FLinearColor(0.45f, 0.55f, 0.7f, 0.7f));
	TimeItem.Scale = FVector2D(0.9f, 0.9f);
	Canvas->DrawItem(TimeItem);

	// Right side: version info with separators
	const FString InfoStr = TEXT("ASTRALIS v2.0");
	const FString StatsStr = TEXT("17 Vehicles  |  13 Missions");
	const FString EngineStr = TEXT("UE 5.8.4");

	FCanvasTextItem EngItem(FVector2D(W - 80.0f, TxtY),
		FText::FromString(EngineStr), Fn, FLinearColor(0.35f, 0.45f, 0.55f, 0.5f));
	EngItem.Scale = FVector2D(0.9f, 0.9f);
	Canvas->DrawItem(EngItem);

	FCanvasTileItem Sep2(FVector2D(W - 90.0f, BarY + 6.0f), FVector2D(1.0f, BarH - 12.0f),
		FLinearColor(0.3f, 0.4f, 0.6f, 0.3f));
	Sep2.BlendMode = SE_BLEND_Translucent;
	Canvas->DrawItem(Sep2);

	FCanvasTextItem StatsItem(FVector2D(W - 290.0f, TxtY),
		FText::FromString(StatsStr), Fn, FLinearColor(0.4f, 0.5f, 0.6f, 0.6f));
	StatsItem.Scale = FVector2D(0.9f, 0.9f);
	Canvas->DrawItem(StatsItem);

	FCanvasTileItem Sep3(FVector2D(W - 300.0f, BarY + 6.0f), FVector2D(1.0f, BarH - 12.0f),
		FLinearColor(0.3f, 0.4f, 0.6f, 0.3f));
	Sep3.BlendMode = SE_BLEND_Translucent;
	Canvas->DrawItem(Sep3);

	FCanvasTextItem VerItem(FVector2D(W - 410.0f, TxtY),
		FText::FromString(InfoStr), Fn, FLinearColor(0.3f, 0.7f, 1.0f, 0.7f));
	VerItem.Scale = FVector2D(0.9f, 0.9f);
	Canvas->DrawItem(VerItem);
}

// ─────────────────────────────────────────────────────────────────────────────
// ASSEMBLY PHASE HUD
// ─────────────────────────────────────────────────────────────────────────────
void AAstralisAssemblyHUD::DrawAssemblyHUD(float W, float H, AAstralisAssemblyGameMode* GM)
{
	if (!GM) return;
	AAstralisLaunchVehicleActor* V = GM->LaunchVehicle;
	UFont* SmFont = GEngine->GetSmallFont();
	UFont* BigFont = GEngine->GetLargeFont();

	// Mission Briefing — left panel (dynamic from mission selection)
	{
		const float PX = 20.0f, PY = 60.0f, PW = 340.0f, PH = 320.0f;
		DrawPanel(PX, PY, PW, PH, TEXT("MISSION BRIEFING"), FLinearColor(0.0f, 0.8f, 0.5f, 1.0f));

		float TY = PY + 30.0f;
		const float LX = PX + 14.0f;
		const float Step = 18.0f;

		auto DrawLine = [&](const FString& Str, const FLinearColor& Col = FLinearColor::White)
		{
			FCanvasTextItem TI(FVector2D(LX, TY), FText::FromString(Str), SmFont, Col);
			Canvas->DrawItem(TI);
			TY += Step;
		};

		const FMissionDefinition& Mission = GM->GetSelectedMission();
		DrawLine(FString::Printf(TEXT("MISSION: %s"), *Mission.Name),
			FLinearColor(0.9f, 0.95f, 1.0f, 1.0f));
		TY += 4.0f;
		DrawLine(Mission.Description, FLinearColor(0.7f, 0.8f, 0.9f));
		TY += 6.0f;
		DrawLine(FString::Printf(TEXT("Target Altitude: %.0f km"), Mission.TargetAltitude_km));
		DrawLine(FString::Printf(TEXT("Inclination:     %.1f deg"), Mission.TargetInclination_deg));
		DrawLine(FString::Printf(TEXT("Orbit Type:      %s"), *Mission.OrbitType));
		DrawLine(TEXT("Launch Site:     KSC, Florida"));
		if (GM->BusCatalog.Num() > 0)
		{
			const FSpacecraftBusDefinition& Bus = GM->GetSelectedBus();
			DrawLine(FString::Printf(TEXT("Spacecraft:      %s (%.0f kg)"),
				*Bus.Name, Bus.DryMass_kg + GM->GetInstrumentMass_kg()));
		}
		TY += 8.0f;
		DrawLine(TEXT("Objectives:"), FLinearColor(0.3f, 0.9f, 0.5f, 1.0f));
		DrawLine(FString::Printf(TEXT("  1. Achieve %.0f km orbit"), Mission.TargetAltitude_km));
		DrawLine(TEXT("  2. Deploy payload"));
		DrawLine(TEXT("  3. Confirm spacecraft operational"));
		TY += 10.0f;

		// Mission selector hint
		DrawLine(FString::Printf(TEXT("[M/N] Mission %d/%d"),
			GM->GetSelectedMissionIndex() + 1, GM->GetMissions().Num()),
			FLinearColor(0.6f, 0.9f, 0.3f, 0.9f));
	}

	// Delta-V Requirements Map — below mission briefing
	if (V)
	{
		const float PX = 20.0f, PY = 400.0f, PW = 340.0f, PH = 200.0f;
		DrawPanel(PX, PY, PW, PH, TEXT("DELTA-V MAP"), FLinearColor(0.9f, 0.5f, 0.1f, 1.0f));

		const double VehicleDV = V->GetTotalDeltaV_mps();
		float TY = PY + 28.0f;
		const float LX = PX + 14.0f;

		struct FDVReq { const TCHAR* Label; double DV; };
		const FDVReq Reqs[] = {
			{TEXT("LEO 300km"),   9300.0},
			{TEXT("LEO 400km"),   9400.0},
			{TEXT("SSO 550km"),   9700.0},
			{TEXT("Polar 800km"), 10200.0},
			{TEXT("MEO 20200km"), 13500.0},
			{TEXT("GTO 35786km"), 14000.0},
			{TEXT("Lunar TLI"),   16000.0},
		};

		for (const FDVReq& R : Reqs)
		{
			const bool bCanReach = VehicleDV >= R.DV;
			FLinearColor LblCol = bCanReach
				? FLinearColor(0.2f, 1.0f, 0.4f, 0.9f)
				: FLinearColor(1.0f, 0.3f, 0.2f, 0.7f);

			FCanvasTextItem LblI(FVector2D(LX, TY),
				FText::FromString(R.Label), SmFont, LblCol);
			LblI.Scale = FVector2D(0.9f, 0.9f);
			Canvas->DrawItem(LblI);

			FCanvasTextItem DVI(FVector2D(LX + 120.0f, TY),
				FText::FromString(FString::Printf(TEXT("%.0f m/s"), R.DV)), SmFont,
				FLinearColor(0.6f, 0.7f, 0.8f, 0.8f));
			DVI.Scale = FVector2D(0.9f, 0.9f);
			Canvas->DrawItem(DVI);

			// Mini bar
			const float BarX = LX + 210.0f;
			const float BarW = 100.0f;
			const float BarH = 8.0f;
			FCanvasTileItem BarBG(FVector2D(BarX, TY + 3.0f), FVector2D(BarW, BarH),
				FLinearColor(0.1f, 0.1f, 0.15f, 0.7f));
			BarBG.BlendMode = SE_BLEND_Translucent;
			Canvas->DrawItem(BarBG);

			const float Frac = static_cast<float>(FMath::Clamp(VehicleDV / R.DV, 0.0, 1.0));
			if (Frac > 0.0f)
			{
				FLinearColor BarCol = bCanReach
					? FLinearColor(0.1f, 0.8f, 0.3f, 0.8f)
					: FLinearColor(1.0f, 0.4f, 0.1f, 0.8f);
				FCanvasTileItem Fill(FVector2D(BarX, TY + 3.0f), FVector2D(BarW * Frac, BarH), BarCol);
				Fill.BlendMode = SE_BLEND_Translucent;
				Canvas->DrawItem(Fill);
			}

			TY += 17.0f;
		}

		// Vehicle DV summary
		TY += 4.0f;
		FLinearColor DVSumCol = (VehicleDV >= 9400.0) ? FLinearColor(0.2f, 1.0f, 0.3f, 1.0f)
			: FLinearColor(1.0f, 0.5f, 0.2f, 1.0f);
		FCanvasTextItem DVSum(FVector2D(LX, TY),
			FText::FromString(FString::Printf(TEXT("Vehicle DV: %.0f m/s"), VehicleDV)),
			SmFont, DVSumCol);
		Canvas->DrawItem(DVSum);
	}

	// Weather & Launch Conditions — below Delta-V map
	{
		const float PX = 20.0f, PY = 620.0f, PW = 340.0f, PH = 170.0f;
		DrawPanel(PX, PY, PW, PH, TEXT("LAUNCH CONDITIONS"), FLinearColor(0.2f, 0.6f, 0.9f, 1.0f));

		float TY = PY + 28.0f;
		const float LX = PX + 14.0f;

		auto DrawWLine = [&](const FString& Str, const FLinearColor& Col = FLinearColor::White)
		{
			FCanvasTextItem TI(FVector2D(LX, TY), FText::FromString(Str), SmFont, Col);
			Canvas->DrawItem(TI);
			TY += 17.0f;
		};

		DrawWLine(FString::Printf(TEXT("Weather:  %s"), *GM->WeatherCondition),
			FLinearColor(0.8f, 0.9f, 1.0f));
		DrawWLine(FString::Printf(TEXT("Wind:     %.0f kts @ %.0f deg"),
			GM->WindSpeed_kts, GM->WindDirection_deg),
			GM->WindSpeed_kts > 25.0 ? FLinearColor(1.0f, 0.5f, 0.2f) : FLinearColor(0.7f, 0.8f, 0.9f));
		DrawWLine(FString::Printf(TEXT("Temp:     %.0f F (%.0f C)"),
			GM->Temperature_F, (GM->Temperature_F - 32.0) * 5.0 / 9.0));
		DrawWLine(FString::Printf(TEXT("Visibility: %.0f km"), GM->Visibility_km));
		DrawWLine(FString::Printf(TEXT("Cloud Cover: %.0f%%"), GM->CloudCover_pct));

		FString GOStr = GM->bLaunchGO ? TEXT("GO FOR LAUNCH") : TEXT("NO-GO  —  SCRUBBED");
		FLinearColor GOCol = GM->bLaunchGO
			? FLinearColor(0.1f, 1.0f, 0.3f, 1.0f) : FLinearColor(1.0f, 0.2f, 0.1f, 1.0f);
		TY += 4.0f;
		FCanvasTextItem GOI(FVector2D(LX, TY), FText::FromString(GOStr), SmFont, GOCol);
		GOI.Scale = FVector2D(1.2f, 1.2f);
		Canvas->DrawItem(GOI);
	}

	// ── SPACECRAFT BUS — center-left panel ───────────────────────────────
	{
		const float PX = 380.0f, PY = 60.0f, PW = 300.0f, PH = 220.0f;
		DrawPanel(PX, PY, PW, PH, TEXT("SPACECRAFT BUS"), FLinearColor(0.7f, 0.4f, 1.0f, 1.0f));

		float TY = PY + 30.0f;
		const float LX = PX + 14.0f;
		const float Step = 16.0f;

		auto DrawLine2 = [&](const FString& Str, const FLinearColor& Col = FLinearColor::White)
		{
			FCanvasTextItem TI(FVector2D(LX, TY), FText::FromString(Str), SmFont, Col);
			TI.Scale = FVector2D(0.9f, 0.9f);
			Canvas->DrawItem(TI);
			TY += Step;
		};

		if (GM->BusCatalog.Num() > 0)
		{
			const FSpacecraftBusDefinition& Bus = GM->GetSelectedBus();
			DrawLine2(FString::Printf(TEXT("%s"), *Bus.Name), FLinearColor(0.85f, 0.7f, 1.0f));
			DrawLine2(Bus.Description, FLinearColor(0.6f, 0.6f, 0.8f));
			TY += 4.0f;
			DrawLine2(FString::Printf(TEXT("Dry Mass:     %.0f kg"), Bus.DryMass_kg));
			DrawLine2(FString::Printf(TEXT("Max Payload:  %.0f kg"), Bus.MaxPayloadMass_kg));
			DrawLine2(FString::Printf(TEXT("Power:        %.0f W (%.1f m2 solar)"), Bus.PowerGeneration_W, Bus.SolarPanelArea_m2));
			DrawLine2(FString::Printf(TEXT("Battery:      %.0f Wh"), Bus.BatteryCapacity_Wh));
			DrawLine2(FString::Printf(TEXT("Data Rate:    %.0f Mbps | %.0f GB"), Bus.MaxDataRate_Mbps, Bus.DataStorage_GB));
			DrawLine2(FString::Printf(TEXT("Pointing:     %.3f deg ADCS"), Bus.ADCS_PointingAccuracy_deg));
			DrawLine2(FString::Printf(TEXT("Slots:        %d instruments"), Bus.MaxInstrumentSlots));
			DrawLine2(FString::Printf(TEXT("Cost:         $%.0fM"), Bus.Cost_M), FLinearColor(0.3f, 1.0f, 0.5f));
			TY += 4.0f;
			DrawLine2(FString::Printf(TEXT("[P/Q] Bus %d/%d"), GM->SelectedBusIndex + 1, GM->BusCatalog.Num()),
				FLinearColor(0.7f, 0.5f, 1.0f, 0.9f));
		}
	}

	// ── INSTRUMENT SELECTION — center-left panel below bus ────────────────
	{
		const float PX = 380.0f, PY = 300.0f, PW = 300.0f;
		const float InstrCount = static_cast<float>(GM->InstrumentCatalog.Num());
		const float PH = 50.0f + InstrCount * 16.0f;
		DrawPanel(PX, PY, PW, PH, TEXT("INSTRUMENTS"), FLinearColor(1.0f, 0.7f, 0.2f, 1.0f));

		float TY = PY + 30.0f;
		const float LX = PX + 14.0f;

		const int32 MaxSlots = (GM->BusCatalog.Num() > 0) ? GM->GetSelectedBus().MaxInstrumentSlots : 4;
		const int32 Used = GM->GetSelectedInstrumentCount();

		FCanvasTextItem SlotI(FVector2D(LX, TY),
			FText::FromString(FString::Printf(TEXT("Slots: %d/%d"), Used, MaxSlots)), SmFont,
			(Used >= MaxSlots) ? FLinearColor(1.0f, 0.5f, 0.2f) : FLinearColor(0.5f, 0.8f, 1.0f));
		SlotI.Scale = FVector2D(0.85f, 0.85f);
		Canvas->DrawItem(SlotI);
		TY += 18.0f;

		const double TargetAlt = GM->GetSelectedMission().TargetAltitude_km;

		for (int32 i = 0; i < GM->InstrumentCatalog.Num(); ++i)
		{
			const FInstrumentDefinition& Inst = GM->InstrumentCatalog[i];
			bool bSel = GM->InstrumentSelected[i];
			FString Key = (i < 4) ? FString::Printf(TEXT("F%d"), i + 1)
				: FString::Printf(TEXT("F%d"), i + 2); // skip F5

			bool bAltOK = (TargetAlt >= Inst.MinAltitude_km && TargetAlt <= Inst.MaxAltitude_km);
			FString AltWarn = bAltOK ? TEXT("") : TEXT(" !");

			FLinearColor Col;
			if (bSel && !bAltOK) Col = FLinearColor(1.0f, 0.5f, 0.2f);
			else if (bSel) Col = FLinearColor(0.3f, 1.0f, 0.5f);
			else Col = FLinearColor(0.5f, 0.5f, 0.6f);

			FString Marker = bSel ? TEXT("[X]") : TEXT("[ ]");

			FCanvasTextItem II(FVector2D(LX, TY),
				FText::FromString(FString::Printf(TEXT("%s %s %s  %.0fkg %.0fW $%.0fM%s"),
					*Key, *Marker, *Inst.Name, Inst.Mass_kg, Inst.Power_W, Inst.Cost_M, *AltWarn)),
				SmFont, Col);
			II.Scale = FVector2D(0.8f, 0.8f);
			Canvas->DrawItem(II);
			TY += 16.0f;
		}

		// Altitude compatibility note
		{
			int32 BadCount = 0;
			for (int32 i = 0; i < GM->InstrumentCatalog.Num(); ++i)
			{
				if (!GM->InstrumentSelected[i]) continue;
				const FInstrumentDefinition& Inst = GM->InstrumentCatalog[i];
				if (TargetAlt < Inst.MinAltitude_km || TargetAlt > Inst.MaxAltitude_km) BadCount++;
			}
			if (BadCount > 0)
			{
				FCanvasTextItem Warn(FVector2D(LX, TY),
					FText::FromString(FString::Printf(TEXT("! %d instrument(s) outside target alt range"), BadCount)),
					SmFont, FLinearColor(1.0f, 0.4f, 0.1f));
				Warn.Scale = FVector2D(0.8f, 0.8f);
				Canvas->DrawItem(Warn);
				TY += 16.0f;
			}
		}
	}

	// ── BUDGET — center panel ─────────────────────────────────────────────
	{
		const float PX = 380.0f, PY = H - 250.0f, PW = 300.0f, PH = 180.0f;
		DrawPanel(PX, PY, PW, PH, TEXT("MISSION BUDGET"), FLinearColor(0.2f, 1.0f, 0.6f, 1.0f));

		float TY = PY + 30.0f;
		const float LX = PX + 14.0f;
		const float Step = 17.0f;

		auto BudgetLine = [&](const FString& Label, double Val, const FLinearColor& Col = FLinearColor::White)
		{
			FCanvasTextItem LI(FVector2D(LX, TY), FText::FromString(Label), SmFont, FLinearColor(0.6f, 0.7f, 0.8f));
			LI.Scale = FVector2D(0.9f, 0.9f);
			Canvas->DrawItem(LI);
			FCanvasTextItem VI(FVector2D(LX + 160.0f, TY),
				FText::FromString(FString::Printf(TEXT("$%.1fM"), Val)), SmFont, Col);
			VI.Scale = FVector2D(0.9f, 0.9f);
			Canvas->DrawItem(VI);
			TY += Step;
		};

		const FMissionBudget& B = GM->Budget;
		BudgetLine(TEXT("Launch Vehicle:"), B.LaunchCost_M);
		BudgetLine(TEXT("Spacecraft Bus:"), B.BusCost_M);
		BudgetLine(TEXT("Instruments:"), B.InstrumentCost_M);
		BudgetLine(TEXT("Operations:"), B.OperationsCost_M);
		BudgetLine(TEXT("Insurance (10%):"), B.InsuranceCost_M);

		TY += 4.0f;
		const double Spent = B.GetTotalSpent();
		const double Remaining = B.GetRemaining();
		FLinearColor TotalCol = B.IsOverBudget()
			? FLinearColor(1.0f, 0.2f, 0.1f) : FLinearColor(0.3f, 1.0f, 0.5f);
		BudgetLine(TEXT("TOTAL SPENT:"), Spent, TotalCol);
		BudgetLine(TEXT("REMAINING:"), Remaining, TotalCol);

		// Budget bar
		const float BarX = LX, BarW = PW - 28.0f, BarH = 10.0f;
		FCanvasTileItem BarBG(FVector2D(BarX, TY), FVector2D(BarW, BarH),
			FLinearColor(0.1f, 0.1f, 0.15f, 0.7f));
		BarBG.BlendMode = SE_BLEND_Translucent;
		Canvas->DrawItem(BarBG);
		float Frac = static_cast<float>(FMath::Clamp(Spent / B.TotalBudget_M, 0.0, 1.0));
		FLinearColor BarCol = (Frac > 0.9f) ? FLinearColor(1.0f, 0.2f, 0.1f)
			: (Frac > 0.7f) ? FLinearColor(1.0f, 0.8f, 0.2f)
			: FLinearColor(0.2f, 0.8f, 0.4f);
		FCanvasTileItem BarFill(FVector2D(BarX, TY), FVector2D(BarW * Frac, BarH), BarCol);
		BarFill.BlendMode = SE_BLEND_Translucent;
		Canvas->DrawItem(BarFill);
	}

	// Vehicle Overview — right panel
	if (V)
	{
		const float PX = W - 340.0f, PY = 60.0f, PW = 320.0f, PH = 430.0f;
		DrawPanel(PX, PY, PW, PH, TEXT("VEHICLE OVERVIEW"), FLinearColor(0.0f, 0.6f, 1.0f, 1.0f));

		float TY = PY + 30.0f;
		const float LX = PX + 14.0f;
		const float Step = 18.0f;

		auto DrawLine = [&](const FString& Str, const FLinearColor& Col = FLinearColor::White)
		{
			FCanvasTextItem TI(FVector2D(LX, TY), FText::FromString(Str), SmFont, Col);
			Canvas->DrawItem(TI);
			TY += Step;
		};

		const FLaunchVehicleDefinition& VehDef = GM->GetSelectedVehicle();
		DrawLine(FString::Printf(TEXT("%s — %s"),
			*VehDef.DisplayName.ToString(), *VehDef.Manufacturer),
			FLinearColor(0.9f, 0.9f, 1.0f, 1.0f));
		TY += 6.0f;

		DrawLine(FString::Printf(TEXT("Total Mass:    %.0f kg (%.1f t)"),
			V->GetTotalMass_kg(), V->GetTotalMass_kg() / 1000.0));

		const double DV = V->GetTotalDeltaV_mps();
		FLinearColor DVColor = (DV >= 9400.0) ? FLinearColor(0.2f, 1.0f, 0.3f, 1.0f)
			: (DV >= 7000.0) ? FLinearColor(1.0f, 0.9f, 0.2f, 1.0f)
			: FLinearColor(1.0f, 0.3f, 0.2f, 1.0f);
		DrawLine(FString::Printf(TEXT("Delta-V:       %.0f m/s"), DV), DVColor);

		const double LiftThr = (V->Stages.Num() > 0 && V->Stages[0])
			? V->Stages[0]->Spec.Thrust_N : 0.0;
		const double PadTWR = (V->GetTotalMass_kg() > 0)
			? LiftThr / (V->GetTotalMass_kg() * 9.80665) : 0.0;
		FLinearColor TWRColor = (PadTWR >= 1.2) ? FLinearColor(0.2f, 1.0f, 0.3f, 1.0f)
			: (PadTWR >= 1.0) ? FLinearColor(1.0f, 0.9f, 0.2f, 1.0f)
			: FLinearColor(1.0f, 0.3f, 0.2f, 1.0f);
		DrawLine(FString::Printf(TEXT("Liftoff TWR:   %.2f"), PadTWR), TWRColor);

		DrawLine(FString::Printf(TEXT("Stages:        %d"), V->Stages.Num()));

		if (VehDef.PayloadToLEO_kg > 0)
			DrawLine(FString::Printf(TEXT("To LEO:        %.0f kg"), VehDef.PayloadToLEO_kg));
		if (VehDef.PayloadToGTO_kg > 0)
			DrawLine(FString::Printf(TEXT("To GTO:        %.0f kg"), VehDef.PayloadToGTO_kg));
		DrawLine(FString::Printf(TEXT("Country:       %s"), *VehDef.Country),
			FLinearColor(0.6f, 0.7f, 0.9f, 0.8f));
		if (!VehDef.FirstFlight.IsEmpty())
			DrawLine(FString::Printf(TEXT("First Flight:  %s"), *VehDef.FirstFlight),
				FLinearColor(0.6f, 0.7f, 0.8f, 0.7f));
		// Recovery indicator
		if (VehDef.Stages.Num() > 0 && VehDef.Stages[0].bHasRecoverySystem)
			DrawLine(TEXT("Recovery:      REUSABLE S1"), FLinearColor(0.3f, 1.0f, 0.5f, 0.9f));
		TY += 6.0f;

		for (int32 i = 0; i < V->Stages.Num(); ++i)
		{
			if (!V->Stages[i]) continue;
			const FStageSpec& S = V->Stages[i]->Spec;
			DrawLine(FString::Printf(TEXT("S%d: %dx %s"), i + 1, S.NumEngines,
				*S.EngineName.ToString()), FLinearColor(0.7f, 0.8f, 1.0f, 1.0f));
			DrawLine(FString::Printf(TEXT("    %.0f kN SL / %.0f kN Vac | %.0f/%.0f s"),
				S.Thrust_N / 1000.0, S.ThrustVacuum_N / 1000.0, S.Isp_s, S.IspVacuum_s));
			DrawLine(FString::Printf(TEXT("    Prop: %.0f kg | Dry: %.0f kg"),
				S.PropellantCapacity_kg, S.DryMass_kg));
		}
	}

	// Launch Readiness Checklist — right panel below Vehicle Overview
	if (V)
	{
		const float PX = W - 340.0f, PY = 510.0f, PW = 320.0f, PH = 220.0f;
		DrawPanel(PX, PY, PW, PH, TEXT("LAUNCH CHECKLIST"), FLinearColor(0.2f, 0.9f, 0.3f, 1.0f));

		float TY = PY + 28.0f;
		const float LX = PX + 14.0f;

		const double DV = V->GetTotalDeltaV_mps();
		const FMissionDefinition& Mission = GM->GetSelectedMission();
		double RequiredDV = Mission.TargetAltitude_km < 600.0 ? 9400.0
			: Mission.TargetAltitude_km < 2000.0 ? 10200.0
			: Mission.TargetAltitude_km < 36000.0 ? 14000.0 : 16000.0;

		const double LiftThr = (V->Stages.Num() > 0 && V->Stages[0])
			? V->Stages[0]->Spec.Thrust_N : 0.0;
		const double PadTWR = (V->GetTotalMass_kg() > 0)
			? LiftThr / (V->GetTotalMass_kg() * 9.80665) : 0.0;

		struct FCheck { FString Label; bool bPass; };
		TArray<FCheck> Checks;
		const bool bHasInstruments = GM->GetSelectedInstrumentCount() > 0;
		const bool bMassOK = GM->BusCatalog.Num() > 0 &&
			GM->GetInstrumentMass_kg() <= GM->GetSelectedBus().MaxPayloadMass_kg;
		const bool bPowerOK = GM->BusCatalog.Num() > 0 &&
			(80.0 + GM->GetInstrumentPower_W()) <= GM->GetSelectedBus().PowerGeneration_W;
		const bool bBudgetOK = !GM->Budget.IsOverBudget();

		Checks.Add({TEXT("Vehicle Selected"), true});
		Checks.Add({FString::Printf(TEXT("DV >= %.0f m/s"), RequiredDV), DV >= RequiredDV});
		Checks.Add({TEXT("TWR > 1.0"), PadTWR > 1.0});
		Checks.Add({TEXT("Stages Valid"), V->Stages.Num() >= 2});
		Checks.Add({TEXT("Instruments Selected"), bHasInstruments});
		Checks.Add({TEXT("Mass Budget OK"), bMassOK});
		Checks.Add({TEXT("Power Budget OK"), bPowerOK});
		Checks.Add({TEXT("Mission Budget OK"), bBudgetOK});
		Checks.Add({TEXT("Weather GO"), GM->bLaunchGO});
		Checks.Add({TEXT("Propellant Loaded"), true});

		bool bAllGo = true;
		for (const FCheck& C : Checks)
		{
			FString Status = C.bPass ? TEXT("[GO]  ") : TEXT("[HOLD]");
			FLinearColor Col = C.bPass
				? FLinearColor(0.2f, 1.0f, 0.3f, 0.9f)
				: FLinearColor(1.0f, 0.3f, 0.2f, 0.9f);
			FCanvasTextItem CI(FVector2D(LX, TY),
				FText::FromString(FString::Printf(TEXT("%s %s"), *Status, *C.Label)),
				SmFont, Col);
			Canvas->DrawItem(CI);
			TY += 18.0f;
			if (!C.bPass) bAllGo = false;
		}

		TY += 6.0f;
		FString SummaryStr = bAllGo ? TEXT("ALL STATIONS GO") : TEXT("HOLD — CHECK ITEMS");
		FLinearColor SumCol = bAllGo ? FLinearColor(0.1f, 1.0f, 0.3f, 1.0f)
			: FLinearColor(1.0f, 0.4f, 0.1f, 1.0f);
		FCanvasTextItem SumI(FVector2D(LX, TY), FText::FromString(SummaryStr), SmFont, SumCol);
		SumI.Scale = FVector2D(1.1f, 1.1f);
		Canvas->DrawItem(SumI);
	}

	// Engine swap overlay — center panel
	if (V && V->bSwapMode)
	{
		const float PW = 420.0f;
		const float PX = (W - PW) / 2.0f;
		const float PY = 60.0f;

		const auto& Catalog = V->GetSwapCatalog();
		const float PH = 80.0f + Catalog.Num() * 22.0f;

		DrawPanel(PX, PY, PW, PH, TEXT("ENGINE SWAP"), FLinearColor(0.9f, 0.6f, 0.0f, 1.0f));

		float TY = PY + 30.0f;
		const float LX = PX + 14.0f;

		FCanvasTextItem StageLabel(FVector2D(LX, TY),
			FText::FromString(FString::Printf(TEXT("Editing Stage %d  [Tab] Switch  [E] Close"),
				V->SwapStageIndex + 1)),
			SmFont, FLinearColor(0.9f, 0.7f, 0.2f, 1.0f));
		Canvas->DrawItem(StageLabel);
		TY += 24.0f;

		for (int32 i = 0; i < Catalog.Num(); ++i)
		{
			const FEngineSpec_Vehicle& E = Catalog[i];
			FString Line = FString::Printf(TEXT("[%d] %s — %.0f/%.0f kN  %.0f/%.0f s  %.0f kg"),
				i + 1, *E.Name.ToString(),
				E.ThrustSeaLevel_N / 1000.0, E.ThrustVacuum_N / 1000.0,
				E.IspSeaLevel_s, E.IspVacuum_s, E.Mass_kg);
			FCanvasTextItem EI(FVector2D(LX, TY), FText::FromString(Line), SmFont, FLinearColor::White);
			Canvas->DrawItem(EI);
			TY += 20.0f;
		}
	}

	// Flight History / Records
	if (GM->TotalLaunches > 0)
	{
		const int32 NumShow = FMath::Min(GM->FlightHistory.Num(), 5);
		const float PX = W - 340.0f, PW = 320.0f;
		const float PH = 110.0f + NumShow * 18.0f;
		const float PY = H - 60.0f - PH;
		DrawPanel(PX, PY, PW, PH, TEXT("FLIGHT RECORDS"), FLinearColor(0.9f, 0.7f, 0.1f, 1.0f));

		float TY = PY + 28.0f;
		const float LX = PX + 14.0f;

		auto DrawRecLine = [&](const FString& Str, const FLinearColor& Col = FLinearColor::White)
		{
			FCanvasTextItem TI(FVector2D(LX, TY), FText::FromString(Str), SmFont, Col);
			Canvas->DrawItem(TI);
			TY += 18.0f;
		};

		DrawRecLine(FString::Printf(TEXT("Launches: %d  |  Success: %d/%d (%.0f%%)"),
			GM->TotalLaunches, GM->SuccessfulMissions, GM->TotalLaunches,
			(double)GM->SuccessfulMissions / GM->TotalLaunches * 100.0),
			FLinearColor(0.8f, 0.9f, 1.0f));
		DrawRecLine(FString::Printf(TEXT("Best: %s (%.0f/100)"), *GM->BestGrade, GM->BestScore),
			FLinearColor(1.0f, 0.85f, 0.0f, 1.0f));
		TY += 4.0f;

		const int32 Start = FMath::Max(0, GM->FlightHistory.Num() - 5);
		for (int32 i = GM->FlightHistory.Num() - 1; i >= Start; --i)
		{
			const auto& Rec = GM->FlightHistory[i];
			FLinearColor RecCol = Rec.bSuccess
				? FLinearColor(0.2f, 0.9f, 0.3f, 0.9f)
				: FLinearColor(1.0f, 0.4f, 0.2f, 0.9f);
			DrawRecLine(FString::Printf(TEXT("%s %s | %s | %.0f"),
				Rec.bSuccess ? TEXT("OK") : TEXT("FAIL"),
				*Rec.VehicleName, *Rec.MissionName, Rec.Score), RecCol);
		}
	}

	// Controls hint — bottom center
	{
		UFont* Fn = SmFont;
		FString Controls = V && V->bSwapMode
			? TEXT("[1-8] Select Engine  [Tab] Switch Stage  [E] Close Swap")
			: TEXT("[F5] Launch  [V/B] Vehicle  [M/N] Mission  [P/Q] Bus  [F1-F4,F6-F11] Instruments  [E] Engines");

		// Vehicle selector label
		FString VehLabel = FString::Printf(TEXT("Vehicle %d/%d: %s"),
			GM->GetSelectedVehicleIndex() + 1, GM->GetVehicleLibrary().Num(),
			*GM->GetSelectedVehicle().DisplayName.ToString());
		float VW, VH;
		Canvas->TextSize(Fn, VehLabel, VW, VH);
		FCanvasTextItem VI(FVector2D((W - VW) / 2.0f, H - 70.0f),
			FText::FromString(VehLabel), Fn, FLinearColor(0.3f, 0.8f, 1.0f, 0.9f));
		Canvas->DrawItem(VI);

		float CW, CH;
		Canvas->TextSize(Fn, Controls, CW, CH);
		FCanvasTextItem CI(FVector2D((W - CW) / 2.0f, H - 52.0f),
			FText::FromString(Controls), Fn, FLinearColor(0.6f, 0.8f, 0.4f, 0.9f));
		Canvas->DrawItem(CI);
	}
}

// ─────────────────────────────────────────────────────────────────────────────
// LAUNCH PHASE HUD
// ─────────────────────────────────────────────────────────────────────────────
void AAstralisAssemblyHUD::DrawLaunchHUD(float W, float H, AAstralisAssemblyGameMode* GM)
{
	if (!GM || !GM->LaunchVehicle) return;
	AAstralisLaunchVehicleActor* V = GM->LaunchVehicle;
	UFont* BigFont = GEngine->GetLargeFont();

	// ── CINEMATIC VIGNETTE — subtle darkening at edges (AAA standard) ─────
	{
		const float VigW = W * 0.08f;
		const float VigAlpha = 0.25f;
		FCanvasTileItem VL(FVector2D(0, 0), FVector2D(VigW, H), FLinearColor(0.0f, 0.0f, 0.0f, VigAlpha));
		VL.BlendMode = SE_BLEND_Translucent; Canvas->DrawItem(VL);
		FCanvasTileItem VR(FVector2D(W - VigW, 0), FVector2D(VigW, H), FLinearColor(0.0f, 0.0f, 0.0f, VigAlpha));
		VR.BlendMode = SE_BLEND_Translucent; Canvas->DrawItem(VR);
		FCanvasTileItem VT(FVector2D(0, 0), FVector2D(W, VigW * 0.6f), FLinearColor(0.0f, 0.0f, 0.0f, VigAlpha * 0.7f));
		VT.BlendMode = SE_BLEND_Translucent; Canvas->DrawItem(VT);
		FCanvasTileItem VB(FVector2D(0, H - VigW * 0.6f), FVector2D(W, VigW * 0.6f), FLinearColor(0.0f, 0.0f, 0.0f, VigAlpha * 0.7f));
		VB.BlendMode = SE_BLEND_Translucent; Canvas->DrawItem(VB);
	}

	// ── SUBTLE HUD SCANLINES — barely visible, gives broadcast/NASA TV feel ──
	{
		const float ScanAlpha = 0.04f;
		for (float SY = 0.0f; SY < H; SY += 4.0f)
		{
			FCanvasTileItem ScanLine(FVector2D(0, SY), FVector2D(W, 1.0f),
				FLinearColor(0.0f, 0.0f, 0.0f, ScanAlpha));
			ScanLine.BlendMode = SE_BLEND_Translucent;
			Canvas->DrawItem(ScanLine);
		}
	}

	// Space darkening — sky goes black as altitude increases
	const double Alt_km_atm = V->Altitude_m / 1000.0;
	{
		const float SpaceDark = static_cast<float>(FMath::Clamp(Alt_km_atm / 100.0, 0.0, 0.7));
		if (SpaceDark > 0.01f)
		{
			FCanvasTileItem DarkSky(FVector2D(0, 0), FVector2D(W, H),
				FLinearColor(0.0f, 0.0f, 0.02f, SpaceDark));
			DarkSky.BlendMode = SE_BLEND_Translucent;
			Canvas->DrawItem(DarkSky);
		}

		// ── EARTH'S ATMOSPHERIC LIMB — glowing blue-white band at the horizon ──
		if (Alt_km_atm > 30.0)
		{
			const float LimbIntensity = FMath::Clamp(static_cast<float>((Alt_km_atm - 30.0) / 80.0), 0.0f, 1.0f);
			const float HorizonY = H * FMath::Clamp(0.75f - static_cast<float>(Alt_km_atm) * 0.0005f, 0.5f, 0.75f);
			const float BandH = 40.0f + LimbIntensity * 30.0f;

			// Outer atmosphere glow (deep blue)
			FCanvasTileItem AtmOuter(FVector2D(0, HorizonY - BandH * 1.5f), FVector2D(W, BandH * 2.0f),
				FLinearColor(0.05f, 0.15f, 0.5f, LimbIntensity * 0.3f));
			AtmOuter.BlendMode = SE_BLEND_Translucent;
			Canvas->DrawItem(AtmOuter);

			// Inner atmosphere (bright cyan-white)
			FCanvasTileItem AtmInner(FVector2D(0, HorizonY - BandH * 0.5f), FVector2D(W, BandH),
				FLinearColor(0.3f, 0.6f, 1.0f, LimbIntensity * 0.25f));
			AtmInner.BlendMode = SE_BLEND_Translucent;
			Canvas->DrawItem(AtmInner);

			// Bright limb line
			FCanvasTileItem LimbLine(FVector2D(0, HorizonY), FVector2D(W, 3.0f),
				FLinearColor(0.5f, 0.8f, 1.0f, LimbIntensity * 0.5f));
			LimbLine.BlendMode = SE_BLEND_Translucent;
			Canvas->DrawItem(LimbLine);

			// Earth surface below (dark blue-green)
			FCanvasTileItem EarthBelow(FVector2D(0, HorizonY + 3.0f), FVector2D(W, H - HorizonY),
				FLinearColor(0.02f, 0.06f, 0.12f, LimbIntensity * 0.4f));
			EarthBelow.BlendMode = SE_BLEND_Translucent;
			Canvas->DrawItem(EarthBelow);
		}

		// ── STARS — 300 with color variety (blue, white, yellow, red giants) ──
		if (SpaceDark > 0.12f)
		{
			const float StarAlpha = FMath::Clamp((SpaceDark - 0.12f) / 0.35f, 0.0f, 1.0f);
			const int32 NumStars = static_cast<int32>(StarAlpha * 300.0f);
			uint32 Seed = 42;
			for (int32 i = 0; i < NumStars; ++i)
			{
				Seed = Seed * 1103515245 + 12345;
				const float SX = static_cast<float>((Seed >> 16) % static_cast<uint32>(W));
				Seed = Seed * 1103515245 + 12345;
				const float SY = static_cast<float>((Seed >> 16) % static_cast<uint32>(H * 0.65f));
				Seed = Seed * 1103515245 + 12345;
				const float Brightness = 0.3f + 0.7f * static_cast<float>((Seed >> 16) % 100) / 100.0f;
				Seed = Seed * 1103515245 + 12345;
				const int32 StarType = (Seed >> 16) % 20;

				float R, G, B;
				if (StarType < 10) { R = Brightness; G = Brightness; B = Brightness; }          // White
				else if (StarType < 14) { R = Brightness * 0.7f; G = Brightness * 0.85f; B = Brightness; } // Blue-white
				else if (StarType < 17) { R = Brightness; G = Brightness * 0.9f; B = Brightness * 0.6f; }  // Yellow
				else if (StarType < 19) { R = Brightness; G = Brightness * 0.6f; B = Brightness * 0.4f; }  // Orange
				else { R = Brightness; G = Brightness * 0.3f; B = Brightness * 0.2f; }                      // Red giant

				const float Size = (Brightness > 0.85f) ? 2.5f : (Brightness > 0.6f ? 2.0f : 1.5f);

				// Bright stars get a subtle glow
				if (Brightness > 0.8f && StarAlpha > 0.5f)
				{
					FCanvasTileItem Glow(FVector2D(SX - 2.0f, SY - 2.0f), FVector2D(Size + 4.0f, Size + 4.0f),
						FLinearColor(R, G, B, StarAlpha * 0.15f));
					Glow.BlendMode = SE_BLEND_Translucent;
					Canvas->DrawItem(Glow);
				}

				FCanvasTileItem Star(FVector2D(SX, SY), FVector2D(Size, Size),
					FLinearColor(R, G, B, StarAlpha * Brightness));
				Star.BlendMode = SE_BLEND_Translucent;
				Canvas->DrawItem(Star);
			}
		}
	}

	// ── SUN GLINT — bright spot in upper corner during daytime ascent ─────
	if (Alt_km_atm < 200.0 && Alt_km_atm > 5.0)
	{
		const float SunX = W * 0.85f;
		const float SunY = H * 0.08f;
		const float SunAlpha = FMath::Clamp(1.0f - static_cast<float>(Alt_km_atm) / 200.0f, 0.0f, 0.6f);

		// Outer glow
		const float GlowR = 80.0f;
		FCanvasTileItem SunGlow(FVector2D(SunX - GlowR, SunY - GlowR), FVector2D(GlowR * 2.0f, GlowR * 2.0f),
			FLinearColor(1.0f, 0.95f, 0.8f, SunAlpha * 0.15f));
		SunGlow.BlendMode = SE_BLEND_Translucent;
		Canvas->DrawItem(SunGlow);

		// Inner glow
		const float InnerR = 30.0f;
		FCanvasTileItem SunInner(FVector2D(SunX - InnerR, SunY - InnerR), FVector2D(InnerR * 2.0f, InnerR * 2.0f),
			FLinearColor(1.0f, 0.98f, 0.9f, SunAlpha * 0.3f));
		SunInner.BlendMode = SE_BLEND_Translucent;
		Canvas->DrawItem(SunInner);

		// Core
		FCanvasTileItem SunCore(FVector2D(SunX - 6.0f, SunY - 6.0f), FVector2D(12.0f, 12.0f),
			FLinearColor(1.0f, 1.0f, 0.95f, SunAlpha * 0.7f));
		SunCore.BlendMode = SE_BLEND_Translucent;
		Canvas->DrawItem(SunCore);

		// Lens flare streak
		FCanvasTileItem Streak(FVector2D(SunX - 60.0f, SunY - 1.0f), FVector2D(120.0f, 2.0f),
			FLinearColor(1.0f, 0.9f, 0.7f, SunAlpha * 0.2f));
		Streak.BlendMode = SE_BLEND_Translucent;
		Canvas->DrawItem(Streak);
	}

	// ── TRANSONIC VAPOR CONE — visible when Mach ~0.95-1.1 ───────────────
	if (V->GetMachNumber() > 0.92 && V->GetMachNumber() < 1.15 && Alt_km_atm < 20.0)
	{
		const float VaporIntensity = 1.0f - FMath::Abs(static_cast<float>(V->GetMachNumber()) - 1.02f) / 0.12f;
		const float VA = FMath::Clamp(VaporIntensity, 0.0f, 1.0f) * 0.35f;
		// White cone around the center of screen (where rocket is)
		const float CX = W * 0.5f, CY = H * 0.4f;
		const float ConeW = 200.0f, ConeH = 80.0f;
		FCanvasTileItem VaporL(FVector2D(CX - ConeW, CY - ConeH * 0.5f), FVector2D(ConeW, ConeH),
			FLinearColor(1.0f, 1.0f, 1.0f, VA * 0.4f));
		VaporL.BlendMode = SE_BLEND_Translucent;
		Canvas->DrawItem(VaporL);
		FCanvasTileItem VaporR(FVector2D(CX, CY - ConeH * 0.5f), FVector2D(ConeW, ConeH),
			FLinearColor(1.0f, 1.0f, 1.0f, VA * 0.4f));
		VaporR.BlendMode = SE_BLEND_Translucent;
		Canvas->DrawItem(VaporR);
		// Inner bright core
		FCanvasTileItem VaporCore(FVector2D(CX - 30.0f, CY - 15.0f), FVector2D(60.0f, 30.0f),
			FLinearColor(1.0f, 1.0f, 1.0f, VA * 0.6f));
		VaporCore.BlendMode = SE_BLEND_Translucent;
		Canvas->DrawItem(VaporCore);
	}

	// ── HEAT SHIMMER — wavering distortion effect near engines at low altitude ──
	if (Alt_km_atm < 5.0 && V->MissionState != ELaunchVehicleState::Countdown &&
		V->MissionState != ELaunchVehicleState::OnPad && V->MissionState != ELaunchVehicleState::Complete)
	{
		const float T = GetWorld()->GetTimeSeconds();
		const float ShimmerAlpha = FMath::Clamp(1.0f - static_cast<float>(Alt_km_atm) / 5.0f, 0.0f, 1.0f) * 0.08f;
		// Wavy heat distortion bands at the bottom of the screen
		for (int32 i = 0; i < 6; ++i)
		{
			const float BandY = H * 0.7f + static_cast<float>(i) * 20.0f + 10.0f * FMath::Sin(T * 3.0f + i * 1.7f);
			const float BandW = W * (0.3f + 0.1f * FMath::Sin(T * 2.0f + i * 0.9f));
			const float BandX = (W - BandW) * 0.5f + 30.0f * FMath::Sin(T * 1.5f + i * 2.3f);
			FCanvasTileItem Shimmer(FVector2D(BandX, BandY), FVector2D(BandW, 3.0f),
				FLinearColor(1.0f, 0.95f, 0.85f, ShimmerAlpha));
			Shimmer.BlendMode = SE_BLEND_Translucent;
			Canvas->DrawItem(Shimmer);
		}
	}

	UFont* SmFont = GEngine->GetSmallFont();

	// ── BIG MET TIMER ─────────────────────────────────────────────────────
	{
		FString METStr;
		FLinearColor METColor;
		if (V->MissionState == ELaunchVehicleState::Countdown)
		{
			const double T_remain = FMath::CeilToDouble(V->CountdownTimer_s);
			METStr = FString::Printf(TEXT("T - %.0f"), T_remain);
			METColor = (T_remain <= 3.0) ? FLinearColor(1.0f, 0.6f, 0.0f, 1.0f)
				: FLinearColor(1.0f, 0.3f, 0.1f, 1.0f);
		}
		else
		{
			const int32 Min = static_cast<int32>(V->MissionElapsedTime_s) / 60;
			const double Sec = FMath::Fmod(V->MissionElapsedTime_s, 60.0);
			METStr = (Min > 0)
				? FString::Printf(TEXT("T+%d:%04.1f"), Min, Sec)
				: FString::Printf(TEXT("T+%.1f"), V->MissionElapsedTime_s);
			METColor = FLinearColor(1.0f, 1.0f, 1.0f, 1.0f);
		}

		float MW, MH;
		Canvas->TextSize(BigFont, METStr, MW, MH);
		const float Scale = 1.8f;

		// Background for MET
		const float MetBGW = MW * Scale + 40.0f;
		const float MetBGH = MH * Scale + 12.0f;
		FCanvasTileItem MetBG(FVector2D((W - MetBGW) / 2.0f, 50.0f),
			FVector2D(MetBGW, MetBGH), FLinearColor(0.02f, 0.03f, 0.08f, 0.85f));
		MetBG.BlendMode = SE_BLEND_Translucent;
		Canvas->DrawItem(MetBG);

		FCanvasTextItem MI(FVector2D((W - MW * Scale) / 2.0f, 56.0f),
			FText::FromString(METStr), BigFont, METColor);
		MI.Scale = FVector2D(Scale, Scale);
		Canvas->DrawItem(MI);

		// Mission state label below MET
		FString StateStr = V->GetMissionStateString();
		float SW, SH;
		Canvas->TextSize(SmFont, StateStr, SW, SH);
		FCanvasTextItem SI(FVector2D((W - SW) / 2.0f, 56.0f + MH * Scale + 4.0f),
			FText::FromString(StateStr), SmFont, FLinearColor(0.0f, 0.8f, 1.0f, 1.0f));
		Canvas->DrawItem(SI);

		// Countdown event callouts
		if (V->MissionState == ELaunchVehicleState::Countdown)
		{
			FString EventStr;
			FLinearColor EventColor(1.0f, 0.7f, 0.0f, 1.0f);
			const double TR = V->CountdownTimer_s;
			if (TR <= 0.5) { EventStr = TEXT("LIFTOFF!"); EventColor = FLinearColor(1.0f, 0.5f, 0.0f, 1.0f); }
			else if (TR <= 2.0) EventStr = TEXT("RELEASE CLAMPS");
			else if (TR <= 3.0) { EventStr = TEXT("ENGINE IGNITION"); EventColor = FLinearColor(1.0f, 0.6f, 0.1f, 1.0f); }
			else if (TR <= 4.0) EventStr = TEXT("STARTUP SEQUENCE");
			else EventStr = TEXT("SYSTEMS GO FOR LAUNCH");

			float EW, EH;
			Canvas->TextSize(SmFont, EventStr, EW, EH);
			FCanvasTextItem EI(FVector2D((W - EW) / 2.0f, 56.0f + MH * Scale + 4.0f + SH + 2.0f),
				FText::FromString(EventStr), SmFont, EventColor);
			Canvas->DrawItem(EI);
		}
		else if (V->MissionState == ELaunchVehicleState::Liftoff)
		{
			FString LiftStr = TEXT("ALL ENGINES NOMINAL");
			float LW, LH;
			Canvas->TextSize(SmFont, LiftStr, LW, LH);
			FCanvasTextItem LI(FVector2D((W - LW) / 2.0f, 56.0f + MH * Scale + 4.0f + SH + 2.0f),
				FText::FromString(LiftStr), SmFont, FLinearColor(0.2f, 1.0f, 0.4f, 1.0f));
			Canvas->DrawItem(LI);
		}
		else
		{
			// Mission control callouts — NASA-style event announcements
			FString CalloutStr;
			FLinearColor CalloutColor(0.5f, 0.8f, 0.4f, 0.9f);
			const double MET = V->MissionElapsedTime_s;
			const double Alt_km_c = V->Altitude_m / 1000.0;
			const double Mach_c = V->GetMachNumber();

			if (V->MissionState == ELaunchVehicleState::Complete && V->bOrbitAchieved)
			{
				CalloutStr = TEXT("ORBITAL INSERTION CONFIRMED — NOMINAL");
				CalloutColor = FLinearColor(0.2f, 1.0f, 0.4f, 1.0f);
			}
			else if (V->MissionState == ELaunchVehicleState::Complete && V->IsGroundImpact())
			{
				CalloutStr = TEXT("VEHICLE LOST — GROUND IMPACT");
				CalloutColor = FLinearColor(1.0f, 0.1f, 0.0f, 1.0f);
			}
			else if (V->MissionState == ELaunchVehicleState::Complete)
			{
				CalloutStr = TEXT("FLIGHT TERMINATED — INSUFFICIENT DELTA-V");
				CalloutColor = FLinearColor(1.0f, 0.3f, 0.1f, 1.0f);
			}
			else if (V->MissionState == ELaunchVehicleState::Staging)
			{
				if (V->GetVerticalSpeed_mps() < -10.0)
				{
					CalloutStr = FString::Printf(TEXT("STAGING — COASTING  Vr: %+.0f m/s"),
						V->GetVerticalSpeed_mps());
					CalloutColor = FLinearColor(1.0f, 0.5f, 0.0f, 1.0f);
				}
				else
				{
					CalloutStr = TEXT("STAGE SEPARATION — STAND BY FOR IGNITION");
					CalloutColor = FLinearColor(1.0f, 0.7f, 0.0f, 1.0f);
				}
			}
			else if (V->CurrentStageIndex >= 1 && MET < 180.0)
			{
				CalloutStr = TEXT("SECOND ENGINE START — GUIDANCE IS INTERNAL");
				CalloutColor = FLinearColor(0.3f, 0.9f, 1.0f, 1.0f);
			}
			else if (Alt_km_c > 100.0)
			{
				CalloutStr = TEXT("ABOVE KARMAN LINE — SPACECRAFT IS IN SPACE");
				CalloutColor = FLinearColor(0.3f, 0.7f, 1.0f, 0.9f);
			}
			else if (Alt_km_c > 108.0 && Alt_km_c < 115.0)
			{
				CalloutStr = TEXT("FAIRING SEPARATION CONFIRMED");
			}
			else if (V->GetCurrentQ_kPa() >= V->GetMaxQ_kPa() * 0.95 && V->GetCurrentQ_kPa() > 20.0)
			{
				CalloutStr = TEXT("MAX Q — MAXIMUM DYNAMIC PRESSURE");
				CalloutColor = FLinearColor(1.0f, 0.8f, 0.2f, 1.0f);
			}
			else if (Mach_c >= 0.95 && Mach_c <= 1.15)
			{
				CalloutStr = TEXT("VEHICLE IS TRANSONIC");
				CalloutColor = FLinearColor(0.8f, 0.9f, 0.3f, 0.9f);
			}
			else if (Mach_c > 1.15 && Mach_c < 2.0)
			{
				CalloutStr = TEXT("VEHICLE IS SUPERSONIC — NOMINAL");
			}
			else if (MET < 12.0)
			{
				CalloutStr = TEXT("TOWER CLEAR — ALL ENGINES AT FULL THRUST");
				CalloutColor = FLinearColor(0.2f, 1.0f, 0.4f, 1.0f);
			}
			else if (MET < 30.0)
			{
				CalloutStr = TEXT("PITCH PROGRAM INITIATED — ROLL COMPLETE");
			}
			else if (V->CurrentStageIndex == 0 && V->GetCurrentStage() &&
				V->GetCurrentStage()->CurrentPropellant_kg / FMath::Max(1.0, V->GetCurrentStage()->Spec.PropellantCapacity_kg) < 0.05)
			{
				CalloutStr = TEXT("MECO — MAIN ENGINE CUTOFF");
				CalloutColor = FLinearColor(1.0f, 0.7f, 0.0f, 1.0f);
			}
			else
			{
				CalloutStr = TEXT("NOMINAL — 10x TIME ACCELERATION");
				CalloutColor = FLinearColor(0.4f, 0.55f, 0.6f, 0.7f);
			}

			float SpW, SpH;
			Canvas->TextSize(SmFont, CalloutStr, SpW, SpH);
			FCanvasTextItem SpI(FVector2D((W - SpW) / 2.0f, 56.0f + MH * Scale + 4.0f + SH + 2.0f),
				FText::FromString(CalloutStr), SmFont, CalloutColor);
			Canvas->DrawItem(SpI);
		}
	}

	// ── LEFT: ALTITUDE + VELOCITY + G-FORCE GAUGES ────────────────────────
	{
		const float GaugeTop = 120.0f;
		const float GaugeH = H - 310.0f;

		DrawVerticalGauge(22.0f, GaugeTop, 60.0f, GaugeH,
			V->Altitude_m / 1000.0, 0.0, 450.0,
			TEXT("ALTITUDE"), TEXT("km"),
			FLinearColor(0.2f, 0.7f, 1.0f, 1.0f));

		DrawVerticalGauge(100.0f, GaugeTop, 60.0f, GaugeH,
			V->Velocity_mps, 0.0, 8500.0,
			TEXT("VELOCITY"), TEXT("m/s"),
			FLinearColor(0.3f, 1.0f, 0.4f, 1.0f));

		// G-force gauge — actual felt acceleration from the physics engine
		const double Gforce = V->GetFeltGForce();
		DrawVerticalGauge(178.0f, GaugeTop, 55.0f, GaugeH,
			Gforce, 0.0, 6.0,
			TEXT("G-FORCE"), TEXT("G"),
			FLinearColor(1.0f, 0.6f, 0.2f, 1.0f));

		// Karman line / atmosphere status
		const double Alt_km_val = V->Altitude_m / 1000.0;
		FString AtmStatus;
		FLinearColor AtmColor;
		if (Alt_km_val >= 100.0)
		{
			AtmStatus = TEXT("SPACE — ABOVE KARMAN LINE");
			AtmColor = FLinearColor(0.3f, 0.6f, 1.0f, 0.9f);
		}
		else if (Alt_km_val >= 80.0)
		{
			AtmStatus = TEXT("MESOSPHERE — NEAR SPACE");
			AtmColor = FLinearColor(0.2f, 0.5f, 0.9f, 0.9f);
		}
		else if (Alt_km_val >= 50.0)
		{
			AtmStatus = TEXT("STRATOSPHERE");
			AtmColor = FLinearColor(0.3f, 0.7f, 1.0f, 0.8f);
		}
		else if (Alt_km_val >= 11.0)
		{
			AtmStatus = TEXT("TROPOSPHERE — MAX Q ZONE");
			AtmColor = FLinearColor(0.8f, 0.9f, 1.0f, 0.8f);
		}
		else
		{
			AtmStatus = TEXT("LOWER ATMOSPHERE");
			AtmColor = FLinearColor(0.7f, 0.85f, 1.0f, 0.7f);
		}
		FCanvasTextItem AtmItem(FVector2D(22.0f, GaugeTop + GaugeH + 48.0f),
			FText::FromString(AtmStatus), SmFont, AtmColor);
		Canvas->DrawItem(AtmItem);
	}

	// ── RIGHT: FLIGHT DATA PANEL ───────────────────────────────────────────
	{
		const float PX = W - 260.0f, PY = 120.0f, PW = 240.0f;
		const float PH = 440.0f;
		DrawPanel(PX, PY, PW, PH, TEXT("FLIGHT DATA"), FLinearColor(0.0f, 0.6f, 1.0f, 1.0f));

		float TY = PY + 30.0f;
		const float LX = PX + 12.0f;
		const float Step = 20.0f;

		auto DrawLine = [&](const FString& Label, const FString& Value,
			const FLinearColor& ValColor = FLinearColor::White)
		{
			FCanvasTextItem LI(FVector2D(LX, TY), FText::FromString(Label), SmFont,
				FLinearColor(0.5f, 0.6f, 0.7f, 0.9f));
			Canvas->DrawItem(LI);
			FCanvasTextItem VI(FVector2D(LX + 100.0f, TY), FText::FromString(Value), SmFont, ValColor);
			Canvas->DrawItem(VI);
			TY += Step;
		};

		// TWR — use actual altitude-adjusted thrust from the physics engine
		const double Thrust = V->GetActualThrust_N();
		const double Mass = V->GetTotalMass_kg();
		const double TWR = (Mass > 0.0 && Thrust > 0.0) ? Thrust / (Mass * 9.80665) : 0.0;
		FLinearColor TWRColor = (TWR >= 1.2) ? FLinearColor(0.2f, 1.0f, 0.3f, 1.0f)
			: (TWR >= 0.8) ? FLinearColor(1.0f, 0.9f, 0.2f, 1.0f)
			: FLinearColor(1.0f, 0.3f, 0.2f, 1.0f);
		DrawLine(TEXT("TWR"), FString::Printf(TEXT("%.2f"), TWR), TWRColor);

		// Throttle
		{
			FLinearColor ThColor = (V->ThrottlePct >= 1.0) ? FLinearColor(0.2f, 1.0f, 0.3f, 1.0f)
				: (V->ThrottlePct >= 0.7) ? FLinearColor(1.0f, 0.9f, 0.2f, 1.0f)
				: FLinearColor(1.0f, 0.6f, 0.2f, 1.0f);
			DrawLine(TEXT("Throttle"), FString::Printf(TEXT("%.0f%%"), V->ThrottlePct * 100.0), ThColor);
		}

		DrawLine(TEXT("Pitch"), FString::Printf(TEXT("%.1f deg"), V->GetPitchAngle_deg()));

		// Dynamic pressure with color coding
		{
			const double Q = V->GetCurrentQ_kPa();
			FLinearColor QColor = (Q > 30.0) ? FLinearColor(1.0f, 0.3f, 0.1f, 1.0f)
				: (Q > 15.0) ? FLinearColor(1.0f, 0.8f, 0.2f, 1.0f)
				: FLinearColor(0.3f, 1.0f, 0.5f, 1.0f);
			DrawLine(TEXT("Q"), FString::Printf(TEXT("%.1f kPa"), Q), QColor);
		}
		DrawLine(TEXT("Max Q"), FString::Printf(TEXT("%.1f kPa"), V->GetMaxQ_kPa()));
		DrawLine(TEXT("Mach"), FString::Printf(TEXT("%.1f"), V->GetMachNumber()),
			(V->GetMachNumber() > 1.0) ? FLinearColor(1.0f, 0.7f, 0.2f, 1.0f) : FLinearColor::White);
		DrawLine(TEXT("Downrange"), FString::Printf(TEXT("%.0f km"), V->GetDownrange_km()));
		DrawLine(TEXT("Mass"), FString::Printf(TEXT("%.0f kg"), Mass));

		// Velocity decomposition — critical for orbital mechanics understanding
		{
			const double Vr = V->GetVerticalSpeed_mps();
			const double Vt = V->GetTangentialSpeed_mps();
			FLinearColor VrColor = (Vr < -50.0) ? FLinearColor(1.0f, 0.3f, 0.1f, 1.0f)
				: (Vr < 0.0) ? FLinearColor(1.0f, 0.7f, 0.2f, 1.0f)
				: FLinearColor(0.3f, 1.0f, 0.5f, 1.0f);
			DrawLine(TEXT("Vr"), FString::Printf(TEXT("%+.0f m/s"), Vr), VrColor);
			DrawLine(TEXT("Vt"), FString::Printf(TEXT("%.0f m/s"), Vt),
				FLinearColor(0.4f, 0.8f, 1.0f, 1.0f));
		}

		TY += 6.0f;

		// Stage info
		AAstralisStageActor* CS = V->GetCurrentStage();
		if (CS)
		{
			DrawLine(TEXT("Stage"), FString::Printf(TEXT("%d / %d"),
				V->CurrentStageIndex + 1, V->Stages.Num()),
				FLinearColor(0.5f, 0.8f, 1.0f, 1.0f));

			const double PropPct = (CS->Spec.PropellantCapacity_kg > 0)
				? (CS->CurrentPropellant_kg / CS->Spec.PropellantCapacity_kg) * 100.0 : 0.0;
			DrawLine(TEXT("Propellant"), FString::Printf(TEXT("%.1f%%"), PropPct));

			// Propellant bar
			TY += 2.0f;
			FLinearColor PropColor = (PropPct > 30) ? FLinearColor(0.1f, 0.8f, 0.3f, 0.9f)
				: (PropPct > 10) ? FLinearColor(1.0f, 0.8f, 0.0f, 0.9f)
				: FLinearColor(1.0f, 0.2f, 0.1f, 0.9f);
			DrawProgressBar(LX, TY, PW - 24.0f, 12.0f,
				CS->CurrentPropellant_kg, CS->Spec.PropellantCapacity_kg,
				PropColor, TEXT(""));
			TY += 20.0f;
		}

		// Orbit prediction
		if (V->Altitude_m > 80.0e3)
		{
			TY += 4.0f;
			FCanvasTextItem OrbTitle(FVector2D(LX, TY),
				FText::FromString(TEXT("ORBIT PREDICTION")), SmFont,
				FLinearColor(0.9f, 0.8f, 0.2f, 1.0f));
			Canvas->DrawItem(OrbTitle);
			TY += Step;

			if (V->OrbPeri_km < 0.0)
			{
				DrawLine(TEXT("Perigee"), TEXT("SUBORBITAL"),
					FLinearColor(1.0f, 0.4f, 0.1f, 1.0f));
			}
			else
			{
				DrawLine(TEXT("Perigee"), FString::Printf(TEXT("%.0f km"), V->OrbPeri_km),
					FLinearColor(0.3f, 1.0f, 0.5f, 1.0f));
			}
			DrawLine(TEXT("Apogee"), FString::Printf(TEXT("%.0f km"), V->OrbApo_km));

			// Orbital velocity progress
			TY += 4.0f;
			const double VOrbit = 7670.0;
			DrawProgressBar(LX, TY, PW - 24.0f, 14.0f,
				V->Velocity_mps, VOrbit,
				FLinearColor(0.0f, 0.7f, 1.0f, 0.9f),
				FString::Printf(TEXT("%.0f / %.0f m/s"), V->Velocity_mps, VOrbit));
		}
	}

	// ── BOOSTBACK / RECOVERY NOTIFICATION ──────────────────────────────────
	{
		const FLaunchVehicleDefinition& VehDef = GM->GetSelectedVehicle();
		if (V->CurrentStageIndex >= 1 && VehDef.Stages.Num() > 0 && VehDef.Stages[0].bHasRecoverySystem)
		{
			const float NX = W - 260.0f, NY = H - 340.0f, NW = 240.0f, NH = 60.0f;
			DrawPanel(NX, NY, NW, NH, TEXT(""), FLinearColor(0.2f, 0.6f, 1.0f, 0.8f));

			const double TimeSinceStage = V->MissionElapsedTime_s - V->S1BurnTime_s;
			FString RecovStr;
			FLinearColor RecovCol;
			if (TimeSinceStage < 30.0)
			{
				RecovStr = TEXT("S1 BOOSTBACK BURN");
				RecovCol = FLinearColor(1.0f, 0.6f, 0.1f, 1.0f);
			}
			else if (TimeSinceStage < 120.0)
			{
				RecovStr = TEXT("S1 ENTRY BURN");
				RecovCol = FLinearColor(1.0f, 0.8f, 0.2f, 1.0f);
			}
			else if (TimeSinceStage < 200.0)
			{
				RecovStr = TEXT("S1 LANDING BURN");
				RecovCol = FLinearColor(0.3f, 0.9f, 1.0f, 1.0f);
			}
			else
			{
				RecovStr = TEXT("S1 LANDING CONFIRMED");
				RecovCol = FLinearColor(0.2f, 1.0f, 0.3f, 1.0f);
			}

			FCanvasTextItem RI(FVector2D(NX + 10.0f, NY + 8.0f),
				FText::FromString(RecovStr), SmFont, RecovCol);
			Canvas->DrawItem(RI);
			FCanvasTextItem R2(FVector2D(NX + 10.0f, NY + 28.0f),
				FText::FromString(TEXT("First stage autonomous return")), SmFont,
				FLinearColor(0.5f, 0.6f, 0.7f, 0.7f));
			R2.Scale = FVector2D(0.85f, 0.85f);
			Canvas->DrawItem(R2);
		}
	}

	// ── STATUS STRIP (single row) ──────────────────────────────────────────
	{
		const float StripY = H - 270.0f;
		const float StripW = W - 540.0f;
		const float StripH = 26.0f;
		const float StripX = 250.0f;

		FCanvasTileItem StripBG(FVector2D(StripX, StripY), FVector2D(StripW, StripH),
			FLinearColor(0.02f, 0.03f, 0.06f, 0.7f));
		StripBG.BlendMode = SE_BLEND_Translucent;
		Canvas->DrawItem(StripBG);

		float CurrX = StripX + 10.0f;
		auto DrawChip = [&](const FString& Label, const FLinearColor& Col)
		{
			float CW2, CH2;
			Canvas->TextSize(SmFont, Label, CW2, CH2);
			const float ChipW = CW2 * 0.8f + 10.0f;
			FCanvasTileItem Bg(FVector2D(CurrX, StripY + 4.0f), FVector2D(ChipW, 18.0f),
				FLinearColor(Col.R * 0.15f, Col.G * 0.15f, Col.B * 0.15f, 0.5f));
			Bg.BlendMode = SE_BLEND_Translucent;
			Canvas->DrawItem(Bg);
			FCanvasTextItem CI(FVector2D(CurrX + 5.0f, StripY + 5.0f),
				FText::FromString(Label), SmFont, Col);
			CI.Scale = FVector2D(0.8f, 0.8f);
			Canvas->DrawItem(CI);
			CurrX += ChipW + 6.0f;
		};

		DrawChip(TEXT("RANGE: GO"), FLinearColor(0.2f, 1.0f, 0.3f));
		DrawChip(FString::Printf(TEXT("STAGE %d/%d"), V->CurrentStageIndex + 1, V->Stages.Num()),
			FLinearColor(0.5f, 0.8f, 1.0f));
		DrawChip(FString::Printf(TEXT("WIND %.0f kts"), GM->WindSpeed_kts),
			GM->WindSpeed_kts > 25.0 ? FLinearColor(1.0f, 0.5f, 0.2f) : FLinearColor(0.3f, 0.9f, 0.5f));
		if (V->MissionState == ELaunchVehicleState::Countdown)
		{
			const double FuelPct = FMath::Clamp(1.0 - V->CountdownTimer_s / V->CountdownDuration_s, 0.0, 1.0) * 100.0;
			DrawChip(FString::Printf(TEXT("PROP %.0f%%"), FuelPct),
				FuelPct >= 100.0 ? FLinearColor(0.2f, 1.0f, 0.3f) : FLinearColor(1.0f, 0.8f, 0.2f));
		}
		DrawChip(TEXT("GNC: GO"), FLinearColor(0.2f, 1.0f, 0.3f));
	}

	// ── BOTTOM: TELEMETRY GRAPH ───────────────────────────────────────────
	DrawTelemetryGraph(250.0f, H - 220.0f, W - 540.0f, 100.0f, V);

	// ── CONTROLS HINT ─────────────────────────────────────────────────────
	{
		FString LaunchCtrl = TEXT("[Up/Down] Throttle  [C] Camera  [Backspace] Abort  [R] Return");
		float LCW, LCH;
		Canvas->TextSize(SmFont, LaunchCtrl, LCW, LCH);
		FCanvasTextItem LCI(FVector2D((W - LCW) / 2.0f, H - 110.0f),
			FText::FromString(LaunchCtrl), SmFont, FLinearColor(0.5f, 0.7f, 0.4f, 0.7f));
		Canvas->DrawItem(LCI);
	}

	// ── SECONDARY TELEMETRY ROW (Mach / Q / Throttle / Propellant) ──────
	{
		const float Row2Y = H - 172.0f;
		const float Row2H = 30.0f;

		FCanvasTileItem R2BG(FVector2D(0, Row2Y), FVector2D(W, Row2H),
			FLinearColor(0.01f, 0.018f, 0.045f, 0.82f));
		R2BG.BlendMode = SE_BLEND_Translucent;
		Canvas->DrawItem(R2BG);

		FCanvasTileItem R2Acc(FVector2D(0, Row2Y), FVector2D(W, 1.0f),
			FLinearColor(0.0f, 0.35f, 0.7f, 0.5f));
		R2Acc.BlendMode = SE_BLEND_Translucent;
		Canvas->DrawItem(R2Acc);

		const float R2CellW = W / 5.0f;
		auto DrawCell2 = [&](int32 Idx, const FString& Label, const FString& Value, const FLinearColor& VCol, float BarPct = -1.0f)
		{
			const float CX = R2CellW * Idx + 8.0f;
			if (Idx > 0)
			{
				FCanvasTileItem Sep(FVector2D(R2CellW * Idx - 1.0f, Row2Y + 4.0f),
					FVector2D(1.0f, Row2H - 8.0f), FLinearColor(0.2f, 0.3f, 0.5f, 0.25f));
				Sep.BlendMode = SE_BLEND_Translucent;
				Canvas->DrawItem(Sep);
			}
			FCanvasTextItem LI(FVector2D(CX, Row2Y + 2.0f), FText::FromString(Label), SmFont,
				FLinearColor(0.3f, 0.4f, 0.55f, 0.65f));
			LI.Scale = FVector2D(0.65f, 0.65f);
			Canvas->DrawItem(LI);
			FCanvasTextItem VI(FVector2D(CX, Row2Y + 13.0f), FText::FromString(Value), SmFont, VCol);
			VI.Scale = FVector2D(0.9f, 0.9f);
			Canvas->DrawItem(VI);

			if (BarPct >= 0.0f)
			{
				const float BX = CX + 120.0f;
				const float BW = R2CellW - 140.0f;
				const float BY = Row2Y + 16.0f;
				const float BH = 8.0f;
				FCanvasTileItem BBG(FVector2D(BX, BY), FVector2D(BW, BH),
					FLinearColor(0.1f, 0.1f, 0.15f, 0.6f));
				BBG.BlendMode = SE_BLEND_Translucent;
				Canvas->DrawItem(BBG);
				const float FillW = BW * FMath::Clamp(static_cast<float>(BarPct), 0.0f, 1.0f);
				FCanvasTileItem BFill(FVector2D(BX, BY), FVector2D(FillW, BH), VCol * FLinearColor(1, 1, 1, 0.7f));
				BFill.BlendMode = SE_BLEND_Translucent;
				Canvas->DrawItem(BFill);
			}
		};

		const double Mach = V->GetMachNumber();
		FLinearColor MachCol = (Mach > 5.0) ? FLinearColor(1.0f, 0.4f, 0.2f)
			: (Mach > 1.0) ? FLinearColor(1.0f, 0.8f, 0.2f)
			: FLinearColor(0.6f, 0.8f, 1.0f);
		DrawCell2(0, TEXT("MACH"), FString::Printf(TEXT("%.2f"), Mach), MachCol);

		const double QCurr = V->GetCurrentQ_kPa();
		const double QMax = V->GetMaxQ_kPa();
		FLinearColor QCol = (QCurr > QMax * 0.8 && QMax > 0) ? FLinearColor(1.0f, 0.4f, 0.2f)
			: FLinearColor(0.5f, 0.8f, 1.0f);
		DrawCell2(1, TEXT("DYN PRESS"), FString::Printf(TEXT("%.1f kPa"), QCurr), QCol, (QMax > 0) ? QCurr / QMax : 0.0);

		const double Thr = V->ThrottlePct * 100.0;
		FLinearColor ThrCol = (Thr < 70.0) ? FLinearColor(1.0f, 0.7f, 0.2f) : FLinearColor(0.3f, 1.0f, 0.5f);
		DrawCell2(2, TEXT("THROTTLE"), FString::Printf(TEXT("%.0f%%"), Thr), ThrCol, V->ThrottlePct);

		AAstralisStageActor* CurStage = V->GetCurrentStage();
		double PropPct = 0.0;
		double PropKg = 0.0;
		if (CurStage && CurStage->Spec.PropellantCapacity_kg > 0)
		{
			PropKg = CurStage->CurrentPropellant_kg;
			PropPct = PropKg / CurStage->Spec.PropellantCapacity_kg;
		}
		FLinearColor PropCol = (PropPct < 0.15) ? FLinearColor(1.0f, 0.3f, 0.1f)
			: (PropPct < 0.35) ? FLinearColor(1.0f, 0.7f, 0.2f)
			: FLinearColor(0.2f, 0.9f, 0.5f);
		DrawCell2(3, TEXT("PROPELLANT"), FString::Printf(TEXT("%.0f kg (%.0f%%)"), PropKg, PropPct * 100.0), PropCol, PropPct);

		// Flight profile phase indicator in the 5th cell
		FString PhaseStr;
		FLinearColor PhaseCol;
		if (V->MissionState == ELaunchVehicleState::OnPad || V->MissionState == ELaunchVehicleState::Countdown)
		{
			PhaseStr = TEXT("PRE-FLIGHT");
			PhaseCol = FLinearColor(0.5f, 0.5f, 0.6f);
		}
		else if (V->GetPitchAngle_deg() > 80.0)
		{
			PhaseStr = TEXT("VERTICAL CLIMB");
			PhaseCol = FLinearColor(0.3f, 1.0f, 0.5f);
		}
		else if (V->GetPitchAngle_deg() > 30.0)
		{
			PhaseStr = TEXT("GRAVITY TURN");
			PhaseCol = FLinearColor(0.2f, 0.8f, 1.0f);
		}
		else if (V->GetPitchAngle_deg() > 5.0)
		{
			PhaseStr = TEXT("ORBITAL INSERT");
			PhaseCol = FLinearColor(0.8f, 0.5f, 1.0f);
		}
		else if (V->MissionState == ELaunchVehicleState::CoastPhase)
		{
			PhaseStr = TEXT("COAST ARC");
			PhaseCol = FLinearColor(0.5f, 0.8f, 1.0f);
		}
		else if (V->MissionState == ELaunchVehicleState::Complete)
		{
			PhaseStr = V->bOrbitAchieved ? TEXT("ORBIT ACHIEVED") : TEXT("TERMINATED");
			PhaseCol = V->bOrbitAchieved ? FLinearColor(0.2f, 1.0f, 0.4f) : FLinearColor(1.0f, 0.3f, 0.1f);
		}
		else
		{
			PhaseStr = TEXT("POWERED FLIGHT");
			PhaseCol = FLinearColor(1.0f, 0.8f, 0.3f);
		}
		DrawCell2(4, TEXT("PHASE"), PhaseStr, PhaseCol);
	}

	// ── WEBCAST-STYLE TELEMETRY BAR (SpaceX broadcast quality) ───────────
	{
		const float BarY = H - 138.0f;
		const float BarH = 40.0f;

		// Bar background with subtle gradient
		FCanvasTileItem BarBGTop(FVector2D(0, BarY), FVector2D(W, BarH * 0.5f),
			FLinearColor(0.015f, 0.025f, 0.06f, 0.90f));
		BarBGTop.BlendMode = SE_BLEND_Translucent;
		Canvas->DrawItem(BarBGTop);
		FCanvasTileItem BarBGBot(FVector2D(0, BarY + BarH * 0.5f), FVector2D(W, BarH * 0.5f),
			FLinearColor(0.008f, 0.015f, 0.04f, 0.92f));
		BarBGBot.BlendMode = SE_BLEND_Translucent;
		Canvas->DrawItem(BarBGBot);

		// Top and bottom accent lines
		FCanvasTileItem AccTop(FVector2D(0, BarY), FVector2D(W, 1.5f),
			FLinearColor(0.0f, 0.45f, 0.9f, 0.7f));
		AccTop.BlendMode = SE_BLEND_Translucent;
		Canvas->DrawItem(AccTop);
		FCanvasTileItem AccBot(FVector2D(0, BarY + BarH - 1.0f), FVector2D(W, 1.0f),
			FLinearColor(0.0f, 0.3f, 0.6f, 0.3f));
		AccBot.BlendMode = SE_BLEND_Translucent;
		Canvas->DrawItem(AccBot);

		const float CellW = W / 6.0f;
		auto DrawCell = [&](int32 Idx, const FString& Label, const FString& Value, const FLinearColor& VCol)
		{
			const float CX = CellW * Idx + 8.0f;

			// Separator between cells
			if (Idx > 0)
			{
				FCanvasTileItem CellSep(FVector2D(CellW * Idx - 1.0f, BarY + 6.0f),
					FVector2D(1.0f, BarH - 12.0f),
					FLinearColor(0.2f, 0.35f, 0.6f, 0.3f));
				CellSep.BlendMode = SE_BLEND_Translucent;
				Canvas->DrawItem(CellSep);
			}

			// Label (smaller, top)
			FCanvasTextItem LI(FVector2D(CX, BarY + 4.0f),
				FText::FromString(Label), SmFont, FLinearColor(0.35f, 0.45f, 0.6f, 0.7f));
			LI.Scale = FVector2D(0.75f, 0.75f);
			Canvas->DrawItem(LI);

			// Value (larger, bottom, with shadow)
			FCanvasTextItem VShadow(FVector2D(CX + 1.0f, BarY + 19.0f),
				FText::FromString(Value), SmFont, FLinearColor(0.0f, 0.0f, 0.0f, 0.4f));
			VShadow.Scale = FVector2D(1.05f, 1.05f);
			Canvas->DrawItem(VShadow);

			FCanvasTextItem VI(FVector2D(CX, BarY + 18.0f),
				FText::FromString(Value), SmFont, VCol);
			VI.Scale = FVector2D(1.05f, 1.05f);
			Canvas->DrawItem(VI);
		};

		DrawCell(0, TEXT("SPEED"), FString::Printf(TEXT("%.0f m/s"), V->Velocity_mps),
			FLinearColor(0.3f, 1.0f, 0.4f));
		DrawCell(1, TEXT("ALTITUDE"), FString::Printf(TEXT("%.1f km"), V->Altitude_m / 1000.0),
			FLinearColor(0.2f, 0.8f, 1.0f));
		{
			const double Vr = V->GetVerticalSpeed_mps();
			FLinearColor VrCol = (Vr < -50.0) ? FLinearColor(1.0f, 0.3f, 0.1f)
				: (Vr < 0.0) ? FLinearColor(1.0f, 0.7f, 0.2f)
				: FLinearColor(0.3f, 1.0f, 0.5f);
			DrawCell(2, TEXT("V/S"), FString::Printf(TEXT("%+.0f m/s"), Vr), VrCol);
		}
		DrawCell(3, TEXT("G-FORCE"), FString::Printf(TEXT("%.1f G"), V->GetFeltGForce()),
			FLinearColor(1.0f, 0.6f, 0.2f));
		DrawCell(4, TEXT("STAGE"), FString::Printf(TEXT("%d / %d"), V->CurrentStageIndex + 1, V->Stages.Num()),
			FLinearColor(0.5f, 0.8f, 1.0f));
		DrawCell(5, TEXT("DOWNRANGE"), FString::Printf(TEXT("%.0f km"), V->GetDownrange_km()),
			FLinearColor(0.7f, 0.7f, 0.8f));
	}

	// ── BOTTOM: MISSION TIMELINE ───────────────────────────────────────────
	{
		DrawMissionTimeline(20.0f, H - 80.0f, W - 40.0f, 42.0f, V);
	}

	// ── ANOMALY WARNING ───────────────────────────────────────────────
	if (V->bAnomalyTriggered && V->ActiveAnomaly != AAstralisLaunchVehicleActor::EAnomalyType::None)
	{
		const float AW = 500.0f, AH = 50.0f;
		const float AX = (W - AW) / 2.0f;
		const float AY = 140.0f;

		const float Flash = (FMath::Sin(GetWorld()->GetTimeSeconds() * 6.0f) + 1.0f) * 0.5f;
		FLinearColor WarnBG(0.4f * Flash, 0.02f, 0.02f, 0.85f);
		FCanvasTileItem WBG(FVector2D(AX, AY), FVector2D(AW, AH), WarnBG);
		WBG.BlendMode = SE_BLEND_Translucent;
		Canvas->DrawItem(WBG);

		FCanvasTileItem WBorder(FVector2D(AX, AY), FVector2D(AW, 3.0f),
			FLinearColor(1.0f, 0.2f, 0.0f, 1.0f));
		WBorder.BlendMode = SE_BLEND_Translucent;
		Canvas->DrawItem(WBorder);

		FCanvasTextItem WarnLabel(FVector2D(AX + 15.0f, AY + 6.0f),
			FText::FromString(TEXT("ANOMALY DETECTED")), SmFont,
			FLinearColor(1.0f, 0.3f, 0.1f, 1.0f));
		WarnLabel.Scale = FVector2D(1.1f, 1.1f);
		Canvas->DrawItem(WarnLabel);

		FCanvasTextItem WarnDesc(FVector2D(AX + 15.0f, AY + 26.0f),
			FText::FromString(V->AnomalyDescription), SmFont,
			FLinearColor(1.0f, 0.7f, 0.2f, 0.9f));
		Canvas->DrawItem(WarnDesc);
	}

	// ── G-FORCE WARNING OVERLAY ───────────────────────────────────────────
	{
		const double Gf = V->GetFeltGForce();
		if (Gf > 4.0)
		{
			const float GAlpha = static_cast<float>(FMath::Clamp((Gf - 4.0) / 3.0, 0.0, 0.5));
			// Red vignette on edges
			const float EdgeW = W * 0.15f;
			FCanvasTileItem Left(FVector2D(0, 0), FVector2D(EdgeW, H),
				FLinearColor(1.0f, 0.0f, 0.0f, GAlpha * 0.3f));
			Left.BlendMode = SE_BLEND_Translucent;
			Canvas->DrawItem(Left);
			FCanvasTileItem Right(FVector2D(W - EdgeW, 0), FVector2D(EdgeW, H),
				FLinearColor(1.0f, 0.0f, 0.0f, GAlpha * 0.3f));
			Right.BlendMode = SE_BLEND_Translucent;
			Canvas->DrawItem(Right);
			FCanvasTileItem Top(FVector2D(0, 0), FVector2D(W, EdgeW * 0.5f),
				FLinearColor(1.0f, 0.0f, 0.0f, GAlpha * 0.2f));
			Top.BlendMode = SE_BLEND_Translucent;
			Canvas->DrawItem(Top);
			FCanvasTileItem Bottom(FVector2D(0, H - EdgeW * 0.5f), FVector2D(W, EdgeW * 0.5f),
				FLinearColor(1.0f, 0.0f, 0.0f, GAlpha * 0.2f));
			Bottom.BlendMode = SE_BLEND_Translucent;
			Canvas->DrawItem(Bottom);

			// Warning text
			FString WarnStr = FString::Printf(TEXT("HIGH G WARNING: %.1f G"), Gf);
			float WW, WH;
			Canvas->TextSize(SmFont, WarnStr, WW, WH);
			FCanvasTextItem WI(FVector2D((W - WW) / 2.0f, 44.0f),
				FText::FromString(WarnStr), SmFont, FLinearColor(1.0f, 0.3f, 0.1f, 0.9f));
			Canvas->DrawItem(WI);
		}
	}

	// ── MAX-Q CALLOUT ─────────────────────────────────────────────────────
	{
		const double QCurr_mq = V->GetCurrentQ_kPa();
		const double QMax_mq = V->GetMaxQ_kPa();
		if (QCurr_mq > QMax_mq * 0.85 && QMax_mq > 5.0 && V->MissionState == ELaunchVehicleState::Ascent)
		{
			const float MQW = 200.0f, MQH = 36.0f;
			const float MQX = (W - MQW) / 2.0f;
			const float MQY = 100.0f;
			const float Pulse = (FMath::Sin(GetWorld()->GetTimeSeconds() * 3.0f) + 1.0f) * 0.5f;

			FCanvasTileItem MQBG(FVector2D(MQX, MQY), FVector2D(MQW, MQH),
				FLinearColor(0.15f, 0.08f, 0.0f, 0.85f));
			MQBG.BlendMode = SE_BLEND_Translucent;
			Canvas->DrawItem(MQBG);
			FCanvasTileItem MQBorder(FVector2D(MQX, MQY), FVector2D(MQW, 2.0f),
				FLinearColor(1.0f, 0.6f, 0.0f, 0.8f + 0.2f * Pulse));
			MQBorder.BlendMode = SE_BLEND_Translucent;
			Canvas->DrawItem(MQBorder);

			FCanvasTextItem MQLabel(FVector2D(MQX + 10.0f, MQY + 4.0f),
				FText::FromString(TEXT("MAX-Q")), SmFont,
				FLinearColor(1.0f, 0.7f, 0.2f, 0.9f + 0.1f * Pulse));
			MQLabel.Scale = FVector2D(1.1f, 1.1f);
			Canvas->DrawItem(MQLabel);
			FCanvasTextItem MQVal(FVector2D(MQX + 10.0f, MQY + 20.0f),
				FText::FromString(FString::Printf(TEXT("%.1f kPa — THROTTLE DOWN"), QCurr_mq)),
				SmFont, FLinearColor(0.8f, 0.6f, 0.3f, 0.8f));
			MQVal.Scale = FVector2D(0.8f, 0.8f);
			Canvas->DrawItem(MQVal);
		}
	}

	// ── MACH TRANSITION CALLOUT ───────────────────────────────────────────
	{
		const double CurrMach = V->GetMachNumber();
		if (LastMachNumber < 1.0 && CurrMach >= 1.0)
		{
			MachCalloutTimer = 3.0f;
			MachCalloutText = TEXT("SUPERSONIC");
			MachCalloutColor = FLinearColor(1.0f, 0.8f, 0.2f, 1.0f);
		}
		else if (LastMachNumber < 5.0 && CurrMach >= 5.0)
		{
			MachCalloutTimer = 3.0f;
			MachCalloutText = TEXT("HYPERSONIC");
			MachCalloutColor = FLinearColor(1.0f, 0.4f, 0.1f, 1.0f);
		}
		else if (LastMachNumber < 25.0 && CurrMach >= 25.0)
		{
			MachCalloutTimer = 3.0f;
			MachCalloutText = TEXT("ORBITAL VELOCITY");
			MachCalloutColor = FLinearColor(0.3f, 1.0f, 0.5f, 1.0f);
		}
		LastMachNumber = CurrMach;

		if (MachCalloutTimer > 0.0f)
		{
			MachCalloutTimer -= GetWorld()->GetDeltaSeconds();
			const float Alpha = FMath::Clamp(MachCalloutTimer / 1.0f, 0.0f, 1.0f);
			const float MCW = 280.0f, MCH = 32.0f;
			const float MCX = (W - MCW) / 2.0f;
			const float MCY = 60.0f;

			FCanvasTileItem MCBG(FVector2D(MCX, MCY), FVector2D(MCW, MCH),
				FLinearColor(0.0f, 0.0f, 0.0f, 0.7f * Alpha));
			MCBG.BlendMode = SE_BLEND_Translucent;
			Canvas->DrawItem(MCBG);

			float TW_mc, TH_mc;
			Canvas->TextSize(SmFont, MachCalloutText, TW_mc, TH_mc);
			FCanvasTextItem MCI(FVector2D((W - TW_mc * 1.3f) / 2.0f, MCY + 6.0f),
				FText::FromString(MachCalloutText), SmFont,
				FLinearColor(MachCalloutColor.R, MachCalloutColor.G, MachCalloutColor.B, Alpha));
			MCI.Scale = FVector2D(1.3f, 1.3f);
			Canvas->DrawItem(MCI);
		}
	}

	// ── ALTITUDE LOSS WARNING ─────────────────────────────────────────────
	if (V->MissionState != ELaunchVehicleState::Complete &&
		V->GetVerticalSpeed_mps() < -100.0 && V->Altitude_m > 1000.0)
	{
		const float Flash = (FMath::Sin(GetWorld()->GetTimeSeconds() * 4.0f) + 1.0f) * 0.5f;
		FString AltWarn = FString::Printf(TEXT("LOSING ALTITUDE  Vr: %+.0f m/s"), V->GetVerticalSpeed_mps());
		float AWW, AWH;
		Canvas->TextSize(SmFont, AltWarn, AWW, AWH);
		FCanvasTileItem WarnBG(FVector2D((W - AWW - 20.0f) / 2.0f, 190.0f),
			FVector2D(AWW + 20.0f, AWH + 10.0f),
			FLinearColor(0.4f * Flash, 0.1f * Flash, 0.0f, 0.8f));
		WarnBG.BlendMode = SE_BLEND_Translucent;
		Canvas->DrawItem(WarnBG);
		FCanvasTextItem AWI(FVector2D((W - AWW) / 2.0f, 195.0f),
			FText::FromString(AltWarn), SmFont,
			FLinearColor(1.0f, 0.4f, 0.1f, 0.7f + 0.3f * Flash));
		Canvas->DrawItem(AWI);
	}

	// ── IGNITION / STAGING FLASH EFFECTS ──────────────────────────────────
	{
		float FlashAlpha = 0.0f;
		if (V->MissionState == ELaunchVehicleState::Countdown && V->CountdownTimer_s <= 3.0 && V->CountdownTimer_s > 2.5)
		{
			FlashAlpha = 0.15f;
		}
		else if (V->MissionState == ELaunchVehicleState::Liftoff && V->MissionElapsedTime_s < 1.0)
		{
			FlashAlpha = 0.25f * static_cast<float>(1.0 - V->MissionElapsedTime_s);
		}
		else if (V->MissionState == ELaunchVehicleState::Staging)
		{
			FlashAlpha = 0.12f;
		}
		if (FlashAlpha > 0.0f)
		{
			FCanvasTileItem Flash(FVector2D(0, 0), FVector2D(W, H),
				FLinearColor(1.0f, 0.8f, 0.3f, FlashAlpha));
			Flash.BlendMode = SE_BLEND_Translucent;
			Canvas->DrawItem(Flash);
		}
	}

	// ── SUCCESS / FAIL OVERLAY ─────────────────────────────────────────────
	if (V->MissionState == ELaunchVehicleState::Complete)
	{
		const float OW = 440.0f;

		if (V->bOrbitAchieved)
		{
			const float OH = 680.0f;
			const float OX = (W - OW) / 2.0f, OY = FMath::Max(20.0f, H / 2.0f - OH / 2.0f);
			DrawPanel(OX, OY, OW, OH, TEXT(""), FLinearColor(0.1f, 1.0f, 0.3f, 1.0f));

			FCanvasTextItem Suc(FVector2D(OX + 20.0f, OY + 12.0f),
				FText::FromString(TEXT("MISSION SUCCESS")), BigFont,
				FLinearColor(0.1f, 1.0f, 0.3f, 1.0f));
			Suc.Scale = FVector2D(1.5f, 1.5f);
			Canvas->DrawItem(Suc);

			// Mission grade display
			{
				FLinearColor GradeCol;
				if (V->MissionGrade == TEXT("S+") || V->MissionGrade == TEXT("S"))
					GradeCol = FLinearColor(1.0f, 0.85f, 0.0f, 1.0f);
				else if (V->MissionGrade == TEXT("A"))
					GradeCol = FLinearColor(0.2f, 1.0f, 0.4f, 1.0f);
				else if (V->MissionGrade == TEXT("B"))
					GradeCol = FLinearColor(0.3f, 0.8f, 1.0f, 1.0f);
				else
					GradeCol = FLinearColor(1.0f, 0.6f, 0.2f, 1.0f);

				FString GradeStr = FString::Printf(TEXT("%s  %.0f/100"),
					*V->MissionGrade, V->MissionScore);
				float GW_t, GH_t;
				Canvas->TextSize(BigFont, GradeStr, GW_t, GH_t);
				FCanvasTextItem GI(FVector2D(OX + OW - GW_t - 24.0f, OY + 16.0f),
					FText::FromString(GradeStr), BigFont, GradeCol);
				Canvas->DrawItem(GI);
			}

			float TY = OY + 52.0f;
			const float LX = OX + 24.0f;
			const float Step = 18.0f;
			auto StatLine = [&](const FString& Label, const FString& Value, const FLinearColor& Col = FLinearColor::White)
			{
				FCanvasTextItem LI(FVector2D(LX, TY), FText::FromString(Label), SmFont,
					FLinearColor(0.6f, 0.7f, 0.8f, 0.9f));
				LI.Scale = FVector2D(0.9f, 0.9f);
				Canvas->DrawItem(LI);
				FCanvasTextItem VI(FVector2D(LX + 155.0f, TY), FText::FromString(Value), SmFont, Col);
				VI.Scale = FVector2D(0.9f, 0.9f);
				Canvas->DrawItem(VI);
				TY += Step;
			};

			// Mission identity
			StatLine(TEXT("Mission"), GM->GetSelectedMission().Name, FLinearColor(0.5f, 0.85f, 1.0f));
			StatLine(TEXT("Vehicle"), V->VehicleDisplayName, FLinearColor(0.5f, 0.85f, 1.0f));
			if (GM->BusCatalog.Num() > 0)
				StatLine(TEXT("Spacecraft"), GM->GetSelectedBus().Name, FLinearColor(0.7f, 0.5f, 1.0f));
			TY += 4.0f;

			// Flight performance
			StatLine(TEXT("Orbit"), FString::Printf(TEXT("%.0f x %.0f km"), V->OrbPeri_km, V->OrbApo_km),
				FLinearColor(0.3f, 1.0f, 0.5f, 1.0f));
			StatLine(TEXT("Target"), FString::Printf(TEXT("%.0f km"), V->GetTargetAltitude_m() / 1000.0));
			StatLine(TEXT("Max Q / Max G"), FString::Printf(TEXT("%.1f kPa / %.1f G"), V->GetMaxQ_kPa(), V->GetMaxGForce()));
			StatLine(TEXT("Losses"), FString::Printf(TEXT("Grav: %.0f  Drag: %.0f m/s"),
				V->GetGravityLoss_mps(), V->GetDragLoss_mps()));
			StatLine(TEXT("Flight Time"), FString::Printf(TEXT("%.0f s (%.1f min)"),
				V->MissionElapsedTime_s, V->MissionElapsedTime_s / 60.0));
			TY += 4.0f;

			// Spacecraft design summary
			if (GM->BusCatalog.Num() > 0)
			{
				const int32 InstCount = GM->GetSelectedInstrumentCount();
				const double SciMul = GM->GetScienceMultiplier();
				const double BudgetUsed = GM->Budget.GetTotalSpent();
				const double BudgetTotal = GM->Budget.TotalBudget_M;
				const double BudgetPct = (BudgetTotal > 0) ? (BudgetUsed / BudgetTotal * 100.0) : 0.0;

				StatLine(TEXT("Instruments"), FString::Printf(TEXT("%d installed (x%.1f science)"), InstCount, SciMul),
					FLinearColor(1.0f, 0.7f, 0.2f));
				StatLine(TEXT("Budget Used"), FString::Printf(TEXT("$%.0fM / $%.0fM (%.0f%%)"),
					BudgetUsed, BudgetTotal, BudgetPct),
					GM->Budget.IsOverBudget() ? FLinearColor(1.0f, 0.3f, 0.1f) : FLinearColor(0.3f, 1.0f, 0.5f));
				StatLine(TEXT("Payload Mass"), FString::Printf(TEXT("%.0f / %.0f kg"),
					GM->GetInstrumentMass_kg(), GM->GetSelectedBus().MaxPayloadMass_kg));
			}
			TY += 4.0f;

			// Score breakdown with actual calculated values
			FCanvasTextItem ScoreTitle(FVector2D(LX, TY),
				FText::FromString(TEXT("SCORE BREAKDOWN")), SmFont,
				FLinearColor(1.0f, 0.85f, 0.2f, 1.0f));
			ScoreTitle.Scale = FVector2D(0.95f, 0.95f);
			Canvas->DrawItem(ScoreTitle);
			TY += Step;

			const double TargetAlt_km_s = V->GetTargetAltitude_m() / 1000.0;
			const double PeriErr = FMath::Abs(V->OrbPeri_km - TargetAlt_km_s);
			const double AccPct = FMath::Clamp(1.0 - PeriErr / TargetAlt_km_s, 0.0, 1.0);
			auto ScoreColor = [](double Val, double Max) -> FLinearColor {
				double Pct = (Max > 0) ? Val / Max : 0;
				if (Pct >= 0.9) return FLinearColor(0.2f, 1.0f, 0.4f);
				if (Pct >= 0.6) return FLinearColor(1.0f, 0.9f, 0.2f);
				return FLinearColor(1.0f, 0.4f, 0.2f);
			};
			StatLine(TEXT("Orbit Accuracy"), FString::Printf(TEXT("%.1f / 40"), V->ScoreOrbitAccuracy), ScoreColor(V->ScoreOrbitAccuracy, 40.0));
			StatLine(TEXT("Circularity"), FString::Printf(TEXT("%.1f / 15"), V->ScoreCircularity), ScoreColor(V->ScoreCircularity, 15.0));
			StatLine(TEXT("Fuel Efficiency"), FString::Printf(TEXT("%.1f / 20"), V->ScoreFuelEfficiency), ScoreColor(V->ScoreFuelEfficiency, 20.0));
			StatLine(TEXT("Structural"), FString::Printf(TEXT("%.1f / 15"), V->ScoreStructural), ScoreColor(V->ScoreStructural, 15.0));
			StatLine(TEXT("Completion"), FString::Printf(TEXT("%.0f / 10"), V->ScoreCompletion), FLinearColor(0.2f, 1.0f, 0.4f));

			// Flight log (last 4 events)
			TY += 4.0f;
			const int32 LogStart = FMath::Max(0, V->MissionLog.Num() - 4);
			for (int32 i = LogStart; i < V->MissionLog.Num(); ++i)
			{
				const auto& Entry = V->MissionLog[i];
				FString LogLine = FString::Printf(TEXT("T+%.0fs  %s"), Entry.MET_s, *Entry.Event);
				FCanvasTextItem LLI(FVector2D(LX, TY), FText::FromString(LogLine), SmFont,
					FLinearColor(0.5f, 0.6f, 0.7f, 0.7f));
				LLI.Scale = FVector2D(0.8f, 0.8f);
				Canvas->DrawItem(LLI);
				TY += 14.0f;
			}

			TY += 6.0f;
			FCanvasTextItem Trans(FVector2D(LX, TY),
				FText::FromString(TEXT("Transitioning to orbit view...")), SmFont,
				FLinearColor(0.3f, 0.8f, 1.0f, 0.8f));
			Canvas->DrawItem(Trans);
		}
		else
		{
			const float OH = 280.0f;
			const float OX = (W - OW) / 2.0f, OY = H / 2.0f - OH / 2.0f;
			DrawPanel(OX, OY, OW, OH, TEXT(""), FLinearColor(1.0f, 0.2f, 0.1f, 1.0f));

			FString FailTitle = V->IsGroundImpact() ? TEXT("VEHICLE LOST") : TEXT("MISSION FAILED");
			FCanvasTextItem Fail(FVector2D(OX + 20.0f, OY + 12.0f),
				FText::FromString(FailTitle), BigFont,
				FLinearColor(1.0f, 0.2f, 0.1f, 1.0f));
			Fail.Scale = FVector2D(1.5f, 1.5f);
			Canvas->DrawItem(Fail);

			{
				FString GradeStr = FString::Printf(TEXT("Grade: %s  (%.0f/100)"),
					*V->MissionGrade, V->MissionScore);
				float FGW, FGH;
				Canvas->TextSize(SmFont, GradeStr, FGW, FGH);
				FCanvasTextItem FGI(FVector2D(OX + OW - FGW - 24.0f, OY + 20.0f),
					FText::FromString(GradeStr), SmFont, FLinearColor(1.0f, 0.5f, 0.2f, 1.0f));
				Canvas->DrawItem(FGI);
			}

			float TY = OY + 54.0f;
			const float LX = OX + 24.0f;

			if (V->IsGroundImpact())
			{
				FCanvasTextItem CauseMsg(FVector2D(LX, TY),
					FText::FromString(TEXT("Vehicle impacted the ground during flight.")),
					SmFont, FLinearColor(1.0f, 0.5f, 0.3f, 1.0f));
				Canvas->DrawItem(CauseMsg);
				TY += 22.0f;

				FCanvasTextItem CauseDetail(FVector2D(LX, TY),
					FText::FromString(TEXT("The trajectory was not sustainable — vehicle")),
					SmFont, FLinearColor(0.8f, 0.7f, 0.6f, 0.9f));
				Canvas->DrawItem(CauseDetail);
				TY += 18.0f;
				FCanvasTextItem CauseDetail2(FVector2D(LX, TY),
					FText::FromString(TEXT("lost altitude and could not recover.")),
					SmFont, FLinearColor(0.8f, 0.7f, 0.6f, 0.9f));
				Canvas->DrawItem(CauseDetail2);
				TY += 22.0f;
			}
			else
			{
				FCanvasTextItem Msg(FVector2D(LX, TY),
					FText::FromString(FString::Printf(TEXT("Orbit: %.0f x %.0f km (needed %.0f+ km perigee)"),
						V->OrbPeri_km, V->OrbApo_km, V->GetTargetAltitude_m() / 1000.0 - 25.0)),
					SmFont, FLinearColor::White);
				Canvas->DrawItem(Msg);
				TY += 22.0f;
			}

			FCanvasTextItem Msg2(FVector2D(LX, TY),
				FText::FromString(FString::Printf(TEXT("Max Q: %.1f kPa  |  Downrange: %.0f km  |  Alt: %.0f km"),
					V->GetMaxQ_kPa(), V->GetDownrange_km(), V->Altitude_m / 1000.0)),
				SmFont, FLinearColor(0.7f, 0.7f, 0.8f, 0.9f));
			Canvas->DrawItem(Msg2);
			TY += 22.0f;

			FCanvasTextItem Msg3(FVector2D(LX, TY),
				FText::FromString(FString::Printf(TEXT("Velocity: %.0f m/s  |  Flight Time: %.0f s"),
					V->Velocity_mps, V->MissionElapsedTime_s)),
				SmFont, FLinearColor(0.7f, 0.7f, 0.8f, 0.9f));
			Canvas->DrawItem(Msg3);
			TY += 22.0f;

			// Flight log on failure too
			const int32 LogStart = FMath::Max(0, V->MissionLog.Num() - 4);
			for (int32 i = LogStart; i < V->MissionLog.Num(); ++i)
			{
				const auto& Entry = V->MissionLog[i];
				FCanvasTextItem LLI(FVector2D(LX, TY),
					FText::FromString(FString::Printf(TEXT("T+%.0fs  %s"), Entry.MET_s, *Entry.Event)),
					SmFont, FLinearColor(0.6f, 0.7f, 0.8f, 0.8f));
				LLI.Scale = FVector2D(0.85f, 0.85f);
				Canvas->DrawItem(LLI);
				TY += 16.0f;
			}

			TY += 8.0f;
			FCanvasTextItem Hint(FVector2D(LX, TY),
				FText::FromString(TEXT("[R] Return  — try a heavier vehicle or different mission")),
				SmFont, FLinearColor(0.6f, 0.8f, 0.3f, 1.0f));
			Canvas->DrawItem(Hint);
		}
	}
}

// ─────────────────────────────────────────────────────────────────────────────
// ORBIT PHASE HUD
// ─────────────────────────────────────────────────────────────────────────────
void AAstralisAssemblyHUD::DrawOrbitHUD(float W, float H, AAstralisAssemblyGameMode* GM)
{
	if (!GM || !GM->OrbitScene) return;
	AAstralisOrbitSceneActor* OS = GM->OrbitScene;
	const FSpacecraftState& SC = OS->GetSpacecraftState();
	UFont* BigFont = GEngine->GetLargeFont();
	UFont* SmFont = GEngine->GetSmallFont();
	const double EarthR = OS->GetEarthRadius_m();
	const double Mu = 3.986004418e14;

	// ── SPACECRAFT NAME / STATUS ───────────────────────────────────────────
	{
		FString MissionName = (GM->GetMissions().Num() > 0) ? GM->GetSelectedMission().Name : TEXT("Unknown");
		FString BusName = (GM->BusCatalog.Num() > 0) ? GM->GetSelectedBus().Name : TEXT("Spacecraft");
		FString Name = FString::Printf(TEXT("%s — %s"), *BusName.ToUpper(), *GM->GetSelectedMission().OrbitType.ToUpper());
		float NW, NH;
		Canvas->TextSize(BigFont, Name, NW, NH);
		FCanvasTextItem NI(FVector2D((W - NW) / 2.0f, 52.0f),
			FText::FromString(Name), BigFont, FLinearColor(0.5f, 0.85f, 1.0f, 1.0f));
		Canvas->DrawItem(NI);
	}

	// ── TIME WARP CENTER ───────────────────────────────────────────────────
	{
		FString TWLabel = FString::Printf(TEXT("Time Warp: %s"), *OS->GetTimeWarpLabel());
		float TW_w, TW_h;
		Canvas->TextSize(SmFont, TWLabel, TW_w, TW_h);
		const float BWid = TW_w + 30.0f, BHei = TW_h + 12.0f;
		FCanvasTileItem BG(FVector2D((W - BWid) / 2.0f, 76.0f),
			FVector2D(BWid, BHei), FLinearColor(0.02f, 0.04f, 0.08f, 0.85f));
		BG.BlendMode = SE_BLEND_Translucent;
		Canvas->DrawItem(BG);

		FLinearColor TWColor = OS->IsPaused()
			? FLinearColor(1.0f, 0.5f, 0.0f, 1.0f)
			: FLinearColor(0.4f, 0.8f, 1.0f, 1.0f);
		FCanvasTextItem TWI(FVector2D((W - TW_w) / 2.0f, 82.0f),
			FText::FromString(TWLabel), SmFont, TWColor);
		Canvas->DrawItem(TWI);
	}

	// ── LEFT: ORBITAL PARAMETERS ───────────────────────────────────────────
	{
		const float PX = 20.0f, PY = 110.0f, PW = 300.0f, PH = 350.0f;
		DrawPanel(PX, PY, PW, PH, TEXT("ORBITAL PARAMETERS"), FLinearColor(0.3f, 0.7f, 1.0f, 1.0f));

		float TY = PY + 30.0f;
		const float LX = PX + 14.0f;
		const float Step = 22.0f;

		auto DrawLine = [&](const FString& Label, const FString& Value,
			const FLinearColor& ValColor = FLinearColor::White)
		{
			FCanvasTextItem LI(FVector2D(LX, TY), FText::FromString(Label), SmFont,
				FLinearColor(0.5f, 0.6f, 0.7f, 0.9f));
			Canvas->DrawItem(LI);
			FCanvasTextItem VI(FVector2D(LX + 120.0f, TY), FText::FromString(Value), SmFont, ValColor);
			Canvas->DrawItem(VI);
			TY += Step;
		};

		const double R_m = SC.Orbital.Position_m.Size();
		const double Alt_km = (R_m - EarthR) / 1000.0;
		const double Vel = SC.Orbital.Velocity_mps.Size();
		const double Period_min = (2.0 * PI * FMath::Sqrt(FMath::Pow(R_m, 3.0) / Mu)) / 60.0;

		DrawLine(TEXT("Altitude"), FString::Printf(TEXT("%.1f km"), Alt_km));
		DrawLine(TEXT("Velocity"), FString::Printf(TEXT("%.0f m/s"), Vel));
		DrawLine(TEXT("Period"), FString::Printf(TEXT("%.1f min"), Period_min));

		// Orbit number
		const int32 OrbitNum = (Period_min > 0.1) ? static_cast<int32>(SC.SimTime_s / (Period_min * 60.0)) + 1 : 1;
		DrawLine(TEXT("Orbit #"), FString::Printf(TEXT("%d"), OrbitNum),
			FLinearColor(0.9f, 0.8f, 0.3f, 1.0f));

		// Orbital energy
		const double E_orb = 0.5 * Vel * Vel - Mu / R_m;
		const double h_ang = R_m * Vel;
		if (E_orb < 0)
		{
			const double a = -Mu / (2.0 * E_orb);
			const double ecc = FMath::Sqrt(FMath::Max(0.0, 1.0 + 2.0 * E_orb * h_ang * h_ang / (Mu * Mu)));
			const double Peri_km = (a * (1.0 - ecc) - EarthR) / 1000.0;
			const double Apo_km = (a * (1.0 + ecc) - EarthR) / 1000.0;
			DrawLine(TEXT("Perigee"), FString::Printf(TEXT("%.1f km"), Peri_km),
				FLinearColor(0.3f, 1.0f, 0.5f, 1.0f));
			DrawLine(TEXT("Apogee"), FString::Printf(TEXT("%.1f km"), Apo_km),
				FLinearColor(0.3f, 1.0f, 0.5f, 1.0f));
			DrawLine(TEXT("Semi-major"), FString::Printf(TEXT("%.0f km"), a / 1000.0));
			DrawLine(TEXT("Eccentricity"), FString::Printf(TEXT("%.4f"), ecc));
		}

		DrawLine(TEXT("Sim Time"), FString::Printf(TEXT("%.0f s"), SC.SimTime_s));

		// Ground track (sub-satellite point)
		{
			const FVector3d Pos = SC.Orbital.Position_m;
			const double Lat_deg = FMath::RadiansToDegrees(FMath::Asin(FMath::Clamp(Pos.Z / FMath::Max(1.0, Pos.Size()), -1.0, 1.0)));
			const double Lon_deg = FMath::RadiansToDegrees(FMath::Atan2(Pos.Y, Pos.X));
			DrawLine(TEXT("Lat"), FString::Printf(TEXT("%.2f deg"), Lat_deg));
			DrawLine(TEXT("Lon"), FString::Printf(TEXT("%.2f deg"), Lon_deg));
		}
	}

	// ── CENTER: ORBIT ELLIPSE VISUALIZATION ───────────────────────────────
	{
		const double R_m_e = SC.Orbital.Position_m.Size();
		const double Vel_e = SC.Orbital.Velocity_mps.Size();
		const double E_orb_e = 0.5 * Vel_e * Vel_e - Mu / R_m_e;
		const double h_ang_e = R_m_e * Vel_e;

		if (E_orb_e < 0)
		{
			const double a_e = -Mu / (2.0 * E_orb_e);
			const double ecc_e = FMath::Sqrt(FMath::Max(0.0, 1.0 + 2.0 * E_orb_e * h_ang_e * h_ang_e / (Mu * Mu)));
			const float DiagCX = W * 0.5f;
			const float DiagCY = 280.0f;
			const float DiagR = 90.0f;

			FCanvasTileItem DiagBG(FVector2D(DiagCX - DiagR - 20.0f, DiagCY - DiagR - 20.0f),
				FVector2D(DiagR * 2.0f + 40.0f, DiagR * 2.0f + 50.0f),
				FLinearColor(0.02f, 0.03f, 0.06f, 0.75f));
			DiagBG.BlendMode = SE_BLEND_Translucent;
			Canvas->DrawItem(DiagBG);

			FCanvasTextItem DiagTitle(FVector2D(DiagCX - 40.0f, DiagCY - DiagR - 16.0f),
				FText::FromString(TEXT("ORBIT VIEW")), SmFont,
				FLinearColor(0.4f, 0.5f, 0.7f, 0.7f));
			DiagTitle.Scale = FVector2D(0.75f, 0.75f);
			Canvas->DrawItem(DiagTitle);

			// Earth circle
			const float EarthPixR = DiagR * 0.35f;
			const int32 EarthSegs = 24;
			for (int32 seg = 0; seg < EarthSegs; ++seg)
			{
				float A0 = 2.0f * PI * seg / EarthSegs;
				float A1 = 2.0f * PI * (seg + 1) / EarthSegs;
				float EX0 = DiagCX + EarthPixR * FMath::Cos(A0);
				float EY0 = DiagCY + EarthPixR * FMath::Sin(A0);
				float EX1 = DiagCX + EarthPixR * FMath::Cos(A1);
				float EY1 = DiagCY + EarthPixR * FMath::Sin(A1);
				float MinEX = FMath::Min(EX0, EX1), MinEY = FMath::Min(EY0, EY1);
				FCanvasTileItem ESeg(FVector2D(MinEX, MinEY),
					FVector2D(FMath::Abs(EX1 - EX0) + 2.0f, FMath::Max(2.0f, FMath::Abs(EY1 - EY0) + 1.0f)),
					FLinearColor(0.1f, 0.3f, 0.6f, 0.6f));
				ESeg.BlendMode = SE_BLEND_Translucent;
				Canvas->DrawItem(ESeg);
			}

			// Orbit ellipse
			const double ScaleFactor = DiagR / (a_e * (1.0 + ecc_e));
			const float OrbSegs = 48;
			const float FocusOffset = static_cast<float>(a_e * ecc_e * ScaleFactor);
			for (int32 seg = 0; seg < static_cast<int32>(OrbSegs); ++seg)
			{
				float A0 = 2.0f * PI * seg / OrbSegs;
				float A1 = 2.0f * PI * (seg + 1) / OrbSegs;
				float Ra = static_cast<float>(a_e * ScaleFactor);
				float Rb = static_cast<float>(a_e * FMath::Sqrt(FMath::Max(0.0, 1.0 - ecc_e * ecc_e)) * ScaleFactor);
				float OX0 = DiagCX + FocusOffset + Ra * FMath::Cos(A0);
				float OY0 = DiagCY + Rb * FMath::Sin(A0);
				float OX1 = DiagCX + FocusOffset + Ra * FMath::Cos(A1);
				float OY1 = DiagCY + Rb * FMath::Sin(A1);
				float MinOX = FMath::Min(OX0, OX1), MinOY = FMath::Min(OY0, OY1);
				FCanvasTileItem OSeg(FVector2D(MinOX, MinOY),
					FVector2D(FMath::Abs(OX1 - OX0) + 2.0f, FMath::Max(2.0f, FMath::Abs(OY1 - OY0) + 1.0f)),
					FLinearColor(0.3f, 0.8f, 1.0f, 0.7f));
				OSeg.BlendMode = SE_BLEND_Translucent;
				Canvas->DrawItem(OSeg);
			}

			// Spacecraft position dot
			const double TrueAnomaly = FMath::Atan2(SC.Orbital.Position_m.Y, SC.Orbital.Position_m.X);
			float SCAngle = static_cast<float>(TrueAnomaly);
			float SCr = static_cast<float>(R_m_e * ScaleFactor);
			float SCPX = DiagCX + SCr * FMath::Cos(SCAngle);
			float SCPY = DiagCY + SCr * FMath::Sin(SCAngle);
			SCPX = FMath::Clamp(SCPX, DiagCX - DiagR - 10.0f, DiagCX + DiagR + 10.0f);
			SCPY = FMath::Clamp(SCPY, DiagCY - DiagR - 10.0f, DiagCY + DiagR + 10.0f);

			FCanvasTileItem SCDot(FVector2D(SCPX - 4.0f, SCPY - 4.0f), FVector2D(8.0f, 8.0f),
				FLinearColor(1.0f, 1.0f, 1.0f, 1.0f));
			SCDot.BlendMode = SE_BLEND_Translucent;
			Canvas->DrawItem(SCDot);

			// Perigee/Apogee labels
			const double Peri_km_e = (a_e * (1.0 - ecc_e) - EarthR) / 1000.0;
			const double Apo_km_e = (a_e * (1.0 + ecc_e) - EarthR) / 1000.0;
			FCanvasTextItem PeriLbl(FVector2D(DiagCX - DiagR - 16.0f, DiagCY + DiagR + 6.0f),
				FText::FromString(FString::Printf(TEXT("Pe: %.0f km"), Peri_km_e)),
				SmFont, FLinearColor(0.3f, 1.0f, 0.5f, 0.8f));
			PeriLbl.Scale = FVector2D(0.7f, 0.7f);
			Canvas->DrawItem(PeriLbl);
			FCanvasTextItem ApoLbl(FVector2D(DiagCX + 20.0f, DiagCY + DiagR + 6.0f),
				FText::FromString(FString::Printf(TEXT("Ap: %.0f km"), Apo_km_e)),
				SmFont, FLinearColor(0.5f, 0.8f, 1.0f, 0.8f));
			ApoLbl.Scale = FVector2D(0.7f, 0.7f);
			Canvas->DrawItem(ApoLbl);

			FCanvasTextItem EccLbl(FVector2D(DiagCX - 30.0f, DiagCY + DiagR + 18.0f),
				FText::FromString(FString::Printf(TEXT("e = %.4f"), ecc_e)),
				SmFont, FLinearColor(0.5f, 0.6f, 0.7f, 0.6f));
			EccLbl.Scale = FVector2D(0.65f, 0.65f);
			Canvas->DrawItem(EccLbl);
		}
	}

	// ── RIGHT: SPACECRAFT SYSTEMS ──────────────────────────────────────────
	{
		const float PX = W - 320.0f, PY = 110.0f, PW = 300.0f, PH = 320.0f;
		DrawPanel(PX, PY, PW, PH, TEXT("SPACECRAFT SYSTEMS"), FLinearColor(0.0f, 0.9f, 0.5f, 1.0f));

		float TY = PY + 30.0f;
		const float LX = PX + 14.0f;
		const float Step = 22.0f;
		const float BarX = LX;
		const float BarW = PW - 28.0f;

		// POWER
		{
			FCanvasTextItem PTitle(FVector2D(LX, TY),
				FText::FromString(TEXT("POWER")), SmFont,
				FLinearColor(1.0f, 0.8f, 0.2f, 1.0f));
			Canvas->DrawItem(PTitle);

			FString EclipseStr = SC.Power.bInEclipse ? TEXT("  [ECLIPSE]") : TEXT("");
			FCanvasTextItem EI(FVector2D(LX + 60.0f, TY),
				FText::FromString(EclipseStr), SmFont,
				FLinearColor(1.0f, 0.3f, 0.1f, 1.0f));
			Canvas->DrawItem(EI);
			TY += 18.0f;

			DrawProgressBar(BarX, TY, BarW, 14.0f,
				SC.Power.SolarGeneration_W, 30.0,
				FLinearColor(1.0f, 0.85f, 0.1f, 0.9f),
				FString::Printf(TEXT("Solar: %.1f W"), SC.Power.SolarGeneration_W));
			TY += 20.0f;

			const double BattPct = (SC.Power.BatteryCapacity_J > 0)
				? (SC.Power.BatteryEnergy_J / SC.Power.BatteryCapacity_J) * 100.0 : 0.0;
			FLinearColor BattColor = (BattPct > 50) ? FLinearColor(0.1f, 0.8f, 0.3f, 0.9f)
				: (BattPct > 20) ? FLinearColor(1.0f, 0.8f, 0.0f, 0.9f)
				: FLinearColor(1.0f, 0.2f, 0.1f, 0.9f);
			DrawProgressBar(BarX, TY, BarW, 14.0f,
				BattPct, 100.0, BattColor,
				FString::Printf(TEXT("Battery: %.0f%%"), BattPct));
			TY += 24.0f;
		}

		// COMMUNICATIONS
		{
			FCanvasTextItem CTitle(FVector2D(LX, TY),
				FText::FromString(TEXT("COMMUNICATIONS")), SmFont,
				FLinearColor(0.4f, 0.8f, 1.0f, 1.0f));
			Canvas->DrawItem(CTitle);
			TY += 18.0f;

			FLinearColor LinkColor = SC.Comms.bGroundStationVisible
				? FLinearColor(0.1f, 1.0f, 0.3f, 1.0f)
				: FLinearColor(1.0f, 0.3f, 0.1f, 1.0f);
			FString LinkStr = SC.Comms.bGroundStationVisible
				? TEXT("LINK ESTABLISHED") : TEXT("NO LINK");
			FCanvasTextItem LI(FVector2D(LX, TY), FText::FromString(LinkStr), SmFont, LinkColor);
			Canvas->DrawItem(LI);
			TY += 18.0f;

			FCanvasTextItem MI(FVector2D(LX, TY),
				FText::FromString(FString::Printf(TEXT("Margin: %.1f dB  |  Rate: %.0f bps"),
					SC.Comms.LinkMargin_dB, SC.Comms.AchievableDataRate_bps)),
				SmFont, FLinearColor(0.7f, 0.7f, 0.8f, 0.9f));
			Canvas->DrawItem(MI);
			TY += 24.0f;
		}

		// THERMAL
		{
			FCanvasTextItem TTitle(FVector2D(LX, TY),
				FText::FromString(TEXT("THERMAL")), SmFont,
				FLinearColor(1.0f, 0.5f, 0.2f, 1.0f));
			Canvas->DrawItem(TTitle);
			TY += 18.0f;

			FLinearColor TempColor = (SC.Thermal.bAnyOvertemp || SC.Thermal.bAnyUndertemp)
				? FLinearColor(1.0f, 0.3f, 0.1f, 1.0f)
				: FLinearColor(0.3f, 1.0f, 0.5f, 1.0f);

			FCanvasTextItem TI(FVector2D(LX, TY),
				FText::FromString(FString::Printf(TEXT("Range: %.0f C to %.0f C"),
					SC.Thermal.MinTemperature_C, SC.Thermal.MaxTemperature_C)),
				SmFont, TempColor);
			Canvas->DrawItem(TI);
			TY += 18.0f;

			FString ThermalStatus = (SC.Thermal.bAnyOvertemp) ? TEXT("WARNING: OVERTEMP")
				: (SC.Thermal.bAnyUndertemp) ? TEXT("WARNING: UNDERTEMP")
				: TEXT("NOMINAL");
			FCanvasTextItem TSI(FVector2D(LX, TY),
				FText::FromString(ThermalStatus), SmFont, TempColor);
			Canvas->DrawItem(TSI);
			TY += 24.0f;
		}

		// MASS
		{
			FCanvasTextItem MTitle(FVector2D(LX, TY),
				FText::FromString(TEXT("MASS")), SmFont,
				FLinearColor(0.7f, 0.7f, 0.8f, 1.0f));
			Canvas->DrawItem(MTitle);
			TY += 18.0f;

			FCanvasTextItem MI(FVector2D(LX, TY),
				FText::FromString(FString::Printf(TEXT("Total: %.0f kg  |  Prop: %.0f kg"),
					SC.Mass.TotalMass_kg, SC.Mass.PropellantMass_kg)),
				SmFont, FLinearColor::White);
			Canvas->DrawItem(MI);
		}
	}

	// ── MANEUVER PANEL ─────────────────────────────────────────────────────
	{
		const float PX = 20.0f, PY = H - 200.0f, PW = 280.0f, PH = 130.0f;
		DrawPanel(PX, PY, PW, PH, TEXT("ORBITAL MANEUVERS"), FLinearColor(0.9f, 0.5f, 0.0f, 1.0f));

		float TY = PY + 26.0f;
		const float LX = PX + 14.0f;

		auto DrawKey = [&](const FString& Str, const FLinearColor& Col = FLinearColor::White)
		{
			FCanvasTextItem TI(FVector2D(LX, TY), FText::FromString(Str), SmFont, Col);
			Canvas->DrawItem(TI);
			TY += 17.0f;
		};

		DrawKey(TEXT("[H] Prograde    [J] Retrograde   (+10 m/s)"),
			FLinearColor(0.4f, 1.0f, 0.4f, 0.9f));
		DrawKey(TEXT("[K] Normal      [L] Anti-Normal   (+10 m/s)"),
			FLinearColor(0.6f, 0.4f, 1.0f, 0.9f));
		DrawKey(TEXT("[I] Radial In   [O] Radial Out    (+10 m/s)"),
			FLinearColor(1.0f, 0.6f, 0.3f, 0.9f));

		if (OS->IsManeuverActive())
		{
			DrawKey(FString::Printf(TEXT("Last: %s  %.0f m/s"),
				*OS->GetManeuverLabel(), OS->GetManeuverDV()),
				FLinearColor(1.0f, 0.8f, 0.2f, 1.0f));
		}

		// Propellant remaining
		const FSpacecraftState& MState = OS->GetSpacecraftState();
		if (MState.Mass.PropellantMass_kg > 0)
		{
			DrawKey(FString::Printf(TEXT("Propellant: %.1f kg"), MState.Mass.PropellantMass_kg),
				FLinearColor(0.7f, 0.8f, 0.9f));
		}
	}

	// ── QUICK ORBIT STATS STRIP (center) ──────────────────────────────────
	{
		const double R_m_q = SC.Orbital.Position_m.Size();
		const double Alt_km_q = (R_m_q - EarthR) / 1000.0;
		const double Vel_q = SC.Orbital.Velocity_mps.Size();
		const double Period_min_q = (2.0 * PI * FMath::Sqrt(FMath::Pow(R_m_q, 3.0) / Mu)) / 60.0;

		const float StripY_q = H - 52.0f - 26.0f;
		const float StripH_q = 22.0f;
		FCanvasTileItem StripBG(FVector2D(0, StripY_q), FVector2D(W, StripH_q),
			FLinearColor(0.01f, 0.03f, 0.06f, 0.7f));
		StripBG.BlendMode = SE_BLEND_Translucent;
		Canvas->DrawItem(StripBG);

		FString StripStr = FString::Printf(TEXT("ALT: %.0f km   VEL: %.0f m/s   PERIOD: %.1f min   SIM: %.0f s"),
			Alt_km_q, Vel_q, Period_min_q, SC.SimTime_s);
		float SW_q, SH_q;
		Canvas->TextSize(SmFont, StripStr, SW_q, SH_q);
		FCanvasTextItem Strip(FVector2D((W - SW_q) / 2.0f, StripY_q + 3.0f),
			FText::FromString(StripStr), SmFont, FLinearColor(0.5f, 0.7f, 0.9f, 0.8f));
		Canvas->DrawItem(Strip);
	}

	// ── GROUND TRACK MAP ───────────────────────────────────────────────────
	{
		DrawGroundTrackMap(W - 320.0f, H - 200.0f, 300.0f, 140.0f,
			SC.Orbital.Position_m, EarthR);
	}

	// ── SPACECRAFT RESOURCES ───────────────────────────────────────────────
	{
		const auto& R = GM->SCResources;
		const float PX = W - 320.0f, PY = 440.0f, PW = 300.0f, PH = 300.0f;
		DrawPanel(PX, PY, PW, PH, TEXT("SPACECRAFT RESOURCES"), FLinearColor(0.7f, 0.4f, 1.0f, 1.0f));

		float TY = PY + 30.0f;
		const float LX = PX + 14.0f;

		auto ResBar = [&](const FString& Label, double Val, double Max, const FLinearColor& Col, const FString& Unit)
		{
			FCanvasTextItem LI(FVector2D(LX, TY),
				FText::FromString(Label), SmFont, FLinearColor(0.5f, 0.6f, 0.7f));
			LI.Scale = FVector2D(0.85f, 0.85f);
			Canvas->DrawItem(LI);

			FCanvasTextItem VI(FVector2D(LX + 100.0f, TY),
				FText::FromString(FString::Printf(TEXT("%.1f / %.0f %s"), Val, Max, *Unit)),
				SmFont, Col);
			VI.Scale = FVector2D(0.85f, 0.85f);
			Canvas->DrawItem(VI);

			TY += 15.0f;
			const float BarW = PW - 28.0f, BarH = 8.0f;
			FCanvasTileItem BG(FVector2D(LX, TY), FVector2D(BarW, BarH),
				FLinearColor(0.08f, 0.08f, 0.12f, 0.7f));
			BG.BlendMode = SE_BLEND_Translucent;
			Canvas->DrawItem(BG);
			float Frac = (Max > 0.0) ? static_cast<float>(FMath::Clamp(Val / Max, 0.0, 1.0)) : 0.0f;
			FCanvasTileItem Fill(FVector2D(LX, TY), FVector2D(BarW * Frac, BarH), Col);
			Fill.BlendMode = SE_BLEND_Translucent;
			Canvas->DrawItem(Fill);
			TY += 16.0f;
		};

		// Battery
		FLinearColor BatCol = R.bPowerWarning ? FLinearColor(1.0f, 0.3f, 0.1f) : FLinearColor(0.3f, 0.8f, 1.0f);
		ResBar(TEXT("Battery:"), R.BatteryCharge_Wh, R.BatteryMax_Wh, BatCol, TEXT("Wh"));

		// Power balance
		{
			double NetPower = (R.bInSunlight ? R.PowerGeneration_W : 0.0) - R.PowerDraw_W;
			FLinearColor PCol = (NetPower >= 0.0) ? FLinearColor(0.3f, 1.0f, 0.5f) : FLinearColor(1.0f, 0.5f, 0.2f);
			FCanvasTextItem PwrI(FVector2D(LX, TY),
				FText::FromString(FString::Printf(TEXT("Power: %+.0f W  (Gen %.0f - Draw %.0f)"),
					NetPower, R.bInSunlight ? R.PowerGeneration_W : 0.0, R.PowerDraw_W)),
				SmFont, PCol);
			PwrI.Scale = FVector2D(0.8f, 0.8f);
			Canvas->DrawItem(PwrI);
			TY += 20.0f;
		}

		// Data storage
		ResBar(TEXT("Data:"), R.DataStored_GB, R.DataStorageMax_GB, FLinearColor(1.0f, 0.8f, 0.3f), TEXT("GB"));

		// Data downlinked
		FCanvasTextItem DLI(FVector2D(LX, TY),
			FText::FromString(FString::Printf(TEXT("Downlinked: %.2f GB"), R.DataDownlinked_GB)),
			SmFont, FLinearColor(0.4f, 0.9f, 0.5f));
		DLI.Scale = FVector2D(0.8f, 0.8f);
		Canvas->DrawItem(DLI);
		TY += 20.0f;

		// Thermal
		FLinearColor ThermCol = R.bThermalWarning ? FLinearColor(1.0f, 0.3f, 0.1f) : FLinearColor(0.8f, 0.6f, 0.3f);
		ResBar(TEXT("Thermal:"), R.ThermalLoad_W, R.ThermalCapacity_W, ThermCol, TEXT("W"));

		// Propellant
		double PropMax = (GM->BusCatalog.Num() > 0) ? GM->GetSelectedBus().PropellantMass_kg : 50.0;
		ResBar(TEXT("Propellant:"), R.PropellantRemaining_kg, PropMax, FLinearColor(0.5f, 0.8f, 1.0f), TEXT("kg"));

		// Status indicators
		TY += 4.0f;

		// Sunlight indicator
		{
			FCanvasTileItem SunDot(FVector2D(LX, TY + 3.0f), FVector2D(8.0f, 8.0f),
				R.bInSunlight ? FLinearColor(1.0f, 0.9f, 0.3f) : FLinearColor(0.3f, 0.3f, 0.4f));
			SunDot.BlendMode = SE_BLEND_Translucent;
			Canvas->DrawItem(SunDot);
			FCanvasTextItem SunTxt(FVector2D(LX + 14.0f, TY),
				FText::FromString(TEXT("SUNLIGHT")), SmFont,
				R.bInSunlight ? FLinearColor(1.0f, 0.9f, 0.3f) : FLinearColor(0.3f, 0.3f, 0.4f));
			SunTxt.Scale = FVector2D(0.85f, 0.85f);
			Canvas->DrawItem(SunTxt);

			// Ground contact — same line, offset right
			FCanvasTileItem GndDot(FVector2D(LX + 130.0f, TY + 3.0f), FVector2D(8.0f, 8.0f),
				R.bInGroundContact ? FLinearColor(0.3f, 1.0f, 0.5f) : FLinearColor(0.3f, 0.3f, 0.4f));
			GndDot.BlendMode = SE_BLEND_Translucent;
			Canvas->DrawItem(GndDot);
			FCanvasTextItem GndTxt(FVector2D(LX + 144.0f, TY),
				FText::FromString(TEXT("GROUND CONTACT")), SmFont,
				R.bInGroundContact ? FLinearColor(0.3f, 1.0f, 0.5f) : FLinearColor(0.3f, 0.3f, 0.4f));
			GndTxt.Scale = FVector2D(0.85f, 0.85f);
			Canvas->DrawItem(GndTxt);
		}
		TY += 20.0f;

		// Science score
		FCanvasTextItem SciI(FVector2D(LX, TY),
			FText::FromString(FString::Printf(TEXT("Science: %.1f pts  (x%.1f)"),
				R.ScienceCollected, GM->GetScienceMultiplier())),
			SmFont, FLinearColor(0.8f, 0.6f, 1.0f));
		SciI.Scale = FVector2D(0.85f, 0.85f);
		Canvas->DrawItem(SciI);
	}

	// ── ACTIVE INSTRUMENTS ────────────────────────────────────────────────
	{
		TArray<FInstrumentDefinition> Active = GM->GetActiveInstruments();
		if (Active.Num() > 0)
		{
			const float PX = W - 320.0f, PY = 810.0f, PW = 300.0f;
			const float PH = 30.0f + Active.Num() * 16.0f;
			DrawPanel(PX, PY, PW, PH, TEXT("ACTIVE INSTRUMENTS"), FLinearColor(1.0f, 0.7f, 0.2f, 1.0f));

			float TY = PY + 28.0f;
			const float LX = PX + 14.0f;

			const auto& R = GM->SCResources;
			const double Alt_km = (SC.Orbital.Position_m.Size() - EarthR) / 1000.0;

			for (const FInstrumentDefinition& Inst : Active)
			{
				bool bCanOperate = (Alt_km >= Inst.MinAltitude_km && Alt_km <= Inst.MaxAltitude_km);
				if (Inst.bRequiresSunPointing && !R.bInSunlight) bCanOperate = false;

				FLinearColor ICol = bCanOperate
					? FLinearColor(0.3f, 1.0f, 0.5f) : FLinearColor(0.5f, 0.4f, 0.3f);
				FString StatusStr = bCanOperate ? TEXT("ACTIVE") : TEXT("IDLE");

				FCanvasTextItem II(FVector2D(LX, TY),
					FText::FromString(FString::Printf(TEXT("%s — %s"), *Inst.Name, *StatusStr)),
					SmFont, ICol);
				II.Scale = FVector2D(0.8f, 0.8f);
				Canvas->DrawItem(II);
				TY += 16.0f;
			}
		}
	}

	// ── CONTROLS HINT ──────────────────────────────────────────────────────
	{
		FString Controls = TEXT("[>/<] Time Warp  [Space] Pause  [H/J/K/L/I/O] Maneuvers  [G] Deorbit  [R] Return");
		float CW, CH;
		Canvas->TextSize(SmFont, Controls, CW, CH);
		FCanvasTextItem CI(FVector2D((W - CW) / 2.0f, H - 52.0f),
			FText::FromString(Controls), SmFont, FLinearColor(0.5f, 0.7f, 0.4f, 0.85f));
		Canvas->DrawItem(CI);
	}
}

// ─────────────────────────────────────────────────────────────────────────────
// REENTRY PHASE HUD
// ─────────────────────────────────────────────────────────────────────────────
void AAstralisAssemblyHUD::DrawReentryHUD(float W, float H, AAstralisAssemblyGameMode* GM)
{
	if (!GM) return;
	UFont* BigFont = GEngine->GetLargeFont();
	UFont* SmFont = GEngine->GetSmallFont();

	const double Alt = GM->ReentryAltitude_km;
	const double Vel = GM->ReentryVelocity_mps;
	const double Temp = GM->ReentryTemperature_C;
	const bool bBlackout = GM->bPlasmaBlackout;
	const bool bChute = GM->bParachuteDeployed;
	const bool bSplash = GM->bReentryComplete;

	// ── REENTRY STATUS HEADER ─────────────────────────────────────────────
	{
		FString StatusStr;
		FLinearColor StatusColor;
		if (bSplash)
		{
			StatusStr = TEXT("SPLASHDOWN — RECOVERY COMPLETE");
			StatusColor = FLinearColor(0.2f, 1.0f, 0.4f, 1.0f);
		}
		else if (bChute)
		{
			StatusStr = TEXT("PARACHUTE DEPLOYED — DESCENDING");
			StatusColor = FLinearColor(0.3f, 0.8f, 1.0f, 1.0f);
		}
		else if (bBlackout)
		{
			StatusStr = TEXT("PLASMA BLACKOUT — COMMS LOST");
			StatusColor = FLinearColor(1.0f, 0.3f, 0.1f, 1.0f);
		}
		else if (Alt < 120.0)
		{
			StatusStr = TEXT("ATMOSPHERIC INTERFACE — HEATING");
			StatusColor = FLinearColor(1.0f, 0.6f, 0.1f, 1.0f);
		}
		else
		{
			StatusStr = TEXT("DEORBIT BURN");
			StatusColor = FLinearColor(1.0f, 0.8f, 0.2f, 1.0f);
		}

		float SW, SH;
		Canvas->TextSize(BigFont, StatusStr, SW, SH);
		const float Scale = 1.4f;
		const float BGW = SW * Scale + 40.0f;
		const float BGH = SH * Scale + 12.0f;
		FCanvasTileItem BG(FVector2D((W - BGW) / 2.0f, 52.0f),
			FVector2D(BGW, BGH), FLinearColor(0.02f, 0.03f, 0.08f, 0.88f));
		BG.BlendMode = SE_BLEND_Translucent;
		Canvas->DrawItem(BG);

		FCanvasTextItem SI(FVector2D((W - SW * Scale) / 2.0f, 58.0f),
			FText::FromString(StatusStr), BigFont, StatusColor);
		SI.Scale = FVector2D(Scale, Scale);
		Canvas->DrawItem(SI);
	}

	// ── LEFT: ALTITUDE + VELOCITY GAUGES ──────────────────────────────────
	{
		const float GaugeTop = 120.0f;
		const float GaugeH = H - 260.0f;

		DrawVerticalGauge(22.0f, GaugeTop, 60.0f, GaugeH,
			Alt, 0.0, 450.0,
			TEXT("ALTITUDE"), TEXT("km"),
			FLinearColor(0.2f, 0.7f, 1.0f, 1.0f));

		DrawVerticalGauge(100.0f, GaugeTop, 60.0f, GaugeH,
			Vel, 0.0, 8000.0,
			TEXT("VELOCITY"), TEXT("m/s"),
			FLinearColor(0.3f, 1.0f, 0.4f, 1.0f));
	}

	// ── RIGHT: REENTRY DATA PANEL ─────────────────────────────────────────
	{
		const float PX = W - 280.0f, PY = 120.0f, PW = 260.0f, PH = 340.0f;
		DrawPanel(PX, PY, PW, PH, TEXT("REENTRY DATA"), FLinearColor(1.0f, 0.4f, 0.1f, 1.0f));

		float TY = PY + 30.0f;
		const float LX = PX + 14.0f;
		const float Step = 22.0f;

		auto DrawLine = [&](const FString& Label, const FString& Value,
			const FLinearColor& ValColor = FLinearColor::White)
		{
			FCanvasTextItem LI(FVector2D(LX, TY), FText::FromString(Label), SmFont,
				FLinearColor(0.5f, 0.6f, 0.7f, 0.9f));
			Canvas->DrawItem(LI);
			FCanvasTextItem VI(FVector2D(LX + 110.0f, TY), FText::FromString(Value), SmFont, ValColor);
			Canvas->DrawItem(VI);
			TY += Step;
		};

		DrawLine(TEXT("Altitude"), FString::Printf(TEXT("%.1f km"), Alt));
		DrawLine(TEXT("Velocity"), FString::Printf(TEXT("%.0f m/s"), Vel));

		// Mach number (speed of sound ~343 m/s at sea level, varies)
		const double Mach = Vel / 343.0;
		FLinearColor MachColor = (Mach > 25.0) ? FLinearColor(1.0f, 0.2f, 0.1f, 1.0f)
			: (Mach > 5.0) ? FLinearColor(1.0f, 0.6f, 0.1f, 1.0f)
			: FLinearColor(0.3f, 1.0f, 0.4f, 1.0f);
		DrawLine(TEXT("Mach"), FString::Printf(TEXT("%.1f"), Mach), MachColor);

		// Temperature
		FLinearColor TempColor;
		if (Temp > 1500.0)
			TempColor = FLinearColor(1.0f, 0.2f, 0.1f, 1.0f);
		else if (Temp > 500.0)
			TempColor = FLinearColor(1.0f, 0.6f, 0.1f, 1.0f);
		else
			TempColor = FLinearColor(0.3f, 1.0f, 0.5f, 1.0f);
		DrawLine(TEXT("Heat Shield"), FString::Printf(TEXT("%.0f C"), Temp), TempColor);

		TY += 6.0f;

		// Heat shield bar
		DrawProgressBar(LX, TY, PW - 28.0f, 16.0f,
			Temp, 2200.0, TempColor,
			FString::Printf(TEXT("%.0f / 2200 C"), Temp));
		TY += 24.0f;

		// Communications status
		FLinearColor CommsColor = bBlackout
			? FLinearColor(1.0f, 0.2f, 0.1f, 1.0f)
			: FLinearColor(0.2f, 1.0f, 0.4f, 1.0f);
		DrawLine(TEXT("Comms"), bBlackout ? TEXT("BLACKOUT") : TEXT("NOMINAL"), CommsColor);

		// G-force estimate
		const double GForce = (Vel > 100.0 && Alt > 10.0 && Alt < 80.0) ?
			FMath::Clamp(1.0 + (7670.0 - Vel) * 0.001, 1.0, 8.0) : 1.0;
		FLinearColor GColor = (GForce > 4.0) ? FLinearColor(1.0f, 0.3f, 0.1f, 1.0f)
			: FLinearColor(0.3f, 1.0f, 0.5f, 1.0f);
		DrawLine(TEXT("G-Force"), FString::Printf(TEXT("%.1f G"), GForce), GColor);

		// Phase
		TY += 6.0f;
		FString Phase;
		if (bSplash) Phase = TEXT("RECOVERY");
		else if (bChute) Phase = TEXT("PARACHUTE DESCENT");
		else if (bBlackout) Phase = TEXT("PLASMA BLACKOUT");
		else if (Alt < 120.0) Phase = TEXT("ATMOSPHERIC ENTRY");
		else Phase = TEXT("DEORBIT BURN");
		DrawLine(TEXT("Phase"), Phase, FLinearColor(0.9f, 0.8f, 0.3f, 1.0f));
	}

	// ── CENTER: REENTRY HEATING VISUALIZATION ─────────────────────────────
	if (!bSplash && Alt < 120.0 && Alt > 10.0 && Temp > 100.0)
	{
		const float EffW = 300.0f;
		const float EffH = 100.0f;
		const float EffX = (W - EffW) / 2.0f;
		const float EffY = H / 2.0f - 60.0f;

		const float HeatAlpha = static_cast<float>(FMath::Clamp(Temp / 2200.0, 0.0, 1.0));

		// Heating glow effect
		FCanvasTileItem Glow(FVector2D(EffX, EffY), FVector2D(EffW, EffH),
			FLinearColor(1.0f, 0.3f * (1.0f - HeatAlpha), 0.0f, HeatAlpha * 0.4f));
		Glow.BlendMode = SE_BLEND_Translucent;
		Canvas->DrawItem(Glow);

		// Inner intense zone
		const float InnerW = EffW * 0.6f;
		FCanvasTileItem Inner(FVector2D((W - InnerW) / 2.0f, EffY + 20.0f),
			FVector2D(InnerW, EffH - 40.0f),
			FLinearColor(1.0f, 0.8f * (1.0f - HeatAlpha), 0.0f, HeatAlpha * 0.6f));
		Inner.BlendMode = SE_BLEND_Translucent;
		Canvas->DrawItem(Inner);

		// Temperature readout in center
		FString TempStr = FString::Printf(TEXT("%.0f C"), Temp);
		float TW_t, TH_t;
		Canvas->TextSize(BigFont, TempStr, TW_t, TH_t);
		FCanvasTextItem TI(FVector2D((W - TW_t) / 2.0f, EffY + EffH / 2.0f - TH_t / 2.0f),
			FText::FromString(TempStr), BigFont,
			FLinearColor(1.0f, 1.0f - HeatAlpha * 0.5f, 0.2f, 1.0f));
		Canvas->DrawItem(TI);
	}

	// ── REENTRY EDGE GLOW — red/orange vignette during heating ───────────
	if (!bSplash && Alt < 110.0 && Alt > 5.0 && Vel > 500.0)
	{
		const float Intensity = static_cast<float>(FMath::Clamp(Temp / 2200.0, 0.0, 1.0));
		const float EdgeW = W * 0.12f;
		FLinearColor GlowCol(1.0f, 0.3f * (1.0f - Intensity), 0.0f, Intensity * 0.35f);
		FCanvasTileItem EL(FVector2D(0, 0), FVector2D(EdgeW, H), GlowCol);
		EL.BlendMode = SE_BLEND_Translucent; Canvas->DrawItem(EL);
		FCanvasTileItem ER(FVector2D(W - EdgeW, 0), FVector2D(EdgeW, H), GlowCol);
		ER.BlendMode = SE_BLEND_Translucent; Canvas->DrawItem(ER);
		FCanvasTileItem ET(FVector2D(0, 0), FVector2D(W, EdgeW * 0.5f), GlowCol);
		ET.BlendMode = SE_BLEND_Translucent; Canvas->DrawItem(ET);
		FCanvasTileItem EB(FVector2D(0, H - EdgeW * 0.5f), FVector2D(W, EdgeW * 0.5f), GlowCol);
		EB.BlendMode = SE_BLEND_Translucent; Canvas->DrawItem(EB);
	}

	// ── SPLASHDOWN COUNTDOWN ──────────────────────────────────────────────
	if (bChute && !bSplash && Alt > 0.5)
	{
		const double DescentRate_kmps = 0.008;
		const double ETA_s = Alt / DescentRate_kmps;
		const int32 ETAmin = static_cast<int32>(ETA_s / 60.0);
		const int32 ETAsec = static_cast<int32>(FMath::Fmod(ETA_s, 60.0));

		FString ETAStr = FString::Printf(TEXT("SPLASHDOWN ETA: %dm %02ds"), ETAmin, ETAsec);
		float ETAW, ETAH;
		Canvas->TextSize(SmFont, ETAStr, ETAW, ETAH);
		FCanvasTileItem ETABG(FVector2D((W - ETAW - 20.0f) / 2.0f, H / 2.0f + 20.0f),
			FVector2D(ETAW + 20.0f, ETAH + 10.0f),
			FLinearColor(0.02f, 0.04f, 0.08f, 0.8f));
		ETABG.BlendMode = SE_BLEND_Translucent;
		Canvas->DrawItem(ETABG);
		FCanvasTextItem ETAI(FVector2D((W - ETAW) / 2.0f, H / 2.0f + 25.0f),
			FText::FromString(ETAStr), SmFont,
			FLinearColor(0.3f, 0.8f, 1.0f, 0.9f));
		Canvas->DrawItem(ETAI);
	}

	// ── PLASMA BLACKOUT OVERLAY ───────────────────────────────────────────
	if (bBlackout)
	{
		const double T = GetWorld()->GetTimeSeconds();
		const float Flicker = 0.7f + 0.3f * static_cast<float>(FMath::Sin(T * 8.0) * FMath::Sin(T * 13.0));

		FCanvasTileItem Static(FVector2D(0, 0), FVector2D(W, H),
			FLinearColor(0.0f, 0.0f, 0.0f, 0.3f * Flicker));
		Static.BlendMode = SE_BLEND_Translucent;
		Canvas->DrawItem(Static);

		FString BlackoutStr = TEXT("/// SIGNAL LOST — PLASMA BLACKOUT ///");
		float BW, BH;
		Canvas->TextSize(BigFont, BlackoutStr, BW, BH);
		FCanvasTextItem BI(FVector2D((W - BW) / 2.0f, H / 2.0f + 80.0f),
			FText::FromString(BlackoutStr), BigFont,
			FLinearColor(1.0f, 0.3f, 0.1f, Flicker));
		Canvas->DrawItem(BI);
	}

	// ── DESCENT PROFILE (left side) ──────────────────────────────────────
	if (!bSplash && Alt > 1.0)
	{
		const float PX = 180.0f, PY = 120.0f, PW = 160.0f, PH_prof = H - 260.0f;
		DrawPanel(PX, PY, PW, PH_prof, TEXT("DESCENT"), FLinearColor(0.5f, 0.5f, 0.7f, 0.8f));

		struct FPhaseMarker { const TCHAR* Name; double Alt_km; FLinearColor Color; };
		const FPhaseMarker Phases[] = {
			{TEXT("Entry IF"), 120.0, FLinearColor(1.0f, 0.5f, 0.1f)},
			{TEXT("Blackout"), 80.0, FLinearColor(1.0f, 0.2f, 0.1f)},
			{TEXT("Peak Heat"), 50.0, FLinearColor(1.0f, 0.8f, 0.0f)},
			{TEXT("End BO"), 40.0, FLinearColor(0.3f, 0.9f, 0.3f)},
			{TEXT("Drogue"), 25.0, FLinearColor(0.3f, 0.7f, 1.0f)},
			{TEXT("Main"), 10.0, FLinearColor(0.3f, 0.8f, 1.0f)},
			{TEXT("Splash"), 0.0, FLinearColor(0.2f, 1.0f, 0.3f)},
		};

		const float BarX = PX + 10.0f, BarY = PY + 28.0f;
		const float BarW = 8.0f;
		const float BarH = PH_prof - 38.0f;

		FCanvasTileItem Track(FVector2D(BarX, BarY), FVector2D(BarW, BarH),
			FLinearColor(0.1f, 0.15f, 0.2f, 0.7f));
		Track.BlendMode = SE_BLEND_Translucent;
		Canvas->DrawItem(Track);

		for (const FPhaseMarker& P : Phases)
		{
			const float Frac = static_cast<float>(1.0 - FMath::Clamp(P.Alt_km / 130.0, 0.0, 1.0));
			const float MY = BarY + BarH * Frac;
			FCanvasTileItem Marker(FVector2D(BarX, MY), FVector2D(BarW, 2.0f), P.Color);
			Marker.BlendMode = SE_BLEND_Translucent;
			Canvas->DrawItem(Marker);

			FCanvasTextItem Label(FVector2D(BarX + BarW + 4.0f, MY - 6.0f),
				FText::FromString(FString::Printf(TEXT("%s %.0fkm"), P.Name, P.Alt_km)),
				SmFont, P.Color);
			Label.Scale = FVector2D(0.7f, 0.7f);
			Canvas->DrawItem(Label);
		}

		// Current position indicator
		const float CurFrac = static_cast<float>(1.0 - FMath::Clamp(Alt / 130.0, 0.0, 1.0));
		const float CurY = BarY + BarH * CurFrac;
		FCanvasTileItem CurMark(FVector2D(BarX - 4.0f, CurY - 3.0f), FVector2D(BarW + 8.0f, 6.0f),
			FLinearColor(1.0f, 1.0f, 1.0f, 0.9f));
		CurMark.BlendMode = SE_BLEND_Translucent;
		Canvas->DrawItem(CurMark);
	}

	// ── SPLASHDOWN SUCCESS OVERLAY — FINAL MISSION REPORT ────────────────
	if (bSplash)
	{
		const float OW = 520.0f, OH = 520.0f;
		const float OX = (W - OW) / 2.0f, OY = FMath::Max(20.0f, H / 2.0f - OH / 2.0f);
		DrawPanel(OX, OY, OW, OH, TEXT(""), FLinearColor(0.1f, 1.0f, 0.3f, 1.0f));

		// Comprehensive score
		const auto CS = GM->ComputeComprehensiveScore();

		FCanvasTextItem Suc(FVector2D(OX + 20.0f, OY + 12.0f),
			FText::FromString(TEXT("MISSION COMPLETE")), BigFont,
			FLinearColor(0.1f, 1.0f, 0.3f, 1.0f));
		Suc.Scale = FVector2D(1.4f, 1.4f);
		Canvas->DrawItem(Suc);

		// Overall grade and score
		{
			FString GradeStr = FString::Printf(TEXT("%s  %.0f/150"), *CS.GetGrade(), CS.GetTotal());
			FLinearColor GradeCol;
			if (CS.GetTotal() >= 125.0) GradeCol = FLinearColor(1.0f, 0.85f, 0.0f);
			else if (CS.GetTotal() >= 95.0) GradeCol = FLinearColor(0.2f, 1.0f, 0.4f);
			else if (CS.GetTotal() >= 60.0) GradeCol = FLinearColor(0.3f, 0.8f, 1.0f);
			else GradeCol = FLinearColor(1.0f, 0.5f, 0.2f);
			float GW_t, GH_t;
			Canvas->TextSize(BigFont, GradeStr, GW_t, GH_t);
			FCanvasTextItem GI(FVector2D(OX + OW - GW_t - 24.0f, OY + 16.0f),
				FText::FromString(GradeStr), BigFont, GradeCol);
			Canvas->DrawItem(GI);
		}

		float TY = OY + 54.0f;
		const float LX = OX + 24.0f;
		const float Step = 18.0f;

		auto ReportLine = [&](const FString& Label, const FString& Value, const FLinearColor& Col = FLinearColor::White)
		{
			FCanvasTextItem LI(FVector2D(LX, TY), FText::FromString(Label), SmFont,
				FLinearColor(0.5f, 0.6f, 0.7f));
			LI.Scale = FVector2D(0.9f, 0.9f);
			Canvas->DrawItem(LI);
			FCanvasTextItem VI(FVector2D(LX + 170.0f, TY), FText::FromString(Value), SmFont, Col);
			VI.Scale = FVector2D(0.9f, 0.9f);
			Canvas->DrawItem(VI);
			TY += Step;
		};

		ReportLine(TEXT("Mission"), GM->GetSelectedMission().Name, FLinearColor(0.5f, 0.85f, 1.0f));
		if (GM->BusCatalog.Num() > 0)
			ReportLine(TEXT("Spacecraft"), GM->GetSelectedBus().Name, FLinearColor(0.7f, 0.5f, 1.0f));
		TY += 4.0f;

		// Science results
		const auto& R = GM->SCResources;
		ReportLine(TEXT("Science Collected"), FString::Printf(TEXT("%.1f points"), R.ScienceCollected),
			FLinearColor(0.8f, 0.6f, 1.0f));
		ReportLine(TEXT("Data Downlinked"), FString::Printf(TEXT("%.2f GB"), R.DataDownlinked_GB),
			FLinearColor(0.4f, 0.9f, 0.5f));
		ReportLine(TEXT("Mission Duration"), FString::Printf(TEXT("%.0f orbits"), R.MissionTime_s / 5400.0),
			FLinearColor(0.5f, 0.8f, 1.0f));
		TY += 4.0f;

		// Budget summary
		if (GM->BusCatalog.Num() > 0)
		{
			const double Spent = GM->Budget.GetTotalSpent();
			const double Remaining = GM->Budget.GetRemaining();
			FLinearColor BudCol = GM->Budget.IsOverBudget()
				? FLinearColor(1.0f, 0.3f, 0.1f) : FLinearColor(0.3f, 1.0f, 0.5f);
			ReportLine(TEXT("Budget Spent"), FString::Printf(TEXT("$%.0fM / $%.0fM"),
				Spent, GM->Budget.TotalBudget_M), BudCol);
			if (Remaining > 0)
				ReportLine(TEXT("Under Budget"), FString::Printf(TEXT("$%.0fM saved"), Remaining),
					FLinearColor(0.3f, 1.0f, 0.5f));
		}
		TY += 4.0f;

		// Instruments deployed
		TArray<FInstrumentDefinition> Active = GM->GetActiveInstruments();
		if (Active.Num() > 0)
		{
			FCanvasTextItem InstTitle(FVector2D(LX, TY),
				FText::FromString(FString::Printf(TEXT("Instruments Deployed: %d"), Active.Num())),
				SmFont, FLinearColor(1.0f, 0.7f, 0.2f));
			InstTitle.Scale = FVector2D(0.9f, 0.9f);
			Canvas->DrawItem(InstTitle);
			TY += Step;

			for (const FInstrumentDefinition& Inst : Active)
			{
				FCanvasTextItem II(FVector2D(LX + 12.0f, TY),
					FText::FromString(FString::Printf(TEXT("• %s"), *Inst.Name)),
					SmFont, FLinearColor(0.6f, 0.7f, 0.8f));
				II.Scale = FVector2D(0.8f, 0.8f);
				Canvas->DrawItem(II);
				TY += 14.0f;
			}
		}

		// Comprehensive score breakdown
		TY += 6.0f;
		{
			FCanvasTileItem ScoreSep(FVector2D(LX, TY), FVector2D(OW - 48.0f, 1.0f),
				FLinearColor(0.3f, 0.5f, 0.7f, 0.4f));
			ScoreSep.BlendMode = SE_BLEND_Translucent;
			Canvas->DrawItem(ScoreSep);
			TY += 6.0f;

			FCanvasTextItem ScTitle(FVector2D(LX, TY),
				FText::FromString(TEXT("COMPREHENSIVE SCORE")), SmFont,
				FLinearColor(1.0f, 0.85f, 0.2f));
			ScTitle.Scale = FVector2D(0.95f, 0.95f);
			Canvas->DrawItem(ScTitle);
			TY += Step;

			auto ScoreLine = [&](const FString& Label, double Val, double Max, const FLinearColor& Col)
			{
				FCanvasTextItem SLI(FVector2D(LX, TY), FText::FromString(Label), SmFont,
					FLinearColor(0.5f, 0.6f, 0.7f));
				SLI.Scale = FVector2D(0.85f, 0.85f);
				Canvas->DrawItem(SLI);
				FCanvasTextItem SVI(FVector2D(LX + 170.0f, TY),
					FText::FromString(FString::Printf(TEXT("%.1f / %.0f"), Val, Max)), SmFont, Col);
				SVI.Scale = FVector2D(0.85f, 0.85f);
				Canvas->DrawItem(SVI);
				TY += 15.0f;
			};

			ScoreLine(TEXT("Launch Performance"), CS.LaunchScore, 100.0, FLinearColor(0.3f, 0.8f, 1.0f));
			ScoreLine(TEXT("Budget Efficiency"), CS.BudgetBonus, 15.0, FLinearColor(0.3f, 1.0f, 0.5f));
			ScoreLine(TEXT("Science Return"), CS.ScienceBonus, 15.0, FLinearColor(0.8f, 0.6f, 1.0f));
			ScoreLine(TEXT("Design Compatibility"), CS.DesignBonus, 10.0, FLinearColor(1.0f, 0.7f, 0.2f));
			ScoreLine(TEXT("Engineering Margin"), CS.EfficiencyBonus, 10.0, FLinearColor(0.5f, 0.8f, 1.0f));

			// Total bar
			TY += 4.0f;
			const float BarX_s = LX, BarW_s = OW - 48.0f, BarH_s = 12.0f;
			FCanvasTileItem TotalBG(FVector2D(BarX_s, TY), FVector2D(BarW_s, BarH_s),
				FLinearColor(0.08f, 0.08f, 0.12f, 0.8f));
			TotalBG.BlendMode = SE_BLEND_Translucent;
			Canvas->DrawItem(TotalBG);
			float TotalFrac = static_cast<float>(FMath::Clamp(CS.GetTotal() / 150.0, 0.0, 1.0));
			FLinearColor TotalBarCol = (TotalFrac > 0.83f) ? FLinearColor(1.0f, 0.85f, 0.0f)
				: (TotalFrac > 0.6f) ? FLinearColor(0.2f, 0.8f, 0.4f)
				: FLinearColor(1.0f, 0.5f, 0.2f);
			FCanvasTileItem TotalFill(FVector2D(BarX_s, TY), FVector2D(BarW_s * TotalFrac, BarH_s), TotalBarCol);
			TotalFill.BlendMode = SE_BLEND_Translucent;
			Canvas->DrawItem(TotalFill);
			TY += BarH_s + 4.0f;

			FCanvasTextItem TotalTxt(FVector2D(LX, TY),
				FText::FromString(FString::Printf(TEXT("TOTAL: %.0f / 150 — Grade: %s"), CS.GetTotal(), *CS.GetGrade())),
				SmFont, TotalBarCol);
			Canvas->DrawItem(TotalTxt);
			TY += 20.0f;
		}

		FCanvasTextItem Ret(FVector2D(LX, TY),
			FText::FromString(TEXT("Returning to assembly...")), SmFont,
			FLinearColor(0.3f, 0.8f, 1.0f, 0.8f));
		Canvas->DrawItem(Ret);
	}

	// ── CONTROLS HINT ─────────────────────────────────────────────────────
	if (!bSplash)
	{
		FString Controls = TEXT("[R] Skip to Assembly");
		float CW_t, CH_t;
		Canvas->TextSize(SmFont, Controls, CW_t, CH_t);
		FCanvasTextItem CI(FVector2D((W - CW_t) / 2.0f, H - 52.0f),
			FText::FromString(Controls), SmFont, FLinearColor(0.5f, 0.7f, 0.4f, 0.85f));
		Canvas->DrawItem(CI);
	}
}

// ─────────────────────────────────────────────────────────────────────────────
// GROUND TRACK MINI-MAP
// ─────────────────────────────────────────────────────────────────────────────
void AAstralisAssemblyHUD::DrawGroundTrackMap(float X, float Y, float MW, float MH,
	const FVector3d& Position_m, double EarthR)
{
	DrawPanel(X, Y, MW, MH, TEXT("GROUND TRACK"), FLinearColor(0.2f, 0.5f, 0.8f, 1.0f));

	UFont* Fn = GEngine->GetSmallFont();

	const float MapX = X + 4.0f, MapY = Y + 24.0f;
	const float MapW = MW - 8.0f, MapH = MH - 32.0f;

	// Map background (dark blue ocean)
	FCanvasTileItem Ocean(FVector2D(MapX, MapY), FVector2D(MapW, MapH),
		FLinearColor(0.02f, 0.06f, 0.12f, 0.9f));
	Ocean.BlendMode = SE_BLEND_Translucent;
	Canvas->DrawItem(Ocean);

	// Grid lines (30-degree intervals)
	for (int32 i = 1; i < 6; ++i)
	{
		float GY = MapY + (MapH / 6.0f) * i;
		FCanvasTileItem GLine(FVector2D(MapX, GY), FVector2D(MapW, 1.0f),
			FLinearColor(0.15f, 0.2f, 0.3f, 0.4f));
		GLine.BlendMode = SE_BLEND_Translucent;
		Canvas->DrawItem(GLine);
	}
	for (int32 i = 1; i < 12; ++i)
	{
		float GX = MapX + (MapW / 12.0f) * i;
		FCanvasTileItem GLine(FVector2D(GX, MapY), FVector2D(1.0f, MapH),
			FLinearColor(0.15f, 0.2f, 0.3f, 0.4f));
		GLine.BlendMode = SE_BLEND_Translucent;
		Canvas->DrawItem(GLine);
	}

	// Simplified continent outlines (rectangles approximating landmasses)
	auto DrawLand = [&](float Lat1, float Lon1, float Lat2, float Lon2)
	{
		float PX1 = MapX + (Lon1 + 180.0f) / 360.0f * MapW;
		float PY1 = MapY + (90.0f - Lat2) / 180.0f * MapH;
		float PX2 = MapX + (Lon2 + 180.0f) / 360.0f * MapW;
		float PY2 = MapY + (90.0f - Lat1) / 180.0f * MapH;
		FCanvasTileItem Land(FVector2D(PX1, PY1), FVector2D(PX2 - PX1, PY2 - PY1),
			FLinearColor(0.1f, 0.18f, 0.08f, 0.6f));
		Land.BlendMode = SE_BLEND_Translucent;
		Canvas->DrawItem(Land);
	};

	// Major landmasses (rough bounding boxes)
	DrawLand(25.0f, -130.0f, 50.0f, -65.0f);   // North America
	DrawLand(-55.0f, -80.0f, 12.0f, -35.0f);    // South America
	DrawLand(35.0f, -10.0f, 60.0f, 55.0f);       // Europe
	DrawLand(-35.0f, 10.0f, 35.0f, 55.0f);        // Africa
	DrawLand(10.0f, 60.0f, 55.0f, 145.0f);        // Asia
	DrawLand(-45.0f, 112.0f, -10.0f, 155.0f);     // Australia

	// Spacecraft position dot
	const double Lat = FMath::RadiansToDegrees(FMath::Asin(FMath::Clamp(Position_m.Z / FMath::Max(1.0, Position_m.Size()), -1.0, 1.0)));
	const double Lon = FMath::RadiansToDegrees(FMath::Atan2(Position_m.Y, Position_m.X));
	const float DotX = MapX + static_cast<float>((Lon + 180.0) / 360.0) * MapW;
	const float DotY = MapY + static_cast<float>((90.0 - Lat) / 180.0) * MapH;

	// Dot with glow
	FCanvasTileItem Glow(FVector2D(DotX - 6.0f, DotY - 6.0f), FVector2D(12.0f, 12.0f),
		FLinearColor(0.0f, 0.5f, 1.0f, 0.3f));
	Glow.BlendMode = SE_BLEND_Translucent;
	Canvas->DrawItem(Glow);

	FCanvasTileItem Dot(FVector2D(DotX - 3.0f, DotY - 3.0f), FVector2D(6.0f, 6.0f),
		FLinearColor(0.2f, 0.8f, 1.0f, 1.0f));
	Dot.BlendMode = SE_BLEND_Translucent;
	Canvas->DrawItem(Dot);

	// Lat/Lon label
	FString PosLabel = FString::Printf(TEXT("%.1f, %.1f"), Lat, Lon);
	FCanvasTextItem PosItem(FVector2D(MapX + 4.0f, MapY + MapH - 14.0f),
		FText::FromString(PosLabel), Fn, FLinearColor(0.4f, 0.7f, 1.0f, 0.7f));
	PosItem.Scale = FVector2D(0.8f, 0.8f);
	Canvas->DrawItem(PosItem);
}

// ─────────────────────────────────────────────────────────────────────────────
// MAIN DRAW
// ─────────────────────────────────────────────────────────────────────────────
void AAstralisAssemblyHUD::DrawHUD()
{
	Super::DrawHUD();
	if (!Canvas) return;

	const float W = Canvas->SizeX;
	const float H = Canvas->SizeY;

	DrawTitleBar(W, H);
	DrawBottomBar(W, H);

	AAstralisAssemblyGameMode* GM = Cast<AAstralisAssemblyGameMode>(GetWorld()->GetAuthGameMode());
	if (!GM) return;

	switch (CurrentPhase)
	{
	case EAstralisGamePhase::Assembly: DrawAssemblyHUD(W, H, GM); break;
	case EAstralisGamePhase::Launch:   DrawLaunchHUD(W, H, GM); break;
	case EAstralisGamePhase::Orbit:    DrawOrbitHUD(W, H, GM); break;
	case EAstralisGamePhase::Reentry:  DrawReentryHUD(W, H, GM); break;
	}
}
