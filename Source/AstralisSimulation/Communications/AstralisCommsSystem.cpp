#include "AstralisCommsSystem.h"

double FAstralisCommsSystem::FreeSpacePathLoss_dB(double Range_m, double Frequency_Hz)
{
	if (Range_m <= 0.0 || Frequency_Hz <= 0.0)
	{
		return 0.0;
	}
	const double Lambda = C_mps / Frequency_Hz;
	return 20.0 * FMath::Log2(4.0 * UE_DOUBLE_PI * Range_m / Lambda) / FMath::Log2(10.0);
}

double FAstralisCommsSystem::EIRP_dBW(double TransmitPower_W, double AntennaGain_dBi, double Losses_dB)
{
	if (TransmitPower_W <= 0.0)
	{
		return -999.0;
	}
	return 10.0 * FMath::Log2(TransmitPower_W) / FMath::Log2(10.0) + AntennaGain_dBi - Losses_dB;
}

double FAstralisCommsSystem::NoiseFloor_dBW(double SystemNoiseTemp_K, double Bandwidth_Hz)
{
	if (SystemNoiseTemp_K <= 0.0 || Bandwidth_Hz <= 0.0)
	{
		return -999.0;
	}
	return 10.0 * FMath::Log2(K_Boltzmann * SystemNoiseTemp_K * Bandwidth_Hz) / FMath::Log2(10.0);
}

FLinkBudgetResult FAstralisCommsSystem::ComputeLinkBudget(
	const FAntennaSpec& SpacecraftAntenna,
	const FGroundStationSpec& GroundStation,
	double Range_m,
	double RequiredSNR_dB)
{
	FLinkBudgetResult Result;

	Result.EIRP_dBW = EIRP_dBW(
		SpacecraftAntenna.TransmitPower_W,
		SpacecraftAntenna.Gain_dBi,
		SpacecraftAntenna.PointingLoss_dB);

	Result.FSPL_dB = FreeSpacePathLoss_dB(Range_m, SpacecraftAntenna.Frequency_Hz);

	Result.ReceivedPower_dBW = Result.EIRP_dBW - Result.FSPL_dB + GroundStation.ReceiveGain_dBi;

	// Assume bandwidth = 1 MHz for link budget evaluation
	const double Bandwidth_Hz = 1.0e6;
	Result.NoiseFloor_dBW = NoiseFloor_dBW(GroundStation.SystemNoiseTemp_K, Bandwidth_Hz);

	Result.SNR_dB = Result.ReceivedPower_dBW - Result.NoiseFloor_dBW;
	Result.LinkMargin_dB = Result.SNR_dB - RequiredSNR_dB;
	Result.bLinkClosed = Result.LinkMargin_dB >= 0.0;

	// Achievable data rate
	const double SNR_linear = FMath::Pow(10.0, Result.SNR_dB / 10.0);
	Result.AchievableDataRate_bps = AchievableDataRate(SNR_linear, Bandwidth_Hz);

	return Result;
}

double FAstralisCommsSystem::AchievableDataRate(double SNR_linear, double Bandwidth_Hz)
{
	if (SNR_linear <= 0.0 || Bandwidth_Hz <= 0.0)
	{
		return 0.0;
	}
	// Shannon capacity: C = B * log2(1 + SNR) with 50% coding efficiency
	return Bandwidth_Hz * FMath::Log2(1.0 + SNR_linear) * 0.5;
}

double FAstralisCommsSystem::Wavelength(double Frequency_Hz)
{
	if (Frequency_Hz <= 0.0)
	{
		return 0.0;
	}
	return C_mps / Frequency_Hz;
}
