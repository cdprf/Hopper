/******************************************************************
 Created with PROGRAMINO IDE for Arduino - 01.04.2017 00:11:49
 Project     : Hopper Driver
 Libraries   : None
 Author      : Yunus Emre YÜCE
 Description : This project for controling 12v Motor, 
******************************************************************/

const int ParaMekanizmasi = 2; // İkinci Pin Paramakenizması olarak Anılacak bundan sonra
byte MotorDurumu =0;  //1 ileri 2 Geri 3 İkinci İleri 4 Geçici Stop
long MotorDurumZamani =0;
const byte OdulMekanizmasibir = 6;
const byte OdulMekanizmasiiki = 7;
const byte OdulSensoru = 12;

byte MotorIleriSuruluyor = 0;
byte MotorGeriSuruluyor =0;
byte MotorIleriSurerkenAlgilananOdul =0;
int GelenKomut = 0;
int MekanizmaDurumu = 0;
int SensorDegeri;

unsigned long SonOdulZamani;
unsigned long SonParaZamani;


unsigned long last_durum =0;

void setup() { // Setup İçindekiler Cihaz açıldığında yapılacak olan işlemler
  pinMode(ParaMekanizmasi, INPUT); // Para Mekanizması Giriş Tipidedir. Veri Girişi
  pinMode(OdulMekanizmasibir, OUTPUT); // Odul Mekanizmasi Çıkış tipindedir.
  pinMode(OdulMekanizmasiiki, OUTPUT);
  pinMode(OdulSensoru,INPUT);
  Serial.begin(9600); // Bilgisayara Veri Göndermek İçin Seri Port Haberleşmesini Açtık
  SonOdulZamani = millis();
  SonParaZamani = millis();
  MotorDurumu =0;
  MotorIleriSurerkenAlgilananOdul =0;
}

void loop() {
  SensorOku();
  MekanizmaOku();
  SeriPortOku();
  MotorHareket();
}

void ilerisur()
{
  digitalWrite(OdulMekanizmasiiki, LOW);
  digitalWrite(OdulMekanizmasibir, HIGH);
}
  
void GeriSur()
{
  digitalWrite(OdulMekanizmasiiki, HIGH);
  digitalWrite(OdulMekanizmasibir, LOW);
}

void MotorDurdur()
{
  digitalWrite(OdulMekanizmasibir, LOW);
  digitalWrite(OdulMekanizmasiiki, LOW);
}

void MotorHareket()
{
  unsigned long SimdikiMiliSaniye = millis();
  // İleri Sürme Bitişi  
  long fark = SimdikiMiliSaniye - MotorDurumZamani;
    
 if ((MotorDurumu ==1) && ( fark >= 1000)) // Motor ileri Sürme bitti.
  {
    MotorDurdur();
    MotorDurumu = 0;
    if (MotorIleriSurerkenAlgilananOdul==0) // Para vermediyse ters çevir.
    {
      MotorDurumu = 2;
      MotorDurumZamani = millis();
      delay(100);
      GeriSur();
      return;
    }
  }
  
  // Geri Sürme Bitti
  if ((MotorDurumu ==2) && (fark >= 1000))
  {
    MotorDurdur();
    MotorDurumu=0;
    MotorDurumu=3;
    MotorDurumZamani = millis();  
    delay(100);
    ilerisur();
    return;
  }
  
  // Son İleri Sürme
  if ((MotorDurumu==3) && (fark >= 1000))
  {
    MotorDurumu =0;
    MotorDurumZamani=millis();  
    MotorDurdur();
    delay(100);
    return;
  }
  
}


void SensorOku()
{
  unsigned long SimdikiMiliSaniye = millis();// Devre Calismaya basladigindan Buyana gecen Milisaniye Cinsinden Süre
  SensorDegeri = digitalRead(OdulSensoru);//Ödül Mekanizmasına Bağlı Sensörü Oku
  if (SensorDegeri == LOW)   // Eğer Sensörde Sinyal voltajı(5V) varsa 
    {
      // Eğer Son Ödülü 250 ms den daha uzun bir zaman önce verdiyse (Sensörden Gelen Sinyali biden fazla kez okumamak için)
      if (( SimdikiMiliSaniye - SonOdulZamani) > 250)
      {
        //Ödül Mekanizmasından Gelen Para Verildi Sinyalini algıladık. Motor Daha fazla Dönmesin diyerek
        MotorIleriSurerkenAlgilananOdul=1;
        MotorDurumu =0;
        MotorDurdur();
        // Bilgisayara Ödül Algılandı Mesajı Gönderiyoruz.
        Serial.print('O');   // Ödülün O su :)
        // Sensörden Gelen Sinyali Algıladık, Önümüzdeki 250 Ms boyunca tekrar algılamamak için Son Sinyal zamanını Kaydediyoruz.
        SonOdulZamani = SimdikiMiliSaniye;
      }
    }
}

void MekanizmaOku()
{
  MekanizmaDurumu = digitalRead(ParaMekanizmasi); // Mekanizma ne durumda bilgisini okuyalim
  if (MekanizmaDurumu == LOW) // Para Mekanizması normalde sürekli High'dadır.(5v da yani) değer veriyor. Eğer Low'a düşerse(0 volta yani) Para atılmıştır demek
  {
    if (( millis() - SonParaZamani) > 250)
    {
      SonParaZamani=millis();  // Seri Porta 80 Gönder (Ascii 80 = P harfine denk gelir)
      Serial.print('P');
    }
  }
}

void SeriPortOku()
{
  GelenKomut = Serial.read();
  
  if (GelenKomut == 86) // Verin V si KOMUTU GELDi, Motoru Sür ve Geri çek
  {
      MotorIleriSurerkenAlgilananOdul=0;
      MotorDurumu =1;
      MotorDurumZamani = millis();   
      ilerisur();
  }
  
  if (GelenKomut == 87) // Geri Werin W si KOMUTU GELDi, Motoru Sür ve Geri çek
  {
      MotorDurumu =2;
      MotorDurumZamani = millis();
      GeriSur();
  }

  if (GelenKomut == 88) // X Stop Motoru Sür ve Geri çek
  {
    MotorDurumu =0;
    MotorDurumZamani = millis();
    MotorDurdur();
  } 
  
}
