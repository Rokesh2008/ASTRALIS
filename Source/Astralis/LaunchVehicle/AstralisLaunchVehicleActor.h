#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AstralisStageActor.h"
#include "Astralis/Spacecraft/AstralisSpacecraftActor.h"
#include "AstralisData/VehicleLibrary/AstralisVehicleTypes.h"
#include "AstralisLaunchVehicleActor.generated.h"

UENUM(BlueprintType)
enum class ELaunchVehicleState : uint8
{
	OnPad         UMETA(DisplayName = "On Launch Pad"),
	Countdown     UMETA(DisplayName = "Countdown"),
	Liftoff       UMETA(DisplayName = "Liftoff"),
	Ascent        UMETA(DisplayName = "Powered Ascent"),
	Staging       UMETA(DisplayName = "Staging"),
	CoastPhase    UMETA(DisplayName = "Coast Phase"),
	Deployment    UMETA(DisplayName = "Payload Deployment"),
	Complete      UMETA(DisplayName = "Mission Complete")
};

UCLASS()
class ASTRALIS_API AAstralisLaunchVehicleActor : public AActor
{
	GENERATED_BODY()

public:
	AAstralisLaunchVehicleActor();

	// Root
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USceneComponent> VehicleRoot;

	// Visual components — built procedurally in BeginPlay
	UPROPERTY()
	TObjectPtr<UStaticMeshComponent> Stage1Mesh;

	UPROPERTY()
	TObjectPtr<UStaticMeshComponent> Stage2Mesh;

	UPROPERTY()
	TObjectPtr<UStaticMeshComponent> FairingMesh;

	UPROPERTY()
	TObjectPtr<UStaticMeshComponent> NoseCone;

	// Parts not parented to a stage body that must disappear with that stage
	UPROPERTY()
	TArray<TObjectPtr<UStaticMeshComponent>> Stage1LooseParts;

	UPROPERTY()
	TArray<TObjectPtr<UStaticMeshComponent>> Stage2LooseParts;

	// Stage configuration
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Launch Vehicle")
	TArray<AAstralisStageActor*> Stages;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Launch Vehicle")
	TObjectPtr<AAstralisSpacecraftActor> Payload;

	// Mission state
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mission")
	ELaunchVehicleState MissionState = ELaunchVehicleState::OnPad;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mission")
	int32 CurrentStageIndex = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mission")
	double MissionElapsedTime_s = 0.0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mission")
	double CountdownDuration_s = 5.0;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mission")
	double CountdownTimer_s = 0.0;

	// Flight state
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Flight")
	double Altitude_m = 0.0;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Flight")
	double Velocity_mps = 0.0;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Flight")
	double Acceleration_mps2 = 0.0;

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable, Category = "Launch")
	void StartCountdown();

	UFUNCTION(BlueprintCallable, Category = "Launch")
	void AbortLaunch();

	UFUNCTION(BlueprintCallable, Category = "Launch")
	void StageSeparation();

	UFUNCTION(BlueprintCallable, Category = "Launch")
	void DeployPayload();

	UFUNCTION(BlueprintCallable, Category = "Engine Swap")
	void ToggleSwapMode();

	UFUNCTION(BlueprintCallable, Category = "Engine Swap")
	void SwapNextStage();

	UFUNCTION(BlueprintCallable, Category = "Engine Swap")
	void ApplyEngineFromCatalog(int32 Index);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Engine Swap")
	int32 SwapStageIndex = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Engine Swap")
	bool bSwapMode = false;

	UFUNCTION(BlueprintPure, Category = "Performance")
	double GetTotalMass_kg() const;

	UFUNCTION(BlueprintPure, Category = "Performance")
	double GetCurrentThrust_N() const;

	UFUNCTION(BlueprintPure, Category = "Performance")
	double GetThrustToWeightRatio() const;

	UFUNCTION(BlueprintPure, Category = "Performance")
	double GetTotalDeltaV_mps() const;

	UFUNCTION(BlueprintPure, Category = "Performance")
	FString GetMissionStateString() const;

	UFUNCTION(BlueprintPure, Category = "Performance")
	AAstralisStageActor* GetCurrentStage() const;

	double GetPitchAngle_deg() const { return PitchAngle_deg; }
	double GetMaxQ_kPa() const { return MaxQ_kPa; }
	double GetCurrentQ_kPa() const { return CurrentQ_kPa; }
	double GetDownrange_km() const { return FlightTheta_rad * RE / 1000.0; }
	double GetTargetAltitude_m() const { return TargetAltitude_m; }
	void SetTargetAltitude(double Alt_m) { TargetAltitude_m = Alt_m; }
	const TArray<FEngineSpec_Vehicle>& GetSwapCatalog() const { return SwapCatalog; }
	double GetMachNumber() const { return MachNumber; }
	double GetMaxGForce() const { return MaxGForce; }
	double GetGravityLoss_mps() const { return GravityLoss_mps; }
	double GetDragLoss_mps() const { return DragLoss_mps; }

	// Actual flight-frame values (altitude-adjusted, used by HUD)
	double GetActualThrust_N() const { return ActualThrust_N; }
	double GetFeltGForce() const { return FeltGForce; }
	double GetVerticalSpeed_mps() const { return FlightVr_mps; }
	double GetTangentialSpeed_mps() const { return FlightVt_mps; }
	bool IsGroundImpact() const { return bGroundImpact; }

	// Vehicle dimensions for visual scaling
	double VehicleDiameter_m = 3.7;
	double VehicleS1Length_m = 42.6;
	double VehicleS2Length_m = 13.1;
	FString VehicleDisplayName;

	// Stage 1 burn time — set from vehicle library for adaptive gravity turn
	double S1BurnTime_s = 162.0;

	// Throttle control (50%-100%)
	double ThrottlePct = 1.0;
	void SetThrottle(double Pct);

	// Mission performance scoring
	double MissionScore = 0.0;
	FString MissionGrade;
	double ScoreOrbitAccuracy = 0.0;
	double ScoreCircularity = 0.0;
	double ScoreFuelEfficiency = 0.0;
	double ScoreStructural = 0.0;
	double ScoreCompletion = 0.0;
	void ComputeMissionScore();

private:
	void BuildRocketVisual();
	void UpdateMissionState(float DeltaTime);
	void UpdateFlightPhysics(float DeltaTime);
	void IgniteCurrentStage();
	bool IsCurrentStageComplete() const;

	// Flight sim
	double StagingPauseTimer = 0.0;
	double CoastTimer = 0.0;
	FVector LaunchOrigin = FVector::ZeroVector;

	// 2D polar coordinate ascent simulation
	static constexpr double RE = 6.371e6;
	static constexpr double MU = 3.986004418e14;
	static constexpr double G0_PHYS = 9.80665;
	static constexpr double LAUNCH_TIME_SCALE = 10.0;

	double FlightR_m = 0.0;
	double FlightVr_mps = 0.0;
	double FlightVt_mps = 0.0;
	double FlightTheta_rad = 0.0;
	double PitchAngle_deg = 90.0;
	double MaxQ_kPa = 0.0;
	double CurrentQ_kPa = 0.0;
	bool bFairingJettisoned = false;
	double TargetAltitude_m = 400.0e3;
	double MachNumber = 0.0;
	double MaxGForce = 0.0;
	double GravityLoss_mps = 0.0;
	double DragLoss_mps = 0.0;

	// Actual frame state (altitude-adjusted)
	double ActualThrust_N = 0.0;
	double FeltGForce = 0.0;
	bool bMaxQThrottleDown = false;
	bool bGroundImpact = false;

	// Trajectory trail for visual flight path
	TArray<FVector> TrailPositions;
	double TrailSampleTimer_Launch = 0.0;

	double TelemSampleTimer = 0.0;

public:
	// Telemetry history for graph display
	static constexpr int32 TELEM_MAX = 200;
	TArray<double> TelemAltitude_km;
	TArray<double> TelemVelocity_mps;
	TArray<double> TelemTime_s;

	double OrbPeri_km = 0.0;
	double OrbApo_km = 0.0;
	bool bOrbitAchieved = false;

	// Mission event log
	struct FMissionEvent { double MET_s; FString Event; };
	TArray<FMissionEvent> MissionLog;
	void LogEvent(const FString& Event);

	// Anomaly / failure system
	enum class EAnomalyType : uint8 { None, EngineOut, GuidanceDeviation, ThrustOscillation, PropellantLeak, CommunicationsLoss };
	EAnomalyType ActiveAnomaly = EAnomalyType::None;
	double AnomalyTimer = 0.0;
	bool bAnomalyTriggered = false;
	FString AnomalyDescription;
	void CheckForAnomalies(double DeltaTime);

private:

	// Engine swap catalog
	TArray<FEngineSpec_Vehicle> SwapCatalog;

	// Engine exhaust visual — multi-layered for realism
	UPROPERTY()
	TObjectPtr<UStaticMeshComponent> ExhaustMesh;
	UPROPERTY()
	TObjectPtr<UStaticMeshComponent> ExhaustOuter;
	UPROPERTY()
	TObjectPtr<UStaticMeshComponent> ExhaustShock;
	void UpdateExhaust(bool bActive);
	void InitFlightState();
	double ExhaustDiamScale = 1.0;

	// Separated stage falling animation
	struct FSeparatingStage
	{
		TArray<UStaticMeshComponent*> Parts;
		TArray<FVector> OriginalScales;
		FVector RelVelocity;      // separation velocity in world space
		float TumbleRate;         // degrees per second
		float Timer;              // elapsed since separation
		float FadeDuration;       // total fade time
		FVector TumbleAxis;
	};
	TArray<FSeparatingStage> SeparatingStages;
	void UpdateSeparatingStages(float DeltaTime);
};
