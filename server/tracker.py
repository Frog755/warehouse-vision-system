import cv2
import threading

EXIT_MARGIN_RATIO = 0.15


class CSRTTracker:
    def __init__(self):
        self.tracker = None
        self.active = False
        self.bbox = None
        self.lock = threading.Lock()
        self.exit_margin = EXIT_MARGIN_RATIO
        self._frame_h = 0
        self._frame_w = 0

    def start(self, frame, roi):
        x, y, w, h = roi
        fh, fw = frame.shape[:2]
        if w <= 0 or h <= 0:
            return False
        if x < 0 or y < 0 or x + w > fw or y + h > fh:
            return False

        new_tracker = cv2.TrackerMIL_create()
        new_tracker.init(frame, (x, y, w, h))

        with self.lock:
            self.tracker = new_tracker
            self.active = True
            self.bbox = (x, y, w, h)
            self._frame_h, self._frame_w = fh, fw
        return True

    def stop(self):
        with self.lock:
            self.active = False
            self.tracker = None
            self.bbox = None

    def update(self, frame):
        with self.lock:
            if not self.active or self.tracker is None:
                return False
            t = self.tracker

        try:
            ok, bbox = t.update(frame)
        except Exception as e:
            print(f"[Tracker] update exception: {e}")
            self.stop()
            return False

        if not ok:
            self.stop()
            return False

        x, y, w, h = bbox
        fh, fw = frame.shape[:2]

        center_x = x + w / 2
        center_y = y + h / 2
        margin_x = fw * self.exit_margin
        margin_y = fh * self.exit_margin

        if (center_x < margin_x or center_x > fw - margin_x or
                center_y < margin_y or center_y > fh - margin_y):
            self.stop()
            return False

        with self.lock:
            self.bbox = (int(x), int(y), int(w), int(h))
        return True

    def set_exit_margin(self, ratio):
        self.exit_margin = max(0.0, min(0.5, ratio))
