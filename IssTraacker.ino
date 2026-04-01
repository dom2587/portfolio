// ISS Tracker
// This code is ran on an ESP32 with an OLED screen attached and will display the ISS position in real time.
// It will also shorten some names to allow them to be displayed correctly


#include <WiFi.h>        // Include the Wi-Fi library
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <TimeLib.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>

WiFiClient client;  // or WiFiClientSecure for HTTPS
HTTPClient http;

/* Uncomment the initialize the I2C address , uncomment only one, If you get a totally blank screen try the other*/
#define i2c_Address 0x3c //initialize with the I2C addr 0x3C Typically eBay OLED's
//#define i2c_Address 0x3d //initialize with the I2C addr 0x3D Typically Adafruit OLED's

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET -1   //   QT-PY / XIAO
Adafruit_SH1106G display = Adafruit_SH1106G(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);


#define NUMFLAKES 10
#define XPOS 0
#define YPOS 1
#define DELTAY 2


#define LOGO16_GLCD_HEIGHT 16
#define LOGO16_GLCD_WIDTH  16
static const unsigned char PROGMEM logo16_glcd_bmp[] =
{ B00000000, B11000000,
  B00000001, B11000000,
  B00000001, B11000000,
  B00000011, B11100000,
  B11110011, B11100000,
  B11111110, B11111000,
  B01111110, B11111111,
  B00110011, B10011111,
  B00011111, B11111100,
  B00001101, B01110000,
  B00011011, B10100000,
  B00111111, B11100000,
  B00111111, B11110000,
  B01111100, B11110000,
  B01110000, B01110000,
  B00000000, B00110000
};
              
const char* ssid     = "home";         // The SSID (name) of the Wi-Fi network you want to connect to
const char* password = "dfg25878";     // The password of the Wi-Fi network

void setup() {
  Serial.begin(115200);         // Start the Serial communication to send messages to the computer
  delay(250); // wait for the OLED to power up
  display.begin(i2c_Address, true); // Address 0x3C default
  Serial.println('\n');
  display.display();
  delay(2000);

  // Clear the buffer.
  display.clearDisplay();
  
  WiFi.begin(ssid, password);             // Connect to the network
  Serial.print("Connecting to ");
  Serial.print(ssid);

  while (WiFi.status() != WL_CONNECTED) { // Wait for the Wi-Fi to connect
    delay(500);
    Serial.print('.');
  }

  Serial.println('\n');
  Serial.println("Connection established!");  
  Serial.print("IP address:\t");
  Serial.println(WiFi.localIP());         // Send the IP address of the ESP8266 to the computer
}

void loop() { 
  // api request for long + lat
  http.setFollowRedirects(HTTPC_FORCE_FOLLOW_REDIRECTS);  //force redirect follow
  http.begin(client, "http://api.open-notify.org/iss-now.json");
  http.GET();
  
  //create string
  String location =(http.getString());
  Serial.println(location);

  StaticJsonDocument<192> doc;

  DeserializationError error = deserializeJson(doc, location);

  if (error) {
  Serial.print("deserializeJson() failed: ");
  Serial.println(error.c_str());
  return;
}

long timestamp = doc["timestamp"]; // 1699010995
const char* message = doc["message"]; // "success"

const char* iss_position_longitude = doc["iss_position"]["longitude"]; // "-85.8464"
const char* iss_position_latitude = doc["iss_position"]["latitude"]; // "43.8316"

  // Disconnect
  http.end();



  // convert time
  int timec(timestamp);
  
  int h = hour(timec);
  int m = minute(timec);
  Serial.println(h);
  Serial.println(m);

  //  reverse geocoding


  // convert char to string 

  String lat(iss_position_latitude);

  String lon(iss_position_longitude);

  String url = "http://api-bdc.net/data/reverse-geocode?latitude=" + lat + "&longitude=" + lon + "&localityLanguage=en&key=bdc_521f578cb67b4589a4c1293328b7f67c";
  Serial.println(url);
  Serial.setTimeout(10000);

  http.begin(client, url);
  http.GET();

  String georeturn =(http.getString());  


  //reverse geocode json

  DynamicJsonDocument doc2(3072);
  deserializeJson(doc2, georeturn);

  JsonObject obj = doc2.as<JsonObject>();

  DeserializationError error2 = deserializeJson(doc2, georeturn);

  const char* countryName = doc2["countryName"];
  const char* locality = doc2["locality"];
  
  JsonArray localityInfo_informative = doc2["localityInfo"]["informative"];
  JsonObject localityInfo_informative_0 = localityInfo_informative[0];

  const char* localityInfo_informative_0_name = localityInfo_informative_0["name"];

  JsonObject localityInfo_informative_1 = localityInfo_informative[1];

  const char* localityInfo_informative_1_name = localityInfo_informative_1["name"]; 

  JsonObject localityInfo_informative_2 = localityInfo_informative[2];

  const char* localityInfo_informative_2_name = localityInfo_informative_2["name"];


  

  
  Serial.println(countryName);
  Serial.println(locality);
  Serial.println(localityInfo_informative_0_name);
  Serial.println(localityInfo_informative_1_name);

  String namell(countryName);
  String name0(localityInfo_informative_0_name);
  String local(locality);
  String name00(name0.substring(0,21));

  //int namel = namell.toInt();
  int namel = namell.length();
  Serial.println(namel);
  
   
 // rename main name

  Serial.println("namell 1 =" + namell);
  if (namell.equals("Russian Federation (the)")) {
    namell="Russia";
  } 
  else if(namell.equals("Korea (the Republic of)")) {
    namell="Korea";
  }
  else if(namell.equals("United States of America (the)")) {
    namell="USA";
  }
  else if(namell.equals("China")) {
    namell="China";
  }
  else if (namell.equals("United Kingdom of Great Britain and Northern Ireland (the)")) {
    namell="United Kingdom";
  }
  else if(namell.equals("Iran (Islamic Republic of)")){
    namell="Iran";
  }
    else if(namell.equals("Tanzania, the United Republic of")){
    namell="Tanzania";
  }
    else if(namell.equals("Central African Republic (the)")){
    namell="Central Africa";
  }
    else if(namell.equals("United Arab Emirates (the)")){
    namell="UAE";
  }
    else if(namell.equals("Netherlands (the)")){
    namell="Netherlands";
  }
  Serial.println("namell 2 = " + namell);


  // rename region
  if (local.equals("Wales [Cymru GB-CYM]")) {
    local="Wales";
  }

  // shorten local
  String local0(local.substring(0,21));


    // Disconnect
  http.end();

    // Display on scren

  display.setTextSize(1);
  display.setTextColor(SH110X_WHITE);
  display.setCursor(32,0);
  display.println("ISS Tracker"); // title
  display.println(""); // blank line 

  //display long + lat
  display.println("Longitude " + lon);
  display.println("Latuide   " + lat);
  display.println("");  

  // Top location line
  if (namel != 0 ) {display.println(namell);}
  else if (name0.equals("") || name0.equals("Asia") || name0.equals("North America") || name0.equals("South America") || name0.equals("Africa") || name0.equals("Insluar Oceania") || name0.equals("Europe")){display.println(localityInfo_informative_1_name);}
  else if (name0.equals("Australian continent")){display.println(localityInfo_informative_2_name);}
  else {display.println(name00);}

  // 2nd location line 
  if (local.indexOf("GMT") >0){display.println("");}
  else {display.println(local0);}
  
  // display time
  display.print(h);
  display.print(":");
  if (m<10) {display.print("0");}
  display.print(m);
  
  display.display();
  delay(120000);
  display.clearDisplay();

}
