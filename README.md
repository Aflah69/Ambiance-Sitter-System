# Ambiance-Sitter-System
ESP32-based Ambient Monitoring System that measures temperature and humidity using a DHT11 sensor and publishes the data to an MQTT broker. A Python Tkinter dashboard subscribes to the MQTT topic and visualizes readings in real time with color-coded feedback and humidity-based status icons.

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

Communication Process
The DHT11 uses a one-wire digital communication protocol, requiring only one GPIO pin from the microcontroller.
Start Signal
The ESP32 initially pulls the data pin LOW for about 18 ms to request data.
Sensor Response
The DHT11 acknowledges by pulling the line LOW and then HIGH.
Data Transmission
The sensor sends a 40-bit digital data packet:
8 bits  → Humidity integer
8 bits  → Humidity decimal
8 bits  → Temperature integer
8 bits  → Temperature decimal
8 bits  → Checksum

Bit Interpretation
Each bit is transmitted as a signal with a specific pulse width:
Short HIGH pulse → 0
Longer HIGH pulse → 1
The microcontroller measures pulse timing to reconstruct data.

Checksum
The final byte ensures data validity.
The microcontroller adds the first four bytes and compares with the checksum.
If they match, the data is valid.
