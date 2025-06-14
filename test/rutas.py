import requests
import json

def menu():
    print("\nSeleccione una opción:")
    print("1. Generar token para usuario")
    print("2. Cambiar estado de WaterFlow")
    print("3. Obtener estado de WaterFlow")
    print("4. Emparejar WaterFlow con token")
    print("5. Salir")

BASE_URL = "https://vw6jc4s1-5000.usw3.devtunnels.ms"

while True:
    menu()
    try:
        choice = int(input("Opción: "))
    except ValueError:
        print("Por favor ingrese un número válido.")
        continue

    if choice == 1:
        user_id = input("Ingrese user_id: ").strip()
        payload = {"user_id": user_id}
        resp = requests.post(f"{BASE_URL}/generate-token", json=payload)
        print(f"Status: {resp.status_code}\nRespuesta: {resp.json()}\n")

    elif choice == 2:
        mac = input("Ingrese waterflow_mac: ").strip()
        activate_str = input("Activar? (True/False): ").strip().lower()
        if activate_str in ('true', '1', 't'):
            activate = True
        elif activate_str in ('false', '0', 'f'):
            activate = False
        else:
            print("Valor de 'activate' inválido. Use True o False.")
            continue
        payload = {"waterflow_mac": mac, "activate": activate}
        resp = requests.post(f"{BASE_URL}/send-command", json=payload)
        print(f"Status: {resp.status_code}\nRespuesta: {resp.json()}\n")

    elif choice == 3:
        mac = input("Ingrese waterflow_mac: ").strip()
        params = {"waterflow_mac": mac}
        resp = requests.get(f"{BASE_URL}/waterflow-state", params=params)
        print(f"Status: {resp.status_code}\nRespuesta: {resp.json()}\n")

    elif choice == 4:
        token = input("Ingrese token: ").strip()
        mac = input("Ingrese waterflow_mac a emparejar: ").strip()
        payload = {"token": token, "waterflow_mac": mac}
        resp = requests.post(f"{BASE_URL}/send-token", json=payload)
        print(f"Status: {resp.status_code}\nRespuesta: {resp.json()}\n")

    elif choice == 5:
        print("Saliendo...")
        break

    else:
        print("Opción no válida. Intente de nuevo.")
