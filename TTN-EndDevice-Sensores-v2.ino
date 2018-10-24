#include <TheThingsNetwork.h>

//Biblioteca do sensor ultrassonico
#include <Ultrasonic.h>

//Biblioteca para o sensor de temperatura
#include <Thermistor.h>

//Carrega a biblioteca LiquidCrystal
#include <LiquidCrystal.h>

//Define os pinos que serão utilizados para ligação ao display
LiquidCrystal lcd(10, 9, 8, 7, 6, 3);
 
//Define os pinos para o trigger e echo
#define pino_trigger 4
#define pino_echo 5
#define PinoSensor A0
 
//Inicializa o sensor nos pinos definidos acima
Ultrasonic ultrasonic(pino_trigger, pino_echo);

// Set your AppEUI and AppKey
const char *appEui = "70B3D57ED0013A2B";
const char *appKey = "BB4B6256CCE9EF1E5E65D603B6D79FC7";

#define loraSerial Serial1
#define debugSerial Serial

// Replace REPLACE_ME with TTN_FP_EU868 or TTN_FP_US915
#define freqPlan TTN_FP_US915

uint8_t sf = 8;
//TheThingsNetwork ttn(loraSerial, debugSerial, freqPlan);
TheThingsNetwork ttn(loraSerial, debugSerial, freqPlan, sf);

const int LDR = 1; 
int Luminosidade = 0;

void setup()
{
  //Define o número de colunas e linhas do LCD
  lcd.begin(16, 2);

  //Limpa a tela
  lcd.clear();
  //Posiciona o cursor na coluna 3, linha 0;
  lcd.setCursor(3, 0);
  //Envia o texto entre aspas para o LCD
  lcd.print("The Things");
  lcd.setCursor(3, 1);
  lcd.print(" Network");
  delay(4000);
 
  loraSerial.begin(57600);
  debugSerial.begin(9600);

  // Wait a maximum of 10s for Serial Monitor
  while (!debugSerial && millis() < 10000);

  //debugSerial.println("-- STATUS");
  ttn.showStatus();

  //debugSerial.println("-- JOIN");
  ttn.join(appEui, appKey);

  ttn.onMessage(mensagem);
  
}

void loop()
{
  debugSerial.println("---------- LOOP ----------");

  //EstadoBotao = digitalRead(Botao); /*novo estado do botão*/

  //---------------------------------
  // Sensor ultrassônico

 //Le as informacoes do sensor em cm
  String cmMsec, distancia;
  uint32_t microsec = ultrasonic.timing();
  cmMsec = ultrasonic.convert(microsec, Ultrasonic::CM) * 100;  // 8 100 pra poder mandar no payload
  distancia = ultrasonic.convert(microsec, Ultrasonic::CM);        //centimetros para printar na serial
  
  //Exibe informacoes no serial monitor
  debugSerial.print("Distancia: ");
  debugSerial.print(distancia);
  debugSerial.println("cm");

  // Separar String por vírgula
  String antesVirgula, depoisVirgula;

  for (int i = 0; i < distancia.length(); i++) {
    if (distancia.substring(i, i+1) == ".") {
      antesVirgula = distancia.substring(0, i).toInt();
      depoisVirgula = distancia.substring(i+1).toInt();
      break;
    }
  }
 
  uint32_t aV = antesVirgula.toInt();
  uint32_t dV = depoisVirgula.toInt();

  //-------------------------------------
  // Sensor de temperatura 
  
  int ValorSensor = analogRead(PinoSensor);
  int Temp = (ValorSensor*0.2027)-82;

  debugSerial.print("Temperatura:");
  debugSerial.print(Temp);
  debugSerial.println("ºC");

  // --------------------------------
  // Sensor de luminosidade
  

  Luminosidade = analogRead(LDR);
  debugSerial.print("Intensidade de luz:");
  debugSerial.println(Luminosidade);
  int Lum;

  if (Luminosidade > 200){
    debugSerial.println("Ta claro, apagar a luz!");
    Lum = 0;
  } else {
    debugSerial.println("Ta escuro, acender a luz!");
    Lum = 1;
  }

  debugSerial.print("Lum: ");
  debugSerial.println(Lum);
  
  // --------------------------------
  // Envio do payload
        
  byte payload[4];
  payload[0] = aV;
  payload[1] = dV;
  payload[2] = Temp;
  payload[3] = Lum;
  
  ttn.sendBytes(payload, sizeof(payload));
  
  delay(5000);
}

void mensagem(const uint8_t *payload, size_t size, port_t port)
{
  if (payload[0] != NULL)
  {
    //Limpa a tela
    lcd.clear();
    //Posiciona o cursor na coluna 3, linha 0;
    lcd.setCursor(0, 0);
    //Envia o texto entre aspas para o LCD
    lcd.print("TTN DOWNLINK:");
    lcd.setCursor(0, 1);
    lcd.print(payload[0]);
    delay(2000);
  }

}
