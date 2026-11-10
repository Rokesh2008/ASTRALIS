#include "Misc/AutomationTest.h"
#include "AstralisThermalSystem.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FEquilibriumTemperatureTest,
	"Astralis.Thermal.EquilibriumTemperature",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)

bool FEquilibriumTemperatureTest::RunTest(const FString& Parameters)
{
	// Black body sphere at 1 AU: alpha=1, epsilon=1, A_solar = A_rad (sphere)
	// T = (S / (4 * sigma))^0.25 for a sphere (projected = pi*r^2, radiating = 4*pi*r^2)
	// But our function uses explicit areas, so set A_solar=1, A_rad=4 to simulate sphere
	const double T = FAstralisThermalSystem::EquilibriumTemperature(
		1.0, 1361.0, 1.0, 1.0, 4.0);

	// Expected: (1361 / (4 * 5.67e-8))^0.25 ≈ 278.6 K
	const double Expected = FMath::Pow(1361.0 / (4.0 * 5.670374419e-8), 0.25);
	const double Error = FMath::Abs(T - Expected);

	TestTrue(
		FString::Printf(TEXT("Equilibrium temp %.2f K, expected %.2f K, error %.4f"), T, Expected, Error),
		Error < 0.1);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FThermalNodeUpdateTest,
	"Astralis.Thermal.NodeHeating",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)

bool FThermalNodeUpdateTest::RunTest(const FString& Parameters)
{
	FThermalNode Node;
	Node.Temperature_K = 280.0;
	Node.HeatCapacity_JK = 1000.0;
	Node.InternalHeat_W = 50.0;
	Node.RadiatorArea_m2 = 0.1;
	Node.Emissivity = 0.85;
	Node.SolarArea_m2 = 0.0; // No solar input
	Node.SolarAbsorptivity = 0.0;

	const double InitialT = Node.Temperature_K;

	// Step for 10 seconds — internal heat should raise temperature
	for (int i = 0; i < 100; ++i)
	{
		FAstralisThermalSystem::UpdateNode(Node, 0.0, 2.7, 0.1);
	}

	TestTrue(TEXT("Temperature should increase with internal heat"),
		Node.Temperature_K > InitialT);

	return true;
}
