// Adafruit IO Digital Input Example
// Tutorial Link: https://learn.adafruit.com/adafruit-io-basics-digital-input
//
// Adafruit invests time and resources providing this open source code.
// Please support Adafruit and open source hardware by purchasing
// products from Adafruit!
//
// Written by Todd Treece for Adafruit Industries
// Copyright (c) 2016 Adafruit Industries
// Licensed under the MIT license.
//
// All text above must be included in any redistribution.

/************************ Link to Dashboard *********************************/

//https://io.adafruit.com/steventy/dashboards/hcde-440

/************************** Configuration ***********************************/

#include "config.h"                                                             //Include the config.h file to keep passwords and keys private.

/************************ Example Starts Here *******************************/

#define PHOTORESISTOR_PIN A0                                                    //Assign the name PHOTORESISTOR_PIN to pin A0.
#define BUTTON_PIN 5                                                            //Assign the name BUTTON_PIN to pin 5.

int current_Light = 0;                                                          //Create an integer variable called current_Light and initialize it as 0.
int last_Light = -1;                                                            //Create an integer variable called last_Light and initialize it as -1.
bool current_Button = false;                                                    //Create a boolean variable called current_Button and initialize it as false.
bool last_Button = false;                                                       //Create a boolean variable called last_Button and initialize it as false.

AdafruitIO_Feed *analog = io.feed("analog");                                    //Connect to the feed called "analog."
AdafruitIO_Feed *digital = io.feed("digital");                                  //Connect to the feed called "digital."

void setup() {

  pinMode(PHOTORESISTOR_PIN, INPUT);                                            //Set the PHOTORESISTOR_PIN as an input.
  pinMode(BUTTON_PIN, INPUT);                                                   //Set the BUTTON_PIN as an input.


  Serial.begin(115200);                                                         //Initialize the serial monitor at a rate of 115200 baud.

  while(! Serial);                                                              //Wait for the serial monitor to open.

  Serial.print("Connecting to Adafruit IO");                                    //Print this in the serial monitor.
  io.connect();                                                                 //Attempt to connect to Adafruit IO.

  while(io.status() < AIO_CONNECTED) {                                          //Execute the following while waiting for a response from Adafruit IO.
    Serial.println(io.statusText());                                            //Print the status in the serial monitor.
    delay(500);                                                                 //Delay by 500 milliseconds.
  }

  Serial.println();                                                             //Print this in the serial monitor.
  Serial.println(io.statusText());                                              //Print the status in the serial monitor.

}

void loop() {

  io.run();                                                                     //Keep the connection with Adafruit IO.

  current_Light = analogRead(PHOTORESISTOR_PIN);                                //Read the analog value from the photoresistor and assign it to current_Light.
  
  if(digitalRead(BUTTON_PIN) == LOW)                                            //Read the digital value from the button and if it's not pressed, execute the following.
    current_Button = true;                                                      //Assign the value of true to current_Button.
  else                                                                          //Else (the button is pressed), execute the following.
    current_Button = false;                                                     //Assign the value of false to current_Button.

  if(current_Light == last_Light and current_Button == last_Button)             //If both current_Light and current_Button have not changed, execute the following.
    return;                                                                     //Return nothing and start the loop again.
  
  Serial.print("sending photoresistor value -> ");                              //Print this in the serial monitor.
  Serial.println(current_Light);                                                //Print current_Light in the serial monitor.
  analog->save(current_Light);                                                  //Save current_Light to the analog feed.
  
  Serial.print("sending button status -> ");                                    //Print this in the serial monitor.
  Serial.println(current_Button);                                               //Print current_Button in the serial monitor.
  digital->save(current_Button);                                                //Save current_Button to the digital feed.
  
  last_Light = current_Light;                                                   //Set last_Light to current_Light.
  last_Button = current_Button;                                                 //Set last_Button to current_Button.

  delay(10000);                                                                 //Delay by 10000 milliseconds.

}
