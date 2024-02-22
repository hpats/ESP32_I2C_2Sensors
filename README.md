# Air Quality Monitoring System with ESP32, BME280, and CCS811

This project implements an Air Quality Monitoring System using an ESP32 microcontroller, a BME280 environmental sensor, and a CCS811 air quality sensor. The system provides real-time measurements of carbon dioxide (CO2), total volatile organic compounds (TVOC), temperature, humidity, and pressure.

## Setup

### Hardware Requirements

- ESP32 Development Board
- BME280 Sensor
- CCS811 Sensor
- Breadboard and jumper wires

### Wiring Diagram

![Wiring Diagram](schematic.png)

| ESP32 Pin  | BME280 Pin | CCS811 Pin |
|------------|------------|------------|
| 3V3        | VCC        | VCC        |
| GND        | GND        | GND        |
| D21        | SDA        |            |
| D22        | SCL        |            |
| D27        |            | SDA or SDI |
| D26        |            | SCL or SCK |
| D23        |            | nWAKE      |


### Libraries

- [WebServer Library](https://github.com/espressif/arduino-esp32/tree/master/libraries/WebServer)
- [Wire Library](https://www.arduino.cc/en/reference/wire)
- [Adafruit Sensor Library](https://github.com/adafruit/Adafruit_Sensor)
- [Adafruit BME280 Library](https://github.com/adafruit/Adafruit_BME280)
- [ccs811.h](https://github.com/maarten-pennings/CCS811)

Install the libraries through the Arduino Library Manager.

## Usage

1. Connect the sensors as per the provided wiring diagram.
2. Upload the code to your ESP32 board using the Arduino IDE.
3. Open the Serial Monitor to view sensor readings and IP address.
4. Access the web interface by navigating to the ESP32's IP address in a web browser.

## Web Interface

The web interface displays real-time air quality information, including CO2, TVOC, temperature, humidity, and pressure. It updates automatically every second.

## JSON Data

Navigate to `YOUR_IP/json-data` to retrieve data in JSON format.

```json
{
  "co2": 810.00,
  "tvoc": 62.00,
  "temperature": 19.53,
  "humidity": 46.69,
  "pressure": 1024.10
}
```

## Notes

- Adjust the sensor addresses and pins in the code if you use different sensor modules or pin configurations.
- Ensure your Wi-Fi credentials (SSID and password) are correctly set in the code.
