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
A8 para leitura de tensao e 31 para controle da celulas:
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
#define QUANTIDADE_CELULAS 1

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
#define FATOR_CORRECAO 1.862
//#define FATOR_CORRECAO 0.298

//Configuracao do valor referencial que a bms trabalha em mV ou seja so vai agir caso de diferença de 0.060 milivolts
#define BMS 0.060

//Tensao Minima por celula
#define MIN_VOLTAGE_CELL 11.5

//Tensao Maxima por celula usado para estatistica.
#define MAX_VOLTAGE_CELL 13.8

/*
Porta controla rele contatora ou inversor
Porta que desliga dispotivo quando tensao minima atingida.
*/
#define CONTATORA_INVERSOR 30

/*
Dados de leitura com divisor de tensao
*/
//Numero de amostras na leitura
#define AMOSTRAS 2000

//Metodo Base de relacao
//#define RELACAO 18.18500 //11.86, old methodo Deprecated
//#define RELACAO 17.72637 //4.2

/*
Dados MQTT
*/
//#define BROKER_MQTT "iot.eclipse.org"
//#define MQTT_LOG_ENABLED 1
#define BROKER_MQTT "mqtt.serhmatica.com.br"
#define BROKER_PORT 1883
#define ID_MQTT "Esp32-v2-MQTT"
// #define MQTT_VERSION MQTT_VERSION_3_1_1

//Esta Chave deve ser criada peloAPP RTTHome e atualizada aqui para comunicação.
#define MQTT_KEY "376f0d9743"
//Definicoes MQTT
//#define MQTT_DATA "376f0d9743/dados"
//Dev
#define MQTT_DATA "376f0d9743/dados"
#define MQTT_SONOFF1 "376f0d9743/sonoff/SENSOR"

//#define MQTT_RESEND1 "/sonoff1/SENSOR"

//#define MQTT_TOPIC "376f0d9743/bateria"
//Topico Sonoff Concessionaria
//#define MQTT_TOPIC_SONOFF_CON "376f0d9743/sonoff/SENSOR"

//NTP Server
#define NTPSERVER "gps.ntp.br"

/*
Outros dados
*/
//Configuracoes placa de rede.
//#define W5100_CS  10
#define SDCARD_CS 4

//Velocidade Serial Arduino
#define VELOCIDADE_SERIAL_ARDUINO 115200
#define LED_PLACA 2
