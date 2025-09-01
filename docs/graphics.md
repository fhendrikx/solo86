
# PiUART Graphics

## I/O Ports

 - 0x28 VC_CTRL
   - I/O Write: Write commands to the video controller
   - I/O Read:  Read status (see below)
 - 0x2A VC_PARAM
   - I/O Write: Write parameters for commands
   - I/O Read:  Unused
 - 0x2C VC_DATA
   - I/O Write: Write video memory with auto increment
   - I/O Read:  Read video memory with auto increment

## VC_CTRL Read Status

  - bit 7 : busy
  - bit 6-0 : reserved

## VC_CTRL Write Commands

### 0x00 -> 0x3F Control Commands

 - 0x00 : Display Terminal  (ALT-F1)
 - 0x01 : Display Graphics  (ALT-F2)
 - 0x02 : Display Debug Log (ALT-F3)

### 0x40 -> 0x5F Graphics Commands

 - 0x40 Set Resolution 256x192
 - 0x41 Set Resolution 512x384
 - 0x42 Set Resolution 1024x768
 - 0x43 Set Custom Resolution (width_low, width_high, height_low, height_high)
 - 0x44 Set Resolution 256x192 with double buffering
 - 0x45 Set Resolution 512x384 with double buffering
 - 0x46 Set Resolution 1024x768 with double buffering
 - 0x47 Set Custom Resolution (width_low, width_high, height_low, height_high) with double buffering

#### 8 Bit

 - 0x48 Set Mem Write X (x)
 - 0x49 Set Mem Write Y (y)
 - 0x4A Set Mem Read X (x)
 - 0x4B Set Mem Read Y (y)

#### 16 Bit

 - 0x4C Set Mem Write X (x_low, x_high)
 - 0x4D Set Mem Write Y (y_low, y_high)
 - 0x4E Set Mem Read X (x_low, x_high)
 - 0x4F Set Mem Read Y (y_low, y_high)

### Misc

 - 0x50 Set Clipping Mode
 - 0x51 Set Wrap Mode
 - 0x52 Update display when using double buffering
 - 0x53 Clear screen
 - 0x54 Fill Screen (colour)

### 0x60 -> 0x7F Drawing Commands 8 Bit

 - 0x60 Set Pixel (x, y, colour)
 - 0x61 Draw Line (x1, y1, x2, y2, colour)

### 0x80 -> 0x9F Drawing Commands 16 Bit

 - 0x80 Set Pixel (x_low, x_high, y_low, y_high, colour)
 - 0x81 Draw line (x1_low, x1_high, y1_low, y1_high, x2_low, x2_high, y2_low, y2_high, colour)

