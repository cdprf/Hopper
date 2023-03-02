/******************************************************************
  Created with PROGRAMINO IDE for Arduino - 01.04.2018 00:11:49
  Project     : Hopper Driver
  Author      : Yunus Emre Yüce
  Libraries   : None
  Description : This project for controling 12v Motor, COLLECTIN SIGNAL FROM 12V INDUSTRIAL NPN SENSOR
******************************************************************/
const int Role1 = 6;
const int Role2 = 7;
const int Role3 = 8;
const int Role4 = 9;
const int JetonMekanizmasi = 2; // İkinci Pin Paramakenizması olarak Anılacak bundan sonra
int JetonDurumu = 0;

int GelenKomut = 0;

int SensorDegeri;
unsigned long SonOdulZamani;
unsigned long SonParaZamani;

void setup() { // Setup İçindekiler Cihaz açıldığında yapılacak olan işlemler
  pinMode(JetonMekanizmasi, INPUT); // Para Mekanizması Giriş Tipidedir. Veri Girişi
  pinMode(Role1, OUTPUT);
  pinMode(Role2, OUTPUT);
  pinMode(Role3, OUTPUT);
  pinMode(Role4, OUTPUT);
  Serial.begin(9600); // Bilgisayara Veri Göndermek İçin Seri Port Haberleşmesini Açtık
  SonOdulZamani = millis();
  SonParaZamani = millis();
}


void RoleKapat()
{
    digitalWrite(Role1, LOW);
    digitalWrite(Role2, LOW);
    
    digitalWrite(Role3, LOW);
    digitalWrite(Role4, LOW);
  }
  
void ilerisur()
{
    RoleKapat();
    digitalWrite(Role1, HIGH);
    digitalWrite(Role2, HIGH);
    
}

void gericek()
{
    RoleKapat();
    digitalWrite(Role3, HIGH);
    digitalWrite(Role4, HIGH);
}

void loop() {
  //unsigned long currentMillis = millis();

  JetonDurumu = digitalRead(JetonMekanizmasi); // Mekanizma ne durumda bilgisini okuyalim
  if (JetonDurumu == LOW) // Para Mekanizması normalde sürekli High'dadır.(5v da yani) değer veriyor. Eğer Low'a düşerse(0 volta yani) Para atılmıştır demek
  {
    if (( millis() - SonParaZamani) > 250)
    {
      SonParaZamani = millis();  // Seri Porta 80 Gönder (Ascii 80 = P harfine denk gelir)
      Serial.print('P');
    }
  }

  SensorDegeri = analogRead(A0);
  if (SensorDegeri < 100)
  {
    if (( millis() - SonOdulZamani) > 250)
    {
      Serial.print('O');   // Ödülün O su :)
      SonOdulZamani = millis();
    }
  }

  GelenKomut = Serial.read();
  if (GelenKomut == 86) // Verin V si KOMUTU GELDi, Motoru Sür ve Geri çek
  {
    Serial.print('G');
    ilerisur();
    delay(200);
    gericek();
    delay(200);
    RoleKapat();
  }



}
