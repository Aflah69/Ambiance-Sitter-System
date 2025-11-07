import tkinter as tk
import json
import paho.mqtt.client as mqtt

BROKER = "broker.hivemq.com"
PORT = 1883
TOPIC = "esp32/dht112"   

temp_value = 0
hum_value = 0

def on_connect(client, userdata, flags, reason_code, properties):
    print(" Connected to Broker:", reason_code)
    client.subscribe(TOPIC)

def on_message(client, userdata, msg):
    global temp_value, hum_value
    print("RAW:", msg.payload)

    try:
        data = json.loads(msg.payload.decode())
        temp_value = data.get("temperature", 0)
        hum_value = data.get("humidity", 0)
        update_ui()
    except:
        print("Invalid Message")

def update_ui():
    temp_label.config(text=f"Temperature: {temp_value} °C")
    hum_label.config(text=f"Humidity: {hum_value} %")

    # Background
    if temp_value < 20:
        root.config(bg="lightblue")
    elif temp_value <= 28:
        root.config(bg="lightgreen")
    else:
        root.config(bg="red")

    # Humidity icon
    if hum_value < 40:
        icon_label.config(text="☀️ (Dry)")
    elif hum_value <= 60:
        icon_label.config(text="😊 (Good)")
    else:
        icon_label.config(text="💧 (Humid)")


# GUI
root = tk.Tk()
root.title("Ambiance Dashboard")
root.geometry("400x250")

temp_label = tk.Label(root, text="Temperature: -- °C", font=("Arial", 18))
temp_label.pack(pady=10)

hum_label = tk.Label(root, text="Humidity: -- %", font=("Arial", 18))
hum_label.pack(pady=10)

icon_label = tk.Label(root, text="--", font=("Arial", 30))
icon_label.pack(pady=10)

# MQTT
client = mqtt.Client(mqtt.CallbackAPIVersion.VERSION2)
client.on_connect = on_connect
client.on_message = on_message

client.connect(BROKER, PORT, 60)
client.loop_start()

root.mainloop()
