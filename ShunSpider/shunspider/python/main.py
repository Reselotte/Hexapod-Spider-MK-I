from arduino.app_utils import *
from arduino.app_bricks.web_ui import WebUI
from arduino.app_utils import Bridge

ui = WebUI()
bridge = Bridge()

ui.on_connect(lambda sid: print(f"{sid} has connected"))
ui.on_disconnect(lambda sid: print(f"{sid} has disconnected"))
ui.on_message("hello", lambda sid, data: print(f"Recieved message f rom {sid}: {data}"))


#mapping คำสั่ง
motion_control = {
    "standBtn": 1,
    "sitBtn": 2,
    "walkBtn": 3,
    "walkbackBtn": 4,
    "spinleftBtn": 5,
    "spinrightBtn": 6,
    "stand90Btn": 7
}

def on_cmd(sid, data):
    print("Received:", data)
    cmd = motion_control.get(data)
    
    if cmd:
        bridge.call("motion_control", cmd)

ui.on_message("cmd", on_cmd)

        
def main():
    pass

App.run(main)
