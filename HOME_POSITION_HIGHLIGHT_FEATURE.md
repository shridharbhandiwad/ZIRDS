# Home Position Highlight Feature

## Overview
Added an animated expanding circles UI effect to highlight the home position when navigating to home view or using zoom-to-fit functionality in the globe view.

## Implementation Details

### New Files Created

1. **MapDisplay/chomepositionhighlightlayer.h**
   - Header file for the home position highlight layer
   - Defines the `CHomePositionHighlightLayer` class
   - Extends `QgsMapCanvasItem` for integration with QGIS map canvas

2. **MapDisplay/chomepositionhighlightlayer.cpp**
   - Implementation of the animated expanding circles effect
   - Uses Qt timer-based animation at 60 FPS
   - Draws 3 staggered expanding circles with fade-out effect
   - Includes a pulsing center marker

### Modified Files

1. **MapDisplay/cmapcanvas.h**
   - Added include for `chomepositionhighlightlayer.h`
   - Added member variable `_m_homeHighlightLayer` of type `CHomePositionHighlightLayer*`

2. **MapDisplay/cmapcanvas.cpp**
   - Updated constructor to initialize `_m_homeHighlightLayer` to nullptr
   - Modified `_loadLayers()` to create and configure the highlight layer
   - Updated `mapHome()` to trigger the 3-second expanding circles animation

3. **cppiwindow.cpp**
   - Modified `onZoomFitToScreen()` to trigger home position animation after zoom
   - Uses QTimer::singleShot with 300ms delay to allow zoom to complete first

4. **Makefile**
   - Added new source file to SOURCES list
   - Added new object file to OBJECTS list
   - Added compilation rules for the new files
   - Added MOC (Meta-Object Compiler) rules for Qt signal/slot support

## Animation Features

### Visual Design
- **3 Expanding Circles**: 
  - Cyan color (#00FFFF) matching the PPI ring style
  - Staggered start times (0%, 25%, 50% delay) for wave effect
  - Maximum radius: 1500 meters
  - Line width: 3 pixels

- **Center Marker**:
  - Pulsing red dot with 30% scale variation
  - White inner dot for contrast
  - Always visible during animation

- **Animation Properties**:
  - Duration: 3000ms (3 seconds)
  - Frame rate: 60 FPS for smooth animation
  - Cubic ease-out for natural expansion
  - Linear fade-out starting at 50% progress

### Technical Details
- **Z-Index**: 150 (higher than PPI layer at 100) to render on top
- **Coordinate System**: Uses lat/lon (WGS84) coordinates
- **Meters to Pixels**: Dynamic conversion based on current zoom level
- **Auto-cleanup**: Animation stops and hides layer after completion

## Usage

The animation is automatically triggered when:

1. **Home Button Clicked**: 
   - User clicks the "🏠 Home" button in PPI Window
   - Calls `m_mapCanvas->mapHome()`
   - Animation starts immediately

2. **Zoom to Fit**:
   - User clicks the "🔍 Zoom Fit" button
   - Map zooms to full extent
   - After 300ms delay, home view is triggered with animation

3. **Keyboard Shortcuts**:
   - `H` key: Triggers home view with animation
   - `F` key: Triggers zoom fit, then home view with animation

## Benefits

1. **Visual Feedback**: Clear indication of home/radar position
2. **User Orientation**: Helps users understand where they are in the map
3. **Professional Look**: Smooth, modern animation effect
4. **Non-intrusive**: Auto-hides after 3 seconds
5. **Performance**: Efficient timer-based rendering at 60 FPS

## Configuration

Animation parameters can be easily adjusted in `chomepositionhighlightlayer.h`:

```cpp
static constexpr int NUM_CIRCLES = 3;         // Number of expanding circles
static constexpr double MAX_RADIUS = 1500.0;  // Maximum radius in meters
static constexpr double CIRCLE_WIDTH = 3.0;   // Line width for circles
static constexpr int ANIMATION_FPS = 60;      // Animation frame rate
```

Duration can be changed when calling `startAnimation()`:
```cpp
_m_homeHighlightLayer->startAnimation(3000); // Duration in milliseconds
```

## Future Enhancements

Possible improvements:
- Add configuration UI for animation settings
- Support for different animation styles (pulse, ripple, etc.)
- Customizable colors and patterns
- Option to repeat animation on a timer
- Sound effects on home navigation
