/******************************************************************
  Created with PROGRAMINO IDE for Arduino - 01.04.2016 00:11:49
  Project     : Hopper Driver
  Author      : Yunus Emre Yüce
  Libraries   : None
  Description : This project for new controling 12v Motor, COLLECTIN SIGNAL FROM 12V INDUSTRIAL NPN SENSOR
******************************************************************/
//#include <HardwareSerial.h>
//#include <MessageReader.h>
//#include <AltSoftSerial.h>
#define SERIAL_TX_BUFFER_SIZE 64
#define SERIAL_BUFFER_SIZE 64
HardwareSerial* altSerial;
// Transmit pin 9
// Receive Pn 8
// PWM  10
//AltSoftSerial altSerial;
//HardwareSerial altSerial = HardwareSerial(&&);

long xxxx[0];
char simplePool[] = {2, 0, 1, 254, 255};
char serialNoGetir[] ={2,0,1,242,11};
char simpleStatus[] = {2, 0, 1, 248, 251};
char checkstatus[] = {3, 0, 1, 163, 59};
char cCommand[] = {3, 0, 1, 164,165, 178};
char cCommand2[] = {3, 0, 1, 166,56};
char count = 0 ;
byte buffer[50];
int MYADDRESS = 1;
boolean isMessage = false;
int state = 0;

//Channel value, each channel got its number of credits to add
//Change the values in this table to match between channels and credits
//Here, channel 5 means 1 credit, channel 7 means 2 credits and channel 9 is 5 credits
int channelValue[] = {0,0,0,0,0,1,0,2,0,5,0,0,0,0,0,0,0};

byte checksum (char* pack )
{
  byte pack_size = 5;
  byte sum = 0;
   for (int i = 0; i < pack_size-1; i++) {
       sum += pack[i];
   }
   return sum;
}


void disableRx(){
  UCSR0B &= ~(1<<RXEN0); // use UCSR0B and RXEN0 to use with Arduino
}

void enableRx(){
  UCSR0B |= (1<<RXEN0); // use UCSR0B and RXEN0 to use with Arduino
}

void sendSimplePoll(){
  //<cctalk src=1 dst=2 length=0 header=254>
  byte buff[] = {0x02,0x00,0x01,0xfe,0xff};
  altSerial->write(buff,5);
  //altSerial->flush();
}


void sendInhibits(){//Sends a change inhibit status request to enable all channels
  //<cctalk src=1 dst=2 length=2 header=231 data=ffff>
  byte buff1[] = {0x02,0x02,0x01,0xe7,0xff,0xff,0x16};
  altSerial->write(buff1,7);
  //altSerial->flush();
  delay(500);
  byte buff2[] = {0x02,0x01,0x01,0xe4,0x01,0x17};
  altSerial->write(buff2,6);
 // altSerial->flush();
}

void sendManufID(){
  //Requests manufacturer ID
  //<cctalk src=1 dst=2 length=0 header=246>
  byte buff[] = {0x02,0x00,0x01,0xf6,0x07};
  altSerial->write(buff,5);
  //altSerial->flush();
}


void sendProdCode(){
  //Requests product code
  //<cctalk src=1 dst=2 length=0 header=244>
  byte buff[] = {0x02,0x00,0x01,0xf4,0x09};
  altSerial->write(buff,5);
  //altSerial->flush();
}

void sendCredits(){
  //Requests credit status
  //<cctalk src=1 dst=2 length=0 header=229>
  byte buff[] = {0x02,0x00,0x01,0xe5,0x18};
  altSerial->write(buff,5);
}



void SendCommand(byte* komut)
{
  char c;
  if (count < 5) {
  altSerial->write(komut[count]);
    count++;
  }
}


void printHex(int num, int precision) {
     char tmp[16];
     char format[128];
     sprintf(format, "0x%%.%dX", precision);
     sprintf(tmp, format, num);
     Serial.println(tmp);
}


void readPacket() {
  bool isError = false;
  while(altSerial->available()>=2){
    Serial.print("Gelen packet : ");  Serial.println(altSerial->available());
    int dest = altSerial->read();
    int length = altSerial->read();
    Serial.print("Dest : ");  Serial.println(dest);
    Serial.print("len : ");  Serial.println(length);
    Serial.print("Got packet, waiting for ");
    Serial.print(length+3);
    Serial.println(" bytes of data");
    
    //Wait for full message bytes, or end receive loop
    while (altSerial->available() < length+3){
      delay(50);
      if (altSerial->available() < length+3) {
        isError=true;
        break;
      }
    }
    if(isError) {
      Serial.println("[*]Impossible to fetch packet data, flushing");
      while (altSerial->available()) {
        altSerial->read();
      }
      break;
    }
    
    
    buffer[0] = dest;
    buffer[1] = length;
    
    for (int i=2;i<length+5;i++) {
      buffer[i] = altSerial->read();
    }
    
    int sum = 0;
    for (int i=0;i<length+4;i++){
      sum += (int)buffer[i];
    }

    if (! (buffer[length+4] == 256-(sum%256))) {
      Serial.println("Packet is invalid, flushing");
      delay(50);
      for (int i=0;i<20;i++){
        Serial.print(buffer[i], HEX);
        Serial.print(" ");
      }
      Serial.println(" ");
      while (altSerial->available()){
        altSerial->read();
      }
    }
    
    if (dest == MYADDRESS) {
      isMessage = true;
      break;
    }
  }
}


void setup() {
Serial.begin(9600);
altSerial->begin(9600);
  disableRx();
  sendSimplePoll();
  delay(500);
  sendInhibits();
  delay(500);
  sendManufID();
  delay(500);
  sendProdCode();
  delay(500);
  enableRx();

  while(altSerial->available()){
     Serial.println(altSerial->read());
  }

  
}

void loop() {
char c;
int incomingByte = 0;  
 if(Serial.available()){
    sendProdCode();
  }

readPacket();


 if (isMessage) {
    if (state<buffer[4]) {
      int channel = int(buffer[5]);
      if (channel <= 16) {
        Serial.println(channel, DEC);
        Serial.print("Adding ");
        Serial.print(channelValue[channel]);
        Serial.println(" credits.");
        //sendCreditKey(channelValue[channel]);
      }
      if (state == 255) {
        state = 1;
      }else{
        state++;
      }
    }
    isMessage = false;
  }

  
// sendSimplePoll();
// sendInhibits();
// sendManufID();
//
//  incomingByte = Serial.read();
//  if (incomingByte > 0)
//  {
//               Serial.println(incomingByte, DEC); 
//               if (incomingByte == 86){ SendCommand(simplePool);}
//               if (incomingByte == 49){ SendCommand(simpleStatus); }
//               if (incomingByte == 50){ SendCommand(serialNoGetir); }
//               if (incomingByte == 51){ SendCommand(checkstatus); }
//               if (incomingByte == 52){ SendCommand(cCommand); }
//               if (incomingByte == 53){ SendCommand(cCommand2); }
//               if (incomingByte == 54){ SendCommand(simplePool); }
//               if (incomingByte == 55){ SendCommand(simplePool); }
//               if (incomingByte == 56){ SendCommand(simplePool); }
//               if (incomingByte == 57){ SendCommand(simplePool); }
//               if (incomingByte == 58){ SendCommand(simplePool); }
//               
//  }
//
//incomingByte = altSerial->read();
//  if (incomingByte > 0)
//  {
//   Serial.println(incomingByte, DEC);
//  }
           
}
