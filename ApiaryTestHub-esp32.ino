#include <WiFi.h>
#include "WiFiCredentials.h"
#include "DHT.h"

#define DHTPIN 4    
#define DHTTYPE DHT22

DHT dht(DHTPIN, DHTTYPE);


const char* ssid = WiFiCredentials::SSID; 
const char* password = WiFiCredentials::PASSWORD;  

void setup() {
  Serial.begin(115200);
  Serial.println(F("DHTxx test!"));

  dht.begin();

  // Connect to Wi-Fi
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected!");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

void sendPostRequest() {
    HTTPClient http;

    // Your JSON data
    String jsonData = "{\"key1\":\"value1\",\"key2\":\"value2\"}";

    http.begin(serverName); // Specify the URL
    http.addHeader("Content-Type", "application/json"); // Specify content-type header

    int httpResponseCode = http.POST(jsonData); // Send the actual POST request

    if (httpResponseCode > 0) {
        // If response, print out the response to the serial port
        String response = http.getString();
        Serial.println(httpResponseCode);
        Serial.println(response);
    } else {
        Serial.print("Error on sending POST: ");
        Serial.println(httpResponseCode);
    }

    http.end(); // Free resources
}

void loop() {
  // Nothing to do here
}
