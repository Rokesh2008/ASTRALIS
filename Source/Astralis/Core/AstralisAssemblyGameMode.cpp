#include "AstralisAssemblyGameMode.h"
#include "Astralis/Spacecraft/AstralisSpacecraftActor.h"
#include "Astralis/Spacecraft/AstralisComponentActor.h"
#include "AstralisSimulation/MassProperties/AstralisMassCalculator.h"
#include "AstralisData/ComponentCatalog/AstralisComponentTypes.h"
#include "Astralis/Core/AstralisAssemblyPC.h"
#include "Astralis/Core/AstralisAssemblyPawn.h"
#include "Astralis/UI/AstralisAssemblyHUD.h"
#include "Astralis/Orbit/AstralisOrbitSceneActor.h"
#include "Engine/AssetManager.h"
#include "Engine/StaticMesh.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/DirectionalLight.h"
#include "Components/DirectionalLightComponent.h"
#include "Engine/SkyLight.h"
#include "Components/SkyLightComponent.h"

AAstralisAssemblyGameMode::AAstralisAssemblyGameMode()
{
	PlayerControllerClass = AAstralisAssemblyPC::StaticClass();
	DefaultPawnClass = AAstralisAssemblyPawn::StaticClass();
	HUDClass = AAstralisAssemblyHUD::StaticClass();
}

void AAstralisAssemblyGameMode::BeginPlay()
{
	Super::BeginPlay();
	SpawnSpacecraft();
}

void AAstralisAssemblyGameMode::SpawnAssemblyEnvironment()
{
	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	// Assembly floor — large flat plane
	{
		FActorSpawnParameters Params;
		Params.Name = FName(TEXT("AssemblyFloor"));
		AActor* Floor = World->SpawnActor<AActor>(AActor::StaticClass(),
			FVector(0.0f, 0.0f, -10.0f), FRotator::ZeroRotator, Params);
		if (Floor)
		{
			UStaticMeshComponent* FloorMesh = NewObject<UStaticMeshComponent>(Floor, TEXT("FloorMesh"));
			Floor->SetRootComponent(FloorMesh);
			FloorMesh->RegisterComponent();

			UStaticMesh* CubeMesh = LoadObject<UStaticMesh>(nullptr,
				TEXT("/Engine/BasicShapes/Cube.Cube"));
			if (CubeMesh)
			{
				FloorMesh->SetStaticMesh(CubeMesh);
				FloorMesh->SetWorldScale3D(FVector(100.0f, 100.0f, 0.1f));
				FloorMesh->SetCollisionProfileName(TEXT("BlockAll"));

				UMaterial* FloorMat = LoadObject<UMaterial>(nullptr,
					TEXT("/Game/Materials/M_AssemblyFloor.M_AssemblyFloor"));
				if (FloorMat)
				{
					FloorMesh->SetMaterial(0, FloorMat);
				}
			}
		}
	}

	// Directional light
	{
		FActorSpawnParameters Params;
		Params.Name = FName(TEXT("AssemblyLight"));
		ADirectionalLight* Light = World->SpawnActor<ADirectionalLight>(
			ADirectionalLight::StaticClass(),
			FVector(0.0f, 0.0f, 1000.0f),
			FRotator(-45.0f, 30.0f, 0.0f),
			Params);
		if (Light)
		{
			Light->GetLightComponent()->SetIntensity(4.0f);
		}
	}

	// Sky light for ambient fill
	{
		FActorSpawnParameters Params;
		Params.Name = FName(TEXT("AssemblySkyLight"));
		ASkyLight* Sky = World->SpawnActor<ASkyLight>(
			ASkyLight::StaticClass(),
			FVector::ZeroVector, FRotator::ZeroRotator, Params);
		if (Sky)
		{
			Sky->GetLightComponent()->SetIntensity(1.0f);
			Sky->GetLightComponent()->RecaptureSky();
		}
	}

	// Spawn a visible bus blockout at origin so there's something to see immediately
	{
		FActorSpawnParameters Params;
		Params.Name = FName(TEXT("SpacecraftBus_Visual"));
		AActor* Bus = World->SpawnActor<AActor>(AActor::StaticClass(),
			FVector(0.0f, 0.0f, 50.0f), FRotator::ZeroRotator, Params);
		if (Bus)
		{
			UStaticMeshComponent* BusMesh = NewObject<UStaticMeshComponent>(Bus, TEXT("BusMesh"));
			Bus->SetRootComponent(BusMesh);
			BusMesh->RegisterComponent();

			UStaticMesh* CubeMesh = LoadObject<UStaticMesh>(nullptr,
				TEXT("/Engine/BasicShapes/Cube.Cube"));
			if (CubeMesh)
			{
				BusMesh->SetStaticMesh(CubeMesh);
				BusMesh->SetWorldScale3D(FVector(1.0f, 1.0f, 1.0f));
				BusMesh->SetCollisionProfileName(TEXT("BlockAll"));

				UMaterial* BusMat = LoadObject<UMaterial>(nullptr,
					TEXT("/Game/Materials/M_SpacecraftBus.M_SpacecraftBus"));
				if (BusMat)
				{
					BusMesh->SetMaterial(0, BusMat);
				}
			}
		}
	}
}

AAstralisSpacecraftActor* AAstralisAssemblyGameMode::SpawnSpacecraft()
{
	if (ActiveSpacecraft)
	{
		return ActiveSpacecraft;
	}

	FActorSpawnParameters Params;
	Params.Name = FName(TEXT("ASTRALIS_Spacecraft"));
	ActiveSpacecraft = GetWorld()->SpawnActor<AAstralisSpacecraftActor>(
		AAstralisSpacecraftActor::StaticClass(),
		FVector::ZeroVector,
		FRotator::ZeroRotator,
		Params
	);

	return ActiveSpacecraft;
}

AAstralisComponentActor* AAstralisAssemblyGameMode::AddSpacecraftComponent(
	UAstralisComponentData* Data,
	const FVector& Position_SI_m)
{
	if (!Data || !ActiveSpacecraft)
	{
		return nullptr;
	}

	FActorSpawnParameters Params;
	AAstralisComponentActor* CompActor = GetWorld()->SpawnActor<AAstralisComponentActor>(
		AAstralisComponentActor::StaticClass(),
		FVector::ZeroVector,
		FRotator::ZeroRotator,
		Params
	);

	if (CompActor)
	{
		CompActor->ComponentData = Data;
		CompActor->ApplyComponentData();
		CompActor->SetPositionSI(Position_SI_m);
		CompActor->AttachToActor(ActiveSpacecraft, FAttachmentTransformRules::KeepWorldTransform);
		InstalledComponents.Add(CompActor);
		UpdateMassProperties();
	}

	return CompActor;
}

bool AAstralisAssemblyGameMode::RemoveSpacecraftComponent(AAstralisComponentActor* Component)
{
	if (!Component)
	{
		return false;
	}

	const int32 Removed = InstalledComponents.Remove(Component);
	if (Removed > 0)
	{
		Component->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
		Component->Destroy();
		UpdateMassProperties();
		return true;
	}
	return false;
}

TArray<UAstralisComponentData*> AAstralisAssemblyGameMode::GetAvailableComponents() const
{
	TArray<UAstralisComponentData*> Result;

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
				Result.Add(Comp);
			}
		}
	}

	return Result;
}

void AAstralisAssemblyGameMode::UpdateMassProperties()
{
	if (!ActiveSpacecraft)
	{
		return;
	}

	ActiveSpacecraft->ComponentEntries.Empty();

	for (const auto& Comp : InstalledComponents)
	{
		if (Comp && Comp->ComponentData)
		{
			FComponentMassEntry Entry;
			Entry.Mass_kg = Comp->ComponentData->Mass_kg;
			Entry.Position_m = FVector3d(Comp->Position_SI_m);
			ActiveSpacecraft->ComponentEntries.Add(Entry);
		}
	}

	ActiveSpacecraft->RecalculateMassProperties();
}

void AAstralisAssemblyGameMode::LaunchToOrbit()
{
	if (CurrentPhase == EAstralisGamePhase::Orbit) return;
	if (!ActiveSpacecraft) return;

	CurrentPhase = EAstralisGamePhase::Orbit;

	// Gather propellant mass from placed propulsion components
	double PropMass = 0.0;
	for (const auto& Comp : InstalledComponents)
	{
		if (Comp && Comp->ComponentData &&
			Comp->ComponentData->Category == EComponentCategory::Propulsion)
		{
			PropMass += Comp->ComponentData->Mass_kg;
		}
	}

	// Spawn orbit scene at a separate location (far from assembly lab)
	FActorSpawnParameters Params;
	Params.Name = FName(TEXT("OrbitScene"));
	OrbitScene = GetWorld()->SpawnActor<AAstralisOrbitSceneActor>(
		AAstralisOrbitSceneActor::StaticClass(),
		FVector(0.0, 0.0, 50000.0),
		FRotator::ZeroRotator,
		Params);

	if (OrbitScene)
	{
		OrbitScene->InitializeFromAssembly(
			ActiveSpacecraft->ComponentEntries, PropMass);
	}

	// Hide assembly actors
	if (ActiveSpacecraft)
		ActiveSpacecraft->SetActorHiddenInGame(true);
	for (auto& Comp : InstalledComponents)
	{
		if (Comp) Comp->SetActorHiddenInGame(true);
	}

	// Move pawn to orbit scene
	APawn* Pawn = GetWorld()->GetFirstPlayerController()->GetPawn();
	if (Pawn && OrbitScene)
	{
		Pawn->SetActorLocation(OrbitScene->GetActorLocation() + FVector(0.0, -200.0, 50.0));
	}

	// Notify HUD to switch
	if (AAstralisAssemblyPC* PC = Cast<AAstralisAssemblyPC>(
		GetWorld()->GetFirstPlayerController()))
	{
		PC->OnPhaseChanged(CurrentPhase);
	}
}

void AAstralisAssemblyGameMode::ReturnToAssembly()
{
	if (CurrentPhase == EAstralisGamePhase::Assembly) return;

	CurrentPhase = EAstralisGamePhase::Assembly;

	// Show assembly actors
	if (ActiveSpacecraft)
		ActiveSpacecraft->SetActorHiddenInGame(false);
	for (auto& Comp : InstalledComponents)
	{
		if (Comp) Comp->SetActorHiddenInGame(false);
	}

	// Destroy orbit scene
	if (OrbitScene)
	{
		OrbitScene->Destroy();
		OrbitScene = nullptr;
	}

	// Move pawn back to assembly area
	APawn* Pawn = GetWorld()->GetFirstPlayerController()->GetPawn();
	if (Pawn)
	{
		Pawn->SetActorLocation(FVector(0.0, -300.0, 150.0));
	}

	if (AAstralisAssemblyPC* PC = Cast<AAstralisAssemblyPC>(
		GetWorld()->GetFirstPlayerController()))
	{
		PC->OnPhaseChanged(CurrentPhase);
	}
}
