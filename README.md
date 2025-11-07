# Ambiance-Sitter-System
ESP32-based Ambient Monitoring System that measures temperature and humidity using a DHT11 sensor and publishes the data to an MQTT broker. A Python Tkinter dashboard subscribes to the MQTT topic and visualizes readings in real time with color-coded feedback and humidity-based status icons.
This project implements an IoT-based Ambient Monitoring System using an LOLIN 23 ESP32-WROOM-32 microcontroller, a DHT11 temperature-humidity sensor, and a Python graphical dashboard. The ESP32 reads temperature and humidity values from the DHT11 sensor and publishes the data wirelessly to an MQTT broker. A Python GUI application subscribes to this MQTT data and displays the live sensor readings. The user interface dynamically updates its background color based on temperature ranges—blue for cold conditions below 20°C, green for comfortable temperatures between 20°C and 28°C, and red for hot conditions above 28°C. In addition, the dashboard shows a symbolic icon representing humidity levels: a sun symbol (☀️) when humidity is less than 40% indicating dryness, a smiley (😊) for comfortable humidity between 40% and 60%, and a droplet (💧) when humidity exceeds 60%, indicating high moisture levels. This provides an intuitive real-time visualization of environmental conditions.

How the DHT11 Sensor Works

The DHT11 is a basic and inexpensive digital sensor used to measure temperature and humidity. It integrates sensing components and a small processing chip inside a single package to provide calibrated output.

Internal Working
The DHT11 contains:

Humidity Sensor (Capacitive humidity element)
Humidity is measured by a small polymer capacitor whose electrical capacitance changes depending on the amount of water vapor in the air.
The sensor continuously monitors this change and converts it into a humidity value.

Temperature Sensor (NTC Thermistor)
A thermistor changes its resistance with temperature.
The internal chip converts this resistance change into a temperature reading.

Integrated Microcontroller
The internal MCU processes raw signals from both sensing elements.
It applies calibration data stored during manufacturing and prepares the final digital output.

Communication Process:

The DHT11 uses a one-wire digital communication protocol, requiring only one GPIO pin from the microcontroller.

Start Signal:

The ESP32 initially pulls the data pin LOW for about 18 ms to request data.

Sensor Response:

The DHT11 acknowledges by pulling the line LOW and then HIGH.

Data Transmission:

The sensor sends a 40-bit digital data packet:
8 bits  → Humidity integer
8 bits  → Humidity decimal
8 bits  → Temperature integer
8 bits  → Temperature decimal
8 bits  → Checksum

Bit Interpretation:

Each bit is transmitted as a signal with a specific pulse width:
Short HIGH pulse → 0
Longer HIGH pulse → 1
The microcontroller measures pulse timing to reconstruct data.

Checksum:

The final byte ensures data validity.
The microcontroller adds the first four bytes and compares with the checksum.
If they match, the data is valid.

Hardware Connections — DHT11 to ESP32

The DHT11 sensor communicates with the ESP32 using a single-wire digital interface, so only one GPIO pin is required for data transfer.
<img width="723" height="740" alt="Blank diagram (3)" src="https://github.com/user-attachments/assets/1c11273a-73db-49c7-b8ac-cf1e21c12c36" />

Connect VCC of the DHT11 to 3.3V output of ESP32.The DHT11 works on 3.3V–5V. We use 3.3V to match ESP32 logic levels.

Connect GND of DHT11 to GND of ESP32.Common ground ensures proper reference voltage.

Connect DATA pin of DHT11 to GPIO 4 of ESP32.This pin is used to read sensor values.

Steps to Flash
Install VS Code ESP-IDF Extension
Open this project folder in VS Code
Build firmware
→ ESP-IDF: Build (idf.py build)
Connect ESP32 LOLIN D32 via USB
Flash firmware
→ ESP-IDF: Flash (idf.py -p <port> flash)
Monitor serial output
→ ESP-IDF: Monitor (idf.py -p <port> monitor)

Serial Output:

<img width="500" height="92" alt="serial_ouput" src="https://github.com/user-attachments/assets/a1a734b0-5f65-4c74-861b-c5b2d8ddce84" />

Dashboard Instructions (Python):
Open Windows Command Prompt
Check version:
python --version
You should see something like:
Python 3.12.0
If you do not have Python installed → install from:
https://www.python.org/downloads/

Create a Project Folder
Create a new folder for your dashboard (for example, on Desktop):
cd Desktop
mkdir ui-dashboard
cd ui-dashboard

Create the Python File
Now create a new file named mqtt_dashboard.py (the file name can vary) using a text editor like Notepad or VS Code, and write code inside it.
notepad mqtt_dashboard.py
Install Required Packages
You need to install paho-mqtt for MQTT communication.
Run this command in the same folder:
pip install paho-mqtt
Run the Dashboard
Now run the Python application:
python mqtt_dashboard.py

GUI Window:
![11zon_1000073531](https://github.com/user-attachments/assets/f1e40719-758e-41f4-8220-bf7c51568e49)
![11zon_1000073527](https://github.com/user-attachments/assets/67bb38af-5de1-4ec0-ad06-da68c49f61de)

Design Choices
ESP32-WROOM-32
Built-in Wi-Fi → easy MQTT communication
Good processing capability

DHT11
Low cost & easy integration
Measures Temp + Humidity

MQTT Protocol
Lightweight
Designed for IoT
Reliable communication

Python Dashboard
Fast development
Easy to customize
Real-time monitoring

Intuitive UI
Background & icon give instant feedback





