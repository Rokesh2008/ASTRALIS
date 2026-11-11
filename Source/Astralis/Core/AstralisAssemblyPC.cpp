#include "AstralisAssemblyPC.h"
#include "AstralisAssemblyGameMode.h"
#include "Astralis/Spacecraft/AstralisComponentActor.h"
#include "Astralis/Presentation/AstralisCoordinateService.h"
#include "Astralis/Orbit/AstralisOrbitSceneActor.h"
#include "Astralis/UI/AstralisAssemblyHUD.h"
#include "AstralisData/ComponentCatalog/AstralisComponentTypes.h"
#include "Engine/World.h"

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
	InputComponent->BindKey(EKeys::Escape, IE_Pressed, this, &AAstralisAssemblyPC::OnReturnPressed);

	// Time warp (Period = >, Comma = <)
	InputComponent->BindKey(EKeys::Period, IE_Pressed, this, &AAstralisAssemblyPC::OnTimeWarpUp);
	InputComponent->BindKey(EKeys::Comma, IE_Pressed, this, &AAstralisAssemblyPC::OnTimeWarpDown);
	InputComponent->BindKey(EKeys::SpaceBar, IE_Pressed, this, &AAstralisAssemblyPC::OnPauseToggle);
}

void AAstralisAssemblyPC::BeginPlay()
{
	Super::BeginPlay();

	// Override mouse capture so cursor stays visible
	SetInputMode(FInputModeGameAndUI()
		.SetHideCursorDuringCapture(false)
		.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock));
	bShowMouseCursor = true;
}

void AAstralisAssemblyPC::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

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
	if (AAstralisAssemblyGameMode* GM = GetAssemblyGameMode())
	{
		if (GM->CurrentPhase == EAstralisGamePhase::Assembly)
		{
			GM->LaunchToOrbit();
		}
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
