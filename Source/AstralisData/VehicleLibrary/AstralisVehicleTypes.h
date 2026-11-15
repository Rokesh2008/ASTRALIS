#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "AstralisVehicleTypes.generated.h"

UENUM(BlueprintType)
enum class EEngineType : uint8
{
	GasGenerator       UMETA(DisplayName = "Gas Generator"),
	StagedCombustion   UMETA(DisplayName = "Staged Combustion"),
	ExpanderCycle      UMETA(DisplayName = "Expander Cycle"),
	PressureFed        UMETA(DisplayName = "Pressure Fed"),
	SolidMotor         UMETA(DisplayName = "Solid Motor"),
	Hybrid             UMETA(DisplayName = "Hybrid")
};

UENUM(BlueprintType)
enum class EPropellantType : uint8
{
	LOX_RP1      UMETA(DisplayName = "LOX / RP-1"),
	LOX_LH2      UMETA(DisplayName = "LOX / LH2"),
	NTO_UDMH     UMETA(DisplayName = "NTO / UDMH"),
	SolidHTPB    UMETA(DisplayName = "Solid (HTPB)"),
	LOX_Methane  UMETA(DisplayName = "LOX / Methane")
};

UENUM(BlueprintType)
enum class EAttachmentInterface : uint8
{
	Standard1194mm  UMETA(DisplayName = "1194 mm Standard"),
	Standard937mm   UMETA(DisplayName = "937 mm Standard"),
	Standard666mm   UMETA(DisplayName = "666 mm Standard"),
	Custom          UMETA(DisplayName = "Custom Interface")
};

UENUM(BlueprintType)
enum class EVehicleStatus : uint8
{
	Historical    UMETA(DisplayName = "Historical"),
	Operational   UMETA(DisplayName = "Operational"),
	Retired       UMETA(DisplayName = "Retired"),
	Developmental UMETA(DisplayName = "Developmental"),
	Illustrative  UMETA(DisplayName = "Illustrative")
};

// ---------------------------------------------------------------------------
// Engine specification
// ---------------------------------------------------------------------------
USTRUCT(BlueprintType)
struct ASTRALISDATA_API FEngineSpec_Vehicle
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Identity")
	FName Name;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Identity")
	EEngineType EngineType = EEngineType::GasGenerator;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Performance", meta = (Units = "N"))
	double ThrustSeaLevel_N = 0.0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Performance", meta = (Units = "N"))
	double ThrustVacuum_N = 0.0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Performance", meta = (Units = "s"))
	double IspSeaLevel_s = 0.0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Performance", meta = (Units = "s"))
	double IspVacuum_s = 0.0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Performance")
	double MassFlowRate_kgps = 0.0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Physical", meta = (Units = "kg"))
	double Mass_kg = 0.0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Physical", meta = (Units = "m"))
	double Diameter_m = 0.0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Physical", meta = (Units = "m"))
	double Length_m = 0.0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Propellant")
	EPropellantType PropellantType = EPropellantType::LOX_RP1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Configuration")
	int32 NumChambers = 1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Interface")
	EAttachmentInterface AttachmentInterface = EAttachmentInterface::Custom;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Provenance")
	FString Manufacturer;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Provenance")
	FString SpecSource;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Provenance", meta = (ClampMin = "0", ClampMax = "1"))
	double SpecConfidence = 0.0;
};

// ---------------------------------------------------------------------------
// Stage definition
// ---------------------------------------------------------------------------
USTRUCT(BlueprintType)
struct ASTRALISDATA_API FStageDefinition
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Identity")
	FText StageName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Identity")
	int32 StageNumber = 1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Mass", meta = (Units = "kg"))
	double DryMass_kg = 0.0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Mass", meta = (Units = "kg"))
	double PropellantCapacity_kg = 0.0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Propellant")
	EPropellantType PropellantType = EPropellantType::LOX_RP1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dimensions", meta = (Units = "m"))
	double Diameter_m = 0.0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dimensions", meta = (Units = "m"))
	double Length_m = 0.0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Propulsion")
	FEngineSpec_Vehicle EngineSpec;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Propulsion")
	int32 NumEngines = 1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Interface")
	EAttachmentInterface UpperInterface = EAttachmentInterface::Custom;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Interface")
	EAttachmentInterface LowerInterface = EAttachmentInterface::Custom;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Recovery")
	bool bHasRecoverySystem = false;
};

// ---------------------------------------------------------------------------
// Complete launch vehicle definition
// ---------------------------------------------------------------------------
USTRUCT(BlueprintType)
struct ASTRALISDATA_API FLaunchVehicleDefinition
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Identity")
	FName VehicleID;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Identity")
	FText DisplayName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Identity")
	FString Country;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Identity")
	FString Manufacturer;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Identity")
	EVehicleStatus Status = EVehicleStatus::Illustrative;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Identity")
	FText Description;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Architecture")
	TArray<FStageDefinition> Stages;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Performance", meta = (Units = "kg"))
	double PayloadToLEO_kg = 0.0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Performance", meta = (Units = "kg"))
	double PayloadToGTO_kg = 0.0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Provenance")
	FString FirstFlight;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Provenance")
	FString SpecSource;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Provenance", meta = (ClampMin = "0", ClampMax = "1"))
	double DataCompleteness = 0.0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Modification")
	bool bIsModified = false;
};

// ---------------------------------------------------------------------------
// Scientific Instrument — selectable payload instruments
// ---------------------------------------------------------------------------
UENUM(BlueprintType)
enum class EInstrumentType : uint8
{
	Camera           UMETA(DisplayName = "Imaging Camera"),
	Spectrometer     UMETA(DisplayName = "Spectrometer"),
	Radar            UMETA(DisplayName = "SAR Radar"),
	Magnetometer     UMETA(DisplayName = "Magnetometer"),
	ParticleDetector UMETA(DisplayName = "Particle Detector"),
	Radiometer       UMETA(DisplayName = "Radiometer"),
	Lidar            UMETA(DisplayName = "LIDAR"),
	GravityGrad      UMETA(DisplayName = "Gravity Gradiometer"),
	CommRelay        UMETA(DisplayName = "Communications Relay"),
	TechDemo         UMETA(DisplayName = "Technology Demo")
};

USTRUCT(BlueprintType)
struct ASTRALISDATA_API FInstrumentDefinition
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly) FName ID;
	UPROPERTY(EditAnywhere, BlueprintReadOnly) FString Name;
	UPROPERTY(EditAnywhere, BlueprintReadOnly) FString Description;
	UPROPERTY(EditAnywhere, BlueprintReadOnly) EInstrumentType Type = EInstrumentType::Camera;
	UPROPERTY(EditAnywhere, BlueprintReadOnly) double Mass_kg = 50.0;
	UPROPERTY(EditAnywhere, BlueprintReadOnly) double Power_W = 100.0;
	UPROPERTY(EditAnywhere, BlueprintReadOnly) double DataRate_Mbps = 10.0;
	UPROPERTY(EditAnywhere, BlueprintReadOnly) double Cost_M = 25.0; // millions USD
	UPROPERTY(EditAnywhere, BlueprintReadOnly) double ScienceValue = 1.0; // multiplier on mission score
	UPROPERTY(EditAnywhere, BlueprintReadOnly) double MinAltitude_km = 0.0;
	UPROPERTY(EditAnywhere, BlueprintReadOnly) double MaxAltitude_km = 100000.0;
	UPROPERTY(EditAnywhere, BlueprintReadOnly) bool bRequiresSunPointing = false;
};

// ---------------------------------------------------------------------------
// Spacecraft Bus — the platform that carries instruments
// ---------------------------------------------------------------------------
USTRUCT(BlueprintType)
struct ASTRALISDATA_API FSpacecraftBusDefinition
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly) FName ID;
	UPROPERTY(EditAnywhere, BlueprintReadOnly) FString Name;
	UPROPERTY(EditAnywhere, BlueprintReadOnly) FString Description;
	UPROPERTY(EditAnywhere, BlueprintReadOnly) double DryMass_kg = 200.0;
	UPROPERTY(EditAnywhere, BlueprintReadOnly) double MaxPayloadMass_kg = 500.0;
	UPROPERTY(EditAnywhere, BlueprintReadOnly) double SolarPanelArea_m2 = 5.0;
	UPROPERTY(EditAnywhere, BlueprintReadOnly) double PowerGeneration_W = 1000.0;
	UPROPERTY(EditAnywhere, BlueprintReadOnly) double BatteryCapacity_Wh = 2000.0;
	UPROPERTY(EditAnywhere, BlueprintReadOnly) double AntennaGain_dBi = 20.0;
	UPROPERTY(EditAnywhere, BlueprintReadOnly) double TransmitPower_W = 50.0;
	UPROPERTY(EditAnywhere, BlueprintReadOnly) double MaxDataRate_Mbps = 100.0;
	UPROPERTY(EditAnywhere, BlueprintReadOnly) double DataStorage_GB = 256.0;
	UPROPERTY(EditAnywhere, BlueprintReadOnly) double ThermalCapacity_W = 500.0; // max heat dissipation
	UPROPERTY(EditAnywhere, BlueprintReadOnly) double PropellantMass_kg = 50.0; // for station-keeping
	UPROPERTY(EditAnywhere, BlueprintReadOnly) double ADCS_PointingAccuracy_deg = 0.1;
	UPROPERTY(EditAnywhere, BlueprintReadOnly) double Cost_M = 50.0;
	UPROPERTY(EditAnywhere, BlueprintReadOnly) int32 MaxInstrumentSlots = 4;
	UPROPERTY(EditAnywhere, BlueprintReadOnly) double DesignLife_years = 5.0;
};

// ---------------------------------------------------------------------------
// Mission budget breakdown
// ---------------------------------------------------------------------------
USTRUCT(BlueprintType)
struct ASTRALISDATA_API FMissionBudget
{
	GENERATED_BODY()

	double TotalBudget_M = 300.0;     // millions USD
	double LaunchCost_M = 0.0;
	double BusCost_M = 0.0;
	double InstrumentCost_M = 0.0;
	double OperationsCost_M = 15.0;    // fixed ops cost
	double InsuranceCost_M = 0.0;      // 10% of hardware
	double ContingencyPct = 0.10;

	double GetTotalSpent() const
	{
		double Hardware = LaunchCost_M + BusCost_M + InstrumentCost_M;
		return Hardware + OperationsCost_M + Hardware * ContingencyPct + InsuranceCost_M;
	}
	double GetRemaining() const { return TotalBudget_M - GetTotalSpent(); }
	bool IsOverBudget() const { return GetTotalSpent() > TotalBudget_M; }
};

// ---------------------------------------------------------------------------
// Engine catalogue — browsable collection of engine options
// ---------------------------------------------------------------------------
UCLASS(BlueprintType)
class ASTRALISDATA_API UAstralisEngineCatalog : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Catalog")
	TArray<FEngineSpec_Vehicle> Engines;

	static TArray<FEngineSpec_Vehicle> GetDefaultCatalog();
};

// ---------------------------------------------------------------------------
// Vehicle library — collection of known launch vehicles
// ---------------------------------------------------------------------------
UCLASS(BlueprintType)
class ASTRALISDATA_API UAstralisVehicleLibrary : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Library")
	TArray<FLaunchVehicleDefinition> Vehicles;

	static TArray<FLaunchVehicleDefinition> GetDefaultLibrary();
};
