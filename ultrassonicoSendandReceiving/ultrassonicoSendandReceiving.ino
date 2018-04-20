#include <TheThingsNetwork.h>

//Carrega a biblioteca do sensor ultrassonico
#include <Ultrasonic.h>
 
//Define os pinos para o trigger e echo
#define pino_trigger 4
#define pino_echo 5
 
//Inicializa o sensor nos pinos definidos acima
Ultrasonic ultrasonic(pino_trigger, pino_echo);

// Set your AppEUI and AppKey
const char *appEui = "70B3D57ED000B9C0";
const char *appKey = "9B98649146CB775AB9361632FC81427B";

#define loraSerial Serial1
#define debugSerial Serial

// Replace REPLACE_ME with TTN_FP_EU868 or TTN_FP_US915
#define freqPlan TTN_FP_US915

TheThingsNetwork ttn(loraSerial, debugSerial, freqPlan);

#define ledVerde 12
#define ledVermelho 13

void setup()
{
 
  loraSerial.begin(57600);
  debugSerial.begin(9600);

  // Wait a maximum of 10s for Serial Monitor
  while (!debugSerial && millis() < 10000);

  debugSerial.println("-- STATUS");
  ttn.showStatus();

  debugSerial.println("-- JOIN");
  ttn.join(appEui, appKey);

  pinMode(ledVerde, OUTPUT); //Pino do led será saída
  pinMode(ledVermelho, OUTPUT); //Pino do led será saída

  ttn.onMessage(mensagem);
  
}

void loop()
{
  debugSerial.println("-- LOOP");

  //Le as informacoes do sensor em cm
  String cmMsec, teste;
  uint32_t microsec = ultrasonic.timing();
  cmMsec = ultrasonic.convert(microsec, Ultrasonic::CM) * 100;  // 8 100 pra poder mandar no payload
  teste = ultrasonic.convert(microsec, Ultrasonic::CM);        //centimetros para printar na serial
  
  //Exibe informacoes no serial monitor
  debugSerial.print("Distancia em cm: ");
  debugSerial.println(teste);

  String antesVirgula = cmMsec.substring(0,2); 
  String depoisVirgula = cmMsec.substring(2,4);
  uint32_t aV = antesVirgula.toInt();
  uint32_t dV = depoisVirgula.toInt();
  
  //Inicializar payload
  byte payload[4];
  payload[0] = highByte(aV);
  payload[1] = lowByte(aV);
  payload[2] = highByte(dV);
  payload[3] = lowByte(dV);

  ttn.sendBytes(payload, sizeof(payload));
  
  delay(7000);
}

void mensagem(const uint8_t *payload, size_t size, port_t port)
{
  if (payload[0] == 1)
  {
    digitalWrite(ledVermelho, HIGH);
    digitalWrite(ledVerde, LOW);
  }
  else
  {
    digitalWrite(ledVermelho, LOW);
    digitalWrite(ledVerde, HIGH);
  }
}

