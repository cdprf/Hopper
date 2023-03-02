/******************************************************************
  Created with PROGRAMINO IDE for Arduino - 01.04.2017 00:11:49
  Project     : Hopper Driver
  Libraries   : None
  Description : This project for controling 12v Motor, COLLECTIN SIGNAL FROM 12V INDUSTRIAL NPN SENSOR
******************************************************************/

const int ParaAtmaMekanizmasiSensorPini = 2; // D2 Pini Bundan sonra ParaAtmaMekanizmasi olarak anılacak
int ParaAtmaMekanizmaDurumu = 0;             // ParaAtmaMekanizmaDurumu Para Atma Mekanizmasından Gelen Sinyal Degerini Tutacak Olan Değişken Tanımlandı. Açılışta Değeri 0 olsun Denildi.

const byte OdulMekanizmasibir = 6;
const byte OdulMekanizmasiiki = 7;
const byte OdulMekanizmaHizi= 255;  //(25% = 64; 50% = 127; 75% = 191; 100% = 255)
const byte OdulMekanizmaHizKontrolPini = 13;
const byte OdulSensorPini =12;
int OdulSensorDegeri;

byte MotorIleriSuruluyor = 0;
int GelenKomut = 0;

unsigned long SonOdulZamani;
unsigned long SonParaZamani;
unsigned long IleriSurmeZamani = 0;
unsigned long Sayac =0;
unsigned long SimdikiMiliSaniye =0;

void setup() { // Setup İçindekiler Cihaz açıldığında yapılacak olan işlemler
  pinMode(ParaAtmaMekanizmasiSensorPini, INPUT); // Para Mekanizması Giriş Tipidedir. Veri Girişi
  pinMode(OdulMekanizmasibir, OUTPUT); // Odul Mekanizmasi Çıkış tipindedir.
  pinMode(OdulMekanizmasiiki, OUTPUT);
  pinMode(OdulMekanizmaHizKontrolPini,OUTPUT);
  pinMode(OdulSensorPini,INPUT);
  
  Serial.begin(9600); // Bilgisayara Veri Göndermek İçin Seri Port Haberleşmesini Açtık
  SonOdulZamani = millis();
  SonParaZamani = millis();
  MotorDurdur();
}

void loop() {
  // Devre Calismaya basladigindan Buyana gecen Milisaniye Cinsinden Süre
  SimdikiMiliSaniye = millis();
   
  if (( SimdikiMiliSaniye - IleriSurmeZamani >= 2000) && (IleriSurmeZamani > 0))
  { // Dört Saniye İleri Sürdüysek, Artık Motoru İleri Sürmeyi Bırakalım.
   MotorDurdur();
  }
  
  SensorOku();
  MekanizmaOku();
  SeriPortOku();
}

void ilerisur()
{
  analogWrite(OdulMekanizmaHizKontrolPini, 255);
  digitalWrite(OdulMekanizmasibir, HIGH);
  digitalWrite(OdulMekanizmasiiki, LOW);
  IleriSurmeZamani = millis();
  Serial.println(OdulMekanizmaHizKontrolPini);
  Serial.println(OdulMekanizmaHizi);
  Serial.println(OdulMekanizmasibir);
  Serial.println(OdulMekanizmasiiki);
  Serial.println("ileri sur");
}


void MotorDurdur()
{
  digitalWrite(OdulMekanizmasibir, LOW);
  digitalWrite(OdulMekanizmasiiki, LOW);
  //Serial.println("durdur.");
  IleriSurmeZamani = 0;
  MotorIleriSuruluyor = 0;
}

void SensorOku()
{
  //if ((currentMillis - IleriSurmeGeriZamani >= 50) && (IleriSurmeGeriZamani > 0)) // Kolu geri çekmek için
  //{
  //  IleriSurmeGeriZamani = 0;
  //}

  //Ödül Mekanizmasına Bağlı Sensörü Oku
  OdulSensorDegeri = digitalRead(OdulSensorPini);
  // Serial.println(OdulSensorDegeri); // TEST AMAÇLIYDI
  // Eğer Sensörde Sinyal voltajı(5V) varsa Sensör Normalde HIGH , Algılama Durumunda ise LOW oluyor
  if (OdulSensorDegeri == LOW)
  {
    // Eğer Son Ödülü 250 ms den daha uzun bir zaman önce verdiyse (Sensörden Gelen Sinyali biden fazla kez okumamak için)
    if (( SimdikiMiliSaniye - SonOdulZamani) > 250)
    {
      //Ödül Mekanizmasından Gelen Para Verildi Sinyalini algıladık. Motor Daha fazla Dönmesin diyerek
      MotorDurdur();
      // Bilgisayara Ödül Algılandı Mesajı Gönderiyoruz.
      Serial.print('O');   // Ödülün O su :)
      // Serial.print(Sayac); Serial.println('O'); // TEST AMAÇLIYDI
      Sayac++;
      // Sensörden Gelen Sinyali Algıladık, Önümüzdeki 250 Ms boyunca tekrar algılamamak için Son Sinyal zamanını Kaydediyoruz.
      SonOdulZamani = SimdikiMiliSaniye;
    }
  }
}

void MekanizmaOku()
{
  ParaAtmaMekanizmaDurumu = digitalRead(ParaAtmaMekanizmasiSensorPini); // Mekanizma ne durumda bilgisini okuyalim
  if (ParaAtmaMekanizmaDurumu == LOW) // Para Mekanizması normalde sürekli High'dadır.(5v da yani) değer veriyor. Eğer Low'a düşerse(0 volta yani) Para atılmıştır demek
  {
    if (( millis() - SonParaZamani) > 250)
    {
      SonParaZamani = millis();  // Seri Porta 80 Gönder (Ascii 80 = P harfine denk gelir)
      Serial.print('P');
    }
  }
}

void SeriPortOku()
{
  GelenKomut = Serial.read();
  if (GelenKomut == 86) // Verin V si KOMUTU GELDi, Motoru Sür ve Geri çek
  {
    Serial.println("Ver Komutu geldi");
    if (MotorIleriSuruluyor == 0)
    {
      ilerisur();
      MotorIleriSuruluyor = 1;
    }
  }
}




