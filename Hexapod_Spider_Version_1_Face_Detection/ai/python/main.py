from arduino.app_utils import App, Bridge
from arduino.app_bricks.video_objectdetection import VideoObjectDetection
from arduino.app_bricks.web_ui import WebUI

# ---------- INIT ----------
web_ui = WebUI()
video_detector = VideoObjectDetection(confidence=0.4, debounce_sec=1.5)
# ---------- CALLBACK ----------
def on_all_detections(detections: dict):
    
    print("All detections:", detections)

    for label, data in detections.items():
        if "bounding_box_xyxy" in data:
            x1, y1, x2, y2 = data["bounding_box_xyxy"]

            if x1 > 72:
                zone = 1
                print(zone)
                Bridge.call("onZone", zone);

            elif x1 < 32:
                zone = 2
                print(zone)
                Bridge.call("onZone", zone);

            else:
                zone = 3
                print(zone)
                Bridge.call("onZone", zone);

    

video_detector.on_detect_all(on_all_detections)
# ---------- START ----------
web_ui.start()
App.run()
()
