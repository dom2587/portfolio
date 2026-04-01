// ISS Tracker


#include <WiFi.h>        // Include the Wi-Fi library

const char* ssid     = "home";         // The SSID (name) of the Wi-Fi network you want to connect to
const char* ssid = "home" // SSID duh
const char* password = "dfg25878";     // The password of the Wi-Fi network

void setup() {
  Serial.begin(115200);         // Start the Serial communication to send messages to the computer
  delay(10);
  Serial.println('\n');
  
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
  // Send request
  http.begin(client, "http://api.open-notify.org/iss-now.json");
  http.GET();

  // Print the response
  Serial.print(http.getString());  
}
