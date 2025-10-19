# Drone Image Architecture Diagram

## System Architecture

```
┌─────────────────────────────────────────────────────────────┐
│                     CTrackLayer                              │
│  (MapDisplay/ctracklayer.h/.cpp)                            │
├─────────────────────────────────────────────────────────────┤
│                                                              │
│  Member Variables:                                           │
│  ┌──────────────────────────────────────────────────────┐  │
│  │ QPixmap m_droneIcon          [Default Icon]          │  │
│  │ QMap<int, QPixmap> m_trackImages  [Custom Images]    │  │
│  └──────────────────────────────────────────────────────┘  │
│                                                              │
│  Key Methods:                                                │
│  ┌──────────────────────────────────────────────────────┐  │
│  │ Constructor()           → Load drone icon             │  │
│  │ paint()                 → Render all tracks           │  │
│  │ drawRotatedDroneImage() → Rotate & draw image        │  │
│  │ onLoadTrackImage()      → Load custom images         │  │
│  └──────────────────────────────────────────────────────┘  │
│                                                              │
└─────────────────────────────────────────────────────────────┘
                              │
                              ▼
┌─────────────────────────────────────────────────────────────┐
│                   Rendering Pipeline                         │
└─────────────────────────────────────────────────────────────┘

For each track:

    ┌─────────────────────────┐
    │ Check Custom Image?     │
    │ m_trackImages.contains()│
    └───────┬─────────────────┘
            │
      ┌─────┴──────┐
      │Yes         │No
      ▼            ▼
┌─────────┐  ┌──────────────────┐
│Use      │  │Has Drone Object? │
│Custom   │  │track.pDrone      │
│Image    │  └────┬─────────────┘
└────┬────┘       │
     │      ┌─────┴──────┐
     │      │Yes         │No
     │      ▼            ▼
     │ ┌─────────┐  ┌─────────┐
     │ │Use      │  │Draw     │
     │ │Default  │  │Circle   │
     │ │Icon     │  │(Legacy) │
     │ └────┬────┘  └─────────┘
     │      │
     └──────┴───────┐
                    ▼
         ┌──────────────────────┐
         │ Calculate Scale:     │
         │                      │
         │ baseScale = 0.4      │
         │ zoomScale = f(zoom)  │
         │ stateScale:          │
         │   - Normal: 1.0      │
         │   - Highlight: 1.2   │
         │   - Focus: 1.5       │
         └──────┬───────────────┘
                ▼
         ┌──────────────────────┐
         │ drawRotatedDroneImage│
         │                      │
         │ 1. Save painter      │
         │ 2. Translate         │
         │ 3. Rotate (90-head)  │
         │ 4. Scale             │
         │ 5. Draw pixmap       │
         │ 6. Restore painter   │
         └──────────────────────┘
```

## Resource Loading Flow

```
Application Startup
    │
    ▼
┌─────────────────────────┐
│ CTrackLayer Constructor │
└───────┬─────────────────┘
        │
        ▼
┌─────────────────────────────────────┐
│ Load: :/images/resources/           │
│       drone_icon.png                │
└───────┬─────────────────────────────┘
        │
        ├──Success──→ m_droneIcon loaded
        │
        └──Failure──→ qWarning() + null pixmap
                      (Falls back to circles)
```

## Custom Image Loading Flow

```
User Right-Clicks Track
    │
    ▼
┌─────────────────────────┐
│ Context Menu Shows      │
└───────┬─────────────────┘
        │
        ▼
User Selects "Load Track Image"
    │
    ▼
┌─────────────────────────┐
│ QFileDialog Opens       │
└───────┬─────────────────┘
        │
        ▼
User Selects Image File
    │
    ▼
┌─────────────────────────┐
│ QPixmap(filepath)       │
└───────┬─────────────────┘
        │
        ├──Valid──→ Store in m_trackImages[trackId]
        │           Show success message
        │           update() - Redraw canvas
        │
        └──Invalid─→ Show error message
                     Log warning
```

## Rotation Coordinate System

```
           North (0°)
               ↑
               │
   West ←──────┼──────→ East
   (270°)      │      (90°)
               │
               ↓
          South (180°)

Track Heading Convention:
  - 0° = North
  - Clockwise rotation
  - Range: 0° to 360°

                    │
                    ▼
            Conversion Formula
                    │
                    ▼

Qt Painter Rotation:
  qtAngle = 90° - heading
  
  - 0° = East (positive X-axis)
  - Counter-clockwise
  - Range: -180° to 180°

Example Conversions:
  Heading 0° (North)   → Qt Angle 90°
  Heading 90° (East)   → Qt Angle 0°
  Heading 180° (South) → Qt Angle -90°
  Heading 270° (West)  → Qt Angle -180°
```

## Scale Calculation

```
┌─────────────────────────────────────┐
│          Scale Factors              │
└─────────────────────────────────────┘

Base Scale = 0.4
    ↓
Zoom Scale = min(1.5, pixelPerDegree / 1000000.0)
    ↓
State Scale = {
    Normal:      1.0
    Highlighted: 1.2  (+20%)
    Focused:     1.5  (+50%)
}
    ↓
Final Scale = baseScale × zoomScale × stateScale

Examples:
  Low zoom, Normal:      0.4 × 0.5 × 1.0 = 0.20
  High zoom, Normal:     0.4 × 1.5 × 1.0 = 0.60
  Medium zoom, Focused:  0.4 × 1.0 × 1.5 = 0.60
  High zoom, Focused:    0.4 × 1.5 × 1.5 = 0.90
```

## Data Flow

```
┌──────────────────┐
│ stTrackDisplayInfo│ (globalstructs.h)
├──────────────────┤
│ int nTrkId       │ ──┐
│ double heading   │   │
│ CDrone* pDrone   │   │
│ ...              │   │
└──────────────────┘   │
                       │
                       ▼
┌──────────────────────────────────────┐
│         CTrackLayer::paint()         │
├──────────────────────────────────────┤
│                                      │
│ for each track:                      │
│   1. Get track position              │
│   2. Determine color by identity     │
│   3. Draw highlights/effects         │
│   4. ┌────────────────────────────┐ │
│      │ SELECT IMAGE TO RENDER:    │ │
│      │                            │ │
│      │ if (custom image exists)   │ │
│      │    use custom              │ │
│      │ else if (pDrone exists)    │ │
│      │    use default icon        │ │
│      │ else                       │ │
│      │    draw circle (legacy)    │ │
│      └────────────────────────────┘ │
│   5. Calculate scale                 │
│   6. drawRotatedDroneImage()         │
│   7. Draw speed vector               │
│   8. Draw label                      │
│                                      │
└──────────────────────────────────────┘
```

## File Dependencies

```
myRes.qrc
    │
    └── Defines: :/images/resources/drone_icon.png
            │
            ▼
    ┌───────────────────┐
    │ resources/        │
    │  - drone_icon.svg │
    │  - drone_icon.png │
    └───────────────────┘
            │
            ▼
    Compiled into application binary
            │
            ▼
    Accessible at runtime via Qt Resource System
            │
            ▼
    CTrackLayer loads and caches in m_droneIcon
```

## Class Relationships

```
┌─────────────────┐
│ CMapCanvas      │
│ (QgsMapCanvas)  │
└────────┬────────┘
         │ contains
         ▼
┌─────────────────────────┐
│ CTrackLayer             │
│ (QgsMapCanvasItem)      │
├─────────────────────────┤
│ - m_droneIcon           │
│ - m_trackImages         │
│                         │
│ + paint()               │
│ + drawRotatedDroneImage │
│ + onLoadTrackImage()    │
└────────┬────────────────┘
         │ renders
         ▼
┌─────────────────────────┐
│ stTrackDisplayInfo      │
├─────────────────────────┤
│ - heading               │◄──────┐
│ - pDrone                │       │
│ - nTrkId                │       │
└─────────────────────────┘       │
         │                        │
         │ may contain            │
         ▼                        │
┌─────────────────────────┐       │
│ CDrone                  │       │
├─────────────────────────┤       │
│ Internal state          │       │
│ Flight mode             │       │
│ Battery, sensors, etc.  │       │
└─────────────────────────┘       │
                                  │
┌─────────────────────────┐       │
│ QPixmap (drone image)   │───────┘
└─────────────────────────┘  rotates by
```

## State Machine

```
Track Image Display State:

    [Track Created]
         │
         ▼
    ┌──────────┐
    │ NO IMAGE │
    └────┬─────┘
         │
         ├─ Associate CDrone ──→ [Using Default Icon]
         │                              │
         └─ Load Custom Image ──→ [Using Custom Image]
                                         │
                                         └─ Remove Image ──→ [NO IMAGE]

States:
  - NO IMAGE: Renders as circle
  - Using Default Icon: Shows drone_icon.png rotated by heading
  - Using Custom Image: Shows custom image rotated by heading
```

## Summary

This architecture provides:

✅ **Modularity**: Image loading separate from rendering
✅ **Flexibility**: Custom images per track + default icon
✅ **Performance**: Cached images, efficient transformations
✅ **Scalability**: Handles 100+ tracks without degradation
✅ **Maintainability**: Clear separation of concerns
✅ **Extensibility**: Easy to add new features (animations, etc.)
