// Modified for SPFD5408 Library by Joao Lopes  
 #include <SPFD5408_Adafruit_GFX.h>  // Core graphics library  
 #include <SPFD5408_Adafruit_TFTLCD.h> // Hardware-specific library  
 #include <SPFD5408_TouchScreen.h>  
 #include <String.h>

unsigned long lastTouchTime = 0;
const unsigned long debounceDelay = 200; // milliseconds

int Temperature, temp;

String switchstate = "OFF"; 

 // Pin assignments for the TFT touch screen  
 #define YP  A1  
 #define XM  A2  
 #define YM  7  
 #define XP  6  
 // Calibrated values for the TFT touch screen   
 #define TS_MINX  178  
 #define TS_MINY  75  
 #define TS_MAXX  931  
 #define TS_MAXY  895  
 // Instantiate the ts object  
 TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);  
 #define LCD_RD  A0  
 #define LCD_WR  A1  
 #define LCD_CD  A2  
 #define LCD_CS  A3  
 #define LCD_RESET A4  
 // Instantiate the tft object  
 Adafruit_TFTLCD tft(LCD_CS, LCD_CD, LCD_WR, LCD_RD, LCD_RESET);  
 // Assign human-readable names to some common 16-bit color values:  
 #define BLACK  0x0000  
 #define BLUE  0x001F  
 #define RED   0xF800  
 #define GREEN  0x07E0  
 #define CYAN  0x07FF  
 #define MAGENTA 0xF81F  
 #define YELLOW 0xFFE0  
 #define WHITE  0xFFFF  
 #define BOXSIZE   40  
 #define PENRADIUS  3  
 #define MINPRESSURE 10  
 #define MAXPRESSURE 1000  
 #define LED_PIN   A5    

void setSwitchBox()
{
  tft.fillRect(0, 100, 480, 100, YELLOW);
  tft.setTextSize(4); 
  tft.setTextColor(BLACK);
  tft.setCursor(80, 130);
  tft.println(switchstate);
}

void setUpBox()
{
  tft.fillRect(0, 200, 120, 120, RED); 
  tft.setTextSize(4);
  tft.setTextColor(BLACK);
  tft.setCursor(30, 250);  
  tft.println("UP");
}

void setDownBox()
{
  tft.fillRect(120, 200, 120, 120, CYAN); 
  tft.setTextSize(4);
  tft.setTextColor(BLACK);
  tft.setCursor(135, 250);  
  tft.println("DOWN");
}

void showTemperature()
{ 
  clearTemperature();
  tft.setTextColor(BLACK, WHITE);  
  tft.setTextSize(8);  
  tft.setCursor(40,10);  
  tft.print(Temperature);
  tft.print(" C");
}

void clearUpDownBox()
{
  tft.fillRect(0, 200, 480, 120, WHITE);
}

void clearTemperature()
{
  tft.setCursor(40,10);
  tft.print("    ");
}

 void setup()
{  
  Serial.begin(115200);
  tft.reset();  
  tft.begin(0x9341);  
  tft.setRotation(2); // This settings works for me with correct orientation  
  tft.fillScreen(WHITE);  // screen color 
  setSwitchBox();  
  if(Serial.available())  // if serial connection available          
    Temperature = Serial.parseInt(); // serial conncetion happening in stringss, so parse it
  showTemperature(); 
}  

 void loop() 
{  

  // get temperature form esp32
  if (switchstate == "OFF")
  {
    if(Serial.available())  // if serial connection available  
    {        
      Temperature = Serial.parseInt();
      showTemperature(); 
      temp = Temperature;
    }
  }
  // Get the touch points  
  TSPoint p = ts.getPoint();  
  // Restore the mode  
  pinMode(XM, OUTPUT);  
  pinMode(YP, OUTPUT); 
  if (p.z > MINPRESSURE && p.z < MAXPRESSURE) 
  {
  // Debounce the touch input
    p.x = map(p.x, TS_MINX, TS_MAXX, 0, tft.width());  
    p.y = map(p.y, TS_MINY, TS_MAXY, 0, tft.height());  
    // switch box
    if (p.x > 0 && p.x < 480)
    {
      if (p.y > 100 && p.y < 200)
        {
          if (switchstate == "OFF")
          {
            switchstate = "ON";
            setUpBox();
            setDownBox();
          }
          else
          {
            switchstate = "OFF";
            clearUpDownBox();
          }
          setSwitchBox();
        }
    } 
      if (switchstate == "ON")
      { 
        // Touch area for box up  
        if (p.x > 0 && p.x < 120) 
        {  
          if (p.y > 200 && p.y < 300) 
          {  
            temp += 1;
            delay(100);
            if (temp > 50)
              {
                temp = 50;
              } 
            Temperature = temp;
          }  
        }  
        // Touch area for box down  
        if (p.x > 120 && p.x < 240)
        {  
          if (p.y > 200 && p.y < 300) 
          {  
            temp -= 1;
            delay(100);
            if (Temperature < 0)
            {
              temp = 0;
            }
            Temperature = temp;
          }  
        } 
        showTemperature();
        Serial.println(Temperature); // something something discrete and indicates end of data on other side, easy for esp32 for parse and process,  very important for this 
      }
  }
}  