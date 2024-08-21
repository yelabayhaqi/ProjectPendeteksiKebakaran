# IoT-Based Fire Detection System

This project is an IoT-based fire detection system that monitors environmental conditions to detect fire hazards. The system uses multiple sensors, including a gas sensor (MQ2), a temperature sensor (MLX90614), and a UV flame sensor (UVTRON). It integrates with Thinger.io for IoT connectivity and uses a Nextion display for real-time data visualization.

## Table of Contents
- [Features](#features)
- [Hardware Components](#hardware-components)
- [How It Works](#how-it-works)
- [Installation](#installation)
- [Usage](#usage)
- [License](#license)

## Features
- **Real-time Monitoring:** Continuously monitors gas levels, temperature, and UV light to detect fire.
- **Alarms and Indicators:** Triggers an alarm and activates a water pump in case of fire detection.
- **IoT Integration:** Sends sensor data to Thinger.io for remote monitoring.
- **User Interface:** Displays sensor data and alerts on a Nextion HMI display.
- **Timekeeping:** Utilizes an RTC module to maintain accurate timestamps.

## Hardware Components
- **ESP32:** Microcontroller for processing sensor data and handling IoT communication.
- **MQ2 Gas Sensor:** Detects the presence of flammable gases.
- **Adafruit MLX90614:** Measures object temperature to detect potential fire sources.
- **UVTRON Flame Sensor:** Detects UV light emitted by flames.
- **RTC DS3231:** Real-Time Clock module for accurate timekeeping.
- **Nextion HMI Display:** Provides a user-friendly interface to display sensor readings and alerts.
- **Buzzer & Water Pump:** Activated when fire is detected to alert and mitigate the hazard.

## How It Works
1. **Sensor Readings:** The ESP32 reads data from the MQ2, MLX90614, and UVTRON sensors to monitor gas levels, temperature, and UV light, respectively.
2. **Data Processing:** The collected data is processed to determine the presence of fire. The system compares sensor values against predefined thresholds.
3. **Fire Detection:** If gas levels, temperature, or UV light exceed safe limits, the system identifies a potential fire.
4. **Alarm Activation:** The system triggers an alarm and activates the water pump to control the fire.
5. **Data Display:** Sensor readings and alerts are displayed on the Nextion HMI display.
6. **IoT Connectivity:** The system sends sensor data to Thinger.io, enabling remote monitoring.

## Installation
1. **Hardware Setup:**
   - Connect the MQ2, MLX90614, and UVTRON sensors to the ESP32.
   - Attach the RTC DS3231 module for timekeeping.
   - Connect the Nextion HMI display for user interaction.
   - Wire the alarm and water pump for fire response.

2. **Software Setup:**
   - Clone this repository.
   - Install the required libraries: `Adafruit_MLX90614`, `RTClib`, `ThingerESP32`, and `Nextion`.
   - Configure the Wi-Fi credentials and Thinger.io credentials in the code.
   - Upload the code to the ESP32 using the Arduino IDE.

## Usage
1. **Power On:** Power the system and wait for it to connect to Wi-Fi.
2. **Monitoring:** The system will start monitoring sensor data in real-time. Any fire hazard will trigger the alarm and water pump.
3. **Remote Access:** Access Thinger.io to monitor the system remotely and receive alerts.
