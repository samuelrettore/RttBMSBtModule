#ifndef ControlaIO_h
#define ControlaIO_h

#include <Arduino.h>


class ControlaIO {
public:
  float lePortaShield25v(uint8_t portaAnalogica);
  double lePortaCalculoResistor(uint8_t portaAnalogica, bool filtro);
  float lePortaDivisor(uint8_t portaAnalogica, float referencia);
  double f13_semPow(double x);
  double f6_semPow(double x); 
private:

};
#endif
