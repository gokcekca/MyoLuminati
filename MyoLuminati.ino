/*
  WiFi Web Server LED Blink
 
 A UDP web server that lets you control LEDs via the net.
 Will print IP address and MAC address to the serial port
 once connected. Adapted from the SimpleWebServer and 
 WiFiUDPSendRecieveString example sketches.
 
 Recieves UDP packets, and parses them to get info for
 appropriate action. ASCII 'a' - 'y' and 'A' - 'C' are
 used in the first byte of transmitted packet to indicate
 actions or modes, and ASCII 0 - 255 are used in the second
 byte to encode light intensity values.
 */
#include <SPI.h>
#include <WiFi.h>
#include <WiFiUdp.h>
char ssid[] = "TP-LINK_2.4GHz_5B2BDE";
char pass[] = "tplink1234";

int redIntensity = 0;
int greenIntensity = 0;
int blueIntensity = 0;
unsigned long t;
unsigned int localPort = 2390;      // local port to listen on
char packetBuffer[1];               //buffer to hold incoming packet
char  ReplyBuffer[] = "acknowledged";// a string to send back
int status = WL_IDLE_STATUS;

WiFiUDP Udp;
void setup() {
  Serial.begin(9600);      // initialize serial communication
  pinMode(3, OUTPUT);      // set the LED pin mode
  pinMode(5, OUTPUT);      // set the LED pin mode
  pinMode(6, OUTPUT);      // set the LED pin mode

  // check for the presence of the shield:
  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("WiFi shield not present"); 
    while(true);        // don't continue
  } 

  String fv = WiFi.firmwareVersion();
  if( fv != "1.1.0" )
    Serial.println("Please upgrade the firmware");

  // attempt to connect to Wifi network:
  while ( status != WL_CONNECTED) { 
    Serial.print("Attempting to connect to Network named: ");
    Serial.println(ssid);                   // print the network name (SSID);

    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:    
    status = WiFi.begin(ssid, pass);
    // wait 10 seconds for connection:
    delay(10000);
  } 
  server.begin();                           // start the web server on port 80
  printWifiStatus();                        // you're connected now, so print out the status
  Serial.end();
  Udp.begin(localPort);  
}


void loop() {
  
  int packetSize = Udp.parsePacket();
  if(packetSize)
  {   
    //Serial.print("Received packet of size ");
    //Serial.println(packetSize);
    //Serial.print("From ");
    //IPAddress remoteIp = Udp.remoteIP();
    //Serial.print(remoteIp);
    //Serial.print(", port ");
    //Serial.println(Udp.remotePort());

    // read the packet into packetBufffer
    int len = Udp.read(packetBuffer,255);
    if (len >0) packetBuffer[len]=0;
    //Serial.println("Contents:");
    //Serial.println(packetBuffer);
   
    
        if (packetBuffer[0] == 'a') {
          analogWrite(3,1);
        }
        if (packetBuffer[0] == 'b') {
          analogWrite(3,255);
        }
        if (packetBuffer[0] == 'c') {
          analogWrite(5,1);
        }
        if (packetBuffer[0] == 'd') {
          analogWrite(5,255);
        }
        if (packetBuffer[0] == 'e') {
          analogWrite(6,1);
        }
        if (packetBuffer[0] == 'f') {
          analogWrite(6,255);
        }
        
        //Setting brightness of individual colors:
        if (packetBuffer[0] == 'g') {
          greenIntensity+=10;
          analogWrite(6,greenIntensity);
        }
        if (packetBuffer[0] == 'h') {
          greenIntensity-=10;
          analogWrite(6,greenIntensity);
        }
        if (packetBuffer[0] == 'i') {
          redIntensity+=10;
          analogWrite(6,redIntensity);
        }
        if (packetBuffer[0] == 'j') {
          redIntensity-=10;
          analogWrite(6,redIntensity);
        }
        if (packetBuffer[0] == 'k') {
          blueIntensity+=10;
          analogWrite(5,blueIntensity);
        }
        if (packetBuffer[0] == 'l') {
          blueIntensity-=10;
          analogWrite(5,blueIntensity);
        }
        
        //Setting brightness of all colors:
        if (packetBuffer[0] == 'm') {
          redIntensity+=40;
          greenIntensity+=40;
          blueIntensity+=40;
          analogWrite(3,redIntensity);
          analogWrite(6,greenIntensity);
          analogWrite(5,blueIntensity);
        }
        if (packetBuffer[0] == 'n') {
          redIntensity-=40;
          greenIntensity-=40;
          blueIntensity-=40;
          analogWrite(3,redIntensity);
          analogWrite(6,greenIntensity);
          analogWrite(5,blueIntensity);
        }
        //States to store for list of colors we will iterate through:
        if (packetBuffer[0] == 'q') {
          analogWrite(3,255);
          analogWrite(6,255);
          analogWrite(5,255);
        }        
        if (packetBuffer[0] == 'r') {
          analogWrite(3,1);
          analogWrite(6,255);
          analogWrite(5,255);
        }      
        if (packetBuffer[0] == 's') {
          analogWrite(3,255);
          analogWrite(6,255);
          analogWrite(5,1);
        }      
        if (packetBuffer[0] == 't') {
          analogWrite(3,255);
          analogWrite(6,1);
          analogWrite(5,255);
        }      
        if (packetBuffer[0] == 'u') {
          analogWrite(3,255);
          analogWrite(6,1);
          analogWrite(5,1);
        }      
        if (packetBuffer[0] == 'v') {
          analogWrite(3,1);
          analogWrite(6,255);
          analogWrite(5,1);
        }      
        if (packetBuffer[0] == 'w') {
          analogWrite(3,1);
          analogWrite(6,1);
          analogWrite(5,255);
        }      
        if (packetBuffer[0] == 'y') {
          analogWrite(3,1);
          analogWrite(6,1);
          analogWrite(5,1);
        }      
        //Code to turn all colors 'off':
        if (packetBuffer[0] == 'x') {
          redIntensity = 255- int(packetBuffer[1]*5);
          greenIntensity =255- int(packetBuffer[1]*5);
          blueIntensity = 255-int(packetBuffer[1]*5);
          analogWrite(3,redIntensity);
          analogWrite(6,greenIntensity);
          analogWrite(5,blueIntensity);
          //Serial.print(int(packetBuffer[1]));      //You can print the recieved value to serial
        }
        
        //Code to fade through colors:
        if (packetBuffer[0] == 'A') {
          redIntensity = 255- int(packetBuffer[1]*5);
          greenIntensity =int(packetBuffer[1]*5);
          blueIntensity = int(packetBuffer[1]*5);
          analogWrite(3,redIntensity);
          analogWrite(6,greenIntensity);
          analogWrite(5,blueIntensity);
          //Serial.print(int(packetBuffer[1]));
        }
        
        if (packetBuffer[0] == 'B') {
          redIntensity =  int(packetBuffer[1]*5);
          greenIntensity =int(packetBuffer[1]*5);
          blueIntensity = 255-int(packetBuffer[1]*5);
          analogWrite(3,redIntensity);
          analogWrite(6,greenIntensity);
          analogWrite(5,blueIntensity);
          //Serial.print(int(packetBuffer[1]));
        }        
        if (packetBuffer[0] == 'C') {
          redIntensity = int(packetBuffer[1]*5);
          greenIntensity =255- int(packetBuffer[1]*5);
          blueIntensity = -int(packetBuffer[1]*5);
          analogWrite(3,redIntensity);
          analogWrite(6,greenIntensity);
          analogWrite(5,blueIntensity);
          //Serial.print(int(packetBuffer[1]));
        } 
    // send a reply, to the IP address and port that sent us the packet we received
    Udp.beginPacket(Udp.remoteIP(), Udp.remotePort());
    Udp.write(ReplyBuffer);
    Udp.endPacket();
   }

  
}

void printWifiStatus() {
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your WiFi shield's IP address:
  byte mac[6];
  WiFi.macAddress(mac);
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.print(ip);
  Serial.print(mac[5],HEX);
  Serial.print(":");
  Serial.print(mac[4],HEX);
  Serial.print(":");
  Serial.print(mac[3],HEX);
  Serial.print(":");
  Serial.print(mac[2],HEX);
  Serial.print(":");
  Serial.print(mac[1],HEX);
  Serial.print(":");
  Serial.print(mac[0],HEX);
  

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
  Serial.println(ip);
}
