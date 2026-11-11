#pragma once

#include "CoreMinimal.h"
#include "AstralisCommsSystem.generated.h"

USTRUCT(BlueprintType)
struct ASTRALISSIMULATION_API FAntennaSpec
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FName AntennaID;

	// Gain (dBi)
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	double Gain_dBi = 6.0;

	// Transmit power (Watts)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (ToolTip = "Transmit power (Watts)"))
	double TransmitPower_W = 5.0;

	// Operating frequency (Hz)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (Units = "Hertz"))
	double Frequency_Hz = 2.2e9;

	// Beamwidth (degrees)
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	double Beamwidth_deg = 60.0;

	// Pointing loss (dB)
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	double PointingLoss_dB = 1.0;

	// Mass (kg)
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	double Mass_kg = 0.5;
};

USTRUCT(BlueprintType)
struct ASTRALISSIMULATION_API FGroundStationSpec
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FName StationID;

	// Geodetic position
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	double Latitude_deg = 0.0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	double Longitude_deg = 0.0;

	// Receive antenna gain (dBi)
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	double ReceiveGain_dBi = 40.0;

	// System noise temperature (Kelvin)
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	double SystemNoiseTemp_K = 150.0;

	// Minimum elevation angle (degrees)
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	double MinElevation_deg = 5.0;
};

USTRUCT(BlueprintType)
struct ASTRALISSIMULATION_API FLinkBudgetResult
{
	GENERATED_BODY()

	double EIRP_dBW = 0.0;
	double FSPL_dB = 0.0;
	double ReceivedPower_dBW = 0.0;
	double NoiseFloor_dBW = 0.0;
	double SNR_dB = 0.0;
	double LinkMargin_dB = 0.0;
	double AchievableDataRate_bps = 0.0;
	bool bLinkClosed = false;
};

// Communications link budget calculations.
class ASTRALISSIMULATION_API FAstralisCommsSystem
{
public:
	static constexpr double C_mps = 2.998e8;       // Speed of light
	static constexpr double K_Boltzmann = 1.381e-23; // Boltzmann constant (J/K)

	// Free-space path loss in dB: 20*log10(4*pi*R/lambda)
	static double FreeSpacePathLoss_dB(double Range_m, double Frequency_Hz);

	// EIRP in dBW: 10*log10(Ptx) + Gtx - Losses
	static double EIRP_dBW(double TransmitPower_W, double AntennaGain_dBi, double Losses_dB);

	// Noise floor: 10*log10(k*T*B) in dBW
	static double NoiseFloor_dBW(double SystemNoiseTemp_K, double Bandwidth_Hz);

	// Compute full link budget
	static FLinkBudgetResult ComputeLinkBudget(
		const FAntennaSpec& SpacecraftAntenna,
		const FGroundStationSpec& GroundStation,
		double Range_m,
		double RequiredSNR_dB);

	// Achievable data rate given link conditions (Shannon-limited estimate)
	static double AchievableDataRate(double SNR_linear, double Bandwidth_Hz);

	// Wavelength from frequency
	static double Wavelength(double Frequency_Hz);
};
