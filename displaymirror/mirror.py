import tkinter as tk
import serial
import time
import threading

# --- Arduino Serial Setup ---
arduino = serial.Serial(port='COM5', baudrate=9600, timeout=1)
time.sleep(2)  # wait for Arduino reset

# --- Tkinter GUI Setup ---
root = tk.Tk()
root.title("Arduino LCD Mirror")
root.configure(bg="white")

# Big font label
label = tk.Label(root, text="Waiting for message...", font=("Helvetica", 48, "bold"), fg="black", bg="white")
label.pack(expand=True, fill="both")

def show_message(msg):
    # Show the message
    label.config(text=msg)
    # After 2 seconds (2000 ms), clear it
    root.after(2000, lambda: label.config(text=""))

def read_serial():
    while True:
        if arduino.in_waiting > 0:
            msg = arduino.readline().decode().strip()
            if msg:
                # Update GUI with new message
                root.after(0, show_message, msg)

# Run serial reading in background thread
thread = threading.Thread(target=read_serial, daemon=True)
thread.start()

root.mainloop()
arduino.close()