import socket
import struct
import threading
import time
import numpy as np
import cv2

UDP_IP = "0.0.0.0"
UDP_PORT = 8888
MAX_FRAME_BYTES = 80 * 1024
FRAME_TIMEOUT = 0.8


class UDPListener:
    def __init__(self):
        self.latest_frame = None
        self.frame_lock = threading.Lock()
        self.frame_count = 0
        self.running = False
        self._frame_buffer = bytearray()
        self._expected_size = 0
        self._last_packet_time = 0
        self._drop_count = 0

    def start(self):
        self.running = True
        self._thread = threading.Thread(target=self._listen_loop, daemon=True)
        self._thread.start()

    def stop(self):
        self.running = False

    def get_frame(self):
        with self.frame_lock:
            if self.latest_frame is not None:
                return self.latest_frame.copy()
            return None

    def _listen_loop(self):
        sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        sock.setsockopt(socket.SOL_SOCKET, socket.SO_RCVBUF, 256 * 1024)
        sock.bind((UDP_IP, UDP_PORT))
        sock.settimeout(0.3)
        print(f"[UDP] 监听 {UDP_IP}:{UDP_PORT}")

        while self.running:
            try:
                data, addr = sock.recvfrom(65535)
            except socket.timeout:
                self._force_reset_if_stale()
                continue
            except Exception as e:
                print(f"[UDP] recv异常: {e}")
                continue

            if len(data) < 4:
                continue

            total_size = struct.unpack(">I", data[0:4])[0]
            chunk_data = data[4:]

            if total_size <= 0 or total_size > MAX_FRAME_BYTES:
                continue

            if total_size != self._expected_size or self._is_timeout():
                self._reset_buffer(total_size)

            if len(self._frame_buffer) + len(chunk_data) > MAX_FRAME_BYTES:
                self._reset_buffer(total_size)

            self._frame_buffer.extend(chunk_data)
            self._last_packet_time = time.time()

            if len(self._frame_buffer) >= self._expected_size:
                jpeg_bytes = bytes(self._frame_buffer[:self._expected_size])
                frame = cv2.imdecode(np.frombuffer(jpeg_bytes, np.uint8), cv2.IMREAD_COLOR)
                if frame is not None:
                    frame = cv2.flip(frame, -1)
                    with self.frame_lock:
                        self.latest_frame = frame
                    self.frame_count += 1
                self._frame_buffer = bytearray()
                self._expected_size = 0

        sock.close()
        print("[UDP] 监听已停止")

    def _reset_buffer(self, total_size):
        self._frame_buffer = bytearray()
        self._expected_size = total_size

    def _is_timeout(self):
        return (time.time() - self._last_packet_time) > FRAME_TIMEOUT

    def _force_reset_if_stale(self):
        if self._expected_size > 0 and self._is_timeout():
            self._frame_buffer = bytearray()
            self._expected_size = 0
            self._drop_count += 1
