import json
import logging
from datetime import datetime
import paho.mqtt.client as mqtt
from database import save_temperature, init_db
from paho.mqtt.enums import CallbackAPIVersion

# Configuração do logging
logging.basicConfig(
    level=logging.INFO,
    format="%(asctime)s [%(levelname)s] %(message)s",
    datefmt="%Y-%m-%d %H:%M:%S"
)
logger = logging.getLogger(__name__)

BROKER = "localhost"
TOPIC = "agroedge/temperature"

def on_connect(client, userdata, flags, reason_code, properties):
    if reason_code == 0:
        logger.info(f"Conectado ao broker MQTT em {BROKER}")
        client.subscribe(TOPIC)
        logger.info(f"Inscrito no tópico: {TOPIC}")
    else:
        logger.error(f"Falha na conexão. Código de retorno: {reason_code}")

def on_message(client, userdata, msg):
    try:
        data = json.loads(msg.payload.decode())
        value = data["value"]
        logger.info(f"Mensagem recebida | Tópico: {msg.topic} | Temperatura: {value}°C")
        save_temperature(value)
        logger.info(f"Temperatura {value}°C salva no banco de dados com sucesso")
    except json.JSONDecodeError as e:
        logger.error(f"Erro ao decodificar JSON: {e} | Payload: {msg.payload}")
    except KeyError as e:
        logger.error(f"Campo ausente no payload: {e} | Data: {data}")
    except Exception as e:
        logger.error(f"Erro inesperado ao processar mensagem: {e}")

def on_disconnect(client, userdata, flags, reason_code, properties):
    if reason_code != 0:
        logger.warning(f"Desconexão inesperada. Código: {reason_code}. Tentando reconectar...")
    else:
        logger.info("Desconectado do broker MQTT.")

client = mqtt.Client(callback_api_version=mqtt.CallbackAPIVersion.VERSION2)
client.on_connect = on_connect
client.on_message = on_message
client.on_disconnect = on_disconnect

logger.info(f"Iniciando servidor MQTT | Broker: {BROKER} | Tópico: {TOPIC}")
init_db()

try:
    client.connect(BROKER, 1883, 60)
    client.loop_forever()
except KeyboardInterrupt:
    logger.info("Servidor encerrado pelo usuário.")
    client.disconnect()
except Exception as e:
    logger.error(f"Erro ao conectar ao broker: {e}")
