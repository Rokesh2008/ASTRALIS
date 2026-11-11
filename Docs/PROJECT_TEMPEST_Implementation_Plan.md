# PROJECT TEMPEST - Implementation Plan
## ASTRALIS Vertical Slice: Flood Observation Mission

**Status:** Phase 1 - Asset Foundation  
**Target:** Complete playable mission from briefing to evaluation  
**Timeline:** 4 weeks to NASA Space Apps Challenge  
**Current Date:** 2026-11-11

---

## Mission Statement

Transform ASTRALIS from an engineering prototype (cubes and cylinders) into a polished, playable Earth observation mission that demonstrates:

1. **Professional Visual Quality** - NASA spacecraft model, enhanced Earth, aerospace UI
2. **Real Engineering Simulation** - Validated physics driving gameplay constraints
3. **Complete Mission Flow** - Briefing → Design → Launch → Operations → Evaluation
4. **Meaningful Decisions** - Engineering choices affect mission outcomes

---

## Current State Assessment

### ✅ What Works (Keep & Build Upon)
- **Physics Core:** FAstralisSimEngine with 25/25 tests passing
  - Two-body propagation (RK4)
  - Eclipse detection (cylindrical shadow)
  - Ground station visibility (elevation angle)
  - Power budget (solar + battery)
  - Thermal simulation
  - Link budget calculations
  - Time warp (0.1x to 10000x)
  
- **Data Architecture:** Component catalog system with real specifications
- **Assembly Logic:** Snap-to-face placement, mass calculations
- **Orbit Visualization:** Living simulation with telemetry panel

### ❌ What Must Transform
- **Visuals:** Engine primitives → NASA spacecraft models
- **Environment:** Empty space → Mission complex environments
- **UI:** Basic Slate panels → Aerospace mission control aesthetic
- **Experience:** Disconnected tools → Integrated mission narrative
- **Materials:** Default → PBR with proper lighting

### 🆕 What's Missing
- Mission briefing system
- Scientific observation gameplay
- Launch sequence
- Mission evaluation
- Decision consequence tracking

---

## The Vertical Slice: PROJECT TEMPEST Mission

**Scenario:** An unexpected flood event requires urgent satellite observation for disaster response planning.

**Player Role:** Mission engineer responsible for deploying an Earth observation satellite.

**Duration:** 15-25 minutes for complete playthrough

**Success Criteria:** Acquire scientifically useful observations of the target region within mission constraints.

---

## Phase-by-Phase Implementation

### PHASE 1: Visual Foundation (Week 1, Days 1-2) ✅ IN PROGRESS

**Goal:** Replace all primitive visuals with professional NASA assets

#### Tasks:
- [x] Download NASA Landsat 8 model
- [x] Download NASA Earth texture
- [x] Create attribution documentation
- [ ] Import Landsat 8 GLB to Unreal
- [ ] Import Earth texture
- [ ] Create enhanced Earth material with atmospheric scattering
- [ ] Replace orbit scene spacecraft actor mesh
- [ ] Test scene: Landsat 8 orbiting photorealistic Earth
- [ ] Screenshot comparison: Before (cubes) vs After (NASA assets)

#### Acceptance:
- Orbit scene shows recognizable spacecraft model
- Earth has day/night terminator and atmosphere
- Simulation still runs at 60 FPS

---

### PHASE 2: Mission Structure (Week 1, Days 3-4)

**Goal:** Implement complete mission flow framework

#### Tasks:
- [ ] Create mission briefing screen (UMG)
  - Mission title and scenario text
  - Target region visualization on globe
  - Scientific objectives
  - Constraints (power, data storage, timeline)
  - "Begin Mission" button
  
- [ ] Implement mission phase system
  - `EAstralisMissionPhase` enum (Briefing, Design, Launch, Operations, Evaluation)
  - Phase transition logic
  - UI state management per phase
  
- [ ] Create mission data structure
  - Target coordinates
  - Required observation parameters
  - Success criteria
  - Timeline constraints
  
- [ ] Mission timer/clock system
  - Mission elapsed time
  - Countdown to observation windows
  - Event scheduling

#### Acceptance:
- Player can read mission briefing
- Briefing explains what must be accomplished
- "Begin Mission" transitions to design phase
- Mission objectives are clear and measurable

---

### PHASE 3: Engineering Constraints (Week 1-2, Days 5-7)

**Goal:** Make engineering decisions matter for mission success

#### Tasks:
- [ ] Observation window calculator
  - Compute when satellite passes over target region
  - Display pass times and durations
  - Visibility constraints (elevation angle, lighting)
  
- [ ] Instrument activation system
  - Power draw during observation
  - Data generation rate
  - Thermal constraints
  
- [ ] Mission Causality tracker
  - Log every significant engineering state change
  - Track decision → consequence chains
  - Provide "Why did this happen?" queries
  
- [ ] Engineering dashboard enhancements
  - Battery reserve vs observation duration
  - Data storage vs downlink opportunities
  - Thermal margins during operation
  
- [ ] Constraint violation system
  - Battery depleted → instrument shutdown
  - Thermal limits → sensor degradation
  - Storage full → data loss

#### Acceptance:
- Player can see when target is observable
- Activating instrument drains battery realistically
- Running out of power prevents observation
- Player understands WHY they failed (not just that they failed)

---

### PHASE 4: Visual Polish (Week 2, Days 8-10)

**Goal:** Professional aerospace aesthetic throughout

#### Tasks:
- [ ] UI/UX overhaul
  - Design aerospace color palette (dark gray, cyan, amber, red)
  - Create custom UMG style assets
  - Implement data-dense mission control layouts
  - Add status indicators (nominal/caution/critical)
  
- [ ] Enhanced Earth rendering
  - Cloud layer (procedural or texture)
  - Night lights for populated areas
  - Atmospheric glow at limb
  - Accurate day/night terminator
  
- [ ] Spacecraft visual enhancements
  - Emissive materials for active components
  - Solar panel reflectivity
  - Antenna orientation toward Earth
  - Status lights (power, comms, thermal)
  
- [ ] Camera work
  - Cinematic orbit camera transitions
  - Smooth follow during time warp
  - Picture-in-picture for spacecraft close-up
  
- [ ] Lighting
  - Realistic space lighting (harsh sun, pure black shadows)
  - Earth albedo reflection on spacecraft
  - Eclipse transition

#### Acceptance:
- Screenshots look like a professional space mission game
- UI is readable and aesthetically cohesive
- Earth looks photorealistic from orbit
- Spacecraft details are visible and attractive

---

### PHASE 5: Launch Sequence (Week 2-3, Days 11-14)

**Goal:** Cinematic transition from design to orbit

#### Tasks:
- [ ] Launch vehicle selection
  - Display payload mass
  - Show compatible launchers
  - Simple selection (Falcon 9 class representative)
  
- [ ] Pre-launch checklist
  - Mass budget ✓/✗
  - Power budget ✓/✗
  - Mission feasibility ✓/✗
  
- [ ] Launch cinematic
  - Countdown (T-10 seconds)
  - Ignition effects (particle system)
  - Vehicle ascent (animated)
  - Stage separation (simple)
  - Payload fairing jettison
  - Spacecraft deployment
  - Solar array deployment
  - Transition to orbital view
  
- [ ] Use Sequencer for cinematic
  - Camera cuts
  - Timed events
  - Smooth transitions

#### Acceptance:
- Launch feels like a significant event, not a menu transition
- Spacecraft deployed matches the one player configured
- Transition to orbit is smooth and exciting
- 30-60 second cinematic maximum

---

### PHASE 6: Scientific Observation (Week 3, Days 15-17)

**Goal:** Turn simulation into gameplay

#### Tasks:
- [ ] Target region system
  - 3D globe with target overlay
  - Satellite ground track visualization
  - FOV (field of view) projection on surface
  
- [ ] Observation mechanics
  - Manual instrument activation during pass
  - Power/thermal/data simulation during imaging
  - Visual feedback (instrument animation)
  
- [ ] Data product system
  - Simulated observation imagery
  - Data volume calculation
  - Storage management
  
- [ ] Downlink system
  - Ground station contact scheduler
  - Data transmission simulation
  - Mission data backlog tracking
  
- [ ] Random challenge event
  - Example: "Unexpected solar storm reduces power generation 30%"
  - Player must adapt operations
  - Tests engineering margin decisions

#### Acceptance:
- Player can see when to activate instrument
- Observation generates data and consumes resources
- Player must manage trade-offs (power vs data)
- Challenge event creates meaningful tension

---

### PHASE 7: Mission Evaluation (Week 3-4, Days 18-20)

**Goal:** Close the loop - show consequences of engineering decisions

#### Tasks:
- [ ] Mission results screen
  - Target coverage map (% observed)
  - Data quality metrics
  - Resource utilization graphs
  - Timeline replay
  
- [ ] Decision review system
  - "What happened" narrative
  - Consequence chain visualization
  - Alternative scenario simulation
  
- [ ] Mission grading (not arbitrary score)
  - Scientific objectives met/missed
  - Efficiency metrics
  - Constraint violations
  
- [ ] Debrief summary
  - What went well
  - What failed and why
  - Engineering lessons learned

#### Acceptance:
- Player understands if they succeeded or failed
- Evaluation explains WHY based on engineering
- Player can see how different decisions would change outcome
- Not a simple A-F grade

---

### PHASE 8: Integration & Polish (Week 4, Days 21-28)

**Goal:** Complete, bug-free, optimized experience

#### Tasks:
- [ ] End-to-end playthrough testing
- [ ] Performance optimization (target: 60 FPS on RTX 3050)
- [ ] Audio integration (NASA sound library)
- [ ] Tutorial/onboarding for first-time players
- [ ] Credits screen with NASA attribution
- [ ] Bug fixing
- [ ] Final visual pass
- [ ] Packaging for distribution

---

## Technical Implementation Details

### Asset Integration Workflow

1. **NASA Landsat 8 Import:**
   ```
   Content/NASA/Spacecraft/Landsat8/
   ├── SM_Landsat8.uasset          # Static Mesh
   ├── M_Landsat8_Bus.uasset       # Material instance (metallic body)
   ├── M_Landsat8_Panels.uasset    # Material instance (solar arrays)
   └── M_Landsat8_Instruments.uasset # Material instance (sensors)
   ```

2. **Enhanced Earth Actor:**
   ```cpp
   AAstralisEnhancedEarth : public AActor
   {
       UStaticMeshComponent* SphereMesh;
       UMaterialInstanceDynamic* EarthMaterial;
       UMaterialInstanceDynamic* AtmosphereMaterial;
       void UpdateDayNightTerminator(FVector SunDirection);
       void SetCloudCoverage(float Coverage);
   };
   ```

3. **Mission Briefing Widget:**
   ```cpp
   UMissionBriefingWidget : public UUserWidget
   {
       UTextBlock* MissionTitle;
       UTextBlock* MissionDescription;
       UImage* TargetRegionMap;
       UButton* BeginMissionButton;
       FMissionData MissionParameters;
   };
   ```

### Performance Targets

- **Frame Rate:** 60 FPS minimum (RTX 3050 6GB, 16GB RAM)
- **Load Times:** < 5 seconds between phases
- **Memory:** < 4GB VRAM usage
- **Disk:** < 2GB installed size

### Code Changes Required

**Minimal disruption to existing validated systems:**

- Keep: FAstralisSimEngine, all simulation classes
- Keep: Test suite
- Keep: Data model (UAstralisComponentData)
- Replace: Procedural mesh factory → NASA model loader
- Replace: Primitive Earth sphere → Enhanced Earth actor
- Add: Mission phase management
- Add: Observation/science layer
- Add: UI systems (briefing, evaluation)

---

## Risk Mitigation

### Risk: Timeline Too Aggressive
**Mitigation:** Phase 1-4 = Minimum Viable Product. Phases 5-8 are enhancements.

### Risk: NASA Assets Don't Import Cleanly
**Mitigation:** GLB format is natively supported by UE5. Test import immediately (Phase 1).

### Risk: Performance Issues with High-Res Textures
**Mitigation:** Use texture streaming, LODs, and tested earth on RTX 3050 early.

### Risk: Mission Gameplay Too Complex
**Mitigation:** Single observation pass, clear objectives, tutorial guidance.

### Risk: Scope Creep
**Mitigation:** PROJECT TEMPEST is the ONLY mission in initial release. No additional scenarios.

---

## Success Metrics

**By the end of Week 4, ASTRALIS must:**

1. ✅ **Look professional** - Screenshots indistinguishable from real space mission games
2. ✅ **Play complete** - Briefing → Launch → Observation → Evaluation without crashes
3. ✅ **Demonstrate engineering** - Decisions have clear consequences
4. ✅ **Run smoothly** - 60 FPS on target hardware
5. ✅ **Tell a story** - Player understands they're managing a real mission

**Competition Readiness Checklist:**

- [ ] 2-minute gameplay video recorded
- [ ] 10 polished screenshots
- [ ] Project description written
- [ ] NASA attribution included
- [ ] Source code documented
- [ ] Builds package without errors
- [ ] Tested on clean Windows machine

---

## Development Discipline

**Every commit must:**
- Build cleanly
- Pass all existing tests
- Have meaningful commit message
- Not break previous functionality

**Every milestone must:**
- Be visually verified in-editor
- Have screenshot proof
- Be playable (not just "code works")

**No claims without evidence:**
- "Feature works" = Player can use it successfully
- "Looks good" = Screenshot proves it
- "Runs fast" = FPS counter confirms it

---

## Next Immediate Action

**Right now:**
1. Import Landsat8.glb to Unreal (Content/NASA/Spacecraft/)
2. Import Earth_A.jpg to Unreal (Content/NASA/Textures/)
3. Create test map: "MAP_AssetTest"
4. Replace orbit scene spacecraft mesh with Landsat 8
5. Apply Earth texture to sphere
6. Take screenshot
7. Compare with previous cube-based orbit scene

**Status Update Required:**
- Screenshot showing Landsat 8 in orbit
- Confirmation simulation still runs
- FPS measurement

---

**Document Owner:** ASTRALIS Technical Direction  
**Last Updated:** 2026-11-11  
**Next Milestone:** Phase 1 Complete - NASA Assets Integrated

---

*This is a LIVING DOCUMENT. Update after each phase completion.*
