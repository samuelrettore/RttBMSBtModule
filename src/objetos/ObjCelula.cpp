#include "Arduino.h"
#include "ObjCelula.h"

double ObjCelula::getLeituraTensao(){
  return _leitura_tensao;
}

void ObjCelula::setLeituraTensao(double leitura){
  //copia tensao anterior
  _tensao_anterior = _leitura_tensao;
  //Calcula percentual da celula.
  _percentual = map((leitura*100), (MIN_VOLTAGE_CELL*100), (MAX_VOLTAGE_CELL*100), 0, 100);
  _leitura_tensao = leitura;
}

int ObjCelula::getNumeroCelula(){
  return _numero_celula;
};

void ObjCelula::setNumeroCelula(int numero_celula){
  _numero_celula = numero_celula;
}

double ObjCelula::getLeituraTensaoAnterior(){
  return _tensao_anterior;
}

void ObjCelula::setPortaInput(int porta){
  _porta_input = porta;
}

int ObjCelula::getPortaInput(){
  return _porta_input;
}

void ObjCelula::setPortaControle(int porta){
  _porta_controle = porta;
}

int ObjCelula::getPortaControle(){
  return _porta_controle;
}

int ObjCelula::getPercentual(){
  return _percentual;
}

void ObjCelula::setReferencia(double referencia){
  _referencia = referencia;
}

double ObjCelula::getReferencia(){
    return _referencia;
}

double ObjCelula::getTensaoMinima(){
  return _tensao_minima;
}

double ObjCelula::getTensaoMaxima(){
  return _tensao_maxima;
}
