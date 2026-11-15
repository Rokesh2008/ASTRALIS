# ASTRALIS - Spaceflight Engineering Simulator

A real-time spaceflight engineering simulator built in Unreal Engine 5.8 with C++. Design, configure, launch, and operate spacecraft with physics-accurate orbital mechanics.

## Features

### Spacecraft Assembly
- Component-based spacecraft construction (drag-and-place on pad)
- Snap-to-face system for attaching modules
- Real-time mass properties calculation (center of mass, inertia tensor)
- Landsat 8 reference payload (2,623 kg)

### Launch Vehicle Engineering
- Falcon 9-class two-stage launch vehicle with procedural 3D model
  - 9-engine first stage with nozzle ring, grid fins, landing legs
  - Single-engine vacuum-optimized second stage
  - Payload fairing with jettison at 110 km
- **Engine Swap System**: 8 real-world engines to choose from
  - Merlin-1D, Raptor, RS-25, RL-10, Vikas, Vulcain-2, RD-180, S200
  - See real-time impact on TWR, delta-v, and mass
- Full vehicle specification display (mass, delta-v, TWR, staging)

### Ascent Simulation (2D Polar Coordinate Gravity Turn)
- Sub-stepped physics at 0.1s resolution with 10x time acceleration
- Exponential atmospheric model (scale height 8.5 km)
- Thrust interpolation between sea-level and vacuum performance
- Pitch program: vertical climb -> polynomial gravity turn -> energy-optimal guidance
- Aerodynamic drag model (Cd*A = 3.76 m^2)
- Real-time telemetry: altitude, velocity, TWR, pitch, max-Q, downrange, orbital elements
- Stage separation with visual effects
- Orbit achievement detection via perigee check

### Orbital Operations
- Two-body Keplerian propagation with sub-stepping
- Full spacecraft subsystem simulation:
  - **Power**: Solar array generation, eclipse detection, battery charge/discharge
  - **Thermal**: Radiative equilibrium, component temperature tracking
  - **Communications**: Link budget (transmit power, gain, free-space loss, SNR margin)
  - **GNC**: Attitude determination and control
  - **Propulsion**: On-orbit maneuvering capability
- Earth visualization with rotation (sidereal day)
- Orbit trail rendering
- Time warp: 0.1x to 10,000x with pause

## Controls

### Assembly Phase (On Pad)
| Key | Action |
|-----|--------|
| F5 | Launch to orbit |
| E | Toggle engine swap mode |
| Tab | Switch stage (in swap mode) |
| 1-8 | Select engine from catalog |
| Scroll | Zoom camera |
| Right-click drag | Orbit camera |

### Launch Phase
- Automatic ascent -- watch the gravity turn unfold
- Real-time telemetry displayed on screen

### Orbit Phase
| Key | Action |
|-----|--------|
| > (Period) | Increase time warp |
| < (Comma) | Decrease time warp |
| Space | Toggle pause |
| R | Return to assembly |

## Architecture

Three-module C++ architecture:

```
Astralis/           -- Presentation layer (actors, UI, camera)
  Core/             -- Game mode, player controller, pawn
  LaunchVehicle/    -- Stage actors, vehicle actor, flight physics
  Orbit/            -- Orbit scene with subsystem simulation
  Spacecraft/       -- Assembly actors, component placement
  UI/               -- HUD, panels

AstralisSimulation/ -- Physics engine (no UE dependencies)
  Orbit/            -- Two-body propagator
  Power/            -- Solar + battery model
  Thermal/          -- Radiative equilibrium
  Comms/            -- Link budget calculator
  GNC/              -- Attitude control
  Propulsion/       -- Engine + propellant management
  MassProperties/   -- CoM + inertia tensor

AstralisData/       -- Data definitions
  ComponentCatalog/ -- Spacecraft component specs
  VehicleLibrary/   -- Engine catalog (8 real-world engines)
```

## Technical Details

- **Engine**: Unreal Engine 5.8.4
- **Language**: C++ (no Blueprints for core logic)
- **Physics constants**: WGS-84 Earth (mu = 3.986e14 m^3/s^2, R = 6.371e6 m)
- **Orbital mechanics**: Vis-viva, Tsiolkovsky rocket equation, Keplerian elements
- **Atmosphere**: ISO standard exponential model
- **Launch site**: Kennedy Space Center (28.5 deg N latitude)

## Building

```
# From UE 5.8 installation
Engine/Build/BatchFiles/Build.bat game1Editor Win64 Development -Project="<path>/game1.uproject"
```

Open `game1.uproject` in Unreal Editor, press Play (or F5 in-game to launch).

## Mission Profile

Default mission: Deploy Landsat 8 to 400 km LEO
1. T-5s countdown
2. 9x Merlin-1D ignition, liftoff (TWR ~1.4)
3. Pitch-over at T+12s, polynomial gravity turn
4. Max-Q passage (~30 kPa)
5. MECO at T+162s, stage separation
6. MVac ignition, energy-optimal guidance to 400 km
7. Fairing jettison at 110 km
8. Orbit insertion (400 km circular, ~92 min period)
9. Payload deployment, transition to orbital operations
