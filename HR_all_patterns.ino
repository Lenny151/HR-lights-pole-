#include <FastLED.h>

#define DATA_PIN    5
#define LED_TYPE    WS2811
#define COLOR_ORDER RGB
#define NUM_LEDS    56 //28 per 2m section x 2
CRGB leds[NUM_LEDS];

uint8_t paletteIndex = 0;
uint8_t colorIndex[NUM_LEDS];

#define BRIGHTNESS          64

DEFINE_GRADIENT_PALETTE (hr_colours) {
//  0, 120, 63,  255,   //purple
//  77, 255, 0,   255,    //pink
//  128, 255, 145, 0,   //orange
//  179,  0,   255, 200,   //blue
//  255, 120, 63,  255,   //purple
 0,   255, 145, 0,   //orange
  51,  0,   255, 200,   //blue
  127, 120, 63,  255,   //purple
  204, 255, 0,   255,    //pink
  255, 255, 145, 0,   //orange
};

CRGBPalette16 myPal = hr_colours;

void setup() {
  delay(3000); // 3 second delay for recovery
  
  // tell FastLED about the LED strip configuration
  FastLED.addLeds<LED_TYPE,DATA_PIN,COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);

  // limit my draw to 1A at 5v of power draw
  FastLED.setMaxPowerInVoltsAndMilliamps(5,800); 
  
  //Fill the colorIndex array with random numbers
  for (int i = 0; i < NUM_LEDS; i++) {
    colorIndex[i] = random8();
  }

  // set master brightness control
  FastLED.setBrightness(BRIGHTNESS);
}


// List of patterns to cycle through.  Each is defined as a separate function below.
typedef void (*SimplePatternList[])();
SimplePatternList gPatterns = { fade_palette, random_fill, fade_palette, twinkle };
   
uint8_t gCurrentPatternNumber = 0; // Index number of which pattern is current
uint8_t gHue = 0; // rotating "base color" used by many of the patterns
  
void loop()
{
  // Call the current pattern function once, updating the 'leds' array
  gPatterns[gCurrentPatternNumber]();

  // send the 'leds' array out to the actual LED strip
  FastLED.show();  
  // insert a delay to keep the framerate modest

  // do some periodic updates
  EVERY_N_SECONDS( 300 ) { nextPattern(); } // change patterns periodically
}

#define ARRAY_SIZE(A) (sizeof(A) / sizeof((A)[0]))

void nextPattern()
{
  // add one to the current pattern number, and wrap around at the end
  gCurrentPatternNumber = (gCurrentPatternNumber + 1) % ARRAY_SIZE( gPatterns);
}

void bpm()
{
  // colored stripes pulsing at a defined Beats-Per-Minute (BPM)
  uint8_t BeatsPerMinute = 62;
  CRGBPalette16 palette = myPal;
  uint8_t beat = beatsin8( BeatsPerMinute, 64, 255);
  for( int i = 0; i < NUM_LEDS; i++) { //9948
    leds[i] = ColorFromPalette(palette, gHue+(i*2), beat-gHue+(i*10));
  }
}

void fade_palette()
{
  fill_palette(leds, NUM_LEDS, paletteIndex, 255 / 70, myPal, 255, LINEARBLEND);
  
  EVERY_N_MILLISECONDS(50){
    paletteIndex--;
  }
  
  FastLED.show();
}

void random_fill()
{ 
  // Color each pixel from the palette using the index from colorIndex[]
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = ColorFromPalette(myPal, colorIndex[i]);
  }
  
  EVERY_N_MILLISECONDS(10){
    for (int i = 0; i < NUM_LEDS; i++) {
      colorIndex[i]++;
    }
  }
  FastLED.show();
}

void twinkle()
{
  EVERY_N_MILLISECONDS(1){
    //Switch on an LED at random, choosing a random color from the palette
    leds[random8(0, NUM_LEDS)] = ColorFromPalette(myPal, random8(), 255, LINEARBLEND);
  }

   // Fade all LEDs down by 1 in brightness each time this is called
   fadeToBlackBy(leds, NUM_LEDS, 5);
  
  FastLED.show();
}
