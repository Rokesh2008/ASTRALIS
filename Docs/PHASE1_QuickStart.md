# PHASE 1 QUICK START - NASA Asset Integration

**Time Required:** 10-15 minutes in Unreal Editor  
**Result:** Professional spacecraft and Earth replace primitives

---

## ✅ COMPLETED (Code Ready)

- [x] NASA Landsat 8 downloaded (762KB)
- [x] NASA Earth texture downloaded (1.4MB)
- [x] C++ code updated to support NASA assets
- [x] Project compiles successfully
- [x] Fallback logic implemented

---

## 🎯 YOUR TASKS (In Unreal Editor)

### Task 1: Import Landsat 8 (5 min)

1. **Open** `game1.uproject` in Unreal Editor
2. **Content Browser** → Create folder: `Content/NASA/Spacecraft/`
3. **Right-click** in that folder → **Import**
4. **Select:** `D:/Game/game1/Content/NASA_Import/Spacecraft/Landsat8.glb`
5. **Import Settings:**
   - ✅ Import Mesh = TRUE
   - ✅ Import Materials = TRUE
   - ✅ Skeletal Mesh = FALSE
   - ✅ Combine Meshes = TRUE
   - ✅ Import Uniform Scale = **100.0** (critical - converts meters to cm)
6. **Import All**
7. **Verify:** See `Landsat8` static mesh and materials

### Task 2: Import Earth Texture (2 min)

1. **Content Browser** → Create folder: `Content/NASA/Planets/`
2. **Right-click** → **Import**
3. **Select:** `D:/Game/game1/Content/NASA_Import/Textures/Earth_A.jpg`
4. **Import Settings:** Leave defaults (sRGB=TRUE)
5. **Import**
6. **Verify:** See `Earth_A` texture

### Task 3: Create Earth Material (3 min)

1. **Content Browser** → Create folder: `Content/NASA/Materials/`
2. **Right-click** → **Material**
3. **Name:** `M_Earth_Enhanced`
4. **Double-click** to open
5. **Drag** `Earth_A` texture into graph
6. **Connect:**
   - Texture → **Base Color**
   - Texture → **Emissive Color** (subtle glow)
7. **Set Constants:**
   - Roughness: 0.8
   - Metallic: 0.0
8. **Apply** and **Save**

### Task 4: Test in Orbit Scene (5 min)

1. **Play** the game (Click Play button)
2. **Press F5** to launch to orbit
3. **Expected Results:**
   - Landsat 8 spacecraft visible (not cube)
   - Earth has NASA texture (not solid blue)
   - Simulation runs smoothly
   - Time warp works (Period/Comma keys)
4. **Take Screenshot** (F9 key)
5. **Compare** with previous cube-based version

---

## 🔍 VERIFICATION CHECKLIST

After completing tasks, verify:

- [ ] Landsat 8 static mesh exists in `Content/NASA/Spacecraft/`
- [ ] Earth_A texture exists in `Content/NASA/Planets/`
- [ ] M_Earth_Enhanced material created
- [ ] Orbit scene shows spacecraft (not cube)
- [ ] Earth shows texture (not solid color)
- [ ] FPS counter shows 60+ (console: `stat fps`)
- [ ] No red errors in Output Log
- [ ] Screenshot captured

---

## 🚨 TROUBLESHOOTING

### "Can't find Landsat8 asset"
→ Check import path: `/Game/NASA/Spacecraft/Landsat8`  
→ Verify file actually imported (see in Content Browser)  
→ Try restarting editor

### "Spacecraft is invisible"
→ Check asset actually assigned in Details panel  
→ Verify scale isn't zero  
→ Enable `Show → Bounds` in viewport

### "Earth texture is solid color"
→ Verify material actually applied to sphere  
→ Check texture imported with sRGB=TRUE  
→ Rebuild material node connections

### "Performance is slow"
→ Check texture resolution (might be too high)  
→ Enable texture streaming  
→ Generate LODs for spacecraft mesh

---

## 📸 SCREENSHOT COMPARISON

**BEFORE (Primitives):**
```
[Cube spacecraft]
[Blue sphere Earth]
[Basic trail]
```

**AFTER (NASA Assets):**
```
[Landsat 8 satellite]
[Textured Earth with continents]
[Same physics simulation]
```

---

## 🎯 SUCCESS CRITERIA

**You're done when:**

1. Spacecraft looks like a real satellite (not a cube)
2. Earth shows continents and oceans (not solid blue)
3. Game runs smoothly (60 FPS)
4. Physics simulation unchanged (orbit trail, telemetry work)

---

## ⏭️ WHAT'S NEXT (Phase 2)

After Phase 1 complete:
- Mission briefing screen
- Scientific objectives
- Observation target system
- Mission evaluation

**Don't proceed until Phase 1 verified!**

---

## 📝 NOTES

- NASA assets load via fallback system - will work even if blueprint not set
- Spacecraft scale may need tweaking based on visual preference
- Materials from NASA model are auto-applied
- Original primitive scene preserved as fallback

---

## 🆘 IF STUCK

1. Check Output Log for errors
2. Verify import settings (especially scale=100)
3. Restart Unreal Editor
4. Review full guide: `NASA_Asset_Import_Guide.md`
5. Check that downloaded files exist in `NASA_Import/` folders

---

**Estimated Time:** 15 minutes hands-on  
**Difficulty:** Easy (mostly imports)  
**Reward:** Massive visual quality improvement  
**Status:** Ready to execute
