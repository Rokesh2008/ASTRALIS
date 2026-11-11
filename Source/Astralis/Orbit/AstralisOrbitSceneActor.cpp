#include "AstralisOrbitSceneActor.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "Materials/MaterialInterface.h"
#include "DrawDebugHelpers.h"

AAstralisOrbitSceneActor::AAstralisOrbitSceneActor()
{
	PrimaryActorTick.bCanEverTick = true;

	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	SetRootComponent(SceneRoot);

	EarthMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("EarthMesh"));
	EarthMesh->SetupAttachment(SceneRoot);
	EarthMesh->SetCollisionProfileName(TEXT("NoCollision"));

	SpacecraftMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SpacecraftMesh"));
	SpacecraftMesh->SetupAttachment(SceneRoot);
	SpacecraftMesh->SetCollisionProfileName(TEXT("NoCollision"));
}

void AAstralisOrbitSceneActor::BeginPlay()
{
	Super::BeginPlay();

	// Earth sphere with NASA texture support
	UStaticMesh* Sphere = LoadObject<UStaticMesh>(nullptr,
		TEXT("/Engine/BasicShapes/Sphere.Sphere"));

	if (Sphere && EarthMesh)
	{
		EarthMesh->SetStaticMesh(Sphere);

		// Earth radius in UE units: 6.371e6 m * 1e-5 = 63.71 UE cm
		// Engine sphere radius = 50 UU, so scale = 63.71 / 50 = 1.2742
		const double EarthUE = EarthRadius_m * OrbitScale;
		const double SphereScale = EarthUE / 50.0;
		EarthMesh->SetWorldScale3D(FVector(SphereScale));

		// Try NASA enhanced material first, then fallback to basic
		UMaterialInterface* EarthMat = EarthMaterialAsset;
		if (!EarthMat)
		{
			// Try to load NASA enhanced Earth material
			EarthMat = LoadObject<UMaterialInterface>(nullptr,
				TEXT("/Game/NASA/Materials/M_Earth_Enhanced.M_Earth_Enhanced"));
		}
		if (!EarthMat)
		{
			// Fallback to basic blue material
			EarthMat = LoadObject<UMaterialInterface>(nullptr,
				TEXT("/Game/Materials/M_Antenna.M_Antenna"));
		}
		if (EarthMat)
		{
			EarthMesh->SetMaterial(0, EarthMat);
		}
	}

	// Spacecraft visual — NASA Landsat 8 or fallback cube
	UStaticMesh* SCMesh = SpacecraftMeshAsset;
	if (!SCMesh)
	{
		// Try to load NASA Landsat 8 from Content
		SCMesh = LoadObject<UStaticMesh>(nullptr,
			TEXT("/Game/NASA/Spacecraft/Landsat8.Landsat8"));
	}
	if (!SCMesh)
	{
		// Final fallback to primitive cube
		SCMesh = LoadObject<UStaticMesh>(nullptr,
			TEXT("/Engine/BasicShapes/Cube.Cube"));
	}

	if (SCMesh && SpacecraftMesh)
	{
		SpacecraftMesh->SetStaticMesh(SCMesh);

		// If using NASA model (much larger than cube), scale appropriately
		// Landsat 8 is ~3m, at orbit scale of 1e-5 that's 0.03 cm - too small
		// Scale up for visibility while maintaining relative orbital scale
		if (SpacecraftMeshAsset || SCMesh->GetPathName().Contains(TEXT("Landsat")))
		{
			SpacecraftMesh->SetWorldScale3D(FVector(2.0, 2.0, 2.0));
		}
		else
		{
			// Primitive cube fallback
			SpacecraftMesh->SetWorldScale3D(FVector(0.5));

			UMaterialInterface* SCMat = LoadObject<UMaterialInterface>(nullptr,
				TEXT("/Game/Materials/M_SolarPanel.M_SolarPanel"));
			if (SCMat)
			{
				SpacecraftMesh->SetMaterial(0, SCMat);
			}
		}
		// Note: NASA models come with their own materials embedded
	}

	TrailPoints_UE.SetNum(TrailMaxPoints);
	for (auto& P : TrailPoints_UE) P = FVector::ZeroVector;
}

void AAstralisOrbitSceneActor::InitializeFromAssembly(
	const TArray<FComponentMassEntry>& Components,
	double PropellantMass_kg)
{
	// Transfer component data to sim engine
	SimEngine.GetComponents() = Components;
	SimEngine.SetPropellantMass(PropellantMass_kg);

	// Default LEO orbit: 400 km altitude, circular, equatorial
	const double Altitude_m = 400.0e3;
	const double R = EarthRadius_m + Altitude_m;
	const double V = FAstralisTwoBodyPropagator::CircularVelocity(R, FAstralisTwoBodyPropagator::Mu_Earth);

	FSpacecraftOrbitalState InitOrbit;
	InitOrbit.Position_m = FVector3d(R, 0.0, 0.0);
	InitOrbit.Velocity_mps = FVector3d(0.0, V, 0.0);
	SimEngine.SetOrbitalState(InitOrbit);

	// Default solar array (representative small sat)
	FSolarArraySpec SA;
	SA.Area_m2 = 4.0;
	SA.Efficiency = 0.30;
	SA.DegradationPerYear = 0.02;
	SimEngine.SetSolarArray(SA);

	// Default battery
	FBatterySpec Bat;
	Bat.Capacity_Wh = 200.0;
	Bat.MaxDoD = 0.8;
	Bat.Efficiency = 0.95;
	SimEngine.SetBattery(Bat);

	// Default antenna
	FAntennaSpec Ant;
	Ant.Frequency_Hz = 2.2e9;
	Ant.TransmitPower_W = 5.0;
	Ant.Gain_dBi = 6.0;
	SimEngine.SetAntenna(Ant);

	// Default ground station
	FGroundStationSpec GS;
	GS.ReceiveGain_dBi = 34.0;
	GS.SystemNoiseTemp_K = 135.0;
	SimEngine.SetGroundStation(GS);

	// Default engine
	FEngineSpec Eng;
	Eng.MaxThrust_N = 22.0;
	Eng.Isp_s = 220.0;
	SimEngine.SetEngine(Eng);

	// Set initial battery energy to 80% full
	SimEngine.GetClock().SetTimeScale(1.0);

	bInitialized = true;

	// Fill trail with initial position
	const FVector InitialUE = SimToUE(InitOrbit.Position_m);
	for (auto& P : TrailPoints_UE) P = InitialUE;
}

void AAstralisOrbitSceneActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!bInitialized) return;

	// Advance simulation
	SimEngine.GetClock().Advance(static_cast<double>(DeltaTime));
	const double SimDt = SimEngine.GetClock().GetSimDelta();

	if (SimDt > 0.0)
	{
		// Sub-step large time warps to maintain accuracy
		const double MaxStep = 10.0;
		double Remaining = SimDt;
		while (Remaining > 0.0)
		{
			const double Step = FMath::Min(Remaining, MaxStep);
			SimEngine.Step(Step);
			Remaining -= Step;
		}
	}

	// Update spacecraft visual position
	const FSpacecraftState& State = SimEngine.GetState();
	const FVector SCUE = SimToUE(State.Orbital.Position_m);
	if (SpacecraftMesh)
	{
		SpacecraftMesh->SetWorldLocation(GetActorLocation() + SCUE);
	}

	// Orbit trail
	TrailSampleTimer += DeltaTime;
	const double SampleInterval = FMath::Max(0.016, 0.5 / FMath::Max(1.0, GetTimeWarpScale()));
	if (TrailSampleTimer >= SampleInterval)
	{
		TrailSampleTimer = 0.0;
		TrailPoints_UE[TrailWriteIndex] = GetActorLocation() + SCUE;
		TrailWriteIndex = (TrailWriteIndex + 1) % TrailMaxPoints;
	}

	DrawTrail();

	// Slowly rotate Earth (sidereal day = 86164s)
	if (EarthMesh)
	{
		const double RotRate = 360.0 / 86164.0;
		const double RotDeg = RotRate * SimDt;
		EarthMesh->AddLocalRotation(FRotator(0.0, RotDeg, 0.0));
	}
}

FVector AAstralisOrbitSceneActor::SimToUE(const FVector3d& Position_m) const
{
	return FVector(
		Position_m.X * OrbitScale,
		Position_m.Y * OrbitScale,
		Position_m.Z * OrbitScale);
}

void AAstralisOrbitSceneActor::UpdateOrbitTrail()
{
	// Trail is updated in Tick
}

void AAstralisOrbitSceneActor::DrawTrail() const
{
	UWorld* World = GetWorld();
	if (!World) return;

	const FColor TrailColor(100, 200, 255);

	for (int32 i = 0; i < TrailMaxPoints - 1; ++i)
	{
		const int32 Idx0 = (TrailWriteIndex + i) % TrailMaxPoints;
		const int32 Idx1 = (TrailWriteIndex + i + 1) % TrailMaxPoints;

		if (TrailPoints_UE[Idx0].IsNearlyZero() || TrailPoints_UE[Idx1].IsNearlyZero())
			continue;

		DrawDebugLine(World, TrailPoints_UE[Idx0], TrailPoints_UE[Idx1],
			TrailColor, false, -1.0f, 0, 0.3f);
	}
}

// Time warp steps: 0.1x, 0.25x, 0.5x, 1x, 2x, 5x, 10x, 50x, 100x, 1000x, 10000x
const TArray<double>& AAstralisOrbitSceneActor::GetTimeWarpSteps()
{
	static const TArray<double> Steps = {
		0.1, 0.25, 0.5, 1.0, 2.0, 5.0, 10.0, 50.0, 100.0, 1000.0, 10000.0
	};
	return Steps;
}

void AAstralisOrbitSceneActor::IncreaseTimeWarp()
{
	const auto& Steps = GetTimeWarpSteps();
	TimeWarpIndex = FMath::Min(TimeWarpIndex + 1, Steps.Num() - 1);
	SimEngine.GetClock().SetTimeScale(Steps[TimeWarpIndex]);
}

void AAstralisOrbitSceneActor::DecreaseTimeWarp()
{
	const auto& Steps = GetTimeWarpSteps();
	TimeWarpIndex = FMath::Max(TimeWarpIndex - 1, 0);
	SimEngine.GetClock().SetTimeScale(Steps[TimeWarpIndex]);
}

void AAstralisOrbitSceneActor::TogglePause()
{
	SimEngine.GetClock().SetPaused(!SimEngine.GetClock().IsPaused());
}

double AAstralisOrbitSceneActor::GetTimeWarpScale() const
{
	return GetTimeWarpSteps()[TimeWarpIndex];
}

bool AAstralisOrbitSceneActor::IsPaused() const
{
	return SimEngine.GetClock().IsPaused();
}

FString AAstralisOrbitSceneActor::GetTimeWarpLabel() const
{
	if (IsPaused()) return TEXT("PAUSED");
	const double Scale = GetTimeWarpScale();
	if (Scale >= 1.0)
		return FString::Printf(TEXT("%.0fx"), Scale);
	return FString::Printf(TEXT("%.2fx"), Scale);
}
