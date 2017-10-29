#include <Adafruit_NeoPixel.h>

// Parameter 1 = number of pixels in strip
// Parameter 2 = pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
Adafruit_NeoPixel strip_1 = Adafruit_NeoPixel(17, 4, NEO_GRBW + NEO_KHZ800);
Adafruit_NeoPixel strip_2 = Adafruit_NeoPixel(17, 5, NEO_GRBW + NEO_KHZ800);
Adafruit_NeoPixel strip_3 = Adafruit_NeoPixel(17, 2, NEO_GRBW + NEO_KHZ800);  
Adafruit_NeoPixel strip_4 = Adafruit_NeoPixel(17, 0, NEO_GRBW + NEO_KHZ800);  
Adafruit_NeoPixel strip_5 = Adafruit_NeoPixel(17, 15, NEO_GRBW + NEO_KHZ800);
Adafruit_NeoPixel strip_6 = Adafruit_NeoPixel(17, 12, NEO_GRBW + NEO_KHZ800);  
Adafruit_NeoPixel strip_7 = Adafruit_NeoPixel(17, 13, NEO_GRBW + NEO_KHZ800);  
Adafruit_NeoPixel strip_8 = Adafruit_NeoPixel(17, 14, NEO_GRBW + NEO_KHZ800);

void setup() {
  strip_1.begin();
  strip_2.begin();
  strip_3.begin();
  strip_4.begin();
  strip_5.begin();
  strip_6.begin();
  strip_7.begin();
  strip_8.begin();  
  strip_1.setBrightness(30); //adjust brightness here
  strip_2.setBrightness(30); //adjust brightness here
  strip_3.setBrightness(30); //adjust brightness here
  strip_4.setBrightness(30); //adjust brightness here
  strip_5.setBrightness(30); //adjust brightness here
  strip_6.setBrightness(30); //adjust brightness here
  strip_7.setBrightness(30); //adjust brightness here
  strip_8.setBrightness(30); //adjust brightness here
  strip_1.show(); // Initialize all pixels to 'off'
  strip_2.show(); // Initialize all pixels to 'off'}
  strip_3.show(); // Initialize all pixels to 'off'}
  strip_4.show(); // Initialize all pixels to 'off'}
  strip_5.show(); // Initialize all pixels to 'off'}
  strip_6.show(); // Initialize all pixels to 'off'}
  strip_7.show(); // Initialize all pixels to 'off'}
  strip_8.show(); // Initialize all pixels to 'off'}
 }

void loop() {
  // Some example procedures showing how to display to the pixels:
  colorWipe(strip_1.Color(0, 0, 0, 255), 50); // White
  colorWipe(strip_1.Color(255, 0, 0, 0), 50); // Red
  colorWipe(strip_1.Color(0, 255, 0, 0), 50); // Green
  colorWipe(strip_1.Color(0, 0, 255, 0), 50); // Blue
}
 
// Fill the dots one after the other with a color
void colorWipe(uint32_t c, uint8_t wait) {
  for(uint16_t i=0; i<strip_1.numPixels(); i++) {
      strip_1.setPixelColor(i, c);
      strip_2.setPixelColor(i, c);
      strip_3.setPixelColor(i, c);
      strip_4.setPixelColor(i, c);
      strip_5.setPixelColor(i, c);
      strip_6.setPixelColor(i, c);
      strip_7.setPixelColor(i, c);
      strip_8.setPixelColor(i, c);
      strip_1.show();
      strip_2.show();
      strip_3.show();
      strip_4.show();
      strip_5.show();
      strip_6.show();
      strip_7.show();
      strip_8.show();
      delay(wait);
  }
}

