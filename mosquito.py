import paho.mqtt.client as mqtt

def on_connect(client, userdata, flags, rc):
    print("Conectado ao servidor MQTT")
    client.subscribe("canal1")
    client.subscribe("canal2")

def on_message(client, userdata, msg):
    print("Mensagem recebida: " + msg.payload.decode())

def on_publish(client, userdata, mid):
    print("Mensagem enviada")

client = mqtt.Client()
client.on_connect = on_connect
client.on_message = on_message
client.on_publish = on_publish

client.connect("localhost", 1883, 60)

client.loop_start()

while True:
    mensagem = input()
    client.publish("canal1", mensagem)

client.loop_stop()
client.disconnect()