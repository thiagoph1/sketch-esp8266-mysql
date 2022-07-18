/*
 * Monitor o volume na sua caixa d'água Estágio UNINTER
 * Autor: Thiago (1220215)
 * Biblioteca da Placa: "esp8266 by ESP8266 Community versão 2.3.0"
 * Placa: "NodeMCU 1.0 (ESP-12E Module)"
 * Upload Speed: "115200"
 * CPU Frequency: "80MHz"
*/
//=====================================================================
// --- Inclusão de bibliotecas ---
#include <ESP8266WiFi.h> 
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
// ======================================================================
// --- Dados de Acesso do seu roteador ---
#define WLAN_SSID       "----" // Informação da SSID do seu roteador
#define WLAN_PASS       "----"   // senha de acesso do seu roteador
// ======================================================================
// --- Mapeamento de dados ---
#define server_ip    "192.168.0.25"
WiFiClient client; // cria uma classe WiFiClient com o ESP8266 
// ======================================================================
// --- Mapeamento de Hardware ---
#define trigPin 5  //D1 - PINO DE TRIGGER PARA SENSOR ULTRASSONICO
#define echoPin 4  //D2 - PINO DE ECHO PARA SENSOR ULTRASSONICO
// ======================================================================
// --- Variáveis Globais ---
long  duracao   = 0;
float distancia = 0;
float volumevar = 0;
// ======================================================================
// --- Void Setup ---
void setup() {
  delay(2000);
  Serial.begin(115200); // inicia comunicação serial com velocidade 115200
  Serial.println();
  Serial.print("Conectando ao ");
  Serial.println(WLAN_SSID);
  // Conecta ao ponto de acesso WiFi
  WiFi.begin(WLAN_SSID, WLAN_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(300);
    Serial.print(".");
  }
  Serial.println();
  Serial.println("WiFi Conectado");
  Serial.println("IP endereço: "); Serial.println(WiFi.localIP()); 
// ======================================================================
// --- Configuração IO ---
  pinMode(trigPin, OUTPUT); // pino D1 como saída para TRIGGER
  pinMode(echoPin, INPUT);  // pino D2 como entrada para ECHO
}
// ======================================================================
// --- void loop ---
void loop() {

// Leitura dos dados do Sensor HC104
  digitalWrite(trigPin, LOW);    // Limpa trigPin
  delayMicroseconds(2);          // aguarda 2 microsegundos
 
  digitalWrite(trigPin, HIGH);   // Seta trigPin HIGH aguarda 
  delayMicroseconds(10);         // aguada 10 microsegundos
  digitalWrite(trigPin, LOW);    // Seta trigPin LOW 
   
  // Leitura do echoPin, retorna a onda de som em microsegundos
  duracao = pulseIn(echoPin, HIGH);
  distancia= duracao*0.034/2;
 
  if (distancia > 100){  // leitura mínima. Reservatório vazio
    distancia = 100;
  }
    if (distancia < 0){  // leitura máxima. Reservatório vazio
    distancia = 0;
  }
  volumevar = map(distancia, 1, 20, 100, 0); 
  /* Remapeia o range de leitura
   * Ao invés de ler de 1 a 20, lerá de 100 a 0 para expressar a porcentagem*/
 // Serial.println();
 // Serial.print("distancia:"); // imprime "distancia:"
 // Serial.println(distancia);  // imprime a variavel distancia
 // Serial.print("volume:");    // imprime "volume:"
 //  Serial.println(volumevar); // imprime a variavel volume
  // nossa saída será em mL
// ======================================================================
if(WiFi.status()== WL_CONNECTED) {// Checar conexão do Wifi
  HTTPClient http;

  // Inicia comunicação com o servidor
  http.begin(client, "http://"server_ip"/inserir.php/");
  
  //Convertar os dados para String
  String valorDistancia = String(distancia);
  String valorVolume = String(volumevar);

  //  Prepara o HTTP para POST
  //String postDados = "{\"distancia\":\"" + valorDistancia + "\",\"volume\":\"" + valorVolume + "\"}"; ->  JSON
    String postDados = "distancia=" + valorDistancia + "&volume=" + valorVolume;
  
  Serial.print("httpRequestData:");
  Serial.println(postDados);
  
   http.addHeader("Content-Type", "application/x-www-form-urlencoded"); // se for json "application/json"
   int httpCode = http.POST(postDados);
 
    // Serial.println(httpCode);
     if (httpCode > 0) {      
      if (httpCode == HTTP_CODE_OK) {
        const String& payload = http.getString();
        Serial.println("received payload:\n<<");
        Serial.println(payload);
        Serial.println(">>");
      }
    } else {
      Serial.printf("[HTTP] POST failed, error: %s\n", http.errorToString(httpCode).c_str());
    }
  //liberar recursos
  http.end();
} else {
  Serial.println("WiFi Disconnected");
}
  delay(5000); // aguarda 5 segundos
}
 
// ======================================================================
// --- FIM ---
