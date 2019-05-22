#ifndef ObjCelula_h
#define ObjCelula_h

#include "Arduino.h"
#include "../Config.h"

class ObjCelula {
  public:
    double getLeituraTensao();
    double getLeituraTensaoAnterior();
    void setLeituraTensao(double leitura);
    int getNumeroCelula();
    void setNumeroCelula(int numero_celula);
    void setPortaInput(int porta);
    int getPortaInput();
    void setPortaControle(int porta);
    int getPortaControle();
    int getPercentual();
    void setReferencia(double referencia);
    double getReferencia();
    double getTensaoMinima();
    double getTensaoMaxima();
  private:
    //Numero da celula
    int _numero_celula;
    int _porta_input;
    int _porta_controle;
    double _referencia;
    double _leitura_tensao;
    double _tensao_anterior;
    double _percentual;
    double _tensao_minima = MIN_VOLTAGE_CELL;
    double _tensao_maxima = MAX_VOLTAGE_CELL;
};
#endif
