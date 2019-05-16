#include <doxygen.h>

//#include <Nextion.h>
//#include <NexTouch.h>

#include <NexGauge.h>
#include <NexPicture.h>
#include <NexProgressBar.h>

/* Tipke
#include <NexButton.h>
#include <NexDualStateButton.h>
*/

//#include <NexConfig.h>
//#include <NexCrop.h>





int BatteryVoltagePin0 = 0;   //A0 vhod za IL300 opto linear pretvornik za Li-ion
// A0 pin za branje napetosti baterije +B preko sonde/delilnika
int ThermistorPin1 = 1;     //A1 vhod 
int ThermistorPin2 = 2;     //A2 vhod
int ThermistorPin3 = 3;     //A3 vhod
int ThermistorPin4 = 4;     //A4 vhod
int voltBatPin = 5;     //A5 vhod za 12V baterijo

// Avto luč
int Luc = 8; // Pin 8 sprednja in zadnja luč
int StikaloLuc = 2; // Pin 2 za vklop luči spredaj in zadaj via stikalo
int Luc_stat = 0;

int V1, V2, V3, V4, V0;        
float R1 = 6800;
float logR21, logR22, logR23, logR24, R21, R22, R23, R24, T21, T22, T23, T24, T11, T12, T13, T14, Tf, V0Bat;
float c1 = 1.163854283e-03, c2 = 2.489458305e-04, c3 = 1.130393822e-07;
float batVoltage = 0;
int batLow12 = 0; //12V baterija
char dataString[50] = {0};

int VklopBMS = 4; //Pin 4 za vklop BMS

//Vžig in izklop BMS
int On_BMS = 6;  
int Off_BMS = 7;
int Vzig_BMS = 1;

//RGB control PWM
int RGB_G = 3;
int RGB_R = 10;
int RGB_B = 9;

//Fan kontrol
int Fan_bat = 5;
int Fan_stat = 0; //Signal za pie 1 ali 0

//Definirati pine 11, 12 in 13 za SPI komunikacijo za CAN modul

//Zaslon
NexProgressBar j0 = NexProgressBar(0, 4, "j0"); //Graf za prikaz temperature krmilnik
NexProgressBar j1 = NexProgressBar(0, 1, "j1"); //Graf za prikaz temperature baterije
NexProgressBar j2 = NexProgressBar(0, 2, "j2"); //Graf za prikaz temperature BMS
NexProgressBar j3 = NexProgressBar(0, 3, "j3"); //Graf za prikaz temperature motorja
NexProgressBar j4 = NexProgressBar(0, 7, "j4"); //Stanje polnosti 12V baterije
NexProgressBar j5 = NexProgressBar(0, 6, "j5"); //Stanje luči
NexProgressBar j6 = NexProgressBar(0, 8, "j6"); //Stanje ventilatorja za hlajenje baterije

NexGauge z0 = NexGauge(0, 5, "z0"); //Števec za prikaz napetosti baterije

void setup() {
 delay(400);
 Serial.begin(9600);
 pinMode(Luc, OUTPUT);
 pinMode(Fan_bat, OUTPUT);
 pinMode(StikaloLuc, INPUT_PULLUP); // Definirana za vklop sprednje luči
 pinMode(VklopBMS, OUTPUT);
 pinMode(On_BMS, INPUT_PULLUP);
 pinMode(Off_BMS, INPUT_PULLUP);
 digitalWrite(VklopBMS, HIGH);  //Vklop  bms za 3s
 delay(3200);
 digitalWrite(VklopBMS, LOW);  //Izklop tipke --> BMS je sedaj vklopljen
 nexInit();
}

void loop() {
//12v BATTERY setup
batVoltage = ((analogRead(voltBatPin)*5.0)/1023.0)*2.767;
if (batVoltage<=11.8){
  batLow12 = 1;
}
else{
  batLow12 = 0;
}
//temp. 1 - Krmilnik:
V1 = analogRead(ThermistorPin1);
R21 = R1 * (1023.0 / (float)V1 - 1.0);
logR21 = log(R21);
T11 = (1.0 / (c1 + c2*logR21 + c3*logR21*logR21*logR21));
T21 = T11 - 273.15;
//temp. 2 - Baterijska:
V2 = analogRead(ThermistorPin2);
R22 = R1 * (1023.0 / (float)V2 - 1.0);
logR22 = log(R22);
T12 = (1.0 / (c1 + c2*logR22 + c3*logR22*logR22*logR22));
T22 = T12 - 273.15;
//temp. 3 - Motor:
V3 = analogRead(ThermistorPin3);
R23 = R1 * (1023.0 / (float)V3 - 1.0);
logR23 = log(R23);
T13 = (1.0 / (c1 + c2*logR23 + c3*logR23*logR23*logR23));
T23 = T13 - 273.15;
//temp. 4 - BMS:
V4 = analogRead(ThermistorPin4);
R24 = R1 * (1023.0 / (float)V4 - 1.0);
logR24 = log(R24);
T14 = (1.0 / (c1 + c2*logR24 + c3*logR24*logR24*logR24));
T24 = T14 - 273.15;
//napetost +B baterije --> od 40,6V ÷ 58,8V naj bi se izpisalo, ko merimo ...
V0 = analogRead(BatteryVoltagePin0);
V0Bat= V0; //vrednosti so od 0 do cca 45

float V0BatDisplay = (V0Bat-28)*12.857;
//Serial.println(V0BatDisplay);
//preko USB porta (UART) pošiljamo v taki obliki za Raspberry PI:
delay(200);
Serial.print(";TempController:"+String(T21)+";TempBattery:"+String(T22)+";TempMotor:"+String(T23)+";TempBMS:"+String(T24)+";BatVoltage:"+String(V0Bat)+";fan:"+String(Fan_stat)+";BMS_start:"+String(Vzig_BMS)+";"+";AkuVoltage:"+String(batVoltage)+";");

//Obvezno nastavljanje prvega števca dvakrat

j0.setValue(T21); //KRMILNIK
j0.setValue(T21); //KRMILNIK
j1.setValue(T22); //BATERIJE
j3.setValue(T23); //MOTOR
j2.setValue(T24); //BMS
j4.setValue(batLow12*100);  //indikator 12V BAT
j5.setValue(Luc_stat*100);  //indikator luci
j6.setValue(Fan_stat*100);  //indikator ventilator
z0.setValue(V0BatDisplay); //NAPETOST

//serial prenos povzroča resetiranje Arduina na točno 1 min!

/* RGB: 
 42V    <  Rdeča   < 45.35V, --> RGB_R=255, RGB_G=0,   RGB_B=0
 45.36V <  Oranžna < 48.71V, --> RGB_R=255, RGB_G=127, RGB_B=0
 48.72V <  Rumena  < 52.07V, --> RGB_R=255, RGB_G=255, RGB_B=0
 52.08V <  Zelena  < 55.43V, --> RGB_R=0,   RGB_G=255, RGB_B=0
 55.44V <  Modra   < 58.8V   --> RGB_R=0,   RGB_G=127, RGB_B=255
*/

//V0 = 58;

if (V0Bat <= 29) {                       //Rdeča
  analogWrite(RGB_R, 255);
  analogWrite(RGB_G, 0);
  analogWrite(RGB_B, 0);
  }
else if (V0Bat > 29 and V0Bat <= 32) {   //Oranžna
  analogWrite(RGB_R, 255);
  analogWrite(RGB_G, 40);
  analogWrite(RGB_B, 0);
  }
else if (V0Bat >  32 and V0Bat <= 36) {  //Rumena
  analogWrite(RGB_R, 255);
  analogWrite(RGB_G, 175);
  analogWrite(RGB_B, 0);
  }
else if (V0Bat >  36 and V0Bat <= 39) {  //Zelena
  analogWrite(RGB_R, 0);
  analogWrite(RGB_G, 255);
  analogWrite(RGB_B, 0);
  }
else {                                    //Modra
  analogWrite(RGB_R, 0);
  analogWrite(RGB_G, 127);
  analogWrite(RGB_B, 255);
  }
  
// ----------------------------------------------Luči za avto--------------------------------------------

if (digitalRead(StikaloLuc) == LOW){ 
  digitalWrite(Luc, HIGH);
  Luc_stat = 1;
  
  }
else if (digitalRead(StikaloLuc) == HIGH){
  digitalWrite(Luc, LOW);
  Luc_stat = 0;
}

//---------------------------------------------Vklop BMS--------------------------------------------------

if(digitalRead(On_BMS) == LOW and Vzig_BMS == 0){
  digitalWrite(VklopBMS, HIGH);  //Vklop  bms za 3s
  delay(3000);
  digitalWrite(VklopBMS, LOW);  //Izklop tipke --> BMS je sedaj vklopljen
  Vzig_BMS = 1;
  }
else if(digitalRead(Off_BMS) == LOW and Vzig_BMS == 1){
  Vzig_BMS = 0;
  digitalWrite(VklopBMS, HIGH);  //Vklop  bms za 5s
  delay(6500);
  digitalWrite(VklopBMS, LOW);  //Izklop tipke --> BMS je sedaj izklopljen
  }
  
//-------------------------------------------------FAN---------------------------------------------------

if(T22 > 40){
  digitalWrite(Fan_bat, HIGH);
  Fan_stat = 1;
}
else if(T22 < 39.99){
  digitalWrite(Fan_bat, LOW);
  Fan_stat = 0;
}





}
