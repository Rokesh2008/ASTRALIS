# NASA Asset Import - Step by Step Instructions

**Build Complete!** Now follow these steps exactly.

---

## STEP 1: Open Unreal Editor

Open `game1.uproject` and wait for it to fully load.

---

## STEP 2: Import Landsat 8 Spacecraft

### 2.1 Navigate to Import Location
1. **Content Browser** (bottom panel)
2. Click **"Content"** folder
3. Find the **"NASA"** folder → Open it
4. Click **"Spacecraft"** folder (currently empty)

### 2.2 Start Import
1. **Right-click** inside the Spacecraft folder (empty space)
2. Select **"Import to /Game/NASA/Spacecraft/"**

### 2.3 Select Source File
1. File browser opens
2. Navigate to: `D:\Game\game1\Content\NASA_Import\Spacecraft\`
3. Select: **Landsat8.glb**
4. Click **Open**

### 2.4 Import Settings Dialog
**This is critical - set these settings:**

1. **Skeletal Mesh:** ❌ UNCHECK (we want Static Mesh)
2. **Import Meshes:** ✅ CHECK
3. **Import Materials:** ✅ CHECK
4. **Import Textures:** ✅ CHECK
5. **Combine Meshes:** ✅ CHECK
6. **Transform → Import Uniform Scale:** **100.0** ← TYPE THIS NUMBER!

### 2.5 Import
1. Click **"Import All"** button
2. **Wait 5-10 seconds** (watch bottom-right for progress)
3. **Look for assets appearing** in the Spacecraft folder

### 2.6 Verify Success
**You should now see:**
- `Landsat8` (Static Mesh icon - gray 3D cube symbol)
- Multiple materials (sphere icons)
- Maybe some textures

**If you see these = SUCCESS! Continue.**
**If folder still empty = Import failed, tell me.**

---

## STEP 3: Import Earth Texture

### 3.1 Navigate
1. **Content Browser** → Back to **"NASA"** folder
2. Click **"Planets"** folder

### 3.2 Import
1. **Right-click** in Planets folder
2. **Import to /Game/NASA/Planets/**
3. Navigate to: `D:\Game\game1\Content\NASA_Import\Textures\`
4. Select: **Earth_A.jpg**
5. Click **Open**

### 3.3 Settings
- Default settings are fine for textures
- Click **Import**

### 3.4 Verify
**You should see:** `Earth_A` (texture icon - checkerboard pattern)

---

## STEP 4: Create Earth Material

### 4.1 Navigate
1. **Content Browser** → **"NASA"** → **"Materials"** folder

### 4.2 Create Material
1. **Right-click** in Materials folder
2. **Material** → **Material** (not Material Instance)
3. Name it: **M_Earth_Enhanced**
4. Press Enter

### 4.3 Open Material Editor
1. **Double-click** `M_Earth_Enhanced`
2. Material Editor opens (node graph)

### 4.4 Add Texture
1. **From Content Browser** (you can see it while Material Editor is open)
2. Navigate to `Content/NASA/Planets/`
3. Find `Earth_A` texture
4. **DRAG it** into the Material Editor graph

### 4.5 Connect Texture
1. The `Earth_A` node appears in the graph
2. **Drag from the white pin (RGB output)** on Earth_A
3. **Connect to "Base Color"** input on the main node (right side)
4. **Drag from the same white pin again**
5. **Connect to "Emissive Color"** input (gives subtle glow)

### 4.6 Set Material Properties
**On the right side panel (Details):**
1. Find **"Roughness"** → Set to **0.8**
2. Find **"Metallic"** → Set to **0.0**

### 4.7 Save
1. **Click "Apply"** button (toolbar, top-left)
2. **Click "Save"** button
3. **Close** Material Editor

---

## STEP 5: Test Import Success

### Quick Visual Test:
1. **Content Browser** → Find `Landsat8` static mesh
2. **DRAG it** into the viewport (3D view)
3. **What happens?**
   - ✅ **Spacecraft appears** = Import worked!
   - ❌ **Error or nothing** = Import failed
4. **Delete** the test spacecraft from viewport (select it, press Delete)

---

## STEP 6: Test In-Game

### 6.1 Play
1. Click **Play** button (top toolbar)
2. Game starts (you'll be in assembly lab)

### 6.2 Open Console
1. Press **`~`** key (tilde, top-left of keyboard, below Esc)
2. Console opens at bottom of screen

### 6.3 Launch Command
1. Type: **astralis.launch**
2. Press **Enter**

### 6.4 What Should Happen:
- Camera moves to orbit view
- You should see Landsat 8 spacecraft (not cube!)
- Earth sphere with texture (not solid blue!)

### 6.5 Check Output Log
1. **Window** menu → **Developer Tools** → **Output Log**
2. Look for these messages:
   ```
   === AstralisOrbitSceneActor::BeginPlay ===
   Spacecraft: Trying to load /Game/NASA/Spacecraft/Landsat8
   Spacecraft: Mesh loaded: Landsat8
   Earth: Trying to load /Game/NASA/Materials/M_Earth_Enhanced
   Earth: Material applied: M_Earth_Enhanced
   ```

---

## STEP 7: Report Back

**Tell me:**

**A) Import Status:**
- [ ] Landsat8 assets appeared in Content/NASA/Spacecraft/?
- [ ] Earth_A texture appeared in Content/NASA/Planets/?
- [ ] M_Earth_Enhanced material created in Content/NASA/Materials/?

**B) Visual Test:**
- [ ] Dragging Landsat8 into viewport showed spacecraft (not error)?

**C) In-Game Test:**
- [ ] Console command `astralis.launch` worked?
- [ ] Orbit scene appeared?
- [ ] Spacecraft is Landsat 8 (or still cube)?
- [ ] Earth has texture (or still solid blue)?

**D) Output Log:**
- Copy/paste any lines that start with "Spacecraft:" or "Earth:"

---

## IF SOMETHING FAILS:

### Import Dialog Never Appeared:
→ Try: Right-click → **"Import Asset"** instead of "Import to..."

### Landsat8 Import Creates No Assets:
→ GLB format issue. Tell me, I'll convert to FBX or OBJ.

### Dragging Landsat8 Shows Error:
→ Import corrupted. Delete and retry with scale=100.0

### `astralis.launch` Does Nothing:
→ Console command not registered. Check Output Log for errors.

### Still See Cubes in Orbit:
→ Assets imported but paths wrong. I'll fix the C++ code paths.

---

**Start with STEP 1 now. Tell me when you complete each step or if anything goes wrong!**
