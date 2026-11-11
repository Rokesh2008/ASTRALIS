#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "AstralisComponentTypes.generated.h"

UENUM(BlueprintType)
enum class EComponentCategory : uint8
{
	Structure       UMETA(DisplayName = "Structure"),
	Power           UMETA(DisplayName = "Power"),
	Propulsion      UMETA(DisplayName = "Propulsion"),
	Avionics        UMETA(DisplayName = "Avionics"),
	Communications  UMETA(DisplayName = "Communications"),
	Thermal         UMETA(DisplayName = "Thermal"),
	Instrument      UMETA(DisplayName = "Scientific Instrument")
};

UENUM(BlueprintType)
enum class ESpecificationType : uint8
{
	Illustrative    UMETA(DisplayName = "Illustrative (game-balanced)"),
	Representative  UMETA(DisplayName = "Representative (based on real class)"),
	Referenced      UMETA(DisplayName = "Referenced (sourced specification)")
};

USTRUCT(BlueprintType)
struct ASTRALISDATA_API FPowerSpec
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (ToolTip = "Standby power (Watts)"))
	double Standby_W = 0.0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (ToolTip = "Average operating power (Watts)"))
	double Average_W = 0.0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (ToolTip = "Peak power (Watts)"))
	double Peak_W = 0.0;
};

USTRUCT(BlueprintType)
struct ASTRALISDATA_API FThermalSpec
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (Units = "Celsius"))
	double OperatingMin_C = -40.0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (Units = "Celsius"))
	double OperatingMax_C = 60.0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (ToolTip = "Thermal dissipation (Watts)"))
	double Dissipation_W = 0.0;
};

USTRUCT(BlueprintType)
struct ASTRALISDATA_API FGeometrySpec
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (Units = "Meters"))
	FVector Envelope_m = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FName MountingInterface;
};

USTRUCT(BlueprintType)
struct ASTRALISDATA_API FComponentMetadata
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	ESpecificationType SpecType = ESpecificationType::Illustrative;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FString Source;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 Revision = 1;
};

UCLASS(BlueprintType)
class ASTRALISDATA_API UAstralisComponentData : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Identity")
	FName ComponentID;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Identity")
	FText DisplayName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Identity")
	EComponentCategory Category;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Mass", meta = (Units = "Kilograms"))
	double Mass_kg = 0.0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Power")
	FPowerSpec Power;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Geometry")
	FGeometrySpec Geometry;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Thermal")
	FThermalSpec Thermal;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Data", meta = (ToolTip = "Data rate (Mbps)"))
	double DataRate_Mbps = 0.0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Cost")
	double Cost = 0.0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Metadata")
	FComponentMetadata Metadata;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Visual")
	TSoftObjectPtr<UStaticMesh> Mesh;

	virtual FPrimaryAssetId GetPrimaryAssetId() const override
	{
		return FPrimaryAssetId("AstralisComponent", ComponentID);
	}
};
