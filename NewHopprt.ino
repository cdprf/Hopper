/******************************************************************
  Created with PROGRAMINO IDE for Arduino - 01.04.2017 00:11:49
  Project     : Hopper Driver
  Author      : Yunus Emre Yüce
  Libraries   : None
  Description : This project for variation of controling 12v Motor, COLLECTIN SIGNAL FROM 12V INDUSTRIAL NPN SENSOR
******************************************************************/
const int ParaMekanizmasi = 2; // İkinci Pin Paramakenizması olarak Anılacak bundan sonra
const int OdulMekanizmasi = 6;
int GelenKomut = 0;
int MekanizmaDurumu = 0;
int SensorDegeri;
unsigned long SonOdulZamani;
unsigned long SonParaZamani;
unsigned long IleriSurmeZamani = 0;
unsigned long IleriSurmeGeriZamani = 0;
unsigned long GeriSurmeZamani = 0;
void setup() { // Setup İçindekiler Cihaz açıldığında yapılacak olan işlemler
  pinMode(ParaMekanizmasi, INPUT); // Para Mekanizması Giriş Tipidedir. Veri Girişi
  pinMode(OdulMekanizmasi, OUTPUT); // Odul Mekanizmasi Çıkış tipindedir.
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
/*
void gericek()
{
  digitalWrite(OdulMekanizmasi, LOW);
  digitalWrite(OdulMekanizmasi + 1, HIGH);
  GeriSurmeZamani = millis();
}
*/
void loop() {
  unsigned long currentMillis = millis();

  if (( currentMillis - IleriSurmeZamani >= 1000) && (IleriSurmeZamani > 0)) { // İleri itmeyi Boşa çıkartmak için

    digitalWrite(OdulMekanizmasi, LOW);
    digitalWrite(OdulMekanizmasi + 1, LOW);
    IleriSurmeZamani = 0;
    IleriSurmeGeriZamani = millis();
  }

  if ((currentMillis - IleriSurmeGeriZamani >= 50) && (IleriSurmeGeriZamani > 0)) // Kolu geri çekmek için
  {
   //gericek();
    digitalWrite(OdulMekanizmasi, LOW);
    digitalWrite(OdulMekanizmasi + 1, HIGH);
    IleriSurmeGeriZamani = 0;

  }
  if ((currentMillis - GeriSurmeZamani >= 200) && GeriSurmeZamani > 0 ) // Kolu geri çekmeyi boşa çıkartmak için
  {
    digitalWrite(OdulMekanizmasi + 1, LOW);
    GeriSurmeZamani = 0;
  }

  MekanizmaDurumu = digitalRead(ParaMekanizmasi); // Mekanizma ne durumda bilgisini okuyalim

  if (MekanizmaDurumu == LOW) // Para Mekanizması normalde sürekli High'dadır.(5v da yani) değer veriyor. Eğer Low'a düşerse(0 volta yani) Para atılmıştır demek
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
    ilerisur();
  }



}
