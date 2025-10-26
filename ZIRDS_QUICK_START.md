# ZIRDS Quick Start Guide

## 🎯 Zoppler Integrated Radar Display System
**© 2025 Zoppler Systems. All rights reserved.**

---

## What's New in ZIRDS Configuration

### ✅ Application Rebranded
- **New Name**: ZIRDS (Zoppler Integrated Radar Display System)
- **Company**: Zoppler Systems
- **Copyright**: © Zoppler Systems displayed throughout

### ✅ Startup Behavior Changed
1. **Windows Maximize on Startup**
   - PPI Display window: **FULL SCREEN / MAXIMIZED**
   - Control Center window: **FULL SCREEN / MAXIMIZED**

2. **PPI as Welcome Window**
   - PPI Display appears **ON TOP** when application starts
   - Serves as the primary welcome interface
   - User sees radar display first

3. **Dual Monitor Support**
   - Automatically detects multiple monitors
   - PPI Display → Monitor 1 (maximized)
   - Control Center → Monitor 2 (maximized)
   - PPI stays on front after arrangement

### ✅ Zoppler Logo Integration
- Splash screen shows Zoppler logo
- All windows display Zoppler logo in title bar
- Professional branding throughout application

---

## Quick Build & Run

```bash
# Navigate to project directory
cd /workspace

# Build the application
qmake RadarDisplay.pro
make -j$(nproc)

# Run ZIRDS
./bin/RadarDisplay
```

---

## What You'll See on Startup

### 1. Splash Screen (3 seconds)
```
┌─────────────────────────────────┐
│                                 │
│      [ZOPPLER LOGO IMAGE]       │
│                                 │
│           Z I R D S             │
│                                 │
│  Zoppler Integrated Radar       │
│     Display System              │
│                                 │
│   © Zoppler Systems             │
│                                 │
└─────────────────────────────────┘
```

### 2. Windows Launch (Both Maximized)
```
SINGLE MONITOR:
┌─────────────────────────────────────────────┐
│ 🎯 ZIRDS - PPI Display | © Zoppler Systems │ ← ON TOP
│─────────────────────────────────────────────│
│                                             │
│    [MAP CANVAS WITH RADAR TRACKS]           │
│                                             │
│           [TRACK TABLE]                     │
│                                             │
└─────────────────────────────────────────────┘

DUAL MONITOR:
Monitor 1:                    Monitor 2:
┌──────────────────────┐     ┌──────────────────────┐
│ PPI Display          │     │ Control Center       │
│ [MAP & TRACKS]       │     │ [CONFIG & CONTROLS]  │
│                      │     │                      │
│ ← ON TOP (FRONT)     │     │ Behind PPI initially │
└──────────────────────┘     └──────────────────────┘
```

---

## Window Titles

All windows now display ZIRDS branding:

1. **PPI Display Window**
   ```
   🎯 ZIRDS - PPI Display | © Zoppler Systems
   ```

2. **Control Center Window**
   ```
   🎛️ ZIRDS - Control Center | © Zoppler Systems
   ```

3. **Window Manager**
   ```
   🎯 ZIRDS - Window Manager
   ```

---

## Key Features

### Startup Configuration
- ✅ Both windows start **MAXIMIZED**
- ✅ PPI window on **FRONT** (welcome screen)
- ✅ Automatic dual monitor detection
- ✅ Proper window stacking order
- ✅ Professional splash screen

### Branding
- ✅ Application name: ZIRDS
- ✅ Organization: Zoppler Systems
- ✅ Copyright notices in:
  - Splash screen
  - Window titles
  - Debug output
  - About information

### Logo Integration
- ✅ Splash screen shows Zoppler logo
- ✅ Window icons display logo
- ✅ Fallback text branding if logo missing

---

## Keyboard Shortcuts

### Essential Shortcuts
- **F1**: Show PPI Display
- **F2**: Show Control Center
- **F3**: Arrange for dual monitors
- **F4**: Toggle fullscreen
- **ESC**: Exit fullscreen

---

## Replacing the Placeholder Logo

**IMPORTANT**: The application currently uses a placeholder logo.

### To use the actual Zoppler logo:

1. **Save the logo image** (from conversation) as:
   ```bash
   /workspace/resources/zoppler_logo.png
   ```

2. **Rebuild**:
   ```bash
   make clean
   qmake RadarDisplay.pro
   make -j$(nproc)
   ```

3. **Run and verify**:
   ```bash
   ./bin/RadarDisplay
   ```

### Logo Specifications
- Format: PNG with transparent background
- Size: 600x600 pixels (recommended)
- Path: `/workspace/resources/zoppler_logo.png`

---

## Testing Checklist

Verify the following on startup:

- [ ] Splash screen displays for 3 seconds
- [ ] Splash screen shows "ZIRDS" and "© Zoppler Systems"
- [ ] PPI Display window opens maximized
- [ ] Control Center window opens maximized
- [ ] PPI Display is on top (front window)
- [ ] Window titles show "ZIRDS" and copyright
- [ ] Window icons display Zoppler logo (if replaced)
- [ ] Dual monitor arrangement works (if applicable)

---

## Files Modified

### Source Code
1. `main.cpp` - Application properties, splash screen, branding
2. `cmapmainwindow.cpp` - Window maximization, stacking order
3. `cppiwindow.cpp` - Window title, icon
4. `ccontrolswindow.cpp` - Window title, icon

### Resources
1. `myRes.qrc` - Added zoppler_logo.png reference
2. `resources/zoppler_logo.png` - Logo file (placeholder)

### Documentation
1. `ZIRDS_CONFIGURATION_SUMMARY.md` - Complete change documentation
2. `ZIRDS_LOGO_SETUP.md` - Logo setup instructions
3. `BUILD_INSTRUCTIONS.md` - Build and run instructions
4. `ZIRDS_QUICK_START.md` - This file

---

## Version Information

- **Application**: ZIRDS (Zoppler Integrated Radar Display System)
- **Version**: v0.1
- **Build Date**: 23rd June 2025
- **Copyright**: © 2025 Zoppler Systems
- **Website**: zoppler.com

---

## Need Help?

### Documentation Files
- `ZIRDS_CONFIGURATION_SUMMARY.md` - Detailed change log
- `BUILD_INSTRUCTIONS.md` - Complete build guide
- `ZIRDS_LOGO_SETUP.md` - Logo replacement guide

### Debug Output
On startup, ZIRDS prints:
```
🎯 ZIRDS - Zoppler Integrated Radar Display System
📦 APP VERSION: v0.1
📅 APP BUILT DATE: 23rd June 2025
©️  COPYRIGHT: Zoppler Systems
🖥️  DUAL MONITOR SUPPORT: ENABLED
🚀 ZIRDS started successfully!
```

---

**Congratulations! ZIRDS is now configured with:**
- ✅ Full screen maximized windows on startup
- ✅ PPI Display as welcome window (on front)
- ✅ Professional Zoppler Systems branding
- ✅ Copyright notices throughout
- ✅ Logo integration support

*© 2025 Zoppler Systems. All rights reserved.*
