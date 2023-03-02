/******************************************************************
  Created with PROGRAMINO IDE for Arduino - 01.04.2017 00:11:49
  Project     : Hopper Driver
  Author      : Yunus Emre Yüce
  Libraries   : None
  Description : This project for controling 12v Motor, COLLECTIN SIGNAL FROM 12V INDUSTRIAL NPN SENSOR
******************************************************************/
#include <SoftwareSerial.h>
const int ledPin =  LED_BUILTIN;
unsigned long LastAliveMessageTime=0;
unsigned long currentMillis=0;
const int ParaMekanizmasi = 2; // İkinci Pin Paramakenizması olarak Anılacak bundan sonra
const int OdulMekanizmasi = 6;
const int AkimFault=12;
const int DisconnectTimeOutinSec = 61000;
int GelenKomut = 0;
int alldreadydriving =0;
unsigned long SonOdulZamani;
unsigned long SonParaZamani;
unsigned long IleriSurmeZamani = 0;
unsigned long IleriSurmeGeriZamani = 0;
unsigned long GeriSurmeZamani = 0;



void setup() { // Setup İçindekiler Cihaz açıldığında yapılacak olan işlemler
  pinMode(ParaMekanizmasi, INPUT); // Para Mekanizması Giriş Tipidedir. Veri Girişi
  pinMode(OdulMekanizmasi, OUTPUT); // Odul Mekanizmasi Çıkış tipindedir.
  pinMode(ledPin, OUTPUT);
  Serial.begin(9600); // Bilgisayara Veri Göndermek İçin Seri Port Haberleşmesini Açtık
  SonOdulZamani = millis();
  SonParaZamani = millis();
}

void ilerisur()
{
  digitalWrite(OdulMekanizmasi + 1, LOW);
  digitalWrite(OdulMekanizmasi, HIGH);
  IleriSurmeZamani = millis();
}


void durdur()
{
  digitalWrite(OdulMekanizmasi, LOW);
  digitalWrite(OdulMekanizmasi + 1, LOW);
  IleriSurmeZamani = 0;
  IleriSurmeGeriZamani = millis();
  alldreadydriving =0;
  }

void Communicate()
{
   GelenKomut = Serial.read();
    if (GelenKomut == 86) // V 
    {
      if (alldreadydriving ==0)
      {
        ilerisur();
        alldreadydriving=1;
      }
    }
   else if (GelenKomut==77) //M 
    {
        LastAliveMessageTime =(unsigned long)millis() + DisconnectTimeOutinSec;
    }
    else if(GelenKomut==82) //R 
    {
      if(isAlive()) Serial.print("Alive"); 
      else Serial.print("Dead :");
      Serial.println(LastAliveMessageTime);
      Serial.print("Current :");
      Serial.println(currentMillis);
     }
     else if((isAlive()) && (GelenKomut==89))// Y
     {
      //Serial.println("OK");
      LastAliveMessageTime=(unsigned long)millis() + DisconnectTimeOutinSec;
     }
     
   }

void LedControl()
{
    if (isAlive())
    { 
       digitalWrite(ledPin, HIGH);
    }
    else
    { 
      digitalWrite(ledPin, LOW);
    }
}

void ReadSensors()
{
    if (digitalRead(ParaMekanizmasi) == LOW) // Para Mekanizması normalde sürekli High'dadır.(5v da yani) değer veriyor. Eğer Low'a düşerse(0 volta yani) Para atılmıştır demek
    {
      if (( millis() - SonParaZamani) > 250)
      {
        SonParaZamani = millis();  // Seri Porta 80 Gönder (Ascii 80 = P harfine denk gelir)
        Serial.print('P');
      }
    }
      
    if (analogRead(A0) < 100)
    {
      if (( millis() - SonOdulZamani) > 250)
      {
        durdur();
        alldreadydriving=0;
        Serial.print('O');   // Ödülün O su :)
        SonOdulZamani = millis();
        
      }
    }
  
  }

boolean isAlive()
{
  return true;
  if ((unsigned long)(currentMillis) > (unsigned long)(LastAliveMessageTime) )
  {
    return false;
  }
  else
  return true;
}

void loop() {
  currentMillis = millis();
            
     // if (isAlive())      {
      if (( currentMillis - IleriSurmeZamani >= 4000) && (IleriSurmeZamani > 0)) { // İleri itmeyi Boşa çıkartmak için
           durdur();
      }
      ReadSensors();

     // }// end of is alive
  Communicate();
  LedControl();
}
