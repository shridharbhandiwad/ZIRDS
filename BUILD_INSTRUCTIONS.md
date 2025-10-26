# ZIRDS Build Instructions

## Zoppler Integrated Radar Display System (ZIRDS)
**© Zoppler Systems. All rights reserved.**

---

## Prerequisites

### Required Dependencies
- Qt 5.x or Qt 6.x
- QGIS libraries (qgis_core, qgis_gui, qgis_analysis)
- GEOS, PROJ, and SpatialIndex libraries
- C++11 compatible compiler (g++, clang++)

### Install Dependencies (Ubuntu/Debian)
```bash
sudo apt-get update
sudo apt-get install -y \
    qtbase5-dev \
    qtdeclarative5-dev \
    qtquickcontrols2-5-dev \
    qtcharts5-dev \
    qtmultimedia5-dev \
    libqgis-dev \
    libgeos-dev \
    libproj-dev \
    libspatialindex-dev
```

---

## Building ZIRDS

### 1. Generate Makefile
```bash
cd /workspace
qmake RadarDisplay.pro
```

### 2. Build the Application
```bash
make -j$(nproc)
```

### 3. Clean Build (if needed)
```bash
make clean
qmake RadarDisplay.pro
make -j$(nproc)
```

---

## Running ZIRDS

### Start the Application
```bash
./bin/RadarDisplay
```

### Expected Startup Behavior
1. **Splash Screen** (3 seconds)
   - Displays Zoppler logo
   - Shows "ZIRDS" branding
   - Copyright notice: "© Zoppler Systems"

2. **Windows Launch**
   - PPI Display window opens (MAXIMIZED)
   - Control Center window opens (MAXIMIZED)
   - PPI window appears on front as welcome screen

3. **Dual Monitor Setup** (if available)
   - PPI Display on Monitor 1
   - Control Center on Monitor 2
   - Both maximized to full screen

---

## Logo File Setup

### Replace Placeholder Logo
The application currently uses a placeholder logo. To use the actual Zoppler logo:

1. **Save the logo file:**
   ```bash
   cp /path/to/zoppler_logo.png /workspace/resources/zoppler_logo.png
   ```

2. **Rebuild the application:**
   ```bash
   make clean
   qmake RadarDisplay.pro
   make -j$(nproc)
   ```

### Logo Requirements
- **Format**: PNG with transparent background
- **Size**: 600x600 pixels recommended
- **Location**: `/workspace/resources/zoppler_logo.png`

---

## Keyboard Shortcuts

### Window Management
- **F1**: Show/Hide PPI Display window
- **F2**: Show/Hide Control Center window
- **F3**: Arrange windows for dual monitor setup
- **F4**: Toggle fullscreen mode
- **ESC**: Exit fullscreen mode

### PPI Display
- **H**: Map Home view
- **G**: Toggle Grid
- **C**: Toggle Compass
- **F**: Zoom Fit to screen
- **L**: Load new map
- **S**: Settings dialog

### Control Center
- **1-8**: Switch between tabs
- **Ctrl+Tab**: Cycle through tabs

---

## Directory Structure

```
/workspace/
├── bin/                    # Compiled executable
│   └── RadarDisplay
├── resources/              # Resource files
│   ├── zoppler_logo.png   # Zoppler logo (replace with actual)
│   ├── drone_icon.png
│   └── drone_icon.svg
├── main.cpp                # Application entry point
├── cmapmainwindow.*        # Window manager
├── cppiwindow.*            # PPI Display window
├── ccontrolswindow.*       # Control Center window
├── MapDisplay/             # Map and display components
├── myRes.qrc              # Qt resource file
├── RadarDisplay.pro       # Qt project file
└── ZIRDS_CONFIGURATION_SUMMARY.md
```

---

## Troubleshooting

### Build Errors

**Issue**: QGIS libraries not found
```bash
# Solution: Install QGIS development libraries
sudo apt-get install libqgis-dev
```

**Issue**: Qt version mismatch
```bash
# Solution: Ensure correct Qt version is installed
qmake --version
# Should show Qt 5.x or Qt 6.x
```

### Runtime Errors

**Issue**: Logo not displaying
- Check that `/workspace/resources/zoppler_logo.png` exists
- Rebuild after adding logo: `make clean && make`
- Application will use fallback text-based splash if logo missing

**Issue**: Windows not maximizing
- Check system window manager settings
- Try manual maximize: Press **F3** for dual monitor arrangement

---

## Version Information

- **Application**: ZIRDS (Zoppler Integrated Radar Display System)
- **Version**: v0.1
- **Build Date**: 23rd June 2025
- **Organization**: Zoppler Systems
- **Copyright**: © 2025 Zoppler Systems. All rights reserved.

---

## Support

For technical support or inquiries:
- Website: zoppler.com
- Email: support@zoppler.com (if applicable)

---

*This is a proprietary product of Zoppler Systems. All rights reserved.*
