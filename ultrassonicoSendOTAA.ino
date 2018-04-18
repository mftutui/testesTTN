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

void setup()
{
 
  loraSerial.begin(57600);
  debugSerial.begin(9600);

  // Wait a maximum of 10s for Serial Monitor
  while (!debugSerial && millis() < 10000)
    ;

  debugSerial.println("-- STATUS");
  ttn.showStatus();

  debugSerial.println("-- JOIN");
  ttn.join(appEui, appKey);
}

void loop()
{
  debugSerial.println("-- LOOP");

  //Le as informacoes do sensor em cm
  String cmMsec;
  uint32_t microsec = ultrasonic.timing();
  cmMsec = ultrasonic.convert(microsec, Ultrasonic::CM) * 100;
  
  //Exibe informacoes no serial monitor
  debugSerial.println("Distancia em cm: ");
  debugSerial.println(cmMsec);

  String antesVirgula = cmMsec.substring(0,2);
  String depoisVirgula = cmMsec.substring(2,4);
  uint32_t aV = antesVirgula.toInt();
  uint32_t dV = depoisVirgula.toInt();
  
  //Inicializar payload
  byte payload[4];
  payload[0] = highByte(aV);
  payload[2] = lowByte(aV);
  payload[3] = highByte(dV);
  payload[4] = lowByte(dV);

  ttn.sendBytes(payload, sizeof(payload));
  
  delay(10000);
}
