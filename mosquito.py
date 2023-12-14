import paho.mqtt.client as mqtt

# Função chamada quando o cliente se conecta ao broker MQTT
def on_connect(client, userdata, flags, rc):
    print("Conectado: " + str(rc))
    client.subscribe("canal/teste")

# Função chamada quando uma nova mensagem é recebida no canal
def on_message(client, userdata, msg):
    print("Mensagem Recebida: " + msg.topic + " " + str(msg.payload.decode()))

# Configuração e inicialização do cliente MQTT
client = mqtt.Client()
client.on_connect = on_connect
client.on_message = on_message
client.connect("localhost", 1883, 60)

# Loop infinito para manter a conexão ativa
client.loop_forever()