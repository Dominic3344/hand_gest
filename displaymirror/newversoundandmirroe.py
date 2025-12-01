import tkinter as tk
import serial
import time
import threading
import pyttsx3

# --- Arduino Serial Setup ---
arduino = serial.Serial(port='COM6', baudrate=9600, timeout=1)
time.sleep(2)

# --- Tkinter GUI Setup ---
root = tk.Tk()
root.title("Arduino LCD Mirror")
root.configure(bg="white")

label = tk.Label(root, text="Waiting for message...",
                 font=("Helvetica", 48, "bold"), fg="black", bg="white")
label.pack(expand=True, fill="both")

# --- Proper pyttsx3 setup ---
engine = pyttsx3.init()

# --- Flashing background control ---
flashing = False
flash_state = False

def start_flashing():
    global flashing
    flashing = True
    flash_bg()

def stop_flashing():
    global flashing
    flashing = False
    root.configure(bg="white")

def flash_bg():
    global flash_state
    if flashing:
        flash_state = not flash_state
        root.configure(bg="red" if flash_state else "white")
        root.after(300, flash_bg)

# --- Show message logic ---
def show_message(msg):
    # Update label immediately
    label.config(text=msg)
    root.update_idletasks()

    # Flash if help requested
    if "help" in msg.lower():
        start_flashing()
    else:
        stop_flashing()

    # Speak only for Morse or decoded text
    if msg.startswith("Morse:") or msg.isalpha() or msg.strip().isdigit() or "Reset" in msg:
        engine.say(msg)
        engine.runAndWait()

    # Clear only normal messages (not Morse)
    if not msg.startswith("Morse:") and "Reset" not in msg:
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