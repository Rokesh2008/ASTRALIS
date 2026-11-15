#include "AstralisAssemblyPC.h"
#include "AstralisAssemblyGameMode.h"
#include "AstralisAssemblyPawn.h"
#include "Astralis/Spacecraft/AstralisComponentActor.h"
#include "Astralis/Presentation/AstralisCoordinateService.h"
#include "Astralis/Orbit/AstralisOrbitSceneActor.h"
#include "Astralis/UI/AstralisAssemblyHUD.h"
#include "AstralisData/ComponentCatalog/AstralisComponentTypes.h"
#include "Astralis/LaunchVehicle/AstralisLaunchVehicleActor.h"
#include "Engine/World.h"

#if WITH_EDITOR
#include "Editor.h"
#include "EditorViewportClient.h"
#include "LevelEditorViewport.h"
#endif

AAstralisAssemblyPC::AAstralisAssemblyPC()
{
	bShowMouseCursor = true;
	bEnableClickEvents = true;
	bEnableMouseOverEvents = true;
	PrimaryActorTick.bCanEverTick = true;
}

void AAstralisAssemblyPC::SetupInputComponent()
{
	Super::SetupInputComponent();

	InputComponent->BindKey(EKeys::LeftMouseButton, IE_Pressed, this, &AAstralisAssemblyPC::OnLeftClick);
	InputComponent->BindKey(EKeys::RightMouseButton, IE_Pressed, this, &AAstralisAssemblyPC::OnRightClick);

	// Launch / return
	InputComponent->BindKey(EKeys::F5, IE_Pressed, this, &AAstralisAssemblyPC::OnLaunchPressed);
	// Esc is reserved by Play-In-Editor to end the session
	InputComponent->BindKey(EKeys::R, IE_Pressed, this, &AAstralisAssemblyPC::OnReturnPressed);

	// Time warp (Period = >, Comma = <)
	InputComponent->BindKey(EKeys::Period, IE_Pressed, this, &AAstralisAssemblyPC::OnTimeWarpUp);
	InputComponent->BindKey(EKeys::Comma, IE_Pressed, this, &AAstralisAssemblyPC::OnTimeWarpDown);
	InputComponent->BindKey(EKeys::SpaceBar, IE_Pressed, this, &AAstralisAssemblyPC::OnPauseToggle);

	// Engine swap
	InputComponent->BindKey(EKeys::E, IE_Pressed, this, &AAstralisAssemblyPC::OnEngineSwapToggle);
	InputComponent->BindKey(EKeys::Tab, IE_Pressed, this, &AAstralisAssemblyPC::OnSwapStageToggle);
	InputComponent->BindKey(EKeys::One, IE_Pressed, this, &AAstralisAssemblyPC::OnSwapEngine1);
	InputComponent->BindKey(EKeys::Two, IE_Pressed, this, &AAstralisAssemblyPC::OnSwapEngine2);
	InputComponent->BindKey(EKeys::Three, IE_Pressed, this, &AAstralisAssemblyPC::OnSwapEngine3);
	InputComponent->BindKey(EKeys::Four, IE_Pressed, this, &AAstralisAssemblyPC::OnSwapEngine4);
	InputComponent->BindKey(EKeys::Five, IE_Pressed, this, &AAstralisAssemblyPC::OnSwapEngine5);
	InputComponent->BindKey(EKeys::Six, IE_Pressed, this, &AAstralisAssemblyPC::OnSwapEngine6);
	InputComponent->BindKey(EKeys::Seven, IE_Pressed, this, &AAstralisAssemblyPC::OnSwapEngine7);
	InputComponent->BindKey(EKeys::Eight, IE_Pressed, this, &AAstralisAssemblyPC::OnSwapEngine8);

	// Vehicle / mission selection (assembly phase)
	InputComponent->BindKey(EKeys::V, IE_Pressed, this, &AAstralisAssemblyPC::OnVehicleNext);
	InputComponent->BindKey(EKeys::B, IE_Pressed, this, &AAstralisAssemblyPC::OnVehiclePrev);
	InputComponent->BindKey(EKeys::M, IE_Pressed, this, &AAstralisAssemblyPC::OnMissionNext);
	InputComponent->BindKey(EKeys::N, IE_Pressed, this, &AAstralisAssemblyPC::OnMissionPrev);

	// Deorbit / reentry
	InputComponent->BindKey(EKeys::G, IE_Pressed, this, &AAstralisAssemblyPC::OnDeorbitBurn);

	// Throttle control & abort
	InputComponent->BindKey(EKeys::Up, IE_Pressed, this, &AAstralisAssemblyPC::OnThrottleUp);
	InputComponent->BindKey(EKeys::Down, IE_Pressed, this, &AAstralisAssemblyPC::OnThrottleDown);
	InputComponent->BindKey(EKeys::BackSpace, IE_Pressed, this, &AAstralisAssemblyPC::OnAbortLaunch);
	InputComponent->BindKey(EKeys::C, IE_Pressed, this, &AAstralisAssemblyPC::OnCycleCamera);

	// Spacecraft design (assembly phase)
	InputComponent->BindKey(EKeys::P, IE_Pressed, this, &AAstralisAssemblyPC::OnBusNext);
	InputComponent->BindKey(EKeys::Q, IE_Pressed, this, &AAstralisAssemblyPC::OnBusPrev);
	InputComponent->BindKey(EKeys::F1, IE_Pressed, this, &AAstralisAssemblyPC::OnInstrument1);
	InputComponent->BindKey(EKeys::F2, IE_Pressed, this, &AAstralisAssemblyPC::OnInstrument2);
	InputComponent->BindKey(EKeys::F3, IE_Pressed, this, &AAstralisAssemblyPC::OnInstrument3);
	InputComponent->BindKey(EKeys::F4, IE_Pressed, this, &AAstralisAssemblyPC::OnInstrument4);
	InputComponent->BindKey(EKeys::F6, IE_Pressed, this, &AAstralisAssemblyPC::OnInstrument5);
	InputComponent->BindKey(EKeys::F7, IE_Pressed, this, &AAstralisAssemblyPC::OnInstrument6);
	InputComponent->BindKey(EKeys::F8, IE_Pressed, this, &AAstralisAssemblyPC::OnInstrument7);
	InputComponent->BindKey(EKeys::F9, IE_Pressed, this, &AAstralisAssemblyPC::OnInstrument8);
	InputComponent->BindKey(EKeys::F10, IE_Pressed, this, &AAstralisAssemblyPC::OnInstrument9);
	InputComponent->BindKey(EKeys::F11, IE_Pressed, this, &AAstralisAssemblyPC::OnInstrument10);

	// Orbit maneuvers
	InputComponent->BindKey(EKeys::H, IE_Pressed, this, &AAstralisAssemblyPC::OnManeuverPrograde);
	InputComponent->BindKey(EKeys::J, IE_Pressed, this, &AAstralisAssemblyPC::OnManeuverRetrograde);
	InputComponent->BindKey(EKeys::K, IE_Pressed, this, &AAstralisAssemblyPC::OnManeuverNormal);
	InputComponent->BindKey(EKeys::L, IE_Pressed, this, &AAstralisAssemblyPC::OnManeuverAntiNormal);
	InputComponent->BindKey(EKeys::I, IE_Pressed, this, &AAstralisAssemblyPC::OnManeuverRadialIn);
	InputComponent->BindKey(EKeys::O, IE_Pressed, this, &AAstralisAssemblyPC::OnManeuverRadialOut);
}

static void ForceEditorLitMode()
{
#if WITH_EDITOR
	if (GCurrentLevelEditingViewportClient)
	{
		if (GCurrentLevelEditingViewportClient->GetViewMode() != VMI_Lit)
			GCurrentLevelEditingViewportClient->SetViewMode(VMI_Lit);
	}
	if (GLastKeyLevelEditingViewportClient)
	{
		if (GLastKeyLevelEditingViewportClient->GetViewMode() != VMI_Lit)
			GLastKeyLevelEditingViewportClient->SetViewMode(VMI_Lit);
	}

	if (GEditor)
	{
		for (FEditorViewportClient* VC : GEditor->GetAllViewportClients())
		{
			if (VC && VC->GetViewMode() != VMI_Lit)
			{
				VC->SetViewMode(VMI_Lit);
			}
		}
	}
#endif

	if (GEngine && GEngine->GameViewport)
	{
		GEngine->GameViewport->EngineShowFlags = FEngineShowFlags(ESFIM_Game);
		GEngine->GameViewport->ViewModeIndex = VMI_Lit;
	}
}

void AAstralisAssemblyPC::BeginPlay()
{
	Super::BeginPlay();

	SetInputMode(FInputModeGameOnly());
	bShowMouseCursor = true;

	ConsoleCommand(TEXT("viewmode lit"));
	ForceEditorLitMode();

	GetWorldTimerManager().SetTimerForNextTick([this]()
	{
		ConsoleCommand(TEXT("viewmode lit"));
		ForceEditorLitMode();
	});
}

void AAstralisAssemblyPC::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Force Lit viewmode — the editor PIE viewport inherits the editor's
	// visualization mode (Shader Complexity, Wireframe, etc.)
	ForceEditorLitMode();
	ViewModeFixTimer -= DeltaTime;
	if (ViewModeFixTimer <= 0.0f)
	{
		ViewModeFixTimer = 0.5f;
		ConsoleCommand(TEXT("viewmode lit"));
	}

	FHitResult Hit;
	HoveredComponent = nullptr;

	if (TraceUnderCursor(Hit))
	{
		AAstralisComponentActor* HitComp = Cast<AAstralisComponentActor>(Hit.GetActor());
		if (HitComp)
		{
			HoveredComponent = HitComp;
		}
	}

	if (ActiveTool == EAssemblyTool::Place && SelectedComponentData)
	{
		UpdateGhostPreview();
	}
	else
	{
		ClearGhostPreview();
	}
}

void AAstralisAssemblyPC::SetActiveTool(EAssemblyTool Tool)
{
	ActiveTool = Tool;
	if (Tool != EAssemblyTool::Place)
	{
		ClearGhostPreview();
		SelectedComponentData = nullptr;
	}
}

void AAstralisAssemblyPC::SetPlacementData(UAstralisComponentData* Data)
{
	SelectedComponentData = Data;
	ActiveTool = EAssemblyTool::Place;
}

void AAstralisAssemblyPC::OnLeftClick()
{
	FHitResult Hit;

	switch (ActiveTool)
	{
	case EAssemblyTool::Select:
		if (SelectedComponent)
		{
			SelectedComponent->SetHighlighted(false);
		}
		if (TraceUnderCursor(Hit))
		{
			SelectedComponent = Cast<AAstralisComponentActor>(Hit.GetActor());
			if (SelectedComponent)
			{
				SelectedComponent->SetHighlighted(true);
			}
		}
		else
		{
			SelectedComponent = nullptr;
		}
		break;

	case EAssemblyTool::Place:
		if (SelectedComponentData && TraceUnderCursor(Hit))
		{
			AAstralisAssemblyGameMode* GM = GetAssemblyGameMode();
			if (GM)
			{
				FVector PlacePos = SnapToNearestFace(CursorWorldPosition_SI());
				GM->AddSpacecraftComponent(SelectedComponentData, PlacePos);
			}
		}
		break;

	case EAssemblyTool::Remove:
		if (TraceUnderCursor(Hit))
		{
			AAstralisComponentActor* Target = Cast<AAstralisComponentActor>(Hit.GetActor());
			if (Target)
			{
				AAstralisAssemblyGameMode* GM = GetAssemblyGameMode();
				if (GM)
				{
					GM->RemoveSpacecraftComponent(Target);
				}
			}
		}
		break;
	}
}

void AAstralisAssemblyPC::OnRightClick()
{
	if (ActiveTool != EAssemblyTool::Select)
	{
		SetActiveTool(EAssemblyTool::Select);
	}
	else if (SelectedComponent)
	{
		SelectedComponent->SetHighlighted(false);
		SelectedComponent = nullptr;
	}
}

bool AAstralisAssemblyPC::TraceUnderCursor(FHitResult& OutHit) const
{
	FVector WorldLoc, WorldDir;
	if (!DeprojectMousePositionToWorld(WorldLoc, WorldDir))
	{
		return false;
	}

	const FVector End = WorldLoc + WorldDir * 100000.0f;
	FCollisionQueryParams Params;
	Params.bTraceComplex = false;
	if (GhostPreview)
	{
		Params.AddIgnoredActor(GhostPreview);
	}

	return GetWorld()->LineTraceSingleByChannel(OutHit, WorldLoc, End, ECC_Visibility, Params);
}

FVector AAstralisAssemblyPC::CursorWorldPosition_SI() const
{
	FHitResult Hit;
	if (TraceUnderCursor(Hit))
	{
		FVector3d SI = FAstralisCoordinateService::UEToSI(FVector3d(Hit.ImpactPoint));
		return FVector(SI);
	}
	return FVector::ZeroVector;
}

void AAstralisAssemblyPC::UpdateGhostPreview()
{
	if (!GhostPreview)
	{
		FActorSpawnParameters Params;
		GhostPreview = GetWorld()->SpawnActor<AAstralisComponentActor>(
			AAstralisComponentActor::StaticClass(),
			FVector::ZeroVector, FRotator::ZeroRotator, Params);
		if (GhostPreview)
		{
			GhostPreview->SetActorEnableCollision(false);
			GhostPreview->ComponentData = SelectedComponentData;
			GhostPreview->ApplyComponentData();
			if (GhostPreview->MeshComponent)
			{
				GhostPreview->MeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
				GhostPreview->MeshComponent->SetRenderCustomDepth(true);
			}
		}
	}

	if (GhostPreview)
	{
		if (GhostPreview->ComponentData != SelectedComponentData)
		{
			GhostPreview->ComponentData = SelectedComponentData;
			GhostPreview->ApplyComponentData();
		}

		FVector Pos = SnapToNearestFace(CursorWorldPosition_SI());
		GhostPreview->SetPositionSI(Pos);
	}
}

void AAstralisAssemblyPC::ClearGhostPreview()
{
	if (GhostPreview)
	{
		GhostPreview->Destroy();
		GhostPreview = nullptr;
	}
}

FVector AAstralisAssemblyPC::SnapToNearestFace(const FVector& CursorPos_SI) const
{
	AAstralisAssemblyGameMode* GM = GetAssemblyGameMode();
	if (!GM || !SelectedComponentData)
	{
		return CursorPos_SI;
	}

	const FVector PlacingHalf = SelectedComponentData->Geometry.Envelope_m * 0.5;
	double BestDist = SnapRadius_m;
	FVector BestPos = CursorPos_SI;

	for (const auto& Installed : GM->InstalledComponents)
	{
		if (!Installed || !Installed->ComponentData) continue;

		const FVector InstalledHalf = Installed->GetEnvelope_SI() * 0.5;
		const FVector& InstalledPos = Installed->Position_SI_m;

		// 6 face directions
		struct FFaceDir { FVector Dir; int32 Axis; };
		const FFaceDir Dirs[] = {
			{{ 1, 0, 0}, 0}, {{-1, 0, 0}, 0},
			{{ 0, 1, 0}, 1}, {{ 0,-1, 0}, 1},
			{{ 0, 0, 1}, 2}, {{ 0, 0,-1}, 2}
		};

		for (const auto& F : Dirs)
		{
			// Snap position: face of installed component + half of placing component
			double InstalledExtent = (F.Axis == 0) ? InstalledHalf.X : (F.Axis == 1) ? InstalledHalf.Y : InstalledHalf.Z;
			double PlacingExtent = (F.Axis == 0) ? PlacingHalf.X : (F.Axis == 1) ? PlacingHalf.Y : PlacingHalf.Z;

			FVector SnapTarget = InstalledPos + F.Dir * (InstalledExtent + PlacingExtent);
			double Dist = FVector::Dist(CursorPos_SI, SnapTarget);

			if (Dist < BestDist)
			{
				BestDist = Dist;
				BestPos = SnapTarget;
			}
		}
	}

	return BestPos;
}

AAstralisAssemblyGameMode* AAstralisAssemblyPC::GetAssemblyGameMode() const
{
	return Cast<AAstralisAssemblyGameMode>(GetWorld()->GetAuthGameMode());
}

void AAstralisAssemblyPC::OnLaunchPressed()
{
	UE_LOG(LogTemp, Warning, TEXT("F5 pressed - attempting launch to orbit"));

	if (AAstralisAssemblyGameMode* GM = GetAssemblyGameMode())
	{
		UE_LOG(LogTemp, Warning, TEXT("GameMode found, current phase: %d"), (int)GM->CurrentPhase);
		if (GM->CurrentPhase == EAstralisGamePhase::Assembly)
		{
			UE_LOG(LogTemp, Warning, TEXT("Calling LaunchToOrbit()"));
			GM->LaunchToOrbit();
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Not in Assembly phase, ignoring F5"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("GameMode not found!"));
	}
}

void AAstralisAssemblyPC::OnReturnPressed()
{
	if (AAstralisAssemblyGameMode* GM = GetAssemblyGameMode())
	{
		if (GM->CurrentPhase == EAstralisGamePhase::Orbit)
		{
			GM->ReturnToAssembly();
		}
		else if (GM->CurrentPhase == EAstralisGamePhase::Reentry)
		{
			GM->ReturnToAssembly();
		}
		else if (GM->CurrentPhase == EAstralisGamePhase::Launch && GM->LaunchVehicle &&
			GM->LaunchVehicle->MissionState == ELaunchVehicleState::Complete &&
			!GM->LaunchVehicle->bOrbitAchieved)
		{
			GM->ReturnToAssembly();
		}
	}
}

void AAstralisAssemblyPC::OnDeorbitBurn()
{
	if (AAstralisAssemblyGameMode* GM = GetAssemblyGameMode())
	{
		if (GM->CurrentPhase == EAstralisGamePhase::Orbit)
		{
			GM->TransitionToReentry();
		}
	}
}

void AAstralisAssemblyPC::OnTimeWarpUp()
{
	if (AAstralisAssemblyGameMode* GM = GetAssemblyGameMode())
	{
		if (GM->OrbitScene)
			GM->OrbitScene->IncreaseTimeWarp();
	}
}

void AAstralisAssemblyPC::OnTimeWarpDown()
{
	if (AAstralisAssemblyGameMode* GM = GetAssemblyGameMode())
	{
		if (GM->OrbitScene)
			GM->OrbitScene->DecreaseTimeWarp();
	}
}

void AAstralisAssemblyPC::OnPauseToggle()
{
	if (AAstralisAssemblyGameMode* GM = GetAssemblyGameMode())
	{
		if (GM->OrbitScene)
			GM->OrbitScene->TogglePause();
	}
}

void AAstralisAssemblyPC::OnPhaseChanged(EAstralisGamePhase NewPhase)
{
	if (AHUD* CurrentHUD = GetHUD())
	{
		if (AAstralisAssemblyHUD* AssemblyHUD = Cast<AAstralisAssemblyHUD>(CurrentHUD))
		{
			AssemblyHUD->OnPhaseChanged(NewPhase);
		}
	}
}

void AAstralisAssemblyPC::OnEngineSwapToggle()
{
	if (AAstralisAssemblyGameMode* GM = GetAssemblyGameMode())
	{
		if (GM->LaunchVehicle)
			GM->LaunchVehicle->ToggleSwapMode();
	}
}

void AAstralisAssemblyPC::OnSwapStageToggle()
{
	if (AAstralisAssemblyGameMode* GM = GetAssemblyGameMode())
	{
		if (GM->LaunchVehicle)
			GM->LaunchVehicle->SwapNextStage();
	}
}

static void DoSwapEngine(AAstralisAssemblyPC* PC, int32 Index)
{
	if (AAstralisAssemblyGameMode* GM = PC->GetAssemblyGameMode())
	{
		if (GM->LaunchVehicle)
			GM->LaunchVehicle->ApplyEngineFromCatalog(Index);
	}
}

void AAstralisAssemblyPC::OnSwapEngine1() { DoSwapEngine(this, 0); }
void AAstralisAssemblyPC::OnSwapEngine2() { DoSwapEngine(this, 1); }
void AAstralisAssemblyPC::OnSwapEngine3() { DoSwapEngine(this, 2); }
void AAstralisAssemblyPC::OnSwapEngine4() { DoSwapEngine(this, 3); }
void AAstralisAssemblyPC::OnSwapEngine5() { DoSwapEngine(this, 4); }
void AAstralisAssemblyPC::OnSwapEngine6() { DoSwapEngine(this, 5); }
void AAstralisAssemblyPC::OnSwapEngine7() { DoSwapEngine(this, 6); }
void AAstralisAssemblyPC::OnSwapEngine8() { DoSwapEngine(this, 7); }

void AAstralisAssemblyPC::OnVehicleNext()
{
	if (AAstralisAssemblyGameMode* GM = GetAssemblyGameMode())
		GM->CycleVehicle(1);
}

void AAstralisAssemblyPC::OnVehiclePrev()
{
	if (AAstralisAssemblyGameMode* GM = GetAssemblyGameMode())
		GM->CycleVehicle(-1);
}

void AAstralisAssemblyPC::OnMissionNext()
{
	if (AAstralisAssemblyGameMode* GM = GetAssemblyGameMode())
		GM->CycleMission(1);
}

void AAstralisAssemblyPC::OnMissionPrev()
{
	if (AAstralisAssemblyGameMode* GM = GetAssemblyGameMode())
		GM->CycleMission(-1);
}

void AAstralisAssemblyPC::OnManeuverPrograde()
{
	if (AAstralisAssemblyGameMode* GM = GetAssemblyGameMode())
	{
		if (GM->OrbitScene)
			GM->OrbitScene->ExecutePrograde(10.0);
	}
}

void AAstralisAssemblyPC::OnManeuverRetrograde()
{
	if (AAstralisAssemblyGameMode* GM = GetAssemblyGameMode())
	{
		if (GM->OrbitScene)
			GM->OrbitScene->ExecuteRetrograde(10.0);
	}
}

void AAstralisAssemblyPC::OnManeuverNormal()
{
	if (AAstralisAssemblyGameMode* GM = GetAssemblyGameMode())
	{
		if (GM->OrbitScene)
			GM->OrbitScene->ExecuteNormal(10.0);
	}
}

void AAstralisAssemblyPC::OnManeuverAntiNormal()
{
	if (AAstralisAssemblyGameMode* GM = GetAssemblyGameMode())
	{
		if (GM->OrbitScene)
			GM->OrbitScene->ExecuteAntiNormal(10.0);
	}
}

void AAstralisAssemblyPC::OnManeuverRadialIn()
{
	if (AAstralisAssemblyGameMode* GM = GetAssemblyGameMode())
	{
		if (GM->OrbitScene)
			GM->OrbitScene->ExecuteRadialIn(10.0);
	}
}

void AAstralisAssemblyPC::OnManeuverRadialOut()
{
	if (AAstralisAssemblyGameMode* GM = GetAssemblyGameMode())
	{
		if (GM->OrbitScene)
			GM->OrbitScene->ExecuteRadialOut(10.0);
	}
}

void AAstralisAssemblyPC::OnThrottleUp()
{
	if (AAstralisAssemblyGameMode* GM = GetAssemblyGameMode())
	{
		if (GM->CurrentPhase == EAstralisGamePhase::Launch && GM->LaunchVehicle)
			GM->LaunchVehicle->SetThrottle(GM->LaunchVehicle->ThrottlePct + 0.1);
	}
}

void AAstralisAssemblyPC::OnThrottleDown()
{
	if (AAstralisAssemblyGameMode* GM = GetAssemblyGameMode())
	{
		if (GM->CurrentPhase == EAstralisGamePhase::Launch && GM->LaunchVehicle)
			GM->LaunchVehicle->SetThrottle(GM->LaunchVehicle->ThrottlePct - 0.1);
	}
}

void AAstralisAssemblyPC::OnCycleCamera()
{
	if (AAstralisAssemblyPawn* CamPawn = Cast<AAstralisAssemblyPawn>(GetPawn()))
	{
		CamPawn->CycleCameraMode();
	}
}

void AAstralisAssemblyPC::OnAbortLaunch()
{
	if (AAstralisAssemblyGameMode* GM = GetAssemblyGameMode())
	{
		if (GM->CurrentPhase == EAstralisGamePhase::Launch && GM->LaunchVehicle)
		{
			if (GM->LaunchVehicle->MissionState == ELaunchVehicleState::Countdown ||
				GM->LaunchVehicle->MissionState == ELaunchVehicleState::Liftoff)
			{
				GM->LaunchVehicle->AbortLaunch();
				GM->ReturnToAssembly();
			}
		}
	}
}

void AAstralisAssemblyPC::OnBusNext()
{
	if (AAstralisAssemblyGameMode* GM = GetAssemblyGameMode())
	{
		if (GM->CurrentPhase == EAstralisGamePhase::Assembly)
			GM->CycleBus(1);
	}
}

void AAstralisAssemblyPC::OnBusPrev()
{
	if (AAstralisAssemblyGameMode* GM = GetAssemblyGameMode())
	{
		if (GM->CurrentPhase == EAstralisGamePhase::Assembly)
			GM->CycleBus(-1);
	}
}

static void DoToggleInstrument(AAstralisAssemblyPC* PC, int32 Index)
{
	if (AAstralisAssemblyGameMode* GM = PC->GetAssemblyGameMode())
	{
		if (GM->CurrentPhase == EAstralisGamePhase::Assembly)
			GM->ToggleInstrument(Index);
	}
}

void AAstralisAssemblyPC::OnInstrument1()  { DoToggleInstrument(this, 0); }
void AAstralisAssemblyPC::OnInstrument2()  { DoToggleInstrument(this, 1); }
void AAstralisAssemblyPC::OnInstrument3()  { DoToggleInstrument(this, 2); }
void AAstralisAssemblyPC::OnInstrument4()  { DoToggleInstrument(this, 3); }
void AAstralisAssemblyPC::OnInstrument5()  { DoToggleInstrument(this, 4); }
void AAstralisAssemblyPC::OnInstrument6()  { DoToggleInstrument(this, 5); }
void AAstralisAssemblyPC::OnInstrument7()  { DoToggleInstrument(this, 6); }
void AAstralisAssemblyPC::OnInstrument8()  { DoToggleInstrument(this, 7); }
void AAstralisAssemblyPC::OnInstrument9()  { DoToggleInstrument(this, 8); }
void AAstralisAssemblyPC::OnInstrument10() { DoToggleInstrument(this, 9); }
