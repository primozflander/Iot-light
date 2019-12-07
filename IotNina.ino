/*
Project: IotNina v1.0
Created: Primož Flander 12.5.2018

ESP8266 pin / Connection
DI1           Button
DO4           LED
*/

/************************** Configuration ***********************************/

// edit the config.h tab and enter your Adafruit IO credentials

#include "config.h"
#include <FastLED.h>

#define button      5   // D1

// led
#define LED_PIN     4   // D4
#define NUM_LEDS    5   // edit
#define BRIGHTNESS  255
#define LED_TYPE    WS2811
#define COLOR_ORDER GRB
CRGB leds[NUM_LEDS];
#define UPDATES_PER_SECOND 100

/************************ Example Starts Here *******************************/

int buttonState = 0;         // current state of the button
int lastButtonState = 1;     // previous state of the button
int buttonTime = 0;          // button press time

// set up Adafruit IO feeds
AdafruitIO_Feed *color = io.feed("iotlight.color");
AdafruitIO_Feed *stat = io.feed("iotlight.stat");

// led init
CRGBPalette16 currentPalette;
TBlendType    currentBlending;
extern CRGBPalette16 myRedWhiteBluePalette;
extern const TProgmemPalette16 myRedWhiteBluePalette_p PROGMEM;
static uint8_t startIndex = 1;


void setup() {

  pinMode(button, INPUT_PULLUP); // enable pullup

  // start the serial connection
  Serial.begin(115200);

  // wait for serial monitor to open
  while(! Serial);

  Serial.print("Connecting to Adafruit IO");

  // connect to io.adafruit.com
  io.connect();

  // set up a message handler for the count feed.
  // the handleMessage function (defined below)
  // will be called whenever a message is
  // received from adafruit io.
  color->onMessage(handleMessage);
  stat->onMessage(handleMessage);
  
  // wait for a connection
  while(io.status() < AIO_CONNECTED) {
    Serial.print(".");
    delay(500);
  }

  // Because Adafruit IO doesn't support the MQTT retain flag, we can use the
  // get() function to ask IO to resend the last value for this feed to just
  // this MQTT client after the io client is connected.
  color->get(); // get led color
  stat->get();  // get light status  

  // we are connected
  Serial.println();
  Serial.println(io.statusText());

  // led
  FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection( TypicalLEDStrip );
  FastLED.setBrightness(  BRIGHTNESS );
  currentPalette = RainbowColors_p;
  currentBlending = LINEARBLEND;
}

void loop() {

  // io.run(); is required for all sketches.
  // it should always be present at the top of your loop
  // function. it keeps the client connected to
  // io.adafruit.com, and processes any incoming data.
  io.run();
  
  // read the pushbutton input pin:
  buttonState = digitalRead(button);

  // compare the buttonState to its previous state
  if (buttonState != lastButtonState) {
    // if the state has changed
    if (buttonState == HIGH && buttonTime > 2) {
      // if the current state is HIGH then the button went from on to off:

      // save color to feed on Adafruit IO
      Serial.print("sending color -> ");
      Serial.println(startIndex);
      color->save(startIndex);
      
      Serial.println("sending stat -> 1");
      stat->save(1);

      delay(3000);
    }
    
    if (buttonState == HIGH && buttonTime <= 2) {
      Serial.println("light off");
      Serial.println(buttonTime);
      stat->save(0);

      delay(3000);
    }
                 
    // Delay a little bit to avoid bouncing
    delay(50);
    buttonTime=0;
  }
  // save the current state as the last state, for next time through the loop
  lastButtonState = buttonState;

  if (buttonState == LOW) {

    buttonTime+=1;  // button press time
  
    startIndex = startIndex + 1; /* motion speed */
      
    FillLEDsFromPaletteColors(startIndex);
      
    FastLED.show();
    FastLED.delay(1000 / UPDATES_PER_SECOND); 
  }
  

}

void handleMessage(AdafruitIO_Data *data) {

  int reading = data->toInt();

  Serial.print("received <- ");
  Serial.println(data->value());
  
  if (reading > 1){
    Serial.println("color received");

    FillLEDsFromPaletteColors(reading);      
    FastLED.show();   
  }

  if (reading == 0){
    Serial.println("light off command received");
        
    // leds off
    for( int i = 0; i < NUM_LEDS; i++) {
    leds[i] = CRGB::Black;
    }
    FastLED.show();    
  }
  
}

void FillLEDsFromPaletteColors( uint8_t colorIndex)
{
    uint8_t brightness = 255;
    
    for( int i = 0; i < NUM_LEDS; i++) {
        leds[i] = ColorFromPalette( currentPalette, colorIndex, brightness, currentBlending);
        colorIndex += 3;
    }
}

const TProgmemPalette16 myRedWhiteBluePalette_p PROGMEM =
{
    CRGB::Red,
    CRGB::Gray, // 'white' is too bright compared to red and blue
    CRGB::Blue,
    CRGB::Black,
    
    CRGB::Red,
    CRGB::Gray,
    CRGB::Blue,
    CRGB::Black,
    
    CRGB::Red,
    CRGB::Red,
    CRGB::Gray,
    CRGB::Gray,
    CRGB::Blue,
    CRGB::Blue,
    CRGB::Black,
    CRGB::Black
};
