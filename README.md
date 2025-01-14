# ArduinoMega-to-Esp-to-Blynk

This project connects an **ESP8266** and **Arduino Mega** to monitor and control sensors via **Blynk**. It uses a **DHT sensor** for humidity and temperature readings, a **Turbidity sensor** for water quality monitoring, a **pH sensor** for measuring pH levels, and a **DS18B20 temperature sensor** for temperature readings. The data is sent to the Blynk app for remote monitoring, and the system operates via WiFi.

## Components Used

- **ESP8266**: A WiFi-enabled microcontroller used for communication with Blynk.
- **Arduino Mega**: A microcontroller that interfaces with the sensors and controls the system.
- **DHT11/DHT22**: A sensor for measuring temperature and humidity.
- **Turbidity Sensor**: Measures the turbidity (cloudiness) of water.
- **pH Sensor**: Measures the pH level of the water.
- **DS18B20**: A digital temperature sensor for measuring water temperature.
- **Servo Motor**: Controls a mechanical system (like opening a valve).
- **LCD (16x2)**: Displays real-time data locally on the Mega.
- **WiFi**: ESP8266 connects to the internet and communicates with the Blynk app.

## Wiring Overview

### ESP8266 (WiFi Module)
- **RX Pin**: Pin 3 on Arduino Mega
- **TX Pin**: Pin 2 on Arduino Mega
- **VCC**: 3.3V
- **GND**: Ground

### Arduino Mega
- **DHT Sensor**
  - **VCC**: 5V
  - **GND**: Ground
  - **Data Pin**: Pin 8
- **Turbidity Sensor**
  - **VCC**: 5V
  - **GND**: Ground
  - **Analog Pin**: A1
- **pH Sensor**
  - **VCC**: 5V
  - **GND**: Ground
  - **Analog Pin**: A0
- **DS18B20 Temperature Sensor**
  - **VCC**: 5V
  - **GND**: Ground
  - **Data Pin**: Pin 7
- **Servo Motor**
  - **Control Pin**: Pin 9
- **LCD Display**
  - **SDA**: Pin 20
  - **SCL**: Pin 21

## Setup and Installation

### 1. Install Libraries

Make sure to install the following libraries for Arduino IDE:
- **Blynk**: For IoT integration with the Blynk app.
- **DallasTemperature**: For DS18B20 sensor support.
- **OneWire**: For interfacing with DS18B20.
- **DHT sensor library**: For the DHT11/DHT22 sensor.
- **LiquidCrystal_I2C**: For the I2C LCD display.
- **ArduinoJson**: For handling JSON data.

### 2. Setup Blynk
- Create a new project in the Blynk app.
- Select **ESP8266** as your device.
- Obtain your **Auth Token** and update it in the code where `BLYNK_AUTH_TOKEN` is defined.
- Connect your phone to the same Wi-Fi network as your ESP8266.

### 3. Upload the Code
- Upload `esp.ino` to your ESP8266.
- Upload `mega.ino` to your Arduino Mega.

### 4. Monitor Data
Once everything is connected, open the Blynk app and monitor the data. You can also see the readings on the Arduino Mega's LCD.

## Code Overview

### `esp.ino`
- Handles communication between the **ESP8266** and **Blynk**.
- Connects to WiFi and Blynk cloud.
- Fetches current time from NTP (Network Time Protocol) server.
- Receives sensor data from Arduino Mega and sends it to Blynk.

### `mega.ino`
- Reads sensor values from **DHT**, **Turbidity**, **pH**, and **DS18B20** sensors.
- Sends data to the ESP8266 in JSON format for processing.
- Controls the servo motor and displays data on the LCD.

## Features
- **Real-time monitoring**: View sensor data (pH, turbidity, temperature) on the Blynk app.
- **Remote control**: Control devices (e.g., servo motor) using the Blynk app.
- **Time synchronization**: Displays current time on the LCD and sends it to Blynk using NTP.

## Troubleshooting
- If you're having issues with Wi-Fi connectivity, make sure the ESP8266 is correctly connected to your Wi-Fi and that the correct SSID and password are entered in the code.
- If you don't see data on the Blynk app, check the Blynk Auth Token and ensure your device is correctly linked.

## License
This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.
