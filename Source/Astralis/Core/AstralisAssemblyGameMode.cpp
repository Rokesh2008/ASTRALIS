#include "AstralisAssemblyGameMode.h"
#include "Astralis/Spacecraft/AstralisSpacecraftActor.h"
#include "Astralis/Spacecraft/AstralisComponentActor.h"
#include "AstralisSimulation/MassProperties/AstralisMassCalculator.h"
#include "AstralisData/ComponentCatalog/AstralisComponentTypes.h"
#include "Astralis/Core/AstralisAssemblyPC.h"
#include "Astralis/Core/AstralisAssemblyPawn.h"
#include "Astralis/UI/AstralisAssemblyHUD.h"
#include "Astralis/Orbit/AstralisOrbitSceneActor.h"
#include "Astralis/LaunchVehicle/AstralisLaunchVehicleActor.h"
#include "Astralis/LaunchVehicle/AstralisStageActor.h"
#include "Engine/AssetManager.h"
#include "Engine/StaticMesh.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "EngineUtils.h"
#include "DrawDebugHelpers.h"
#include "Landscape.h"
#include "LandscapeComponent.h"
#include "Materials/MaterialInstanceDynamic.h"

// Console command for testing
static FAutoConsoleCommand LaunchToOrbitCmd(
	TEXT("astralis.launch"),
	TEXT("Launch to orbit (debug command)"),
	FConsoleCommandDelegate::CreateLambda([]()
	{
		if (GEngine && GEngine->GameViewport)
		{
			UWorld* World = GEngine->GameViewport->GetWorld();
			if (World)
			{
				AAstralisAssemblyGameMode* GM = Cast<AAstralisAssemblyGameMode>(World->GetAuthGameMode());
				if (GM)
				{
					UE_LOG(LogTemp, Warning, TEXT("Manual launch triggered via console"));
					GM->LaunchToOrbit();
				}
				else
				{
					UE_LOG(LogTemp, Error, TEXT("GameMode is not AstralisAssemblyGameMode!"));
				}
			}
		}
	})
);

AAstralisAssemblyGameMode::AAstralisAssemblyGameMode()
{
	PlayerControllerClass = AAstralisAssemblyPC::StaticClass();
	DefaultPawnClass = AAstralisAssemblyPawn::StaticClass();
	HUDClass = AAstralisAssemblyHUD::StaticClass();

	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;
}

void AAstralisAssemblyGameMode::BeginPlay()
{
	Super::BeginPlay();

	VehicleLibrary = UAstralisVehicleLibrary::GetDefaultLibrary();
	SelectedVehicleIndex = 0;
	InitMissions();
	InitBusCatalog();
	InitInstrumentCatalog();
	RecalculateBudget();

	ApplyLandscapeMaterial();
	SpawnSpacecraft();
	CreateExampleLaunchVehicle();
	FocusCameraOnPad();
	GetWorldTimerManager().SetTimerForNextTick(this, &AAstralisAssemblyGameMode::FocusCameraOnPad);

	RandomizeWeather();

	UE_LOG(LogTemp, Warning, TEXT("=== ASTRALIS READY — V=Vehicle, M=Mission, F5=Launch ==="));
}

void AAstralisAssemblyGameMode::RandomizeWeather()
{
	WindSpeed_kts = FMath::RandRange(0.0, 35.0);
	WindDirection_deg = FMath::RandRange(0.0, 360.0);
	Temperature_F = FMath::RandRange(40.0, 95.0);
	CloudCover_pct = FMath::RandRange(0.0, 100.0);
	Visibility_km = FMath::RandRange(3.0, 20.0);

	if (CloudCover_pct < 15.0) WeatherCondition = TEXT("Clear");
	else if (CloudCover_pct < 40.0) WeatherCondition = TEXT("Partly Cloudy");
	else if (CloudCover_pct < 70.0) WeatherCondition = TEXT("Mostly Cloudy");
	else if (CloudCover_pct < 90.0) WeatherCondition = TEXT("Overcast");
	else WeatherCondition = TEXT("Thunderstorms");

	bLaunchGO = (WindSpeed_kts < 30.0) && (Visibility_km > 5.0) &&
		!WeatherCondition.Equals(TEXT("Thunderstorms")) && (Temperature_F > 35.0);
}

double AAstralisAssemblyGameMode::GetMissionDifficulty() const
{
	if (Missions.Num() == 0) return 0.0;
	const FMissionDefinition& M = Missions[SelectedMissionIndex];
	double Diff = 1.0;
	if (M.TargetAltitude_km > 2000.0) Diff += 1.5;
	else if (M.TargetAltitude_km > 600.0) Diff += 0.5;
	if (M.TargetInclination_deg > 90.0) Diff += 0.5;
	if (M.OrbitType.Equals(TEXT("GTO")) || M.OrbitType.Equals(TEXT("HEO"))) Diff += 1.0;
	if (M.OrbitType.Equals(TEXT("TLI"))) Diff += 2.0;
	if (M.OrbitType.Equals(TEXT("Molniya"))) Diff += 1.5;
	if (!bLaunchGO) Diff += 0.5;
	return FMath::Clamp(Diff, 1.0, 5.0);
}

void AAstralisAssemblyGameMode::InitMissions()
{
	{
		FMissionDefinition M;
		M.Name = TEXT("LEO 400 km");
		M.Description = TEXT("Low Earth Orbit — International Space Station altitude");
		M.TargetAltitude_km = 400.0;
		M.TargetInclination_deg = 51.6;
		M.OrbitType = TEXT("LEO");
		M.MinPayload_kg = 100.0;
		Missions.Add(M);
	}
	{
		FMissionDefinition M;
		M.Name = TEXT("LEO 550 km");
		M.Description = TEXT("Sun-synchronous orbit — Earth observation satellites");
		M.TargetAltitude_km = 550.0;
		M.TargetInclination_deg = 97.6;
		M.OrbitType = TEXT("SSO");
		M.MinPayload_kg = 100.0;
		Missions.Add(M);
	}
	{
		FMissionDefinition M;
		M.Name = TEXT("MEO 20200 km");
		M.Description = TEXT("Medium Earth Orbit — GPS constellation altitude");
		M.TargetAltitude_km = 20200.0;
		M.TargetInclination_deg = 55.0;
		M.OrbitType = TEXT("MEO");
		M.MinPayload_kg = 50.0;
		Missions.Add(M);
	}
	{
		FMissionDefinition M;
		M.Name = TEXT("GTO 35786 km");
		M.Description = TEXT("Geostationary Transfer Orbit — communications satellites");
		M.TargetAltitude_km = 35786.0;
		M.TargetInclination_deg = 28.5;
		M.OrbitType = TEXT("GTO");
		M.MinPayload_kg = 50.0;
		Missions.Add(M);
	}
	{
		FMissionDefinition M;
		M.Name = TEXT("Lunar TLI");
		M.Description = TEXT("Trans-Lunar Injection — Artemis-class trajectory");
		M.TargetAltitude_km = 384400.0;
		M.TargetInclination_deg = 28.5;
		M.OrbitType = TEXT("TLI");
		M.MinPayload_kg = 25.0;
		Missions.Add(M);
	}
	{
		FMissionDefinition M;
		M.Name = TEXT("Starlink 300 km");
		M.Description = TEXT("SpaceX Starlink constellation deployment orbit");
		M.TargetAltitude_km = 300.0;
		M.TargetInclination_deg = 53.0;
		M.OrbitType = TEXT("LEO");
		M.MinPayload_kg = 200.0;
		Missions.Add(M);
	}
	{
		FMissionDefinition M;
		M.Name = TEXT("Polar 800 km");
		M.Description = TEXT("Polar orbit — weather and spy satellites");
		M.TargetAltitude_km = 800.0;
		M.TargetInclination_deg = 98.7;
		M.OrbitType = TEXT("Polar");
		M.MinPayload_kg = 50.0;
		Missions.Add(M);
	}
	{
		FMissionDefinition M;
		M.Name = TEXT("Molniya 500x40000");
		M.Description = TEXT("Highly elliptical orbit — Russian comms coverage");
		M.TargetAltitude_km = 40000.0;
		M.TargetInclination_deg = 63.4;
		M.OrbitType = TEXT("HEO");
		M.MinPayload_kg = 25.0;
		Missions.Add(M);
	}
	{
		FMissionDefinition M;
		M.Name = TEXT("Hubble 547 km");
		M.Description = TEXT("Hubble Space Telescope service orbit");
		M.TargetAltitude_km = 547.0;
		M.TargetInclination_deg = 28.5;
		M.OrbitType = TEXT("LEO");
		M.MinPayload_kg = 100.0;
		Missions.Add(M);
	}
	{
		FMissionDefinition M;
		M.Name = TEXT("ISS Resupply");
		M.Description = TEXT("Cargo delivery to International Space Station");
		M.TargetAltitude_km = 420.0;
		M.TargetInclination_deg = 51.6;
		M.OrbitType = TEXT("LEO");
		M.MinPayload_kg = 500.0;
		Missions.Add(M);
	}
	{
		FMissionDefinition M;
		M.Name = TEXT("OneWeb 1200 km");
		M.Description = TEXT("OneWeb broadband constellation deployment");
		M.TargetAltitude_km = 1200.0;
		M.TargetInclination_deg = 87.9;
		M.OrbitType = TEXT("LEO");
		M.MinPayload_kg = 150.0;
		Missions.Add(M);
	}
	{
		FMissionDefinition M;
		M.Name = TEXT("GNSS 23222 km");
		M.Description = TEXT("Galileo navigation satellite orbit");
		M.TargetAltitude_km = 23222.0;
		M.TargetInclination_deg = 56.0;
		M.OrbitType = TEXT("MEO");
		M.MinPayload_kg = 50.0;
		Missions.Add(M);
	}
	{
		FMissionDefinition M;
		M.Name = TEXT("Crew Dragon Demo");
		M.Description = TEXT("Crew transport to ISS — human-rated trajectory");
		M.TargetAltitude_km = 408.0;
		M.TargetInclination_deg = 51.6;
		M.OrbitType = TEXT("LEO");
		M.MinPayload_kg = 500.0;
		Missions.Add(M);
	}
}

void AAstralisAssemblyGameMode::CycleVehicle(int32 Direction)
{
	if (CurrentPhase != EAstralisGamePhase::Assembly) return;
	if (VehicleLibrary.Num() == 0) return;

	SelectedVehicleIndex = (SelectedVehicleIndex + Direction + VehicleLibrary.Num()) % VehicleLibrary.Num();
	DestroyLaunchVehicle();
	CreateExampleLaunchVehicle();
	FocusCameraOnPad();
	RecalculateBudget();

	UE_LOG(LogTemp, Warning, TEXT("Vehicle: %s"), *VehicleLibrary[SelectedVehicleIndex].DisplayName.ToString());
}

void AAstralisAssemblyGameMode::CycleMission(int32 Direction)
{
	if (CurrentPhase != EAstralisGamePhase::Assembly) return;
	if (Missions.Num() == 0) return;

	SelectedMissionIndex = (SelectedMissionIndex + Direction + Missions.Num()) % Missions.Num();
	UE_LOG(LogTemp, Warning, TEXT("Mission: %s"), *Missions[SelectedMissionIndex].Name);
}

static AAstralisAssemblyPawn* GetCameraPawn(UWorld* World)
{
	APlayerController* PC = World ? World->GetFirstPlayerController() : nullptr;
	return PC ? Cast<AAstralisAssemblyPawn>(PC->GetPawn()) : nullptr;
}

void AAstralisAssemblyGameMode::FocusCameraOnPad()
{
	AAstralisAssemblyPawn* Pawn = GetCameraPawn(GetWorld());
	if (!Pawn || !LaunchVehicle) return;

	// Rocket height in UE units — matches BuildRocketVisual() scaling
	const double DiamScale = LaunchVehicle->VehicleDiameter_m / 3.7;
	const double S1H = 3000.0 * (LaunchVehicle->VehicleS1Length_m / 42.6);
	const double S2H = 800.0 * (LaunchVehicle->VehicleS2Length_m / 13.1);
	const double FairingH = 700.0 * DiamScale;
	const double NoseH = 200.0 * DiamScale;
	const double TotalH = S1H + 200.0 * DiamScale + S2H + FairingH + NoseH;
	const float FocusZ = static_cast<float>(TotalH * 0.45);
	const float Dist = static_cast<float>(TotalH * 1.8);
	// Yaw -90 views rocket from the side — same angle used during launch
	Pawn->FocusOn(LaunchVehicle->GetActorLocation() + FVector(0, 0, FocusZ), Dist, -90.0f, -5.0f);
}

void AAstralisAssemblyGameMode::SetLevelEnvironmentVisible(bool bVisible)
{
	if (bVisible)
	{
		for (const TWeakObjectPtr<AActor>& Weak : HiddenForOrbit)
		{
			if (AActor* Actor = Weak.Get())
			{
				Actor->SetActorHiddenInGame(false);
				for (UActorComponent* Comp : Actor->GetComponents())
				{
					if (USceneComponent* SC = Cast<USceneComponent>(Comp)) SC->SetVisibility(true);
				}
			}
		}
		HiddenForOrbit.Empty();
		return;
	}

	// Hide sky, clouds, fog and terrain so the orbit view has a black space background.
	// Matched by class name to avoid pulling in the Landscape module.
	static const TCHAR* EnvClassTokens[] = {
		TEXT("SkyAtmosphere"), TEXT("VolumetricCloud"), TEXT("ExponentialHeightFog"), TEXT("Landscape")
	};

	for (TActorIterator<AActor> It(GetWorld()); It; ++It)
	{
		AActor* Actor = *It;
		bool bIsEnv = false;

		for (UActorComponent* Comp : Actor->GetComponents())
		{
			const FString ClassName = Comp->GetClass()->GetName();
			for (const TCHAR* Token : EnvClassTokens)
			{
				if (ClassName.Contains(Token)) { bIsEnv = true; break; }
			}
			if (const UStaticMeshComponent* SMC = Cast<UStaticMeshComponent>(Comp))
			{
				if (SMC->GetStaticMesh() && SMC->GetStaticMesh()->GetName().Contains(TEXT("SkySphere")))
				{
					bIsEnv = true;
				}
			}
			if (bIsEnv) break;
		}

		if (!bIsEnv) continue;

		Actor->SetActorHiddenInGame(true);
		for (UActorComponent* Comp : Actor->GetComponents())
		{
			if (USceneComponent* SC = Cast<USceneComponent>(Comp)) SC->SetVisibility(false);
		}
		HiddenForOrbit.Add(Actor);
	}
}

void AAstralisAssemblyGameMode::ApplyLandscapeMaterial()
{
	// Create an Earth-like landscape material: green lowlands, brown hills, gray peaks
	// Uses a simple world-position-Z based color since we have no terrain textures
	UMaterial* BaseMat = LoadObject<UMaterial>(nullptr,
		TEXT("/Engine/EngineMaterials/WorldGridMaterial.WorldGridMaterial"));
	if (!BaseMat)
	{
		BaseMat = LoadObject<UMaterial>(nullptr,
			TEXT("/Engine/BasicShapes/BasicShapeMaterial.BasicShapeMaterial"));
	}
	if (!BaseMat) return;

	for (TActorIterator<ALandscape> It(GetWorld()); It; ++It)
	{
		ALandscape* Land = *It;

		UMaterialInstanceDynamic* LandMID = UMaterialInstanceDynamic::Create(BaseMat, this);
		// Florida coastal green — warm, slightly yellowish green
		LandMID->SetVectorParameterValue(TEXT("Color"), FLinearColor(0.15f, 0.28f, 0.08f));

		// Apply to every landscape component
		TArray<ULandscapeComponent*> LandComps;
		Land->GetComponents<ULandscapeComponent>(LandComps);
		for (ULandscapeComponent* LC : LandComps)
		{
			const int32 NumMats = LC->GetNumMaterials();
			for (int32 i = 0; i < FMath::Max(1, NumMats); ++i)
			{
				LC->SetMaterial(i, LandMID);
			}
		}

		UE_LOG(LogTemp, Warning, TEXT("Applied Earth landscape material to %s (%d components)"),
			*Land->GetName(), LandComps.Num());
	}
}

void AAstralisAssemblyGameMode::DestroyLaunchVehicle()
{
	if (LaunchPadActor)
	{
		LaunchPadActor->Destroy();
		LaunchPadActor = nullptr;
	}

	if (!LaunchVehicle) return;

	for (AAstralisStageActor* Stage : LaunchVehicle->Stages)
	{
		if (Stage) Stage->Destroy();
	}
	LaunchVehicle->Destroy();
	LaunchVehicle = nullptr;
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
	if (CurrentPhase != EAstralisGamePhase::Assembly) return;
	if (!ActiveSpacecraft) return;

	UE_LOG(LogTemp, Warning, TEXT("=== LAUNCH INITIATED ==="));

	CurrentPhase = EAstralisGamePhase::Launch;

	// Hide assembly actors
	if (ActiveSpacecraft)
		ActiveSpacecraft->SetActorHiddenInGame(true);
	for (auto& Comp : InstalledComponents)
	{
		if (Comp) Comp->SetActorHiddenInGame(true);
	}

	if (LaunchVehicle)
	{
		LaunchVehicle->StartCountdown();
	}

	// Notify HUD
	if (AAstralisAssemblyPC* PC = Cast<AAstralisAssemblyPC>(
		GetWorld()->GetFirstPlayerController()))
	{
		PC->OnPhaseChanged(CurrentPhase);
	}
}

void AAstralisAssemblyGameMode::ReturnToAssembly()
{
	if (CurrentPhase == EAstralisGamePhase::Assembly) return;

	if (CurrentPhase == EAstralisGamePhase::Launch && LaunchVehicle)
		RecordFlightResult();

	CurrentPhase = EAstralisGamePhase::Assembly;
	SmokePuffs.Empty();
	bWaitingForOrbitTransition = false;
	bLandscapeHiddenForLaunch = false;

	// Show assembly actors
	if (ActiveSpacecraft)
		ActiveSpacecraft->SetActorHiddenInGame(false);
	for (auto& Comp : InstalledComponents)
	{
		if (Comp) Comp->SetActorHiddenInGame(false);
	}

	if (OrbitScene)
	{
		OrbitScene->Destroy();
		OrbitScene = nullptr;
	}

	SetLevelEnvironmentVisible(true);

	// Fresh vehicle on the pad so the player can launch again
	DestroyLaunchVehicle();
	CreateExampleLaunchVehicle();
	FocusCameraOnPad();

	if (AAstralisAssemblyPC* PC = Cast<AAstralisAssemblyPC>(
		GetWorld()->GetFirstPlayerController()))
	{
		PC->OnPhaseChanged(CurrentPhase);
	}
}

void AAstralisAssemblyGameMode::CreateExampleLaunchVehicle()
{
	UWorld* World = GetWorld();
	if (!World || !ActiveSpacecraft) return;

	// Place the vehicle on whatever ground the level has under the origin
	FVector PadLocation(0.0, 0.0, 0.0);
	{
		FHitResult Hit;
		const FVector TraceStart(0.0, 0.0, 200000.0);
		const FVector TraceEnd(0.0, 0.0, -200000.0);
		if (World->LineTraceSingleByChannel(Hit, TraceStart, TraceEnd, ECC_Visibility))
		{
			PadLocation = Hit.ImpactPoint;
		}
	}

	LaunchPadOrigin = PadLocation;

	FActorSpawnParameters VehicleParams;
	LaunchVehicle = World->SpawnActor<AAstralisLaunchVehicleActor>(
		AAstralisLaunchVehicleActor::StaticClass(),
		PadLocation,
		FRotator::ZeroRotator,
		VehicleParams);

	if (!LaunchVehicle) return;

	// Build from selected vehicle definition
	if (VehicleLibrary.IsValidIndex(SelectedVehicleIndex))
	{
		const FLaunchVehicleDefinition& Def = VehicleLibrary[SelectedVehicleIndex];
		BuildVehicleFromDefinition(Def);

		// Pass vehicle dimensions for visual scaling
		LaunchVehicle->VehicleDisplayName = Def.DisplayName.ToString();
		if (Def.Stages.Num() > 0)
		{
			LaunchVehicle->VehicleDiameter_m = Def.Stages[0].Diameter_m;
			LaunchVehicle->VehicleS1Length_m = Def.Stages[0].Length_m;
			if (Def.Stages.Num() > 1)
				LaunchVehicle->VehicleS2Length_m = Def.Stages[1].Length_m;
		}

		// Set S1 burn time for adaptive gravity turn
		if (LaunchVehicle->Stages.Num() > 0 && LaunchVehicle->Stages[0])
		{
			LaunchVehicle->S1BurnTime_s = LaunchVehicle->Stages[0]->Spec.BurnTime_s;
		}
	}

	// Apply mission target altitude
	if (Missions.IsValidIndex(SelectedMissionIndex))
	{
		LaunchVehicle->SetTargetAltitude(Missions[SelectedMissionIndex].TargetAltitude_km * 1000.0);
	}

	LaunchVehicle->Payload = ActiveSpacecraft;

	const double TotalMass = LaunchVehicle->GetTotalMass_kg();
	const double DeltaV = LaunchVehicle->GetTotalDeltaV_mps();
	UE_LOG(LogTemp, Warning, TEXT("Vehicle: %s | Mass=%.0f t | DV=%.0f m/s"),
		VehicleLibrary.IsValidIndex(SelectedVehicleIndex) ? *VehicleLibrary[SelectedVehicleIndex].DisplayName.ToString() : TEXT("Unknown"),
		TotalMass / 1000.0, DeltaV);

	// Launch pad infrastructure — LC-39A style pad
	UStaticMesh* PadCylinder = LoadObject<UStaticMesh>(nullptr, TEXT("/Engine/BasicShapes/Cylinder.Cylinder"));
	UStaticMesh* PadCube = LoadObject<UStaticMesh>(nullptr, TEXT("/Engine/BasicShapes/Cube.Cube"));

	UMaterial* PadBaseMat = LoadObject<UMaterial>(nullptr,
		TEXT("/Engine/BasicShapes/BasicShapeMaterial.BasicShapeMaterial"));
	UMaterialInstanceDynamic* ConcreteMat = nullptr;
	UMaterialInstanceDynamic* MetalMat = nullptr;
	UMaterialInstanceDynamic* RedMat = nullptr;
	UMaterialInstanceDynamic* DarkMat = nullptr;
	if (PadBaseMat)
	{
		ConcreteMat = UMaterialInstanceDynamic::Create(PadBaseMat, this);
		ConcreteMat->SetVectorParameterValue(TEXT("Color"), FLinearColor(0.55f, 0.53f, 0.5f));
		MetalMat = UMaterialInstanceDynamic::Create(PadBaseMat, this);
		MetalMat->SetVectorParameterValue(TEXT("Color"), FLinearColor(0.35f, 0.37f, 0.4f));
		RedMat = UMaterialInstanceDynamic::Create(PadBaseMat, this);
		RedMat->SetVectorParameterValue(TEXT("Color"), FLinearColor(0.8f, 0.15f, 0.1f));
		DarkMat = UMaterialInstanceDynamic::Create(PadBaseMat, this);
		DarkMat->SetVectorParameterValue(TEXT("Color"), FLinearColor(0.12f, 0.12f, 0.14f));
	}

	if (PadCube && PadCylinder)
	{
		FActorSpawnParameters PadParams;
		LaunchPadActor = World->SpawnActor<AActor>(AActor::StaticClass(), PadLocation, FRotator::ZeroRotator, PadParams);
		if (LaunchPadActor)
		{
			USceneComponent* PadRoot = NewObject<USceneComponent>(LaunchPadActor, TEXT("PadRoot"));
			LaunchPadActor->SetRootComponent(PadRoot);
			PadRoot->RegisterComponent();
			PadRoot->SetWorldLocation(PadLocation);

			auto MakePart = [&](const TCHAR* Name, UStaticMesh* Mesh, FVector Loc, FVector Scale, FRotator Rot, UMaterialInterface* Mat)
			{
				auto* SMC = NewObject<UStaticMeshComponent>(LaunchPadActor, FName(Name));
				SMC->SetupAttachment(PadRoot);
				SMC->RegisterComponent();
				SMC->SetStaticMesh(Mesh);
				SMC->SetRelativeLocation(Loc);
				SMC->SetRelativeScale3D(Scale);
				SMC->SetRelativeRotation(Rot);
				SMC->SetCollisionProfileName(TEXT("NoCollision"));
				if (Mat) SMC->SetMaterial(0, Mat);
				return SMC;
			};

			// ── TERRAIN / GROUND ──────────────────────────────────────────
			// Large green ground plane — Florida coastal scrubland
			UMaterialInstanceDynamic* GrassMat = UMaterialInstanceDynamic::Create(PadBaseMat, this);
			GrassMat->SetVectorParameterValue(TEXT("Color"), FLinearColor(0.18f, 0.30f, 0.10f));
			UMaterialInstanceDynamic* DarkGrassMat = UMaterialInstanceDynamic::Create(PadBaseMat, this);
			DarkGrassMat->SetVectorParameterValue(TEXT("Color"), FLinearColor(0.12f, 0.22f, 0.08f));
			UMaterialInstanceDynamic* SandMat = UMaterialInstanceDynamic::Create(PadBaseMat, this);
			SandMat->SetVectorParameterValue(TEXT("Color"), FLinearColor(0.72f, 0.68f, 0.55f));
			UMaterialInstanceDynamic* WaterMat = UMaterialInstanceDynamic::Create(PadBaseMat, this);
			WaterMat->SetVectorParameterValue(TEXT("Color"), FLinearColor(0.05f, 0.18f, 0.35f));
			UMaterialInstanceDynamic* WhiteMat = UMaterialInstanceDynamic::Create(PadBaseMat, this);
			WhiteMat->SetVectorParameterValue(TEXT("Color"), FLinearColor(0.85f, 0.85f, 0.82f));
			UMaterialInstanceDynamic* OrangeMat = UMaterialInstanceDynamic::Create(PadBaseMat, this);
			OrangeMat->SetVectorParameterValue(TEXT("Color"), FLinearColor(0.9f, 0.45f, 0.1f));
			UMaterialInstanceDynamic* BlueMat = UMaterialInstanceDynamic::Create(PadBaseMat, this);
			BlueMat->SetVectorParameterValue(TEXT("Color"), FLinearColor(0.15f, 0.35f, 0.7f));
			UMaterialInstanceDynamic* YellowMat = UMaterialInstanceDynamic::Create(PadBaseMat, this);
			YellowMat->SetVectorParameterValue(TEXT("Color"), FLinearColor(0.9f, 0.8f, 0.2f));

			// Main ground plane — large green area
			MakePart(TEXT("GroundMain"), PadCube, FVector(0, 0, -120.0), FVector(500.0, 500.0, 0.5), FRotator::ZeroRotator, GrassMat);
			// Second layer to prevent seeing through edges
			MakePart(TEXT("GroundBase"), PadCube, FVector(0, 0, -150.0), FVector(600.0, 600.0, 0.5), FRotator::ZeroRotator, DarkGrassMat);

			// Ocean — east side (rocket launches east over Atlantic)
			MakePart(TEXT("Ocean"), PadCube, FVector(30000.0, 0, -180.0), FVector(500.0, 600.0, 0.3), FRotator::ZeroRotator, WaterMat);
			// Beach strip between land and ocean
			MakePart(TEXT("Beach"), PadCube, FVector(12000.0, 0, -130.0), FVector(60.0, 500.0, 0.3), FRotator::ZeroRotator, SandMat);

			// Scrubland patches — varied terrain
			MakePart(TEXT("Scrub1"), PadCube, FVector(3000.0, 2000.0, -110.0), FVector(30.0, 40.0, 0.4), FRotator::ZeroRotator, DarkGrassMat);
			MakePart(TEXT("Scrub2"), PadCube, FVector(-4000.0, -1500.0, -110.0), FVector(25.0, 35.0, 0.4), FRotator::ZeroRotator, DarkGrassMat);
			MakePart(TEXT("Scrub3"), PadCube, FVector(5000.0, -3000.0, -110.0), FVector(20.0, 30.0, 0.4), FRotator::ZeroRotator, DarkGrassMat);

			// ── LAUNCH PAD INFRASTRUCTURE ─────────────────────────────────
			// Main launch platform — wide concrete slab
			MakePart(TEXT("LaunchPlatform"), PadCube, FVector(0, 0, -50.0), FVector(20.0, 20.0, 1.0), FRotator::ZeroRotator, ConcreteMat);
			// Surrounding apron
			MakePart(TEXT("Apron"), PadCube, FVector(0, 0, -60.0), FVector(40.0, 40.0, 0.3), FRotator::ZeroRotator, ConcreteMat);
			// Pad perimeter road (ring around the pad)
			MakePart(TEXT("PerimeterN"), PadCube, FVector(0, 2200.0, -70.0), FVector(50.0, 2.0, 0.15), FRotator::ZeroRotator, ConcreteMat);
			MakePart(TEXT("PerimeterS"), PadCube, FVector(0, -2200.0, -70.0), FVector(50.0, 2.0, 0.15), FRotator::ZeroRotator, ConcreteMat);
			MakePart(TEXT("PerimeterE"), PadCube, FVector(2200.0, 0, -70.0), FVector(2.0, 50.0, 0.15), FRotator::ZeroRotator, ConcreteMat);
			MakePart(TEXT("PerimeterW"), PadCube, FVector(-2200.0, 0, -70.0), FVector(2.0, 50.0, 0.15), FRotator::ZeroRotator, ConcreteMat);

			// Transporter/Erector (strongback) — offset enough to not clip rocket
			MakePart(TEXT("Strongback"), PadCube, FVector(-450.0, 0, 2200.0), FVector(0.6, 0.6, 45.0), FRotator::ZeroRotator, MetalMat);
			// Strongback cradle arms — stop short of the rocket body
			MakePart(TEXT("CradleUpper"), PadCube, FVector(-350.0, 0, 3500.0), FVector(1.5, 0.2, 0.2), FRotator::ZeroRotator, MetalMat);
			MakePart(TEXT("CradleLower"), PadCube, FVector(-350.0, 0, 1500.0), FVector(1.5, 0.2, 0.2), FRotator::ZeroRotator, MetalMat);

			// Service tower (Fixed Service Structure) — further back
			MakePart(TEXT("ServiceTower"), PadCylinder, FVector(-1000.0, 0, 2800.0), FVector(2.5, 2.5, 56.0), FRotator::ZeroRotator, MetalMat);
			MakePart(TEXT("CraneArm"), PadCube, FVector(-500.0, 0, 5800.0), FVector(5.0, 0.4, 0.35), FRotator::ZeroRotator, MetalMat);

			// Lightning protection towers — positioned clear of flight path
			MakePart(TEXT("LightningMast1"), PadCylinder, FVector(-1000.0, 0, 6200.0), FVector(0.25, 0.25, 14.0), FRotator::ZeroRotator, RedMat);
			MakePart(TEXT("LightningMast2"), PadCylinder, FVector(800.0, 800.0, 4500.0), FVector(0.2, 0.2, 10.0), FRotator::ZeroRotator, RedMat);
			MakePart(TEXT("LightningMast3"), PadCylinder, FVector(800.0, -800.0, 4500.0), FVector(0.2, 0.2, 10.0), FRotator::ZeroRotator, RedMat);

			// Flame trench — runs under the pad
			MakePart(TEXT("FlameTrench"), PadCube, FVector(0, 0, -250.0), FVector(5.0, 25.0, 4.0), FRotator::ZeroRotator, DarkMat);
			// Flame deflector — angled steel plate
			MakePart(TEXT("FlameDeflector"), PadCube, FVector(250.0, 0, -100.0), FVector(5.0, 10.0, 0.6), FRotator(0, 0, 25.0), MetalMat);

			// Water deluge system — pipes
			for (int32 i = 0; i < 4; ++i)
			{
				FName PipeName(*FString::Printf(TEXT("WaterPipe_%d"), i));
				MakePart(*PipeName.ToString(), PadCylinder,
					FVector(-350.0 + i * 60.0, 400.0, 100.0),
					FVector(0.12, 0.12, 4.0),
					FRotator(0, 0, 85.0), MetalMat);
			}

			// Water tower
			MakePart(TEXT("WaterTowerBase"), PadCylinder, FVector(-1800.0, 800.0, 600.0), FVector(1.0, 1.0, 12.0), FRotator::ZeroRotator, MetalMat);
			MakePart(TEXT("WaterTowerTank"), PadCylinder, FVector(-1800.0, 800.0, 1400.0), FVector(4.0, 4.0, 4.0), FRotator::ZeroRotator, WhiteMat);

			// ── SURROUNDING BUILDINGS ──────────────────────────────────────
			// Vehicle Assembly Building (VAB) silhouette — large white building in the distance
			MakePart(TEXT("VAB"), PadCube, FVector(-8000.0, -4000.0, 2500.0), FVector(30.0, 25.0, 50.0), FRotator::ZeroRotator, WhiteMat);
			MakePart(TEXT("VABRoof"), PadCube, FVector(-8000.0, -4000.0, 5050.0), FVector(32.0, 27.0, 1.0), FRotator::ZeroRotator, MetalMat);
			// NASA logo stripe on VAB (blue band)
			MakePart(TEXT("VABStripe"), PadCube, FVector(-8000.0 + 1510.0, -4000.0, 3500.0), FVector(0.3, 18.0, 15.0), FRotator::ZeroRotator, BlueMat);

			// Launch Control Center
			MakePart(TEXT("LCC"), PadCube, FVector(-6000.0, -2000.0, 300.0), FVector(15.0, 10.0, 6.0), FRotator::ZeroRotator, WhiteMat);
			MakePart(TEXT("LCCRoof"), PadCube, FVector(-6000.0, -2000.0, 620.0), FVector(16.0, 11.0, 0.3), FRotator::ZeroRotator, MetalMat);

			// Press site viewing stands
			MakePart(TEXT("PressStand"), PadCube, FVector(-4000.0, 3000.0, 100.0), FVector(8.0, 3.0, 2.5), FRotator::ZeroRotator, ConcreteMat);

			// Fuel storage tanks (LOX/RP-1 farm)
			for (int32 i = 0; i < 3; ++i)
			{
				FName TankName(*FString::Printf(TEXT("FuelTank_%d"), i));
				MakePart(*TankName.ToString(), PadCylinder,
					FVector(-2500.0, -1200.0 + i * 600.0, 300.0),
					FVector(2.5, 2.5, 6.0), FRotator::ZeroRotator, WhiteMat);
			}
			// LOX tank (slightly blue tint)
			MakePart(TEXT("LOXTank"), PadCylinder, FVector(-2500.0, 800.0, 400.0), FVector(3.5, 3.5, 8.0), FRotator::ZeroRotator, WhiteMat);

			// ── ACCESS ROADS ──────────────────────────────────────────────
			// Main road from VAB to pad (crawlerway)
			MakePart(TEXT("Crawlerway"), PadCube, FVector(-4000.0, -2000.0, -75.0), FVector(80.0, 4.0, 0.15), FRotator::ZeroRotator, ConcreteMat);
			// Access road south
			MakePart(TEXT("AccessRoad"), PadCube, FVector(0, -3000.0, -75.0), FVector(4.0, 60.0, 0.15), FRotator::ZeroRotator, ConcreteMat);
			// Perimeter road to press site
			MakePart(TEXT("PressRoad"), PadCube, FVector(-2000.0, 3000.0, -75.0), FVector(40.0, 2.0, 0.15), FRotator::ZeroRotator, ConcreteMat);

			// ── SAFETY / MARKERS ──────────────────────────────────────────
			// Range safety markers (orange poles)
			for (int32 i = 0; i < 6; ++i)
			{
				FName PoleName(*FString::Printf(TEXT("SafetyPole_%d"), i));
				const double Angle = (double)i / 6.0 * 2.0 * PI;
				MakePart(*PoleName.ToString(), PadCylinder,
					FVector(FMath::Cos(Angle) * 3000.0, FMath::Sin(Angle) * 3000.0, 150.0),
					FVector(0.15, 0.15, 3.0), FRotator::ZeroRotator, OrangeMat);
			}

			// Pad 39A designation markers (yellow stripe)
			MakePart(TEXT("PadMarkerL"), PadCube, FVector(-800.0, -600.0, -55.0), FVector(0.3, 12.0, 0.2), FRotator::ZeroRotator, YellowMat);
			MakePart(TEXT("PadMarkerR"), PadCube, FVector(-800.0, 600.0, -55.0), FVector(0.3, 12.0, 0.2), FRotator::ZeroRotator, YellowMat);

			// ── VEGETATION / TREE CLUSTERS ────────────────────────────────
			// Florida scrubland palm/tree clusters (dark green blocks at ground level)
			UMaterialInstanceDynamic* TreeMat = UMaterialInstanceDynamic::Create(PadBaseMat, this);
			TreeMat->SetVectorParameterValue(TEXT("Color"), FLinearColor(0.08f, 0.22f, 0.06f));
			UMaterialInstanceDynamic* TreeLightMat = UMaterialInstanceDynamic::Create(PadBaseMat, this);
			TreeLightMat->SetVectorParameterValue(TEXT("Color"), FLinearColor(0.12f, 0.30f, 0.08f));

			// Tree clusters scattered around the launch site
			struct FTreeCluster { FVector Pos; FVector Scale; };
			const FTreeCluster Trees[] = {
				{{4000.0, 1500.0, 100.0}, {8.0, 6.0, 3.0}},
				{{4500.0, -2000.0, 80.0}, {6.0, 10.0, 2.5}},
				{{-3000.0, 3500.0, 90.0}, {10.0, 7.0, 2.8}},
				{{-5000.0, 1000.0, 100.0}, {12.0, 8.0, 3.2}},
				{{6000.0, 4000.0, 70.0}, {15.0, 10.0, 2.5}},
				{{-2000.0, -5000.0, 80.0}, {8.0, 12.0, 2.8}},
				{{7000.0, -1000.0, 60.0}, {10.0, 6.0, 2.2}},
				{{-7000.0, -3000.0, 90.0}, {14.0, 9.0, 3.0}},
				{{2000.0, 6000.0, 70.0}, {9.0, 11.0, 2.6}},
				{{8000.0, 2000.0, 50.0}, {7.0, 7.0, 2.0}},
				{{-4000.0, 5000.0, 80.0}, {11.0, 8.0, 2.7}},
				{{5500.0, 5500.0, 60.0}, {13.0, 9.0, 2.4}},
			};
			for (int32 i = 0; i < UE_ARRAY_COUNT(Trees); ++i)
			{
				FName TreeName(*FString::Printf(TEXT("TreeCluster_%d"), i));
				MakePart(*TreeName.ToString(), PadCube, Trees[i].Pos, Trees[i].Scale, FRotator::ZeroRotator,
					(i % 2 == 0) ? TreeMat : TreeLightMat);
			}

			// Individual taller palm tree trunks (cylinders)
			UMaterialInstanceDynamic* TrunkMat = UMaterialInstanceDynamic::Create(PadBaseMat, this);
			TrunkMat->SetVectorParameterValue(TEXT("Color"), FLinearColor(0.35f, 0.25f, 0.15f));
			for (int32 i = 0; i < 8; ++i)
			{
				FName TrunkName(*FString::Printf(TEXT("PalmTrunk_%d"), i));
				const double Angle = (double)i / 8.0 * 2.0 * PI;
				const double Dist = 4500.0 + (i % 3) * 1200.0;
				MakePart(*TrunkName.ToString(), PadCylinder,
					FVector(FMath::Cos(Angle) * Dist, FMath::Sin(Angle) * Dist, 200.0),
					FVector(0.1, 0.1, 4.0), FRotator::ZeroRotator, TrunkMat);
				// Palm canopy
				FName CanopyName(*FString::Printf(TEXT("PalmCanopy_%d"), i));
				MakePart(*CanopyName.ToString(), PadCube,
					FVector(FMath::Cos(Angle) * Dist, FMath::Sin(Angle) * Dist, 450.0),
					FVector(1.5, 1.5, 0.8), FRotator::ZeroRotator, TreeLightMat);
			}

			// ── MORE DISTANT BUILDINGS ────────────────────────────────────
			// Operations Support Building
			MakePart(TEXT("OSB"), PadCube, FVector(-10000.0, 2000.0, 200.0), FVector(10.0, 8.0, 4.0), FRotator::ZeroRotator, WhiteMat);
			// Payload Processing Facility
			MakePart(TEXT("PPF"), PadCube, FVector(-9000.0, -1000.0, 250.0), FVector(8.0, 12.0, 5.0), FRotator::ZeroRotator, WhiteMat);
			// Tracking station antenna dishes
			for (int32 i = 0; i < 3; ++i)
			{
				FName DishName(*FString::Printf(TEXT("TrackDish_%d"), i));
				MakePart(*DishName.ToString(), PadCylinder,
					FVector(-7000.0 + i * 400.0, 5000.0, 200.0),
					FVector(1.5, 1.5, 0.3), FRotator::ZeroRotator, WhiteMat);
				FName DishPole(*FString::Printf(TEXT("DishPole_%d"), i));
				MakePart(*DishPole.ToString(), PadCylinder,
					FVector(-7000.0 + i * 400.0, 5000.0, 100.0),
					FVector(0.1, 0.1, 2.0), FRotator::ZeroRotator, MetalMat);
			}

			// Parking lot near LCC
			UMaterialInstanceDynamic* AsphaltMat = UMaterialInstanceDynamic::Create(PadBaseMat, this);
			AsphaltMat->SetVectorParameterValue(TEXT("Color"), FLinearColor(0.18f, 0.18f, 0.2f));
			MakePart(TEXT("ParkingLot"), PadCube, FVector(-6500.0, -3000.0, -72.0), FVector(10.0, 8.0, 0.12), FRotator::ZeroRotator, AsphaltMat);

			// Additional road connecting buildings
			MakePart(TEXT("ServiceRoad"), PadCube, FVector(-7000.0, 0, -75.0), FVector(2.0, 120.0, 0.12), FRotator::ZeroRotator, AsphaltMat);

			// Distant shoreline dunes
			MakePart(TEXT("Dunes1"), PadCube, FVector(11000.0, 3000.0, -100.0), FVector(15.0, 50.0, 0.5), FRotator::ZeroRotator, SandMat);
			MakePart(TEXT("Dunes2"), PadCube, FVector(11000.0, -3000.0, -100.0), FVector(15.0, 50.0, 0.5), FRotator::ZeroRotator, SandMat);

			// ── RADIO / COMMUNICATION TOWERS ─────────────────────────────
			UMaterialInstanceDynamic* TowerMat = UMaterialInstanceDynamic::Create(PadBaseMat, this);
			TowerMat->SetVectorParameterValue(TEXT("Color"), FLinearColor(0.5f, 0.1f, 0.05f));
			for (int32 i = 0; i < 3; ++i)
			{
				FName TowerName(*FString::Printf(TEXT("RadioTower_%d"), i));
				MakePart(*TowerName.ToString(), PadCylinder,
					FVector(-12000.0 + i * 2000.0, 7000.0, 400.0),
					FVector(0.15, 0.15, 8.0), FRotator::ZeroRotator, TowerMat);
				// Red aviation warning light on top
				UMaterialInstanceDynamic* LightMat = UMaterialInstanceDynamic::Create(PadBaseMat, this);
				LightMat->SetVectorParameterValue(TEXT("Color"), FLinearColor(1.0f, 0.05f, 0.02f));
				FName LightName(*FString::Printf(TEXT("TowerLight_%d"), i));
				MakePart(*LightName.ToString(), PadCube,
					FVector(-12000.0 + i * 2000.0, 7000.0, 820.0),
					FVector(0.3, 0.3, 0.3), FRotator::ZeroRotator, LightMat);
			}

			// ── LIGHTHOUSE at the cape ──────────────────────────────────
			UMaterialInstanceDynamic* LighthouseMat = UMaterialInstanceDynamic::Create(PadBaseMat, this);
			LighthouseMat->SetVectorParameterValue(TEXT("Color"), FLinearColor(0.9f, 0.9f, 0.85f));
			MakePart(TEXT("Lighthouse"), PadCylinder,
				FVector(15000.0, 8000.0, 250.0), FVector(0.6, 0.6, 5.0), FRotator::ZeroRotator, LighthouseMat);
			// Red stripe
			UMaterialInstanceDynamic* LhRedMat = UMaterialInstanceDynamic::Create(PadBaseMat, this);
			LhRedMat->SetVectorParameterValue(TEXT("Color"), FLinearColor(0.8f, 0.15f, 0.1f));
			MakePart(TEXT("LhStripe"), PadCylinder,
				FVector(15000.0, 8000.0, 350.0), FVector(0.65, 0.65, 0.8), FRotator::ZeroRotator, LhRedMat);
			// Lantern room
			UMaterialInstanceDynamic* LhLightMat = UMaterialInstanceDynamic::Create(PadBaseMat, this);
			LhLightMat->SetVectorParameterValue(TEXT("Color"), FLinearColor(1.0f, 0.95f, 0.5f));
			MakePart(TEXT("LhLantern"), PadCylinder,
				FVector(15000.0, 8000.0, 530.0), FVector(0.4, 0.4, 0.8), FRotator::ZeroRotator, LhLightMat);

			// ── FLAME TRENCH — below the launch pad ─────────────────────
			UMaterialInstanceDynamic* TrenchMat = UMaterialInstanceDynamic::Create(PadBaseMat, this);
			TrenchMat->SetVectorParameterValue(TEXT("Color"), FLinearColor(0.12f, 0.10f, 0.08f));
			MakePart(TEXT("FlameTrench"), PadCube,
				FVector(300.0, 0.0, -130.0), FVector(3.0, 12.0, 0.8), FRotator::ZeroRotator, TrenchMat);
			// Flame deflector
			UMaterialInstanceDynamic* DeflectorMat = UMaterialInstanceDynamic::Create(PadBaseMat, this);
			DeflectorMat->SetVectorParameterValue(TEXT("Color"), FLinearColor(0.3f, 0.25f, 0.2f));
			MakePart(TEXT("FlameDeflector"), PadCube,
				FVector(300.0, 0.0, -90.0), FVector(2.5, 6.0, 0.4), FRotator(10.0, 0.0, 0.0), DeflectorMat);

			// ── STRONGBACK / TRANSPORTER ERECTOR ─────────────────────────
			MakePart(TEXT("Strongback"), PadCube,
				FVector(400.0, 0.0, 1800.0), FVector(0.8, 2.0, 36.0), FRotator::ZeroRotator, MetalMat);
			// Strongback arm hinges
			MakePart(TEXT("SB_Arm1"), PadCube,
				FVector(300.0, -50.0, 1200.0), FVector(2.0, 0.3, 0.3), FRotator::ZeroRotator, MetalMat);
			MakePart(TEXT("SB_Arm2"), PadCube,
				FVector(300.0, -50.0, 2400.0), FVector(2.0, 0.3, 0.3), FRotator::ZeroRotator, MetalMat);
			MakePart(TEXT("SB_Arm3"), PadCube,
				FVector(300.0, -50.0, 3200.0), FVector(2.0, 0.3, 0.3), FRotator::ZeroRotator, MetalMat);

			// ── WATER SOUND SUPPRESSION SYSTEM PIPES ──────────────────────
			UMaterialInstanceDynamic* PipeMat = UMaterialInstanceDynamic::Create(PadBaseMat, this);
			PipeMat->SetVectorParameterValue(TEXT("Color"), FLinearColor(0.4f, 0.45f, 0.5f));
			MakePart(TEXT("WaterPipe1"), PadCylinder,
				FVector(-200.0, 200.0, -20.0), FVector(0.15, 0.15, 3.0), FRotator::ZeroRotator, PipeMat);
			MakePart(TEXT("WaterPipe2"), PadCylinder,
				FVector(-200.0, -200.0, -20.0), FVector(0.15, 0.15, 3.0), FRotator::ZeroRotator, PipeMat);

			// ── DISTANT CAPE CANAVERAL BUILDINGS ─────────────────────────
			for (int32 i = 0; i < 5; ++i)
			{
				FName BldgName(*FString::Printf(TEXT("DistBldg_%d"), i));
				float BldgH = 3.0f + FMath::FRandRange(1.0f, 5.0f);
				MakePart(*BldgName.ToString(), PadCube,
					FVector(-18000.0 + i * 3500.0, -10000.0, BldgH * 50.0f),
					FVector(4.0 + i * 0.5, 3.0 + i * 0.3, BldgH), FRotator::ZeroRotator, WhiteMat);
			}
		}
	}
}

void AAstralisAssemblyGameMode::BuildVehicleFromDefinition(const FLaunchVehicleDefinition& Def)
{
	UWorld* World = GetWorld();
	if (!World || !LaunchVehicle) return;

	for (int32 i = 0; i < Def.Stages.Num(); ++i)
	{
		const FStageDefinition& SD = Def.Stages[i];

		FActorSpawnParameters StageParams;
		AAstralisStageActor* Stage = World->SpawnActor<AAstralisStageActor>(
			AAstralisStageActor::StaticClass(),
			FVector::ZeroVector, FRotator::ZeroRotator, StageParams);

		if (!Stage) continue;

		Stage->StageNumber = i + 1;
		Stage->Spec.StageName = SD.StageName;
		Stage->Spec.DryMass_kg = SD.DryMass_kg;
		Stage->Spec.PropellantCapacity_kg = SD.PropellantCapacity_kg;
		Stage->Spec.Thrust_N = SD.EngineSpec.ThrustSeaLevel_N * SD.NumEngines;
		Stage->Spec.ThrustVacuum_N = SD.EngineSpec.ThrustVacuum_N * SD.NumEngines;
		Stage->Spec.Isp_s = SD.EngineSpec.IspSeaLevel_s;
		Stage->Spec.IspVacuum_s = SD.EngineSpec.IspVacuum_s;
		Stage->Spec.NumEngines = SD.NumEngines;
		Stage->Spec.SingleEngineMass_kg = SD.EngineSpec.Mass_kg;
		Stage->Spec.EngineName = SD.EngineSpec.Name;

		double AvgIsp = (SD.EngineSpec.IspSeaLevel_s > 0)
			? SD.EngineSpec.IspSeaLevel_s
			: SD.EngineSpec.IspVacuum_s;
		if (AvgIsp > 0 && Stage->Spec.Thrust_N > 0)
		{
			double mdot = Stage->Spec.Thrust_N / (AvgIsp * 9.80665);
			Stage->Spec.BurnTime_s = SD.PropellantCapacity_kg / mdot;
		}

		Stage->CurrentPropellant_kg = SD.PropellantCapacity_kg;
		LaunchVehicle->Stages.Add(Stage);
	}
}

void AAstralisAssemblyGameMode::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (CurrentPhase == EAstralisGamePhase::Launch && LaunchVehicle)
	{
		if (AAstralisAssemblyPawn* Pawn = GetCameraPawn(GetWorld()))
		{
			const double Alt_km = LaunchVehicle->Altitude_m / 1000.0;
			const double PitchRad = FMath::DegreesToRadians(LaunchVehicle->GetPitchAngle_deg());

			// Focus on the rocket's rotated visual center, not a fixed Z offset.
			// The rocket mesh extends ~3800 UU along its body axis from the actor origin.
			// As it tilts during gravity turn, we follow the center of the visible body.
			const double HalfBody = 1900.0;
			const FVector RocketCenter = LaunchVehicle->GetActorLocation() + FVector(
				HalfBody * FMath::Cos(PitchRad),
				0.0,
				HalfBody * FMath::Sin(PitchRad));

			float DesiredDist, DesiredPitch;
			float InterpSpeed = 2.0f;

			switch (Pawn->CameraMode)
			{
			case AAstralisAssemblyPawn::ECameraMode::Close:
				DesiredDist = FMath::Clamp(4000.0f + static_cast<float>(Alt_km) * 12.0f, 4000.0f, 12000.0f);
				DesiredPitch = -3.0f;
				InterpSpeed = 3.0f;
				break;
			case AAstralisAssemblyPawn::ECameraMode::Wide:
				DesiredDist = FMath::Clamp(12000.0f + static_cast<float>(Alt_km) * 50.0f, 12000.0f, 40000.0f);
				DesiredPitch = -12.0f;
				InterpSpeed = 1.5f;
				break;
			default: // Tracking
				DesiredDist = FMath::Clamp(6000.0f + static_cast<float>(Alt_km) * 25.0f, 6000.0f, 20000.0f);
				DesiredPitch = -5.0f;
				InterpSpeed = 2.0f;
				break;
			}

			Pawn->SetFocusPoint(RocketCenter);

			if (Pawn->CameraBoom)
			{
				Pawn->CameraBoom->TargetArmLength = FMath::FInterpTo(
					Pawn->CameraBoom->TargetArmLength, DesiredDist, DeltaTime, InterpSpeed);

				// View from the side (-90° Yaw) so gravity turn looks like a broadcast
				FRotator CurrRot = Pawn->CameraBoom->GetComponentRotation();
				FRotator TargetRot(DesiredPitch, -90.0f, 0.0f);
				Pawn->CameraBoom->SetWorldRotation(
					FMath::RInterpTo(CurrRot, TargetRot, DeltaTime, 1.5f));
			}

			// Camera shake — strongest at liftoff, fades with altitude
			const float AtmAtten = FMath::Max(0.0f, 1.0f - static_cast<float>(Alt_km) / 50.0f);
			const float LiftoffBoost = FMath::Max(0.0f, 1.0f - static_cast<float>(LaunchVehicle->MissionElapsedTime_s) / 8.0f);
			float ShakeBase = (Pawn->CameraMode == AAstralisAssemblyPawn::ECameraMode::Close) ? 14.0f : 8.0f;
			Pawn->SetShakeIntensity(AtmAtten * (ShakeBase + LiftoffBoost * 25.0f));
		}

		// Hide launch pad infrastructure once rocket clears the tower
		if (LaunchPadActor && LaunchVehicle->Altitude_m > 500.0 && !LaunchPadActor->IsHidden())
		{
			LaunchPadActor->SetActorHiddenInGame(true);
		}

		// Hide only the landscape terrain above 30km so terrain doesn't clip the rocket.
		// Keep SkyAtmosphere/fog/clouds so the sky fades naturally to space.
		if (LaunchVehicle->Altitude_m > 30000.0 && !bLandscapeHiddenForLaunch)
		{
			for (TActorIterator<AActor> It(GetWorld()); It; ++It)
			{
				AActor* Actor = *It;
				for (UActorComponent* Comp : Actor->GetComponents())
				{
					if (Comp->GetClass()->GetName().Contains(TEXT("Landscape")))
					{
						Actor->SetActorHiddenInGame(true);
						HiddenForOrbit.Add(Actor);
						break;
					}
				}
			}
			if (LaunchPadActor && !LaunchPadActor->IsHidden())
				LaunchPadActor->SetActorHiddenInGame(true);
			bLandscapeHiddenForLaunch = true;
		}

		// Liftoff exhaust cloud — massive water deluge + engine exhaust billow
		const bool bInFlight = LaunchVehicle->MissionState != ELaunchVehicleState::OnPad &&
			LaunchVehicle->MissionState != ELaunchVehicleState::Countdown;
		if (bInFlight && LaunchVehicle->MissionElapsedTime_s < 15.0 && SmokePuffs.Num() < 200)
		{
			// Spawn multiple puffs per frame for dense cloud
			const int32 SpawnCount = (LaunchVehicle->MissionElapsedTime_s < 3.0) ? 5 : 2;
			for (int32 s = 0; s < SpawnCount; ++s)
			{
				FSmokePuff P;
				if (LaunchVehicle->MissionElapsedTime_s < 5.0)
				{
					// Ground-level billow — spreads outward from flame trench
					P.Position = LaunchPadOrigin + FVector(
						FMath::FRandRange(-200.0f, 200.0f),
						FMath::FRandRange(-200.0f, 200.0f),
						FMath::FRandRange(-40.0f, 60.0f));
					const float Outward = FMath::FRandRange(80.0f, 250.0f);
					const float Dir = FMath::FRandRange(0.0f, 360.0f);
					P.Velocity = FVector(
						FMath::Cos(FMath::DegreesToRadians(Dir)) * Outward,
						FMath::Sin(FMath::DegreesToRadians(Dir)) * Outward,
						FMath::FRandRange(20.0f, 80.0f));
				}
				else
				{
					// Rising column — follows the rocket upward
					P.Position = LaunchPadOrigin + FVector(
						FMath::FRandRange(-100.0f, 100.0f),
						FMath::FRandRange(-100.0f, 100.0f),
						FMath::FRandRange(0.0f, 300.0f));
					P.Velocity = FVector(
						FMath::FRandRange(-60.0f, 60.0f),
						FMath::FRandRange(-60.0f, 60.0f),
						FMath::FRandRange(40.0f, 120.0f));
				}
				P.Scale = FMath::FRandRange(8.0f, 20.0f);
				P.Life = 0.0f;
				SmokePuffs.Add(P);
			}
		}
		for (int32 i = SmokePuffs.Num() - 1; i >= 0; --i)
		{
			FSmokePuff& S = SmokePuffs[i];
			S.Position += S.Velocity * DeltaTime;
			S.Velocity *= (1.0f - 1.2f * DeltaTime);
			S.Scale += DeltaTime * 2.0f;
			S.Life += DeltaTime;
			if (S.Life > 8.0f) { SmokePuffs.RemoveAt(i); continue; }
			const float Alpha = FMath::Max(0.0f, 1.0f - S.Life / 8.0f);
			const uint8 Gray = static_cast<uint8>(210 * Alpha);
			const uint8 Warm = (S.Life < 1.5f) ? static_cast<uint8>(80 * FMath::Max(0.0f, 1.0f - S.Life / 1.5f)) : 0;
			DrawDebugPoint(GetWorld(), S.Position, S.Scale,
				FColor(FMath::Min(255, Gray + Warm), Gray, FMath::Max(0, Gray - Warm / 2)), false, -1.0f, 0);
		}

		// Delay orbit transition so user can read the mission success stats
		if (LaunchVehicle->MissionState == ELaunchVehicleState::Complete && LaunchVehicle->bOrbitAchieved)
		{
			if (!bWaitingForOrbitTransition)
			{
				bWaitingForOrbitTransition = true;
				OrbitTransitionTimer = 5.0f;
			}
			OrbitTransitionTimer -= DeltaTime;
			if (OrbitTransitionTimer <= 0.0f)
			{
				bWaitingForOrbitTransition = false;
				TransitionToOrbit();
			}
		}
	}

	// Reentry phase simulation
	if (CurrentPhase == EAstralisGamePhase::Reentry)
	{
		if (!bReentryComplete)
		{
			ReentryTimer += DeltaTime;
			const double TimeScale = 20.0;
			const double SimStep = DeltaTime * TimeScale;

			// Deorbit burn phase (first 3 real seconds = ~60 sim seconds)
			if (ReentryTimer < 3.0f)
			{
				ReentryVelocity_mps -= 2.0 * SimStep;
				ReentryAltitude_km -= 0.8 * SimStep;
			}
			else
			{
				// Atmospheric reentry — exponential drag deceleration
				const double DragFactor = (ReentryAltitude_km < 120.0) ?
					FMath::Exp(-ReentryAltitude_km / 40.0) * 50.0 : 0.5;
				ReentryVelocity_mps -= DragFactor * SimStep;
				ReentryVelocity_mps = FMath::Max(0.0, ReentryVelocity_mps);

				// Altitude descent rate increases as we go lower
				const double DescentRate = (ReentryAltitude_km > 40.0) ? 3.0 :
					(ReentryAltitude_km > 10.0) ? 1.5 : 0.5;
				ReentryAltitude_km -= DescentRate * SimStep;
			}

			// Temperature model — peaks around 50-70km altitude
			if (ReentryAltitude_km < 120.0 && ReentryAltitude_km > 20.0)
			{
				const double HeatingFactor = FMath::Exp(-(ReentryAltitude_km - 50.0) * (ReentryAltitude_km - 50.0) / 1800.0);
				const double VelFactor = FMath::Clamp(ReentryVelocity_mps / 7000.0, 0.0, 1.0);
				ReentryTemperature_C = 20.0 + 2200.0 * HeatingFactor * VelFactor;
			}
			else if (ReentryAltitude_km <= 20.0)
			{
				ReentryTemperature_C = FMath::FInterpTo(ReentryTemperature_C, 40.0, DeltaTime, 1.0);
			}

			// Plasma blackout: 80-40 km at high speed
			bPlasmaBlackout = (ReentryAltitude_km < 80.0 && ReentryAltitude_km > 40.0 &&
				ReentryVelocity_mps > 3000.0);

			// Parachute deployment below 10 km
			if (ReentryAltitude_km < 10.0 && !bParachuteDeployed)
			{
				bParachuteDeployed = true;
				ReentryVelocity_mps = FMath::Min(ReentryVelocity_mps, 200.0);
			}

			if (bParachuteDeployed)
			{
				ReentryVelocity_mps = FMath::FInterpTo(ReentryVelocity_mps, 8.0, DeltaTime, 2.0);
			}

			// Splashdown
			if (ReentryAltitude_km <= 0.0)
			{
				ReentryAltitude_km = 0.0;
				ReentryVelocity_mps = 0.0;
				bReentryComplete = true;
				ReentrySplashTimer = 0.0f;
				UE_LOG(LogTemp, Warning, TEXT("=== SPLASHDOWN — MISSION COMPLETE ==="));
			}
		}
		else
		{
			// Auto-return after splashdown
			ReentrySplashTimer += DeltaTime;
			if (ReentrySplashTimer > 8.0f)
			{
				ReturnToAssembly();
			}
		}
	}

	// Orbit phase — spacecraft resource management
	if (CurrentPhase == EAstralisGamePhase::Orbit && OrbitScene)
	{
		UpdateSpacecraftResources(DeltaTime);
	}
}

void AAstralisAssemblyGameMode::RecordFlightResult()
{
	if (!LaunchVehicle) return;

	FFlightRecord Record;
	Record.VehicleName = LaunchVehicle->VehicleDisplayName;
	Record.MissionName = (Missions.Num() > 0) ? GetSelectedMission().Name : TEXT("Unknown");
	Record.Grade = LaunchVehicle->MissionGrade;
	Record.Score = LaunchVehicle->MissionScore;
	Record.MaxAltitude_km = LaunchVehicle->Altitude_m / 1000.0;
	Record.MaxVelocity_mps = LaunchVehicle->Velocity_mps;
	Record.bSuccess = LaunchVehicle->bOrbitAchieved;

	FlightHistory.Add(Record);
	TotalLaunches++;
	if (Record.bSuccess) SuccessfulMissions++;
	if (Record.Score > BestScore)
	{
		BestScore = Record.Score;
		BestGrade = Record.Grade;
	}
}

void AAstralisAssemblyGameMode::TransitionToOrbit()
{
	RecordFlightResult();
	CurrentPhase = EAstralisGamePhase::Orbit;

	double PropMass = 0.0;
	for (const auto& Comp : InstalledComponents)
	{
		if (Comp && Comp->ComponentData &&
			Comp->ComponentData->Category == EComponentCategory::Propulsion)
		{
			PropMass += Comp->ComponentData->Mass_kg;
		}
	}

	FActorSpawnParameters Params;
	OrbitScene = GetWorld()->SpawnActor<AAstralisOrbitSceneActor>(
		AAstralisOrbitSceneActor::StaticClass(),
		FVector(0.0, 0.0, 200000.0),
		FRotator::ZeroRotator,
		Params);

	if (OrbitScene && ActiveSpacecraft)
	{
		OrbitScene->InitializeFromAssembly(
			ActiveSpacecraft->ComponentEntries, PropMass);
	}

	// Hide launch vehicle
	if (LaunchVehicle)
	{
		LaunchVehicle->SetActorHiddenInGame(true);
		LaunchVehicle->SetActorTickEnabled(false);
	}

	SetLevelEnvironmentVisible(false);

	// Frame the whole Earth plus the orbit track
	if (AAstralisAssemblyPawn* Pawn = GetCameraPawn(GetWorld()))
	{
		if (OrbitScene)
		{
			Pawn->FocusOn(OrbitScene->GetActorLocation(), 2600.0f, 90.0f, -25.0f);
		}
	}

	if (AAstralisAssemblyPC* PC = Cast<AAstralisAssemblyPC>(
		GetWorld()->GetFirstPlayerController()))
	{
		PC->OnPhaseChanged(CurrentPhase);
	}

	InitSpacecraftResources();

	UE_LOG(LogTemp, Warning, TEXT("=== NOW IN ORBIT — Spacecraft deployed ==="));
}

void AAstralisAssemblyGameMode::TransitionToReentry()
{
	if (CurrentPhase != EAstralisGamePhase::Orbit) return;

	CurrentPhase = EAstralisGamePhase::Reentry;

	// Initialize reentry state from orbit
	if (OrbitScene)
	{
		const FSpacecraftState& SC = OrbitScene->GetSpacecraftState();
		const double R_m = SC.Orbital.Position_m.Size();
		ReentryAltitude_km = (R_m - 6.371e6) / 1000.0;
		ReentryVelocity_mps = SC.Orbital.Velocity_mps.Size();

		OrbitScene->Destroy();
		OrbitScene = nullptr;
	}
	else
	{
		ReentryAltitude_km = 400.0;
		ReentryVelocity_mps = 7670.0;
	}

	ReentryTimer = 0.0f;
	ReentrySplashTimer = 0.0f;
	ReentryTemperature_C = 20.0;
	bReentryComplete = false;
	bPlasmaBlackout = false;
	bParachuteDeployed = false;

	SetLevelEnvironmentVisible(true);

	if (AAstralisAssemblyPawn* Pawn = GetCameraPawn(GetWorld()))
	{
		Pawn->FocusOn(FVector(0, 0, 5000.0), 8000.0f, 60.0f, -15.0f);
	}

	if (AAstralisAssemblyPC* PC = Cast<AAstralisAssemblyPC>(
		GetWorld()->GetFirstPlayerController()))
	{
		PC->OnPhaseChanged(CurrentPhase);
	}

	UE_LOG(LogTemp, Warning, TEXT("=== DEORBIT BURN — REENTRY INITIATED ==="));
}

// ─────────────────────────────────────────────────────────────────────────────
// SPACECRAFT BUS CATALOG
// ─────────────────────────────────────────────────────────────────────────────
void AAstralisAssemblyGameMode::InitBusCatalog()
{
	auto Add = [&](FName ID, const TCHAR* Name, const TCHAR* Desc,
		double DryMass, double MaxPay, double Solar, double PwrGen, double Bat,
		double Ant, double TxPwr, double DataRate, double Storage,
		double Thermal, double Prop, double ADCS, double Cost, int32 Slots, double Life)
	{
		FSpacecraftBusDefinition B;
		B.ID = ID; B.Name = Name; B.Description = Desc;
		B.DryMass_kg = DryMass; B.MaxPayloadMass_kg = MaxPay;
		B.SolarPanelArea_m2 = Solar; B.PowerGeneration_W = PwrGen;
		B.BatteryCapacity_Wh = Bat; B.AntennaGain_dBi = Ant;
		B.TransmitPower_W = TxPwr; B.MaxDataRate_Mbps = DataRate;
		B.DataStorage_GB = Storage; B.ThermalCapacity_W = Thermal;
		B.PropellantMass_kg = Prop; B.ADCS_PointingAccuracy_deg = ADCS;
		B.Cost_M = Cost; B.MaxInstrumentSlots = Slots; B.DesignLife_years = Life;
		BusCatalog.Add(B);
	};

	//          ID              Name                   Desc                                DryMass MaxPay Solar PwrGen  Bat    Ant  TxPwr DataRate Storage Thermal Prop  ADCS  Cost Slots Life
	Add("SmallSat",  TEXT("SmallSat 100"),     TEXT("Compact bus for LEO missions"),        80,    120,   2.0,  400,   800,   15,  10,    20,     64,     200,   15,  0.5,  15,  2,   3);
	Add("MediumBus", TEXT("Medium Explorer"),   TEXT("Versatile mid-range platform"),       250,    400,   6.0, 1200,  2500,   22,  50,   100,    256,    500,   50,  0.1,  50,  4,   5);
	Add("LargeBus",  TEXT("Large Observatory"), TEXT("High-capability science platform"),   500,    800,  12.0, 2800,  5000,   28, 100,   300,    512,   1000,  100,  0.02, 100, 6,   10);
	Add("HeavyBus",  TEXT("Deep Space Bus"),    TEXT("For GEO/interplanetary missions"),    800,   1500,  20.0, 5000, 10000,   35, 200,   500,   1024,   2000,  200,  0.01, 180, 8,   15);
}

// ─────────────────────────────────────────────────────────────────────────────
// INSTRUMENT CATALOG
// ─────────────────────────────────────────────────────────────────────────────
void AAstralisAssemblyGameMode::InitInstrumentCatalog()
{
	auto Add = [&](FName ID, const TCHAR* Name, const TCHAR* Desc, EInstrumentType Type,
		double Mass, double Power, double DataRate, double Cost, double SciVal,
		double MinAlt, double MaxAlt, bool bSunPoint)
	{
		FInstrumentDefinition I;
		I.ID = ID; I.Name = Name; I.Description = Desc; I.Type = Type;
		I.Mass_kg = Mass; I.Power_W = Power; I.DataRate_Mbps = DataRate;
		I.Cost_M = Cost; I.ScienceValue = SciVal;
		I.MinAltitude_km = MinAlt; I.MaxAltitude_km = MaxAlt;
		I.bRequiresSunPointing = bSunPoint;
		InstrumentCatalog.Add(I);
		InstrumentSelected.Add(false);
	};

	//              ID              Name                         Desc                                                Type                       Mass  Power DataRate Cost  Sci   MinAlt MaxAlt  Sun
	Add("OLI",      TEXT("OLI Imager"),           TEXT("Operational Land Imager — 30m multispectral"),                EInstrumentType::Camera,        57,  100,    384,   35,  1.5,  400,   900,  false);
	Add("TIRS",     TEXT("Thermal IR Sensor"),    TEXT("Thermal infrared for land surface temperature"),              EInstrumentType::Radiometer,    89,  120,    150,   40,  1.2,  400,   900,  false);
	Add("SAR",      TEXT("SAR Radar"),            TEXT("Synthetic aperture radar — all-weather imaging"),             EInstrumentType::Radar,        250,  500,    800,   80,  2.0,  500,  1200,  false);
	Add("MODIS",    TEXT("MODIS Spectrometer"),   TEXT("Moderate resolution imaging spectroradiometer"),              EInstrumentType::Spectrometer, 228,  163,    500,   55,  1.8,  680,   720,  false);
	Add("MAG",      TEXT("Magnetometer"),         TEXT("Fluxgate magnetometer for geomagnetic field"),               EInstrumentType::Magnetometer,   8,   12,      1,   10,  0.8,  300, 50000,  false);
	Add("EPD",      TEXT("Energetic Particle Det"), TEXT("Measures charged particle flux in radiation belts"),        EInstrumentType::ParticleDetector, 15,  25,      5,   18,  1.0,  200, 40000,  false);
	Add("LIDAR",    TEXT("CALIOP LIDAR"),         TEXT("Cloud-aerosol lidar for atmospheric profiling"),             EInstrumentType::Lidar,        160,  220,    120,   65,  1.6,  600,   750,  true);
	Add("GRACE",    TEXT("Gravity Gradiometer"),  TEXT("Precision gravity field mapping"),                           EInstrumentType::GravityGrad,   40,   55,     10,   30,  1.3,  300,   600,  false);
	Add("RELAY",    TEXT("Comm Relay Package"),   TEXT("Data relay transponder for ground network"),                 EInstrumentType::CommRelay,     35,  150,   1000,   25,  0.3, 35000, 36000,  false);
	Add("TECH",     TEXT("Tech Demonstration"),   TEXT("In-orbit tech validation experiment"),                       EInstrumentType::TechDemo,      20,   40,      2,   12,  0.5,  200, 50000,  false);
}

void AAstralisAssemblyGameMode::CycleBus(int32 Direction)
{
	if (BusCatalog.Num() == 0) return;
	SelectedBusIndex = (SelectedBusIndex + Direction + BusCatalog.Num()) % BusCatalog.Num();
	RecalculateBudget();
}

void AAstralisAssemblyGameMode::ToggleInstrument(int32 Index)
{
	if (Index < 0 || Index >= InstrumentCatalog.Num()) return;

	if (!InstrumentSelected[Index])
	{
		// Check slot limit
		if (BusCatalog.Num() == 0) return;
		if (GetSelectedInstrumentCount() >= GetSelectedBus().MaxInstrumentSlots) return;
		InstrumentSelected[Index] = true;
	}
	else
	{
		InstrumentSelected[Index] = false;
	}
	RecalculateBudget();
}

int32 AAstralisAssemblyGameMode::GetSelectedInstrumentCount() const
{
	int32 Count = 0;
	for (bool b : InstrumentSelected) { if (b) Count++; }
	return Count;
}

double AAstralisAssemblyGameMode::GetInstrumentMass_kg() const
{
	double Total = 0.0;
	for (int32 i = 0; i < InstrumentCatalog.Num(); ++i)
	{
		if (InstrumentSelected[i]) Total += InstrumentCatalog[i].Mass_kg;
	}
	return Total;
}

double AAstralisAssemblyGameMode::GetInstrumentPower_W() const
{
	double Total = 0.0;
	for (int32 i = 0; i < InstrumentCatalog.Num(); ++i)
	{
		if (InstrumentSelected[i]) Total += InstrumentCatalog[i].Power_W;
	}
	return Total;
}

double AAstralisAssemblyGameMode::GetInstrumentDataRate_Mbps() const
{
	double Total = 0.0;
	for (int32 i = 0; i < InstrumentCatalog.Num(); ++i)
	{
		if (InstrumentSelected[i]) Total += InstrumentCatalog[i].DataRate_Mbps;
	}
	return Total;
}

double AAstralisAssemblyGameMode::GetInstrumentCost_M() const
{
	double Total = 0.0;
	for (int32 i = 0; i < InstrumentCatalog.Num(); ++i)
	{
		if (InstrumentSelected[i]) Total += InstrumentCatalog[i].Cost_M;
	}
	return Total;
}

double AAstralisAssemblyGameMode::GetScienceMultiplier() const
{
	double Total = 1.0;
	for (int32 i = 0; i < InstrumentCatalog.Num(); ++i)
	{
		if (InstrumentSelected[i]) Total += InstrumentCatalog[i].ScienceValue;
	}
	return Total;
}

TArray<FInstrumentDefinition> AAstralisAssemblyGameMode::GetActiveInstruments() const
{
	TArray<FInstrumentDefinition> Active;
	for (int32 i = 0; i < InstrumentCatalog.Num(); ++i)
	{
		if (InstrumentSelected[i]) Active.Add(InstrumentCatalog[i]);
	}
	return Active;
}

double AAstralisAssemblyGameMode::GetLaunchVehicleCost_M() const
{
	if (VehicleLibrary.Num() == 0) return 0.0;
	const FLaunchVehicleDefinition& V = VehicleLibrary[SelectedVehicleIndex];
	// Approximate cost based on payload capacity
	if (V.PayloadToLEO_kg > 50000.0) return 350.0;  // Super heavy
	if (V.PayloadToLEO_kg > 20000.0) return 150.0;  // Heavy
	if (V.PayloadToLEO_kg > 10000.0) return 67.0;   // Medium (Falcon 9 class)
	if (V.PayloadToLEO_kg > 5000.0)  return 50.0;   // Small-medium
	return 30.0;                                       // Small
}

void AAstralisAssemblyGameMode::RecalculateBudget()
{
	Budget.LaunchCost_M = GetLaunchVehicleCost_M();
	Budget.BusCost_M = (BusCatalog.Num() > 0) ? BusCatalog[SelectedBusIndex].Cost_M : 0.0;
	Budget.InstrumentCost_M = GetInstrumentCost_M();
	double Hardware = Budget.LaunchCost_M + Budget.BusCost_M + Budget.InstrumentCost_M;
	Budget.InsuranceCost_M = Hardware * 0.10;
}

// ─────────────────────────────────────────────────────────────────────────────
// SPACECRAFT RESOURCE MANAGEMENT (live during orbit)
// ─────────────────────────────────────────────────────────────────────────────
void AAstralisAssemblyGameMode::InitSpacecraftResources()
{
	if (BusCatalog.Num() == 0) return;
	const FSpacecraftBusDefinition& Bus = BusCatalog[SelectedBusIndex];

	SCResources.BatteryMax_Wh = Bus.BatteryCapacity_Wh;
	SCResources.BatteryCharge_Wh = Bus.BatteryCapacity_Wh;
	SCResources.PowerGeneration_W = Bus.PowerGeneration_W;
	SCResources.PowerDraw_W = 80.0 + GetInstrumentPower_W(); // 80W bus housekeeping
	SCResources.DataStorageMax_GB = Bus.DataStorage_GB;
	SCResources.DataStored_GB = 0.0;
	SCResources.DataDownlinked_GB = 0.0;
	SCResources.PropellantRemaining_kg = Bus.PropellantMass_kg;
	SCResources.ThermalCapacity_W = Bus.ThermalCapacity_W;
	SCResources.ThermalLoad_W = 0.0;
	SCResources.bInSunlight = true;
	SCResources.bInGroundContact = true;
	SCResources.bThermalWarning = false;
	SCResources.bPowerWarning = false;
	SCResources.MissionTime_s = 0.0;
	SCResources.ScienceCollected = 0.0;
}

void AAstralisAssemblyGameMode::UpdateSpacecraftResources(float DeltaTime)
{
	if (!OrbitScene) return;
	const double TWarp = OrbitScene->GetTimeWarpScale();
	const double SimDT = DeltaTime * TWarp;

	SCResources.MissionTime_s += SimDT;

	// Orbital period determines eclipse fraction
	const FSpacecraftState& SC = OrbitScene->GetSpacecraftState();
	const double R_m = SC.Orbital.Position_m.Size();
	const double Alt_km = (R_m - 6.371e6) / 1000.0;

	// Simple eclipse model: ~35% of orbit in shadow for LEO
	double OrbitalPhase = FMath::Fmod(SCResources.MissionTime_s / 5400.0, 1.0); // ~90min orbit
	SCResources.bInSunlight = (OrbitalPhase < 0.65);

	// Ground station contact: visible ~15% of time in LEO
	double ContactPhase = FMath::Fmod(SCResources.MissionTime_s / 5400.0 * 3.7, 1.0);
	SCResources.bInGroundContact = (ContactPhase < 0.15);

	// Power balance
	double Gen = SCResources.bInSunlight ? SCResources.PowerGeneration_W : 0.0;
	double NetPower_W = Gen - SCResources.PowerDraw_W;

	// Battery: charge in sun, drain in eclipse
	SCResources.BatteryCharge_Wh += (NetPower_W / 3600.0) * SimDT;
	SCResources.BatteryCharge_Wh = FMath::Clamp(SCResources.BatteryCharge_Wh,
		0.0, SCResources.BatteryMax_Wh);
	SCResources.bPowerWarning = (SCResources.BatteryCharge_Wh < SCResources.BatteryMax_Wh * 0.2);

	// Data collection (instruments generate data when active and in sunlight for imaging)
	double DataGen_Mbps = 0.0;
	for (int32 i = 0; i < InstrumentCatalog.Num(); ++i)
	{
		if (!InstrumentSelected[i]) continue;
		const FInstrumentDefinition& Inst = InstrumentCatalog[i];
		bool bCanOperate = (Alt_km >= Inst.MinAltitude_km && Alt_km <= Inst.MaxAltitude_km);
		if (Inst.bRequiresSunPointing && !SCResources.bInSunlight) bCanOperate = false;
		if (bCanOperate) DataGen_Mbps += Inst.DataRate_Mbps;
	}
	SCResources.DataStored_GB += (DataGen_Mbps / 8.0 / 1024.0) * SimDT;

	// Downlink when in ground contact
	if (SCResources.bInGroundContact && SCResources.DataStored_GB > 0.0 && BusCatalog.Num() > 0)
	{
		const FSpacecraftBusDefinition& Bus = BusCatalog[SelectedBusIndex];
		double DownlinkRate_GBps = Bus.MaxDataRate_Mbps / 8.0 / 1024.0;
		double Downloaded = DownlinkRate_GBps * SimDT;
		Downloaded = FMath::Min(Downloaded, SCResources.DataStored_GB);
		SCResources.DataStored_GB -= Downloaded;
		SCResources.DataDownlinked_GB += Downloaded;
	}

	// Storage overflow = data loss
	if (SCResources.DataStored_GB > SCResources.DataStorageMax_GB)
	{
		SCResources.DataStored_GB = SCResources.DataStorageMax_GB;
	}

	// Thermal model — simplified
	SCResources.ThermalLoad_W = SCResources.PowerDraw_W * 0.6;
	if (!SCResources.bInSunlight) SCResources.ThermalLoad_W *= 0.4;
	SCResources.bThermalWarning = (SCResources.ThermalLoad_W > SCResources.ThermalCapacity_W * 0.85);

	// Science score accumulates
	SCResources.ScienceCollected += GetScienceMultiplier() * SimDT * 0.001;
}

FString AAstralisAssemblyGameMode::FComprehensiveScore::GetGrade() const
{
	double T = GetTotal();
	if (T >= 140.0) return TEXT("S+");
	if (T >= 125.0) return TEXT("S");
	if (T >= 110.0) return TEXT("A+");
	if (T >= 95.0) return TEXT("A");
	if (T >= 80.0) return TEXT("B");
	if (T >= 60.0) return TEXT("C");
	if (T >= 40.0) return TEXT("D");
	return TEXT("F");
}

AAstralisAssemblyGameMode::FComprehensiveScore AAstralisAssemblyGameMode::ComputeComprehensiveScore() const
{
	FComprehensiveScore CS;

	// Launch performance (0-100)
	if (LaunchVehicle)
		CS.LaunchScore = LaunchVehicle->MissionScore;

	// Budget efficiency bonus (0-15) — reward coming in under budget
	if (!Budget.IsOverBudget())
	{
		double Remaining = Budget.GetRemaining();
		double Pct = FMath::Clamp(Remaining / Budget.TotalBudget_M, 0.0, 0.5);
		CS.BudgetBonus = 15.0 * (Pct / 0.5);
	}

	// Science bonus (0-15) — based on data collected and downlinked
	{
		double SciScore = FMath::Clamp(SCResources.ScienceCollected / 10.0, 0.0, 1.0);
		double DataScore = FMath::Clamp(SCResources.DataDownlinked_GB / 1.0, 0.0, 1.0);
		CS.ScienceBonus = 15.0 * (SciScore * 0.6 + DataScore * 0.4);
	}

	// Design bonus (0-10) — instruments compatible with target orbit
	if (BusCatalog.Num() > 0 && Missions.Num() > 0)
	{
		double TargetAlt = GetSelectedMission().TargetAltitude_km;
		int32 CompatCount = 0;
		int32 TotalSel = 0;
		for (int32 i = 0; i < InstrumentCatalog.Num(); ++i)
		{
			if (!InstrumentSelected[i]) continue;
			TotalSel++;
			const FInstrumentDefinition& Inst = InstrumentCatalog[i];
			if (TargetAlt >= Inst.MinAltitude_km && TargetAlt <= Inst.MaxAltitude_km)
				CompatCount++;
		}
		if (TotalSel > 0)
			CS.DesignBonus = 10.0 * (static_cast<double>(CompatCount) / TotalSel);
		else
			CS.DesignBonus = 0.0;
	}

	// Efficiency bonus (0-10) — power/mass margins
	if (BusCatalog.Num() > 0)
	{
		const FSpacecraftBusDefinition& Bus = GetSelectedBus();
		double MassPct = FMath::Clamp(1.0 - GetInstrumentMass_kg() / FMath::Max(1.0, Bus.MaxPayloadMass_kg), 0.0, 1.0);
		double PwrPct = FMath::Clamp(1.0 - (80.0 + GetInstrumentPower_W()) / FMath::Max(1.0, Bus.PowerGeneration_W), 0.0, 1.0);
		CS.EfficiencyBonus = 10.0 * (MassPct * 0.5 + PwrPct * 0.5);
	}

	return CS;
}
