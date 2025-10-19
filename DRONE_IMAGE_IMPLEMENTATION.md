# Drone Image Implementation

## Overview
This document describes the implementation of drone image visualization for tracks in the radar display system. Drone images are now displayed and rotated according to their heading for better visual representation.

## Features Implemented

### 1. Drone Icon Creation
- Created a default drone icon in both SVG and PNG formats
- Located in `resources/drone_icon.svg` and `resources/drone_icon.png`
- Icon features:
  - Quadcopter design with 4 propellers
  - Central body with direction indicator (red arrow pointing forward)
  - Blue rotating propellers for visual clarity
  - Scales well at different zoom levels

### 2. Resource Integration
- Added drone images to Qt resource file (`myRes.qrc`)
- Images accessible via `:/images/resources/drone_icon.png`
- Resource path: `/images` prefix

### 3. Track Layer Enhancements

#### Header File Changes (`MapDisplay/ctracklayer.h`)
- Added `QPixmap` and `QMap` includes
- New member variables:
  - `QPixmap m_droneIcon` - Default drone icon for all drone tracks
  - `QMap<int, QPixmap> m_trackImages` - Custom images per track ID
- New method:
  - `drawRotatedDroneImage()` - Renders rotated drone images based on heading

#### Implementation Changes (`MapDisplay/ctracklayer.cpp`)

##### Constructor Updates
- Loads default drone icon from Qt resources on initialization
- Provides debug logging for successful/failed image loading

##### Image Rotation Function
```cpp
void CTrackLayer::drawRotatedDroneImage(QPainter *pPainter, const QPixmap &image, 
                                       const QPointF &screenPos, double heading, double scale)
```
- Rotates image based on track heading (0° = North, clockwise)
- Applies appropriate coordinate transformation (Qt uses counter-clockwise from X-axis)
- Supports dynamic scaling based on zoom level and track state
- Centers image at track position

##### Paint Method Updates
- Checks if track has custom image or is associated with a drone
- Uses custom image if available, otherwise default drone icon
- Falls back to traditional circle rendering for non-drone tracks
- Implements dynamic scaling:
  - Base scale: 0.4x
  - Zoom-dependent scaling
  - State-dependent scaling (focused: 1.5x, highlighted: 1.2x)

##### Custom Image Loading
- Enhanced `onLoadTrackImage()` method
- Supports multiple image formats: PNG, JPG, JPEG, BMP, GIF, SVG
- Stores custom images per track ID
- Provides user feedback via message boxes
- Automatic redraw after loading

## Usage

### Automatic Drone Display
Tracks associated with a `CDrone` object will automatically display the drone icon rotated according to their heading.

### Custom Track Images
1. Right-click on any track
2. Select "🖼️ Load Track Image" from context menu
3. Choose an image file
4. Image will be displayed and rotated based on track heading

### Image Behavior
- **Rotation**: Images rotate smoothly to match track heading
- **Scaling**: Images scale based on:
  - Map zoom level
  - Track state (focused/highlighted)
  - Base size optimized for visibility
- **Priority**: Custom images take precedence over default drone icon

## Technical Details

### Coordinate System
- Track heading: 0° = North, increases clockwise
- Qt rotation: Counter-clockwise from positive X-axis
- Conversion: Qt angle = 90° - heading

### Rendering Order
1. Check for custom track image
2. Check for default drone icon (if track has associated drone)
3. Fall back to traditional circle rendering

### Performance Considerations
- Images loaded once and cached
- Rotation uses efficient QPainter transformations
- Scale calculations minimize unnecessary operations

## Testing Recommendations

1. **Drone Icon Display**
   - Create tracks with associated drone objects
   - Verify drone icon appears and rotates with heading changes
   
2. **Custom Images**
   - Load custom images for specific tracks
   - Test various image formats (PNG, SVG, JPG)
   - Verify rotation follows heading correctly
   
3. **State Changes**
   - Test highlighted tracks (should scale to 1.2x)
   - Test focused tracks (should scale to 1.5x)
   - Verify smooth transitions
   
4. **Zoom Behavior**
   - Test at various zoom levels
   - Verify images scale appropriately
   - Check visibility at extreme zoom levels

## Future Enhancements

Potential improvements:
1. Image caching and memory management
2. Animation support for propeller rotation
3. Different icons for different drone types/states
4. Image rotation smoothing/interpolation
5. Custom image persistence (save/load with track data)
6. Image library browser for quick selection
7. Support for animated GIFs or sprite sheets

## Files Modified

1. `myRes.qrc` - Added drone image resources
2. `MapDisplay/ctracklayer.h` - Added image members and methods
3. `MapDisplay/ctracklayer.cpp` - Implemented image loading and rendering

## Files Created

1. `resources/drone_icon.svg` - SVG drone icon
2. `resources/drone_icon.png` - PNG drone icon
3. `DRONE_IMAGE_IMPLEMENTATION.md` - This documentation

## Dependencies

- Qt 5+ (QPixmap, QPainter, QTransform)
- QGIS libraries (QgsMapCanvas, QgsMapToPixel)
- Existing track and drone infrastructure
