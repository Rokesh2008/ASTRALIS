#include "AstralisStageActor.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "Materials/MaterialInterface.h"

AAstralisStageActor::AAstralisStageActor()
{
	PrimaryActorTick.bCanEverTick = true;

	StageRoot = CreateDefaultSubobject<USceneComponent>(TEXT("StageRoot"));
	SetRootComponent(StageRoot);

	StageMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StageMesh"));
	StageMesh->SetupAttachment(StageRoot);
	StageMesh->SetCollisionProfileName(TEXT("NoCollision"));
}

void AAstralisStageActor::BeginPlay()
{
	Super::BeginPlay();

	CurrentPropellant_kg = Spec.PropellantCapacity_kg;

	UE_LOG(LogTemp, Log, TEXT("Stage %d initialized: DryMass=%.0f kg, Propellant=%.0f kg, Thrust=%.0f kN"),
		StageNumber, Spec.DryMass_kg, Spec.PropellantCapacity_kg, Spec.Thrust_N / 1000.0);
}

void AAstralisStageActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bEnginesActive && !IsPropellantDepleted() && !bVehicleControlled)
	{
		BurnTimer_s += DeltaTime;
		ConsumePropellant(DeltaTime);

		if (IsPropellantDepleted())
		{
			ShutdownEngines();
			UE_LOG(LogTemp, Warning, TEXT("Stage %d: Propellant depleted, engines shutdown"), StageNumber);
		}
	}

	if (bSeparated)
	{
		SeparationTimer += DeltaTime;
		// Fall away and fade
		FVector SepVelocity = FVector(0, 0, -2000.0) * DeltaTime;
		AddActorWorldOffset(SepVelocity, false);

		// Destroy after 3 seconds
		if (SeparationTimer > 3.0)
		{
			SetActorHiddenInGame(true);
		}
	}
}

void AAstralisStageActor::IgniteEngines()
{
	if (!bSeparated && !IsPropellantDepleted())
	{
		bEnginesActive = true;
		BurnTimer_s = 0.0;
		UE_LOG(LogTemp, Warning, TEXT("Stage %d: Engines ignited (Thrust=%.0f kN, Isp=%.0f s)"),
			StageNumber, Spec.Thrust_N / 1000.0, Spec.Isp_s);
	}
}

void AAstralisStageActor::ShutdownEngines()
{
	bEnginesActive = false;
	UE_LOG(LogTemp, Warning, TEXT("Stage %d: Engines shutdown (Burned %.1f s)"), StageNumber, BurnTimer_s);
}

void AAstralisStageActor::Separate()
{
	if (!bSeparated)
	{
		bSeparated = true;
		SeparationTimer = 0.0;
		ShutdownEngines();

		// Detach from parent so it falls independently
		DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);

		UE_LOG(LogTemp, Warning, TEXT("Stage %d: SEPARATED"), StageNumber);
	}
}

double AAstralisStageActor::GetTotalMass_kg() const
{
	return Spec.DryMass_kg + CurrentPropellant_kg;
}

double AAstralisStageActor::GetCurrentThrust_N() const
{
	return bEnginesActive ? Spec.Thrust_N : 0.0;
}

double AAstralisStageActor::GetMassFlowRate_kgps() const
{
	if (!bEnginesActive) return 0.0;
	const double g0 = 9.80665;
	return Spec.Thrust_N / (Spec.Isp_s * g0);
}

bool AAstralisStageActor::IsPropellantDepleted() const
{
	return CurrentPropellant_kg <= 0.01;
}

void AAstralisStageActor::ConsumePropellant(double DeltaTime_s)
{
	if (bEnginesActive && CurrentPropellant_kg > 0.0)
	{
		const double MassFlowRate = GetMassFlowRate_kgps();
		const double Consumed = MassFlowRate * DeltaTime_s;
		CurrentPropellant_kg = FMath::Max(0.0, CurrentPropellant_kg - Consumed);
	}
}
