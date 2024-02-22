#include <WebServer.h>
#include <Wire.h>
#include <ESPmDNS.h>
#include "ccs811.h"
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>

#define ONBOARD_LED  2
#define SDA_1 27
#define SCL_1 26

#define SDA_2 21
#define SCL_2 22

CCS811 ccs811(23); // nWAKE on D3
Adafruit_BME280 bme; // I2C
float temperature, humidity, pressure, altitude;

float val1, val2;

const char *ssid = "SSID";
const char *password = "PASS";

WebServer server(80);

// Function declarations
void ReadSensors();
void ReadSensorsDataOnly();
void handle_OnConnect();
void handle_JsonData();
void handle_NotFound();
String SendHTML(float val1, float val2);

void setup(void) {
  pinMode(ONBOARD_LED,OUTPUT);
  digitalWrite(ONBOARD_LED,HIGH);
  Serial.begin(115200);
  Wire1.begin(SDA_2, SCL_2);

  // Initialize BME280 sensor
  bool status1 = bme.begin(0x77, &Wire1);
  if (!status1) {
    Serial.println("Could not find a valid BME280_1 sensor, check wiring!");
    while (1);
  }
  Serial.println(status1);

  Wire.begin(SDA_1, SCL_1);

  // Initialize CCS811 sensor
  ccs811.set_i2cdelay(50);
  bool ok = ccs811.begin();
  if (!ok)
    Serial.println("setup: CCS811 begin FAILED");

  Serial.print("setup: hardware version: ");
  Serial.println(ccs811.hardware_version(), HEX);
  Serial.print("setup: bootloader version: ");
  Serial.println(ccs811.bootloader_version(), HEX);
  Serial.print("setup: application version: ");
  Serial.println(ccs811.application_version(), HEX);

  ok = ccs811.start(CCS811_MODE_1SEC);
  if (!ok)
    Serial.println("setup: CCS811 start FAILED");

  Serial.println("Connecting to ");
  Serial.println(ssid);

  // Connect to WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  // Initialize MDNS
  if (MDNS.begin("esp32")) {
    Serial.println("MDNS responder started");
  }

  // Define server routes
  server.on("/", HTTP_GET, handle_OnConnect);
  server.on("/json-data", HTTP_GET, handle_JsonData);
  server.onNotFound(handle_NotFound);

  server.begin();
  Serial.println("HTTP server started");
  ReadSensors();
  digitalWrite(ONBOARD_LED,LOW);
}

void loop() {
  server.handleClient();
}

void handle_OnConnect() {
  server.send(200, "text/html", SendHTML(val1, val2));
}

void handle_JsonData() {
  ReadSensorsDataOnly();
  String json = "{\"co2\":" + String(val1) + ", \"tvoc\":" + String(val2) +
                ", \"temperature\":" + String(temperature) +
                ", \"humidity\":" + String(humidity) +
                ", \"pressure\":" + String(pressure) + "}";
  server.send(200, "application/json", json);
}

void handle_NotFound() {
  server.send(404, "text/plain", "Not found");
}

String SendHTML(float val1, float val2) {
  String ptr = "<!DOCTYPE html> <html>\n";
  ptr += "<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0, user-scalable=no\">\n";
  ptr += "<title>Measured Air Quality</title>\n";
  ptr += "<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}\n";
  ptr += "body{margin-top: 50px;} h1 {color: #444444;margin: 50px auto 30px;}\n";
  ptr += "p {font-size: 24px;color: #444444;margin-bottom: 10px;}\n";
  ptr += "</style>\n";
  ptr += "<script>\n";
  ptr += "setInterval(loadDoc,1000);\n";
  ptr += "function loadDoc() {\n";
  ptr += "var xhttp = new XMLHttpRequest();\n";
  ptr += "xhttp.onreadystatechange = function() {\n";
  ptr += "if (this.readyState == 4 && this.status == 200) {\n";
  ptr += "document.body.innerHTML =this.responseText}\n";
  ptr += "};\n";
  ptr += "xhttp.open(\"GET\", \"/\", true);\n";
  ptr += "xhttp.send();\n";
  ptr += "}\n";
  ptr += "</script>\n";
  ptr += "</head>\n";
  ptr += "<body>\n";
  ptr += "<div id=\"webpage\">\n";
  ptr += "<h1>Measured Air Quality</h1>\n";

  ptr += "<p>CO2: ";
  ptr += val1;
  ptr += " ppm</p>";

  ptr += "<p>TVOC: ";
  ptr += val2;
  ptr += " ppb</p>";

  ptr += "<p>Temperature: ";
  ptr += temperature;
  ptr += " °C</p>";

  ptr += "<p>Humidity: ";
  ptr += humidity;
  ptr += " %</p>";

  ptr += "<p>Pressure: ";
  ptr += pressure;
  ptr += " Mb</p>";

  ptr += "</div>\n";
  ptr += "</body>\n";
  ptr += "</html>\n";
  return ptr;
}

void ReadSensorsDataOnly() {
  uint16_t eco2, etvoc, errstat, raw;
  ccs811.read(&eco2, &etvoc, &errstat, &raw);

  if (errstat == CCS811_ERRSTAT_OK) {
    val1 = eco2;
    val2 = etvoc;
  }
  temperature = bme.readTemperature();
  humidity = bme.readHumidity();
  pressure = bme.readPressure() / 100;
}

void ReadSensors() {
  temperature = bme.readTemperature();
  humidity = bme.readHumidity();
  pressure = bme.readPressure() / 100;

  // Print values to the serial monitor:
  Serial.print(F("Pressure: "));
  Serial.print(pressure);
  Serial.print(" Mb");
  Serial.print("\t");
  Serial.print(("Temp: "));
  Serial.print(temperature);
  Serial.print(" °C");
  Serial.print("\t");
  Serial.print("Humidity: ");
  Serial.print(humidity);
  Serial.println(" %");
}
