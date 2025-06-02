/*
  This example connects to an unencrypted WiFi network.
  Then it prints the MAC address of the WiFi module,
  the IP address obtained, and other network details.

  created 13 July 2010
  by dlf (Metodo2 srl)
  modified 31 May 2012
  by Tom Igoe

  Find the full UNO R4 WiFi Network documentation here:
  https://docs.arduino.cc/tutorials/uno-r4-wifi/wifi-examples#connect-with-wpa
 */

/*
  This file has been modified to suit the needs of SPRO2-MC at the University of Southern Denmark by BSc Group 3
*/
#include <WiFiS3.h>
#include <Wire.h>
#include <string>

WiFiClient client;
int data_bus[6];

#include "arduino_secrets.h" 

char ssid[] = SECRET_SSID;        // your network SSID (name)
char pass[] = SECRET_PASS;    // your network password (use for WPA, or use as key for WEP)
int status = WL_IDLE_STATUS;     // the WiFi radio's status

volatile int RPM, ANGLE, TIME;
volatile bool dataReady = false;

void setup() {
  //Initialize serial and wait for port to open:
  Serial.begin(57600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  // check for the WiFi module:
  if (WiFi.status() == WL_NO_MODULE) {
    Serial.println("Communication with WiFi module failed!");
    // don't continue
    while (true);
  }

  String fv = WiFi.firmwareVersion();
  if (fv < WIFI_FIRMWARE_LATEST_VERSION) {
    Serial.println("Please upgrade the firmware");
  }

  // attempt to connect to WiFi network:
  while (status != WL_CONNECTED) {
    Serial.print("Attempting to connect to WPA SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network:
    status = WiFi.begin(ssid, pass);

    // wait 10 seconds for connection:
    delay(10000);
  }

  // you're connected now, so print out the data:
  Serial.print("You're connected to the network");
  printCurrentNet();
  printWifiData();

  // Join the I2C bus as a slave device.
  Wire.begin(0x69);
  Wire.onReceive(onMasterData);
}

void loop() {
  if (dataReady){
    sendHttpRequest("/data/bus", "?time=" + String(TIME) + "&angle=" + String(ANGLE) + "&speed=" + String(RPM));
    dataReady = false;
  } 
}

void sendHttpRequest(String PATH_NAME, String QUERY) {
  int       HTTP_PORT   = 3000;
  String    HTTP_METHOD = "GET";
  IPAddress HOST_NAME(172, 20, 10, 3); 

  if (client.connect(HOST_NAME, HTTP_PORT)) {
    Serial.println("Connected successfully!");

    String req = HTTP_METHOD + " " + PATH_NAME + QUERY + " HTTP/1.1";
    Serial.println(req);
    // send HTTP request header
    client.println(req);
    client.println("Connection: close");
    client.println("Host: " + HOST_NAME.toString() + ":" + HTTP_PORT);
    client.println(); // end HTTP request header

    if (QUERY) client.println(QUERY);

    while (client.connected())
    {
      if (client.available())
      {
        // read an incoming byte from the server and print them to serial monitor:
        char c = client.read();
        Serial.print(c);
      }
    }

    client.stop();
    Serial.println("\nDisconnected");
  } else {
    Serial.println("Connection failed");
  }
}

void onMasterData(int numBytes) {
  int n = 0;

  //Serial.println("Hello World!");

  while (Wire.available() && n < 6) {
    data_bus[n] = Wire.read();
    n++;
    //RPM HI/LO    
    //ANGLE HI/LO  deg
    //TIME HI/LO * 5ms
  }

  RPM = (data_bus[0] << 8) + data_bus[1];
  ANGLE = (data_bus[2] << 8) + data_bus[3];
  TIME = ((data_bus[4] << 8) + data_bus[5]) * 5;

  dataReady = true;
}

void printWifiData() {
  // print your board's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  
  Serial.println(ip);

  // print your MAC address:
  byte mac[6];
  WiFi.macAddress(mac);
  Serial.print("MAC address: ");
  printMacAddress(mac);
}

void printCurrentNet() {
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print the MAC address of the router you're attached to:
  byte bssid[6];
  WiFi.BSSID(bssid);
  Serial.print("BSSID: ");
  printMacAddress(bssid);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.println(rssi);

  // print the encryption type:
  byte encryption = WiFi.encryptionType();
  Serial.print("Encryption Type:");
  Serial.println(encryption, HEX);
  Serial.println();
}

void printMacAddress(byte mac[]) {
  for (int i = 0; i < 6; i++) {
    if (i > 0) {
      Serial.print(":");
    }
    if (mac[i] < 16) {
      Serial.print("0");
    }
    Serial.print(mac[i], HEX);
  }
  Serial.println();
}
