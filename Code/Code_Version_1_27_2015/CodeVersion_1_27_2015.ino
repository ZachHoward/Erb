/*
Arduino code for Erb the Smart Garden. 

Written By: Zach Howard
            zacharyianhoward@gmail.com


Lots of thanks to Adafruit for their amazing Neopixel, DHT22 and TSL2561 libraries, and Arduino for their GSM library. 

*/
//-----Add Libraries-----//
#include <GSM.h>
#include <Adafruit_NeoPixel.h>
#include <Wire.h>
#include "TSL2561.h"
#include "DHT.h"
#include "pitches.h"
#include<stdlib.h>

//-----Defines-----//
#define PINNUMBER ""
#define PIXEL_PIN 6   
#define DHTPIN 12 
#define DHTTYPE DHT22   
#define PIXEL_COUNT 1  // If more neopixels get added update this number

//-----Initialization-----//
Adafruit_NeoPixel strip = Adafruit_NeoPixel(PIXEL_COUNT, PIXEL_PIN, NEO_GRB + NEO_KHZ800);
DHT dht(DHTPIN, DHTTYPE);
TSL2561 tsl(TSL2561_ADDR_FLOAT); 
GSM gsmAccess;
GSM_SMS sms;

// Array to hold the number a SMS is retreived from
char senderNumber[20]; 
int motorPin = 11;

void setup() 
{
  // initialize serial communications and wait for port to open:
  Serial.begin(9600);
  pinMode( motorPin, OUTPUT); // Motor pin Setup
  strip.begin(); // Neopixel Setup
  strip.show(); // Initialize all pixels to 'off'
  dht.begin(); // Temp Sensor (DHT22) setup
  tsl.begin(); // Light temsor 
  tsl.setGain(TSL2561_GAIN_16X); 
  tsl.setTiming(TSL2561_INTEGRATIONTIME_402MS);
  //Serial.println("SMS Messages Receiver");
    
  // connection state
  boolean notConnected = true;
  // Start GSM connection
  while(notConnected)
  {
    if(gsmAccess.begin(PINNUMBER)==GSM_READY)
      notConnected = false;
    else
    {
      Serial.println("Not connected");
      delay(500);
    }
  }
  
  Serial.println("GSM initialized");
  Serial.println("Waiting for messages");
}

void loop() 
{
  colorWipe(strip.Color(0, 255, 0), 50); // Green
  char c;
  String inputcommand="";
  
  // If there are any SMSs available()  
  if (sms.available())
  {
    Serial.println("Message received from:");
    
    // Get remote number
    sms.remoteNumber(senderNumber, 20);
    Serial.println(senderNumber);

    // An example of message disposal    
    // Any messages starting with # should be discarded
    if(sms.peek()=='#')
    {
      Serial.println("Discarded SMS");
      sms.flush();
    }
    
    // Read message bytes and print them
    while(c=sms.read())
      inputcommand+=c;
      //Serial.print(c);
    
    //Serial.println("\nEND OF MESSAGE");
    //Serial.print("Before Flush:");
    //Serial.println(sms.ready());
    
    // Delete message from modem memory
    sms.flush();
    Serial.println("MESSAGE DELETED");
    Serial.print("After Flush:");
    Serial.println(sms.ready());
    
    // Repeat saved message
    Serial.println(inputcommand);
    
    Serial.print("Before Text:");
    Serial.println(sms.ready());
    // Send back a confirmation
    sms.beginSMS(senderNumber);
    delay(100);
    
    if(findWord("water",inputcommand)){
      // Put code to activate motor pin here & blink light
      //sms.print("Starting to Water");
      colorWipe(strip.Color(0, 0, 255), 50);  // Blue
      //delay(1000);
      //sms.endSMS(); 
      //delay(500);
      activateWatering(motorPin,5);
      //sms.beginSMS(senderNumber);
      //delay(500);
      sms.print("Thanks for the drink!");
    }
    else if(findWord("light",inputcommand)){
      colorWipe(strip.Color(0, 0, 0), 50);    // Black/off
      uint16_t x = tsl.getLuminosity(TSL2561_VISIBLE);  
      String stringOne = "The lumosity is: " ;
      String stringThree = stringOne + x;
      // Put code to activate motor pin here & blink light
      sms.print(stringThree);  
    }
    else if(findWord("temp",inputcommand)){
      colorWipe(strip.Color(0, 0, 0), 50);
      char buffer[10];
      float h = dht.readHumidity();
      float f = dht.readTemperature(true); // Read temperature as Fahrenheit
      String dht_stringOne = "The temp is: " ;
      String dht_stringThree = dht_stringOne + dtostrf(f, 5, 1, buffer);
      sms.print(dht_stringThree); 
    }
    
    else if(findWord("hello",inputcommand)){
      sms.print("Hello my name is Erb, lets grow some plants!");
      colorWipe(strip.Color(255, 128, 255), 50);
      delay(100);  
    }
    else if(findWord("error",inputcommand)){
      colorWipe(strip.Color(255, 0, 0), 50);
      delay(1000);
      sms.print("You are experiencing an error....");  
    }
    
    else {
      sms.print("I dont know what that means :( ");
    }
    
    // delay and send
    delay(250);
    sms.endSMS(); 
    delay(100);
    
    //Serial.print("After Text:");
    //Serial.println(sms.ready())
    //Serial.println("\nCOMPLETE!\n");
  }
  //Serial.print("Before delay: ");
  //Serial.println(sms.ready());
  //delay(1000);
  //Serial.print("After delay:");
  //Serial.println(sms.ready());
  }


 int findWord(String searchWord, String sentence) {
   // first check to see if the first letter of the word is in the sentence
   searchWord.toLowerCase();
   sentence.toLowerCase();
   int firstletterpos = sentence.indexOf(searchWord[0]);
   Serial.println(firstletterpos);
     if (sentence.startsWith(searchWord,firstletterpos)){
       return true;
     }
     return false;
 }
 
 int activateWatering(int motor_pin,int delaySeconds){
   digitalWrite(motor_pin,HIGH);
   play_melody();
   delay(delaySeconds*1000);
   digitalWrite(motor_pin,LOW);
 }

// Fill the dots one after the other with a color
void colorWipe(uint32_t c, uint8_t wait) {
  for(uint16_t i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, c);
      strip.show();
      delay(wait);
   }
 }
 
 void play_melody(){
   int melody[] = {NOTE_F4, NOTE_A4,NOTE_B4,NOTE_F4, NOTE_A4,NOTE_B4,NOTE_F4,NOTE_A4,NOTE_B4,NOTE_E5,NOTE_D5,NOTE_B4,NOTE_C5,NOTE_B4,NOTE_G4,NOTE_E4,NOTE_D4,NOTE_E4,NOTE_G4,NOTE_E4};
   int noteDurations[] = {8, 8, 4, 8, 8, 4, 8, 8, 8, 8, 4, 8, 8, 8, 8, 3, 8, 8, 8, 2,8,8,8,2,8, 8, 4, 8, 8, 4, 8, 8, 8, 8, 4, 8, 8, 8, 8, 3, 8, 8, 8, 2,8,8,8,2 };
   int song_length= sizeof(melody);
   for (int thisNote = 0; thisNote < 20 ; thisNote++) {
    int noteDuration = 2000/noteDurations[thisNote];
    tone(8, melody[thisNote],noteDuration);
    int pauseBetweenNotes = noteDuration * 1.10;
    delay(pauseBetweenNotes);
    noTone(8);
  }
 }
 
/*  void play_hello(){
   int melody[] = {NOTE_F4, NOTE_A4,NOTE_B4,NOTE_F4, NOTE_A4,NOTE_B4,NOTE_F4,NOTE_A4,NOTE_B4,NOTE_E5,NOTE_D5,NOTE_B4,NOTE_C5,NOTE_B4,NOTE_G4,NOTE_E4,NOTE_D4,NOTE_E4,NOTE_G4,NOTE_E4,NOTE_F4, NOTE_A4,NOTE_B4,NOTE_F4, NOTE_A4,NOTE_B4,NOTE_F4,NOTE_A4,NOTE_B4,NOTE_E5,NOTE_D5,NOTE_B4,NOTE_C5,NOTE_B4,NOTE_G4,NOTE_E4,NOTE_D4,NOTE_E4,NOTE_G4,NOTE_E4};
   int noteDurations[] = {8, 8, 4, 8, 8, 4, 8, 8, 8, 8, 4, 8, 8, 8, 8, 3, 8, 8, 8, 2,8,8,8,2 };
   int song_length= sizeof(melody);
   for (int thisNote = 0; thisNote < 20 ; thisNote++) {
    int noteDuration = 2000/noteDurations[thisNote];
    tone(8, melody[thisNote],noteDuration);
    int pauseBetweenNotes = noteDuration * 1.10;
    delay(pauseBetweenNotes);
    noTone(8);
  }
 }
 */
/* Initialization Notes

DHT22: Initialize DHT sensor for normal 16mhz Arduino
Neopixel: 
Parameter 1 = number of pixels in strip,  neopixel stick has 8
Parameter 2 = pin number (most are valid)
Parameter 3 = pixel type flags, add together as needed:
   NEO_RGB     Pixels are wired for RGB bitstream
   NEO_GRB     Pixels are wired for GRB bitstream, correct for neopixel stick
   NEO_KHZ400  400 KHz bitstream (e.g. FLORA pixels)
   NEO_KHZ800  800 KHz bitstream (e.g. High Density LED strip), correct for neopixel stick
   */
