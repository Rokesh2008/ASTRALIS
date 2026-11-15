#include "AstralisLaunchVehicleActor.h"
#include "Astralis/Spacecraft/AstralisSpacecraftActor.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "Materials/MaterialInterface.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "DrawDebugHelpers.h"

AAstralisLaunchVehicleActor::AAstralisLaunchVehicleActor()
{
	PrimaryActorTick.bCanEverTick = true;

	VehicleRoot = CreateDefaultSubobject<USceneComponent>(TEXT("VehicleRoot"));
	SetRootComponent(VehicleRoot);
}

void AAstralisLaunchVehicleActor::BeginPlay()
{
	Super::BeginPlay();

	LaunchOrigin = GetActorLocation();

	Stages.Sort([](const AAstralisStageActor& A, const AAstralisStageActor& B)
	{
		return A.StageNumber < B.StageNumber;
	});

	BuildRocketVisual();
	SwapCatalog = UAstralisEngineCatalog::GetDefaultCatalog();

	const double TotalMass = GetTotalMass_kg();
	const double DeltaV = GetTotalDeltaV_mps();

	UE_LOG(LogTemp, Warning, TEXT("Launch Vehicle: Mass=%.0f kg, DV=%.0f m/s"), TotalMass, DeltaV);
}

void AAstralisLaunchVehicleActor::BuildRocketVisual()
{
	UStaticMesh* Cylinder = LoadObject<UStaticMesh>(nullptr,
		TEXT("/Engine/BasicShapes/Cylinder.Cylinder"));
	UStaticMesh* Cone = LoadObject<UStaticMesh>(nullptr,
		TEXT("/Engine/BasicShapes/Cone.Cone"));
	if (!Cylinder) return;

	UMaterialInterface* SilverMat = LoadObject<UMaterialInterface>(nullptr,
		TEXT("/Game/NASA/Spacecraft/Landsat8/Materials/LDCM-silver-fl.LDCM-silver-fl"));
	UMaterialInterface* DarkMat = LoadObject<UMaterialInterface>(nullptr,
		TEXT("/Game/NASA/Spacecraft/Landsat8/Materials/LDCM-darkbronze-fl.LDCM-darkbronze-fl"));
	UMaterialInterface* GoldMat = LoadObject<UMaterialInterface>(nullptr,
		TEXT("/Game/NASA/Spacecraft/Landsat8/Materials/LDCM-gold-fl.LDCM-gold-fl"));

	// Falcon 9 paint scheme — always use MIDs for consistent appearance
	UMaterial* BaseMat = LoadObject<UMaterial>(nullptr,
		TEXT("/Engine/BasicShapes/BasicShapeMaterial.BasicShapeMaterial"));

	// White body (Falcon 9 first/second stage, fairing)
	if (!SilverMat && BaseMat)
	{
		UMaterialInstanceDynamic* MID = UMaterialInstanceDynamic::Create(BaseMat, this);
		MID->SetVectorParameterValue(TEXT("Color"), FLinearColor(0.92f, 0.93f, 0.95f));
		SilverMat = MID;
	}
	// Dark carbon fiber (interstage, engine skirt, grid fins, legs)
	if (!DarkMat && BaseMat)
	{
		UMaterialInstanceDynamic* MID = UMaterialInstanceDynamic::Create(BaseMat, this);
		MID->SetVectorParameterValue(TEXT("Color"), FLinearColor(0.08f, 0.08f, 0.1f));
		DarkMat = MID;
	}
	// Gold thermal blanket (shock diamonds)
	if (!GoldMat && BaseMat)
	{
		UMaterialInstanceDynamic* MID = UMaterialInstanceDynamic::Create(BaseMat, this);
		MID->SetVectorParameterValue(TEXT("Color"), FLinearColor(0.85f, 0.7f, 0.2f));
		GoldMat = MID;
	}

	// Scale everything from vehicle dimensions (1m = 100 UE units)
	const double DiamScale = VehicleDiameter_m / 3.7;
	const double S1Scale = VehicleS1Length_m / 42.6;
	const double S2Scale = VehicleS2Length_m / 13.1;
	const double BodyRadius = 185.0 * DiamScale;
	const double BodyScaleXY = BodyRadius / 50.0;

	ExhaustDiamScale = DiamScale;

	// Computed heights
	const double S1H = 3000.0 * S1Scale;
	const double S1Mid = S1H / 2.0;
	const double S1Top = S1H;
	const double InterH = 200.0 * DiamScale;
	const double S2H = 800.0 * S2Scale;
	const double S2Bot = S1Top + InterH;
	const double S2Mid = S2Bot + S2H / 2.0;
	const double S2Top = S2Bot + S2H;
	const double FairingBot = S2Top + 50.0;
	const double FairingH = 700.0 * DiamScale;
	const double FairingMid = FairingBot + FairingH / 2.0;
	const double NoseZ = FairingBot + FairingH;

	// Stage 1 body
	Stage1Mesh = NewObject<UStaticMeshComponent>(this, TEXT("Stage1Body"));
	Stage1Mesh->SetupAttachment(VehicleRoot);
	Stage1Mesh->RegisterComponent();
	Stage1Mesh->SetStaticMesh(Cylinder);
	Stage1Mesh->SetRelativeLocation(FVector(0, 0, S1Mid));
	Stage1Mesh->SetRelativeScale3D(FVector(BodyScaleXY, BodyScaleXY, S1H / 100.0));
	Stage1Mesh->SetCollisionProfileName(TEXT("NoCollision"));
	if (SilverMat) Stage1Mesh->SetMaterial(0, SilverMat);

	// Engine skirt
	{
		auto* Skirt = NewObject<UStaticMeshComponent>(this, TEXT("S1_EngineSkirt"));
		Skirt->SetupAttachment(Stage1Mesh);
		Skirt->RegisterComponent();
		Skirt->SetStaticMesh(Cylinder);
		Skirt->SetRelativeLocation(FVector(0, 0, -49.0));
		Skirt->SetRelativeScale3D(FVector(1.15, 1.15, 0.07));
		Skirt->SetCollisionProfileName(TEXT("NoCollision"));
		if (DarkMat) Skirt->SetMaterial(0, DarkMat);
	}

	// 9 engine nozzles
	if (Cone)
	{
		const double NozzleScaleXY = 0.12;
		const double NozzleScaleZ = 0.06;
		const double NozzleZ = -51.0;

		auto* CenterNozzle = NewObject<UStaticMeshComponent>(this, TEXT("S1_Nozzle_C"));
		CenterNozzle->SetupAttachment(Stage1Mesh);
		CenterNozzle->RegisterComponent();
		CenterNozzle->SetStaticMesh(Cone);
		CenterNozzle->SetRelativeLocation(FVector(0, 0, NozzleZ));
		CenterNozzle->SetRelativeScale3D(FVector(NozzleScaleXY, NozzleScaleXY, NozzleScaleZ));
		CenterNozzle->SetRelativeRotation(FRotator(180.0, 0, 0));
		CenterNozzle->SetCollisionProfileName(TEXT("NoCollision"));
		if (DarkMat) CenterNozzle->SetMaterial(0, DarkMat);

		const double RingR = 18.0;
		for (int32 i = 0; i < 8; ++i)
		{
			const double Angle = (2.0 * PI * i) / 8.0;
			const double LX = FMath::Cos(Angle) * RingR;
			const double LY = FMath::Sin(Angle) * RingR;

			FName Name(*FString::Printf(TEXT("S1_Nozzle_%d"), i));
			auto* Nozzle = NewObject<UStaticMeshComponent>(this, Name);
			Nozzle->SetupAttachment(Stage1Mesh);
			Nozzle->RegisterComponent();
			Nozzle->SetStaticMesh(Cone);
			Nozzle->SetRelativeLocation(FVector(LX, LY, NozzleZ));
			Nozzle->SetRelativeScale3D(FVector(NozzleScaleXY, NozzleScaleXY, NozzleScaleZ));
			Nozzle->SetRelativeRotation(FRotator(180.0, 0, 0));
			Nozzle->SetCollisionProfileName(TEXT("NoCollision"));
			if (DarkMat) Nozzle->SetMaterial(0, DarkMat);
		}
	}

	// Grid fins
	for (int32 i = 0; i < 4; ++i)
	{
		const double Angle = (PI * 0.25) + (PI * 0.5 * i);
		const double FinR = BodyRadius + 30.0;
		const double FX = FMath::Cos(Angle) * FinR;
		const double FY = FMath::Sin(Angle) * FinR;

		FName Name(*FString::Printf(TEXT("S1_GridFin_%d"), i));
		auto* Fin = NewObject<UStaticMeshComponent>(this, Name);
		Fin->SetupAttachment(Stage1Mesh);
		Fin->RegisterComponent();
		Fin->SetStaticMesh(Cylinder);
		Fin->SetRelativeLocation(FVector(FX / BodyScaleXY, FY / BodyScaleXY, 46.0 * S1Scale));
		Fin->SetRelativeScale3D(FVector(0.14, 0.02, 0.03));
		Fin->SetRelativeRotation(FRotator(0, FMath::RadiansToDegrees(Angle), 0));
		Fin->SetCollisionProfileName(TEXT("NoCollision"));
		if (DarkMat) Fin->SetMaterial(0, DarkMat);
	}

	// Landing legs
	for (int32 i = 0; i < 4; ++i)
	{
		const double Angle = (PI * 0.5 * i);
		const double LegR = BodyRadius + 20.0;
		const double LX = FMath::Cos(Angle) * LegR;
		const double LY = FMath::Sin(Angle) * LegR;

		FName Name(*FString::Printf(TEXT("S1_Leg_%d"), i));
		auto* Leg = NewObject<UStaticMeshComponent>(this, Name);
		Leg->SetupAttachment(VehicleRoot);
		Leg->RegisterComponent();
		Leg->SetStaticMesh(Cylinder);
		Leg->SetRelativeLocation(FVector(LX, LY, 200.0 * S1Scale));
		Leg->SetRelativeScale3D(FVector(0.1, 0.1, 5.0));
		Leg->SetRelativeRotation(FRotator(0, FMath::RadiansToDegrees(Angle), 20.0));
		Leg->SetCollisionProfileName(TEXT("NoCollision"));
		if (SilverMat) Leg->SetMaterial(0, SilverMat);
		Stage1LooseParts.Add(Leg);
	}

	// Interstage
	{
		auto* Interstage = NewObject<UStaticMeshComponent>(this, TEXT("Interstage"));
		Interstage->SetupAttachment(VehicleRoot);
		Interstage->RegisterComponent();
		Interstage->SetStaticMesh(Cylinder);
		Interstage->SetRelativeLocation(FVector(0, 0, S1Top + InterH / 2.0));
		Interstage->SetRelativeScale3D(FVector(BodyScaleXY * 1.02, BodyScaleXY * 1.02, InterH / 100.0));
		Interstage->SetCollisionProfileName(TEXT("NoCollision"));
		if (DarkMat) Interstage->SetMaterial(0, DarkMat);
		Stage1LooseParts.Add(Interstage);
	}

	// Cable raceway — thin dark strip running the length of S1
	{
		auto* Raceway = NewObject<UStaticMeshComponent>(this, TEXT("S1_Raceway"));
		Raceway->SetupAttachment(Stage1Mesh);
		Raceway->RegisterComponent();
		Raceway->SetStaticMesh(Cylinder);
		Raceway->SetRelativeLocation(FVector(1.05, 0, 0));
		Raceway->SetRelativeScale3D(FVector(0.015, 0.06, 1.02));
		Raceway->SetCollisionProfileName(TEXT("NoCollision"));
		if (DarkMat) Raceway->SetMaterial(0, DarkMat);
	}

	// S1 dark band near top (logo area on real Falcon 9)
	{
		auto* LogoBand = NewObject<UStaticMeshComponent>(this, TEXT("S1_LogoBand"));
		LogoBand->SetupAttachment(Stage1Mesh);
		LogoBand->RegisterComponent();
		LogoBand->SetStaticMesh(Cylinder);
		LogoBand->SetRelativeLocation(FVector(0, 0, 35.0));
		LogoBand->SetRelativeScale3D(FVector(1.005, 1.005, 0.02));
		LogoBand->SetCollisionProfileName(TEXT("NoCollision"));
		if (DarkMat) LogoBand->SetMaterial(0, DarkMat);
	}

	// S1 soot line near bottom (from reuse)
	if (BaseMat)
	{
		UMaterialInstanceDynamic* SootMat = UMaterialInstanceDynamic::Create(BaseMat, this);
		SootMat->SetVectorParameterValue(TEXT("Color"), FLinearColor(0.15f, 0.14f, 0.13f));
		auto* Soot = NewObject<UStaticMeshComponent>(this, TEXT("S1_SootRing"));
		Soot->SetupAttachment(Stage1Mesh);
		Soot->RegisterComponent();
		Soot->SetStaticMesh(Cylinder);
		Soot->SetRelativeLocation(FVector(0, 0, -35.0));
		Soot->SetRelativeScale3D(FVector(1.008, 1.008, 0.08));
		Soot->SetCollisionProfileName(TEXT("NoCollision"));
		Soot->SetMaterial(0, SootMat);
	}

	// Stage 2 body
	Stage2Mesh = NewObject<UStaticMeshComponent>(this, TEXT("Stage2Body"));
	Stage2Mesh->SetupAttachment(VehicleRoot);
	Stage2Mesh->RegisterComponent();
	Stage2Mesh->SetStaticMesh(Cylinder);
	Stage2Mesh->SetRelativeLocation(FVector(0, 0, S2Mid));
	Stage2Mesh->SetRelativeScale3D(FVector(BodyScaleXY, BodyScaleXY, S2H / 100.0));
	Stage2Mesh->SetCollisionProfileName(TEXT("NoCollision"));
	if (SilverMat) Stage2Mesh->SetMaterial(0, SilverMat);

	// Stage 2 nozzle
	if (Cone)
	{
		auto* S2Nozzle = NewObject<UStaticMeshComponent>(this, TEXT("S2_Nozzle"));
		S2Nozzle->SetupAttachment(VehicleRoot);
		S2Nozzle->RegisterComponent();
		S2Nozzle->SetStaticMesh(Cone);
		S2Nozzle->SetRelativeLocation(FVector(0, 0, S2Bot));
		S2Nozzle->SetRelativeScale3D(FVector(BodyScaleXY * 0.5, BodyScaleXY * 0.5, 3.0 * DiamScale));
		S2Nozzle->SetRelativeRotation(FRotator(180.0, 0, 0));
		S2Nozzle->SetCollisionProfileName(TEXT("NoCollision"));
		if (DarkMat) S2Nozzle->SetMaterial(0, DarkMat);
		Stage2LooseParts.Add(S2Nozzle);
	}

	// S2 RCS thruster pods (4 around the top)
	for (int32 i = 0; i < 4; ++i)
	{
		const double Angle = (PI * 0.5 * i) + PI * 0.25;
		const double RCSR = BodyRadius * 1.15;
		FName RCSName(*FString::Printf(TEXT("S2_RCS_%d"), i));
		auto* RCS = NewObject<UStaticMeshComponent>(this, RCSName);
		RCS->SetupAttachment(VehicleRoot);
		RCS->RegisterComponent();
		RCS->SetStaticMesh(Cylinder);
		RCS->SetRelativeLocation(FVector(
			FMath::Cos(Angle) * RCSR,
			FMath::Sin(Angle) * RCSR,
			S2Top - 30.0 * DiamScale));
		RCS->SetRelativeScale3D(FVector(0.08 * DiamScale, 0.08 * DiamScale, 0.15 * DiamScale));
		RCS->SetCollisionProfileName(TEXT("NoCollision"));
		if (DarkMat) RCS->SetMaterial(0, DarkMat);
		Stage2LooseParts.Add(RCS);
	}

	// S2 RP-1 tank band (gold thermal blanket visible on real MVac)
	{
		auto* GoldBand = NewObject<UStaticMeshComponent>(this, TEXT("S2_GoldBand"));
		GoldBand->SetupAttachment(VehicleRoot);
		GoldBand->RegisterComponent();
		GoldBand->SetStaticMesh(Cylinder);
		GoldBand->SetRelativeLocation(FVector(0, 0, S2Bot + S2H * 0.3));
		GoldBand->SetRelativeScale3D(FVector(BodyScaleXY * 1.005, BodyScaleXY * 1.005, S2H * 0.004));
		GoldBand->SetCollisionProfileName(TEXT("NoCollision"));
		if (GoldMat) GoldBand->SetMaterial(0, GoldMat);
		Stage2LooseParts.Add(GoldBand);
	}

	// Fairing — white composite like real Falcon 9
	FairingMesh = NewObject<UStaticMeshComponent>(this, TEXT("FairingBody"));
	FairingMesh->SetupAttachment(VehicleRoot);
	FairingMesh->RegisterComponent();
	FairingMesh->SetStaticMesh(Cylinder);
	FairingMesh->SetRelativeLocation(FVector(0, 0, FairingMid));
	FairingMesh->SetRelativeScale3D(FVector(BodyScaleXY * 1.08, BodyScaleXY * 1.08, FairingH / 100.0));
	FairingMesh->SetCollisionProfileName(TEXT("NoCollision"));
	UMaterialInterface* WhiteMat = LoadObject<UMaterialInterface>(nullptr,
		TEXT("/Game/NASA/Spacecraft/Landsat8/Materials/LDCM-white-fl.LDCM-white-fl"));
	if (!WhiteMat && BaseMat)
	{
		UMaterialInstanceDynamic* MID = UMaterialInstanceDynamic::Create(BaseMat, this);
		MID->SetVectorParameterValue(TEXT("Color"), FLinearColor(0.95f, 0.95f, 0.97f));
		WhiteMat = MID;
	}
	if (WhiteMat) FairingMesh->SetMaterial(0, WhiteMat);

	// Fairing vertical seam — the split line between the two halves
	{
		auto* Seam = NewObject<UStaticMeshComponent>(this, TEXT("FairingSeam"));
		Seam->SetupAttachment(VehicleRoot);
		Seam->RegisterComponent();
		Seam->SetStaticMesh(Cylinder);
		Seam->SetRelativeLocation(FVector(BodyRadius * 1.08 + 2.0, 0, FairingMid));
		Seam->SetRelativeScale3D(FVector(0.01, 0.01, FairingH / 100.0));
		Seam->SetCollisionProfileName(TEXT("NoCollision"));
		if (DarkMat) Seam->SetMaterial(0, DarkMat);
	}

	// Fairing access door (small rectangle on the fairing)
	{
		auto* Door = NewObject<UStaticMeshComponent>(this, TEXT("FairingDoor"));
		Door->SetupAttachment(VehicleRoot);
		Door->RegisterComponent();
		Door->SetStaticMesh(Cylinder);
		Door->SetRelativeLocation(FVector(-(BodyRadius * 1.08 + 2.0), 0, FairingBot + FairingH * 0.4));
		Door->SetRelativeScale3D(FVector(0.008, 0.04 * DiamScale, 0.03 * DiamScale));
		Door->SetCollisionProfileName(TEXT("NoCollision"));
		if (DarkMat) Door->SetMaterial(0, DarkMat);
	}

	// Nose cone
	if (Cone)
	{
		NoseCone = NewObject<UStaticMeshComponent>(this, TEXT("NoseConeVisual"));
		NoseCone->SetupAttachment(VehicleRoot);
		NoseCone->RegisterComponent();
		NoseCone->SetStaticMesh(Cone);
		NoseCone->SetRelativeLocation(FVector(0, 0, NoseZ + 200.0 * DiamScale));
		NoseCone->SetRelativeScale3D(FVector(BodyScaleXY * 1.08, BodyScaleXY * 1.08, 6.0 * DiamScale));
		NoseCone->SetCollisionProfileName(TEXT("NoCollision"));
		if (SilverMat) NoseCone->SetMaterial(0, SilverMat);
	}

	// Engine exhaust — 3-layer plume for realistic fire
	UMaterialInterface* ExhWhiteMat = WhiteMat;
	UMaterialInterface* ExhOrangeMat = LoadObject<UMaterialInterface>(nullptr,
		TEXT("/Game/NASA/Spacecraft/Landsat8/Materials/LDCM-thruster-out.LDCM-thruster-out"));
	if (!ExhOrangeMat && BaseMat)
	{
		UMaterialInstanceDynamic* MID = UMaterialInstanceDynamic::Create(BaseMat, this);
		MID->SetVectorParameterValue(TEXT("Color"), FLinearColor(1.0f, 0.5f, 0.05f));
		ExhOrangeMat = MID;
	}

	if (Cone)
	{
		const double ExhInitZ = -300.0 * DiamScale;

		// Inner core — white-hot combustion (brightest, narrowest)
		ExhaustMesh = NewObject<UStaticMeshComponent>(this, TEXT("ExhaustCore"));
		ExhaustMesh->SetupAttachment(VehicleRoot);
		ExhaustMesh->RegisterComponent();
		ExhaustMesh->SetStaticMesh(Cone);
		ExhaustMesh->SetRelativeLocation(FVector(0, 0, ExhInitZ));
		ExhaustMesh->SetRelativeScale3D(FVector(BodyScaleXY * 0.5, BodyScaleXY * 0.5, 8.0 * DiamScale));
		ExhaustMesh->SetRelativeRotation(FRotator(180.0, 0, 0));
		ExhaustMesh->SetCollisionProfileName(TEXT("NoCollision"));
		if (ExhWhiteMat) ExhaustMesh->SetMaterial(0, ExhWhiteMat);
		ExhaustMesh->SetVisibility(false);

		// Outer plume — orange expansion (wider, longer)
		ExhaustOuter = NewObject<UStaticMeshComponent>(this, TEXT("ExhaustOuter"));
		ExhaustOuter->SetupAttachment(VehicleRoot);
		ExhaustOuter->RegisterComponent();
		ExhaustOuter->SetStaticMesh(Cone);
		ExhaustOuter->SetRelativeLocation(FVector(0, 0, ExhInitZ));
		ExhaustOuter->SetRelativeScale3D(FVector(BodyScaleXY * 1.0, BodyScaleXY * 1.0, 10.0 * DiamScale));
		ExhaustOuter->SetRelativeRotation(FRotator(180.0, 0, 0));
		ExhaustOuter->SetCollisionProfileName(TEXT("NoCollision"));
		if (ExhOrangeMat) ExhaustOuter->SetMaterial(0, ExhOrangeMat);
		ExhaustOuter->SetVisibility(false);

		// Shock diamond layer — gold pulsing cones inside the plume
		ExhaustShock = NewObject<UStaticMeshComponent>(this, TEXT("ExhaustShock"));
		ExhaustShock->SetupAttachment(VehicleRoot);
		ExhaustShock->RegisterComponent();
		ExhaustShock->SetStaticMesh(Cone);
		ExhaustShock->SetRelativeLocation(FVector(0, 0, ExhInitZ));
		ExhaustShock->SetRelativeScale3D(FVector(BodyScaleXY * 0.3, BodyScaleXY * 0.3, 6.0 * DiamScale));
		ExhaustShock->SetRelativeRotation(FRotator(180.0, 0, 0));
		ExhaustShock->SetCollisionProfileName(TEXT("NoCollision"));
		if (GoldMat) ExhaustShock->SetMaterial(0, GoldMat);
		ExhaustShock->SetVisibility(false);
	}
}

void AAstralisLaunchVehicleActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	MissionElapsedTime_s += DeltaTime * ((MissionState != ELaunchVehicleState::OnPad &&
		MissionState != ELaunchVehicleState::Countdown) ? LAUNCH_TIME_SCALE : 1.0);
	UpdateMissionState(DeltaTime);
	UpdateFlightPhysics(DeltaTime);
	UpdateSeparatingStages(DeltaTime);

	const bool bInFlight = (MissionState != ELaunchVehicleState::OnPad &&
		MissionState != ELaunchVehicleState::Countdown);
	UpdateExhaust(bInFlight && MissionState != ELaunchVehicleState::Complete &&
		MissionState != ELaunchVehicleState::CoastPhase);

	// Telemetry sampling for HUD graph
	if (bInFlight)
	{
		TelemSampleTimer += DeltaTime;
		if (TelemSampleTimer >= 0.5 && TelemAltitude_km.Num() < TELEM_MAX)
		{
			TelemSampleTimer = 0.0;
			TelemAltitude_km.Add(Altitude_m / 1000.0);
			TelemVelocity_mps.Add(Velocity_mps);
			TelemTime_s.Add(MissionElapsedTime_s);
		}
	}

	// Trajectory trail — draw flight path arc
	if (bInFlight)
	{
		TrailSampleTimer_Launch += DeltaTime;
		if (TrailSampleTimer_Launch >= 0.05)
		{
			TrailSampleTimer_Launch = 0.0;
			TrailPositions.Add(GetActorLocation());
			if (TrailPositions.Num() > 500) TrailPositions.RemoveAt(0);
		}
		UWorld* W = GetWorld();
		if (W && TrailPositions.Num() > 1)
		{
			for (int32 i = 1; i < TrailPositions.Num(); ++i)
			{
				const float Alpha = static_cast<float>(i) / TrailPositions.Num();
				const uint8 R = static_cast<uint8>(30 + 200 * Alpha);
				const uint8 G = static_cast<uint8>(180 + 75 * Alpha);
				const uint8 B = 255;
				DrawDebugLine(W, TrailPositions[i - 1], TrailPositions[i],
					FColor(R, G, B), false, -1.0f, 0, 1.5f);
			}
		}
	}
}

void AAstralisLaunchVehicleActor::InitFlightState()
{
	FlightR_m = RE;
	FlightVr_mps = 0.0;
	// Earth rotation at ~28.5 deg latitude (Kennedy)
	FlightVt_mps = 7.2921e-5 * RE * FMath::Cos(FMath::DegreesToRadians(28.5));
	FlightTheta_rad = 0.0;
	PitchAngle_deg = 90.0;
	MaxQ_kPa = 0.0;
	MachNumber = 0.0;
	MaxGForce = 0.0;
	GravityLoss_mps = 0.0;
	DragLoss_mps = 0.0;
	ActualThrust_N = 0.0;
	FeltGForce = 0.0;
	bMaxQThrottleDown = false;
	bGroundImpact = false;
	bFairingJettisoned = false;
	bOrbitAchieved = false;
	SeparatingStages.Empty();
	OrbPeri_km = 0.0;
	OrbApo_km = 0.0;
	TelemAltitude_km.Empty();
	TelemVelocity_mps.Empty();
	TelemTime_s.Empty();
	TelemSampleTimer = 0.0;
}

void AAstralisLaunchVehicleActor::UpdateFlightPhysics(float DeltaTime)
{
	if (MissionState != ELaunchVehicleState::Liftoff &&
		MissionState != ELaunchVehicleState::Ascent &&
		MissionState != ELaunchVehicleState::CoastPhase &&
		MissionState != ELaunchVehicleState::Staging)
	{
		return;
	}

	AAstralisStageActor* CS = GetCurrentStage();
	if (!CS) return;

	double SimDt = DeltaTime * LAUNCH_TIME_SCALE;
	const double MaxStep = 0.1;

	while (SimDt > 0.0)
	{
		const double Step = FMath::Min(SimDt, MaxStep);
		SimDt -= Step;

		const double h = FlightR_m - RE;
		const double AtmP = FMath::Exp(FMath::Clamp(-h / 8500.0, -100.0, 0.0));

		const double TSL = CS->Spec.Thrust_N;
		const double TVac = (CS->Spec.ThrustVacuum_N > 0) ? CS->Spec.ThrustVacuum_N : TSL;
		const double ISL = CS->Spec.Isp_s;
		const double IVac = (CS->Spec.IspVacuum_s > 0) ? CS->Spec.IspVacuum_s : ISL;

		double Thrust = TVac - (TVac - TSL) * AtmP;
		double Isp = IVac - (IVac - ISL) * AtmP;
		Thrust *= ThrottlePct;

		if (CS->IsPropellantDepleted() || MissionState == ELaunchVehicleState::CoastPhase)
		{
			Thrust = 0.0;
			Isp = 0.0;
		}

		const double m = GetTotalMass_kg();
		if (m <= 0.0) break;

		const double g = MU / (FlightR_m * FlightR_m);
		const double cent = FlightVt_mps * FlightVt_mps / FlightR_m;

		// Pitch guidance
		// Stage 1: gentle arc from 90° down to ~35°, looks like a proper launch
		// Stage 2: closed-loop guidance finishes the push to orbital velocity
		double DesiredPitch = PitchAngle_deg;
		if (MissionElapsedTime_s < 12.0)
		{
			DesiredPitch = 90.0;
		}
		else if (CurrentStageIndex == 0)
		{
			// Smooth, gentle pitchover: 90° → 35° over the Stage 1 burn
			// This looks like a proper rocket launch, not a missile
			const double BurnRemain = FMath::Max(1.0, S1BurnTime_s - 12.0);
			const double f = FMath::Clamp((MissionElapsedTime_s - 12.0) / BurnRemain, 0.0, 1.0);
			// Ease-in-out for smooth visual arc
			const double eased = f * f * (3.0 - 2.0 * f);
			DesiredPitch = 90.0 - 55.0 * eased;
			DesiredPitch = FMath::Clamp(DesiredPitch, 35.0, 90.0);
		}
		else
		{
			// Stage 2: closed-loop guidance targeting orbit altitude
			const double w = 0.018;
			const double ar_desired = w * w * (TargetAltitude_m - h) - 2.0 * w * FlightVr_mps + g - cent;
			const double accel = (Thrust > 0) ? Thrust / m : 1.0;
			const double sg = FMath::Clamp(ar_desired / accel, -0.25, 1.0);
			DesiredPitch = FMath::RadiansToDegrees(FMath::Asin(FMath::Clamp(sg, -1.0, 1.0)));
			DesiredPitch = FMath::Clamp(DesiredPitch, -10.0, 35.0);

			if (FlightVr_mps < -50.0)
			{
				const double MaxRecoverPitch = FMath::Clamp(10.0 + FlightVr_mps * 0.04, -10.0, 20.0);
				DesiredPitch = FMath::Min(DesiredPitch, MaxRecoverPitch);
			}
		}

		// Rate-limit pitch changes (realistic TVC authority)
		const double MaxPitchRate = (CurrentStageIndex == 0) ? 5.0 : 8.0;
		const double PitchDelta = DesiredPitch - PitchAngle_deg;
		const double MaxChange = MaxPitchRate * Step;
		PitchAngle_deg += FMath::Clamp(PitchDelta, -MaxChange, MaxChange);

		// Atmospheric drag
		const double rho = 1.225 * AtmP;
		const double v = FMath::Sqrt(FlightVr_mps * FlightVr_mps + FlightVt_mps * FlightVt_mps);
		const double q = 0.5 * rho * v * v;
		CurrentQ_kPa = q / 1000.0;
		MaxQ_kPa = FMath::Max(MaxQ_kPa, CurrentQ_kPa);

		// Mach number (speed of sound varies with altitude temperature)
		const double TempK = 288.15 - 6.5e-3 * FMath::Min(h, 11000.0);
		const double SoundSpeed = FMath::Sqrt(1.4 * 287.05 * FMath::Max(TempK, 186.0));
		MachNumber = v / SoundSpeed;

		// Mach-dependent drag coefficient — transonic drag rise
		double Cd = 0.3;
		if (MachNumber > 0.8 && MachNumber < 1.2)
		{
			// Transonic drag rise: peaks at ~Mach 1.0-1.05
			const double MachDelta = FMath::Abs(MachNumber - 1.05);
			Cd = 0.3 + 0.4 * FMath::Max(0.0, 1.0 - MachDelta / 0.25);
		}
		else if (MachNumber >= 1.2)
		{
			// Supersonic: gradual decrease from transonic peak
			Cd = 0.3 + 0.15 / FMath::Max(1.0, MachNumber - 0.5);
		}
		const double RefArea = PI * 1.85 * 1.85 * (VehicleDiameter_m / 3.7) * (VehicleDiameter_m / 3.7);
		const double CdA = Cd * RefArea;
		const double D = q * CdA;

		// Max-Q auto throttle-down (Falcon 9 throttles to ~70% near Max-Q)
		if (CurrentStageIndex == 0 && CurrentQ_kPa > 25.0 && !bMaxQThrottleDown)
		{
			bMaxQThrottleDown = true;
		}
		if (bMaxQThrottleDown)
		{
			if (CurrentQ_kPa > MaxQ_kPa * 0.7)
			{
				Thrust *= 0.72;
			}
			else if (h > 15000.0)
			{
				bMaxQThrottleDown = false;
			}
		}

		// G-limit: throttle back to keep acceleration under 4G (crew/payload safety)
		if (Thrust > 0.0 && m > 0.0)
		{
			const double CurrentG = Thrust / (m * G0_PHYS);
			if (CurrentG > 4.0)
			{
				Thrust *= 4.0 / CurrentG;
			}
		}
		const double Dr = (v > 1.0) ? D * FlightVr_mps / v : 0.0;
		const double Dt = (v > 1.0) ? D * FlightVt_mps / v : 0.0;

		// Polar coordinate equations of motion
		const double ga = FMath::DegreesToRadians(PitchAngle_deg);
		const double ar = (Thrust * FMath::Sin(ga) - Dr) / m - g + cent;
		const double at = (Thrust * FMath::Cos(ga) - Dt) / m - FlightVr_mps * FlightVt_mps / FlightR_m;

		FlightVr_mps += ar * Step;
		FlightVt_mps += at * Step;
		FlightR_m += FlightVr_mps * Step;
		FlightTheta_rad += FlightVt_mps / FlightR_m * Step;

		// Track actual thrust and felt G-force (for HUD)
		ActualThrust_N = Thrust;
		if (m > 0.0)
		{
			// Felt G-force: total acceleration magnitude in units of g0
			const double accel_r = ar + g - cent; // restore gravity to get body-frame
			const double accel_t = at + FlightVr_mps * FlightVt_mps / FlightR_m;
			const double totalAccel = FMath::Sqrt(accel_r * accel_r + accel_t * accel_t);
			FeltGForce = totalAccel / G0_PHYS;
		}
		if (Thrust > 0.0 && m > 0.0)
		{
			const double Gforce = Thrust / (m * G0_PHYS);
			MaxGForce = FMath::Max(MaxGForce, Gforce);
			GravityLoss_mps += g * FMath::Sin(ga) * Step;
			if (v > 1.0) DragLoss_mps += (D / m) * Step;
		}

		if (FlightR_m < RE && FlightVr_mps < 0.0)
		{
			FlightR_m = RE;
			FlightVr_mps = 0.0;
			FlightVt_mps = 0.0;
			bGroundImpact = true;
			MissionState = ELaunchVehicleState::Complete;
			bOrbitAchieved = false;
			LogEvent(TEXT("VEHICLE LOST — ground impact"));
			ComputeMissionScore();
			return;
		}

		// Consume propellant
		if (!CS->IsPropellantDepleted() && Thrust > 0.0 && Isp > 0.0)
		{
			const double mdot = Thrust / (Isp * G0_PHYS);
			CS->CurrentPropellant_kg = FMath::Max(0.0, CS->CurrentPropellant_kg - mdot * Step);
		}

		// Fairing jettison — two halves split apart and tumble away
		if (!bFairingJettisoned && h > 110.0e3)
		{
			bFairingJettisoned = true;

			// Animate fairing halves separating sideways
			if (FairingMesh || NoseCone)
			{
				// Left half (fairing body)
				if (FairingMesh)
				{
					FSeparatingStage FairL;
					FairL.Timer = 0.0f;
					FairL.FadeDuration = 5.0f;
					FairL.RelVelocity = FVector(80.0f, 250.0f, 50.0f);
					FairL.TumbleRate = 25.0f;
					FairL.TumbleAxis = FVector(0.3f, 0.1f, 1.0f).GetSafeNormal();
					FairingMesh->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
					FairL.Parts.Add(FairingMesh.Get());
					FairL.OriginalScales.Add(FairingMesh->GetComponentScale());
					SeparatingStages.Add(MoveTemp(FairL));
				}
				// Right half (nose cone)
				if (NoseCone)
				{
					FSeparatingStage FairR;
					FairR.Timer = 0.0f;
					FairR.FadeDuration = 5.0f;
					FairR.RelVelocity = FVector(-80.0f, -250.0f, 50.0f);
					FairR.TumbleRate = 30.0f;
					FairR.TumbleAxis = FVector(-0.2f, 0.3f, 1.0f).GetSafeNormal();
					NoseCone->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
					FairR.Parts.Add(NoseCone.Get());
					FairR.OriginalScales.Add(NoseCone->GetComponentScale());
					SeparatingStages.Add(MoveTemp(FairR));
				}
			}

			LogEvent(FString::Printf(TEXT("Fairing jettison at %.1f km"), h / 1000.0));
		}
	}

	// Update public state
	Altitude_m = FlightR_m - RE;
	Velocity_mps = FMath::Sqrt(FlightVr_mps * FlightVr_mps + FlightVt_mps * FlightVt_mps);

	// Orbital elements
	const double E_orb = 0.5 * (FlightVr_mps * FlightVr_mps + FlightVt_mps * FlightVt_mps) - MU / FlightR_m;
	const double h_ang = FlightR_m * FlightVt_mps;
	if (E_orb < 0.0)
	{
		const double a_sma = -MU / (2.0 * E_orb);
		const double ecc = FMath::Sqrt(FMath::Max(0.0, 1.0 + 2.0 * E_orb * h_ang * h_ang / (MU * MU)));
		OrbPeri_km = (a_sma * (1.0 - ecc) - RE) / 1000.0;
		OrbApo_km = (a_sma * (1.0 + ecc) - RE) / 1000.0;
	}

	// Orbit achievement check during last-stage powered ascent
	if (MissionState == ELaunchVehicleState::Ascent &&
		CurrentStageIndex == Stages.Num() - 1 &&
		OrbPeri_km >= (TargetAltitude_m / 1000.0 - 25.0))
	{
		bOrbitAchieved = true;
		MissionState = ELaunchVehicleState::Complete;
		ComputeMissionScore();
		LogEvent(FString::Printf(TEXT("ORBIT ACHIEVED — %.0f x %.0f km"), OrbPeri_km, OrbApo_km));
		UE_LOG(LogTemp, Warning, TEXT("=== ORBIT ACHIEVED: %.0f x %.0f km ==="), OrbPeri_km, OrbApo_km);
	}

	// Check for anomalies
	CheckForAnomalies(DeltaTime);

	if (bAnomalyTriggered && ActiveAnomaly != EAnomalyType::None)
	{
		AnomalyTimer += DeltaTime * LAUNCH_TIME_SCALE;
		if (AnomalyTimer > 20.0)
		{
			ActiveAnomaly = EAnomalyType::None;
			AnomalyDescription.Empty();
			LogEvent(TEXT("Anomaly resolved — systems nominal"));
		}
	}

	// Visual position: altitude → Z, downrange → X
	// Altitude uses larger scale so rocket clearly separates from terrain.
	// Downrange is compressed but proportional enough that the arc looks correct.
	const double AltScale = 200.0;
	const double DownrangeScale = 80.0;
	const double UEAltitude = (Altitude_m / 1000.0) * AltScale;
	const double UEDownrange = (FlightTheta_rad * RE / 1000.0) * DownrangeScale;
	SetActorLocation(LaunchOrigin + FVector(UEDownrange, 0, UEAltitude));

	// Pitch the rocket visually
	SetActorRotation(FRotator(PitchAngle_deg - 90.0, 0, 0));
}

void AAstralisLaunchVehicleActor::UpdateExhaust(bool bActive)
{
	if (!ExhaustMesh) return;

	ExhaustMesh->SetVisibility(bActive);
	if (ExhaustOuter) ExhaustOuter->SetVisibility(bActive);
	if (ExhaustShock) ExhaustShock->SetVisibility(bActive);

	if (!bActive) return;

	const double h = FMath::Max(0.0, FlightR_m - RE);
	const double AtmP = FMath::Exp(FMath::Clamp(-h / 8500.0, -100.0, 0.0));
	const double VacExpand = 1.0 + 2.5 * (1.0 - AtmP);
	const double T = GetWorld()->GetTimeSeconds();
	const double DS = FMath::Max(0.5, ExhaustDiamScale);
	const double ExZ = (CurrentStageIndex == 0) ? -300.0 * DS : 2800.0 * DS;
	const double LenMult = 1.0 + 0.5 * (1.0 - AtmP);
	const double BaseW = 185.0 * DS / 50.0;

	// Core — tight white-hot flame, fast flicker
	{
		const double Flicker = 1.0 + 0.15 * FMath::Sin(T * 25.0) + 0.08 * FMath::Sin(T * 53.0);
		const double CoreW = BaseW * 0.5;
		const double CoreLen = ((CurrentStageIndex == 0) ? 8.0 : 5.0) * Flicker * LenMult * DS;
		ExhaustMesh->SetRelativeScale3D(FVector(CoreW * VacExpand, CoreW * VacExpand, CoreLen));
		ExhaustMesh->SetRelativeLocation(FVector(0, 0, ExZ));
	}

	// Outer plume — wider orange glow, slower flicker, longer
	if (ExhaustOuter)
	{
		const double Flicker = 1.0 + 0.10 * FMath::Sin(T * 14.0) + 0.07 * FMath::Cos(T * 31.0);
		const double OutW = BaseW * 1.0;
		const double OutLen = ((CurrentStageIndex == 0) ? 10.0 : 7.0) * Flicker * LenMult * DS;
		ExhaustOuter->SetRelativeScale3D(FVector(OutW * VacExpand, OutW * VacExpand, OutLen));
		ExhaustOuter->SetRelativeLocation(FVector(0, 0, ExZ - 50.0 * DS));
	}

	// Shock diamonds — pulsing bright nodes inside the plume
	if (ExhaustShock)
	{
		const double Pulse = 0.8 + 0.4 * FMath::Sin(T * 35.0) + 0.2 * FMath::Sin(T * 71.0);
		const double ShkW = BaseW * 0.3 * Pulse;
		const double ShkLen = ((CurrentStageIndex == 0) ? 6.0 : 4.0) * LenMult * DS;
		ExhaustShock->SetRelativeScale3D(FVector(ShkW, ShkW, ShkLen));
		ExhaustShock->SetRelativeLocation(FVector(0, 0, ExZ + 100.0 * DS));
	}
}

void AAstralisLaunchVehicleActor::StartCountdown()
{
	if (MissionState == ELaunchVehicleState::OnPad)
	{
		bSwapMode = false;
		MissionState = ELaunchVehicleState::Countdown;
		CountdownTimer_s = CountdownDuration_s;
		MissionElapsedTime_s = 0.0;
		MissionLog.Empty();
		LogEvent(TEXT("Countdown initiated"));
		UE_LOG(LogTemp, Warning, TEXT("=== COUNTDOWN STARTED: T-%.0f ==="), CountdownDuration_s);
	}
}

void AAstralisLaunchVehicleActor::AbortLaunch()
{
	MissionState = ELaunchVehicleState::OnPad;
	CountdownTimer_s = 0.0;

	for (AAstralisStageActor* Stage : Stages)
	{
		if (Stage) Stage->ShutdownEngines();
	}

	LogEvent(TEXT("ABORT — Launch aborted"));
	UE_LOG(LogTemp, Error, TEXT("=== LAUNCH ABORTED ==="));
}

void AAstralisLaunchVehicleActor::StageSeparation()
{
	if (CurrentStageIndex < Stages.Num())
	{
		AAstralisStageActor* OldStage = Stages[CurrentStageIndex];
		if (OldStage)
		{
			OldStage->bVehicleControlled = false;
			OldStage->Separate();
			LogEvent(FString::Printf(TEXT("Stage %d separation at %.1f km"), OldStage->StageNumber, Altitude_m / 1000.0));
			UE_LOG(LogTemp, Warning, TEXT("=== STAGE %d SEPARATION at ALT %.1f km ==="),
				OldStage->StageNumber, Altitude_m / 1000.0);
		}

		// Collect all visual parts of this stage for animated separation
		FSeparatingStage Sep;
		Sep.Timer = 0.0f;
		Sep.FadeDuration = 6.0f;

		UStaticMeshComponent* Body = (CurrentStageIndex == 0) ? Stage1Mesh.Get()
			: (CurrentStageIndex == 1) ? Stage2Mesh.Get() : nullptr;
		if (Body)
		{
			// Detach from vehicle root so it can move independently
			Body->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
			Sep.Parts.Add(Body);
		}
		const TArray<TObjectPtr<UStaticMeshComponent>>* Loose = (CurrentStageIndex == 0) ? &Stage1LooseParts
			: (CurrentStageIndex == 1) ? &Stage2LooseParts : nullptr;
		if (Loose)
		{
			for (UStaticMeshComponent* Part : *Loose)
			{
				if (Part)
				{
					Part->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
					Sep.Parts.Add(Part);
				}
			}
		}

		// Separation kick: downward + slight backward relative to rocket orientation
		const double PitchRad = FMath::DegreesToRadians(PitchAngle_deg);
		// Push away from the nose (opposite to flight direction) and slightly sideways
		Sep.RelVelocity = FVector(
			-FMath::Cos(PitchRad) * 300.0f,  // backward along body axis
			FMath::FRandRange(-50.0f, 50.0f),
			-FMath::Sin(PitchRad) * 300.0f - 150.0f  // downward kick
		);
		Sep.TumbleRate = FMath::FRandRange(15.0f, 40.0f);
		Sep.TumbleAxis = FVector(
			FMath::FRandRange(-0.3f, 0.3f),
			1.0f,
			FMath::FRandRange(-0.2f, 0.2f)
		).GetSafeNormal();

		for (UStaticMeshComponent* P : Sep.Parts)
		{
			Sep.OriginalScales.Add(P ? P->GetComponentScale() : FVector::OneVector);
		}

		if (Sep.Parts.Num() > 0)
		{
			SeparatingStages.Add(MoveTemp(Sep));
		}

		CurrentStageIndex++;

		if (CurrentStageIndex < Stages.Num())
		{
			MissionState = ELaunchVehicleState::Ascent;
			IgniteCurrentStage();
		}
		else
		{
			MissionState = ELaunchVehicleState::CoastPhase;
			CoastTimer = 0.0;
			LogEvent(FString::Printf(TEXT("All stages complete — coast at %.0f m/s"), Velocity_mps));
			UE_LOG(LogTemp, Warning, TEXT("=== ALL STAGES COMPLETE — COAST PHASE at %.0f m/s ==="), Velocity_mps);
		}
	}
}

void AAstralisLaunchVehicleActor::UpdateSeparatingStages(float DeltaTime)
{
	for (int32 i = SeparatingStages.Num() - 1; i >= 0; --i)
	{
		FSeparatingStage& Sep = SeparatingStages[i];
		Sep.Timer += DeltaTime;

		// Gravity pull accelerates the separated stage downward over time
		Sep.RelVelocity.Z -= 400.0f * DeltaTime;

		// Compute world offset this frame
		const FVector Offset = Sep.RelVelocity * DeltaTime;

		// Fade: fully visible for first 2s, then fade to invisible by FadeDuration
		float Alpha = 1.0f;
		if (Sep.Timer > 2.0f)
		{
			Alpha = FMath::Clamp(1.0f - (Sep.Timer - 2.0f) / (Sep.FadeDuration - 2.0f), 0.0f, 1.0f);
		}

		// Tumble rotation this frame
		const FQuat TumbleQuat(Sep.TumbleAxis, FMath::DegreesToRadians(Sep.TumbleRate * DeltaTime));

		for (int32 pi = 0; pi < Sep.Parts.Num(); ++pi)
		{
			UStaticMeshComponent* Part = Sep.Parts[pi];
			if (!Part) continue;

			// Move
			Part->AddWorldOffset(Offset, false);

			// Tumble around the group center (approximate: just rotate each part)
			FRotator CurRot = Part->GetComponentRotation();
			FQuat NewRot = TumbleQuat * CurRot.Quaternion();
			Part->SetWorldRotation(NewRot.Rotator());

			// Scale down to simulate receding into distance
			if (Sep.Timer > 1.0f && pi < Sep.OriginalScales.Num())
			{
				float Shrink = FMath::Clamp(1.0f - (Sep.Timer - 1.0f) * 0.15f, 0.3f, 1.0f);
				Part->SetWorldScale3D(Sep.OriginalScales[pi] * Shrink);
			}

			// Hide when fully faded
			if (Alpha <= 0.01f)
			{
				Part->SetVisibility(false);
			}
		}

		// Remove from list when done
		if (Sep.Timer >= Sep.FadeDuration)
		{
			for (UStaticMeshComponent* Part : Sep.Parts)
			{
				if (Part) Part->SetVisibility(false);
			}
			SeparatingStages.RemoveAt(i);
		}
	}
}

void AAstralisLaunchVehicleActor::DeployPayload()
{
	MissionState = ELaunchVehicleState::Deployment;
	// Fairing already jettisoned during ascent; just hide if somehow still visible
	if (FairingMesh && FairingMesh->IsVisible()) FairingMesh->SetVisibility(false);
	if (NoseCone && NoseCone->IsVisible()) NoseCone->SetVisibility(false);

	LogEvent(FString::Printf(TEXT("Payload deployed at %.1f km, %.0f m/s"), Altitude_m / 1000.0, Velocity_mps));
	UE_LOG(LogTemp, Warning, TEXT("=== PAYLOAD DEPLOYED at ALT %.1f km, VEL %.0f m/s ==="),
		Altitude_m / 1000.0, Velocity_mps);

	MissionState = ELaunchVehicleState::Complete;
}

double AAstralisLaunchVehicleActor::GetTotalMass_kg() const
{
	double TotalMass = 0.0;
	for (const AAstralisStageActor* Stage : Stages)
	{
		if (Stage && !Stage->bSeparated)
		{
			TotalMass += Stage->GetTotalMass_kg();
		}
	}
	if (Payload)
	{
		double PayloadMass = Payload->GetTotalMass();
		TotalMass += (PayloadMass > 0) ? PayloadMass : 2623.0;
	}
	return TotalMass;
}

double AAstralisLaunchVehicleActor::GetCurrentThrust_N() const
{
	double TotalThrust = 0.0;
	for (const AAstralisStageActor* Stage : Stages)
	{
		if (Stage && !Stage->bSeparated)
		{
			TotalThrust += Stage->GetCurrentThrust_N();
		}
	}
	return TotalThrust;
}

double AAstralisLaunchVehicleActor::GetThrustToWeightRatio() const
{
	const double Mass_kg = GetTotalMass_kg();
	if (Mass_kg <= 0.0) return 0.0;
	return GetCurrentThrust_N() / (Mass_kg * 9.80665);
}

double AAstralisLaunchVehicleActor::GetTotalDeltaV_mps() const
{
	double TotalDV = 0.0;
	double RemainingMass = Payload ? FMath::Max(Payload->GetTotalMass(), 2623.0) : 2623.0;

	for (int32 i = Stages.Num() - 1; i >= 0; --i)
	{
		const AAstralisStageActor* Stage = Stages[i];
		if (!Stage) continue;

		const double M0 = RemainingMass + Stage->Spec.DryMass_kg + Stage->Spec.PropellantCapacity_kg;
		const double M1 = RemainingMass + Stage->Spec.DryMass_kg;

		const double UseIsp = (Stage->Spec.IspVacuum_s > 0) ? Stage->Spec.IspVacuum_s : Stage->Spec.Isp_s;
		if (M0 > 0.0 && M1 > 0.0 && UseIsp > 0.0)
		{
			TotalDV += UseIsp * 9.80665 * FMath::Loge(M0 / M1);
		}

		RemainingMass = M0;
	}

	return TotalDV;
}

FString AAstralisLaunchVehicleActor::GetMissionStateString() const
{
	switch (MissionState)
	{
	case ELaunchVehicleState::OnPad:       return TEXT("ON PAD");
	case ELaunchVehicleState::Countdown:   return FString::Printf(TEXT("T-%.0f"), CountdownTimer_s);
	case ELaunchVehicleState::Liftoff:     return TEXT("LIFTOFF!");
	case ELaunchVehicleState::Ascent:      return FString::Printf(TEXT("ASCENT — STAGE %d"), CurrentStageIndex + 1);
	case ELaunchVehicleState::Staging:     return TEXT("STAGE SEPARATION");
	case ELaunchVehicleState::CoastPhase:  return TEXT("COAST PHASE");
	case ELaunchVehicleState::Deployment:  return TEXT("PAYLOAD DEPLOYMENT");
	case ELaunchVehicleState::Complete:    return TEXT("MISSION COMPLETE");
	default:                               return TEXT("UNKNOWN");
	}
}

AAstralisStageActor* AAstralisLaunchVehicleActor::GetCurrentStage() const
{
	if (CurrentStageIndex >= 0 && CurrentStageIndex < Stages.Num())
	{
		return Stages[CurrentStageIndex];
	}
	return nullptr;
}

void AAstralisLaunchVehicleActor::UpdateMissionState(float DeltaTime)
{
	switch (MissionState)
	{
	case ELaunchVehicleState::Countdown:
		CountdownTimer_s -= DeltaTime;

		if (CountdownTimer_s <= 0.0)
		{
			MissionState = ELaunchVehicleState::Liftoff;
			MissionElapsedTime_s = 0.0;
			InitFlightState();
			IgniteCurrentStage();
			LogEvent(TEXT("Liftoff — all engines at full thrust"));
		}
		break;

	case ELaunchVehicleState::Liftoff:
		if (MissionElapsedTime_s > 2.0)
		{
			MissionState = ELaunchVehicleState::Ascent;
		}
		break;

	case ELaunchVehicleState::Ascent:
		if (IsCurrentStageComplete())
		{
			MissionState = ELaunchVehicleState::Staging;
			StagingPauseTimer = 0.0;
		}
		break;

	case ELaunchVehicleState::Staging:
		StagingPauseTimer += DeltaTime;
		if (StagingPauseTimer > 0.5)
		{
			StageSeparation();
		}
		break;

	case ELaunchVehicleState::CoastPhase:
	{
		CoastTimer += DeltaTime;
		if (bOrbitAchieved || OrbPeri_km >= (TargetAltitude_m / 1000.0 - 25.0))
		{
			bOrbitAchieved = true;
			DeployPayload();
		}
		else if (CoastTimer > 10.0)
		{
			ComputeMissionScore();
			MissionState = ELaunchVehicleState::Complete;
		}
		break;
	}

	default:
		break;
	}
}

void AAstralisLaunchVehicleActor::IgniteCurrentStage()
{
	AAstralisStageActor* Stage = GetCurrentStage();
	if (Stage)
	{
		Stage->bVehicleControlled = true;
		Stage->IgniteEngines();
	}
}

bool AAstralisLaunchVehicleActor::IsCurrentStageComplete() const
{
	const AAstralisStageActor* Stage = GetCurrentStage();
	return Stage ? Stage->IsPropellantDepleted() : true;
}

void AAstralisLaunchVehicleActor::ComputeMissionScore()
{
	ScoreOrbitAccuracy = 0.0;
	ScoreCircularity = 0.0;
	ScoreFuelEfficiency = 0.0;
	ScoreStructural = 0.0;
	ScoreCompletion = 0.0;

	// 1. Orbit accuracy (40 points max) — how close perigee is to target
	const double TargetAlt_km = TargetAltitude_m / 1000.0;
	const double PeriError_km = FMath::Abs(OrbPeri_km - TargetAlt_km);
	if (bOrbitAchieved)
	{
		const double AccuracyPct = FMath::Clamp(1.0 - PeriError_km / FMath::Max(1.0, TargetAlt_km), 0.0, 1.0);
		ScoreOrbitAccuracy = 40.0 * AccuracyPct;
	}

	// 2. Orbit circularity (15 points max) — lower eccentricity = better
	if (bOrbitAchieved && OrbApo_km > 0 && OrbPeri_km > 0)
	{
		const double Diff_km = FMath::Abs(OrbApo_km - OrbPeri_km);
		const double CircPct = FMath::Clamp(1.0 - Diff_km / FMath::Max(1.0, TargetAlt_km), 0.0, 1.0);
		ScoreCircularity = 15.0 * CircPct;
	}

	// 3. Fuel efficiency (20 points max) — propellant remaining in final stage
	{
		double TotalPropRemain = 0.0;
		double TotalPropCap = 0.0;
		for (const AAstralisStageActor* Stage : Stages)
		{
			if (Stage && !Stage->bSeparated)
			{
				TotalPropRemain += Stage->CurrentPropellant_kg;
				TotalPropCap += Stage->Spec.PropellantCapacity_kg;
			}
		}
		if (TotalPropCap > 0)
		{
			ScoreFuelEfficiency = 20.0 * FMath::Clamp(TotalPropRemain / TotalPropCap, 0.0, 1.0);
		}
	}

	// 4. Structural safety (15 points max) — max Q within limits
	{
		const double QSafe = FMath::Clamp(1.0 - FMath::Max(0.0, MaxQ_kPa - 35.0) / 15.0, 0.0, 1.0);
		ScoreStructural = 15.0 * QSafe;
	}

	// 5. Mission completion bonus (10 points)
	if (bOrbitAchieved)
	{
		ScoreCompletion = 10.0;
	}

	double Score = ScoreOrbitAccuracy + ScoreCircularity + ScoreFuelEfficiency + ScoreStructural + ScoreCompletion;
	MissionScore = FMath::Clamp(Score, 0.0, 100.0);

	// Grade assignment
	if (MissionScore >= 95.0) MissionGrade = TEXT("S+");
	else if (MissionScore >= 90.0) MissionGrade = TEXT("S");
	else if (MissionScore >= 80.0) MissionGrade = TEXT("A");
	else if (MissionScore >= 70.0) MissionGrade = TEXT("B");
	else if (MissionScore >= 60.0) MissionGrade = TEXT("C");
	else if (MissionScore >= 40.0) MissionGrade = TEXT("D");
	else MissionGrade = TEXT("F");

	UE_LOG(LogTemp, Warning, TEXT("Mission Score: %.1f / 100 — Grade: %s"), MissionScore, *MissionGrade);
}

void AAstralisLaunchVehicleActor::SetThrottle(double Pct)
{
	ThrottlePct = FMath::Clamp(Pct, 0.5, 1.0);
}

void AAstralisLaunchVehicleActor::LogEvent(const FString& Event)
{
	MissionLog.Add({MissionElapsedTime_s, Event});
}

void AAstralisLaunchVehicleActor::CheckForAnomalies(double DeltaTime)
{
	if (bAnomalyTriggered || MissionState == ELaunchVehicleState::Complete ||
		MissionState == ELaunchVehicleState::OnPad || MissionState == ELaunchVehicleState::Countdown)
		return;

	const double AnomalyChancePerSec = 0.005;
	const double Roll = FMath::FRand();
	if (Roll > AnomalyChancePerSec * DeltaTime * LAUNCH_TIME_SCALE) return;

	const int32 Type = FMath::RandRange(1, 5);
	bAnomalyTriggered = true;
	AnomalyTimer = 0.0;

	switch (Type)
	{
	case 1:
		ActiveAnomaly = EAnomalyType::EngineOut;
		AnomalyDescription = TEXT("ENGINE OUT — COMPENSATING WITH REMAINING ENGINES");
		LogEvent(TEXT("ANOMALY: Engine shutdown detected"));
		break;
	case 2:
		ActiveAnomaly = EAnomalyType::GuidanceDeviation;
		AnomalyDescription = TEXT("GNC DEVIATION — FLIGHT COMPUTER CORRECTING");
		LogEvent(TEXT("ANOMALY: Guidance deviation detected"));
		break;
	case 3:
		ActiveAnomaly = EAnomalyType::ThrustOscillation;
		AnomalyDescription = TEXT("POGO OSCILLATION — MONITORING");
		LogEvent(TEXT("ANOMALY: Thrust oscillation detected"));
		break;
	case 4:
		ActiveAnomaly = EAnomalyType::PropellantLeak;
		AnomalyDescription = TEXT("PROPELLANT ANOMALY — REDUCED FLOW RATE");
		LogEvent(TEXT("ANOMALY: Propellant pressure low"));
		break;
	case 5:
		ActiveAnomaly = EAnomalyType::CommunicationsLoss;
		AnomalyDescription = TEXT("LOSS OF SIGNAL — AWAITING REACQUISITION");
		LogEvent(TEXT("ANOMALY: Communications loss"));
		break;
	}
}

// Engine swap

void AAstralisLaunchVehicleActor::ToggleSwapMode()
{
	if (MissionState != ELaunchVehicleState::OnPad) return;
	bSwapMode = !bSwapMode;
}

void AAstralisLaunchVehicleActor::SwapNextStage()
{
	if (!bSwapMode) return;
	SwapStageIndex = (SwapStageIndex + 1) % FMath::Max(1, Stages.Num());
}

void AAstralisLaunchVehicleActor::ApplyEngineFromCatalog(int32 Index)
{
	if (!bSwapMode) return;
	if (!SwapCatalog.IsValidIndex(Index)) return;
	if (!Stages.IsValidIndex(SwapStageIndex)) return;

	AAstralisStageActor* Stage = Stages[SwapStageIndex];
	if (!Stage) return;

	const FEngineSpec_Vehicle& NewEngine = SwapCatalog[Index];
	const int32 NumEng = Stage->Spec.NumEngines;

	// Adjust dry mass: remove old engine mass, add new
	Stage->Spec.DryMass_kg -= NumEng * Stage->Spec.SingleEngineMass_kg;
	Stage->Spec.SingleEngineMass_kg = NewEngine.Mass_kg;
	Stage->Spec.DryMass_kg += NumEng * NewEngine.Mass_kg;
	Stage->Spec.DryMass_kg = FMath::Max(500.0, Stage->Spec.DryMass_kg);

	// Apply thrust and Isp (total = numEngines * per-engine)
	Stage->Spec.Thrust_N = NumEng * ((NewEngine.ThrustSeaLevel_N > 0)
		? NewEngine.ThrustSeaLevel_N : NewEngine.ThrustVacuum_N);
	Stage->Spec.ThrustVacuum_N = NumEng * NewEngine.ThrustVacuum_N;
	Stage->Spec.Isp_s = (NewEngine.IspSeaLevel_s > 0)
		? NewEngine.IspSeaLevel_s : NewEngine.IspVacuum_s;
	Stage->Spec.IspVacuum_s = NewEngine.IspVacuum_s;
	Stage->Spec.EngineName = NewEngine.Name;

	// Reset propellant to full
	Stage->CurrentPropellant_kg = Stage->Spec.PropellantCapacity_kg;

	UE_LOG(LogTemp, Warning, TEXT("ENGINE SWAP: Stage %d -> %dx %s (%.0f kN, %.0f/%.0f s Isp)"),
		SwapStageIndex + 1, NumEng, *NewEngine.Name.ToString(),
		Stage->Spec.Thrust_N / 1000.0, Stage->Spec.Isp_s, Stage->Spec.IspVacuum_s);
}
