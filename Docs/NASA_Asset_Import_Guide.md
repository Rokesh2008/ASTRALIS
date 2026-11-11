# NASA Asset Import Guide - Step-by-Step

**Purpose:** Replace primitive cubes with professional NASA spacecraft and Earth texture  
**Time Required:** 15-20 minutes  
**Prerequisites:** Unreal Editor closed, assets downloaded to `Content/NASA_Import/`

---

## Part 1: Import Assets to Unreal

### Step 1: Launch Unreal Editor

1. Open `game1.uproject`
2. Wait for engine to fully load
3. **Close any "Live Coding" prompts** - we need a clean import

### Step 2: Create Content Folder Structure

1. In **Content Browser**, right-click in root
2. **New Folder** → `NASA`
3. Inside NASA, create:
   - **Spacecraft** folder
   - **Planets** folder
   - **Materials** folder

Your structure:
```
Content/
└── NASA/
    ├── Spacecraft/
    ├── Planets/
    └── Materials/
```

### Step 3: Import Landsat 8 Spacecraft

1. Navigate to `Content/NASA/Spacecraft/` in Content Browser
2. **Right-click** → **Import to /Game/NASA/Spacecraft/**
3. Browse to: `D:/Game/game1/Content/NASA_Import/Spacecraft/Landsat8.glb`
4. **Import Settings:**
   - ✅ Import Mesh
   - ✅ Import Materials
   - ✅ Import Textures
   - ✅ Skeletal Mesh = FALSE (static mesh only)
   - ✅ Combine Meshes = TRUE
   - ✅ Import Uniform Scale = 100.0 (GLB is in meters, UE is cm)
5. Click **Import All**
6. Wait for import to complete

**Expected Results:**
- `SM_Landsat8` (Static Mesh)
- Multiple materials (M_Landsat8_...)
- Textures embedded in materials

### Step 4: Verify Landsat 8 Import

1. **Double-click** `SM_Landsat8` to open Static Mesh Editor
2. Check dimensions: Should be roughly 300-400 cm (3-4 meters real size)
3. Verify materials applied
4. Look for:
   - Solar panels (should be dark blue/black)
   - Spacecraft bus (metallic gray/silver)
   - Instruments (various colors)
5. If mesh is HUGE or TINY:
   - Re-import with different uniform scale
   - NASA models are typically 1:1 meter scale
   - UE expects centimeters
   - Scale = 100 converts meters to cm

### Step 5: Import Earth Texture

1. Navigate to `Content/NASA/Planets/` in Content Browser
2. **Right-click** → **Import**
3. Browse to: `D:/Game/game1/Content/NASA_Import/Textures/Earth_A.jpg`
4. **Import Settings:**
   - Texture Group: World
   - sRGB: TRUE
   - Compression: Default
   - Mip Gen Settings: FromTextureGroup
5. Click **Import**
6. Texture named: `T_Earth_A`

### Step 6: Create Enhanced Earth Material

1. In `Content/NASA/Materials/`, **right-click** → **Material**
2. Name: `M_Earth_Enhanced`
3. **Double-click** to open Material Editor
4. Build this material graph:

**Material Blueprint:**
```
[T_Earth_A] → [Multiply by 1.2] → Base Color
                                 ↓
                          [Power 1.8] → Emissive Color (for day side glow)

Roughness: 0.8 (mostly diffuse)
Metallic: 0.0 (not metal)
Specular: 0.3 (slight highlight on oceans)
```

**Detailed Steps:**
- Drag `T_Earth_A` texture into graph
- Connect to **Base Color**
- Add **Multiply** node (value 1.2) to brighten
- Add **Power** node (value 1.8) → **Emissive Color** (subtle glow)
- Set **Roughness** to constant 0.8
- Set **Metallic** to constant 0.0
- **Apply** and **Save**

### Step 7: Test Assets in Editor

1. Create new test map: **File → New Level → Empty Level**
2. Save as: `Content/Maps/MAP_NASA_AssetTest`
3. Add **Sky Light** (for ambient lighting)
4. Add **Directional Light** (as sun)
5. Drag `SM_Landsat8` into viewport
6. Scale to 1.0, 1.0, 1.0 (should be reasonable size)
7. Create a sphere (BSP or static mesh)
8. Apply `M_Earth_Enhanced` to sphere
9. Position camera to view both
10. **Play** to verify visuals

**Expected:** Professional-looking spacecraft and textured Earth sphere.

---

## Part 2: Update C++ Code to Use NASA Assets

Now we modify the orbit scene actor to reference the imported assets instead of creating primitives.

### Changes Required:

**File:** `Source/Astralis/Orbit/AstralisOrbitSceneActor.h`

Add asset reference properties:

```cpp
// Near the top with other properties
UPROPERTY(EditDefaultsOnly, Category = "NASA Assets")
TObjectPtr<UStaticMesh> LandsatSpacecraftMesh;

UPROPERTY(EditDefaultsOnly, Category = "NASA Assets")
TObjectPtr<UMaterialInterface> EarthMaterial;
```

**File:** `Source/Astralis/Orbit/AstralisOrbitSceneActor.cpp`

Replace primitive loading with asset references:

**BEFORE (BeginPlay):**
```cpp
// Earth sphere — engine sphere is 100 UU diameter (radius 50)
UStaticMesh* Sphere = LoadObject<UStaticMesh>(nullptr,
    TEXT("/Engine/BasicShapes/Sphere.Sphere"));

if (Sphere && EarthMesh)
{
    EarthMesh->SetStaticMesh(Sphere);
    // ... scaling code ...
}
```

**AFTER (BeginPlay):**
```cpp
// Earth sphere with NASA texture
UStaticMesh* Sphere = LoadObject<UStaticMesh>(nullptr,
    TEXT("/Engine/BasicShapes/Sphere.Sphere"));

if (Sphere && EarthMesh)
{
    EarthMesh->SetStaticMesh(Sphere);
    
    // Scale for 6371 km Earth radius at orbital scene scale
    const double EarthUE = EarthRadius_m * OrbitScale;
    const double SphereScale = EarthUE / 50.0;
    EarthMesh->SetWorldScale3D(FVector(SphereScale));

    // Apply NASA Earth material if set, otherwise try to load
    UMaterialInterface* EarthMat = EarthMaterial;
    if (!EarthMat)
    {
        EarthMat = LoadObject<UMaterialInterface>(nullptr,
            TEXT("/Game/NASA/Materials/M_Earth_Enhanced.M_Earth_Enhanced"));
    }
    if (EarthMat)
    {
        EarthMesh->SetMaterial(0, EarthMat);
    }
}
```

**BEFORE (Spacecraft):**
```cpp
// Spacecraft visual — small bright cube
UStaticMesh* Cube = LoadObject<UStaticMesh>(nullptr,
    TEXT("/Engine/BasicShapes/Cube.Cube"));
if (Cube && SpacecraftMesh)
{
    SpacecraftMesh->SetStaticMesh(Cube);
    SpacecraftMesh->SetWorldScale3D(FVector(0.5));
    // ... material code ...
}
```

**AFTER (Spacecraft):**
```cpp
// Spacecraft visual — NASA Landsat 8
UStaticMesh* SCMesh = LandsatSpacecraftMesh;
if (!SCMesh)
{
    // Fallback to load from content if not set in blueprint
    SCMesh = LoadObject<UStaticMesh>(nullptr,
        TEXT("/Game/NASA/Spacecraft/Landsat8.Landsat8"));
}

if (SCMesh && SpacecraftMesh)
{
    SpacecraftMesh->SetStaticMesh(SCMesh);
    
    // Landsat 8 is ~3m, at orbit scale of 1e-5 that's 0.03 cm
    // Scale up for visibility at orbital distances
    SpacecraftMesh->SetWorldScale3D(FVector(2.0, 2.0, 2.0));
    
    // Materials should come with the imported model
    // If needed, can override specific material slots here
}
```

---

## Part 3: Blueprint Setup (After Code Changes)

### Step 1: Compile C++ Changes

1. **Close Unreal Editor**
2. **Build project** (Hot Reload won't work with new properties)
3. Re-open Editor

### Step 2: Set Asset References

1. Find `BP_OrbitScene` blueprint (or create from `AstralisOrbitSceneActor`)
2. **Select** the blueprint
3. In **Details** panel, find **NASA Assets** category:
   - **Landsat Spacecraft Mesh**: Drag `SM_Landsat8` from Content Browser
   - **Earth Material**: Drag `M_Earth_Enhanced` from Content Browser
4. **Compile** and **Save**

### Step 3: Update GameMode

The `AstralisAssemblyGameMode::LaunchToOrbit()` already spawns the orbit scene. No changes needed unless you want to explicitly set the blueprint class.

### Step 4: Test Complete Integration

1. **Play** the assembly level
2. Place some components (spacecraft design)
3. **Press F5** to launch to orbit
4. **Verify:**
   - ✅ Landsat 8 appears instead of cube
   - ✅ Earth has NASA texture
   - ✅ Orbit trail still draws
   - ✅ Telemetry panel updates
   - ✅ Time warp works (Period/Comma)
   - ✅ FPS stays at 60+
5. **Take screenshot** for before/after comparison

---

## Part 4: Material Enhancements (Optional but Recommended)

### Emissive Spacecraft Components

Make active instruments glow:

1. Open `SM_Landsat8` Static Mesh
2. Identify material slots (instruments, solar panels, bus)
3. For instrument materials:
   - Add **Emissive Color** (subtle cyan glow)
   - Strength: 0.5-1.0
   - Makes spacecraft feel "alive"

### Atmospheric Scattering

Enhance Earth material:

1. Open `M_Earth_Enhanced`
2. Add **Fresnel** node:
   ```
   [Fresnel (exp 3)] → [Multiply by blue color (0.3, 0.5, 1.0)] → Add to Emissive
   ```
3. Creates blue atmospheric glow at planet limb
4. Makes Earth look more photorealistic

### Solar Panel Reflectivity

1. Find solar panel material slots in Landsat 8
2. Set:
   - Metallic: 0.8
   - Roughness: 0.2
   - Base Color: Dark blue-black
3. Creates realistic solar cell reflection

---

## Part 5: Verification Checklist

Before proceeding to Phase 2:

- [ ] Landsat 8 imports without errors
- [ ] Earth texture applied to sphere
- [ ] Spacecraft mesh appears in orbit scene
- [ ] Earth has NASA texture
- [ ] Simulation runs at 60 FPS
- [ ] Time warp still functions
- [ ] Orbit trail draws correctly
- [ ] No console errors related to assets
- [ ] Screenshot taken showing transformation
- [ ] Git commit: "Integrate NASA Landsat 8 and Earth texture"

---

## Troubleshooting

### Issue: Landsat 8 is Invisible

**Solution:**
- Check mesh actually assigned to `SpacecraftMesh` component
- Verify scale isn't too small (increase scale factor)
- Check materials aren't fully transparent
- Use **Show → Bounds** in viewport to see mesh bounding box

### Issue: Earth Texture is Blurry

**Solution:**
- Re-import with higher resolution if available
- Check mip map settings (ensure not over-compressed)
- Adjust material roughness
- Verify sRGB is enabled on texture

### Issue: Spacecraft is Gigantic

**Solution:**
- Verify import scale was correct (100 for meter→cm)
- Adjust `SetWorldScale3D` in code
- NASA model might be in different units

### Issue: Performance Drop

**Solution:**
- Check LODs generated on import
- Reduce texture resolution
- Disable real-time lighting if needed
- Use texture streaming

### Issue: Materials Look Wrong

**Solution:**
- Inspect imported materials in Content Browser
- May need to manually adjust PBR values
- Check if textures embedded correctly
- Re-import with different settings if needed

---

## Expected Outcome

**Before:** Cube spacecraft orbiting blue sphere  
**After:** NASA Landsat 8 orbiting photorealistic Earth with texture

**Visual Quality Leap:**
- Professional spacecraft model
- Recognizable satellite design
- Textured planetary body
- Foundation for further polish

**Next Phase:** Mission briefing system (Phase 2)

---

## Quick Reference: File Paths

```
Content/NASA/
├── Spacecraft/
│   └── Landsat8.Landsat8 (Static Mesh)
├── Planets/
│   └── T_Earth_A (Texture)
└── Materials/
    └── M_Earth_Enhanced (Material)

Source/Astralis/Orbit/
├── AstralisOrbitSceneActor.h (add properties)
└── AstralisOrbitSceneActor.cpp (update BeginPlay)
```

---

## Git Commit Message Template

```
Integrate NASA Landsat 8 spacecraft and Earth texture

- Import Landsat 8 GLB model from NASA 3D Resources
- Import Earth texture from NASA image library
- Update AstralisOrbitSceneActor to use real assets
- Replace primitive cube with NASA spacecraft model
- Apply photorealistic Earth texture
- Add material enhancements for realism
- Maintain 60 FPS performance target

Assets courtesy of NASA (see NASA_Attribution.txt)

Co-Authored-By: Claude Opus 4.6 <noreply@anthropic.com>
```

---

**Document Status:** Ready for execution  
**Estimated Time:** 20 minutes hands-on work  
**Next Step:** Open Unreal Editor and begin imports
