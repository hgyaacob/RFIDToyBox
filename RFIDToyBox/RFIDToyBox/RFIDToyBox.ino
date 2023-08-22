#include <FastLED.h>
#include <SPI.h>
#include <MFRC522.h>

FASTLED_USING_NAMESPACE

#define SS_PIN 10
#define RST_PIN 9

#define BUZZ 8

#if defined(FASTLED_VERSION) && (FASTLED_VERSION < 3001000)
#warning "Requires FastLED 3.1 or later; check github for latest code."
#endif

#define DATA_PIN    6
//#define CLK_PIN   4
#define LED_TYPE    WS2812
#define COLOR_ORDER GRB
#define NUM_LEDS    12
CRGB leds[NUM_LEDS];

#define BRIGHTNESS          10
#define FRAMES_PER_SECOND  1000


MFRC522 rfid(SS_PIN, RST_PIN);

bool detectCard = false;
long previousMillis = 0;
long previousTime = 0;
long interval = 1000;
bool tonePlayed = false;



uint8_t gHue = 0; // rotating "base color" used by many of the patterns

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  SPI.begin();

  rfid.PCD_Init();
  rfid.PCD_DumpVersionToSerial();
  Serial.println("RFID Sensor Ready");

  // tell FastLED about the LED strip configuration
  FastLED.addLeds<LED_TYPE,DATA_PIN,COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);

  // set master brightness control
  FastLED.setBrightness(BRIGHTNESS);

  //set BUZZer pin
  pinMode(BUZZ, OUTPUT);
  
}

void loop() {
  // put your main code here, to run repeatedly:
  unsigned long currentTime = millis();
  
  
  while(detectCard != true){
    checkRFIDCard();
    sinelon();
    FastLED.show();  
    FastLED.delay(1000/FRAMES_PER_SECOND); 
    EVERY_N_MILLISECONDS( 10 )  { gHue++; } ;

  }
    rainbowWithGlitter();
    FastLED.show();  
    FastLED.delay(1000/FRAMES_PER_SECOND); 
    EVERY_N_MILLISECONDS( 5 )  { gHue++; } ;
    tonePlayed = false;

    
    if (currentTime - previousTime > 5000){
      previousTime = currentTime;
      detectCard = false;
    }
}


void checkRFIDCard(){
 unsigned long currentMillis = millis();
  detectCard = false;
     
     // Look for new cards
  if ( ! rfid.PICC_IsNewCardPresent()) 
  {
    return;
  }
  // Select one of the cards
  if ( ! rfid.PICC_ReadCardSerial()) 
  {
    return;
  }

  String content= "";
  byte letter;
  for (byte i = 0; i < rfid.uid.size; i++) 
  {
     content.concat(String(rfid.uid.uidByte[i] < 0x10 ? " 0" : " "));
     content.concat(String(rfid.uid.uidByte[i], HEX));
  }
  content.toUpperCase();
  
  if (content.substring(1) == "89 B1 12 10"){

    // if(currentMillis - previousMillis > interval) {
     // previousMillis = currentMillis;
    
      Serial.println("RFID Card Detected");
      detectCard = true;


      if(tonePlayed == false){
      play_ok();
      tonePlayed = true;
    }
    // }
   
  }

  if (content.substring(1) == "EB CC AF 1B"){

    // if(currentMillis - previousMillis > interval) {
    //  previousMillis = currentMillis;
      
      Serial.println("RFID Tag Keychain Detected");
      detectCard = true;

      if(tonePlayed == false){
      play_nok();
      tonePlayed = true;
      }
   //  }
   
  }
}




void rainbow() 
{
  // FastLED's built-in rainbow generator
  fill_rainbow( leds, NUM_LEDS, gHue, 7);
}

void rainbowWithGlitter() 
{
  // built-in FastLED rainbow, plus some random sparkly glitter
  rainbow();
  addGlitter(80);
}

void addGlitter( fract8 chanceOfGlitter) 
{
  if( random8() < chanceOfGlitter) {
    leds[ random16(NUM_LEDS) ] += CRGB::White;
  }
}

void juggle() {
  // eight colored dots, weaving in and out of sync with each other
  fadeToBlackBy( leds, NUM_LEDS, 20);
  byte dothue = 0;
  for( int i = 0; i < 8; i++) {
    leds[beatsin16( i+7, 0, NUM_LEDS-1 )] |= CHSV(dothue, 200, 255);
    dothue += 32;
  }
}

void sinelon()
{
  // a colored dot sweeping back and forth, with fading trails
  fadeToBlackBy( leds, NUM_LEDS, 20);
  int pos = beatsin16( 13, 0, NUM_LEDS-1 );
  leds[pos] += CHSV( gHue, 255, 192);
}

void play_ok(){
  tone(BUZZ,523,50);
  delay(50);
  tone(BUZZ, 783, 50);
  delay(50);
  tone(BUZZ, 1046, 50);
  delay(50);
  tone(BUZZ, 1568, 50);
  delay(50);
  tone(BUZZ, 2093, 70);
  delay(250);
}

void play_nok(){
     tone(BUZZ,370,50);
     delay(100);
     tone(BUZZ, 370, 300);
     delay(250);
}
