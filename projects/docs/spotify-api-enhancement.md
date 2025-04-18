# Spotify API Enhancement for ESP32 Display

## Problem
The current Spotify API returns base64-encoded JPEG images that are difficult to render properly on the ESP32 device without a full JPEG decoder library. The ESP32 needs RGB565 format data for direct display.

## Proposed Solution
Add a new endpoint or modify the existing `/spotify/now-playing` endpoint to provide album cover images in a format that can be directly displayed on the ESP32.

### Option 1: Add a new endpoint that returns RGB565 data

Add a new endpoint:
```
GET /spotify/cover/rgb565
```

This endpoint would:
1. Fetch the cover image from Spotify API 
2. Resize it to 64x64 pixels (or configurable size)
3. Convert it to RGB565 format (16-bit color format used by LVGL)
4. Return the raw bytes directly (not base64 encoded)

#### Response Format
The response would be the raw binary data of the RGB565 image, with Content-Type `application/octet-stream`.

### Option 2: Enhance the existing endpoint

Modify the `/spotify/now-playing` endpoint to include a new field:

```json
{
  "isPlaying": true,
  "title": "Song Title",
  "artists": ["Artist 1", "Artist 2"],
  "album": "Album Name",
  "coverUrl": "https://full-resolution-image-url",
  "coverSmall": "data:image/jpeg;base64,...", 
  "coverRGB565": "data:image/rgb565;base64,...", // Base64 encoded RGB565 data
  "duration": 180000,
  "progress": 45000,
  "id": "spotify:track:id"
}
```

The `coverRGB565` would contain base64-encoded RGB565 image data that can be directly used after base64 decoding, without needing JPEG decoding.

## Implementation Notes

To convert JPEG to RGB565 format on the server:
1. Use a library like Sharp, Jimp, or node-canvas to load and resize the image
2. Convert each pixel to RGB565 format using: `pixel = ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3)`
3. Pack the result into a buffer and either serve directly or base64 encode

## ESP32 Client Code Changes

With this change, the ESP32 client code would become much simpler:

```cpp
bool parseBase64Image(const char* base64Data) {
  if (!cover_img || !base64Data) return false;
  
  Serial.println("Decoding RGB565 album cover...");
  
  // Skip the data:image/rgb565;base64, part if present
  const char* data = base64Data;
  if (strncmp(data, "data:", 5) == 0) {
    // Find the comma after base64,
    const char* comma = strstr(data, ",");
    if (comma) {
      data = comma + 1;
    }
  }
  
  // Calculate the length of decoded data
  size_t base64_len = strlen(data);
  size_t output_len = 0;
  
  // Decode the base64 data directly into rgb565_buffer
  static uint16_t rgb565_buffer[ALBUM_COVER_SIZE * ALBUM_COVER_SIZE];
  
  int ret = mbedtls_base64_decode(
    (unsigned char*)rgb565_buffer,
    sizeof(rgb565_buffer),
    &output_len,
    (const unsigned char*)data,
    base64_len
  );
  
  if (ret != 0 || output_len == 0) {
    Serial.printf("Base64 decode failed with error code %d\n", ret);
    return false;
  }
  
  // Set up the LVGL image descriptor
  album_art.data = (uint8_t*)rgb565_buffer;
  
  // Create or update the LVGL image widget
  lv_obj_clean(cover_img);
  lv_obj_t* img = lv_img_create(cover_img);
  lv_img_set_src(img, &album_art);
  lv_obj_center(img);
  
  return true;
}
```

This approach would be much more reliable and display proper album artwork.