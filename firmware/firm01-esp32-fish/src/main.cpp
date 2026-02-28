/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
                              AgroEdge - ESP32
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
VERSÃO : 1.0
DATA   : 28/02/2026
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
//==========================================================================
// Firmware da smartbox | Fish.
//==========================================================================

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
  1.      ARQUIVOS DE DEFINIÇÕES (INCLUDES)
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#include <WiFi.h>
#include <PubSubClient.h>
#include <OneWire.h>
#include <DallasTemperature.h>

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
  2.      DECLARAÇÕES
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

// ===== WIFI =====
const char *ssid = "SSID";
const char *password = "PASSWORD";

// ===== MQTT =====
const char *mqtt_server = "192.168.1.19"; // IP da Raspberry
const int mqtt_port = 1883;
const char *topic = "agroedge/temperature";

// ===== SENSOR =====
#define ONE_WIRE_BUS 4
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

// ===== CLIENTS =====
WiFiClient espClient;
PubSubClient client(espClient);

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
  3.     FUNÇÕES
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void initWifi(void);
void reconnect(void);

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
  4.      FUNÇÃO SETUP
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void setup(){
  Serial.begin(115200);
  initWifi();
  client.setServer(mqtt_server, mqtt_port);
  sensors.begin();
  Serial.println("Setup Finished");
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
  5.      FUNÇÃO LOOP
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void loop(){
  if (!client.connected()){
    reconnect();
  }

  client.loop();

  sensors.requestTemperatures();
  float temperature = sensors.getTempCByIndex(0);

  char payload[50];
  sprintf(payload, "{\"value\": %.2f}", temperature);

  client.publish(topic, payload);

  Serial.println("Pacote enviado: " + String(payload));

  delay(10000); // envia a cada 10 segundos
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
  6.      FUNÇÕES DE USUÁRIO
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

 /* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
    6.1     Configurações
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

//==========================================================================
// Função: Inicializa Wifi
// Parâmetros: nenhum
// Retorno: nenhum
void initWifi(void) {
  delay(5000);
  Serial.println("Conectando...");
  WiFi.disconnect(true);
  WiFi.mode(WIFI_MODE_STA);
  WiFi.begin(ssid, password);

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to2 ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.print("MAC address: ");
  Serial.println(WiFi.macAddress());
}

//==========================================================================
// Função: Reconnect Wifi
// Parâmetros: nenhum
// Retorno: nenhum
void reconnect(void){
  while (!client.connected()) {
    Serial.print("Tentando conectar ao MQTT... ");
    if (client.connect("ESP32_Temp_Node")) {
      Serial.println("Conectado!");
    } else {
      Serial.print("Falhou. Estado: ");
      Serial.println(client.state()); // imprime o código de erro
      delay(2000);
    }
  }
}
