#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
  #include <avr/power.h>
#endif

#define BAUDRATE 115200
#define PIXELSCOUNT 5
#define LED_PIN 4

String cmd = "";
char writebuffer[32];
boolean justReceivedCmd = false;
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(PIXELSCOUNT, LED_PIN, NEO_GRB + NEO_KHZ800);
  


void setup() {
  Serial.begin(BAUDRATE);
  cmd.reserve(1024);
  writebuffer[31] = '\0';

  // for (int i = 0; i < PIXELSCOUNT; ++i)
  //   pixelsBuffer[i] = 0;
}

void loop() {

  // put your main code here, to run repeatedly:

}

void serialEvent() {
  while (Serial.available()) {
    char c = (char)Serial.read();
    if (justReceivedCmd && isEOL(c))
      continue;
    else
      justReceivedCmd = false;

    if (isEOL(c))
      parseCmd();
    else
      cmd += c;
    } 
}

void parseCmd(){
  if (cmd.length() == 0)
    return;

  switch (cmd[0])
  {
    case 'C':
      countCmd();
      break;
    case 'S':
      setCmd();
      break;
  }
}



void countCmd() {
  Serial.print(PIXELSCOUNT);
}

void setCmd() {
  int currentPixel = 0;
  uint32_t v = 0;
  for (int i = 1; i < cmd.length() && currentPixel < PIXELSCOUNT; ++i) {
    char c = cmd[i];
    uint8_t b = c >= '0' && c <= '9' ? c - '0' : c >= 'a' && c <= 'f' ? c - 'a' : c >= 'A' && c <= 'F' ? c - 'A' : 32;
    if (b > 15)
      return;

    v |= c;
    v << 4;

    if ((i - 1) % 8 == 0) {
      pixels.setPixelColor(currentPixel++, v);
      v = 0;
    }
  }

  Serial.println("Done"
}


inline bool isEOL(char c)
{
  return c == '\r' || c == '\n';
}

