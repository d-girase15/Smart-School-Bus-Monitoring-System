# Smart School Bus Monitoring System 🚍📡

An IoT-based smart safety system for school buses using **Arduino, GPS, GSM, Alcohol Sensor, Vibration Sensor, LCD**, and **Student Boarding Detection**.

This project helps parents and school authorities monitor:
- Student boarding alerts 📲
- Real-time GPS tracking 🌍
- Driver alcohol detection 🚫🍺
- Accident/vibration detection 🚨
- Bus safety status on LCD display 📟

---

## 🧑‍💻 Features

### ✅ Student Boarding Alerts  
When any student gets on board, an SMS alert with GPS location is automatically sent to predefined phone numbers.

### ✅ GPS Tracking  
The system uses **NEO-6M GPS module** and sends:
- Latitude
- Longitude
- Google Maps location link

### ✅ Alcohol Detection  
If alcohol is detected:
- Buzzer ON
- Engine relay OFF
- SMS alert sent with live location

### ✅ Vibration/Accident Detection  
Detects impact using vibration sensors and triggers alert mode.

### ✅ LCD Display  
Shows system status:
- “Waiting for Student”
- “Student On Board”
- “Sending Location”
- “Monitoring Bus”

---

## 🛠️ Hardware Used

| Component | Purpose |
|----------|---------|
| Arduino UNO / Mega | Main controller |
| SIM800L GSM Module | SMS sending + calls |
| NEO-6M GPS Module | Live GPS tracking |
| Alcohol Sensor (MQ-3/MQ-135) | Driver alcohol detection |
| Vibration Sensor | Accident detection |
| 16x2 LCD Display | System status |
| Buzzer | Alert |
| Relay Module | Engine cutoff |
| Push Buttons | Student boarding inputs |

---

## 📁 File Structure

```
Smart-School-Bus-Monitoring-System/
 ├── smart_bus_system.ino
 └── README.md
```

---

## 🚀 How It Works

1. System waits for student input.
2. If a student boards:
   - Reads GPS location
   - Sends SMS:
     ```
     Student On Board Alert!!
     https://maps.google.com/maps?q=loc:LAT,LNG
     ```
3. Alcohol sensor continuously monitors driver.
4. Vibration sensor detects accidents.
5. LCD shows real-time status.

---

## 🔧 How to Upload Code

1. Open **Arduino IDE**  
2. Create new sketch → paste `smart_bus_system.ino`  
3. Select board & port  
4. Upload to Arduino  
5. Open Serial Monitor (9600 baud)

---

## 👨‍💼 Author
*Digvijay Girase**  

---

## ⭐ Support  
If you like this project, give it a **star ⭐ on GitHub**!

