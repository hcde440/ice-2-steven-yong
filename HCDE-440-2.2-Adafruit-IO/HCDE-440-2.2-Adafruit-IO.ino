//https://io.adafruit.com/steventy/dashboards/hcde-440

#include "config.h"                                               //Include the the config.h to access private information.

#include <ESP8266WiFi.h>                                          //Include the WiFi library for the microcontroller.
#include <ESP8266HTTPClient.h>                                    //Include the client library for the microcontroller.
#include <ArduinoJson.h>                                          //Include the library to parse JSON.

#include "Adafruit_Si7021.h"                                      //Include the library for the temperature and humidity sensor.
Adafruit_Si7021 sensor = Adafruit_Si7021();                       //Create an instance of Si7021 and give it the name "sensor."

#include <SPI.h>                                                  //Include the SPI library for I2C.
#include <Wire.h>                                                 //Include the wire library for I2C.
#include <Adafruit_GFX.h>                                         //Include the Adafruit graphics library.
#include <Adafruit_SSD1306.h>                                     //Include the library for the OLED screen.
#define SCREEN_WIDTH 128                                          //Set the OLED display width to 128 pixels.
#define SCREEN_HEIGHT 32                                          //Set the OLED display height to 32 pixels.
#define OLED_RESET 13                                             //Set the OLED reset pin to 13.
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);     //Initiate the OLED display with the following parameters.


typedef struct {                                                  //Create a struct type.
  String ip;                                                      //Put a string called ip in the struct.
  String cc;                                                      //Put a string called cc in the struct.
  String cn;                                                      //Put a string called cn in the struct.
  String rc;                                                      //Put a string called rc in the struct.
  String rn;                                                      //Put a string called rn in the struct.
  String cy;                                                      //Put a string called cy in the struct.
  String ln;                                                      //Put a string called ln in the struct.
  String lt;                                                      //Put a string called lt in the struct.
} GeoData;                                                        //Name the struct GeoData.
GeoData location;                                                 //Create a GeoData variable called location.
typedef struct {                                                  //Create a struct type.
  String temp;                                                    //Put a string called temp in the struct.
  String humidity;                                                //Put a string called humidity in the struct.
  String ws;                                                      //Put a string called ws in the struct.
  String wd;                                                      //Put a string called wd in the struct.
  String cc;                                                      //Put a string called cc in the struct.
} MetData;                                                        //Name the struct MetData.
MetData weather;                                                  //Create a MetData variable called weather.


AdafruitIO_Feed *input = io.feed("input");                        //Connect the input feed.
AdafruitIO_Feed *output = io.feed("output");                      //Connect the output feed.



void setup() {
  Serial.begin(115200);                                           //Initialize the serial monitor at a rate of 115200 baud.
  while(! Serial);                                                //Keep looping until the serial monitor responds.

  Serial.println();                                               //Print this in the serial monitor.
  Serial.println("Testing the Si7021 sensor.");                   //Print this in the serial monitor.
  if (!sensor.begin()) {                                          //If the temperature and humidity sensor does not respond, execute the following.
    Serial.println("Did not find Si7021 sensor!");                //Print this in the serial monitor.
    while (true)                                                  //Keep looping while if the temperature and humidity sensor does not respond.
      ;
  }
  Serial.print("Found model ");                                   //Print this in the serial monitor.
  switch(sensor.getModel()) {                                     //Start a switch statement and pass it the sensor model.
    case SI_Engineering_Samples:                                  //If the model is this, execute the following.
      Serial.print("SI engineering samples"); break;              //Print this in the serial monitor, then exit the switch statement.
    case SI_7013:                                                 //If the model is this, execute the following.
      Serial.print("Si7013"); break;                              //Print this in the serial monitor, then exit the switch statement.
    case SI_7020:                                                 //If the model is this, execute the following.
      Serial.print("Si7020"); break;                              //Print this in the serial monitor, then exit the switch statement.
    case SI_7021:                                                 //If the model is this, execute the following.
      Serial.print("Si7021"); break;                              //Print this in the serial monitor, then exit the switch statement.
    case SI_UNKNOWN:                                              //If the model is this, execute the following.
    default:                                                      //Set the default to the following.
      Serial.print("Unknown");                                    //Print this in the serial monitor, then exit the switch statement.
  }
  Serial.println();                                               //Print this in the serial monitor. 
  Serial.print("Rev(");                                           //Print this in the serial monitor.
  Serial.print(sensor.getRevision());                             //Print the revision number in the serial monitor.
  Serial.print(")");                                              //Print this in the serial monitor.
  Serial.println();                                               //Print this in the serial monitor.
  Serial.print("Serial #");                                       //Print this in the serial monitor.
  Serial.print(sensor.sernum_a, HEX);                             //Print the first serial number in the serial monitor.
  Serial.println(sensor.sernum_b, HEX);                           //Print the second serial number in the serial monitor.
    
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {                //If the display does not respond after generating display voltage from 3.3V internally and addressing 0x3C for 128x32 pixels, execute the following.
    Serial.println(F("SSD1306 allocation failed"));               //Print this in the serial monitor.
    for(;;);                                                      //Loop forever.
  }
  display.display();                                              //Display the Adafruit logo.
  delay(2000);                                                    //Delay by 2000 milliseconds.
  display.clearDisplay();                                         //Clear the display.
  display.drawPixel(10, 10, WHITE);                               //Draw a single white pixel at (10,10).
  display.display();                                              //Display the white pixel at (10,10).
  
  Serial.println();                                               //Print this in the serial monitor. 
  Serial.print("Connecting to Adafruit IO");                      //Print this in the serial monitor.
  io.connect();                                                   //Connect to Adafruit IO.
  input->onMessage(handleMessage);                                //Pass the input feed into the handleMessage function whenever a message is received.
  while(io.status() < AIO_CONNECTED) {                            //Keep looping the following if Adafruit IO does not connect.
    Serial.print(".");                                            //Print this in the serial monitor.
    delay(500);                                                   //Delay by 500 milliseconds.
  }
  Serial.println(io.statusText());                                //Print the status of the Adafruit IO connection.
  input->get();                                                   //Save the input feed to the get function.
}


void loop() {                                                     //Keep looping the following.
  io.run();                                                       //Maintain connection to Adafruit IO.
}


void handleMessage(AdafruitIO_Data *data) {                               //Define the handleMessage function.
  display.clearDisplay();                                                 //Clear the display.
  display.setTextSize(1);                                                 //Set the scale to 1.
  display.setTextColor(WHITE);                                            //Set the color to white.
  display.setCursor(0,0);                                                 //Set the cursor at (0,0).
  
  Serial.print("received <- ");                                           //Print this in the serial monitor.
  if(data->toString() == "ON") {                                          //If the data from the input feed is "ON," then execute the following.
    Serial.println("ON");                                                 //Print this in the serial monitor.
    getIP();                                                              //Call the getIP function.
    getGeo();                                                             //Call the getGeo function.
    getMet();                                                             //Call the getMet function.
    Serial.println("API Temperature: " + weather.temp + "°F");            //Retrieve the temperature and print this in the serial monitor.
    
    display.print(F("API Temp: "));                                       //Print this to the display.
    display.print(weather.temp);                                          //Print weather.temp to the display.
    display.println(F("F"));                                              //Print this to the display.
    
    float convertedTemp = ((sensor.readTemperature() * (9/5)) + 32);      //Retrieve the temperature from the sensor, convert it to Fahrenheit, and name it convertedTemp.
    Serial.print("Sensor Humidity: ");                                    //Print this in the serial monitor.
    Serial.println(sensor.readHumidity(), 1);                             //Retrieve the humidity level from the sensor and print it in the serial monitor.
    Serial.print("Sensor Temperature: ");                                 //Print this in the serial monitor.
    Serial.println(convertedTemp, 1);                                     //Print this in the serial monitor.

    display.print(F("Sensor Humidity: "));                                //Print this to the display.
    display.println(sensor.readHumidity(), 1);                            //Print sensor.readHumidit() to the display with 1 decimal place.
    display.print(F("Sensor Temp: "));                                    //Print this to the display.
    display.print(convertedTemp, 1);                                      //Print convertedTemp to the display with 1 decimal place.
    display.println(F("F"));                                              //Print this to the display.
    delay(1000);                                                          //Delay by 1000 milliseconds.
    
    if(weather.temp.toInt() > convertedTemp) {                            //If the temperature from the API is greater than the sensor reading, execute the following.
      Serial.println("The temperature is colder than usual.");            //Print this in the serial monitor.
      display.print(F("Temp < API Temp"));                                //Print this to the display.
      output->save("The temperature is colder than usual.");              //Save the following into the output feed.
    }
    else if (weather.temp.toInt() < convertedTemp) {                      //If the temperature from the API is less than the sensor reading, execute the following.
      Serial.println("The temperature is hotter than usual.");            //Print this in the serial monitor.
      display.print(F("Temp > API Temp"));                                //Print this to the display.
      output->save("The temperature is hotter than usual.");              //Save the following into the output feed.
    }
    else {                                                                //Else, execute the following.
      Serial.println("The temperature is average.");                      //Print this in the serial monitor.
      display.print(F("Temp = API Temp"));                                //Print this to the display.
      output->save("The temperature is average.");                        //Save the following into the output feed.
    }
  }
  else                                                                    //Else, execute the following.
    Serial.println("OFF");                                                //Print this in the serial monitor.
  display.display();                                                      //Show the display.
}


String getIP() {                                                                          //Define the getIP function.
  HTTPClient theClient;                                                                   //Create a mini browser on the microcontroller.
  String ipAddress;                                                                       //Create a string variable called ipAddress.

  theClient.begin("http://api.ipify.org/?format=json");                                   //Open this link with the mini browser.
  int httpCode = theClient.GET();                                                         //Retrieve the HTTP code.

  if (httpCode > 0) {                                                                     //If the HTTP code is greater than 0, do this.
    if (httpCode == 200) {                                                                //If the HTTP code is 200, do this.

      DynamicJsonBuffer jsonBuffer;                                                       //Create a dynamic JSON buffer.

      String payload = theClient.getString();                                             //Store what the mini browser gives back as one big string.
      JsonObject& root = jsonBuffer.parse(payload);                                       //Parse the string.
      ipAddress = root["ip"].as<String>();                                                //Retrieve the IP address and convert it into a string.

    } else {                                                                              //If the HTTP code is not 200, do this.
      Serial.println("Something went wrong with connecting to the endpoint.");            //Print this in the serial monitor.
      return "error";                                                                     //Return an informational string for errors.
    }
  }
  return ipAddress;                                                                       //Return the IP address.
}


void getGeo() {                                                                           //Define the getGeo function.
  Serial.println();                                                                       //Print this in the serial monitor. 
  String ipAddress = getIP();                                                             //Create a string and assign it to the string returned by the getIP function.
  HTTPClient theClient;                                                                   //Create a mini browser on the microcontroller.
  Serial.println("Making HTTP request to get location.");                                  //Print this in the serial monitor.
  theClient.begin("http://api.ipstack.com/" + ipAddress + "?access_key=" + geo_Key);      //Open this link in the mini browser.
  int httpCode = theClient.GET();                                                         //Retrieve the HTTP code.

  if (httpCode > 0) {                                                                     //If the HTTP code is greater than 0, do this.
    if (httpCode == 200) {                                                                //If the HTTP code is 200, do this.
      Serial.println("Received HTTP payload for location.");                              //Print this in the serial monitor.
      DynamicJsonBuffer jsonBuffer;                                                       //Create a dynamic JSON buffer.
      String payload = theClient.getString();                                             //Store what the mini browser gives back as one big string.
      Serial.println("Parsing...");                                                       //Print this in the serial monitor.
      JsonObject& root = jsonBuffer.parse(payload);                                       //Parse the string.

      if (!root.success()) {                                                              //If the parsing was not successful, do this.
        Serial.println("parseObject() failed");                                           //Print this in the serial monitor.
        Serial.println(payload);                                                          //Print the unparsed string in the serial monitor.
        return;                                                                           //Return nothing.
      }

      location.ip = root["ip"].as<String>();                                              //Cast the IP address as a string and put it into the struct.
      location.cc = root["country_code"].as<String>();                                    //Cast the country code as a string and put it into the struct.
      location.cn = root["country_name"].as<String>();                                    //Cast the country name as a string and put it into the struct.
      location.rc = root["region_code"].as<String>();                                     //Cast the region code as a string and put it into the struct.
      location.rn = root["region_name"].as<String>();                                     //Cast the region name as a string and put it into the struct.
      location.cy = root["city"].as<String>();                                            //Cast the city as a string and put it into the struct.
      location.lt = root["latitude"].as<String>();                                        //Cast the latitude as a string and put it into the struct.
      location.ln = root["longitude"].as<String>();                                       //Cast the longitude as a string and put it into the struct.
      
    } else {                                                                              //If the HTTP code is not 200, do this.
      Serial.println("Something went wrong with connecting to the endpoint.");            //Print this in the serial monitor.
    }
  }
}


void getMet() {                                                                           //Define the getMet function.
  HTTPClient theClient;                                                                   //Create a mini browser on the microcontroller.
  Serial.println("Making HTTP request to get the API temperature.");                      //Print this in the serial monitor.
  theClient.begin("http://api.openweathermap.org/data/2.5/weather?q=" + location.cy + "&units=imperial" + "&APPID=" + weather_Key);       //Open this link in the mini browser.
  int httpCode = theClient.GET();                                                         //Retrieve the HTTP code.

  if (httpCode > 0) {                                                                     //If the HTTP code is greater than 0, do this.
    if (httpCode == 200) {                                                                //If the HTTP code is 200, do this.
      Serial.println("Received HTTP payload for the API temperature.");                   //Print this in the serial monitor.
      DynamicJsonBuffer jsonBuffer;                                                       //Create a dynamic JSON buffer.
      String payload = theClient.getString();                                             //Store what the mini browser gives back as one big string.
      Serial.println("Parsing...");                                                       //Print this in the serial monitor.
      JsonObject& root = jsonBuffer.parse(payload);                                       //Parse the string.

      if (!root.success()) {                                                              //If the parsing was not successful, do this.
        Serial.println("parseObject() failed");                                           //Print this in the serial monitor.
        Serial.println(payload);                                                          //Print the unparsed string in the serial monitor.
        return;                                                                           //Return nothing.
      }

      weather.temp = root["main"]["temp"].as<String>();                                   //Cast the temperature as a string and put it into the struct.
      weather.humidity = root["main"]["humidity"].as<String>();                           //Cast the humidity as a string and put it into the struct.
      weather.ws = root["wind"]["speed"].as<String>();                                    //Cast the wind speed as a string and put it into the struct.
      weather.wd = root["wind"]["deg"].as<String>();                                      //Cast the wind direction as a string and put it into the struct.
      weather.cc = root["clouds"]["all"].as<String>();                                    //Cast the cloud coverage as a string and put it into the struct.
      
    } else {                                                                              //If the HTTP code is not 200, do this.
      Serial.println("Something went wrong with connecting to the endpoint.");            //Print this in the serial monitor.
    }
  }
  Serial.println();                                                                       //Print this in the serial monitor.
}
