#include <ESP8266HTTPClient.h>
#include <ESP8266WiFi.h>
#include <ArduinoJson.h>
#include <SparkFun_ADXL345.h>  

ADXL345 adxl = ADXL345();             

//int interruptPin = 2;   

void setup(){

   Serial.begin(9600); 
                            
   WiFi.begin("AndroidAP", "");    //WiFi connection
 
   while (WiFi.status() != WL_CONNECTED) {  //Wait for the WiFI connection
 
   delay(500);
   Serial.println("Waiting for connection");
 
  }
  
  
  adxl.powerOn();                     // Power on the ADXL345

  adxl.setRangeSetting(16);           // Give the range settings
                                      // Accepted values are 2g, 4g, 8g or 16g
                                      // Higher Values = Wider Measurement Range
                                      // Lower Values = Greater Sensitivity

  adxl.setSpiBit(0);                  // Configure the device to be in 4 wire SPI mode when set to '0' or 3 wire SPI mode when set to 1
                                      // Default: Set to 1
                                      // SPI pins on the ATMega328: 11, 12 and 13 as reference in SPI Library 
   
  adxl.setActivityXYZ(1, 0, 0);       // Set to activate movement detection in the axes "adxl.setActivityXYZ(X, Y, Z);" (1 == ON, 0 == OFF)
  adxl.setActivityThreshold(75);      // 62.5mg per increment   // Set activity   // Inactivity thresholds (0-255)
 
  adxl.setInactivityXYZ(1, 0, 0);     // Set to detect inactivity in all the axes "adxl.setInactivityXYZ(X, Y, Z);" (1 == ON, 0 == OFF)
  adxl.setInactivityThreshold(75);    // 62.5mg per increment   // Set inactivity // Inactivity thresholds (0-255)
  adxl.setTimeInactivity(30);         // How many seconds of no activity is inactive?

  adxl.setTapDetectionOnXYZ(0, 0, 1); // Detect taps in the directions turned ON "adxl.setTapDetectionOnX(X, Y, Z);" (1 == ON, 0 == OFF)
 
  // Set values for what is considered a TAP and what is a DOUBLE TAP (0-255)
  adxl.setTapThreshold(50);           // 62.5 mg per increment
  adxl.setTapDuration(15);            // 625 μs per increment
  adxl.setDoubleTapLatency(80);       // 1.25 ms per increment
  adxl.setDoubleTapWindow(200);       // 1.25 ms per increment
 
  // Set values for what is considered FREE FALL (0-255)
  adxl.setFreeFallThreshold(7);       // (5 - 9) recommended - 62.5mg per increment
  adxl.setFreeFallDuration(30);       // (20 - 70) recommended - 5ms per increment
 
  // Setting all interupts to take place on INT1 pin
  //adxl.setImportantInterruptMapping(1, 1, 1, 1, 1);     // Sets "adxl.setEveryInterruptMapping(single tap, double tap, free fall, activity, inactivity);" 
                                                        // Accepts only 1 or 2 values for pins INT1 and INT2. This chooses the pin on the ADXL345 to use for Interrupts.
                                                        // This library may have a problem using INT2 pin. Default to INT1 pin.
  
  // Turn on Interrupts for each mode (1 == ON, 0 == OFF)
  adxl.InactivityINT(1);
  adxl.ActivityINT(1);
  adxl.FreeFallINT(1);
  adxl.doubleTapINT(1);
  adxl.singleTapINT(1);
  
//attachInterrupt(digitalPinToInterrupt(interruptPin), ADXL_ISR, RISING);   // Attach Interrupt

}

/****************** MAIN CODE ******************/
/*     Accelerometer Readings and Interrupt    */
void loop(){

  String url = "http://192.168.43.14:5000/IrrigationSystem/api/sensors";
  String url_status = "http://192.168.43.14:5000/IrrigationSystem/api/status";
  // Accelerometer Readings
  int x,y,z;   
  adxl.readAccel(&x, &y, &z);         // Read the accelerometer values and store them in variables declared above x,y,z

  // Output Results to Serial
  /* UNCOMMENT TO VIEW X Y Z ACCELEROMETER VALUES */  
 // Serial.print(x);
 //  Serial.print(", ");
 //  Serial.print(y);
 // Serial.print(", ");
  // Serial.println(z); 
  
  ADXL_ISR(x, y, z,url, url_status);
  // You may also choose to avoid using interrupts and simply run the functions within ADXL_ISR(); 
  //  and place it within the loop instead.  
  // This may come in handy when it doesn't matter when the action occurs. 

  //SENSOR_STATUS(x, y, z,url,inactivity);

}

void sendJSONData(int x, int y,int z, String url){
  
if (WiFi.status() == WL_CONNECTED) { //Check WiFi connection status
   
    Serial.println("Now Connected To Wifi");

StaticJsonBuffer<300> JSONbuffer; //Declaring static JSON buffer
JsonObject& JSONencoder = JSONbuffer.createObject();

JSONencoder["sensorType"] = "Accelerometer";
JSONencoder["Serial"] = "123456";

JsonArray& values = JSONencoder.createNestedArray("values"); //JSON array
values.add(x); //Add value to array
values.add(y); //Add value to array
values.add(z); //Add value to array

char JSONmessageBuffer[300];
JSONencoder.prettyPrintTo(JSONmessageBuffer, sizeof(JSONmessageBuffer));
Serial.println(JSONmessageBuffer);

HTTPClient http; //Declare object of class HTTPClient

http.begin(url); //Specify request destination
http.addHeader("Content-Type", "application/json"); //Specify content-type header

int httpCode = http.POST(JSONmessageBuffer); //Send the request
String payload = http.getString(); //Get the response payload

Serial.println(httpCode); //Print HTTP return code
Serial.println(payload); //Print request response payload

http.end(); //Close connection

} else {

Serial.println("Error in WiFi connection");

}
delay(1000);
}

//Sensor Status Method

void sendJSONData_STATUS(int x, int y,int z, String url_status){
  
if (WiFi.status() == WL_CONNECTED) { //Check WiFi connection status
   
    Serial.println("Now Connected To Wifi");

StaticJsonBuffer<300> JSONbuffer; //Declaring static JSON buffer
JsonObject& JSONencoder = JSONbuffer.createObject();

JSONencoder["sensorType"] = "Accelerometer";
JSONencoder["Serial"] = "123456";

JsonArray& values = JSONencoder.createNestedArray("values"); //JSON array
values.add(x); //Add value to array
values.add(y); //Add value to array
values.add(z); //Add value to array

char JSONmessageBuffer[300];
JSONencoder.prettyPrintTo(JSONmessageBuffer, sizeof(JSONmessageBuffer));
Serial.println(JSONmessageBuffer);

HTTPClient http; //Declare object of class HTTPClient

http.begin(url_status); //Specify request destination
http.addHeader("Content-Type", "application/json"); //Specify content-type header

int httpCode = http.POST(JSONmessageBuffer); //Send the request
String payload = http.getString(); //Get the response payload

Serial.println(httpCode); //Print HTTP return code
Serial.println(payload); //Print request response payload

http.end(); //Close connection

} else {

Serial.println("Error in WiFi connection");

}
delay(1000);
}

void ADXL_ISR(int x, int y , int z, String url, String url_status) {
  
  // getInterruptSource clears all triggered actions after returning value
  // Do not call again until you need to recheck for triggered actions
  byte interrupts = adxl.getInterruptSource();
  
  // Free Fall Detection
  if(adxl.triggered(interrupts, ADXL345_FREE_FALL)){
    Serial.println("*** FREE FALL ***");
    //add code here to do when free fall is sensed
  } 
  
  // Inactivity
  if(adxl.triggered(interrupts, ADXL345_INACTIVITY)){
    Serial.println("*** INACTIVITY ***");
     sendJSONData_STATUS(x, y, z, url_status);
  }
  
  // Activity
  if(adxl.triggered(interrupts, ADXL345_ACTIVITY)){
    Serial.println("*** ACTIVITY ***"); 
     sendJSONData(x, y, z, url);
  }
  
  // Double Tap Detection
  if(adxl.triggered(interrupts, ADXL345_DOUBLE_TAP)){
    Serial.println("*** DOUBLE TAP ***");
     sendJSONData(x, y, z, url);
  }
  
  // Tap Detection
  if(adxl.triggered(interrupts, ADXL345_SINGLE_TAP)){
    Serial.println("*** TAP ***");
     sendJSONData(x, y, z, url);
  } 
}
