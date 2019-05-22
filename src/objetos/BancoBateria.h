#ifndef BancoBateria_h
#define BancoBateria_h
#include "Arduino.h"
#include "ObjCelula.h"

class BancoBateria {
  public:
    //construtor
    int getQuantidadeCelulas();
    void setQuantidadeCelulas(int numero_celula);
    void inicializaBanco();
    void imprimeDados();
    int getPercentual();
    double getTensaoMinima();
    double getTensaoMaxima();
    //CELULAS
    ObjCelula getCelula(int posicao);
    void setCelula(ObjCelula objeto, int posicao);
    double getTensaoBanco();
  private:
    //Numero de celulas do banco
    int _quantidade_celulas = 0;
    double _percentual;
    double _tensao_minima;
    double _tensao_maxima;
    ObjCelula* celulas;
};
#endif
