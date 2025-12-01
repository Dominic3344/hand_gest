import serial, time, pyttsx3

arduino = serial.Serial(port='COM5', baudrate=9600, timeout=1)
time.sleep(2)

print("Listening to Arduino...")

while True:
    if arduino.in_waiting > 0:
        msg = arduino.readline().decode(errors="ignore").strip()
        if msg:
            print("Received:", msg)
            pyttsx3.speak(msg)  # speaks every time, even duplicates