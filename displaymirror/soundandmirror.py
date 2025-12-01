import tkinter as tk
import serial
import time
import threading
import pyttsx3

# --- Arduino Serial Setup ---
arduino = serial.Serial(port='COM7', baudrate=9600, timeout=1)
time.sleep(2)

# --- Tkinter GUI Setup ---
root = tk.Tk()
root.title("Arduino LCD Mirror")
root.configure(bg="white")

label = tk.Label(root, text="Waiting for message...",
                 font=("Helvetica", 48, "bold"), fg="black", bg="white")
label.pack(expand=True, fill="both")

def show_message(msg):
    # Update label immediately
    label.config(text=msg)
    root.update_idletasks()   # force GUI refresh now

    # Speak message after label is updated
    pyttsx3.speak(msg)

    # Clear after 2 seconds
    root.after(2000, lambda: label.config(text=""))

def read_serial():
    while True:
        if arduino.in_waiting > 0:
            msg = arduino.readline().decode(errors="ignore").strip()
            if msg:
                print("Received:", msg)
                root.after(0, show_message, msg)

# Start serial reader in background
threading.Thread(target=read_serial, daemon=True).start()

# Run Tkinter GUI
root.mainloop()

arduino.close()


# add flashing bg red and white when help requested 
#other all are working 