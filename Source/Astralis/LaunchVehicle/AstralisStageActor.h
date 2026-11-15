#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AstralisStageActor.generated.h"

// Simplified stage specification for emergency 24h implementation
USTRUCT(BlueprintType)
struct FStageSpec
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stage Specs")
	FText StageName = FText::FromString("Stage");

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stage Specs", meta = (Units = "kg"))
	double DryMass_kg = 1000.0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stage Specs", meta = (Units = "kg"))
	double PropellantCapacity_kg = 10000.0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stage Specs", meta = (Units = "N"))
	double Thrust_N = 50000.0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stage Specs", meta = (Units = "s"))
	double Isp_s = 300.0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stage Specs", meta = (Units = "s"))
	double BurnTime_s = 120.0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stage Specs", meta = (Units = "N"))
	double ThrustVacuum_N = 0.0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stage Specs", meta = (Units = "s"))
	double IspVacuum_s = 0.0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stage Specs")
	int32 NumEngines = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stage Specs", meta = (Units = "kg"))
	double SingleEngineMass_kg = 470.0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stage Specs")
	FName EngineName;
};

// Individual rocket stage with propellant and engine
UCLASS()
class ASTRALIS_API AAstralisStageActor : public AActor
{
	GENERATED_BODY()

public:
	AAstralisStageActor();

	// Stage configuration
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stage")
	int32 StageNumber = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stage")
	FStageSpec Spec;

	// Current state
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stage State")
	double CurrentPropellant_kg = 0.0;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stage State")
	bool bEnginesActive = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stage State")
	bool bSeparated = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stage State")
	double BurnTimer_s = 0.0;

	// Visual
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USceneComponent> StageRoot;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> StageMesh;

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

	// Stage control
	UFUNCTION(BlueprintCallable, Category = "Stage")
	void IgniteEngines();

	UFUNCTION(BlueprintCallable, Category = "Stage")
	void ShutdownEngines();

	UFUNCTION(BlueprintCallable, Category = "Stage")
	void Separate();

	// Mass calculations
	UFUNCTION(BlueprintPure, Category = "Stage")
	double GetTotalMass_kg() const;

	UFUNCTION(BlueprintPure, Category = "Stage")
	double GetCurrentThrust_N() const;

	UFUNCTION(BlueprintPure, Category = "Stage")
	double GetMassFlowRate_kgps() const;

	UFUNCTION(BlueprintPure, Category = "Stage")
	bool IsPropellantDepleted() const;

	// Burn propellant (called by vehicle actor)
	void ConsumePropellant(double DeltaTime_s);

	double SeparationTimer = 0.0;
	bool bVehicleControlled = false;
};
