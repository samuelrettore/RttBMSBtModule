#include "../Config.h"
#include "Controle.h"

/*
Le Dados porta Analogica analogReader
Usando Shield de tensão 0-25v
*/
float ControlaIO::lePortaShield25v(uint8_t portaAnalogica) {
  int valorDC = 0;
  float tensaoDC = 0;
  valorDC = analogRead(portaAnalogica);
  tensaoDC = ((valorDC*0.00489)*TENSAO_REFERENCIA);
  return tensaoDC;
}

/*
Le Dados porta Analogica analogReader
usando calculo de resistores.
*/
double ControlaIO::lePortaCalculoResistor(uint8_t portaAnalogica, bool filtro) {
  double vout = 0.0;
  double vin = 0.0;
  double R1 = 1000000.0; // Resistor 1 - 1M
  double R2 = 100000.0; // Resistor 2 - 100k
  double value = 0;
  // Le valor da porta de entrada
  //value = analogRead(portaAnalogica);

  //Le Media
  int x = AMOSTRAS;
  for(int i = 1; i<=x;i++){
    if(filtro){
      value += f6_semPow(analogRead(portaAnalogica));
    }else{
      value += analogRead(portaAnalogica);
    }
  }
  value = value/AMOSTRAS;
  //Fim Media
  vout = (value *TENSAO_REFERENCIA) / 4095.0;
  vin = vout / (R2/(R1+R2));
  if(vin<0.1){
    vin = 0;
  }else{
    vin += FATOR_CORRECAO;
  }
  Serial.print("Filtro == ");
  Serial.println(filtro);
  return vin;
}
/*
@Deprecated
Le Dados porta Analogica analogReader
Usando bloco de divisores de tensao.
*/
float ControlaIO::lePortaDivisor(uint8_t portaAnalogica, float referencia) {
  float total=0;
  for (int i=0; i<AMOSTRAS; i++) {
    total += 1.0 * analogRead(portaAnalogica);
  }
  total = total / (float)AMOSTRAS;
  if(referencia<=0){
    Serial.print("#------------------- leitura relacao porta = ");
    Serial.print(portaAnalogica);
    Serial.print(", valor ");
    Serial.print(total,5);
    Serial.println("/Tensao Multimetro");
  }
  //Divide pela relacao
  total = total/referencia;
  if(total<0.1){
    total = 0.0;
  }
  //total  =4.1;
  return total;
}

//FUNÇÃO DE GRAU 13 polimonial SEM USAR O POW, melhora performance
//Filtro leitura ADC
double ControlaIO::f13_semPow(double x) {
  return   2.161282383460e+02
  +   3.944594843419e-01 * x
  +   5.395439724295e-04 * x * x
  +  -3.968558178426e-06 * x * x * x
  +   1.047910519933e-08 * x * x * x * x
  +  -1.479271312313e-11 * x * x * x * x * x
  +   1.220894795714e-14 * x * x * x * x * x * x
  +  -6.136200785076e-18 * x * x * x * x * x * x * x
  +   1.910015248179e-21 * x * x * x * x * x * x * x * x
  +  -3.566607830903e-25 * x * x * x * x * x * x * x * x * x
  +   5.000280815521e-30 * x * x * x * x * x * x * x * x * x * x
  +   3.434515045670e-32 * x * x * x * x * x * x * x * x * x * x * x
  +  -1.407635444704e-35 * x * x * x * x * x * x * x * x * x * x * x * x
  +   9.871816383223e-40 * x * x * x * x * x * x * x * x * x * x * x * x * x;
}


//FUNÇÃO DE GRAU 13 polimonial SEM USAR O POW, melhora performance
//Filtro leitura ADC
double ControlaIO::f6_semPow(double x) {
  return   2.161282383460e+02
  +   3.944594843419e-01 * x
  +   5.395439724295e-04 * x * x
  +  -3.968558178426e-06 * x * x * x;
  // +   1.047910519933e-08 * x * x * x * x
  // +  -1.479271312313e-11 * x * x * x * x * x
  // +   1.220894795714e-14 * x * x * x * x * x * x
  // +  -6.136200785076e-18 * x * x * x * x * x * x * x
  // +   1.910015248179e-21 * x * x * x * x * x * x * x * x
  // +  -3.566607830903e-25 * x * x * x * x * x * x * x * x * x;
  }
