#include <SoftwareSerial.h>
#include "hc_sr04.h"

 
#define DEBUG true
#define RX 2
#define TX 3
 
SoftwareSerial esp8266(RX, TX);
Ultrasonic ultrasonic(7,8);

void setup()
{
  Serial.begin(115200);
  esp8266.begin(115200);
  
  pinMode(13, OUTPUT);

  sendData("AT+RST\r\n",2000,DEBUG);           // reset module
  sendData("AT+CWMODE=2\r\n",1000,DEBUG);      // configure as access point
  sendData("AT+CWLAP\r\n",1000,DEBUG);         // get ip address
  sendData("AT+CIFSR\r\n",1000,DEBUG);         // get ip address  
  sendData("AT+CIPMUX=1\r\n",1000,DEBUG);      // configure for multiple connections
  sendData("AT+CIPSERVER=1,80\r\n",1000,DEBUG);// turn on server on port 80
}
 
void blinkled()
{
  digitalWrite(13, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(500);              // wait for a second
  digitalWrite(13, LOW);    // turn the LED off by making the voltage LOW
}
void loop()
{
  //delay(3000);
  String data = "";
  data+=ultrasonic.Ranging();
  
  if(DEBUG)
  {
    //Serial.print(data + "\r\n");
  }

  sendWebData(data);
}

void sendWebData(String content)
{
    if(esp8266.available()) // check if the esp is sending a message 
  {    
    if(esp8266.find("+IPD,"))
    {
     //delay(1000);
 
     int connectionId = esp8266.read()-48; // subtract 48 because the read() function returns 
                                           // the ASCII decimal value and 0 (the first decimal number) starts at 48
     
     String webpage = content;
 
     String cipSend = "AT+CIPSEND=";
     cipSend += connectionId;
     cipSend += ",";
     cipSend +=webpage.length();
     cipSend +="\r\n";
     
     sendData(cipSend,1000,DEBUG);
     sendData(webpage,1000,DEBUG);
 
     String closeCommand = "AT+CIPCLOSE="; 
     closeCommand+= connectionId;
     closeCommand+="\r\n";
     
     sendData(closeCommand,3000,DEBUG);
    }
  }

}
 
 
String sendData(String command, const int timeout, boolean debug)
{
    String response = "";
    
    esp8266.print(command); // send the read character to the esp8266
    
    long int time = millis();
    
    while( (time+timeout) > millis())
    {
      while(esp8266.available())
      {
        
        // The esp has data so display its output to the serial window 
        char c = esp8266.read(); // read the next character.
        response+=c;
      }  
    }
    
    if(debug)
    {
      //Serial.print(response);
      blinkled();
    }
    
    return response;
}
 
