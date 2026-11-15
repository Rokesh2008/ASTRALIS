# ASTRALIS Architecture Audit & Time-Critical Roadmap

**Date:** 2026-11-13  
**Auditor:** Principal Technical Architect  
**Context:** Vision V2 — Full launch vehicle + spacecraft + mission simulator  
**Constraint:** Time-critical delivery required  

---

## EXECUTIVE SUMMARY

**Status:** ASTRALIS has an **excellent engineering foundation** but is visually incomplete and architecturally scoped for spacecraft-only missions.

**Core Finding:** The existing component data system, simulation engines, and test coverage provide a **solid base** for expansion into full launch vehicle simulation. The vision is achievable with focused architectural additions rather than a complete rebuild.

**Critical Blocker:** NASA visual assets downloaded but **not imported** — still showing primitive cubes. Must fix immediately.

**Recommendation:** Execute **3-tier roadmap** — fix immediate blockers (24-48h), add launch vehicle architecture (1-2 weeks), then expand catalogue and compatibility systems (1-2 months).

---

## PART 1: CURRENT ARCHITECTURE ASSESSMENT

### 1.1 What Exists (Assets)

| System | Status | Quality | Notes |
|--------|--------|---------|-------|
| **3-Module Architecture** | ✅ Complete | Excellent | Clean separation: Presentation / Simulation / Data |
| **Component Data System** | ✅ Implemented | Excellent | Already has `ESpecificationType` (Illustrative/Representative/Referenced) |
| **Simulation Engine** | ✅ Complete | Excellent | 6 subsystems: Propulsion, Power, Thermal, Comms, GNC, Orbital |
| **Automated Tests** | ✅ 25 tests | Excellent | All physics validated, 25/25 passing |
| **Assembly System** | ✅ Working | Good | Snap-to-face, component placement, mass tracking |
| **Orbit Visualization** | ✅ Working | Good | RK4 propagator, eclipse detection, time warp |
| **NASA Assets** | ⚠️ Downloaded | Blocked | Landsat 8 + Earth texture downloaded but **not imported** |

**Verdict:** The engineering core is **production-ready**. The blocker is **visual presentation only**.

### 1.2 What's Missing (Gaps)

| Required System | Current Status | Priority | Estimated Effort |
|----------------|----------------|----------|------------------|
| **Launch Vehicle Architecture** | ❌ Missing | CRITICAL | 3-5 days |
| **Stage/Booster System** | ❌ Missing | CRITICAL | 2-3 days |
| **Component Catalogue/Library** | ❌ Missing | HIGH | 2-4 days |
| **Compatibility Engine** | ❌ Missing | HIGH | 3-5 days |
| **Material/Structural Modification** | ❌ Missing | MEDIUM | 4-6 days |
| **Vehicle Performance Analysis** | ⚠️ Partial | MEDIUM | 2-3 days |
| **Rocket Model Import Pipeline** | ❌ Missing | MEDIUM | 3-4 days |
| **Global Vehicle Library** | ❌ Missing | LOW | Ongoing |
| **NASA Asset Import** | ❌ BLOCKED | **URGENT** | 1-2 hours |

### 1.3 Existing Component System Analysis

**File:** `Source/AstralisData/ComponentCatalog/AstralisComponentTypes.h`

**Already Supports:**
- ✅ Component categories: Structure, Power, Propulsion, Avionics, Communications, Thermal, Instrument
- ✅ Specification types: Illustrative, Representative, Referenced (aligns perfectly with Vision V2!)
- ✅ Engineering specs: Mass, power, thermal, geometry, mounting interfaces
- ✅ Metadata: Source provenance, revision tracking
- ✅ Visual meshes via soft object pointers

**Gaps for Launch Vehicles:**
- ❌ No `Propellant` category
- ❌ No `Booster` or `Stage` categories
- ❌ No structural materials system
- ❌ No compatibility interface system
- ❌ No thrust/Isp specifications in component data (currently in subsystem specs)
- ❌ No dimensional constraints or attachment rules

**Recommendation:** **Extend existing system** rather than replace it. Add:
1. New categories: `Stage`, `Booster`, `Propellant`, `Interstage`, `Fairing`
2. New structs: `FPropulsionSpec`, `FStructuralSpec`, `FCompatibilitySpec`
3. Compatibility checking service

### 1.4 Test Coverage Analysis

**10 test files found:**
- ✅ AstralisCoordinateTests.cpp
- ✅ AstralisAssemblyTests.cpp
- ✅ AstralisCatalogTests.cpp
- ✅ AstralisCommsTests.cpp
- ✅ AstralisMassTests.cpp
- ✅ AstralisOrbitalTests.cpp
- ✅ AstralisPowerTests.cpp
- ✅ AstralisPropulsionTests.cpp
- ✅ AstralisIntegrationTests.cpp
- ✅ AstralisThermalTests.cpp

**Verdict:** **Outstanding test discipline**. All subsystems validated. Tests must be preserved during expansion.

---

## PART 2: VISION V2 ALIGNMENT ANALYSIS

### 2.1 Core Requirements from Vision V2

| Requirement | Current State | Gap Analysis |
|-------------|---------------|--------------|
| **Build complete launch vehicles** | ❌ No launch vehicle system | Need: Stage architecture, booster attachment, fairings |
| **Select existing historical rockets** | ❌ No vehicle library | Need: Vehicle database, asset import pipeline |
| **Import rocket models** | ❌ No import pipeline | Need: GLB/FBX importer, component mapping workflow |
| **Modify vehicles by component swap** | ⚠️ Partial | Have: Component system. Need: Compatibility checking, stage hierarchy |
| **Design spacecraft payloads** | ✅ **Already working** | Existing assembly system covers this |
| **Simulate launch/ascent/staging** | ❌ No launch sim | Need: Atmospheric flight, staging events, guidance |
| **Operate spacecraft missions** | ✅ **Already working** | Orbit sim + subsystems fully functional |
| **Engineering validation** | ⚠️ Partial | Have: Physics tests. Need: Compatibility engine |
| **Material modifications** | ❌ Missing | Need: Material properties, structural analysis |
| **Visual quality (AAA)** | ❌ **BLOCKED** | **URGENT:** Import NASA assets NOW |

### 2.2 Five Core Workspaces — Implementation Status

#### A. Global Vehicle Library
**Status:** ❌ Not started  
**Priority:** MEDIUM (start after immediate fixes)  
**Effort:** Ongoing data collection + 3-4 days initial structure  

**Approach:**
1. Create `ULaunchVehicleData` asset type (similar to `UAstralisComponentData`)
2. JSON/CSV import pipeline for vehicle specs
3. Initial vehicles: Saturn V, Falcon 9, Atlas V (public specs only)
4. Never invent missing data — mark gaps explicitly

#### B. Rocket Architect
**Status:** ❌ Not started  
**Priority:** **CRITICAL**  
**Effort:** 5-7 days  

**Approach:**
1. Create stage hierarchy system: `AAstralisStageActor` with attachment points
2. Extend component categories for launch vehicle parts
3. Build vertical assembly workspace (separate from spacecraft lab)
4. Implement staging logic and mass flow
5. Visual: Exploded view, cutaway rendering

#### C. Spacecraft Laboratory
**Status:** ✅ **ALREADY IMPLEMENTED**  
**Quality:** Good engineering core, poor visuals  

**Preserve:**
- Existing assembly system
- Component actors
- Snap-to-face placement
- Mass/power/thermal tracking

**Fix:**
- Import NASA Landsat 8 model (**URGENT**)
- Replace primitive meshes with production assets

#### D. Flight Dynamics
**Status:** ⚠️ Orbital only — no atmospheric launch  
**Priority:** **HIGH**  
**Effort:** 4-6 days  

**Existing:**
- ✅ RK4 two-body orbital propagator
- ✅ Propulsion system with thrust/Isp
- ✅ Mass depletion tracking

**Missing:**
- ❌ Atmospheric density model
- ❌ Drag/lift calculations
- ❌ Guidance and steering
- ❌ Staging events
- ❌ Gravity turn
- ❌ Max-Q handling

**Approach:** Add `FAstralisAtmosphericFlight` module to AstralisSimulation

#### E. Mission Control
**Status:** ⚠️ Basic telemetry only  
**Priority:** MEDIUM  
**Effort:** 3-5 days  

**Existing:**
- ✅ Orbit telemetry panel
- ✅ Time warp controls
- ✅ Subsystem monitoring

**Missing:**
- ❌ Mission briefing system
- ❌ Objective tracking
- ❌ Launch countdown
- ❌ Anomaly detection
- ❌ Mission scoring

---

## PART 3: TIME-CRITICAL ROADMAP

### TIER 1: IMMEDIATE FIXES (24-48 HOURS)

**Goal:** Get NASA assets visible, fix F5 launch, establish visual baseline

#### Task 1.1: Import NASA Assets (**URGENT — 2 hours**)
**Blocker Status:** CRITICAL — currently showing cubes instead of spacecraft

**Actions:**
1. ✅ Landsat8.glb and Earth_A.jpg already downloaded to `Content/NASA_Import/`
2. ❌ Import failed — `Content/NASA/` folders exist but **contain zero .uasset files**
3. 🔧 **Fix:** Manual import required (MCP server down)
   - Import Landsat8.glb → `Content/NASA/Spacecraft/` (scale: 100.0)
   - Import Earth_A.jpg → `Content/NASA/Planets/`
   - Create M_Earth_Enhanced material
4. ✅ C++ code already has fallback loading with diagnostics
5. ✅ Test with console command `astralis.launch`

**Deliverable:** Landsat 8 spacecraft visible in orbit scene, textured Earth

#### Task 1.2: Verify Launch Transition (**30 minutes**)
**Issue:** F5 key may not be working, console command added as backup

**Actions:**
1. Test F5 key binding (may be captured by editor)
2. Verify console command `astralis.launch` works
3. Check Output Log for diagnostic messages
4. Fix any GameMode transition issues

**Deliverable:** Reliable Assembly → Orbit transition

#### Task 1.3: Status Report Document (**1 hour**)

**Actions:**
1. Document current build status
2. Screenshot NASA assets working in-game
3. List all 25 passing tests
4. Create "What Works Now" demonstration video

**Deliverable:** `STATUS.md` with verified functionality list

---

### TIER 2: SPRINT IMPLEMENTATION (1-2 WEEKS)

**Goal:** Add launch vehicle architecture, basic catalogue, first complete rocket

#### Phase 2.1: Launch Vehicle Core (Days 1-3)

**New C++ Classes:**
```cpp
// Source/Astralis/LaunchVehicle/AstralisStageActor.h
class AAstralisStageActor : public AActor
{
    // Stage number (1 = first/booster, 2 = second, etc.)
    int32 StageNumber;
    
    // Components in this stage
    TArray<AAstralisComponentActor*> StageComponents;
    
    // Propellant mass (separate from dry mass)
    double PropellantMass_kg;
    double PropellantCapacity_kg;
    
    // Attachment points for next stage
    TArray<FStageAttachmentPoint> UpperAttachPoints;
    TArray<FStageAttachmentPoint> LowerAttachPoints;
    
    // Stage state
    bool bSeparated = false;
    bool bEnginesIgnited = false;
};

// Source/Astralis/LaunchVehicle/AstralisLaunchVehicleActor.h
class AAstralisLaunchVehicleActor : public AActor
{
    // Ordered stages (0 = boosters, 1 = first stage, 2 = second, etc.)
    TArray<AAstralisStageActor*> Stages;
    
    // Payload (spacecraft)
    AAstralisSpacecraftActor* Payload;
    
    // Vehicle performance
    double GetTotalMass_kg() const;
    double GetDeltaV_mps() const;
    double GetThrustToWeightRatio() const;
    
    // Staging
    void SeparateStage(int32 StageNumber);
};
```

**New Component Categories:**
```cpp
enum class EComponentCategory : uint8
{
    // Existing...
    Structure, Power, Propulsion, Avionics, Communications, Thermal, Instrument,
    
    // NEW for launch vehicles
    LiquidEngine,      // Liquid rocket engines
    SolidMotor,        // Solid rocket motors
    PropellantTank,    // Fuel/oxidizer tanks
    Booster,           // Complete booster assemblies
    Interstage,        // Structural connectors between stages
    Fairing,           // Payload fairings
    Adapter            // Payload adapters
};
```

**Deliverable:** Launch vehicle hierarchy working, stages can be assembled

#### Phase 2.2: Component Catalogue System (Days 4-5)

**New C++ Classes:**
```cpp
// Source/AstralisData/Catalog/AstralisComponentCatalog.h
class UAstralisComponentCatalog : public UDataAsset
{
    // All available components grouped by category
    TMap<EComponentCategory, TArray<UAstralisComponentData*>> ComponentsByCategory;
    
    // Search/filter
    TArray<UAstralisComponentData*> FindComponents(EComponentCategory Category);
    TArray<UAstralisComponentData*> FindCompatibleEngines(double MinThrust_N, double MaxMass_kg);
};
```

**Initial Catalogue Contents:**
- 5-10 liquid engines (representative specs based on real engines)
- 3-5 solid motors
- 5-10 propellant tanks (various sizes)
- 3-5 fairings
- Mark specification type clearly (Illustrative/Representative/Referenced)

**Deliverable:** Browseable component catalogue, components can be dragged into stages

#### Phase 2.3: First Complete Rocket (Days 6-7)

**Target:** Build one representative medium-lift launch vehicle

**Approach:**
- Use public Falcon 9 / Atlas V specifications as reference (mark as "Representative")
- Model as 2-stage vehicle with fairing
- Simple visual representation (cylinders acceptable for now)
- Full engineering specifications
- Performance analysis

**Deliverable:** One complete launch vehicle that can:
1. Be assembled from catalogue components
2. Have accurate mass/thrust/delta-v calculations
3. Successfully launch and deploy spacecraft payload
4. Pass automated validation tests

---

### TIER 3: FOUNDATION EXPANSION (2-8 WEEKS)

**Goal:** Full Vision V2 implementation

#### Month 1: Compatibility & Validation Engine

**Systems:**
1. Compatibility checking (mechanical, propellant, thermal, structural)
2. Material modification system
3. Structural analysis (simplified beam/column models)
4. Performance analysis dashboard
5. Vehicle comparison tool

#### Month 2: Global Vehicle Library

**Vehicles (Representative specifications):**
1. Saturn V (historical)
2. Space Shuttle (historical)
3. Falcon 9 (current)
4. Atlas V (current)
5. Ariane 6 (current)
6. Long March 5 (current)

**Each vehicle:**
- Complete stage breakdown
- Component list
- Historical specifications
- Visual model (source from public 3D repositories)
- Can be duplicated and modified

#### Months 3-4: Atmospheric Flight Simulation

**Physics:**
1. Atmospheric density model (exponential or tabulated)
2. Aerodynamic forces (drag/lift coefficients)
3. Gravity turn guidance
4. Pitch/yaw/roll control
5. Max-Q structural loading
6. Staging dynamics
7. Fairing separation

#### Months 5-6: Mission Integration

**Systems:**
1. Mission briefing system
2. Launch site selection
3. Trajectory planning
4. Launch windows
5. Mission constraints
6. Anomaly simulation
7. Mission scoring

#### Months 7-8: Import Pipeline & Polish

**Tools:**
1. Rocket model importer (GLB/FBX → component hierarchy)
2. Automated component mapping
3. Specification database integration
4. Visual polish pass
5. Cinematic sequences
6. Tutorial missions

---

## PART 4: CRITICAL DECISIONS

### Decision 1: Spacecraft vs Launch Vehicle Priority

**Question:** Focus on spacecraft missions first (existing strength) or launch vehicles first (new requirement)?

**Recommendation:** **Hybrid approach**
- **Immediate (Tier 1):** Fix spacecraft visuals (NASA assets)
- **Sprint (Tier 2):** Add launch vehicle architecture
- **Result:** Complete launch → orbit → mission experience

**Rationale:** Vision V2 requires both. Starting with visual fixes gives immediate progress while planning launch vehicle architecture.

### Decision 2: Simulation Fidelity

**Question:** How accurate should atmospheric launch simulation be?

**Options:**
1. **Simplified:** Gravity turn with exponential atmosphere, basic drag (3-4 days)
2. **Intermediate:** 3DOF trajectory, tabulated atmosphere, proper aerodynamics (1-2 weeks)
3. **High-fidelity:** 6DOF simulation, wind models, structural dynamics (1-2 months)

**Recommendation:** **Start with Simplified, expand to Intermediate**
- Simplified for Tier 2 (playable launch)
- Intermediate for Tier 3 (accurate performance)
- High-fidelity as stretch goal

### Decision 3: Component Specifications

**Question:** How much real-world data to include initially?

**Recommendation:** **Tiered approach**
1. **Tier 2:** 10-20 "Representative" components (based on real classes, simplified specs)
2. **Tier 3:** 50-100 "Representative" + 10-20 "Referenced" (actual sourced specs)
3. **Ongoing:** Community contributions, expanded database

**Critical Rule:** Never invent specifications. Mark everything clearly as Illustrative/Representative/Referenced.

### Decision 4: Visual Asset Strategy

**Question:** How to get production-quality rocket models?

**Options:**
1. NASA 3D Resources (limited selection)
2. Public 3D model repositories (license verification required)
3. Procedural generation (cylinders/cones with detailed textures)
4. Commission custom models (expensive, time-consuming)

**Recommendation:** **Hybrid approach**
- **Tier 1:** NASA assets (Landsat 8, Earth) — already downloaded
- **Tier 2:** Procedural rocket stages with good materials (acceptable placeholder)
- **Tier 3:** Source/commission high-quality models for key vehicles

---

## PART 5: IMMEDIATE NEXT STEPS

### Right Now (Next 2 Hours)

1. **USER ACTION REQUIRED:** Import NASA assets following `IMPORT_INSTRUCTIONS.md`
2. **Test:** Verify `astralis.launch` console command works
3. **Screenshot:** NASA spacecraft visible in orbit
4. **Report:** What Output Log shows during asset loading

### After Asset Import (Next 24 Hours)

1. **Code:** Create `AstralisStageActor` and `AstralisLaunchVehicleActor` stub classes
2. **Design:** Sketch launch vehicle UI mockups
3. **Data:** Research Falcon 9 / Atlas V public specifications
4. **Plan:** Detailed Tier 2 task breakdown

### Week 1

1. Implement stage hierarchy system
2. Extend component categories
3. Build simple 2-stage test rocket
4. Implement staging logic

---

## PART 6: RISK ASSESSMENT

### High Risks

| Risk | Impact | Mitigation |
|------|--------|------------|
| **NASA assets still not importing** | CRITICAL | Manual import required, MCP server unavailable |
| **Scope creep into vehicle library** | HIGH | Strict focus on ONE reference vehicle for Tier 2 |
| **Atmospheric sim complexity** | MEDIUM | Start with simplified model, expand later |
| **Missing rocket specifications** | MEDIUM | Use "Representative" designation, never invent data |

### Medium Risks

| Risk | Impact | Mitigation |
|------|--------|------------|
| **Test regression during expansion** | MEDIUM | Run full test suite before each commit |
| **Component system refactor needed** | MEDIUM | Extend rather than replace existing system |
| **Performance issues with detailed models** | LOW | LOD system, instanced rendering |

---

## APPENDIX A: EXISTING ARCHITECTURE DIAGRAM

```
ASTRALIS Project Structure (Current)
│
├─ Astralis Module (Presentation Layer)
│  ├─ Core/
│  │  ├─ AstralisAssemblyGameMode      ← Game state, phase management
│  │  ├─ AstralisAssemblyPC            ← Player input, F5 launch binding
│  │  └─ AstralisAssemblyPawn          ← Camera control
│  ├─ Spacecraft/
│  │  ├─ AstralisComponentActor        ← Individual component instances
│  │  └─ AstralisSpacecraftActor       ← Complete spacecraft assembly
│  ├─ Orbit/
│  │  └─ AstralisOrbitSceneActor       ← Orbit visualization, NASA assets
│  ├─ UI/
│  │  ├─ AstralisAssemblyHUD           ← Assembly UI
│  │  ├─ AstralisComponentPalette      ← Component browser
│  │  ├─ AstralisStatusPanel           ← System status
│  │  └─ AstralisOrbitTelemetryPanel   ← Orbit data display
│  └─ Presentation/
│     ├─ AstralisCoordinateService     ← SI ↔ UE coordinate conversion
│     └─ AstralisProceduralMeshFactory ← Mesh generation
│
├─ AstralisSimulation Module (Physics Engine)
│  ├─ SimCore/
│  │  ├─ AstralisSimEngine             ← Central simulation coordinator
│  │  ├─ AstralisSimState              ← Spacecraft state data
│  │  └─ AstralisSimClock              ← Time management, time warp
│  ├─ OrbitalMechanics/
│  │  └─ AstralisTwoBodyPropagator     ← RK4 orbit integration
│  ├─ Propulsion/
│  │  └─ AstralisPropulsionSystem      ← Thrust, Isp, propellant depletion
│  ├─ Power/
│  │  └─ AstralisPowerSystem           ← Solar arrays, batteries, eclipse
│  ├─ Thermal/
│  │  └─ AstralisThermalSystem         ← Heat balance, radiators
│  ├─ Communications/
│  │  └─ AstralisCommsSystem           ← Link budget, ground station visibility
│  ├─ GNC/
│  │  └─ AstralisGNCSystem             ← Attitude control, reaction wheels
│  └─ MassProperties/
│     └─ AstralisMassCalculator        ← Center of mass, inertia tensor
│
└─ AstralisData Module (Component Database)
   └─ ComponentCatalog/
      └─ AstralisComponentTypes         ← Component data assets
         ├─ EComponentCategory
         ├─ ESpecificationType          ← Illustrative/Representative/Referenced
         └─ UAstralisComponentData      ← Base component asset type
```

---

## APPENDIX B: PROPOSED ARCHITECTURE (POST-EXPANSION)

```
ASTRALIS Project Structure (Vision V2)
│
├─ [EXISTING MODULES PRESERVED]
│
├─ NEW: Astralis/LaunchVehicle/
│  ├─ AstralisStageActor               ← Individual stage with propellant
│  ├─ AstralisLaunchVehicleActor       ← Complete vehicle assembly
│  ├─ AstralisBoosterActor             ← Strap-on boosters
│  ├─ AstralisFairingActor             ← Payload fairing with separation
│  └─ AstralisLaunchPadActor           ← Ground infrastructure
│
├─ NEW: AstralisData/VehicleLibrary/
│  ├─ ULaunchVehicleData               ← Complete vehicle specifications
│  ├─ UAstralisComponentCatalog        ← Searchable component database
│  └─ UVehicleCompatibilityRules       ← Compatibility checking rules
│
├─ NEW: AstralisSimulation/Atmospheric/
│  ├─ FAstralisAtmosphericModel        ← Density, temperature, pressure
│  ├─ FAstralisAerodynamics            ← Drag, lift coefficients
│  ├─ FAstralisGuidance                ← Gravity turn, pitch program
│  └─ FAstralisStagingDynamics         ← Stage separation physics
│
└─ NEW: Astralis/VehicleArchitect/
   ├─ AstralisVehicleDesignGameMode    ← Rocket assembly workspace
   ├─ AstralisPerformanceAnalyzer      ← Delta-v, TWR calculations
   └─ AstralisCompatibilityValidator   ← Component fit checking
```

---

## CONCLUSION

**ASTRALIS has an exceptional engineering foundation.** The simulation engine is complete, tested, and production-ready. The component data system already supports the specification provenance tracking required by Vision V2.

**The critical path is:**
1. **Fix visual presentation NOW** (import NASA assets)
2. **Add launch vehicle architecture** (1-2 weeks)
3. **Expand catalogue and compatibility systems** (1-2 months)

**This is NOT a rebuild — it's a focused expansion** of a well-architected system.

The project can absolutely deliver Vision V2. Time pressure requires ruthless prioritization: **spacecraft visuals first, launch vehicles second, full library third.**

**Recommended immediate action:** Import NASA assets following `IMPORT_INSTRUCTIONS.md`, verify F5/console launch works, then proceed to Tier 2 implementation.
