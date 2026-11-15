#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "AstralisData/VehicleLibrary/AstralisVehicleTypes.h"
#include "AstralisAssemblyGameMode.generated.h"

class AAstralisSpacecraftActor;
class AAstralisComponentActor;
class UAstralisComponentData;
class AAstralisAssemblyPC;
class AAstralisAssemblyHUD;
class AAstralisOrbitSceneActor;
class AAstralisLaunchVehicleActor;

UENUM(BlueprintType)
enum class EAstralisGamePhase : uint8
{
	Assembly    UMETA(DisplayName = "Assembly"),
	Launch      UMETA(DisplayName = "Launch Sequence"),
	Orbit       UMETA(DisplayName = "Orbit Simulation"),
	Reentry     UMETA(DisplayName = "Reentry")
};

USTRUCT()
struct FMissionDefinition
{
	GENERATED_BODY()

	FString Name;
	FString Description;
	double TargetAltitude_km = 400.0;
	double TargetInclination_deg = 28.5;
	FString OrbitType;
	double MinPayload_kg = 0.0;
};

UCLASS()
class ASTRALIS_API AAstralisAssemblyGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AAstralisAssemblyGameMode();

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(BlueprintReadOnly, Category = "Assembly")
	TObjectPtr<AAstralisSpacecraftActor> ActiveSpacecraft;

	UPROPERTY(BlueprintReadOnly, Category = "Assembly")
	TArray<TObjectPtr<AAstralisComponentActor>> InstalledComponents;

	UFUNCTION(BlueprintCallable, Category = "Assembly")
	AAstralisSpacecraftActor* SpawnSpacecraft();

	UFUNCTION(BlueprintCallable, Category = "Assembly")
	AAstralisComponentActor* AddSpacecraftComponent(UAstralisComponentData* Data, const FVector& Position_SI_m);

	UFUNCTION(BlueprintCallable, Category = "Assembly")
	bool RemoveSpacecraftComponent(AAstralisComponentActor* Component);

	UFUNCTION(BlueprintCallable, Category = "Assembly")
	void UpdateMassProperties();

	UFUNCTION(BlueprintCallable, Category = "Assembly")
	TArray<UAstralisComponentData*> GetAvailableComponents() const;

	// Phase transition
	UPROPERTY(BlueprintReadOnly, Category = "Mission")
	EAstralisGamePhase CurrentPhase = EAstralisGamePhase::Assembly;

	UFUNCTION(BlueprintCallable, Category = "Mission")
	void LaunchToOrbit();

	UFUNCTION(BlueprintCallable, Category = "Mission")
	void ReturnToAssembly();

	UPROPERTY(BlueprintReadOnly, Category = "Mission")
	TObjectPtr<AAstralisOrbitSceneActor> OrbitScene;

	UPROPERTY(BlueprintReadOnly, Category = "Mission")
	TObjectPtr<AAstralisLaunchVehicleActor> LaunchVehicle;

	UPROPERTY()
	TObjectPtr<AActor> LaunchPadActor;

	UFUNCTION(BlueprintCallable, Category = "Mission")
	void CreateExampleLaunchVehicle();

	void TransitionToOrbit();
	void TransitionToReentry();

	// Vehicle selection
	void CycleVehicle(int32 Direction);
	int32 GetSelectedVehicleIndex() const { return SelectedVehicleIndex; }
	const TArray<FLaunchVehicleDefinition>& GetVehicleLibrary() const { return VehicleLibrary; }
	const FLaunchVehicleDefinition& GetSelectedVehicle() const { static FLaunchVehicleDefinition Fallback; return VehicleLibrary.IsValidIndex(SelectedVehicleIndex) ? VehicleLibrary[SelectedVehicleIndex] : Fallback; }

	// Mission selection
	void CycleMission(int32 Direction);
	int32 GetSelectedMissionIndex() const { return SelectedMissionIndex; }
	const TArray<FMissionDefinition>& GetMissions() const { return Missions; }
	const FMissionDefinition& GetSelectedMission() const { static FMissionDefinition Fallback; return Missions.IsValidIndex(SelectedMissionIndex) ? Missions[SelectedMissionIndex] : Fallback; }

	// Weather / launch conditions (randomized per session)
	double WindSpeed_kts = 0.0;
	double WindDirection_deg = 0.0;
	double Temperature_F = 72.0;
	double CloudCover_pct = 0.0;
	double Visibility_km = 16.0;
	FString WeatherCondition;
	bool bLaunchGO = true;
	void RandomizeWeather();
	double GetMissionDifficulty() const;

	// Reentry simulation state (public for HUD access)
	float ReentryTimer = 0.0f;
	float ReentrySplashTimer = 0.0f;
	double ReentryAltitude_km = 400.0;
	double ReentryVelocity_mps = 7670.0;
	double ReentryTemperature_C = 20.0;
	bool bReentryComplete = false;
	bool bPlasmaBlackout = false;
	bool bParachuteDeployed = false;

	// ── SPACECRAFT DESIGN SYSTEM ──────────────────────────────────────────
	// Spacecraft bus selection
	TArray<FSpacecraftBusDefinition> BusCatalog;
	int32 SelectedBusIndex = 0;
	void CycleBus(int32 Direction);
	const FSpacecraftBusDefinition& GetSelectedBus() const { static FSpacecraftBusDefinition Fallback; return BusCatalog.IsValidIndex(SelectedBusIndex) ? BusCatalog[SelectedBusIndex] : Fallback; }

	// Instrument selection (toggle on/off)
	TArray<FInstrumentDefinition> InstrumentCatalog;
	TArray<bool> InstrumentSelected; // parallel to catalog
	void ToggleInstrument(int32 Index);
	int32 GetSelectedInstrumentCount() const;
	double GetInstrumentMass_kg() const;
	double GetInstrumentPower_W() const;
	double GetInstrumentDataRate_Mbps() const;
	double GetInstrumentCost_M() const;
	double GetScienceMultiplier() const;
	TArray<FInstrumentDefinition> GetActiveInstruments() const;

	// Budget
	FMissionBudget Budget;
	void RecalculateBudget();
	double GetLaunchVehicleCost_M() const;

	// Spacecraft resource state (live during orbit)
	struct FSpacecraftResources
	{
		double BatteryCharge_Wh = 0.0;
		double BatteryMax_Wh = 2000.0;
		double PowerGeneration_W = 1000.0;
		double PowerDraw_W = 0.0;
		double DataStored_GB = 0.0;
		double DataStorageMax_GB = 256.0;
		double DataDownlinked_GB = 0.0;
		double PropellantRemaining_kg = 50.0;
		double ThermalLoad_W = 0.0;
		double ThermalCapacity_W = 500.0;
		bool bInSunlight = true;
		bool bInGroundContact = true;
		bool bThermalWarning = false;
		bool bPowerWarning = false;
		double MissionTime_s = 0.0;
		double ScienceCollected = 0.0;
	};
	FSpacecraftResources SCResources;
	void InitSpacecraftResources();
	void UpdateSpacecraftResources(float DeltaTime);

	// Catalog initialization
	void InitBusCatalog();
	void InitInstrumentCatalog();

	// Comprehensive mission score (launch + design + science)
	struct FComprehensiveScore
	{
		double LaunchScore = 0.0;      // from ComputeMissionScore (0-100)
		double BudgetBonus = 0.0;      // under budget = bonus (0-15)
		double ScienceBonus = 0.0;     // science collected bonus (0-15)
		double DesignBonus = 0.0;      // instrument compatibility bonus (0-10)
		double EfficiencyBonus = 0.0;  // power/mass margin bonus (0-10)
		double GetTotal() const { return FMath::Clamp(LaunchScore + BudgetBonus + ScienceBonus + DesignBonus + EfficiencyBonus, 0.0, 150.0); }
		FString GetGrade() const;
	};
	FComprehensiveScore ComputeComprehensiveScore() const;

	// Flight statistics (session-wide)
	struct FFlightRecord
	{
		FString VehicleName;
		FString MissionName;
		FString Grade;
		double Score = 0.0;
		double MaxAltitude_km = 0.0;
		double MaxVelocity_mps = 0.0;
		bool bSuccess = false;
	};
	TArray<FFlightRecord> FlightHistory;
	int32 TotalLaunches = 0;
	int32 SuccessfulMissions = 0;
	double BestScore = 0.0;
	FString BestGrade;
	void RecordFlightResult();

private:
	float OrbitTransitionTimer = 0.0f;
	bool bWaitingForOrbitTransition = false;

	// Vehicle library
	TArray<FLaunchVehicleDefinition> VehicleLibrary;
	int32 SelectedVehicleIndex = 0;

	// Mission definitions
	TArray<FMissionDefinition> Missions;
	int32 SelectedMissionIndex = 0;

	void InitMissions();
	void BuildVehicleFromDefinition(const FLaunchVehicleDefinition& Def);

	// Liftoff smoke particles
	struct FSmokePuff { FVector Position; FVector Velocity; float Scale; float Life; };
	TArray<FSmokePuff> SmokePuffs;
	FVector LaunchPadOrigin = FVector::ZeroVector;
	void FocusCameraOnPad();
	void DestroyLaunchVehicle();
	void SetLevelEnvironmentVisible(bool bVisible);

	TArray<TWeakObjectPtr<AActor>> HiddenForOrbit;
};
