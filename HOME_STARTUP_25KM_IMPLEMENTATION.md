# Home Position 25 km Focus & Startup Implementation

## Overview
Implemented automatic opening of Home view on startup with 25 km focus area, including animated visual feedback.

## Changes Made

### 1. MapDisplay/cmapcanvas.h
**Added:**
- Include for `chomepositionhighlightlayer.h`
- Member variable `_m_homeHighlightLayer` of type `CHomePositionHighlightLayer*`

### 2. MapDisplay/cmapcanvas.cpp

**Added Include:**
```cpp
#include <QtMath>  // For qDegreesToRadians
```

**Updated Constructor:**
- Initialize `_m_homeHighlightLayer` to nullptr in initialization list

**Updated _loadLayers():**
- Create and initialize home highlight layer
- Set center position to radar position

**Updated mapHome():**
- Calculate 25 km radius extent centered on home position
- Convert 25 km (25000 meters) to degrees using proper lat/lon conversion
- Set map extent to show 25 km radius area
- Trigger 3-second expanding circles animation on home position
- Added debug logging

### 3. cppiwindow.cpp

**Updated Constructor:**
- Added `QTimer::singleShot(800, this, &CPPIWindow::onMapHome);`
- Auto-triggers Home view 800ms after window initialization
- Ensures map canvas is fully initialized before calling mapHome

## Technical Details

### 25 km Focus Calculation
- Uses accurate meters-to-degrees conversion
- Accounts for latitude-dependent longitude degree length
- Formula:
  - `metersPerDegreeLat = 111132.0` (constant)
  - `metersPerDegreeLon = 111320.0 * cos(latitude)` (latitude-dependent)
  - Creates square extent with 25 km radius from center

### Animation Integration
- Reuses existing CHomePositionHighlightLayer
- 3-second animation with expanding cyan circles
- Pulsing red center marker
- Automatically cleans up after animation completes

### Startup Sequence
1. Window initializes (CPPIWindow constructor)
2. Map canvas setup completes
3. Settings loaded
4. 800ms delay (ensures full initialization)
5. `onMapHome()` called
6. Map zooms to 25 km focus
7. Animation starts

## User Experience

### On Startup:
- Application opens
- PPI Display window appears
- After ~800ms, map automatically:
  - Centers on home/radar position
  - Zooms to show 25 km radius area
  - Plays 3-second expanding circles animation
  - Shows pulsing red marker at center

### When Home Button Clicked:
- Same behavior as startup
- Returns to 25 km focused view
- Animation provides visual confirmation

## Configuration

To adjust the focus distance, modify in `cmapcanvas.cpp`:
```cpp
double radiusKm = 25.0; // Change this value for different radius
```

To adjust startup delay, modify in `cppiwindow.cpp`:
```cpp
QTimer::singleShot(800, this, &CPPIWindow::onMapHome); // Change 800 to desired ms
```

To adjust animation duration, modify in `cmapcanvas.cpp`:
```cpp
_m_homeHighlightLayer->startAnimation(3000); // Change 3000 to desired ms
```

## Benefits

1. **Immediate Context**: Users see the relevant 25 km area on startup
2. **Visual Feedback**: Animation clearly indicates the home position
3. **Consistent Behavior**: Home button provides same view as startup
4. **Professional UX**: Smooth, automated initialization experience
5. **Proper Scaling**: View automatically adjusts for different map scales

## Testing Recommendations

1. Verify 25 km extent is accurate at different latitudes
2. Confirm animation plays on both startup and Home button click
3. Test with different window sizes
4. Verify behavior on dual monitor setup
5. Check performance with large maps loaded

## Related Files

- `MapDisplay/chomepositionhighlightlayer.h` - Animation layer header
- `MapDisplay/chomepositionhighlightlayer.cpp` - Animation implementation
- `HOME_POSITION_HIGHLIGHT_FEATURE.md` - Original animation feature docs
- `cppiwindow.h` - PPI window header
- `cdatawarehouse.h` - Radar position data source

## Notes

- The 25 km radius is a square extent (not circular) for simplicity
- Conversion to degrees uses standard Earth radius approximations
- Works correctly with WGS84 coordinate system (EPSG:4326)
- Animation z-index is 150, ensuring it appears on top of PPI layer
