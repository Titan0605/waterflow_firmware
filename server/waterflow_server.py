from flask import Flask, request, jsonify
import secrets
import time

app = Flask(__name__)

usuarios = [
    {
        "user_id": "user_1",
        "email": "ejemplo1@correo.com",
        "token": None,
        "token_timestamp": None,
        "waterflows": ["id_1", "id_2"]
    },
    {
        "user_id": "user_2",
        "email": "ejemplo2@correo.com",
        "token": None,
        "token_timestamp": None,
        "waterflows": ["id_3"]
    }
]

waterflow_database = [
    {"_id": "id_1", "waterflow_mac": "mac_1", "activate": True},
    {"_id": "id_2", "waterflow_mac": "mac_2", "activate": False},
    {"_id": "id_3", "waterflow_mac": "mac_3", "activate": True},
]

def generar_token(n_bytes=16):
    return secrets.token_hex(n_bytes)

@app.route("/generate-token", methods=["POST"])
def generar_token_para_usuario():
    data = request.get_json()
    if not data or "user_id" not in data:
        return jsonify({"status": "error", "message": "Missing 'user_id' field"}), 400

    user_id = data["user_id"].strip()
    usuario = next((u for u in usuarios if u["user_id"] == user_id), None)
    if not usuario:
        return jsonify({"status": "error", "message": "User not found"}), 404

    nuevo_token = generar_token(8)
    usuario["token"] = nuevo_token
    usuario["token_timestamp"] = time.time()

    return jsonify({
        "status": "success",
        "message": "Token generated successfully",
        "user_id": user_id,
        "token": nuevo_token
    }), 201

@app.route("/send-command", methods=["POST"])
def switch_waterflow():
    data = request.get_json()
    if not data or "waterflow_mac" not in data or "activate" not in data:
        return jsonify({"status": "error", "message": "Missing 'waterflow_mac' or 'activate'"}), 400

    mac = data["waterflow_mac"].strip()
    activate = data["activate"]
    if not isinstance(activate, bool):
        return jsonify({"status": "error", "message": "'activate' must be boolean"}), 400

    wf = next((w for w in waterflow_database if w["waterflow_mac"] == mac), None)
    if not wf:
        return jsonify({"status": "error", "message": "WaterFlow not found"}), 404

    wf["activate"] = activate
    return jsonify({
        "status": "success",
        "message": f"WaterFlow state changed to {activate}"
    }), 200

@app.route("/waterflow-state", methods=["GET"])
def get_waterflow_state():
    mac = request.args.get("waterflow_mac", "").strip()
    if not mac:
        return jsonify({"status": "error", "message": "Missing 'waterflow_mac' parameter"}), 400

    wf = next((w for w in waterflow_database if w["waterflow_mac"] == mac), None)
    if not wf:
        return jsonify({"status": "error", "message": "WaterFlow not found"}), 404

    return jsonify({
        "status": "success",
        "message": "State retrieved successfully",
        "waterflow_mac": mac,
        "activate": wf["activate"]
    }), 200

@app.route("/send-token", methods=["POST"])
def send_token():
    data = request.get_json()
    if not data or "token" not in data or "waterflow_mac" not in data:
        return jsonify({"status": "error", "message": "Missing 'token' or 'waterflow_mac'"}), 400

    token = data["token"].strip()
    mac = data["waterflow_mac"].strip()

    usuario = next((u for u in usuarios if u.get("token") == token), None)
    if not usuario:
        return jsonify({"status": "error", "message": "Invalid token"}), 401

    new_id = generar_token(8)
    waterflow_database.append({
        "_id": new_id,
        "waterflow_mac": mac,
        "activate": False
    })
    usuario["waterflows"].append(new_id)

    return jsonify({
        "status": "success",
        "message": "WaterFlow paired successfully",
        "waterflow_id": new_id
    }), 201

@app.route('/in-database')
def check_waterflow_database():
    mac = request.args.get("waterflow_mac", "").strip()
    if not mac:
        return jsonify({"status": "error", "message": "Missing 'waterflow_mac' parameter"}), 400

    wf = next((w for w in waterflow_database if w["waterflow_mac"] == mac), None)
    if not wf:
        return jsonify({"status": "error", "is_in_database": False , "message": "WaterFlow not found"}), 404

    return jsonify({
        "status": "success",
        "is_in_database": True,
        "message": "WaterFlow found"
    }), 200

if __name__ == "__main__":
    app.run(debug=True)