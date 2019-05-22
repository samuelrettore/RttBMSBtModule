/*
_____    _     _     ____    __  __    _____
|  __ \  | |   | |   |  _ \  |  \/  |  / ____|
| |__) | | |_  | |_  | |_) | | \  / | | (___
|  _  /  | __| | __| |  _ <  | |\/| |  \___ \
| | \ \  | |_  | |_  | |_) | | |  | |  ____) |
|_|  \_\  \__|  \__| |____/  |_|  |_| |_____/

Projeto desenvolvido por Samuel Rettore
email -> rettore@gmail.com


Definicao de Quantos S(QUANTIDADE_CELULAS) ...ou celulas tem o Sistema
Por exemplo um Projeto de 2S(duas celulas) segue o seguinte mapeamento
36 para leitura de tensao e 31 para controle da celulas:
A8 -> 31
A9 -> 33
......

SE VOCE DEFINIR 2 CELULAS o sistema entendera que seu banco terá duas celulas,
se definir 7 usará mesma logica.

Ex 3S ou 3 CELULAS

#define QUANTIDADE_CELULAS 3

O sistema ira definir a porta A8 para o leitor de tensão da celula 1 e o 31 aciona sistema de resistor para equalizar celula
O sistema ira definir a porta A9 para o leitor de tensão da celula 2 e o 33 aciona sistema de resistor para equalizar celula
O sistema ira definir a porta A10 para o leitor de tensão da celula 3 e o 35 aciona sistema de resistor para equalizar celula

Ex 7S ou 7 CELULAS

#define QUANTIDADE_CELULAS 7

O sistema ira definir a porta A8 para o leitor de tensão da celula 1 e o 31 aciona sistema de resistor para equalizar celula
O sistema ira definir a porta A9 para o leitor de tensão da celula 2 e o 33 aciona sistema de resistor para equalizar celula
O sistema ira definir a porta A10 para o leitor de tensão da celula 3 e o 35 aciona sistema de resistor para equalizar celula
O sistema ira definir a porta A11 para o leitor de tensão da celula 4 e o 37 aciona sistema de resistor para equalizar celula
O sistema ira definir a porta A12 para o leitor de tensão da celula 5 e o 39 aciona sistema de resistor para equalizar celula
O sistema ira definir a porta A13 para o leitor de tensão da celula 6 e o 41 aciona sistema de resistor para equalizar celula
O sistema ira definir a porta A14 para o leitor de tensão da celula 7 e o 43 aciona sistema de resistor para equalizar celula

Abaixo pré definido sistema de 7S ou 7 celulas
*/
//Aqui define o tamanho do seu banco em celulas ou S
#define QUANTIDADE_CELULAS 7

//Pinos entrada
#define INPUT_PORT {36, 39, 34, 35, 32, 33, 25}
//Pinos de Saida PWM
#define OUTPUT_PORT {22, 1, 3, 21, 19, 18, 5}

/*
Calculo Baseado no resistor
Tensao Referencia porta analogica Arduino - medir com multimetro tensão de operação do arduino melhora eficiencia da
leitura de tensao
*/
#define TENSAO_REFERENCIA 3.3

/*Fator de correcao para adicionar e estabilizar tensao final
*/
#define FATOR_CORRECAO 1.752
//#define FATOR_CORRECAO 0.298

//Configuracao do valor referencial que a bms trabalha em mV ou seja so vai agir caso de diferença de 0.060 milivolts
#define BMS 0.060

//Tensao Minima por celula
#define MIN_VOLTAGE_CELL 11.5

//Tensao Maxima por celula usado para estatistica.
#define MAX_VOLTAGE_CELL 13.8

/*
Dados de leitura com divisor de tensao
*/
//Numero de amostras na leitura
#define AMOSTRAS 2000

//Velocidade Serial Arduino
#define VELOCIDADE_SERIAL_ARDUINO 115200
#define LED_PLACA 2

//Dados Bluehooth
#define SERVICE_UUID   "ab0828b1-198e-4351-b779-901fa0e0371e"
#define CHARACTERISTIC_UUID_RX  "4ac8a682-9736-4e5d-932b-e9b31405049c"
#define CHARACTERISTIC_UUID_TX  "0972EF8C-7613-4075-AD52-756F33D4DA91"
