//  File:  heartbeat_on_string.ino
//
//  Animates a heartbeat at different BPMs.
// Update your LED count and Pin for your hardware.
// Adjust the kInterval to move from bpm to bpm.

#include <Adafruit_NeoPixel.h>    //  Library that provides NeoPixel functions
#include <Wire.h>
#include <TimeLib.h>

 
unsigned long previous_state_change_ts = 0;
const int kInterval = 15000;
const int kBeatSize = 2;
const int kRepeatEveryNPixels = 15; // Should be evenly divisible by LED_COUNT

#define PIN 6  //Which pin the pixels are connected to
#define LED_COUNT 150  //Number of pixels used
// -- Create a NeoPixel object called onePixel that addresses 1 pixel in pin 8
Adafruit_NeoPixel PixelString = Adafruit_NeoPixel(LED_COUNT, PIN, NEO_GRB + NEO_KHZ800);

void setup()  {
  Serial.begin(9600);
  Wire.begin();  //sets up the I2C

  PixelString.begin();             // Start the NeoPixel object
  PixelString.clear();             // Set NeoPixel color to black (0,0,0)
  PixelString.setBrightness(200);   // Affects all subsequent settings
  PixelString.show();              // Update the pixel state
}

enum Speed: int {
  Relaxed = 0,
  Walking,
  Running,
  Scared,
  Dead,
};

enum BeatSection: int {
  Main = 0,
  Dip,
  Secondary,
  End,
  Brightness,
  Interval
};

int heart_beat[][6] = {
  {150, 150, 250, 700, 100, kInterval}, // relaxed rate 48bpm
  {60, 60, 100, 280, 200,kInterval}, // walking 120 bpm
  {60, 60, 80, 200, 200, kInterval}, // running  120 bpm
  {60, 60, 60, 120, 200, kInterval}, // scared 120 bpm
  {600, 200, 800, 8400, 20, kInterval} // dead 120 bpm 
};

int speed = int(Speed::Relaxed);


void loop()  {

  speed = UpdateMode(speed, &previous_state_change_ts);

   uint8_t color[] = {200, 0,0};
   AnimateString(color, heart_beat[speed]); 
}

void AnimateString(uint8_t color_rgb[3], int frame[6]) {
    UpdatePixels( color_rgb[0],color_rgb[1],color_rgb[2], kBeatSize, kRepeatEveryNPixels );
    PixelString.show();                      
    PixelString.setBrightness(frame[BeatSection::Brightness]); 
    delay(frame[BeatSection::Main]);

    PixelString.clear();                       
    PixelString.show();                      
    delay(frame[BeatSection::Dip]);

    UpdatePixels( color_rgb[0],color_rgb[1],color_rgb[2], kBeatSize, kRepeatEveryNPixels );
    PixelString.show();    
    delay(frame[BeatSection::Secondary]);

    PixelString.clear();                       
    PixelString.show();         
    delay(frame[BeatSection::End]);
}

void UpdatePixels(uint8_t r, uint8_t g, uint8_t b, int leds, int repeat_every_n_pixels ) {
  int count = LED_COUNT/repeat_every_n_pixels;
  // For count times: light n'leds' and then clear 'repeat_every_n_pixels - leds'
  
  for (int i = 0; i < count; i++) {

    for (int j = 0; j < leds; j++)
      PixelString.setPixelColor(j + (i* repeat_every_n_pixels), r, g, b);

    for (int c = leds; c < repeat_every_n_pixels; c++)
      PixelString.setPixelColor(c + (i* repeat_every_n_pixels), 0,0,0);    
  }
}

int UpdateMode(int speed, unsigned long* timestamp) {
  if ((millis() - *timestamp) > heart_beat[speed][BeatSection::Interval])
  {
    Serial.println("----------------mode-------");
    *timestamp = millis();

    speed = (speed +1)% 5; //move to next after 1 minute?
    Serial.println(speed);
   }
   return speed;
}
