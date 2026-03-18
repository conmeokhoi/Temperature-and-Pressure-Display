# Environmental Monitoring System (BMP180 & MAX7219)

![Platform](https://img.shields.io/badge/Platform-Raspberry%20Pi%203B%2B-red)
![Language](https://img.shields.io/badge/Language-C-blue)
![Interface](https://img.shields.io/badge/Interface-I2C%20%2F%20SPI-green)

## 📌 Project Overview
This project focuses on building a compact, low-cost, and highly efficient environmental monitoring system. The system utilizes a **BMP180** sensor to collect atmospheric temperature and pressure data, processes it via a **Raspberry Pi 3 Model B+**, and provides real-time visual output on a **MAX7219 7-segment LED module**. Additionally, the collected data is automatically logged into a `.csv` file for further processing and analysis.

## 🎥 Demo Video
Click the image below to watch the real-time operation of the system:

[![System Demo](https://img.youtube.com/vi/Hr0xIVP4JFo/0.jpg)](https://youtu.be/Hr0xIVP4JFo)


*(Click the image to view the video on YouTube)*

## 🚀 Key Features
* **Accurate Measurement:** Acquires real-time temperature ($^\circ C$) and atmospheric pressure (Psi).
* **Visual Display:** Visualizes data on an 8-digit 7-segment LED using the MAX7219 driver.
* **Data Logging:** Automatically formats and saves data into a CSV file with timestamps.
* **Automated Startup:** Integrates a crontab script to automatically launch the monitoring software upon boot.

## 🛠 Hardware Architecture

### 1. Components
* **Raspberry Pi 3B+**: The central embedded computer (Quad-Core 1.4GHz, 1GB RAM).
* **BMP180**: Digital barometric pressure and temperature sensor.
* **MAX7219 Module**: 8-digit 7-segment LED display driver (Common Cathode).

### 2. Wiring Diagram
The system utilizes both I2C and SPI serial communication protocols.

**BMP180 Sensor ($I^2C$):**
* VCC $\rightarrow$ Pin 1 (3V3) 
* GND $\rightarrow$ Pin 6 (GND) 
* SDA $\rightarrow$ Pin 3 (SDA) 
* SCL $\rightarrow$ Pin 5 (SCL) 

**MAX7219 Module ($SPI$):**
* VCC $\rightarrow$ Pin 4 (5V) 
* GND $\rightarrow$ Pin 20 (GND) 
* DIN $\rightarrow$ Pin 19 (MOSI) 
* CS $\rightarrow$ Pin 24 (CE0) 
* CLK $\rightarrow$ Pin 23 (SCLK) 

## 🧠 Software & Algorithms

### 1. BMP180 Data Processing
The system reads raw calibration data from the sensor's EPROM, then acquires raw temperature and pressure values. These values are processed using standard compensation formulas to calculate the true temperature and pressure.

### 2. Output Data Format
Data is structured and appended to a `.csv` file in the following format:
`YYYY-MM-DD HH:MM:SS, Temperature °C, Pressure Psi` 
> *Example:* `2025-06-04 21:17:00, 27.40 °C, 14.848 Psi` 

## 💻 Installation & Usage

### 1. Compilation
To compile the C source code, open your Raspberry Pi terminal and run the following command:
```bash
gcc TEMP_PRES.c -o TEMP_PRES -lwiringPi -lm
