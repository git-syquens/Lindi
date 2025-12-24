# Level Menu UI/Logic Spec (320×240 Landscape)

## Purpose
Non-scrolling **Level** view used as a stabilisation tool when parking an **alkoof camper**. It monitors and displays:
- **Pitch** (front-to-back tilt)
- **Roll** (side-to-side tilt)

Navigation bar exists above this view (persistent). This document defines the **Level view** only.

---

## Display & Layout Constraints
- Display: **320×240**, **landscape**.
- Top navigation region: **~20 px height** reserved (persistent). Level content must fit below.
- Level view must be **non-scrolling**.
- Two main visuals are **slightly smaller** than full-height to leave **room below** for other stats.

---

## Screen Composition (Level View)
Content region (below nav) contains:

1. **Pitch Panel (left side)**
   - **Side-view outline** of the camper.
   - Camper body **rotates to match pitch** (see conventions below).
   - **Ground/contact line** under wheels.
   - **Horizontal slider bar** below the visual.
     - Slider has a **center (0°) marker**.
     - Slider knob position corresponds to pitch magnitude/sign.
     - Degree label below knob shows current pitch in degrees.

2. **Roll Panel (right side)**
   - **Rear-view outline** of the camper.
   - Camper body **tilts to match roll**.
   - **Ground/contact line** under wheels.
   - **Horizontal slider bar** below the visual.
     - Slider has a **center (0°) marker**.
     - Slider knob position corresponds to roll magnitude/sign.
     - Degree label below knob shows current roll in degrees.

3. **Icon Row (below panels)**
   - Small icons (monochrome) that change color/state:
     - **Up arrow** = acceleration
     - **Down arrow** = deceleration
   - Icons are **grey when inactive**, **green when active**.

4. **Reserved Stats Area**
   - Leave a horizontal strip at the bottom (below icon row) for additional stats (not specified yet).

---

## Critical Design Rules (Must-Haves)

### A) Sign Conventions & Direction Labels
**Pitch convention:**
- **Positive pitch = Front up (nose high)**.
- Slider direction labels under pitch slider:
  - Left end: `Rear ↓`
  - Right end: `Front ↑`
- If pitch is positive:
  - Camper side-view must show **front higher** than rear.
  - Slider knob must move toward **Front ↑** side.
  - Degree label should preferably show a `+` sign (e.g., `+4.5°`) OR keep sign-less but direction labels must remain.

**Roll convention:**
- Roll is displayed using rear view.
- Slider direction labels under roll slider:
  - Left end: `Left ↑`
  - Right end: `Right ↑`
- If roll is positive toward one side:
  - Camper rear-view must tilt so the corresponding side is higher.
  - Slider knob must move toward that labeled side.

**Consistency rule:** Numeric value, camper tilt, and slider knob offset must always agree.

### B) Wheel–Surface Contact Constraint (Hard Constraint)
In **all** camper visuals (side and rear):
- **Wheels must always contact the ground/contact line**.
- Wheels must **never** float above the line and must **never** intersect through it.
- The ground line must remain a stable reference; do not rotate it with the camper.
- Rotation must be implemented so the camper appears to be **resting on its wheels** against the contact surface.

### C) Visual Clamping vs Numeric Range
- Numeric pitch/roll may show the true measured value.
- Visual rotation and slider offset may be **clamped** to keep the UI readable.
- Use the **same clamped mapping** for camper rotation and slider offset, while still showing the true numeric value (or show clamped value if you prefer—but be consistent).

### D) Performance/Refresh Behavior
- Avoid full-screen redraws; update only changing elements.
- UI should be stable: rate-limit updates (e.g., 5–10 Hz) and apply filtering/hysteresis to prevent jitter.
- Background/frames static; only tilt/slider knob/labels/icons update.

---

## Data & Logic Model

### Inputs
- `pitch_deg` (float): front-to-back tilt in degrees. **Positive = front up**.
- `roll_deg` (float): side-to-side tilt in degrees. **Positive direction must match the on-screen Left↑/Right↑ labeling**.
- `accel_state` (bool/enum): acceleration active.
- `decel_state` (bool/enum): deceleration active.

### Processing
- Filter pitch/roll (e.g., low-pass).
- Apply dead zone/tolerance and hysteresis as needed (optional but recommended for stability).
- Map pitch/roll to:
  - camper visual rotation (clamped)
  - slider knob x-offset (clamped)
  - numeric label text

### Outputs
- Pitch panel: rotated camper outline + slider knob position + degree label.
- Roll panel: tilted camper outline + slider knob position + degree label.
- Icon row: accel/decel icons grey/green based on state.

---

## Implementation Notes (Practical)
- Keep object tree separation: nav is outside Level view; Level view updates must not invalidate nav.
- Use consistent pixel grids; ensure both panels align and leave reserved space below for future stats.

---

# Concise Prompt for VSCode/Claude

Implement the **Level** menu (non-scrolling) for a **320×240 landscape** embedded display with a **~20 px top nav reserved**. In the Level content region, create two panels: **left = Pitch**, **right = Roll**. Pitch panel shows a **side-view outline of an alkoof camper** that **rotates with pitch**; roll panel shows a **rear-view outline** that **tilts with roll**. Under each visual, render a **horizontal slider** with a **center 0° marker**, end labels (**Pitch:** `Rear ↓` left, `Front ↑` right; **Roll:** `Left ↑` left, `Right ↑` right), a **slider knob** whose x-offset matches the same signed/clamped mapping as the camper tilt, and a **degree label** under the knob (pitch should display `+` for positive if possible). **Hard constraint:** in both visuals, **wheels must always contact the ground/contact line** (no floating/clipping); ground line stays fixed. Below panels, add a row of small icons: **up arrow (acceleration)** and **down arrow (deceleration)**; icons are **grey when inactive** and **green when active**. Make the camper visuals slightly smaller to leave a **reserved bottom strip** for future stats. Update only dynamic elements (no full-screen redraw), rate-limit/low-pass filter to prevent jitter. Inputs: `pitch_deg` (positive=front up), `roll_deg`, `accel_state`, `decel_state`.
