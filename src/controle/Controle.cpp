#include <Arduino.h>
#include "../Config.h"
#include "Controle.h"
#include "../objetos/BancoBateria.h"

//Bluhtooth
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

//Json
#include <ArduinoJson.h>
bool deviceConnected = false;
BLECharacteristic *characteristicTX;
int contador = 0;
//Construtor
Controle::Controle(){
}

//callback para receber os eventos de conexão de dispositivos
class ServerCallbacks: public BLEServerCallbacks {
  void onConnect(BLEServer* pServer) {
    deviceConnected = true;
  };

  void onDisconnect(BLEServer* pServer) {
    deviceConnected = false;
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
  ativaRedeBluethooth();
  pinMode(LED_PLACA,OUTPUT);
  Serial.println("## -- Fim Setup -- ##");
}

/*
* Ativa rede / DHCP
*/
void Controle::ativaRedeBluethooth(){
  Serial.println("Ativando Bluehooth");
  // Create the BLE Device
  BLEDevice::init("Bateria-module"); // nome do dispositivo bluetooth
  // Create the BLE Server
  BLEServer *server = BLEDevice::createServer(); //cria um BLE server

  server->setCallbacks(new ServerCallbacks()); //seta o callback do server

  // Create the BLE Service
  BLEService *service = server->createService(SERVICE_UUID);

  // Create a BLE Characteristic para envio de dados
  characteristicTX = service->createCharacteristic(
    CHARACTERISTIC_UUID_TX,
    BLECharacteristic::PROPERTY_NOTIFY
  );

  characteristicTX->addDescriptor(new BLE2902());
  service->start();
  // Start advertising (descoberta do ESP32)
  server->getAdvertising()->start();

  delay(2000);
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
verifica referencias de leitura do calculo
*/
void Controle::ciloProcessamento(){
  atualizaDadosLeitura();
  controlaSaidas();
  if(deviceConnected){
    // Let's convert the value to a char array:
    std::string valor = "Olaaa ";
    characteristicTX->setValue(contador); //seta o valor que a caracteristica notificará (enviar)
    characteristicTX->notify(); // Envia o valor para o smartphone
    Serial.println("Envia valor");
    contador++;
    delay(1000);
  }
}
