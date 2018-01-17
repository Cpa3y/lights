#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
  #include <avr/power.h>
#endif

#define TEST_OUTPUT

#define BAUDRATE 115200
#define PIXELSCOUNT 12
#define LED_PIN 4

String cmd = "";
char writebuffer[32];
boolean justReceivedCmd = false;
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(PIXELSCOUNT, LED_PIN, NEO_GRB + NEO_KHZ800);
  


void setup() {
  Serial.begin(BAUDRATE);
  cmd.reserve(1024);
  writebuffer[31] = '\0';

  
  pixels.begin();

//  for (int i = 0; i < PIXELSCOUNT; ++i)
//     pixels.setPixelColor(i, 0);
  
  
  pixels.setPixelColor(0, 0xff1f1f1f);
  pixels.setPixelColor(1, 0x001f0000);
  pixels.setPixelColor(2, 0x00001f00);
  pixels.setPixelColor(3, 0x0000001f);
  
  pixels.show();  

  Serial.println("Ready");


  // for (int i = 0; i < PIXELSCOUNT; ++i)
  //   pixelsBuffer[i] = 0;
}

void loop() {

  // put your main code here, to run repeatedly:
  //delay(10000);

}

void serialEvent() {
  while (Serial.available()) {
    char c = (char)Serial.read();
    if (justReceivedCmd && isEOL(c))
      continue;
    else
      justReceivedCmd = false;

    if (isEOL(c))
    {
      justReceivedCmd = true;
      parseCmd();
      cmd = "";
      
    }
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
  Serial.println(PIXELSCOUNT);
}

void setCmd() {
  int currentPixel = 0;
  uint32_t v = 0;
  for (int i = 1; i < cmd.length() && currentPixel < PIXELSCOUNT; ++i) {
    char c = cmd[i];
    uint8_t b = c >= '0' && c <= '9' ? c - '0' : c >= 'a' && c <= 'f' ? c - 'a' + 10 : c >= 'A' && c <= 'F' ? c - 'A' + 10: 32;
    if (b > 15)
    {
      Serial.println("failed");
      return;
    }
      
    v <<= 4;
    v |= b;
   

    if (i % 8 == 0) {
      pixels.setPixelColor(currentPixel++, v);

#ifdef TEST_OUTPUT
      Serial.print("Index ");
      Serial.print(i);
      Serial.print(" ");
      Serial.print("Pixel ");
      Serial.print(currentPixel);
      Serial.print(" color: ");
      Serial.println(v, HEX);
#endif     
      v = 0;
    }
  }

  pixels.show();
  Serial.println("Done");
}


inline bool isEOL(char c)
{
  return c == '\r' || c == '\n';
}

