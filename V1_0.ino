#include "DHT.h"

#define dhtPin 2
#define dhtType DHT22
#define pulsoPluv 3
#define anemometro 1

unsigned int counter = 0;
unsigned int pluv = 0;
unsigned int RpmAnem = 0;
boolean flagSend = false;
boolean ultimoestado = false;

DHT dht(dhtPin, dhtType);

void setup() {
   TCCR1A = 0x00;                                // 
   TCCR1B = B00000110;                           // Configuração do TIMER 1 para contador com clock externo
   TCNT1 = 0;
   TCCR2A = 0x00;                                // Configura interrupção do timer 2 para cada 8ms, para calculo da RPM x 100
   TCCR2B = B00000111;                           // Indica a função que irá ser executada quando interromper
   TIMSK2 |= (1 << OCIE2A);
   TCNT2 = 131;
   sei();
   pinMode(pulsoPluv, INPUT);
   pinMode(anemometro, INPUT);
   Serial.begin(9600);
}

ISR(TIMER2_COMPA_vect){
  counter++;
  if(counter == 125){
    flagSend = true;
    RpmAnem = TCNT1;
    TCNT1 = 0;
    counter = 0;
  }
  TCNT2 = 131;
}

void loop() {
  float h = dht.readHumidity();
  float t = dht.readTemperature();

  if(isnan(h) || isnan(t)){
    Serial.println("Erro DHT");
    return;
  }

  if((digitalRead(pulsoPluv)) && (!ultimoestado)){
    ultimoestado = true;
    pluv++;
  }

  if(!digitalRead(pulsoPluv)){
    ultimoestado = false;
  }

  if(flagSend){
  Serial.print("Umidade: ");
  Serial.println(h);
  Serial.print("Temperatura: ");
  Serial.println(t);
  Serial.print("Indice Pluviometrico: ");
  Serial.print(pluv*0.25);
  Serial.println("mm");
  Serial.print("Anemometro: ");
  Serial.println(RpmAnem);
  RpmAnem = 0;
  flagSend = false;
  }
}
