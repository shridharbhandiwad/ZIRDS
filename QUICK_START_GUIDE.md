# Quick Start Guide - New Features

## Keyboard Shortcuts (The Fastest Way!)

Press these keys to toggle widgets:

| Key | Widget |
|-----|--------|
| **S** | 🎮 Simulation - Generate simulated tracks |
| **R** | 📹 Recording - Record/Replay sessions |
| **M** | 💚 Health Monitor - System health status |
| **P** | 🔮 Predictive Maintenance - Maintenance schedules |
| **T** | 📊 Track Table - View active tracks |
| **A** | 📈 Analytics - Track statistics |
| **C** | ⚙️ Config Panel - Display controls |
| **I** | ⚡ Interfaces - Hardware controllers |
| **H** | 🏠 Home - Reset map view |

---

## Quick Start: Track Simulation

1. Press **S** to open Simulation widget
2. Configure parameters (default: 5 tracks, 10km range)
3. Click **▶ Start** button
4. Watch tracks appear on map in real-time!

### Pro Tips:
- Increase "Azimuth Spread" to 360° for full coverage
- Enable "Random Movement" for realistic behavior
- Adjust "Update Rate" for faster/slower updates

---

## Quick Start: Recording a Session

1. Press **R** to open Recording widget
2. Click **● Record** button
3. Let it run for desired time
4. Click **⏹ Stop** to save
5. Recording saved to `~/RadarRecordings/`

### To Replay:
1. Select recording from library
2. Choose playback speed (1x, 2x, etc.)
3. Click **▶ Play**

---

## Quick Start: Health Monitoring

1. Press **M** to open Health Monitor
2. View overall system health percentage
3. Click any module card for details
4. Check performance metrics (CPU, Memory, etc.)
5. Click **🔍 Run Diagnostics** for full check
6. Click **📄 Export Report** to save health status

### Color Guide:
- 🟢 Green = Healthy (90-100%)
- 🟡 Yellow = Warning (70-90%)
- 🔴 Red = Critical (<70%)

---

## Quick Start: Predictive Maintenance

1. Press **P** to open Predictive Maintenance
2. View next maintenance countdown
3. Click any component for detailed info
4. Check "Upcoming Maintenance Schedule" table
5. Read recommendations at bottom
6. Click **📄 Export Maintenance Report** to save

### Status Icons:
- ✅ Excellent = 90+ days
- 🟢 Good = 60-90 days
- 🟡 Fair = 30-60 days
- 🟠 Poor = 7-30 days
- ⚠️ Service Required = 1-7 days
- 🔴 Critical = Immediate!

---

## Default Layout on Startup

```
┌─────────────────┬──────────────────────┬─────────────────┐
│                 │                      │                 │
│   Simulation    │                      │  Track Table ⭐ │
│   (S key)       │                      │  (T key)        │
│                 │                      │  [DEFAULT TAB]  │
│   ──────────    │   Map Display        │                 │
│   Recording     │   (Center)           │  ───────────    │
│   (R key)       │                      │  Analytics      │
│   [TAB]         │                      │  (A key) [TAB]  │
│                 │                      │                 │
│                 │                      │  Health Monitor │
│   Config Panel  │                      │  (M key)        │
│   (C key)       │                      │                 │
│                 │                      │  ───────────    │
│   ──────────    │                      │  Predictive     │
│   Interfaces    │                      │  Maintenance    │
│   (I key) [TAB] │                      │  (P key) [TAB]  │
│                 │                      │                 │
└─────────────────┴──────────────────────┴─────────────────┘
                  │  Charts (Bottom)     │
                  │  Hidden by default   │
                  └──────────────────────┘
```

⭐ = Visible on startup

---

## Common Tasks

### Create a Realistic Simulation
1. Press **S**
2. Set tracks: 10
3. Set range: 10000m (10km)
4. Set azimuth spread: 360°
5. Enable "Random Movement"
6. Set identity: Random
7. Click **▶ Start**

### Record and Share a Demo
1. Start simulation first
2. Press **R**
3. Click **● Record**
4. Wait 1-2 minutes
5. Click **⏹ Stop**
6. Click **💾 Export**
7. Share the .rdr file

### Monitor System Health
1. Press **M** for quick overview
2. Check overall health percentage
3. Look for red/yellow modules
4. Click module for details
5. Use **🔍 Run Diagnostics** weekly
6. Export report for records

### Plan Maintenance
1. Press **P**
2. Sort components by urgency
3. Check "Next Maintenance" countdown
4. Review schedule table
5. Click components needing attention
6. Follow recommendations
7. Use **📅 Schedule Maintenance** button

---

## Tips & Tricks

### Workspace Management
- Drag dock widgets to rearrange
- Double-click title bars to float/dock
- Close unused widgets to save space
- Use keyboard shortcuts for quick access

### Performance
- Reduce simulation update rate if laggy
- Limit tracks to 20-30 for smooth operation
- Close charts widget when not needed
- Export/delete old recordings to save disk space

### Best Practices
- Record important sessions for later analysis
- Run health diagnostics regularly
- Check maintenance schedule weekly
- Export reports before major system changes
- Use random identities for realistic training

---

## Troubleshooting

### Simulation not working?
- Check status shows "Running" (not Paused)
- Verify tracks count > 0
- Try clicking Stop then Start again

### Can't see tracks on map?
- Press **H** to reset map view
- Check zoom level (may be too far out)
- Verify simulation is running

### Recording failed?
- Check disk space available
- Ensure ~/RadarRecordings/ exists
- Try different filename

### Widget disappeared?
- Use keyboard shortcuts to show again
- Check it's not minimized/tabbed behind another
- Try restarting application

---

## File Locations

### Recordings
- **Location**: `~/RadarRecordings/`
- **Format**: `.rdr` files
- **Naming**: `recording_YYYYMMDD_HHMMSS.rdr`

### Exports
- **Health Reports**: User-selected location, `.txt`
- **Maintenance Reports**: User-selected location, `.txt`
- **Recording Exports**: User-selected location, `.rdr`

---

## Getting Help

1. Check status bar for quick tips
2. Hover over buttons for tooltips
3. Click info labels for descriptions
4. Read NEW_FEATURES_DOCUMENTATION.md for details
5. Check IMPLEMENTATION_SUMMARY.md for technical info

---

## Next Steps

### Try These:
1. ✅ Generate 5 tracks and watch them move
2. ✅ Record a 30-second session
3. ✅ Replay it at 2x speed
4. ✅ Check system health status
5. ✅ Review maintenance schedule
6. ✅ Export a health report
7. ✅ Experiment with different simulation settings
8. ✅ Learn all keyboard shortcuts

---

**Quick Reference Card**

```
╔══════════════════════════════════════════╗
║  RADAR DISPLAY - KEYBOARD SHORTCUTS      ║
╠══════════════════════════════════════════╣
║  S - Simulation   │  M - Health Monitor  ║
║  R - Recording    │  P - Maintenance     ║
║  T - Track Table  │  H - Home View       ║
║  A - Analytics    │  C - Config Panel    ║
║  I - Interfaces   │                      ║
╚══════════════════════════════════════════╝
```

Print or save this for quick reference!

---

**Version**: 1.0  
**Last Updated**: 2025-10-12  
**For Full Documentation**: See NEW_FEATURES_DOCUMENTATION.md
