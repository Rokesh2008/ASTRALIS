#include "AstralisVehicleTypes.h"

// All specifications below are approximate, publicly available values.
// Marked as Representative — not exact manufacturer data.

TArray<FEngineSpec_Vehicle> UAstralisEngineCatalog::GetDefaultCatalog()
{
	TArray<FEngineSpec_Vehicle> Out;

	// Merlin 1D class — gas generator, LOX/RP-1
	{
		FEngineSpec_Vehicle E;
		E.Name = FName(TEXT("Merlin-1D-Class"));
		E.EngineType = EEngineType::GasGenerator;
		E.ThrustSeaLevel_N = 845000.0;
		E.ThrustVacuum_N = 914000.0;
		E.IspSeaLevel_s = 282.0;
		E.IspVacuum_s = 311.0;
		E.MassFlowRate_kgps = 845000.0 / (282.0 * 9.80665);
		E.Mass_kg = 470.0;
		E.Diameter_m = 0.92;
		E.Length_m = 2.7;
		E.PropellantType = EPropellantType::LOX_RP1;
		E.NumChambers = 1;
		E.AttachmentInterface = EAttachmentInterface::Custom;
		E.Manufacturer = TEXT("SpaceX (representative)");
		E.SpecSource = TEXT("Public press kits and NASA ELV documentation. Representative values.");
		E.SpecConfidence = 0.7;
		Out.Add(E);
	}

	// Raptor class — staged combustion, LOX/CH4
	{
		FEngineSpec_Vehicle E;
		E.Name = FName(TEXT("Raptor-Class"));
		E.EngineType = EEngineType::StagedCombustion;
		E.ThrustSeaLevel_N = 2200000.0;
		E.ThrustVacuum_N = 2500000.0;
		E.IspSeaLevel_s = 327.0;
		E.IspVacuum_s = 356.0;
		E.MassFlowRate_kgps = 2200000.0 / (327.0 * 9.80665);
		E.Mass_kg = 1600.0;
		E.Diameter_m = 1.3;
		E.Length_m = 3.1;
		E.PropellantType = EPropellantType::LOX_Methane;
		E.NumChambers = 1;
		E.AttachmentInterface = EAttachmentInterface::Custom;
		E.Manufacturer = TEXT("SpaceX (representative)");
		E.SpecSource = TEXT("Public statements and IAC presentations. Representative values.");
		E.SpecConfidence = 0.5;
		Out.Add(E);
	}

	// RS-25 class — staged combustion, LOX/LH2
	{
		FEngineSpec_Vehicle E;
		E.Name = FName(TEXT("RS-25-Class"));
		E.EngineType = EEngineType::StagedCombustion;
		E.ThrustSeaLevel_N = 1860000.0;
		E.ThrustVacuum_N = 2279000.0;
		E.IspSeaLevel_s = 366.0;
		E.IspVacuum_s = 452.0;
		E.MassFlowRate_kgps = 1860000.0 / (366.0 * 9.80665);
		E.Mass_kg = 3177.0;
		E.Diameter_m = 2.44;
		E.Length_m = 4.27;
		E.PropellantType = EPropellantType::LOX_LH2;
		E.NumChambers = 1;
		E.AttachmentInterface = EAttachmentInterface::Custom;
		E.Manufacturer = TEXT("Aerojet Rocketdyne (representative)");
		E.SpecSource = TEXT("NASA SLS fact sheets. Representative values.");
		E.SpecConfidence = 0.8;
		Out.Add(E);
	}

	// RL-10 class — expander cycle, LOX/LH2
	{
		FEngineSpec_Vehicle E;
		E.Name = FName(TEXT("RL-10-Class"));
		E.EngineType = EEngineType::ExpanderCycle;
		E.ThrustSeaLevel_N = 0.0; // vacuum-only upper stage engine
		E.ThrustVacuum_N = 110000.0;
		E.IspSeaLevel_s = 0.0;
		E.IspVacuum_s = 465.0;
		E.MassFlowRate_kgps = 110000.0 / (465.0 * 9.80665);
		E.Mass_kg = 168.0;
		E.Diameter_m = 1.17;
		E.Length_m = 2.29;
		E.PropellantType = EPropellantType::LOX_LH2;
		E.NumChambers = 1;
		E.AttachmentInterface = EAttachmentInterface::Custom;
		E.Manufacturer = TEXT("Aerojet Rocketdyne (representative)");
		E.SpecSource = TEXT("ULA and NASA documentation. Representative values.");
		E.SpecConfidence = 0.8;
		Out.Add(E);
	}

	// Vikas class — staged combustion, NTO/UDMH
	{
		FEngineSpec_Vehicle E;
		E.Name = FName(TEXT("Vikas-Class"));
		E.EngineType = EEngineType::StagedCombustion;
		E.ThrustSeaLevel_N = 725000.0;
		E.ThrustVacuum_N = 799000.0;
		E.IspSeaLevel_s = 263.0;
		E.IspVacuum_s = 293.0;
		E.MassFlowRate_kgps = 725000.0 / (263.0 * 9.80665);
		E.Mass_kg = 900.0;
		E.Diameter_m = 0.9;
		E.Length_m = 2.8;
		E.PropellantType = EPropellantType::NTO_UDMH;
		E.NumChambers = 1;
		E.AttachmentInterface = EAttachmentInterface::Custom;
		E.Manufacturer = TEXT("ISRO (representative)");
		E.SpecSource = TEXT("ISRO public documentation. Representative values.");
		E.SpecConfidence = 0.6;
		Out.Add(E);
	}

	// Vulcain class — gas generator, LOX/LH2
	{
		FEngineSpec_Vehicle E;
		E.Name = FName(TEXT("Vulcain-2-Class"));
		E.EngineType = EEngineType::GasGenerator;
		E.ThrustSeaLevel_N = 960000.0;
		E.ThrustVacuum_N = 1350000.0;
		E.IspSeaLevel_s = 318.0;
		E.IspVacuum_s = 434.0;
		E.MassFlowRate_kgps = 960000.0 / (318.0 * 9.80665);
		E.Mass_kg = 2100.0;
		E.Diameter_m = 1.76;
		E.Length_m = 3.45;
		E.PropellantType = EPropellantType::LOX_LH2;
		E.NumChambers = 1;
		E.AttachmentInterface = EAttachmentInterface::Custom;
		E.Manufacturer = TEXT("ArianeGroup (representative)");
		E.SpecSource = TEXT("ESA and Arianespace public documentation. Representative values.");
		E.SpecConfidence = 0.7;
		Out.Add(E);
	}

	// RD-180 class — staged combustion, LOX/RP-1
	{
		FEngineSpec_Vehicle E;
		E.Name = FName(TEXT("RD-180-Class"));
		E.EngineType = EEngineType::StagedCombustion;
		E.ThrustSeaLevel_N = 4152000.0;
		E.ThrustVacuum_N = 4520000.0;
		E.IspSeaLevel_s = 311.0;
		E.IspVacuum_s = 338.0;
		E.MassFlowRate_kgps = 4152000.0 / (311.0 * 9.80665);
		E.Mass_kg = 5480.0;
		E.Diameter_m = 3.15;
		E.Length_m = 3.56;
		E.PropellantType = EPropellantType::LOX_RP1;
		E.NumChambers = 2;
		E.AttachmentInterface = EAttachmentInterface::Custom;
		E.Manufacturer = TEXT("NPO Energomash (representative)");
		E.SpecSource = TEXT("Atlas V documentation and public literature. Representative values.");
		E.SpecConfidence = 0.7;
		Out.Add(E);
	}

	// BE-4 class — staged combustion, LOX/CH4 (Blue Origin)
	{
		FEngineSpec_Vehicle E;
		E.Name = FName(TEXT("BE-4-Class"));
		E.EngineType = EEngineType::StagedCombustion;
		E.ThrustSeaLevel_N = 2400000.0;
		E.ThrustVacuum_N = 2710000.0;
		E.IspSeaLevel_s = 310.0;
		E.IspVacuum_s = 340.0;
		E.MassFlowRate_kgps = 2400000.0 / (310.0 * 9.80665);
		E.Mass_kg = 2200.0;
		E.Diameter_m = 2.0;
		E.Length_m = 3.6;
		E.PropellantType = EPropellantType::LOX_Methane;
		E.NumChambers = 1;
		E.AttachmentInterface = EAttachmentInterface::Custom;
		E.Manufacturer = TEXT("Blue Origin (representative)");
		E.SpecSource = TEXT("Blue Origin public materials. Representative values.");
		E.SpecConfidence = 0.5;
		Out.Add(E);
	}

	// RD-191 class — ox-rich staged combustion (Energomash)
	{
		FEngineSpec_Vehicle E;
		E.Name = FName(TEXT("RD-191-Class"));
		E.EngineType = EEngineType::StagedCombustion;
		E.ThrustSeaLevel_N = 1920000.0;
		E.ThrustVacuum_N = 2090000.0;
		E.IspSeaLevel_s = 311.0;
		E.IspVacuum_s = 337.5;
		E.MassFlowRate_kgps = 1920000.0 / (311.0 * 9.80665);
		E.Mass_kg = 2290.0;
		E.Diameter_m = 2.1;
		E.Length_m = 3.78;
		E.PropellantType = EPropellantType::LOX_RP1;
		E.NumChambers = 1;
		E.AttachmentInterface = EAttachmentInterface::Custom;
		E.Manufacturer = TEXT("NPO Energomash (representative)");
		E.SpecSource = TEXT("NPO Energomash public data. Representative values.");
		E.SpecConfidence = 0.6;
		Out.Add(E);
	}

	// Rutherford class — electric turbopump, LOX/RP-1 (Rocket Lab)
	{
		FEngineSpec_Vehicle E;
		E.Name = FName(TEXT("Rutherford-Class"));
		E.EngineType = EEngineType::GasGenerator;
		E.ThrustSeaLevel_N = 24000.0;
		E.ThrustVacuum_N = 26000.0;
		E.IspSeaLevel_s = 303.0;
		E.IspVacuum_s = 343.0;
		E.MassFlowRate_kgps = 24000.0 / (303.0 * 9.80665);
		E.Mass_kg = 35.0;
		E.Diameter_m = 0.3;
		E.Length_m = 0.6;
		E.PropellantType = EPropellantType::LOX_RP1;
		E.NumChambers = 1;
		E.AttachmentInterface = EAttachmentInterface::Custom;
		E.Manufacturer = TEXT("Rocket Lab (representative)");
		E.SpecSource = TEXT("Rocket Lab public documentation. Representative values.");
		E.SpecConfidence = 0.6;
		Out.Add(E);
	}

	// CE-20 class — gas generator, LOX/LH2 (ISRO cryogenic upper)
	{
		FEngineSpec_Vehicle E;
		E.Name = FName(TEXT("CE-20-Class"));
		E.EngineType = EEngineType::GasGenerator;
		E.ThrustSeaLevel_N = 0.0;
		E.ThrustVacuum_N = 200000.0;
		E.IspSeaLevel_s = 0.0;
		E.IspVacuum_s = 443.0;
		E.MassFlowRate_kgps = 200000.0 / (443.0 * 9.80665);
		E.Mass_kg = 590.0;
		E.Diameter_m = 2.0;
		E.Length_m = 2.45;
		E.PropellantType = EPropellantType::LOX_LH2;
		E.NumChambers = 1;
		E.AttachmentInterface = EAttachmentInterface::Custom;
		E.Manufacturer = TEXT("ISRO (representative)");
		E.SpecSource = TEXT("ISRO C25 stage documentation. Representative values.");
		E.SpecConfidence = 0.55;
		Out.Add(E);
	}

	// S200 solid motor class
	{
		FEngineSpec_Vehicle E;
		E.Name = FName(TEXT("S200-Class"));
		E.EngineType = EEngineType::SolidMotor;
		E.ThrustSeaLevel_N = 5150000.0;
		E.ThrustVacuum_N = 5150000.0;
		E.IspSeaLevel_s = 227.0;
		E.IspVacuum_s = 252.0;
		E.MassFlowRate_kgps = 5150000.0 / (227.0 * 9.80665);
		E.Mass_kg = 31000.0;
		E.Diameter_m = 3.2;
		E.Length_m = 25.0;
		E.PropellantType = EPropellantType::SolidHTPB;
		E.NumChambers = 1;
		E.AttachmentInterface = EAttachmentInterface::Custom;
		E.Manufacturer = TEXT("ISRO (representative)");
		E.SpecSource = TEXT("ISRO LVM3 public documentation. Representative values.");
		E.SpecConfidence = 0.6;
		Out.Add(E);
	}

	return Out;
}

TArray<FLaunchVehicleDefinition> UAstralisVehicleLibrary::GetDefaultLibrary()
{
	TArray<FLaunchVehicleDefinition> Out;

	// Falcon 9-class two-stage launch vehicle (representative)
	FLaunchVehicleDefinition F9;
	F9.VehicleID = FName(TEXT("Falcon9-Class"));
	F9.DisplayName = FText::FromString(TEXT("Falcon 9 Class"));
	F9.Country = TEXT("United States");
	F9.Manufacturer = TEXT("SpaceX (representative)");
	F9.Status = EVehicleStatus::Operational;
	F9.Description = FText::FromString(
		TEXT("Two-stage medium-lift launch vehicle with a reusable first stage. "
		     "Representative specifications based on publicly available data."));
	F9.PayloadToLEO_kg = 22800.0;
	F9.PayloadToGTO_kg = 8300.0;
	F9.FirstFlight = TEXT("2010-06-04");
	F9.SpecSource = TEXT("SpaceX public data, NASA press kits, FAA documentation. All values approximate.");
	F9.DataCompleteness = 0.7;
	F9.bIsModified = false;

	// Stage 1 — nine Merlin 1D engines
	{
		FStageDefinition S1;
		S1.StageName = FText::FromString(TEXT("First Stage"));
		S1.StageNumber = 1;
		S1.DryMass_kg = 25600.0;
		S1.PropellantCapacity_kg = 395700.0;
		S1.PropellantType = EPropellantType::LOX_RP1;
		S1.Diameter_m = 3.7;
		S1.Length_m = 42.6;
		S1.NumEngines = 9;
		S1.UpperInterface = EAttachmentInterface::Standard1194mm;
		S1.LowerInterface = EAttachmentInterface::Custom;
		S1.bHasRecoverySystem = true;

		FEngineSpec_Vehicle& E = S1.EngineSpec;
		E.Name = FName(TEXT("Merlin-1D-Class"));
		E.EngineType = EEngineType::GasGenerator;
		E.ThrustSeaLevel_N = 845000.0;
		E.ThrustVacuum_N = 914000.0;
		E.IspSeaLevel_s = 282.0;
		E.IspVacuum_s = 311.0;
		E.MassFlowRate_kgps = 845000.0 / (282.0 * 9.80665);
		E.Mass_kg = 470.0;
		E.Diameter_m = 0.92;
		E.Length_m = 2.7;
		E.PropellantType = EPropellantType::LOX_RP1;
		E.NumChambers = 1;
		E.AttachmentInterface = EAttachmentInterface::Custom;
		E.Manufacturer = TEXT("SpaceX (representative)");
		E.SpecSource = TEXT("Public press kits. Representative values.");
		E.SpecConfidence = 0.7;

		F9.Stages.Add(S1);
	}

	// Stage 2 — single Merlin Vacuum engine
	{
		FStageDefinition S2;
		S2.StageName = FText::FromString(TEXT("Second Stage"));
		S2.StageNumber = 2;
		S2.DryMass_kg = 3900.0;
		S2.PropellantCapacity_kg = 92670.0;
		S2.PropellantType = EPropellantType::LOX_RP1;
		S2.Diameter_m = 3.7;
		S2.Length_m = 13.1;
		S2.NumEngines = 1;
		S2.UpperInterface = EAttachmentInterface::Standard937mm;
		S2.LowerInterface = EAttachmentInterface::Standard1194mm;
		S2.bHasRecoverySystem = false;

		FEngineSpec_Vehicle& E = S2.EngineSpec;
		E.Name = FName(TEXT("Merlin-1D-Vac-Class"));
		E.EngineType = EEngineType::GasGenerator;
		E.ThrustSeaLevel_N = 0.0;
		E.ThrustVacuum_N = 981000.0;
		E.IspSeaLevel_s = 0.0;
		E.IspVacuum_s = 348.0;
		E.MassFlowRate_kgps = 981000.0 / (348.0 * 9.80665);
		E.Mass_kg = 490.0;
		E.Diameter_m = 2.5;
		E.Length_m = 3.0;
		E.PropellantType = EPropellantType::LOX_RP1;
		E.NumChambers = 1;
		E.AttachmentInterface = EAttachmentInterface::Custom;
		E.Manufacturer = TEXT("SpaceX (representative)");
		E.SpecSource = TEXT("Public press kits. Representative values.");
		E.SpecConfidence = 0.7;

		F9.Stages.Add(S2);
	}

	Out.Add(F9);

	// -----------------------------------------------------------------------
	// Atlas V 551 — RD-180 core + 5 AJ-60A SRBs / Centaur RL-10C upper stage
	// -----------------------------------------------------------------------
	{
		FLaunchVehicleDefinition AV;
		AV.VehicleID = FName(TEXT("AtlasV-551-Class"));
		AV.DisplayName = FText::FromString(TEXT("Atlas V 551 Class"));
		AV.Country = TEXT("United States");
		AV.Manufacturer = TEXT("United Launch Alliance (representative)");
		AV.Status = EVehicleStatus::Operational;
		AV.Description = FText::FromString(
			TEXT("Medium-to-heavy lift vehicle with RD-180 core stage, five solid rocket boosters, "
			     "and a Centaur upper stage. Representative specifications."));
		AV.PayloadToLEO_kg = 18850.0;
		AV.PayloadToGTO_kg = 8900.0;
		AV.FirstFlight = TEXT("2006-01-19");
		AV.SpecSource = TEXT("ULA Atlas V User's Guide, NASA LSP documentation. All values approximate.");
		AV.DataCompleteness = 0.7;
		AV.bIsModified = false;

		// Stage 1 — RD-180 core + 5x AJ-60A solid boosters combined
		// RD-180 SL thrust: 4,152 kN; 5x AJ-60A: 5 * 1,270 kN = 6,350 kN; total ~10,502 kN
		// Core propellant: 284,089 kg LOX/RP-1; 5x AJ-60A: 5 * 20,170 = 100,850 kg solid
		{
			FStageDefinition S1;
			S1.StageName = FText::FromString(TEXT("CCB + 5x SRB"));
			S1.StageNumber = 1;
			S1.DryMass_kg = 21054.0 + 5.0 * 5740.0;  // CCB dry + 5 empty SRB casings
			S1.PropellantCapacity_kg = 284089.0 + 5.0 * 20170.0;
			S1.PropellantType = EPropellantType::LOX_RP1;
			S1.Diameter_m = 3.81;
			S1.Length_m = 32.5;
			S1.NumEngines = 6;  // 1 RD-180 + 5 SRBs
			S1.UpperInterface = EAttachmentInterface::Standard1194mm;
			S1.LowerInterface = EAttachmentInterface::Custom;
			S1.bHasRecoverySystem = false;

			FEngineSpec_Vehicle& E = S1.EngineSpec;
			E.Name = FName(TEXT("RD-180+AJ60A-Combined"));
			E.EngineType = EEngineType::StagedCombustion;
			E.ThrustSeaLevel_N = 4152000.0 + 5.0 * 1270000.0;
			E.ThrustVacuum_N = 4520000.0 + 5.0 * 1270000.0;
			E.IspSeaLevel_s = 300.0;  // effective combined
			E.IspVacuum_s = 325.0;
			E.MassFlowRate_kgps = (4152000.0 + 5.0 * 1270000.0) / (300.0 * 9.80665);
			E.Mass_kg = 5480.0 + 5.0 * 5740.0;
			E.Diameter_m = 3.81;
			E.Length_m = 3.56;
			E.PropellantType = EPropellantType::LOX_RP1;
			E.NumChambers = 2;
			E.Manufacturer = TEXT("NPO Energomash / Aerojet (representative)");
			E.SpecSource = TEXT("ULA documentation, combined effective values.");
			E.SpecConfidence = 0.65;

			AV.Stages.Add(S1);
		}

		// Stage 2 — Centaur with single RL-10C
		{
			FStageDefinition S2;
			S2.StageName = FText::FromString(TEXT("Centaur"));
			S2.StageNumber = 2;
			S2.DryMass_kg = 2243.0;
			S2.PropellantCapacity_kg = 20830.0;
			S2.PropellantType = EPropellantType::LOX_LH2;
			S2.Diameter_m = 3.05;
			S2.Length_m = 12.68;
			S2.NumEngines = 1;
			S2.UpperInterface = EAttachmentInterface::Standard937mm;
			S2.LowerInterface = EAttachmentInterface::Standard1194mm;
			S2.bHasRecoverySystem = false;

			FEngineSpec_Vehicle& E = S2.EngineSpec;
			E.Name = FName(TEXT("RL-10C-Class"));
			E.EngineType = EEngineType::ExpanderCycle;
			E.ThrustSeaLevel_N = 0.0;
			E.ThrustVacuum_N = 106300.0;
			E.IspSeaLevel_s = 0.0;
			E.IspVacuum_s = 450.0;
			E.MassFlowRate_kgps = 106300.0 / (450.0 * 9.80665);
			E.Mass_kg = 190.0;
			E.Diameter_m = 1.17;
			E.Length_m = 2.29;
			E.PropellantType = EPropellantType::LOX_LH2;
			E.NumChambers = 1;
			E.Manufacturer = TEXT("Aerojet Rocketdyne (representative)");
			E.SpecSource = TEXT("ULA Centaur documentation. Representative values.");
			E.SpecConfidence = 0.75;

			AV.Stages.Add(S2);
		}

		Out.Add(AV);
	}

	// -----------------------------------------------------------------------
	// Delta IV Heavy — 3x RS-68A CBC cores / RL-10B-2 DCSS upper stage
	// -----------------------------------------------------------------------
	{
		FLaunchVehicleDefinition D4H;
		D4H.VehicleID = FName(TEXT("DeltaIVHeavy-Class"));
		D4H.DisplayName = FText::FromString(TEXT("Delta IV Heavy Class"));
		D4H.Country = TEXT("United States");
		D4H.Manufacturer = TEXT("United Launch Alliance (representative)");
		D4H.Status = EVehicleStatus::Retired;
		D4H.Description = FText::FromString(
			TEXT("Heavy-lift vehicle with three Common Booster Cores each powered by an RS-68A engine, "
			     "and a cryogenic upper stage. Representative specifications."));
		D4H.PayloadToLEO_kg = 28790.0;
		D4H.PayloadToGTO_kg = 14220.0;
		D4H.FirstFlight = TEXT("2004-12-21");
		D4H.SpecSource = TEXT("ULA Delta IV User's Guide, NASA documentation. All values approximate.");
		D4H.DataCompleteness = 0.75;
		D4H.bIsModified = false;

		// Stage 1 — 3x CBC with RS-68A combined
		// Each CBC: 226,400 kg LOX/LH2, dry mass ~26,760 kg, RS-68A: 3,137 kN SL / 3,560 kN vac
		{
			FStageDefinition S1;
			S1.StageName = FText::FromString(TEXT("3x CBC Core"));
			S1.StageNumber = 1;
			S1.DryMass_kg = 3.0 * 26760.0;
			S1.PropellantCapacity_kg = 3.0 * 226400.0;
			S1.PropellantType = EPropellantType::LOX_LH2;
			S1.Diameter_m = 5.0;
			S1.Length_m = 40.8;
			S1.NumEngines = 3;
			S1.UpperInterface = EAttachmentInterface::Standard1194mm;
			S1.LowerInterface = EAttachmentInterface::Custom;
			S1.bHasRecoverySystem = false;

			FEngineSpec_Vehicle& E = S1.EngineSpec;
			E.Name = FName(TEXT("RS-68A-3x-Combined"));
			E.EngineType = EEngineType::GasGenerator;
			E.ThrustSeaLevel_N = 3.0 * 3137000.0;
			E.ThrustVacuum_N = 3.0 * 3560000.0;
			E.IspSeaLevel_s = 360.0;
			E.IspVacuum_s = 412.0;
			E.MassFlowRate_kgps = 3.0 * 3137000.0 / (360.0 * 9.80665);
			E.Mass_kg = 3.0 * 6597.0;
			E.Diameter_m = 5.0;
			E.Length_m = 5.18;
			E.PropellantType = EPropellantType::LOX_LH2;
			E.NumChambers = 3;
			E.Manufacturer = TEXT("Aerojet Rocketdyne (representative)");
			E.SpecSource = TEXT("ULA Delta IV documentation. Representative values.");
			E.SpecConfidence = 0.7;

			D4H.Stages.Add(S1);
		}

		// Stage 2 — DCSS with RL-10B-2
		{
			FStageDefinition S2;
			S2.StageName = FText::FromString(TEXT("DCSS Upper Stage"));
			S2.StageNumber = 2;
			S2.DryMass_kg = 2850.0;
			S2.PropellantCapacity_kg = 27220.0;
			S2.PropellantType = EPropellantType::LOX_LH2;
			S2.Diameter_m = 5.0;
			S2.Length_m = 13.7;
			S2.NumEngines = 1;
			S2.UpperInterface = EAttachmentInterface::Standard937mm;
			S2.LowerInterface = EAttachmentInterface::Standard1194mm;
			S2.bHasRecoverySystem = false;

			FEngineSpec_Vehicle& E = S2.EngineSpec;
			E.Name = FName(TEXT("RL-10B-2-Class"));
			E.EngineType = EEngineType::ExpanderCycle;
			E.ThrustSeaLevel_N = 0.0;
			E.ThrustVacuum_N = 110000.0;
			E.IspSeaLevel_s = 0.0;
			E.IspVacuum_s = 462.0;
			E.MassFlowRate_kgps = 110000.0 / (462.0 * 9.80665);
			E.Mass_kg = 277.0;
			E.Diameter_m = 2.15;
			E.Length_m = 4.14;
			E.PropellantType = EPropellantType::LOX_LH2;
			E.NumChambers = 1;
			E.Manufacturer = TEXT("Aerojet Rocketdyne (representative)");
			E.SpecSource = TEXT("ULA Delta IV documentation. Representative values.");
			E.SpecConfidence = 0.75;

			D4H.Stages.Add(S2);
		}

		Out.Add(D4H);
	}

	// -----------------------------------------------------------------------
	// SLS Block 1 — 4x RS-25 core + 2x 5-segment SRB / ICPS upper stage
	// -----------------------------------------------------------------------
	{
		FLaunchVehicleDefinition SLS;
		SLS.VehicleID = FName(TEXT("SLS-Block1-Class"));
		SLS.DisplayName = FText::FromString(TEXT("SLS Block 1 Class"));
		SLS.Country = TEXT("United States");
		SLS.Manufacturer = TEXT("Boeing / NASA (representative)");
		SLS.Status = EVehicleStatus::Operational;
		SLS.Description = FText::FromString(
			TEXT("NASA super-heavy lift vehicle for the Artemis program. Core stage with four RS-25 engines "
			     "and two five-segment solid rocket boosters. Representative specifications."));
		SLS.PayloadToLEO_kg = 95000.0;
		SLS.PayloadToGTO_kg = 0.0;  // designed for TLI, not GTO
		SLS.FirstFlight = TEXT("2022-11-16");
		SLS.SpecSource = TEXT("NASA SLS fact sheets, Artemis I press kits. All values approximate.");
		SLS.DataCompleteness = 0.8;
		SLS.bIsModified = false;

		// Stage 1 — Core stage (4x RS-25) + 2x 5-segment SRB combined
		// Core: 4x RS-25 = 4 * 1,860 kN SL / 4 * 2,279 kN vac; core prop: 987,000 kg
		// 2x SRB: 2 * 16,000 kN = 32,000 kN; 2 * 631,000 kg propellant
		{
			FStageDefinition S1;
			S1.StageName = FText::FromString(TEXT("Core + 2x SRB"));
			S1.StageNumber = 1;
			S1.DryMass_kg = 85270.0 + 2.0 * 90718.0;  // core dry + 2 empty SRB casings
			S1.PropellantCapacity_kg = 987000.0 + 2.0 * 631000.0;
			S1.PropellantType = EPropellantType::LOX_LH2;
			S1.Diameter_m = 8.4;
			S1.Length_m = 64.6;
			S1.NumEngines = 6;  // 4 RS-25 + 2 SRBs
			S1.UpperInterface = EAttachmentInterface::Custom;
			S1.LowerInterface = EAttachmentInterface::Custom;
			S1.bHasRecoverySystem = false;

			FEngineSpec_Vehicle& E = S1.EngineSpec;
			E.Name = FName(TEXT("RS25+SRB-Combined"));
			E.EngineType = EEngineType::StagedCombustion;
			E.ThrustSeaLevel_N = 4.0 * 1860000.0 + 2.0 * 16000000.0;
			E.ThrustVacuum_N = 4.0 * 2279000.0 + 2.0 * 16000000.0;
			E.IspSeaLevel_s = 269.0;  // effective combined
			E.IspVacuum_s = 295.0;
			E.MassFlowRate_kgps = (4.0 * 1860000.0 + 2.0 * 16000000.0) / (269.0 * 9.80665);
			E.Mass_kg = 4.0 * 3177.0 + 2.0 * 90718.0;
			E.Diameter_m = 8.4;
			E.Length_m = 4.27;
			E.PropellantType = EPropellantType::LOX_LH2;
			E.NumChambers = 4;
			E.Manufacturer = TEXT("Aerojet Rocketdyne / Northrop Grumman (representative)");
			E.SpecSource = TEXT("NASA SLS fact sheets. Combined effective values.");
			E.SpecConfidence = 0.75;

			SLS.Stages.Add(S1);
		}

		// Stage 2 — ICPS (Interim Cryogenic Propulsion Stage) with RL-10B-2
		{
			FStageDefinition S2;
			S2.StageName = FText::FromString(TEXT("ICPS Upper Stage"));
			S2.StageNumber = 2;
			S2.DryMass_kg = 3490.0;
			S2.PropellantCapacity_kg = 27220.0;
			S2.PropellantType = EPropellantType::LOX_LH2;
			S2.Diameter_m = 5.0;
			S2.Length_m = 13.7;
			S2.NumEngines = 1;
			S2.UpperInterface = EAttachmentInterface::Custom;
			S2.LowerInterface = EAttachmentInterface::Custom;
			S2.bHasRecoverySystem = false;

			FEngineSpec_Vehicle& E = S2.EngineSpec;
			E.Name = FName(TEXT("RL-10B-2-ICPS"));
			E.EngineType = EEngineType::ExpanderCycle;
			E.ThrustSeaLevel_N = 0.0;
			E.ThrustVacuum_N = 110000.0;
			E.IspSeaLevel_s = 0.0;
			E.IspVacuum_s = 462.0;
			E.MassFlowRate_kgps = 110000.0 / (462.0 * 9.80665);
			E.Mass_kg = 277.0;
			E.Diameter_m = 2.15;
			E.Length_m = 4.14;
			E.PropellantType = EPropellantType::LOX_LH2;
			E.NumChambers = 1;
			E.Manufacturer = TEXT("Aerojet Rocketdyne (representative)");
			E.SpecSource = TEXT("NASA ICPS documentation. Representative values.");
			E.SpecConfidence = 0.75;

			SLS.Stages.Add(S2);
		}

		Out.Add(SLS);
	}

	// -----------------------------------------------------------------------
	// Ariane 5 ECA — Vulcain 2 core + 2x EAP P241 SRB / HM7B upper stage
	// -----------------------------------------------------------------------
	{
		FLaunchVehicleDefinition A5;
		A5.VehicleID = FName(TEXT("Ariane5-ECA-Class"));
		A5.DisplayName = FText::FromString(TEXT("Ariane 5 ECA Class"));
		A5.Country = TEXT("Europe");
		A5.Manufacturer = TEXT("ArianeGroup (representative)");
		A5.Status = EVehicleStatus::Retired;
		A5.Description = FText::FromString(
			TEXT("European heavy-lift vehicle with cryogenic core stage, two solid rocket boosters, "
			     "and a cryogenic upper stage. Workhorse of European space launch for two decades. "
			     "Representative specifications."));
		A5.PayloadToLEO_kg = 21000.0;
		A5.PayloadToGTO_kg = 10865.0;
		A5.FirstFlight = TEXT("2002-12-11");
		A5.SpecSource = TEXT("Arianespace User's Manual, ESA documentation. All values approximate.");
		A5.DataCompleteness = 0.7;
		A5.bIsModified = false;

		// Stage 1 — EPC core (Vulcain 2) + 2x EAP P241 solid boosters combined
		// Vulcain 2: 960 kN SL / 1,350 kN vac; core prop: 175,000 kg LOX/LH2
		// 2x EAP: 2 * 7,080 kN = 14,160 kN; 2 * 237,000 kg solid propellant
		{
			FStageDefinition S1;
			S1.StageName = FText::FromString(TEXT("EPC Core + 2x EAP"));
			S1.StageNumber = 1;
			S1.DryMass_kg = 14700.0 + 2.0 * 39300.0;  // EPC dry + 2 empty EAP casings
			S1.PropellantCapacity_kg = 175000.0 + 2.0 * 237000.0;
			S1.PropellantType = EPropellantType::LOX_LH2;
			S1.Diameter_m = 5.4;
			S1.Length_m = 30.5;
			S1.NumEngines = 3;  // 1 Vulcain 2 + 2 EAP
			S1.UpperInterface = EAttachmentInterface::Custom;
			S1.LowerInterface = EAttachmentInterface::Custom;
			S1.bHasRecoverySystem = false;

			FEngineSpec_Vehicle& E = S1.EngineSpec;
			E.Name = FName(TEXT("Vulcain2+EAP-Combined"));
			E.EngineType = EEngineType::GasGenerator;
			E.ThrustSeaLevel_N = 960000.0 + 2.0 * 7080000.0;
			E.ThrustVacuum_N = 1350000.0 + 2.0 * 7080000.0;
			E.IspSeaLevel_s = 263.0;  // effective combined
			E.IspVacuum_s = 285.0;
			E.MassFlowRate_kgps = (960000.0 + 2.0 * 7080000.0) / (263.0 * 9.80665);
			E.Mass_kg = 2100.0 + 2.0 * 39300.0;
			E.Diameter_m = 5.4;
			E.Length_m = 3.45;
			E.PropellantType = EPropellantType::LOX_LH2;
			E.NumChambers = 1;
			E.Manufacturer = TEXT("ArianeGroup (representative)");
			E.SpecSource = TEXT("ESA/Arianespace documentation. Combined effective values.");
			E.SpecConfidence = 0.7;

			A5.Stages.Add(S1);
		}

		// Stage 2 — ESC-A with HM7B engine
		{
			FStageDefinition S2;
			S2.StageName = FText::FromString(TEXT("ESC-A Upper Stage"));
			S2.StageNumber = 2;
			S2.DryMass_kg = 4540.0;
			S2.PropellantCapacity_kg = 14900.0;
			S2.PropellantType = EPropellantType::LOX_LH2;
			S2.Diameter_m = 5.4;
			S2.Length_m = 4.7;
			S2.NumEngines = 1;
			S2.UpperInterface = EAttachmentInterface::Custom;
			S2.LowerInterface = EAttachmentInterface::Custom;
			S2.bHasRecoverySystem = false;

			FEngineSpec_Vehicle& E = S2.EngineSpec;
			E.Name = FName(TEXT("HM7B-Class"));
			E.EngineType = EEngineType::GasGenerator;
			E.ThrustSeaLevel_N = 0.0;
			E.ThrustVacuum_N = 64800.0;
			E.IspSeaLevel_s = 0.0;
			E.IspVacuum_s = 446.0;
			E.MassFlowRate_kgps = 64800.0 / (446.0 * 9.80665);
			E.Mass_kg = 165.0;
			E.Diameter_m = 0.99;
			E.Length_m = 1.87;
			E.PropellantType = EPropellantType::LOX_LH2;
			E.NumChambers = 1;
			E.Manufacturer = TEXT("ArianeGroup (representative)");
			E.SpecSource = TEXT("ESA documentation. Representative values.");
			E.SpecConfidence = 0.7;

			A5.Stages.Add(S2);
		}

		Out.Add(A5);
	}

	// -----------------------------------------------------------------------
	// GSLV Mk III / LVM3 — 2x S200 solid + L110 Vikas core / C25 CE-20 upper
	// -----------------------------------------------------------------------
	{
		FLaunchVehicleDefinition LVM3;
		LVM3.VehicleID = FName(TEXT("LVM3-Class"));
		LVM3.DisplayName = FText::FromString(TEXT("LVM3 / GSLV Mk III Class"));
		LVM3.Country = TEXT("India");
		LVM3.Manufacturer = TEXT("ISRO (representative)");
		LVM3.Status = EVehicleStatus::Operational;
		LVM3.Description = FText::FromString(
			TEXT("India's heaviest operational launch vehicle with two S200 solid strap-on boosters, "
			     "a liquid core stage with twin Vikas engines, and a cryogenic upper stage with CE-20. "
			     "Representative specifications."));
		LVM3.PayloadToLEO_kg = 10000.0;
		LVM3.PayloadToGTO_kg = 4000.0;
		LVM3.FirstFlight = TEXT("2017-06-05");
		LVM3.SpecSource = TEXT("ISRO LVM3 User's Guide, public documentation. All values approximate.");
		LVM3.DataCompleteness = 0.6;
		LVM3.bIsModified = false;

		// Stage 1 — 2x S200 solid + L110 liquid core combined
		// 2x S200: 2 * 5,150 kN = 10,300 kN; 2 * 205,000 kg solid propellant
		// L110: 2x Vikas = 2 * 725 kN = 1,450 kN SL; 110,000 kg NTO/UDMH
		{
			FStageDefinition S1;
			S1.StageName = FText::FromString(TEXT("2x S200 + L110 Core"));
			S1.StageNumber = 1;
			S1.DryMass_kg = 2.0 * 31000.0 + 8500.0;  // 2 S200 casings + L110 dry
			S1.PropellantCapacity_kg = 2.0 * 205000.0 + 110000.0;
			S1.PropellantType = EPropellantType::SolidHTPB;
			S1.Diameter_m = 4.0;
			S1.Length_m = 25.3;
			S1.NumEngines = 4;  // 2 S200 + 2 Vikas
			S1.UpperInterface = EAttachmentInterface::Custom;
			S1.LowerInterface = EAttachmentInterface::Custom;
			S1.bHasRecoverySystem = false;

			FEngineSpec_Vehicle& E = S1.EngineSpec;
			E.Name = FName(TEXT("S200+Vikas-Combined"));
			E.EngineType = EEngineType::SolidMotor;
			E.ThrustSeaLevel_N = 2.0 * 5150000.0 + 2.0 * 725000.0;
			E.ThrustVacuum_N = 2.0 * 5150000.0 + 2.0 * 799000.0;
			E.IspSeaLevel_s = 237.0;  // effective combined
			E.IspVacuum_s = 255.0;
			E.MassFlowRate_kgps = (2.0 * 5150000.0 + 2.0 * 725000.0) / (237.0 * 9.80665);
			E.Mass_kg = 2.0 * 31000.0 + 2.0 * 900.0;
			E.Diameter_m = 3.2;
			E.Length_m = 25.0;
			E.PropellantType = EPropellantType::SolidHTPB;
			E.NumChambers = 2;
			E.Manufacturer = TEXT("ISRO (representative)");
			E.SpecSource = TEXT("ISRO LVM3 documentation. Combined effective values.");
			E.SpecConfidence = 0.6;

			LVM3.Stages.Add(S1);
		}

		// Stage 2 — C25 cryogenic upper stage with CE-20 engine
		{
			FStageDefinition S2;
			S2.StageName = FText::FromString(TEXT("C25 Cryogenic Upper"));
			S2.StageNumber = 2;
			S2.DryMass_kg = 3400.0;
			S2.PropellantCapacity_kg = 28000.0;
			S2.PropellantType = EPropellantType::LOX_LH2;
			S2.Diameter_m = 4.0;
			S2.Length_m = 8.0;
			S2.NumEngines = 1;
			S2.UpperInterface = EAttachmentInterface::Custom;
			S2.LowerInterface = EAttachmentInterface::Custom;
			S2.bHasRecoverySystem = false;

			FEngineSpec_Vehicle& E = S2.EngineSpec;
			E.Name = FName(TEXT("CE-20-Class"));
			E.EngineType = EEngineType::GasGenerator;
			E.ThrustSeaLevel_N = 0.0;
			E.ThrustVacuum_N = 200000.0;
			E.IspSeaLevel_s = 0.0;
			E.IspVacuum_s = 443.0;
			E.MassFlowRate_kgps = 200000.0 / (443.0 * 9.80665);
			E.Mass_kg = 590.0;
			E.Diameter_m = 2.0;
			E.Length_m = 2.1;
			E.PropellantType = EPropellantType::LOX_LH2;
			E.NumChambers = 1;
			E.Manufacturer = TEXT("ISRO (representative)");
			E.SpecSource = TEXT("ISRO public documentation. Representative values.");
			E.SpecConfidence = 0.6;

			LVM3.Stages.Add(S2);
		}

		Out.Add(LVM3);
	}

	// -----------------------------------------------------------------------
	// Soyuz-2.1b — RD-107A/108A core+boosters S1 / RD-0124 S2 (Roscosmos)
	// -----------------------------------------------------------------------
	{
		FLaunchVehicleDefinition Soyuz;
		Soyuz.VehicleID = FName(TEXT("Soyuz-21b-Class"));
		Soyuz.DisplayName = FText::FromString(TEXT("Soyuz-2.1b Class"));
		Soyuz.Country = TEXT("Russia");
		Soyuz.Manufacturer = TEXT("Progress Rocket Space Centre (representative)");
		Soyuz.Status = EVehicleStatus::Operational;
		Soyuz.Description = FText::FromString(
			TEXT("Venerable medium-lift vehicle derived from the R-7 family. Four strap-on boosters "
			     "and a central core with RD-107A/108A engines, plus an RD-0124 upper stage. "
			     "Representative specifications."));
		Soyuz.PayloadToLEO_kg = 7020.0;
		Soyuz.PayloadToGTO_kg = 3250.0;
		Soyuz.FirstFlight = TEXT("2006-12-27");
		Soyuz.SpecSource = TEXT("Roscosmos/Starsem Soyuz User's Manual. All values approximate.");
		Soyuz.DataCompleteness = 0.65;
		Soyuz.bIsModified = false;

		{
			FStageDefinition S1;
			S1.StageName = FText::FromString(TEXT("4x Boosters + Core"));
			S1.StageNumber = 1;
			S1.DryMass_kg = 4.0 * 3784.0 + 6545.0;
			S1.PropellantCapacity_kg = 4.0 * 39160.0 + 90200.0;
			S1.PropellantType = EPropellantType::LOX_RP1;
			S1.Diameter_m = 2.95;
			S1.Length_m = 44.0;
			S1.NumEngines = 5;
			S1.UpperInterface = EAttachmentInterface::Custom;
			S1.LowerInterface = EAttachmentInterface::Custom;
			S1.bHasRecoverySystem = false;

			FEngineSpec_Vehicle& E = S1.EngineSpec;
			E.Name = FName(TEXT("RD-107A+108A-Combined"));
			E.EngineType = EEngineType::StagedCombustion;
			E.ThrustSeaLevel_N = 4.0 * 838500.0 + 792500.0;
			E.ThrustVacuum_N = 4.0 * 1021000.0 + 990200.0;
			E.IspSeaLevel_s = 263.0;
			E.IspVacuum_s = 320.0;
			E.MassFlowRate_kgps = (4.0 * 838500.0 + 792500.0) / (263.0 * 9.80665);
			E.Mass_kg = 4.0 * 1190.0 + 1250.0;
			E.Diameter_m = 2.95;
			E.Length_m = 2.9;
			E.PropellantType = EPropellantType::LOX_RP1;
			E.NumChambers = 5;
			E.Manufacturer = TEXT("NPO Energomash (representative)");
			E.SpecSource = TEXT("Starsem Soyuz User's Manual. Combined effective values.");
			E.SpecConfidence = 0.65;

			Soyuz.Stages.Add(S1);
		}

		{
			FStageDefinition S2;
			S2.StageName = FText::FromString(TEXT("Block I Upper"));
			S2.StageNumber = 2;
			S2.DryMass_kg = 2355.0;
			S2.PropellantCapacity_kg = 25400.0;
			S2.PropellantType = EPropellantType::LOX_RP1;
			S2.Diameter_m = 2.66;
			S2.Length_m = 6.7;
			S2.NumEngines = 1;
			S2.UpperInterface = EAttachmentInterface::Custom;
			S2.LowerInterface = EAttachmentInterface::Custom;
			S2.bHasRecoverySystem = false;

			FEngineSpec_Vehicle& E = S2.EngineSpec;
			E.Name = FName(TEXT("RD-0124-Class"));
			E.EngineType = EEngineType::StagedCombustion;
			E.ThrustSeaLevel_N = 0.0;
			E.ThrustVacuum_N = 294300.0;
			E.IspSeaLevel_s = 0.0;
			E.IspVacuum_s = 359.0;
			E.MassFlowRate_kgps = 294300.0 / (359.0 * 9.80665);
			E.Mass_kg = 480.0;
			E.Diameter_m = 2.4;
			E.Length_m = 1.6;
			E.PropellantType = EPropellantType::LOX_RP1;
			E.NumChambers = 4;
			E.Manufacturer = TEXT("KBKhA (representative)");
			E.SpecSource = TEXT("Roscosmos public documentation. Representative values.");
			E.SpecConfidence = 0.6;

			Soyuz.Stages.Add(S2);
		}

		Out.Add(Soyuz);
	}

	// -----------------------------------------------------------------------
	// Long March 5 — YF-77+YF-100 core+boosters S1 / YF-75D S2 (CASC)
	// -----------------------------------------------------------------------
	{
		FLaunchVehicleDefinition CZ5;
		CZ5.VehicleID = FName(TEXT("LongMarch5-Class"));
		CZ5.DisplayName = FText::FromString(TEXT("Long March 5 Class"));
		CZ5.Country = TEXT("China");
		CZ5.Manufacturer = TEXT("CALT/CASC (representative)");
		CZ5.Status = EVehicleStatus::Operational;
		CZ5.Description = FText::FromString(
			TEXT("China's heavy-lift vehicle with a LOX/LH2 core stage and four LOX/RP-1 liquid strap-on "
			     "boosters. Used for space station modules and lunar missions. "
			     "Representative specifications."));
		CZ5.PayloadToLEO_kg = 25000.0;
		CZ5.PayloadToGTO_kg = 14000.0;
		CZ5.FirstFlight = TEXT("2016-11-03");
		CZ5.SpecSource = TEXT("CASC public documentation, CALT fact sheets. All values approximate.");
		CZ5.DataCompleteness = 0.55;
		CZ5.bIsModified = false;

		{
			FStageDefinition S1;
			S1.StageName = FText::FromString(TEXT("Core + 4x LRB"));
			S1.StageNumber = 1;
			S1.DryMass_kg = 21000.0 + 4.0 * 12400.0;
			S1.PropellantCapacity_kg = 158300.0 + 4.0 * 142800.0;
			S1.PropellantType = EPropellantType::LOX_RP1;
			S1.Diameter_m = 5.0;
			S1.Length_m = 57.0;
			S1.NumEngines = 10;
			S1.UpperInterface = EAttachmentInterface::Custom;
			S1.LowerInterface = EAttachmentInterface::Custom;
			S1.bHasRecoverySystem = false;

			FEngineSpec_Vehicle& E = S1.EngineSpec;
			E.Name = FName(TEXT("YF77+YF100-Combined"));
			E.EngineType = EEngineType::GasGenerator;
			E.ThrustSeaLevel_N = 2.0 * 510000.0 + 4.0 * 2.0 * 1200000.0;
			E.ThrustVacuum_N = 2.0 * 700000.0 + 4.0 * 2.0 * 1340000.0;
			E.IspSeaLevel_s = 290.0;
			E.IspVacuum_s = 315.0;
			E.MassFlowRate_kgps = (2.0 * 510000.0 + 4.0 * 2.0 * 1200000.0) / (290.0 * 9.80665);
			E.Mass_kg = 2.0 * 3000.0 + 4.0 * 2.0 * 1530.0;
			E.Diameter_m = 5.0;
			E.Length_m = 3.4;
			E.PropellantType = EPropellantType::LOX_RP1;
			E.NumChambers = 10;
			E.Manufacturer = TEXT("AALPT/CASC (representative)");
			E.SpecSource = TEXT("CASC public documentation. Combined effective values.");
			E.SpecConfidence = 0.5;

			CZ5.Stages.Add(S1);
		}

		{
			FStageDefinition S2;
			S2.StageName = FText::FromString(TEXT("Second Stage"));
			S2.StageNumber = 2;
			S2.DryMass_kg = 5200.0;
			S2.PropellantCapacity_kg = 26500.0;
			S2.PropellantType = EPropellantType::LOX_LH2;
			S2.Diameter_m = 5.0;
			S2.Length_m = 12.4;
			S2.NumEngines = 2;
			S2.UpperInterface = EAttachmentInterface::Custom;
			S2.LowerInterface = EAttachmentInterface::Custom;
			S2.bHasRecoverySystem = false;

			FEngineSpec_Vehicle& E = S2.EngineSpec;
			E.Name = FName(TEXT("YF-75D-Class"));
			E.EngineType = EEngineType::ExpanderCycle;
			E.ThrustSeaLevel_N = 0.0;
			E.ThrustVacuum_N = 88400.0;
			E.IspSeaLevel_s = 0.0;
			E.IspVacuum_s = 442.0;
			E.MassFlowRate_kgps = 88400.0 / (442.0 * 9.80665);
			E.Mass_kg = 550.0;
			E.Diameter_m = 1.65;
			E.Length_m = 2.8;
			E.PropellantType = EPropellantType::LOX_LH2;
			E.NumChambers = 1;
			E.Manufacturer = TEXT("AALPT/CASC (representative)");
			E.SpecSource = TEXT("CASC public documentation. Representative values.");
			E.SpecConfidence = 0.5;

			CZ5.Stages.Add(S2);
		}

		Out.Add(CZ5);
	}

	// -----------------------------------------------------------------------
	// H-IIA 202 — LE-7A core + 2x SRB-A3 / LE-5B upper (JAXA/MHI)
	// -----------------------------------------------------------------------
	{
		FLaunchVehicleDefinition H2A;
		H2A.VehicleID = FName(TEXT("H-IIA-202-Class"));
		H2A.DisplayName = FText::FromString(TEXT("H-IIA 202 Class"));
		H2A.Country = TEXT("Japan");
		H2A.Manufacturer = TEXT("Mitsubishi Heavy Industries (representative)");
		H2A.Status = EVehicleStatus::Operational;
		H2A.Description = FText::FromString(
			TEXT("Japan's primary medium-to-heavy lift vehicle with a LOX/LH2 core stage, "
			     "two solid rocket boosters, and a cryogenic upper stage. "
			     "Representative specifications."));
		H2A.PayloadToLEO_kg = 10000.0;
		H2A.PayloadToGTO_kg = 4100.0;
		H2A.FirstFlight = TEXT("2001-08-29");
		H2A.SpecSource = TEXT("JAXA H-IIA User's Manual. All values approximate.");
		H2A.DataCompleteness = 0.65;
		H2A.bIsModified = false;

		{
			FStageDefinition S1;
			S1.StageName = FText::FromString(TEXT("Core + 2x SRB-A3"));
			S1.StageNumber = 1;
			S1.DryMass_kg = 13600.0 + 2.0 * 10400.0;
			S1.PropellantCapacity_kg = 100700.0 + 2.0 * 65000.0;
			S1.PropellantType = EPropellantType::LOX_LH2;
			S1.Diameter_m = 4.0;
			S1.Length_m = 37.0;
			S1.NumEngines = 3;
			S1.UpperInterface = EAttachmentInterface::Custom;
			S1.LowerInterface = EAttachmentInterface::Custom;
			S1.bHasRecoverySystem = false;

			FEngineSpec_Vehicle& E = S1.EngineSpec;
			E.Name = FName(TEXT("LE7A+SRBA3-Combined"));
			E.EngineType = EEngineType::StagedCombustion;
			E.ThrustSeaLevel_N = 870000.0 + 2.0 * 2305000.0;
			E.ThrustVacuum_N = 1098000.0 + 2.0 * 2305000.0;
			E.IspSeaLevel_s = 280.0;
			E.IspVacuum_s = 300.0;
			E.MassFlowRate_kgps = (870000.0 + 2.0 * 2305000.0) / (280.0 * 9.80665);
			E.Mass_kg = 1800.0 + 2.0 * 10400.0;
			E.Diameter_m = 4.0;
			E.Length_m = 3.4;
			E.PropellantType = EPropellantType::LOX_LH2;
			E.NumChambers = 3;
			E.Manufacturer = TEXT("MHI / IHI Aerospace (representative)");
			E.SpecSource = TEXT("JAXA H-IIA User's Manual. Combined effective values.");
			E.SpecConfidence = 0.65;

			H2A.Stages.Add(S1);
		}

		{
			FStageDefinition S2;
			S2.StageName = FText::FromString(TEXT("Second Stage"));
			S2.StageNumber = 2;
			S2.DryMass_kg = 3000.0;
			S2.PropellantCapacity_kg = 16600.0;
			S2.PropellantType = EPropellantType::LOX_LH2;
			S2.Diameter_m = 4.0;
			S2.Length_m = 11.0;
			S2.NumEngines = 1;
			S2.UpperInterface = EAttachmentInterface::Custom;
			S2.LowerInterface = EAttachmentInterface::Custom;
			S2.bHasRecoverySystem = false;

			FEngineSpec_Vehicle& E = S2.EngineSpec;
			E.Name = FName(TEXT("LE-5B-Class"));
			E.EngineType = EEngineType::ExpanderCycle;
			E.ThrustSeaLevel_N = 0.0;
			E.ThrustVacuum_N = 137200.0;
			E.IspSeaLevel_s = 0.0;
			E.IspVacuum_s = 447.0;
			E.MassFlowRate_kgps = 137200.0 / (447.0 * 9.80665);
			E.Mass_kg = 269.0;
			E.Diameter_m = 1.65;
			E.Length_m = 2.7;
			E.PropellantType = EPropellantType::LOX_LH2;
			E.NumChambers = 1;
			E.Manufacturer = TEXT("MHI (representative)");
			E.SpecSource = TEXT("JAXA documentation. Representative values.");
			E.SpecConfidence = 0.65;

			H2A.Stages.Add(S2);
		}

		Out.Add(H2A);
	}

	// -----------------------------------------------------------------------
	// Electron — Rutherford SL x9 / Rutherford Vac x1 (Rocket Lab)
	// -----------------------------------------------------------------------
	{
		FLaunchVehicleDefinition Elec;
		Elec.VehicleID = FName(TEXT("Electron-Class"));
		Elec.DisplayName = FText::FromString(TEXT("Electron Class"));
		Elec.Country = TEXT("United States / New Zealand");
		Elec.Manufacturer = TEXT("Rocket Lab (representative)");
		Elec.Status = EVehicleStatus::Operational;
		Elec.Description = FText::FromString(
			TEXT("Small-lift vehicle designed for dedicated small satellite launches. "
			     "Uses electric-pump-fed Rutherford engines with 3D-printed combustion chambers. "
			     "Representative specifications."));
		Elec.PayloadToLEO_kg = 300.0;
		Elec.PayloadToGTO_kg = 0.0;
		Elec.FirstFlight = TEXT("2017-05-25");
		Elec.SpecSource = TEXT("Rocket Lab Payload User's Guide. All values approximate.");
		Elec.DataCompleteness = 0.6;
		Elec.bIsModified = false;

		{
			FStageDefinition S1;
			S1.StageName = FText::FromString(TEXT("First Stage"));
			S1.StageNumber = 1;
			S1.DryMass_kg = 950.0;
			S1.PropellantCapacity_kg = 9250.0;
			S1.PropellantType = EPropellantType::LOX_RP1;
			S1.Diameter_m = 1.2;
			S1.Length_m = 12.0;
			S1.NumEngines = 9;
			S1.UpperInterface = EAttachmentInterface::Custom;
			S1.LowerInterface = EAttachmentInterface::Custom;
			S1.bHasRecoverySystem = true;

			FEngineSpec_Vehicle& E = S1.EngineSpec;
			E.Name = FName(TEXT("Rutherford-SL"));
			E.EngineType = EEngineType::GasGenerator;
			E.ThrustSeaLevel_N = 24900.0;
			E.ThrustVacuum_N = 25800.0;
			E.IspSeaLevel_s = 303.0;
			E.IspVacuum_s = 311.0;
			E.MassFlowRate_kgps = 24900.0 / (303.0 * 9.80665);
			E.Mass_kg = 35.0;
			E.Diameter_m = 0.25;
			E.Length_m = 0.6;
			E.PropellantType = EPropellantType::LOX_RP1;
			E.NumChambers = 1;
			E.Manufacturer = TEXT("Rocket Lab (representative)");
			E.SpecSource = TEXT("Rocket Lab public documentation. Representative values.");
			E.SpecConfidence = 0.55;

			Elec.Stages.Add(S1);
		}

		{
			FStageDefinition S2;
			S2.StageName = FText::FromString(TEXT("Second Stage"));
			S2.StageNumber = 2;
			S2.DryMass_kg = 250.0;
			S2.PropellantCapacity_kg = 2050.0;
			S2.PropellantType = EPropellantType::LOX_RP1;
			S2.Diameter_m = 1.2;
			S2.Length_m = 2.3;
			S2.NumEngines = 1;
			S2.UpperInterface = EAttachmentInterface::Custom;
			S2.LowerInterface = EAttachmentInterface::Custom;
			S2.bHasRecoverySystem = false;

			FEngineSpec_Vehicle& E = S2.EngineSpec;
			E.Name = FName(TEXT("Rutherford-Vac"));
			E.EngineType = EEngineType::GasGenerator;
			E.ThrustSeaLevel_N = 0.0;
			E.ThrustVacuum_N = 25800.0;
			E.IspSeaLevel_s = 0.0;
			E.IspVacuum_s = 343.0;
			E.MassFlowRate_kgps = 25800.0 / (343.0 * 9.80665);
			E.Mass_kg = 35.0;
			E.Diameter_m = 0.5;
			E.Length_m = 0.8;
			E.PropellantType = EPropellantType::LOX_RP1;
			E.NumChambers = 1;
			E.Manufacturer = TEXT("Rocket Lab (representative)");
			E.SpecSource = TEXT("Rocket Lab public documentation. Representative values.");
			E.SpecConfidence = 0.55;

			Elec.Stages.Add(S2);
		}

		Out.Add(Elec);
	}

	// -----------------------------------------------------------------------
	// Starship / Super Heavy — 33x Raptor 2 S1 / 6x Raptor 2 S2 (SpaceX)
	// -----------------------------------------------------------------------
	{
		FLaunchVehicleDefinition SS;
		SS.VehicleID = FName(TEXT("Starship-Class"));
		SS.DisplayName = FText::FromString(TEXT("Starship / Super Heavy Class"));
		SS.Country = TEXT("United States");
		SS.Manufacturer = TEXT("SpaceX (representative)");
		SS.Status = EVehicleStatus::Developmental;
		SS.Description = FText::FromString(
			TEXT("Fully reusable super-heavy-lift vehicle. Super Heavy booster with 33 Raptor 2 engines "
			     "and Starship upper stage with 6 Raptor engines. Designed for Mars colonization. "
			     "Representative specifications."));
		SS.PayloadToLEO_kg = 150000.0;
		SS.PayloadToGTO_kg = 21000.0;
		SS.FirstFlight = TEXT("2023-04-20");
		SS.SpecSource = TEXT("SpaceX public statements, FAA documents, IAC presentations. All values approximate.");
		SS.DataCompleteness = 0.4;
		SS.bIsModified = false;

		{
			FStageDefinition S1;
			S1.StageName = FText::FromString(TEXT("Super Heavy Booster"));
			S1.StageNumber = 1;
			S1.DryMass_kg = 200000.0;
			S1.PropellantCapacity_kg = 3400000.0;
			S1.PropellantType = EPropellantType::LOX_Methane;
			S1.Diameter_m = 9.0;
			S1.Length_m = 71.0;
			S1.NumEngines = 33;
			S1.UpperInterface = EAttachmentInterface::Custom;
			S1.LowerInterface = EAttachmentInterface::Custom;
			S1.bHasRecoverySystem = true;

			FEngineSpec_Vehicle& E = S1.EngineSpec;
			E.Name = FName(TEXT("Raptor-2-SL"));
			E.EngineType = EEngineType::StagedCombustion;
			E.ThrustSeaLevel_N = 2300000.0;
			E.ThrustVacuum_N = 2550000.0;
			E.IspSeaLevel_s = 327.0;
			E.IspVacuum_s = 350.0;
			E.MassFlowRate_kgps = 2300000.0 / (327.0 * 9.80665);
			E.Mass_kg = 1600.0;
			E.Diameter_m = 1.3;
			E.Length_m = 3.1;
			E.PropellantType = EPropellantType::LOX_Methane;
			E.NumChambers = 1;
			E.Manufacturer = TEXT("SpaceX (representative)");
			E.SpecSource = TEXT("SpaceX public statements. Representative values.");
			E.SpecConfidence = 0.4;

			SS.Stages.Add(S1);
		}

		{
			FStageDefinition S2;
			S2.StageName = FText::FromString(TEXT("Starship Upper"));
			S2.StageNumber = 2;
			S2.DryMass_kg = 100000.0;
			S2.PropellantCapacity_kg = 1200000.0;
			S2.PropellantType = EPropellantType::LOX_Methane;
			S2.Diameter_m = 9.0;
			S2.Length_m = 50.0;
			S2.NumEngines = 6;
			S2.UpperInterface = EAttachmentInterface::Custom;
			S2.LowerInterface = EAttachmentInterface::Custom;
			S2.bHasRecoverySystem = true;

			FEngineSpec_Vehicle& E = S2.EngineSpec;
			E.Name = FName(TEXT("Raptor-2-Vac"));
			E.EngineType = EEngineType::StagedCombustion;
			E.ThrustSeaLevel_N = 0.0;
			E.ThrustVacuum_N = 2550000.0;
			E.IspSeaLevel_s = 0.0;
			E.IspVacuum_s = 380.0;
			E.MassFlowRate_kgps = 2550000.0 / (380.0 * 9.80665);
			E.Mass_kg = 1600.0;
			E.Diameter_m = 2.0;
			E.Length_m = 3.7;
			E.PropellantType = EPropellantType::LOX_Methane;
			E.NumChambers = 1;
			E.Manufacturer = TEXT("SpaceX (representative)");
			E.SpecSource = TEXT("SpaceX public statements. Representative values.");
			E.SpecConfidence = 0.4;

			SS.Stages.Add(S2);
		}

		Out.Add(SS);
	}

	// -----------------------------------------------------------------------
	// Proton-M — RD-276 S1 / RD-0213 S2 (Khrunichev)
	// -----------------------------------------------------------------------
	{
		FLaunchVehicleDefinition PM;
		PM.VehicleID = FName(TEXT("ProtonM-Class"));
		PM.DisplayName = FText::FromString(TEXT("Proton-M Class"));
		PM.Country = TEXT("Russia");
		PM.Manufacturer = TEXT("Khrunichev State Research and Production Space Center (representative)");
		PM.Status = EVehicleStatus::Operational;
		PM.Description = FText::FromString(
			TEXT("Heavy-lift vehicle with a hypergolic first stage using six RD-276 engines "
			     "arranged around a central oxidizer tank. Long heritage of GEO satellite launches. "
			     "Representative specifications."));
		PM.PayloadToLEO_kg = 23000.0;
		PM.PayloadToGTO_kg = 6920.0;
		PM.FirstFlight = TEXT("2001-04-07");
		PM.SpecSource = TEXT("ILS Proton Launch System Mission Planner's Guide. All values approximate.");
		PM.DataCompleteness = 0.6;
		PM.bIsModified = false;

		{
			FStageDefinition S1;
			S1.StageName = FText::FromString(TEXT("1st + 2nd Stage"));
			S1.StageNumber = 1;
			S1.DryMass_kg = 30600.0 + 11715.0;
			S1.PropellantCapacity_kg = 428300.0 + 168300.0;
			S1.PropellantType = EPropellantType::NTO_UDMH;
			S1.Diameter_m = 4.1;
			S1.Length_m = 42.0;
			S1.NumEngines = 6;
			S1.UpperInterface = EAttachmentInterface::Custom;
			S1.LowerInterface = EAttachmentInterface::Custom;
			S1.bHasRecoverySystem = false;

			FEngineSpec_Vehicle& E = S1.EngineSpec;
			E.Name = FName(TEXT("RD-276-Combined"));
			E.EngineType = EEngineType::StagedCombustion;
			E.ThrustSeaLevel_N = 6.0 * 1745000.0;
			E.ThrustVacuum_N = 6.0 * 1854000.0;
			E.IspSeaLevel_s = 285.0;
			E.IspVacuum_s = 316.0;
			E.MassFlowRate_kgps = 6.0 * 1745000.0 / (285.0 * 9.80665);
			E.Mass_kg = 6.0 * 1070.0;
			E.Diameter_m = 4.1;
			E.Length_m = 2.7;
			E.PropellantType = EPropellantType::NTO_UDMH;
			E.NumChambers = 6;
			E.Manufacturer = TEXT("NPO Energomash (representative)");
			E.SpecSource = TEXT("ILS Proton documentation. Representative values.");
			E.SpecConfidence = 0.6;

			PM.Stages.Add(S1);
		}

		{
			FStageDefinition S2;
			S2.StageName = FText::FromString(TEXT("Third Stage"));
			S2.StageNumber = 2;
			S2.DryMass_kg = 3500.0;
			S2.PropellantCapacity_kg = 46562.0;
			S2.PropellantType = EPropellantType::NTO_UDMH;
			S2.Diameter_m = 4.1;
			S2.Length_m = 14.0;
			S2.NumEngines = 1;
			S2.UpperInterface = EAttachmentInterface::Custom;
			S2.LowerInterface = EAttachmentInterface::Custom;
			S2.bHasRecoverySystem = false;

			FEngineSpec_Vehicle& E = S2.EngineSpec;
			E.Name = FName(TEXT("RD-0213-Class"));
			E.EngineType = EEngineType::StagedCombustion;
			E.ThrustSeaLevel_N = 0.0;
			E.ThrustVacuum_N = 613800.0;
			E.IspSeaLevel_s = 0.0;
			E.IspVacuum_s = 325.0;
			E.MassFlowRate_kgps = 613800.0 / (325.0 * 9.80665);
			E.Mass_kg = 550.0;
			E.Diameter_m = 1.5;
			E.Length_m = 2.3;
			E.PropellantType = EPropellantType::NTO_UDMH;
			E.NumChambers = 1;
			E.Manufacturer = TEXT("KBKhA (representative)");
			E.SpecSource = TEXT("Roscosmos public documentation. Representative values.");
			E.SpecConfidence = 0.6;

			PM.Stages.Add(S2);
		}

		Out.Add(PM);
	}

	// -----------------------------------------------------------------------
	// PSLV-XL — S139+PSOM-XL S1 / Vikas S2 (ISRO)
	// -----------------------------------------------------------------------
	{
		FLaunchVehicleDefinition PSLV;
		PSLV.VehicleID = FName(TEXT("PSLV-XL-Class"));
		PSLV.DisplayName = FText::FromString(TEXT("PSLV-XL Class"));
		PSLV.Country = TEXT("India");
		PSLV.Manufacturer = TEXT("ISRO (representative)");
		PSLV.Status = EVehicleStatus::Operational;
		PSLV.Description = FText::FromString(
			TEXT("India's workhorse medium-lift vehicle with a solid first stage, six strap-on boosters, "
			     "and alternating solid/liquid upper stages. Used extensively for Earth observation "
			     "and navigation satellite launches. Representative specifications."));
		PSLV.PayloadToLEO_kg = 3800.0;
		PSLV.PayloadToGTO_kg = 1425.0;
		PSLV.FirstFlight = TEXT("1993-09-20");
		PSLV.SpecSource = TEXT("ISRO PSLV User's Manual. All values approximate.");
		PSLV.DataCompleteness = 0.55;
		PSLV.bIsModified = false;

		{
			FStageDefinition S1;
			S1.StageName = FText::FromString(TEXT("S139 + 6x PSOM-XL"));
			S1.StageNumber = 1;
			S1.DryMass_kg = 16800.0 + 6.0 * 2380.0;
			S1.PropellantCapacity_kg = 138200.0 + 6.0 * 12200.0;
			S1.PropellantType = EPropellantType::SolidHTPB;
			S1.Diameter_m = 2.8;
			S1.Length_m = 44.0;
			S1.NumEngines = 7;
			S1.UpperInterface = EAttachmentInterface::Custom;
			S1.LowerInterface = EAttachmentInterface::Custom;
			S1.bHasRecoverySystem = false;

			FEngineSpec_Vehicle& E = S1.EngineSpec;
			E.Name = FName(TEXT("S139+PSOM-Combined"));
			E.EngineType = EEngineType::SolidMotor;
			E.ThrustSeaLevel_N = 4800000.0 + 6.0 * 719000.0;
			E.ThrustVacuum_N = 4800000.0 + 6.0 * 719000.0;
			E.IspSeaLevel_s = 237.0;
			E.IspVacuum_s = 262.0;
			E.MassFlowRate_kgps = (4800000.0 + 6.0 * 719000.0) / (237.0 * 9.80665);
			E.Mass_kg = 16800.0 + 6.0 * 2380.0;
			E.Diameter_m = 2.8;
			E.Length_m = 20.3;
			E.PropellantType = EPropellantType::SolidHTPB;
			E.NumChambers = 7;
			E.Manufacturer = TEXT("ISRO (representative)");
			E.SpecSource = TEXT("ISRO PSLV documentation. Combined effective values.");
			E.SpecConfidence = 0.55;

			PSLV.Stages.Add(S1);
		}

		{
			FStageDefinition S2;
			S2.StageName = FText::FromString(TEXT("PS2 Vikas Stage"));
			S2.StageNumber = 2;
			S2.DryMass_kg = 2910.0;
			S2.PropellantCapacity_kg = 41700.0;
			S2.PropellantType = EPropellantType::NTO_UDMH;
			S2.Diameter_m = 2.8;
			S2.Length_m = 8.0;
			S2.NumEngines = 1;
			S2.UpperInterface = EAttachmentInterface::Custom;
			S2.LowerInterface = EAttachmentInterface::Custom;
			S2.bHasRecoverySystem = false;

			FEngineSpec_Vehicle& E = S2.EngineSpec;
			E.Name = FName(TEXT("Vikas-PS2-Class"));
			E.EngineType = EEngineType::GasGenerator;
			E.ThrustSeaLevel_N = 0.0;
			E.ThrustVacuum_N = 799000.0;
			E.IspSeaLevel_s = 0.0;
			E.IspVacuum_s = 293.0;
			E.MassFlowRate_kgps = 799000.0 / (293.0 * 9.80665);
			E.Mass_kg = 900.0;
			E.Diameter_m = 0.9;
			E.Length_m = 2.8;
			E.PropellantType = EPropellantType::NTO_UDMH;
			E.NumChambers = 1;
			E.Manufacturer = TEXT("ISRO (representative)");
			E.SpecSource = TEXT("ISRO public documentation. Representative values.");
			E.SpecConfidence = 0.55;

			PSLV.Stages.Add(S2);
		}

		Out.Add(PSLV);
	}

	// -----------------------------------------------------------------------
	// Falcon Heavy — 3x Falcon 9 cores + Falcon 9 S2 (SpaceX)
	// -----------------------------------------------------------------------
	{
		FLaunchVehicleDefinition FH;
		FH.VehicleID = FName(TEXT("FalconHeavy-Class"));
		FH.DisplayName = FText::FromString(TEXT("Falcon Heavy Class"));
		FH.Country = TEXT("United States");
		FH.Manufacturer = TEXT("SpaceX (representative)");
		FH.Status = EVehicleStatus::Operational;
		FH.Description = FText::FromString(
			TEXT("Heavy-lift vehicle using three Falcon 9 first stage cores, each with nine Merlin 1D engines, "
			     "plus a standard Falcon 9 second stage. Currently the most powerful operational rocket. "
			     "Representative specifications."));
		FH.PayloadToLEO_kg = 63800.0;
		FH.PayloadToGTO_kg = 26700.0;
		FH.FirstFlight = TEXT("2018-02-06");
		FH.SpecSource = TEXT("SpaceX public data, NASA press kits. All values approximate.");
		FH.DataCompleteness = 0.7;
		FH.bIsModified = false;

		{
			FStageDefinition S1;
			S1.StageName = FText::FromString(TEXT("3x Core (27 Merlin)"));
			S1.StageNumber = 1;
			S1.DryMass_kg = 3.0 * 25600.0;
			S1.PropellantCapacity_kg = 3.0 * 395700.0;
			S1.PropellantType = EPropellantType::LOX_RP1;
			S1.Diameter_m = 3.7;
			S1.Length_m = 42.6;
			S1.NumEngines = 27;
			S1.UpperInterface = EAttachmentInterface::Standard1194mm;
			S1.LowerInterface = EAttachmentInterface::Custom;
			S1.bHasRecoverySystem = true;

			FEngineSpec_Vehicle& E = S1.EngineSpec;
			E.Name = FName(TEXT("Merlin-1D-27x"));
			E.EngineType = EEngineType::GasGenerator;
			E.ThrustSeaLevel_N = 845000.0;
			E.ThrustVacuum_N = 914000.0;
			E.IspSeaLevel_s = 282.0;
			E.IspVacuum_s = 311.0;
			E.MassFlowRate_kgps = 845000.0 / (282.0 * 9.80665);
			E.Mass_kg = 470.0;
			E.Diameter_m = 0.92;
			E.Length_m = 2.7;
			E.PropellantType = EPropellantType::LOX_RP1;
			E.NumChambers = 1;
			E.Manufacturer = TEXT("SpaceX (representative)");
			E.SpecSource = TEXT("Public press kits. Representative values.");
			E.SpecConfidence = 0.7;

			FH.Stages.Add(S1);
		}

		{
			FStageDefinition S2;
			S2.StageName = FText::FromString(TEXT("Second Stage"));
			S2.StageNumber = 2;
			S2.DryMass_kg = 3900.0;
			S2.PropellantCapacity_kg = 92670.0;
			S2.PropellantType = EPropellantType::LOX_RP1;
			S2.Diameter_m = 3.7;
			S2.Length_m = 13.1;
			S2.NumEngines = 1;
			S2.UpperInterface = EAttachmentInterface::Standard937mm;
			S2.LowerInterface = EAttachmentInterface::Standard1194mm;
			S2.bHasRecoverySystem = false;

			FEngineSpec_Vehicle& E = S2.EngineSpec;
			E.Name = FName(TEXT("Merlin-1D-Vac-Class"));
			E.EngineType = EEngineType::GasGenerator;
			E.ThrustSeaLevel_N = 0.0;
			E.ThrustVacuum_N = 981000.0;
			E.IspSeaLevel_s = 0.0;
			E.IspVacuum_s = 348.0;
			E.MassFlowRate_kgps = 981000.0 / (348.0 * 9.80665);
			E.Mass_kg = 490.0;
			E.Diameter_m = 2.5;
			E.Length_m = 3.0;
			E.PropellantType = EPropellantType::LOX_RP1;
			E.NumChambers = 1;
			E.Manufacturer = TEXT("SpaceX (representative)");
			E.SpecSource = TEXT("Public press kits. Representative values.");
			E.SpecConfidence = 0.7;

			FH.Stages.Add(S2);
		}

		Out.Add(FH);
	}

	// -----------------------------------------------------------------------
	// Vulcan Centaur VC2S — 2x BE-4 + 2x GEM-63XL / Centaur V (ULA)
	// -----------------------------------------------------------------------
	{
		FLaunchVehicleDefinition VC;
		VC.VehicleID = FName(TEXT("VulcanCentaur-Class"));
		VC.DisplayName = FText::FromString(TEXT("Vulcan Centaur VC2S Class"));
		VC.Country = TEXT("United States");
		VC.Manufacturer = TEXT("United Launch Alliance (representative)");
		VC.Status = EVehicleStatus::Operational;
		VC.Description = FText::FromString(
			TEXT("ULA's next-generation medium-to-heavy lift vehicle replacing Atlas V and Delta IV. "
			     "Two BE-4 methane engines on the first stage with optional GEM-63XL solid boosters "
			     "and a Centaur V upper stage with two RL-10C-1-1 engines. "
			     "Representative specifications."));
		VC.PayloadToLEO_kg = 17600.0;
		VC.PayloadToGTO_kg = 7700.0;
		VC.FirstFlight = TEXT("2024-01-08");
		VC.SpecSource = TEXT("ULA Vulcan User's Guide. All values approximate.");
		VC.DataCompleteness = 0.55;
		VC.bIsModified = false;

		{
			FStageDefinition S1;
			S1.StageName = FText::FromString(TEXT("Core + 2x GEM-63XL"));
			S1.StageNumber = 1;
			S1.DryMass_kg = 18000.0 + 2.0 * 5000.0;
			S1.PropellantCapacity_kg = 226000.0 + 2.0 * 20000.0;
			S1.PropellantType = EPropellantType::LOX_Methane;
			S1.Diameter_m = 5.4;
			S1.Length_m = 33.0;
			S1.NumEngines = 4;
			S1.UpperInterface = EAttachmentInterface::Custom;
			S1.LowerInterface = EAttachmentInterface::Custom;
			S1.bHasRecoverySystem = false;

			FEngineSpec_Vehicle& E = S1.EngineSpec;
			E.Name = FName(TEXT("BE4+GEM63XL-Combined"));
			E.EngineType = EEngineType::StagedCombustion;
			E.ThrustSeaLevel_N = 2.0 * 2400000.0 + 2.0 * 1600000.0;
			E.ThrustVacuum_N = 2.0 * 2710000.0 + 2.0 * 1600000.0;
			E.IspSeaLevel_s = 305.0;
			E.IspVacuum_s = 330.0;
			E.MassFlowRate_kgps = (2.0 * 2400000.0 + 2.0 * 1600000.0) / (305.0 * 9.80665);
			E.Mass_kg = 2.0 * 2200.0 + 2.0 * 5000.0;
			E.Diameter_m = 5.4;
			E.Length_m = 3.6;
			E.PropellantType = EPropellantType::LOX_Methane;
			E.NumChambers = 4;
			E.Manufacturer = TEXT("Blue Origin / Northrop Grumman (representative)");
			E.SpecSource = TEXT("ULA Vulcan documentation. Combined effective values.");
			E.SpecConfidence = 0.5;

			VC.Stages.Add(S1);
		}

		{
			FStageDefinition S2;
			S2.StageName = FText::FromString(TEXT("Centaur V"));
			S2.StageNumber = 2;
			S2.DryMass_kg = 5500.0;
			S2.PropellantCapacity_kg = 54000.0;
			S2.PropellantType = EPropellantType::LOX_LH2;
			S2.Diameter_m = 5.4;
			S2.Length_m = 13.7;
			S2.NumEngines = 2;
			S2.UpperInterface = EAttachmentInterface::Custom;
			S2.LowerInterface = EAttachmentInterface::Custom;
			S2.bHasRecoverySystem = false;

			FEngineSpec_Vehicle& E = S2.EngineSpec;
			E.Name = FName(TEXT("RL-10C-1-1-Class"));
			E.EngineType = EEngineType::ExpanderCycle;
			E.ThrustSeaLevel_N = 0.0;
			E.ThrustVacuum_N = 106300.0;
			E.IspSeaLevel_s = 0.0;
			E.IspVacuum_s = 453.0;
			E.MassFlowRate_kgps = 106300.0 / (453.0 * 9.80665);
			E.Mass_kg = 190.0;
			E.Diameter_m = 1.17;
			E.Length_m = 2.29;
			E.PropellantType = EPropellantType::LOX_LH2;
			E.NumChambers = 1;
			E.Manufacturer = TEXT("Aerojet Rocketdyne (representative)");
			E.SpecSource = TEXT("ULA Centaur V documentation. Representative values.");
			E.SpecConfidence = 0.6;

			VC.Stages.Add(S2);
		}

		Out.Add(VC);
	}

	// -----------------------------------------------------------------------
	// New Glenn — Blue Origin — 7x BE-4 / BE-3U upper stage
	// -----------------------------------------------------------------------
	{
		FLaunchVehicleDefinition NG;
		NG.VehicleID = FName(TEXT("NewGlenn-Class"));
		NG.DisplayName = FText::FromString(TEXT("New Glenn Class"));
		NG.Country = TEXT("United States");
		NG.Manufacturer = TEXT("Blue Origin (representative)");
		NG.Status = EVehicleStatus::Developmental;
		NG.Description = FText::FromString(
			TEXT("Blue Origin's heavy-lift orbital launch vehicle with a reusable first stage. "
			     "Seven BE-4 LOX/methane engines on the booster, two BE-3U LOX/LH2 engines on the upper stage. "
			     "Representative specifications."));
		NG.PayloadToLEO_kg = 45000.0;
		NG.PayloadToGTO_kg = 13000.0;
		NG.FirstFlight = TEXT("2025-10-13");
		NG.SpecSource = TEXT("Blue Origin public materials. All values approximate.");
		NG.DataCompleteness = 0.5;
		NG.bIsModified = false;

		{
			FStageDefinition S1;
			S1.StageName = FText::FromString(TEXT("Booster (7x BE-4)"));
			S1.StageNumber = 1;
			S1.DryMass_kg = 54000.0;
			S1.PropellantCapacity_kg = 550000.0;
			S1.PropellantType = EPropellantType::LOX_Methane;
			S1.Diameter_m = 7.0;
			S1.Length_m = 57.5;
			S1.NumEngines = 7;
			S1.UpperInterface = EAttachmentInterface::Custom;
			S1.LowerInterface = EAttachmentInterface::Custom;
			S1.bHasRecoverySystem = true;

			FEngineSpec_Vehicle& E = S1.EngineSpec;
			E.Name = FName(TEXT("BE-4-Cluster"));
			E.EngineType = EEngineType::StagedCombustion;
			E.ThrustSeaLevel_N = 7.0 * 2400000.0;
			E.ThrustVacuum_N = 7.0 * 2710000.0;
			E.IspSeaLevel_s = 310.0;
			E.IspVacuum_s = 340.0;
			E.MassFlowRate_kgps = (7.0 * 2400000.0) / (310.0 * 9.80665);
			E.Mass_kg = 7.0 * 2200.0;
			E.Diameter_m = 7.0;
			E.Length_m = 3.6;
			E.PropellantType = EPropellantType::LOX_Methane;
			E.NumChambers = 7;
			E.Manufacturer = TEXT("Blue Origin (representative)");
			E.SpecSource = TEXT("Blue Origin press kits. Representative values.");
			E.SpecConfidence = 0.5;

			NG.Stages.Add(S1);
		}

		{
			FStageDefinition S2;
			S2.StageName = FText::FromString(TEXT("Upper Stage (2x BE-3U)"));
			S2.StageNumber = 2;
			S2.DryMass_kg = 7000.0;
			S2.PropellantCapacity_kg = 73000.0;
			S2.PropellantType = EPropellantType::LOX_LH2;
			S2.Diameter_m = 7.0;
			S2.Length_m = 15.5;
			S2.NumEngines = 2;
			S2.UpperInterface = EAttachmentInterface::Custom;
			S2.LowerInterface = EAttachmentInterface::Custom;
			S2.bHasRecoverySystem = false;

			FEngineSpec_Vehicle& E = S2.EngineSpec;
			E.Name = FName(TEXT("BE-3U-Class"));
			E.EngineType = EEngineType::ExpanderCycle;
			E.ThrustSeaLevel_N = 0.0;
			E.ThrustVacuum_N = 2.0 * 530000.0;
			E.IspSeaLevel_s = 0.0;
			E.IspVacuum_s = 445.0;
			E.MassFlowRate_kgps = (2.0 * 530000.0) / (445.0 * 9.80665);
			E.Mass_kg = 2.0 * 600.0;
			E.Diameter_m = 2.1;
			E.Length_m = 3.2;
			E.PropellantType = EPropellantType::LOX_LH2;
			E.NumChambers = 2;
			E.Manufacturer = TEXT("Blue Origin (representative)");
			E.SpecSource = TEXT("Blue Origin public data. Representative values.");
			E.SpecConfidence = 0.45;

			NG.Stages.Add(S2);
		}

		Out.Add(NG);
	}

	// -----------------------------------------------------------------------
	// Angara A5 — Khrunichev / Roscosmos — URM-1 modules / Briz-M
	// -----------------------------------------------------------------------
	{
		FLaunchVehicleDefinition AA5;
		AA5.VehicleID = FName(TEXT("AngaraA5-Class"));
		AA5.DisplayName = FText::FromString(TEXT("Angara A5 Class"));
		AA5.Country = TEXT("Russia");
		AA5.Manufacturer = TEXT("Khrunichev / Roscosmos (representative)");
		AA5.Status = EVehicleStatus::Operational;
		AA5.Description = FText::FromString(
			TEXT("Russia's heavy-lift modular launch vehicle using five URM-1 first stage modules "
			     "each powered by an RD-191 engine, plus a KVTK/Briz-M upper stage. "
			     "Representative specifications."));
		AA5.PayloadToLEO_kg = 24500.0;
		AA5.PayloadToGTO_kg = 5400.0;
		AA5.FirstFlight = TEXT("2014-12-23");
		AA5.SpecSource = TEXT("Khrunichev public documentation. All values approximate.");
		AA5.DataCompleteness = 0.5;
		AA5.bIsModified = false;

		{
			FStageDefinition S1;
			S1.StageName = FText::FromString(TEXT("Core + 4 Boosters (5x RD-191)"));
			S1.StageNumber = 1;
			S1.DryMass_kg = 5.0 * 8300.0;
			S1.PropellantCapacity_kg = 5.0 * 132600.0;
			S1.PropellantType = EPropellantType::LOX_RP1;
			S1.Diameter_m = 8.1;
			S1.Length_m = 25.1;
			S1.NumEngines = 5;
			S1.UpperInterface = EAttachmentInterface::Custom;
			S1.LowerInterface = EAttachmentInterface::Custom;
			S1.bHasRecoverySystem = false;

			FEngineSpec_Vehicle& E = S1.EngineSpec;
			E.Name = FName(TEXT("RD-191-Cluster"));
			E.EngineType = EEngineType::StagedCombustion;
			E.ThrustSeaLevel_N = 5.0 * 1920000.0;
			E.ThrustVacuum_N = 5.0 * 2090000.0;
			E.IspSeaLevel_s = 311.0;
			E.IspVacuum_s = 337.5;
			E.MassFlowRate_kgps = (5.0 * 1920000.0) / (311.0 * 9.80665);
			E.Mass_kg = 5.0 * 2290.0;
			E.Diameter_m = 2.9;
			E.Length_m = 3.78;
			E.PropellantType = EPropellantType::LOX_RP1;
			E.NumChambers = 5;
			E.Manufacturer = TEXT("NPO Energomash (representative)");
			E.SpecSource = TEXT("NPO Energomash data. Representative values.");
			E.SpecConfidence = 0.6;

			AA5.Stages.Add(S1);
		}

		{
			FStageDefinition S2;
			S2.StageName = FText::FromString(TEXT("Briz-M Upper Stage"));
			S2.StageNumber = 2;
			S2.DryMass_kg = 2370.0;
			S2.PropellantCapacity_kg = 19800.0;
			S2.PropellantType = EPropellantType::NTO_UDMH;
			S2.Diameter_m = 4.1;
			S2.Length_m = 2.65;
			S2.NumEngines = 1;
			S2.UpperInterface = EAttachmentInterface::Custom;
			S2.LowerInterface = EAttachmentInterface::Custom;
			S2.bHasRecoverySystem = false;

			FEngineSpec_Vehicle& E = S2.EngineSpec;
			E.Name = FName(TEXT("S5.98M-Class"));
			E.EngineType = EEngineType::PressureFed;
			E.ThrustSeaLevel_N = 0.0;
			E.ThrustVacuum_N = 19620.0;
			E.IspSeaLevel_s = 0.0;
			E.IspVacuum_s = 326.0;
			E.MassFlowRate_kgps = 19620.0 / (326.0 * 9.80665);
			E.Mass_kg = 75.0;
			E.Diameter_m = 0.8;
			E.Length_m = 1.2;
			E.PropellantType = EPropellantType::NTO_UDMH;
			E.NumChambers = 1;
			E.Manufacturer = TEXT("KBKM (representative)");
			E.SpecSource = TEXT("Khrunichev Angara data. Representative values.");
			E.SpecConfidence = 0.5;

			AA5.Stages.Add(S2);
		}

		Out.Add(AA5);
	}

	return Out;
}
