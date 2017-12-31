#include <ESP8266WiFi.h>
#include <Adafruit_NeoPixel.h> // Include the adafruit Neopixel Library
#include <MQTTClient.h>

const char ssid[] = "ssid";
const char pass[] = "password";

WiFiClient net;
MQTTClient client;

unsigned long lastMillis = 0;

void connect();  // <- predefine connect() for setup()

const int stripCount = 8;
const int ledCount = 17;
const int brightness = 100;

enum mode {modeColorWipe, modeRainbowRain, modeRain, modeRainbow, modeRainbowSparkle, modeSnake, modeSparkle};
mode currentMode = modeRainbowSparkle;


// Parameter 1 = number of pixels in strip
// Parameter 2 = pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
Adafruit_NeoPixel strip_1 = Adafruit_NeoPixel(ledCount, 4, NEO_GRBW + NEO_KHZ800);
Adafruit_NeoPixel strip_2 = Adafruit_NeoPixel(ledCount, 5, NEO_GRBW + NEO_KHZ800);
Adafruit_NeoPixel strip_3 = Adafruit_NeoPixel(ledCount, 2, NEO_GRBW + NEO_KHZ800);  
Adafruit_NeoPixel strip_4 = Adafruit_NeoPixel(ledCount, 0, NEO_GRBW + NEO_KHZ800);  
Adafruit_NeoPixel strip_5 = Adafruit_NeoPixel(ledCount, 15, NEO_GRBW + NEO_KHZ800);
Adafruit_NeoPixel strip_6 = Adafruit_NeoPixel(ledCount, 12, NEO_GRBW + NEO_KHZ800);  
Adafruit_NeoPixel strip_7 = Adafruit_NeoPixel(ledCount, 13, NEO_GRBW + NEO_KHZ800);  
Adafruit_NeoPixel strip_8 = Adafruit_NeoPixel(ledCount, 14, NEO_GRBW + NEO_KHZ800);

Adafruit_NeoPixel pixelStrips[8] = {strip_1, strip_2, strip_3, strip_4, strip_5, strip_6, strip_7, strip_8};
uint32_t strips[stripCount][ledCount];

void setup() {  
  for(int x=0; x < stripCount; x++) {
      pixelStrips[x].begin();
      pixelStrips[x].setBrightness(brightness);
      pixelStrips[x].show();
  }
  
  Serial.begin(115200);
  WiFi.begin(ssid, pass);

  // Note: Local domain names (e.g. "Computer.local" on OSX) are not supported by Arduino.
  // You need to set the IP address directly.
  //
  // MQTT brokers usually use port 8883 for secure connections.
  client.begin("IP_ADDRESS", 1883, net);
  client.onMessage(messageReceived);

  connect();
}

void connect() {
  Serial.print("checking wifi...");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(1000);
  }

  Serial.print("\nconnecting...");
  while (!client.connect("led-umbrella", "key", "token")) {
    Serial.print(".");
    delay(1000);
  }

  Serial.println("\nconnected!");
  client.subscribe("lights");
}

void loop() {
  Serial.printf("loop heap size: %u\n", ESP.getFreeHeap());
  
  switch(currentMode)
  {
    case modeColorWipe:
      Serial.print("colorWipe\n");
      colorWipe(strip_1.Color(0, 0, 0, 255), 50); // White
      break;
    case modeRainbow:
      Serial.print("rainbow\n");
      rainbow();
      break;
    case modeRain:
      Serial.print("rain\n");
      rain();
      break;
    case modeRainbowRain:
      Serial.print("rainbow rain\n");
      rainbowRain();
      break;
   case modeRainbowSparkle:
      Serial.print("rainbowSparkle\n");
      rainbowSparkle();
      break;
    case modeSparkle:
      Serial.print("sparkle\n");
      sparkle();
      break;
    case modeSnake:
      Serial.print("snake\n");
      snake();
      break;
    default:
      break;
  }
  
  client.loop();
  delay(10);  // <- fixes some issues with WiFi stability

  if (!client.connected()) {
    connect();
  }
}

void messageReceived(String &topic, String &payload) {
  Serial.println("incoming: " + topic + " - " + payload);
  trigger(payload.c_str());
}

void trigger(const char* event) {
  if(strcmp(event, "rainbow") == 0) {
    Serial.printf("trigger event %s\n", event);
    currentMode = modeRainbow;
  } else if (strcmp(event, "colorWipe") == 0){
     Serial.printf("trigger event %s\n", event);
     currentMode = modeColorWipe;
  } else if (strcmp(event, "rainbowRain") == 0){
     Serial.printf("trigger event %s\n", event);
     currentMode = modeRainbowRain;
  } else if (strcmp(event, "rain") == 0){
     Serial.printf("trigger event %s\n", event);
     currentMode = modeRain;
  } else if (strcmp(event, "sparkle") == 0){
     Serial.printf("trigger event %s\n", event);
     currentMode = modeSparkle;
  } else if (strcmp(event, "snake") == 0){
   Serial.printf("trigger event %s\n", event);
   currentMode = modeSnake;
  } else if (strcmp(event, "rainbowSparkle") == 0){
    Serial.printf("trigger event %s\n", event);
    currentMode = modeRainbowSparkle;
   }
}

void showStrips() {
  for(int x=0; x < stripCount; x++) {
      pixelStrips[x].show();
  }
}

// draw lights according to strips[][] array
void updateStrips() {
  for(int x=0; x < stripCount; x++) {
    for(int y=ledCount-1; y>=0; y--) {
      pixelStrips[x].setPixelColor(y, strips[x][y]);
      pixelStrips[x].show();
    }
  }  
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
      showStrips();
      delay(wait);
  }
}

// MR SPARKLE PROGRAM!!!!
// Call createSparkle with white color
void sparkle() {
  createSparkle(strip_1.Color(255, 255, 255));
}

// Call createSparkle with random color
void rainbowSparkle() {
  createSparkle(strip_1.Color(random(255), random(255), random(255)));
}

// Draw 2 random leds, dim all leds, repeat
void createSparkle(uint32_t color) {
    
  // dim any leds that are on
  for(int x=0; x < stripCount; x++) {
    for(int y=0; y< ledCount; y++) {
      strips[x][y] = DimColor(strips[x][y], .85);  
    }
  }

  int x = random(stripCount);
  int y = random(ledCount);
  strips[x][y] = color;

  x = random(stripCount);
  y = random(ledCount);
  strips[x][y] = color;

  updateStrips();
}

void snake() {
  uint16_t count = 1;
  for(int x=0; x < stripCount; x++) {
    for(uint16_t i=0; i<ledCount; i++) {
      if (count < 18) {
         strip_8.setPixelColor(16-i, 0); // Erase 'tail'
         strip_1.setPixelColor(i, strip_1.Color(75, 250, 100, 0)); // Draw 'head' pixel
      } else if (17 < count && count < 35) {
         strip_1.setPixelColor(i, 0); // Erase 'tail'
         strip_2.setPixelColor(16-i, strip_1.Color(75, 250, 100, 0)); // Draw 'head' pixel
      } else if (34 < count && count < 52) {
         strip_2.setPixelColor(16-i, 0); // Erase 'tail'
         strip_3.setPixelColor(i, strip_1.Color(75, 250, 100, 0)); // Draw 'head' pixel
      } else if (51 < count && count < 69) {
         strip_3.setPixelColor(i, 0); // Erase 'tail'
         strip_4.setPixelColor(16-i, strip_1.Color(75, 250, 100, 0)); // Draw 'head' pixel      
      } else if (68 < count && count < 86) {
        strip_4.setPixelColor(16-i, 0); // Erase 'tail'
        strip_5.setPixelColor(i, strip_1.Color(75, 250, 100, 0)); // Draw 'head' pixel  
      } else if (85 < count && count < 103) {
        strip_5.setPixelColor(i, 0); // Erase 'tail'
        strip_7.setPixelColor(16-i, strip_1.Color(75, 250, 100, 0)); // Draw 'head' pixel 
      } else if (102 < count && count < 120) {
        strip_7.setPixelColor(16-i, 0); // Erase 'tail'
        strip_6.setPixelColor(i, strip_1.Color(75, 250, 100, 0)); // Draw 'head' pixel    
      } else if (119 < count && count < 137) {
        strip_6.setPixelColor(i, 0); // Erase 'tail'
        strip_8.setPixelColor(16-i, strip_1.Color(75, 250, 100, 0)); // Draw 'head' pixel    
      }
      count++;
      showStrips();
      delay(30);
    }
  }
}

// Rain Program
void rain() {
  delay(10);
  uint32_t color = strip_1.Color(0, 0, 255);
  createRain(color);
}

// Rainbow Rain Program
void rainbowRain() {
  delay(25);
  uint32_t color = strip_1.Color(random(255), random(255), random(255));
  createRain(color);
}

// Base Rain Program
void createRain(uint32_t color) {
  // first move any ON lights down one on each strip
  for(int x=0; x < stripCount; x++) {
    for(int y=ledCount-1; y>0; y--) {
      if (strips[x][y-1] != 0) {
          strips[x][y] = strips[x][y-1];
          strips[x][y-1] = DimColor(strips[x][y-1], .50);  
      }
    }
    // each row: special case turn off all first lights
    strips[x][0] = 0;
  }
  // turn on light at first position of random strip
  strips[random(stripCount)][0] = color;

  updateStrips();
}

//Rainbow Program
void rainbow() {
  int j, x, y;
  
  for(j=1; j < 256; j++) {
    for(x=0; x < stripCount; x++) {
      for(y=ledCount-1; y>=0; y--) {   
        pixelStrips[x].setPixelColor(y, Wheel(j));
      }
      pixelStrips[x].show();
    }
  }  
}

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(int WheelPos) {
  if(WheelPos < 85) {
    return strip_1.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
  } else if(WheelPos < 170) {
    WheelPos -= 85;
    return strip_1.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  } else {
    WheelPos -= 170;
    return strip_1.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
}

// Returns the Red component of a 32-bit color
uint8_t Red(uint32_t color)
{
    return (color >> 16) & 0xFF;
}

// Returns the Green component of a 32-bit color
uint8_t Green(uint32_t color)
{
    return (color >> 8) & 0xFF;
}

// Returns the Blue component of a 32-bit color
uint8_t Blue(uint32_t color)
{
    return color & 0xFF;
}

// Return color, dimmed by percentage
uint32_t DimColor(uint32_t color, float dimPercent)
{
  int red = Red(color) * dimPercent;
  int blue = Blue(color) * dimPercent;
  int green = Green(color) * dimPercent;
  uint32_t dimColor = strip_1.Color(red, green, blue);
  return dimColor;
}
