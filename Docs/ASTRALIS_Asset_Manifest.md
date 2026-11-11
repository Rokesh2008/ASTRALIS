# ASTRALIS Asset Manifest & Visual Production Plan

**Document Status:** Phase 1 Planning  
**Date:** 2026-11-11  
**Purpose:** Define the transformation from greybox prototype to AAA-quality space mission game

---

## Executive Summary

This document establishes the asset pipeline and visual production requirements to transform ASTRALIS from an engineering prototype (cubes/cylinders) into a polished, cinematic space mission experience focused on **PROJECT TEMPEST** — an Earth observation flood monitoring mission.

**Core Requirement:** Every visual element must serve the mission narrative and engineering simulation, not be decorative filler.

---

## 1. NASA 3D Resources - Available Assets

**Source:** `https://github.com/nasa/NASA-3D-Resources`  
**License:** Free to download and use per [NASA Media Usage Guidelines](https://www.nasa.gov/nasa-brand-center/images-and-media/)  
**Format:** Primarily `.glb` (glTF binary) - natively supported by Unreal Engine 5.8

### 1.1 Earth Observation Satellites (Priority 1 for PROJECT TEMPEST)

| Asset | Mission Type | Status | Technical Notes |
|-------|-------------|--------|-----------------|
| **Landsat 8** | Land/water observation | ✅ Available | **PRIMARY CHOICE** - Modern Earth observation, appropriate for flood monitoring |
| **Aqua** | Water cycle observation | ✅ Available | 3 variants (A/B/C), excellent for water-related missions |
| **Terra** | Flagship Earth observation | ✅ Available | Alternative to Landsat |
| **ICESat-2** | Ice/land elevation | ✅ Available | 3 variants, laser altimetry |
| **CALIPSO** | Cloud/aerosol lidar | ✅ Available | Specialized atmospheric observation |
| **CYGNSS** | Hurricane monitoring | ✅ Available | Constellation of 8 micro-satellites |
| **GOES** | Geostationary weather | ✅ Available | Weather observation |
| **Earth Observing-1 (EO-1)** | Technology demonstration | ✅ Available | Predecessor to modern systems |

**Decision for PROJECT TEMPEST:** Use **Landsat 8** as the primary spacecraft model.

**Rationale:**
- Iconic, recognizable Earth observation platform
- Appropriate mission profile for flood monitoring
- Well-documented public specifications
- Modern design aesthetic
- Single clear model variant

### 1.2 Modular Satellite Components

| Asset | Purpose | Use Case |
|-------|---------|----------|
| Satellite Kit - Body (3 variants) | Bus structures | Assembly configurator |
| Satellite Kit - Radio (3 variants) | Comm antennas | Subsystem selection |
| Satellite Kit - Wings (2 variants) | Solar arrays | Power configuration |

**Decision:** Archive for future modular assembly features. PROJECT TEMPEST uses complete Landsat 8 model first.

### 1.3 Launch Vehicles

| Asset | Era | Payload Capacity | Status |
|-------|-----|-----------------|--------|
| **Saturn V** | Apollo | ~140,000 kg to LEO | ✅ Available (iconic but oversized) |
| **Atlas 6/7/9** | Mercury | ~1,400 kg to orbit | ✅ Available (historic) |
| **Explorer Jupiter-C** | 1950s | ~14 kg | ✅ Available (historic) |
| **SRB (Solid Rocket Booster)** | Shuttle era | Component only | ✅ Available |
| **Mobile Launcher** | SLS/Artemis | Ground infrastructure | ✅ Available |

**Decision for PROJECT TEMPEST:** 
- **Primary:** Use simplified launch sequence with SRB + generic upper stage visualization
- **Stretch Goal:** Model appropriate modern launch vehicle (Atlas V / Falcon 9 class) or use NASA's Mobile Launcher as ground infrastructure with simplified vehicle

**Gap Identified:** No modern medium-lift launch vehicle (2000-8000 kg class) available in NASA resources. **Mitigation:** Use simplified/representative launch sequence focused on engineering simulation rather than detailed vehicle model.

### 1.4 Planetary Textures

| Asset | Resolution | Format | Status |
|-------|-----------|--------|--------|
| **Earth Texture (A)** | High | JPG/TIF | ✅ Available |
| **Earth Texture (B)** | High | JPG/TIF | ✅ Available |

**Decision:** Import NASA Earth textures to replace current primitive sphere.

### 1.5 Additional Spacecraft (Archive for Future Missions)

- Mars Global Surveyor (interplanetary)
- Various Apollo hardware (lunar)
- ISS components (LEO assembly)
- Hubble/JWST (deep space observation)

---

## 2. Asset Provenance & Attribution

**Licensing:** NASA generally allows free use of its imagery and 3D models for educational, informational, and non-commercial purposes. Commercial use requires case-by-case review.

**ASTRALIS Usage:**
- **Classification:** Educational space mission simulator
- **Attribution Required:** Yes - "3D models courtesy of NASA" in credits
- **Modifications:** Allowed - may be adapted for game integration
- **Distribution:** Models stay within compiled game, not redistributed as standalone assets

**Reference:** https://www.nasa.gov/nasa-brand-center/images-and-media/

**Action Item:** Include attribution screen in game credits.

---

## 3. Asset Import & Integration Pipeline

### 3.1 Unreal Engine 5.8 GLB Import Process

1. **Download from NASA GitHub:** `git clone https://github.com/nasa/NASA-3D-Resources.git`
2. **Locate Asset:** `3D Models/Landsat 8/Landsat 8.glb`
3. **Import to UE:**
   - Content Browser → Import
   - Select `.glb` file
   - UE auto-converts to StaticMesh + Materials
   - Verify scale (NASA models typically meter-scale)
4. **Material Setup:**
   - Inspect auto-generated materials
   - Convert to PBR if needed
   - Add emissive for active components (antennas, instruments)
5. **LOD Configuration:**
   - Generate LODs for performance
   - Test at orbit distances (100m - 10km camera range)
6. **Collision:**
   - Generate simplified collision for selection/interaction
   - Disable for pure visual elements

### 3.2 Scale Verification

**Critical:** NASA models use real-world meter scale. UE uses centimeters.
- Landsat 8 actual dimensions: ~3m × 3m × 4m
- UE expects: 300cm × 300cm × 400cm
- **Verify on import:** Model should be human-graspable size, not massive

### 3.3 Material Enhancement

NASA models come with basic materials. Enhance for game quality:
- **Metallic surfaces:** Spacecraft bus, solar panel frames
- **Reflective:** Solar panels (high metallic, low roughness)
- **Matte:** Thermal blankets, instruments (low metallic, high roughness)
- **Emissive:** Status lights, active instruments
- **Normals:** Add detail to flat surfaces

---

## 4. Environment & Mission Complex Assets

### 4.1 Required Environments

| Environment | Purpose | Asset Strategy |
|-------------|---------|----------------|
| **Mission Control** | Mission briefing, telemetry monitoring | Custom architecture + stock UE assets |
| **Spacecraft Lab** | Engineering/assembly interface | Clean room aesthetic, technical lighting |
| **Launch Complex** | Cinematic launch sequence | NASA Mobile Launcher + simplified pad |
| **Orbital View** | Mission operations | Enhanced Earth, orbital mechanics viz |

### 4.2 Unreal Marketplace Alternatives

For environments not covered by NASA resources:

**Potential Sources:**
- **Architecture:** Modern/industrial environment kits
- **Technical:** Server rooms, control centers, laboratories
- **Space:** Generic sci-fi may be adapted with NASA aesthetic

**Quality Bar:** Must match NASA asset fidelity level. No low-poly or stylized assets.

### 4.3 Earth Enhancement

**Current:** Basic sphere with primitive material  
**Target:** Photorealistic planetary body

**Implementation:**
1. Import NASA Earth texture (8K if available)
2. Atmospheric scattering shader (Unreal's Sky Atmosphere)
3. Cloud layer (procedural or texture-based)
4. Night lights texture for populated areas
5. Dynamic day/night terminator
6. Real-time shadow for eclipse calculations

**Reference:** Unreal Engine's built-in "BP_Sky_Sphere" + NASA textures

---

## 5. UI/UX Visual Language

### 5.1 Aerospace Design Aesthetic

**Inspiration:**
- NASA mission control displays (clean, data-dense, high contrast)
- Modern aerospace HUDs (F-35, Boeing 787 cockpit)
- SpaceX Dragon 2 interface (touchscreen, minimalist)

**Reject:**
- Generic sci-fi holographic effects
- Overcomplicated HUD clutter
- Low-contrast or illegible text

### 5.2 UI Components to Redesign

| Current | Target |
|---------|--------|
| Basic Slate panels | Custom UMG widgets with aerospace styling |
| Default fonts | Nasa/aerospace monospace fonts |
| Primitive layouts | Data-dense mission control layouts |
| Static text | Live telemetry with trend graphs |
| No visual hierarchy | Clear critical/caution/advisory color coding |

### 5.3 Color Palette

**Mission Control Standard:**
- **Background:** Dark gray (#1a1a1a)
- **Primary Text:** White (#ffffff)
- **Critical Alert:** Red (#ff3333)
- **Caution:** Amber (#ffaa00)
- **Nominal:** Green (#33ff66)
- **Inactive:** Gray (#666666)
- **Accent:** Cyan (#00ccff)

---

## 6. Audio Requirements

### 6.1 Required Sound Assets

| Category | Assets Needed | Source |
|----------|---------------|--------|
| **Ambient** | Mission control room tone, spacecraft hum | Freesound.org / custom |
| **Interface** | Button clicks, alert tones | NASA sound library + custom |
| **Launch** | Rocket ignition, ascent roar, stage sep | NASA audio archive |
| **Comms** | Radio static, ground station chatter | Processed voice + effects |
| **Music** | Mission themes (subtle, non-intrusive) | Original or royalty-free |

### 6.2 NASA Audio Resources

**Available:** https://www.nasa.gov/audio-and-ringtones/  
- Apollo mission audio
- Space Shuttle launch audio
- ISS ambient sound
- Mission control communications

---

## 7. Implementation Roadmap

### Phase 1: Asset Foundation (Week 1)
- [ ] Clone NASA-3D-Resources repository
- [ ] Import Landsat 8 model to UE
- [ ] Import Earth textures
- [ ] Set up material library
- [ ] Create test scene with imported assets
- [ ] Verify scales and collisions

### Phase 2: Earth & Orbit Enhancement (Week 1-2)
- [ ] Implement atmospheric scattering shader
- [ ] Add cloud layer
- [ ] Night lights texture
- [ ] Enhanced orbital visualization
- [ ] Trail rendering improvements

### Phase 3: Mission Structure (Week 2)
- [ ] Mission briefing screen (UMG)
- [ ] Objective display system
- [ ] Mission timer/phases
- [ ] Evaluation criteria framework

### Phase 4: Spacecraft Integration (Week 2-3)
- [ ] Replace primitive spacecraft with Landsat 8
- [ ] Engineering detail views
- [ ] Component highlighting system
- [ ] Instrument FOV visualization

### Phase 5: Launch Sequence (Week 3)
- [ ] Launch pad environment
- [ ] Simplified vehicle ascent
- [ ] Stage separation
- [ ] Payload deployment
- [ ] Orbit insertion

### Phase 6: Operations & Science (Week 3-4)
- [ ] Observation target system
- [ ] Imaging simulation
- [ ] Data downlink visualization
- [ ] Mission timeline
- [ ] Engineering constraints

### Phase 7: Polish & Integration (Week 4)
- [ ] UI/UX overhaul
- [ ] Sound design integration
- [ ] Cinematic cameras
- [ ] Tutorial/onboarding
- [ ] Performance optimization

---

## 8. Quality Acceptance Criteria

**Before each milestone is considered complete:**

1. **Visual Quality:** Must look professional, not prototype
2. **Engineering Accuracy:** Simulation values must match NASA-grade calculations
3. **Performance:** Maintain 60 FPS on RTX 3050 (target hardware)
4. **Integration:** All systems connect to unified mission state
5. **Playability:** Player can complete the mission without confusion
6. **Polish:** No obvious bugs, missing assets, or placeholder text

**Final Acceptance Test:** Show screenshots/video to someone unfamiliar with the project. Their reaction should be "This is a space mission game" NOT "What is this?"

---

## 9. Contingency Plans

### If NASA Assets Are Insufficient

**Backup Strategy 1:** Commission custom 3D models
- Hire freelance 3D artist for specific gaps
- Budget estimate: $200-500 per detailed spacecraft model

**Backup Strategy 2:** Use high-quality marketplace assets
- Unreal Marketplace sci-fi/aerospace kits
- Verify licensing for distribution

**Backup Strategy 3:** Simplified representative models
- Clearly labeled as "representative" not real spacecraft
- Prioritize simulation accuracy over visual fidelity

### If Timeline Is Too Aggressive

**Minimum Viable Product:**
- One spacecraft (Landsat 8)
- Enhanced Earth
- Basic mission briefing
- Simplified launch
- Functional orbital operations
- One complete mission playthrough

**Defer to Post-Competition:**
- Multiple mission scenarios
- Full mission complex environment
- Advanced UI polish
- Extensive cinematics

---

## 10. Asset File Organization

### Proposed Content Directory Structure

```
Content/
├── NASA/
│   ├── Spacecraft/
│   │   ├── Landsat8/
│   │   │   ├── SM_Landsat8.uasset
│   │   │   ├── M_Landsat8_Bus.uasset
│   │   │   ├── M_Landsat8_Panels.uasset
│   │   │   └── T_Landsat8_Diffuse.uasset
│   │   └── [Other satellites...]
│   ├── Planets/
│   │   └── Earth/
│   │       ├── T_Earth_8K.uasset
│   │       ├── T_Earth_Night.uasset
│   │       ├── M_Earth_Master.uasset
│   │       └── M_Earth_Atmosphere.uasset
│   └── Attribution.txt
├── Environments/
│   ├── MissionControl/
│   ├── SpacecraftLab/
│   └── LaunchComplex/
├── UI/
│   ├── Fonts/
│   ├── Icons/
│   ├── Widgets/
│   └── Materials/
├── Audio/
│   ├── Ambient/
│   ├── Effects/
│   ├── Launch/
│   └── Music/
└── ProjectTempest/
    ├── Maps/
    ├── Blueprints/
    └── Data/
```

---

## 11. Next Immediate Actions

**Priority 1 (Today):**
1. ✅ Complete this asset manifest
2. ⏳ Download NASA-3D-Resources repository
3. ⏳ Import Landsat 8 model to Unreal
4. ⏳ Import Earth textures
5. ⏳ Create test scene showing Landsat 8 in orbit around textured Earth
6. ⏳ Take screenshot to verify transformation from primitives to NASA assets

**Priority 2 (Next Session):**
1. Implement Mission Briefing screen
2. Replace orbit scene spacecraft with Landsat 8
3. Enhanced Earth material with atmosphere
4. Begin Mission Causality framework

---

## 12. Documentation & Tracking

- **Asset Manifest:** This document (living document, update as assets are integrated)
- **Attribution Log:** Track all external assets used
- **Quality Checklist:** Per-milestone acceptance criteria
- **Known Issues:** Track visual/technical debt

---

**Document Owner:** ASTRALIS Technical Direction  
**Review Cadence:** Update after each major milestone  
**Next Review:** After Phase 1 completion (NASA asset import)

---

## Appendix A: NASA Media Usage Guidelines Summary

**Key Points:**
- NASA content generally in public domain
- Attribution appreciated but not always required
- No NASA endorsement implied
- No misleading use
- Some images have restrictions (check individual files)
- Commercial use may require additional clearance

**Full Guidelines:** https://www.nasa.gov/nasa-brand-center/images-and-media/

---

## Appendix B: Technical Specifications - Landsat 8

**Mission:** Earth Observation (Land/Water)  
**Launch:** 2013  
**Orbit:** Sun-synchronous LEO, ~705 km altitude  
**Instruments:**
- OLI (Operational Land Imager)
- TIRS (Thermal Infrared Sensor)

**Physical:**
- Mass: ~2,780 kg
- Dimensions: ~3m × 3m × 4m (approximate)
- Power: Solar arrays ~4 kW
- Design life: 5 years (exceeded)

**Mission Profile:** Matches PROJECT TEMPEST perfectly - Earth surface observation including flood monitoring.

**Reference:** https://landsat.gsfc.nasa.gov/satellites/landsat-8/

---

*End of Asset Manifest*
