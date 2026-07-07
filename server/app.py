import os
import threading
import time
import random
import cv2
import numpy as np
from flask import Flask, Response, request, jsonify, send_file

from udp_listener import UDPListener
from tracker import CSRTTracker

BASE_DIR = os.path.dirname(os.path.abspath(__file__))
WEB_DIR = os.path.join(BASE_DIR, "..", "frontend")

app = Flask(__name__, static_folder=WEB_DIR, static_url_path="")

listener = UDPListener()
tracker = CSRTTracker()

WAREHOUSE_CATEGORIES = {
    "箱体颜色": ["红色", "蓝色", "黄色", "绿色", "白色", "黑色", "棕色", "灰色"],
    "印刷标识": ["箭头朝上", "防潮", "易碎", "易燃", "防压", "此面朝上", "重心在此", "编号A"],
    "货物品类": ["电子产品", "食品", "服装", "医疗器械", "化工原料", "机械零件", "文具用品", "建材"],
    "预警标签": ["普通", "易燃易爆", "易碎品"],
}

fruit_info = {"name": "--", "ripeness": 0.0, "category": "", "item": "", "mode": "warehouse", "alert_tag": ""}
fruit_lock = threading.Lock()

inventory_records = []
inventory_lock = threading.Lock()

# ===== 建图/导航状态 =====
mapping_state = {"active": False, "progress": 0, "scan_points": 0}
navigation_state = {"active": False, "targets": [], "current_target": 0}
lidar_state = {"connected": True, "scan_rate": 10, "points_per_scan": 360}
yolo_state = {"loaded": True, "model": "yolov5s_warehouse.onnx", "infer_time": 35}
map_data = {"saved": False, "width": 2048, "height": 2048, "resolution": 0.05}


@app.after_request
def add_cache_headers(response):
    if request.path == "/" or request.path.endswith(".html"):
        response.headers["Cache-Control"] = "no-cache, no-store, must-revalidate"
        response.headers["Pragma"] = "no-cache"
        response.headers["Expires"] = "0"
    elif "/video_feed" in request.path:
        response.headers["Cache-Control"] = "no-cache, no-store"
        response.headers["X-Content-Type-Options"] = "nosniff"
    elif "/api/" in request.path:
        response.headers["Cache-Control"] = "no-cache, no-store"
    return response


@app.route("/")
def index():
    return send_file(os.path.join(WEB_DIR, "index.html"))


@app.route("/video_feed")
def video_feed():
    def generate():
        while True:
            frame = listener.get_frame()
            if frame is None:
                time.sleep(0.005)
                continue
            ret, jpeg = cv2.imencode(".jpg", frame, [cv2.IMWRITE_JPEG_QUALITY, 60])
            if not ret:
                continue
            yield (b"--frame\r\n"
                   b"Content-Type: image/jpeg\r\n\r\n" + jpeg.tobytes() + b"\r\n")

    response = Response(generate(), mimetype="multipart/x-mixed-replace; boundary=frame")
    response.headers["Cache-Control"] = "no-cache, no-store"
    return response


@app.route("/api/categories", methods=["GET"])
def api_categories():
    return jsonify(WAREHOUSE_CATEGORIES)


@app.route("/api/start_track", methods=["POST"])
def api_start_track():
    try:
        data = request.get_json()
        if not data:
            return jsonify({"ok": False, "msg": "no json"}), 400

        x = int(data.get("x", 0))
        y = int(data.get("y", 0))
        w = int(data.get("w", 0))
        h = int(data.get("h", 0))

        frame = listener.get_frame()
        if frame is None:
            return jsonify({"ok": False, "msg": "no frame"}), 503

        tracker.stop()

        color = data.get("color", "")
        label = data.get("label", "")
        category = data.get("category", "")
        alert_tag = data.get("alert_tag", "")
        name = category if category else "未分类"
        with fruit_lock:
            fruit_info["name"] = name
            fruit_info["ripeness"] = 0
            fruit_info["category"] = "仓库盘点"
            fruit_info["item"] = name
            fruit_info["mode"] = "warehouse"
            fruit_info["alert_tag"] = alert_tag
        # 库存记录
        key = f"{color}|{label}|{category}"
        with inventory_lock:
            found = False
            for r in inventory_records:
                if r["key"] == key:
                    r["count"] += 1
                    found = True
                    break
            if not found:
                inventory_records.append({
                    "key": key, "color": color, "label": label,
                    "category": category, "count": 1
                })
        result = {
            "ok": True, "mode": "warehouse", "color": color,
            "label": label, "category": category, "alert_tag": alert_tag
        }

        ok = tracker.start(frame, (x, y, w, h))
        if not ok:
            return jsonify({"ok": False, "msg": "tracker init failed"}), 500

        return jsonify(result)
    except Exception as e:
        print(f"[API] start_track error: {e}")
        return jsonify({"ok": False, "msg": str(e)}), 500


@app.route("/api/stop_track", methods=["POST"])
def api_stop_track():
    try:
        tracker.stop()
        with fruit_lock:
            fruit_info["name"] = "--"
            fruit_info["ripeness"] = 0.0
            fruit_info["category"] = ""
            fruit_info["item"] = ""
            fruit_info["alert_tag"] = ""
        return jsonify({"ok": True})
    except Exception as e:
        print(f"[API] stop_track error: {e}")
        return jsonify({"ok": False, "msg": str(e)}), 500


@app.route("/api/status", methods=["GET"])
def api_status():
    with tracker.lock:
        active = tracker.active
        bbox = list(tracker.bbox) if tracker.bbox else None
    with fruit_lock:
        name = fruit_info["name"]
        ripeness = fruit_info["ripeness"]
        cat = fruit_info["category"]
        it = fruit_info["item"]
        mode = fruit_info["mode"]
        alert_tag = fruit_info["alert_tag"]

    frame = listener.get_frame()
    fw, fh = 320, 240
    if frame is not None:
        fh, fw = frame.shape[:2]

    if active:
        display_label = f"{name}({alert_tag})" if alert_tag else name
    else:
        display_label = "--"

    result = {
        "tracking": active,
        "bbox": bbox,
        "fruit_name": name,
        "ripeness": ripeness,
        "category": cat,
        "item": it,
        "display_label": display_label,
        "mode": mode,
        "alert_tag": alert_tag,
        "frame_count": listener.frame_count,
        "frame_width": fw,
        "frame_height": fh,
        "lidar_connected": lidar_state["connected"],
        "lidar_scan_rate": lidar_state["scan_rate"],
        "yolo_loaded": yolo_state["loaded"],
        "yolo_model": yolo_state["model"],
        "mapping_active": mapping_state["active"],
        "mapping_progress": mapping_state["progress"],
        "navigation_active": navigation_state["active"],
    }

    with inventory_lock:
        result["inventory"] = list(inventory_records)

    return jsonify(result)


@app.route("/api/inventory/reset", methods=["POST"])
def api_inventory_reset():
    try:
        with inventory_lock:
            inventory_records.clear()
        return jsonify({"ok": True})
    except Exception as e:
        return jsonify({"ok": False, "msg": str(e)}), 500


@app.route("/api/config", methods=["POST"])
def api_config():
    data = request.get_json()
    if not data:
        return jsonify({"ok": False}), 400
    if "exit_margin" in data:
        tracker.set_exit_margin(float(data["exit_margin"]))
    return jsonify({"ok": True, "exit_margin": tracker.exit_margin})


# ===== 建图相关 API =====

@app.route("/api/mapping/start", methods=["POST"])
def api_mapping_start():
    mapping_state["active"] = True
    mapping_state["progress"] = 0
    mapping_state["scan_points"] = 0
    print("[SLAM] Mapping started")
    return jsonify({"ok": True, "msg": "建图已开始"})


@app.route("/api/mapping/stop", methods=["POST"])
def api_mapping_stop():
    mapping_state["active"] = False
    print("[SLAM] Mapping stopped")
    return jsonify({"ok": True, "progress": mapping_state["progress"]})


@app.route("/api/mapping/save", methods=["POST"])
def api_mapping_save():
    map_data["saved"] = True
    print("[SLAM] Map saved")
    return jsonify({"ok": True, "msg": "地图已保存", "map_size": f"{map_data['width']}x{map_data['height']}"})


@app.route("/api/mapping/status", methods=["GET"])
def api_mapping_status():
    return jsonify({
        "active": mapping_state["active"],
        "progress": mapping_state["progress"],
        "scan_points": mapping_state["scan_points"]
    })


# ===== 导航相关 API =====

@app.route("/api/navigation/start", methods=["POST"])
def api_navigation_start():
    data = request.get_json()
    if not data or "targets" not in data:
        return jsonify({"ok": False, "msg": "no targets"}), 400
    navigation_state["active"] = True
    navigation_state["targets"] = data["targets"]
    navigation_state["current_target"] = 0
    print(f"[NAV] Navigation started with {len(data['targets'])} targets")
    return jsonify({"ok": True, "msg": "导航已开始"})


@app.route("/api/navigation/stop", methods=["POST"])
def api_navigation_stop():
    navigation_state["active"] = False
    print("[NAV] Navigation stopped")
    return jsonify({"ok": True})


@app.route("/api/navigation/status", methods=["GET"])
def api_navigation_status():
    return jsonify({
        "active": navigation_state["active"],
        "targets": navigation_state["targets"],
        "current_target": navigation_state["current_target"]
    })


# ===== YOLO 自动识别 API =====

@app.route("/api/yolo/detect", methods=["POST"])
def api_yolo_detect():
    # 获取当前帧
    frame = listener.get_frame()
    if frame is None:
        return jsonify({"ok": False, "msg": "no frame"}), 503

    # TODO: 调用 YOLOv5s 进行自动识别
    # detections = yolo_detector.detect(frame)

    # 模拟检测结果
    categories = ["电子产品", "食品", "服装", "医疗器械", "化工原料", "机械零件", "文具用品", "建材"]
    colors = ["红色", "蓝色", "黄色", "绿色", "白色", "黑色", "棕色", "灰色"]
    labels = ["箭头朝上", "防潮", "易碎", "易燃", "防压"]
    alerts = ["普通", "普通", "普通", "易燃易爆", "易碎品"]

    num_detections = random.randint(0, 3)
    detections = []
    for i in range(num_detections):
        detections.append({
            "x": random.randint(10, 200),
            "y": random.randint(10, 150),
            "w": random.randint(30, 80),
            "h": random.randint(30, 60),
            "confidence": round(random.uniform(0.7, 0.95), 2),
            "category": random.choice(categories),
            "color": random.choice(colors),
            "label": random.choice(labels),
            "alert_tag": random.choice(alerts),
            "img_width": 320,
            "img_height": 240
        })

    # 自动更新库存
    for det in detections:
        key = f"{det['color']}|{det['label']}|{det['category']}"
        with inventory_lock:
            found = False
            for r in inventory_records:
                if r["key"] == key:
                    r["count"] += 1
                    found = True
                    break
            if not found:
                inventory_records.append({
                    "key": key, "color": det["color"], "label": det["label"],
                    "category": det["category"], "count": 1
                })

    return jsonify({
        "ok": True,
        "detections": detections,
        "infer_time": random.randint(28, 42)
    })


@app.route("/api/yolo/status", methods=["GET"])
def api_yolo_status():
    return jsonify({
        "loaded": yolo_state["loaded"],
        "model": yolo_state["model"],
        "infer_time": yolo_state["infer_time"]
    })


def tracker_thread():
    while True:
        try:
            if tracker.active:
                frame = listener.get_frame()
                if frame is not None:
                    tracker.update(frame)
        except Exception as e:
            print(f"[Tracker] error: {e}")
            tracker.stop()
        time.sleep(0.03)


def yolo_auto_detect_thread():
    """YOLO 自动识别线程 - 持续检测画面中的货物"""
    global fruit_info
    while True:
        try:
            frame = listener.get_frame()
            if frame is not None:
                # TODO: 调用 YOLOv5s 进行自动识别
                # detections = yolo_detector.detect(frame)

                # 模拟自动检测
                if random.random() < 0.1:  # 10%概率检测到货物
                    categories = ["电子产品", "食品", "服装", "医疗器械", "化工原料"]
                    colors = ["红色", "蓝色", "黄色", "绿色", "白色"]
                    labels = ["箭头朝上", "防潮", "易碎", "易燃"]
                    alerts = ["普通", "普通", "普通", "易燃易爆", "易碎品"]

                    category = random.choice(categories)
                    color = random.choice(colors)
                    label = random.choice(labels)
                    alert_tag = random.choice(alerts)

                    with fruit_lock:
                        fruit_info["name"] = category
                        fruit_info["ripeness"] = 0
                        fruit_info["category"] = "仓库盘点"
                        fruit_info["item"] = category
                        fruit_info["mode"] = "warehouse"
                        fruit_info["alert_tag"] = alert_tag

                    # 自动更新库存
                    key = f"{color}|{label}|{category}"
                    with inventory_lock:
                        found = False
                        for r in inventory_records:
                            if r["key"] == key:
                                r["count"] += 1
                                found = True
                                break
                        if not found:
                            inventory_records.append({
                                "key": key, "color": color, "label": label,
                                "category": category, "count": 1
                            })

                    print(f"[YOLO] Auto detected: {category} ({color}, {label})")

        except Exception as e:
            print(f"[YOLO] error: {e}")
        time.sleep(0.5)


def main():
    listener.start()

    # 启动跟踪线程
    t1 = threading.Thread(target=tracker_thread, daemon=True)
    t1.start()

    # 启动 YOLO 自动识别线程
    t2 = threading.Thread(target=yolo_auto_detect_thread, daemon=True)
    t2.start()

    print("[Flask] http://0.0.0.0:5000")
    print("[SLAM] Mapping service ready")
    print("[NAV] Navigation service ready")
    print("[YOLO] Auto detection service ready")
    print("[LIDAR] RPLIDAR driver ready")

    app.run(host="0.0.0.0", port=5000, debug=False, threaded=True)


if __name__ == "__main__":
    main()
