# ZIRDS Logo Setup Instructions

## Logo File Location
Place the Zoppler logo file (zoppler_logo.png) in the following location:
```
/workspace/resources/zoppler_logo.png
```

## Logo Requirements
- **Format**: PNG with transparent background
- **Recommended size**: 600x600 pixels or larger
- **Aspect ratio**: Should maintain the original Zoppler logo proportions

## Already Integrated
The application code has been updated to:
1. Load the logo from the Qt resource system (`:/ images/resources/zoppler_logo.png`)
2. Display it in the splash screen on startup
3. Fallback to text-based branding if logo is not found

## Current Status
✅ QRC file updated to include `resources/zoppler_logo.png`
✅ Splash screen code updated to display logo
✅ Fallback text-based splash screen implemented

## Next Steps
1. Save the Zoppler logo image (from the conversation) to `/workspace/resources/zoppler_logo.png`
2. Rebuild the application: `make clean && make`
3. The logo will automatically appear in the splash screen

## Copyright Notice
© Zoppler Systems - All rights reserved
