#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
  #include <avr/power.h>
#endif

#define TEST_OUTPUT

#define BAUDRATE 115200
#define PIXELSCOUNT 12
#define LED_PIN 4

#ifdef TEST_OUTPUT
#define LOG(index, color) Serial.print(F("Pixel "));\
      Serial.print(index);\
      Serial.print(F(" color: "));\
      Serial.println(color, HEX);
#else
#define LOG(index, color)
#endif

String cmd = "";
boolean justReceivedCmd = false;
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(PIXELSCOUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

unsigned long scheduledTime = -1;
byte scheduledState;
void (*scheduledFunc)(unsigned long, byte);// = null;
  


void setup() {
  Serial.begin(BAUDRATE);
  cmd.reserve(6);

  pixels.begin();

//  for (int i = 0; i < PIXELSCOUNT; ++i)
//     pixels.setPixelColor(i, 0);
  

  signalInit();
  
  /*pixels.setPixelColor(0, 0xff1f1f1f);
  pixels.setPixelColor(1, 0x001f0000);
  pixels.setPixelColor(2, 0x00001f00);
  pixels.setPixelColor(3, 0x0000001f);
  
  pixels.show();  

 */


  // for (int i = 0; i < PIXELSCOUNT; ++i)
  //   pixelsBuffer[i] = 0;
}

void loop() {
  unsigned long elapsed = millis();
  if (scheduledTime <= elapsed) {
    scheduledTime = -1;
    (*scheduledFunc)(elapsed, scheduledState);
  }
  
  // put your main code here, to run repeatedly:
  //delay(10000);

}


void signalInit(){
  Serial.println(F("Ready"));
  schedule(0, 0, &_signalInit);
}

void _signalInit(unsigned long _ignore, byte state) {

  /*const static byte colors[] = {
    0x1F, 0x00, 0x00, 
    0x00, 0x1F, 0x00,
    0x00, 0x00, 0x1F, 
    0x1F,   1F,   1F, 
    0x00,   00    00, 0x001F1F1F, 0x00000000, 0x001F1F1F};
  const static byte indexes[] = {0, 1, 2, 3, 3, 3, 3, 3};
  */
  uint32_t color;
  uint32_t index;
  switch (state)
  {
    case 0:
      color = 0x1f0000;
      index = 0;
      break;
    case 1:
      color = 0x001f00;
      index = 1;
      break;
    case 2:
      color = 0x00001f;
      index = 2;
      break;
    case 3:
      color = 0x1f1f1f;
      index = 3;
      break;
    case 4:
      color = 0x000000;
      index = 3;
      break;
    case 5:
      color = 0x1f1f1f;
      index = 3;
      break;
    case 6:
      color = 0x000000;
      index = 3;
      break;
    case 7:
      color = 0x1f1f1f;
      index = 3;
      break;
  }


  if (state < 8) {
//    pixels.setPixelColor(indexes[state], colors[state]);
    pixels.setPixelColor(index, state);
    schedule(state < 3 ? 300 : 100, state + 1, &_signalInit);
  }
  else {
    pixels.clear();
  }


  pixels.show();
  

}


void schedule(int delayMillis, byte state, void(*func)(unsigned long, byte)){
  scheduledFunc = func;
  scheduledState = state;
  scheduledTime = millis() + delayMillis;
}

inline bool isEOL(char c)
{
  return c == '\r' || c == '\n';
}


void serialEvent() {
  static byte state = 0; // 0 - ready, 1 - skipEOL
              
  
  while (Serial.available()) {
    char c = (char)Serial.read();

    if (isEOL(c))
      c = '\n';

    if (state == 0) {
      state = parseCmd(c);
      runCmd(state, '\0');
    }
    else 
      runCmd(state, state == 0 ? '\0' : c);

    if (c == '\n')
      state = 0;
    
  }
}

void runCmd(const byte& cmd, const char& c)
{
  switch (cmd)
  {
    case 2:
      if (c == '\n') countCmd();
      break;
    case 3:
      setCmd(c);
      break;
  }
}

int parseCmd(const char& c){
  switch (c)  {
    case 'C':
      return 2; // Leds count
    case 'S':   
      return 3; // Set leds
    default:
      return 1; // Ignore until new line
  }
}



void countCmd() {
  Serial.println(PIXELSCOUNT);
}

void setCmd(const char& c) {
  static byte pixelIndex = 0;
  static byte shift = 0;
  static uint32_t v = 0;

  if (shift == -1) //Parsing failed, ignore input until reset
    return;
  //int currentPixel = 0;

  if (c == '\0')
  {
    pixelIndex = 0;
    v = 0;
    shift = 0;
  }
  else if (c == '\n')
  {
    pixels.show();
  }
  else
  {
    uint8_t b = c >= '0' && c <= '9' ? c - '0' : c >= 'a' && c <= 'f' ? c - 'a' + 10 : c >= 'A' && c <= 'F' ? c - 'A' + 10: 32;
    if (b > 15)
    {
      Serial.println(F("failed"));
      shift = -1; // Don't parse next characters
      return;
    }
    v <<= 4;
    v |= b;
    shift++;
    if (shift == 6)
    {
      pixels.setPixelColor(pixelIndex++, v);
      LOG(pixelIndex, v);
      /*
#ifdef TEST_OUTPUT
      Serial.print(F("Pixel "));
      Serial.print(pixelIndex);
      Serial.print(F(" color: "));
      Serial.println(v, HEX);
#endif  */   
      
      v = 0;
      shift = 0;

      
    }
    
    
  }
  
   

  
}




