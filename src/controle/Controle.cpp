#include <Arduino.h>
#include "../Config.h"
#include "Controle.h"
#include "../objetos/BancoBateria.h"
#include "../wireless.h"

//Wireless
#include <WiFi.h> //lib to the Wifi configuration
#include <WiFiUdp.h>//Biblioteca do UDP.

//MQTT
#include <MqttClient.h>
//Json
#include <ArduinoJson.h>
//Date Time
#include <NTPClient.h>

WiFiUDP udp;//Cria um objeto da classe UDP.
static WiFiClient network;
//Global
MqttClient *mqtt = NULL;
//NTPClient
int16_t utc = 3;
//NTP CLient
NTPClient timeClient(udp, NTPSERVER);

//Construtor
Controle::Controle(){
}

// ============== Object to supply system functions ================================
class System: public MqttClient::System {
public:
  unsigned long millis() const {
    return ::millis();
  }
  void yield(void) {
    ::yield();
  }
};


/**
* Metodo inicialização do modulo
*/
void Controle::inicializaModulo(BancoBateria* bateria){
  _bateria = bateria;
  Serial.begin(VELOCIDADE_SERIAL_ARDUINO);
  Serial.println("## -- Iniciou Setup -- ##");
  calibraInicio();
  ativaRedeWIFI();
  configuraMQTT();
  ativaMQTT();
  pinMode(LED_PLACA,OUTPUT);
  Serial.println("## -- Fim Setup -- ##");
}

/*
* Ativa rede / DHCP
*/
void Controle::ativaRedeWIFI(){
  Serial.println("Ativando Wireless");
  WiFi.begin(ssid, password);             // Connect to the network
  Serial.print("Conectando a ");
  Serial.println(ssid);
  while (WiFi.status() != WL_CONNECTED) { // Wait for the Wi-Fi to connect
    delay(500);
    Serial.print('.');
  }
    Serial.println("");
  // print your local IP address:
  Serial.print("Endereco IP: ");
  Serial.println(WiFi.localIP());
  Serial.print("Endereco gateway: ");
  Serial.println(WiFi.gatewayIP());
  Serial.print("Endereco DNS: ");
  Serial.println(WiFi.dnsIP());
  Serial.println();
  Serial.println("Ajusta NTP ");
  timeClient.begin();
  delay(300);
  while(!timeClient.update()) {
    Serial.println("Tentando Update Hora");
    timeClient.forceUpdate();
  }
  timeClient.forceUpdate();
  Serial.print("##Data e Hora = ");
  Serial.println(timeClient.getFormattedTime());
  delay(2000);

}

/**
* Configura MQTT
*/
void Controle::configuraMQTT(){
  // Setup MqttClient
  MqttClient::System *mqttSystem = new System;
  MqttClient::Logger *mqttLogger = new MqttClient::LoggerImpl<HardwareSerial>(Serial);
  MqttClient::Network *mqttNetwork = new MqttClient::NetworkClientImpl<Client>(network, *mqttSystem);
  //// Make 128 bytes send buffer
  MqttClient::Buffer *mqttSendBuffer = new MqttClient::ArrayBuffer<300>();
  //// Make 128 bytes receive buffer
  MqttClient::Buffer *mqttRecvBuffer = new MqttClient::ArrayBuffer<300>();
  //// Allow up to 2 subscriptions simultaneously
  MqttClient::MessageHandlers *mqttMessageHandlers = new MqttClient::MessageHandlersImpl<2>();
  //// Configure client options
  MqttClient::Options mqttOptions;
  ////// Set command timeout to 10 seconds
  mqttOptions.commandTimeoutMs = 20000;
  //// Make client object
  mqtt = new MqttClient (
    mqttOptions, *mqttLogger, *mqttSystem, *mqttNetwork, *mqttSendBuffer,
    *mqttRecvBuffer, *mqttMessageHandlers
  );
}

/*
* Ativa MQTT
*/
void Controle::ativaMQTT(){
  if(!mqtt->isConnected()){
    network.stop();
    network.connect(BROKER_MQTT, BROKER_PORT);
    Serial.print("Conectando MQTT a ");
    Serial.println(BROKER_MQTT);
    MqttClient::ConnectResult connectResult;
    //Conect
    {
      MQTTPacket_connectData options = MQTTPacket_connectData_initializer;
      //4=3.1.1
      options.MQTTVersion = 4;
      String id = (String)ID_MQTT+MQTT_KEY;
      options.clientID.cstring = (char*)id.c_str();
      options.cleansession = false;
      options.keepAliveInterval = 20; // 15 seconds
      MqttClient::Error::type rc = mqtt->connect(options, connectResult);
      if (rc != MqttClient::Error::SUCCESS) {
        Serial.print("Connection error: ");
        Serial.println(rc);
        return;
      }
    }

    // Subscribe
    {
      MqttClient::Error::type rc = mqtt->subscribe(MQTT_SONOFF1, MqttClient::QOS0, processaMessage);
      // Serial.print("Subscribe Topico =|");
      // Serial.print(topico);
      // Serial.println("|");
      if (rc != MqttClient::Error::SUCCESS) {
        Serial.print("Erro leitura topicos:");
        Serial.println(rc);
        Serial.println("Desconectando ");
        mqtt->disconnect();
        return;
      }
    }
  }else{
    mqtt->yield(30000L);
  }
}

/*
* Calibração inicial.
*/
void Controle::calibraInicio(){
  Serial.println("Inicia Calibracao");
  _bateria->setQuantidadeCelulas(QUANTIDADE_CELULAS);
  for(int i = 1; i<=_bateria->getQuantidadeCelulas();i++){
    Serial.print("Celula ");
    Serial.print(i);
    Serial.println(" configurada...");
    delay(100);
  }
  Serial.print("Total de Celulas configuradas = ");
  Serial.println(_bateria->getQuantidadeCelulas());
  delay(100);
  //Inicializa banco de Bateria -> constroi celulas.
  _bateria->inicializaBanco();
  delay(1000);

  //Inicializa celulas com valores
  Serial.println("Configura portas de entrada e Saida e cria Objetos do banco.");
  int pinos_entrada[] = INPUT_PORT;
  int pinos_saida[] = OUTPUT_PORT;

  for(int i=0; i<_bateria->getQuantidadeCelulas();i++){
    // //Verifica se tem referencia registrada na EEprom
    int numero_cel = i+1;
    //Cria Objeto.
    ObjCelula obj;
    obj.setNumeroCelula(numero_cel);
    obj.setLeituraTensao(0.00);
    obj.setPortaInput(pinos_entrada[i]);
    obj.setPortaControle(pinos_saida[i]);
    //obj.setReferencia(RELACAO);
    //Porta digital
    Serial.print("Setando porta analogica GPI(O)");
    Serial.print(obj.getPortaInput());
    Serial.print(" entrada --> porta de controle ");
    //Ativa Input

    //Teste
    //adcAttachPin(pinos_entrada[i]);
    //analogSetClockDiv(255);
    pinMode(pinos_entrada[i], INPUT);
    Serial.print(obj.getPortaControle());
    Serial.println(" saida nivel baixo(LOW).");
    //Ativa em modo baixo
    pinMode(pinos_saida[i], OUTPUT);
    digitalWrite(pinos_saida[i], LOW);

    _bateria->setCelula(obj, i);
    delay(500);
  }
  delay(5000);
}

/*
* Faz leitura dados de input e atualiza dados da bateria
*e atualiza celulas
*/
void Controle::atualizaDadosLeitura(){
  for(int i=0; i<_bateria->getQuantidadeCelulas();i++){
    ObjCelula objj = _bateria->getCelula(i);
    double voltage = lePortaCalculoResistor(objj.getPortaInput(), false);
    objj.setLeituraTensao(voltage);
    //Atualiza Celula.
    _bateria->setCelula(objj,i);
  }
  //Imprime dados.
  _bateria->imprimeDados();
}


/*
Faz ontrole dos Mosfets para ligar e desligar
*/
void Controle::controlaSaidas(){
  for(int i=0; i<_bateria->getQuantidadeCelulas();i++){
    //Busca Objeto
    ObjCelula obj_i = _bateria->getCelula(i);
    //Coleta tensao eporta
    float tensao_i = obj_i.getLeituraTensao();
    int porta_i = obj_i.getPortaControle();
    bool ligaResistencia = false;
    for(int x=0; x<_bateria->getQuantidadeCelulas();x++){
      //Busca Opbjeto
      ObjCelula obj_x = _bateria->getCelula(x);
      if(obj_i.getNumeroCelula() != obj_x.getNumeroCelula()){
        //Coleta dados comparacao
        float tensao_x = obj_x.getLeituraTensao();
        Serial.print("####### Compara celula ");
        Serial.print(i+1);
        Serial.print(" tensao = ");
        Serial.print(tensao_i,5);
        Serial.print(", com celula ");
        Serial.print(x+1);
        Serial.print(" tensao = ");
        tensao_x += BMS;
        Serial.print(tensao_x,5);
        Serial.print(", porta estado = ");
        Serial.println(digitalRead(porta_i));
        if( (tensao_i > tensao_x) && !ligaResistencia){
          ligaResistencia = true;
        }
      }
    }
    if(ligaResistencia && digitalRead(porta_i) == LOW){
      digitalWrite(porta_i, HIGH);
      Serial.print("------------------------- liga porta ");
      Serial.print(porta_i);
      Serial.println("-------------------------");
    }else if(digitalRead(porta_i) == HIGH){
      digitalWrite(porta_i, LOW);
    }
  }
}

/*
Controla envio de dados da bateria ao MQTT via Json
*/
void Controle::MqttEnviaDados(){

  long unix_time = timeClient.getEpochTime();
  StaticJsonDocument<300> doc;
  JsonObject root = doc.to<JsonObject>();
  //Bateria
  root["codigo"] = 0;
  root["bat_qtcells"] = _bateria->getQuantidadeCelulas();
  root["bat_per"] = _bateria->getPercentual();
  root["bat_vbat"] = _bateria->getTensaoBanco();
  root["bat_vmin"] = _bateria->getTensaoMinima();
  root["bat_vmax"] = _bateria->getTensaoMaxima();
  root["time"] = unix_time;

  String mensagem;
  //root.printTo(mensagem);
  serializeJson(root,mensagem);
  MqttSendMessage(MQTT_DATA,  mensagem);

  for(int i=0; i<_bateria->getQuantidadeCelulas();i++){
    //Busca Objeto
    ObjCelula obj_i = _bateria->getCelula(i);
    // //Coleta tensao eporta
    // //float tensao_i = obj_i.getLeituraTensao();
    StaticJsonDocument<300> doc;
    JsonObject root = doc.to<JsonObject>();
    //Celulas
    root["codigo"] = 1;
    root["cel_num"] = i+1;
    root["cel_vcel"] = obj_i.getLeituraTensao();
    root["cel_per"] = obj_i.getPercentual();
    root["cel_vmin"] = obj_i.getTensaoMinima();
    root["cel_vmax"] = obj_i.getTensaoMaxima();
    root["time"] = unix_time;
    String mensagem;
    //root.printTo(mensagem);
    serializeJson(root, mensagem);
    MqttSendMessage(MQTT_DATA,  mensagem);
  }
}

// ============== Subscription callback ========================================
void Controle::processaMessage(MqttClient::MessageData& md) {
  const MqttClient::Message& msg = md.message;
  Serial.print("Topico recebido == ");
  Serial.println(md.topicName.cstring);
  Serial.print("Qos == ");
  Serial.println(msg.qos);
  char payload[msg.payloadLen + 1];
  memcpy(payload, msg.payload, msg.payloadLen);
  payload[msg.payloadLen] = '\0';
  Serial.println("Mensagem Chegou");
  Serial.print("Size = ");
  Serial.println(msg.payloadLen);
  Serial.print("Mensagem Subscribe = ");
  Serial.println(payload);
  //Deserializa Json
  StaticJsonDocument<300> doc;
  deserializeJson(doc,payload);
  //Dados reescritos
  long unix_time = timeClient.getEpochTime();
  StaticJsonDocument<300> doc2;
  JsonObject root = doc2.to<JsonObject>();
  //Energia concessionaria
  //{"Time":"2019-05-15T16:30:39","ENERGY":{"TotalStartTime":"2019-05-01T19:28:55","Total":8.191,"Yesterday":0.828,"To
  //day":0.548,"Period":0,"Power":29,"ApparentPower":52,"ReactivePower":44,"Factor":0.56,"Voltage":219,"Current":0.239}}
  root["codigo"] = 2;
  root["Time"] = doc["Time"];
  root["TotalStartTime"] = doc["ENERGY"]["TotalStartTime"];
  root["Total"] = doc["ENERGY"]["Total"];
  root["Today"] = doc["ENERGY"]["Today"];
  root["Period"] = doc["ENERGY"]["Period"];
  root["Power"] = doc["ENERGY"]["Power"];
  root["ApparentPower"] = doc["ENERGY"]["ApparentPower"];
  root["ReactivePower"] = doc["ENERGY"]["ReactivePower"];
  root["Factor"] = doc["ENERGY"]["Factor"];
  root["Voltage"] = doc["ENERGY"]["Voltage"];
  root["Current"] = doc["ENERGY"]["Current"];
  root["time"] = unix_time;
  String mensagem;
  //root.printTo(mensagem);
  serializeJson(root, mensagem);
  MqttSendMessage(MQTT_DATA,  mensagem);
}


/*
Envia Mensagem MQTT
*/
void Controle::MqttSendMessage(String topico, String mensagem){
  //Verifica se esta OK.
  if(mqtt->isConnected()){
    digitalWrite(LED_PLACA,HIGH);
    Serial.print("Envia MSG  = ");
    Serial.println(mensagem);
    MqttClient::Message message;
    // Send and receive QoS 0 message
    char buf[300];
    strcpy(buf, mensagem.c_str());
    message.qos = MqttClient::QOS1;
    message.retained = false;
    message.dup = false;
    message.payload = (void*)buf;
    message.payloadLen = strlen(buf);
    mqtt->publish(topico.c_str(), message);
    digitalWrite(LED_PLACA,LOW);
  }
}

/*
Verifica rede a cada 2 minutos.
tenta reconectar.
*/
void Controle::verificaRede(){
  Serial.println("Verifica Rede 2 miutos");
  // print your local IP address:
  Serial.print("Endereco IP: ");
  Serial.println(WiFi.localIP());
  if(!mqtt->isConnected()){
    ativaMQTT();
  }
  Serial.print("Atualiza Data e Hora = " );
  timeClient.forceUpdate();
  Serial.println(timeClient.getFormattedTime());
  delay(300);
}

/*
verifica referencias de leitura do calculo
*/
void Controle::ciloProcessamento(){
  atualizaDadosLeitura();
  controlaSaidas();
}
