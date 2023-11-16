
#include <ArduinoJson.h>
#include "DHT.h"
#include <HTTPClient.h>
#include <time.h>
#include <WiFi.h>
#include "WiFiCredentials.h"


#define DHTPIN 4    
#define DHTTYPE DHT22

DHT dht(DHTPIN, DHTTYPE);


const char* ssid = WiFiCredentials::SSID; 
const char* password = WiFiCredentials::PASSWORD;

const char* serverName = "http://192.168.x.x:8000/api/datacollection/datatransmission/"; 


// NTP Server and timezone
const char* ntpServer = "nz.pool.ntp.org";
const long gmtOffset_sec = 36000; // NZST is UTC +10 hours
const int daylightOffset_sec = 3600; // Daylight saving time offset


void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");

  dht.begin();

  // Initialize NTP
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  Serial.println("Waiting for NTP time sync: ");
  delay(2000); // wait a bit for time sync
}

void loop() {
  // Delay for 15 minutes
  delay(1 * 60 * 1000);

char uuidStr[37]; // UUIDs are 36 characters plus null terminator
sprintf(uuidStr, "%08X-%08X-%08X-%08X", esp_random(), esp_random(), esp_random(), esp_random());
String transmission_uuid = String(uuidStr);

  // Get current time
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    Serial.println("Failed to obtain time");
    return;
  }
  char timeStr[32];
  strftime(timeStr, sizeof(timeStr), "%Y-%m-%dT%H:%M:%SZ", &timeinfo);

  // Read data from the DHT sensor
  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();

  // Round temperature and humidity to 2 decimal places
  temperature = round(temperature * 100.0) / 100.0;
  humidity = round(humidity * 100.0) / 100.0;

  // Convert temperature and humidity to strings with 2 decimal places - enforcing length to never more than 5 digits
  char tempStr[6];
  char humStr[6];
  snprintf(tempStr, sizeof(tempStr), "%.2f", temperature);
  snprintf(humStr, sizeof(humStr), "%.2f", humidity);

 
    
 // Create JSON object
DynamicJsonDocument doc(1024);

doc["api_key"] = "12762495-bcc0-4133-ba25-e2ceab5ae11c";
doc["transmission_uuid"] = transmission_uuid;
doc["transmission_tries"] = 1;
doc["start_timestamp"] = "2023-10-18T02:45:00Z";
doc["end_timestamp"] = "2023-10-18T02:45:00Z";
doc["software_version"] = 1;
doc["battery"] = 5;
doc["type"] = "esp32-test";

// Create 'data' as an array
JsonArray dataArray = doc.createNestedArray("data");

// Add sensor data object to 'data' array
JsonObject data_0 = dataArray.createNestedObject();
data_0["hive_id"] = 1;
JsonArray sensors = data_0.createNestedArray("sensors");

// Temperature sensor
JsonObject sensor_temp = sensors.createNestedObject();
sensor_temp["sensor_id"] = "dfa0c8d3-56f8-4c3f-a0fd-036b43d1d824";
sensor_temp["type"] = "temp";
JsonArray readings_temp = sensor_temp.createNestedArray("readings");
JsonObject reading_temp = readings_temp.createNestedObject();
reading_temp["timestamp"] = timeStr;
reading_temp["value"] = tempStr;  // Use the formatted temperature string

// Humidity sensor
JsonObject sensor_humidity = sensors.createNestedObject();
sensor_humidity["sensor_id"] = "6da59cec-c9c6-4752-b3cc-0d3a3bb98e98";
sensor_humidity["type"] = "humidity";
JsonArray readings_humidity = sensor_humidity.createNestedArray("readings");
JsonObject reading_humidity = readings_humidity.createNestedObject();
reading_humidity["timestamp"] = timeStr;
reading_humidity["value"] = humStr;  // Use the formatted humidity string
// Convert JSON object to String
String requestBody;
serializeJson(doc, requestBody);

  // Post data to the server
  if(WiFi.status() == WL_CONNECTED){
    HTTPClient http;
    http.begin(serverName);
    http.addHeader("Content-Type", "application/json");
    int httpResponseCode = http.POST(requestBody);

    if (httpResponseCode > 0) {
      String response = http.getString();
      Serial.println(httpResponseCode);
      Serial.println(response);
    }
    else {
      Serial.print("Error on sending POST: ");
      Serial.println(httpResponseCode);
    }

    http.end();
  }
  else {
    Serial.println("WiFi Disconnected");
  }
}



