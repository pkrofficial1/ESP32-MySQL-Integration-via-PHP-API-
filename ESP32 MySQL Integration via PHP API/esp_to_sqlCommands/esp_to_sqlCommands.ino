#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

const char* ssid = "hostpot_name";
const char* password = "hostpot_password";

const char* serverName = "http://your_ip/esp32_api/sql_api.php"; // Update with your server's IP and path

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }

  Serial.println("Connected to WiFi");
  Serial.println("Enter your SQL query:");
}

void loop() {
  if (WiFi.status() == WL_CONNECTED) {
    if (Serial.available()) {
      String sqlQuery = Serial.readStringUntil('\n');
      sqlQuery.trim(); // Remove any extra whitespace

      if (sqlQuery.length() > 0) {
        HTTPClient http;
        http.begin(serverName);
        http.addHeader("Content-Type", "application/json");

        String jsonPayload = "{\"sql_query\": \"" + sqlQuery + "\"}";

        int httpResponseCode = http.POST(jsonPayload);

        if (httpResponseCode > 0) {
          String response = http.getString();
          
          // Print raw response for debugging
          Serial.println("Raw Server Response:");
          Serial.println(response);

          // Parse the JSON response
          DynamicJsonDocument doc(2048);
          DeserializationError error = deserializeJson(doc, response);

          if (error) {
            Serial.print("JSON deserialization failed: ");
            Serial.println(error.c_str());
            return;
          }

          // Check if the status is success
          if (doc["status"] == "success") {
            // Check if there's data
            if (doc.containsKey("data")) {
              JsonArray data = doc["data"].as<JsonArray>();

              for (JsonObject row : data) {
                for (JsonPair kv : row) {
                  Serial.print(kv.key().c_str());
                  Serial.print(": ");
                  Serial.println(kv.value().as<String>());
                }
                Serial.println("-----------------------------"); // Separate each row
              }
            } else {
              Serial.println(doc["message"].as<String>()); // Print the "No records found" message
            }
          } else {
            Serial.println("Error: " + doc["message"].as<String>());
          }
        } else {
          Serial.print("Error on sending POST: ");
          Serial.println(httpResponseCode);
        }

        http.end();
      }
    }
  } else {
    Serial.println("WiFi not connected!");
  }

  delay(1000); // Adjust this delay as needed
}
