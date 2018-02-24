/*************************************************************
  Download latest Blynk library here:
    https://github.com/blynkkk/blynk-library/releases/latest

  Blynk is a platform with iOS and Android apps to control
  Arduino, Raspberry Pi and the likes over the Internet.
  You can easily build graphic interfaces for all your
  projects by simply dragging and dropping widgets.

    Downloads, docs, tutorials: http://www.blynk.cc
    Sketch generator:           http://examples.blynk.cc
    Blynk community:            http://community.blynk.cc
    Follow us:                  http://www.fb.com/blynkapp
                                http://twitter.com/blynk_app

  Blynk library is licensed under MIT license
  This example code is in public domain.

  Virtual Pins for reference:
  V2 -> IPs of potential VLC locations
  V3 -> Alexa location and service response
  V4 -> Netflix last show ID
  V5 -> Location of video file for VLC player

  Service IDs for reference:
  1 -> Netflix
  2 -> Spotify
  3 -> VLC
 *************************************************************/
#define BLYNK_PRINT Serial


#include <ESP8266_Lib.h>

//For Wifi shields use this:
#include <BlynkSimpleShieldEsp8266.h>
//For other ESP8266 (untested) use the following line instead:
//#include <BlynkSimpleEsp8266.h>

// You should get Auth Token in the Blynk App.
// Go to the Project Settings (nut icon).
char auth[] = "8f284db453644ea2be0ca5a18b220848";

// Your WiFi credentials.
// Set password to "" for open networks.
char ssid[] = "elisha1";
char pass[] = "elisha100";

//Define your device IPs, !!if you have less leave them as 0.0.0.0!!
char livingroom[] = "10.0.0.12"; //ID 4
char bedroom[] = "10.0.0.15"; //ID 5
char computer[] = "0.0.0.0"; //ID 6

String all = (String)livingroom + "." + bedroom + "." + computer;

// Hardware Serial on Mega, Leonardo, Micro...
//#define EspSerial Serial1

// or Software Serial on Uno, Nano...
#include <SoftwareSerial.h>
//Most Wifi shields will use port 2,3, Sparkfun wifi shield uses ports 8,9
//SoftwareSerial EspSerial(2, 3); // RX, TX
SoftwareSerial EspSerial(8, 9); // RX, TX

// Set your ESP8266 baud rate:
#define ESP8266_BAUD 9600

ESP8266 wifi(&EspSerial);

//Blynk writes here whenever a value is updated in the cloud
BLYNK_WRITE(V3)
{
  uint8_t buffer[128] = {0};

  int pinValue = param.asInt(); // assigning incoming value from pin V3 to a variable
  
  String val = String(pinValue);
  char service = val.charAt(0);
  char location = val.charAt(1);
  Serial.print(service);
  Serial.print(location);


  //Update cloud with potential IPs
  Blynk.virtualWrite(V2,all);

  //Parse all locations and services
  if (location == '4' && livingroom != "0.0.0.0")
  {
      static uint8_t mux_id = 0;
    if (wifi.createTCP(mux_id, livingroom, 13000)) {
        Serial.println("create tcp ");
        Serial.print(mux_id);
        Serial.println(" ok");
    } else {
        Serial.println("create tcp ");
        Serial.print(mux_id);
        Serial.println(" err");
    }
    if (service == '1')
    {
          if (wifi.send(mux_id, (const uint8_t*)"1-EOF-", 6)) {
              Serial.println("send ok");
          } else {
              Serial.println("send err");
          }

          if (wifi.releaseTCP(mux_id)) {
            Serial.print("release tcp ");
            Serial.print(mux_id);
            Serial.println(" ok");
        } else {
            Serial.print("release tcp ");
            Serial.print(mux_id);
            Serial.println(" err");
            restartAP();
        } 
    }
    else if (service == '2')
    {
      if (wifi.send(mux_id, (const uint8_t*)"2-EOF-", 6)) {
              Serial.println("send ok");
          } else {
              Serial.println("send err");
          }

          if (wifi.releaseTCP(mux_id)) {
            Serial.print("release tcp ");
            Serial.print(mux_id);
            Serial.println(" ok");
        } else {
            Serial.print("release tcp ");
            Serial.print(mux_id);
            Serial.println(" err");
            restartAP();
        }
    }
    else if (service == '3')
    {
      if (wifi.send(mux_id, (const uint8_t*)"3-EOF-", 6)) {
              Serial.println("send ok");
          } else {
              Serial.println("send err");
          }
          
          if (wifi.releaseTCP(mux_id)) {
            Serial.print("release tcp ");
            Serial.print(mux_id);
            Serial.println(" ok");
        } else {
            Serial.print("release tcp ");
            Serial.print(mux_id);
            Serial.println(" err");
            restartAP();
        }
    }
  }
  else if (location == '5' && bedroom != "0.0.0.0")
  {
          static uint8_t mux_id = 1;
    if (wifi.createTCP(mux_id, bedroom, 13000)) {
        Serial.println("create tcp ");
        Serial.print(mux_id);
        Serial.println(" ok");
    } else {
        Serial.println("create tcp ");
        Serial.print(mux_id);
        Serial.println(" err");
    }

    if (service == '1')
    {
          if (wifi.send(mux_id, (const uint8_t*)"1-EOF-", 6)) {
              Serial.println("send ok");
          } else {
              Serial.println("send err");
          }
          if (wifi.releaseTCP(mux_id)) {
            Serial.print("release tcp ");
            Serial.print(mux_id);
            Serial.println(" ok");
        } else {
            Serial.print("release tcp ");
            Serial.print(mux_id);
            Serial.println(" err");
            restartAP();
        }
    }
    else if (service == '2')
    {
      if (wifi.send(mux_id, (const uint8_t*)"2-EOF-", 6)) {
              Serial.println("send ok");
          } else {
              Serial.println("send err");
          }
          if (wifi.releaseTCP(mux_id)) {
            Serial.print("release tcp ");
            Serial.print(mux_id);
            Serial.println(" ok");
        } else {
            Serial.print("release tcp ");
            Serial.print(mux_id);
            Serial.println(" err");
            restartAP();
        }
    }
    else if (service == '3')
    {
      if (wifi.send(mux_id, (const uint8_t*)"3-EOF-\n", 6)) {
              Serial.println("send ok");
          } else {
              Serial.println("send err");
          }
          if (wifi.releaseTCP(mux_id)) {
            Serial.print("release tcp ");
            Serial.print(mux_id);
            Serial.println(" ok");
        } else {
            Serial.print("release tcp ");
            Serial.print(mux_id);
            Serial.println(" err");
            restartAP();
        }
          
    }
    
  }
  else if (location == '6' && computer != "0.0.0.0")
  {
          static uint8_t mux_id = 2;
    if (wifi.createTCP(mux_id, computer, 13000)) {
        Serial.println("create tcp ");
        Serial.print(mux_id);
        Serial.println(" ok");
    } else {
        Serial.println("create tcp ");
        Serial.print(mux_id);
        Serial.println(" err");
    }

    if (service == '1')
    {
          if (wifi.send(mux_id, (const uint8_t*)"1-EOF-", 6)) {
              Serial.println("send ok");
          } else {
              Serial.println("send err");
          }
          if (wifi.releaseTCP(mux_id)) {
            Serial.print("release tcp ");
            Serial.print(mux_id);
            Serial.println(" ok");
        } else {
            Serial.print("release tcp ");
            Serial.print(mux_id);
            Serial.println(" err");
            restartAP();
        }
    }
    else if (service == '2')
    {
      if (wifi.send(mux_id, (const uint8_t*)"2-EOF-", 6)) {
              Serial.println("send ok");
          } else {
              Serial.println("send err");
          }
          if (wifi.releaseTCP(mux_id)) {
            Serial.print("release tcp ");
            Serial.print(mux_id);
            Serial.println(" ok");
        } else {
            Serial.print("release tcp ");
            Serial.print(mux_id);
            Serial.println(" err");
            restartAP();
        }
    }
    else if (service == '3')
    {
      if (wifi.send(mux_id, (const uint8_t*)"3-EOF-", 6)) {
              Serial.println("send ok");
          } else {
              Serial.println("send err");
          }
          if (wifi.releaseTCP(mux_id)) {
            Serial.print("release tcp ");
            Serial.print(mux_id);
            Serial.println(" ok");
        } else {
            Serial.print("release tcp ");
            Serial.print(mux_id);
            Serial.println(" err");
            restartAP();
        }
    } 
  }  
}

//fallback in case TCP connection does not close
void restartAP()
{
  
        if (wifi.leaveAP()) {
        Serial.print("leave AP success\r\n");
        Serial.print("IP: ");       
        Serial.println(wifi.getLocalIP().c_str());
    } else {
        Serial.print("leaveAP failure\r\n");
    }
    
       if (wifi.joinAP(ssid, pass)) {
        Serial.print("Join AP success\r\n");
        Serial.print("IP: ");       
        Serial.println(wifi.getLocalIP().c_str());
    } else {
        Serial.print("Join AP failure\r\n");
    }
}

void setup()
{
  
  // Debug console
  Serial.begin(9600);

  delay(100);

  // Set ESP8266 baud rate
  EspSerial.begin(ESP8266_BAUD);
  delay(100);

    if (wifi.joinAP(ssid, pass)) {
        Serial.print("Join AP success\r\n");
        Serial.print("IP: ");       
        Serial.println(wifi.getLocalIP().c_str());
    } else {
        Serial.print("Join AP failure\r\n");
    }

    if (wifi.enableMUX()) {
        Serial.print("mux enabled");
       Blynk.config(wifi, auth);
    } else {
       Serial.print("Software will probably not work, multiple conections (MUX) needs to be enabled");
       Blynk.begin(auth, wifi, ssid, pass);
    }

}

void loop()
{
  Blynk.run();
}

