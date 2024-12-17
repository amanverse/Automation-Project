#include "arduino_secrets.h"  // credentials for wifi
#include "thingProperties.h" // connection to wifi
#include <OneWire.h>
#include<DallasTemperature.h>

/* 
  Arduino IoT Cloud Variables description

  The following variables are automatically generated and updated when changes are made to the Thing

  bool swithch;
  CloudTemperature dashtemperature;
*/

// Data wire is conntec to the Arduino digital pin 4
#define ONE_WIRE_BUS 26 // data pin for db18s20 

// Setup a oneWire instance to communicate with any OneWire devices
OneWire oneWire(ONE_WIRE_BUS);

// Pass our oneWire reference to Dallas Temperature sensor 
DallasTemperature sensors(&oneWire);

#define relay 25 // output pin for relay control
int Temperature; // temperature by sensor

#define RXp2 16 // reciver pin from uno
#define TXp2 17 // transmission pin ro uno

// #define MasterSwitchO
// #define MasterSwitchI 

void setup() {
    // Start up the temperature library
  sensors.begin();
  // Initialize serial and wait for port to open:
  Serial.begin(230400);
  Serial2.begin(115200, SERIAL_8N1, RXp2, TXp2);
  // This delay gives the chance to wait for a Serial Monitor without blocking if none is found
  delay(1500); 

  // Defined in thingProperties.h
  initProperties();

  // Connect to Arduino IoT Cloud
  ArduinoCloud.begin(ArduinoIoTPreferredConnection);
  
  /*
     The following function allows you to obtain more information
     related to the state of network and IoT Cloud connection and errors
     the higher number the more granular information you’ll get.
     The default is 0 (only errors).
     Maximum is 4
 */
  setDebugMessageLevel(2);
  ArduinoCloud.printDebugInfo();

  pinMode(relay, OUTPUT);

}

void loop() {
  ArduinoCloud.update();
  // Your code here
  // Call sensors.requestTemperatures() to issue a global temperature and Requests to all devices on the bus
  sensors.requestTemperatures(); 
  // Why "byIndex"? You can have more than one IC on the same bus. 0 refers to the first IC on the wire
  Temperature = sensors.getTempCByIndex(0);
  delay(1000);
  Serial.print("Water Temperature : ");
  Serial.println(Temperature);

  if (!Serial2.available())
  {
    Serial2.println(dashtemperature); // transmitting data to uno,println : discrete and indicates end of data on other side, easy for esp32 for parse and process,  very important for this 
    onDashtemperatureChange();
  }

  else
  {
    dashtemperature = Serial2.readString().toInt(); // receiving from uno
    onDashtemperatureChange();
  }
}


/*
  Since Swithch is READ_WRITE variable, onSwithchChange() is
  executed every time a new value is received from IoT Cloud.
*/
void onSwithchChange()  {
  // Add your code here to act upon Swithch change
  if (!Serial2.available())
  {
    if (swithch == 0)
    {
      digitalWrite(relay, LOW);
    } 
  }

}

/*
  Since Dashtemperature is READ_WRITE variable, onDashtemperatureChange() is
  executed every time a new value is received from IoT Cloud.
*/
void onDashtemperatureChange()  
{
  // Add your code here to act upon Dashtemperature change
  if(swithch ==1)
  {
    if (dashtemperature >= Temperature)
        digitalWrite(relay, HIGH);
    else
        digitalWrite(relay, LOW);
  }
}
