#include <OneWireSTM.h>
#include <DallasTemperature.h>
#include <Arduino.h>
#include <U8g2lib.h>
#include <SPI.h>
#include <Wire.h>

U8G2_SSD1306_128X32_UNIVISION_F_HW_I2C u8g2(U8G2_R0); 

#define ONE_WIRE_BUS PA6                                            // inicijalizacija pina A6 za senzor temperature ds18b20
#define fan PB8                                                     // ventilator odvlažavanje i hlađenje
#define Hfan PA8                                                    // ventilator za upuh toplog zraka
#define pump PA9                                                    // pumpa za navodnjavanje 
#define led PB9                                                     // LED
#define heater PA10                                                 // grijač
 
OneWire oneWire(ONE_WIRE_BUS);
 
DallasTemperature sensors(&oneWire);                               //proslijeđivanje reference oneWire library u DallasTemperature library

//varijable za dohvaćanje podataka sa senzora
unsigned int moisture, light;
int xm, x;
float temp;
int lightTimer = 0;
int lightChecker = 0;


 void setup(void) {
  sensors.begin();
  u8g2.begin();
  pinMode(fan, OUTPUT);
  digitalWrite(fan, HIGH);
  pinMode(heater, OUTPUT);
  digitalWrite(heater, HIGH);
  pinMode(Hfan, OUTPUT);
  digitalWrite(Hfan, HIGH);
  pinMode(pump, OUTPUT);
  digitalWrite(pump, HIGH);
  pinMode(led, OUTPUT);
  digitalWrite(led, HIGH);
  digitalWrite(heater, LOW);
}



//Glavni program
  void loop(void) {
  vrijeme();
  checkMoisture();
  checkTemperature();
  checkLightning();
  delay(200);
}

//Funkcija za ispis vlage na ekran oled 128x32

void checkMoisture(){
  
  moisture = analogRead(PA7);
  xm = map(moisture,1200,2800,100,0);

  if(xm>9){
   u8g2.clearBuffer();                                         // oslobađanje interne memorije
   //u8g2.setFont(u8g2_font_profont15_tf);                     //u8g2.setFont(u8g2_font_logisoso28_tr);  // choose a suitable font at https://github.com/olikraus/u8g2/wiki/fntlistall
   u8g2.setFont(u8g2_font_helvR12_tf);
   u8g2.drawStr(28,24,"Vlaga: ");                              // pisanje u internu memoriju
   u8g2.setCursor(77,24);
   u8g2.print(xm);
   u8g2.drawStr(98,24,"%");
   u8g2.sendBuffer();                                          // slanje sadržaja interne memorije na display
   delay(2000);
  }
  else{
   u8g2.clearBuffer();                                         // oslobađanje interne memorije
   u8g2.setFont(u8g2_font_helvR12_tf);
   u8g2.drawStr(33,24,"Vlaga: ");                              // pisanje u internu memoriju
   u8g2.setCursor(82,24);
   u8g2.print(xm);
   u8g2.drawStr(93,24,"%");
   u8g2.sendBuffer();                                          // slanje sadržaja interne memorije na display
   delay(2000);
  }
  
   if(xm<40){
   u8g2.clearBuffer();                                         // oslobađanje interne memorije
   u8g2.setFont(u8g2_font_helvR12_tf);
   u8g2.drawStr(45,12,"Palim");                                // pisanje u internu memoriju
   u8g2.drawStr(15,32,"navodnjavanje");
   u8g2.sendBuffer();
   delay(1000);
    
    digitalWrite(pump, LOW);
    delay(2500);
    digitalWrite(pump, HIGH);
  }
  else{
   digitalWrite(pump, HIGH);
  }

   delay(500);
}


//Funkcija za ispis temperature na ekran oled 128x32

void checkTemperature(){

  sensors.requestTemperatures();                              // slanje zahtjeva za dohvaćanje temperature
  temp = sensors.getTempCByIndex(0);
   
   u8g2.clearBuffer();                                        // oslobađanje interne memorije              
   u8g2.setFont(u8g2_font_helvR12_tf);
   u8g2.drawStr(18,12,"Temperatura:");                        // pisanje u internu memoriju
   u8g2.setCursor(42,32);
   u8g2.print(temp);
   u8g2.drawStr(83,32,"\xb0");
   u8g2.drawStr(89,32,"C");
   u8g2.sendBuffer();                                         // slanje sadržaja interne memorije na display
   delay(2000);

   unsigned long mls = millis();
   int ss = mls / 1000;
   int sc = ss % 60;
   int lastTimeOn;
   int firstTime = 0;
   int lastTimeOff;
   
  if(temp<24){
      digitalWrite(heater, HIGH);
      digitalWrite(Hfan, LOW);
      digitalWrite(fan, HIGH);
  }
  if(temp>24 && temp<32){
      digitalWrite(heater, LOW);
      digitalWrite(Hfan, HIGH);
      digitalWrite(fan, HIGH);
  }

  if(temp > 32){
      digitalWrite(heater, LOW);
      digitalWrite(fan, LOW);
      digitalWrite(Hfan, HIGH);
  }
  delay(1000);
}

//Funkcija za ispis osvjetljenja na ekran oled 128x32

void checkLightning(){
  
  light = analogRead(PB0);
  x = map(light,400,4096,0,100); 

  if(x>9){                                                    // Prilagođeni ispis na display ukoliko je vrijednost jednoznamenkasta
   u8g2.clearBuffer();                                        // oslobađanje interne memorije
   //u8g2.setFont(u8g2_font_profont15_tf);                    //u8g2.setFont(u8g2_font_logisoso28_tr);  // choose a suitable font at https://github.com/olikraus/u8g2/wiki/fntlistall
   u8g2.setFont(u8g2_font_helvR12_tf);
   u8g2.drawStr(20,24,"Svjetlost: ");                         // pisanje u internu memoriju
   u8g2.setCursor(89,24);
   u8g2.print(x);
   u8g2.drawStr(110,24,"%");
   u8g2.sendBuffer();                                         // slanje sadržaja interne memorije na display
  }
  else{                                                       // Prilagođeni ispis na display ukoliko je vrijednost dvoznamenkasta
   u8g2.clearBuffer();         
   u8g2.setFont(u8g2_font_helvR12_tf);
   u8g2.drawStr(20,24,"Svjetlost: ");
   u8g2.setCursor(89,24);
   u8g2.print(x);
   u8g2.drawStr(101,24,"%");
   u8g2.sendBuffer();
  }

  if(x<50){
    digitalWrite(led, HIGH);
  }
  else
  digitalWrite(led,LOW);
  
}




//funkcija za ispis vremena proteklog od početka rada

void vrijeme(){
   unsigned long ms = millis();
   int s = ms / 1000;
   int sec = s % 60;
   int h = s / 60;
   int minuta = h % 60;
   int sat = h / 60;
   u8g2.clearBuffer();                                         // oslobađanje interne memorije
   u8g2.setFont(u8g2_font_helvR12_tf);                         // pisanje u internu memoriju
   u8g2.setCursor(28,24);
   u8g2.print(sat);
   u8g2.drawStr(49,24,":");
   u8g2.setCursor(60,24);
   u8g2.print(minuta);
   u8g2.drawStr(80,24,":");
   u8g2.setCursor(90,24);
   u8g2.print(sec);
   u8g2.sendBuffer();                                          // slanje sadržaja interne memorije na display
   delay(2000);
}
