
#include <RCSwitch.h>
#define RELE 2 
RCSwitch mySwitch = RCSwitch();
#include <SoftwareSerial.h>
#include <DFPlayer_Mini_Mp3.h>
 int saundGryffindor = 1;
 int saundSlytherin = 8;
 int saundStart = 15;
 int saundFinish = 17; 

void setup() {
  Serial.begin(9600);
  pinMode(RELE,OUTPUT);
  digitalWrite(RELE,LOW);
  mySwitch.enableReceive(1);  // Receiver on interrupt 1 => that is pin #3
    mp3_set_serial (Serial);
     delay (100);   
    mp3_set_volume (30);
    delay (100);
}

void loop() { 
  if (mySwitch.available()) {
    int value = mySwitch.getReceivedValue();
     if (value == 0){
      Serial.print("Unknown encoding"); 
     }
     else{
    Serial.print("Received ");
    Serial.print( mySwitch.getReceivedValue() );
    Serial.print(" / ");
    Serial.print( mySwitch.getReceivedBitlength() );
    Serial.print("bit ");
    Serial.print("Protocol: ");
    Serial.println( mySwitch.getReceivedProtocol() );
     }
     
     if(mySwitch.getReceivedValue()==753828 )
     {
      mp3_play (saundGryffindor);
     delay(100);
     switch(saundGryffindor){
     case 1:
   digitalWrite(RELE,HIGH);
    delay(400);
    digitalWrite(RELE,LOW); 
     delay(600);
     digitalWrite(RELE,HIGH);
    delay(500);
    digitalWrite(RELE,LOW);
    delay(500);
    digitalWrite(RELE,HIGH);
    delay(800);
    digitalWrite(RELE,LOW); 
     delay(400);
     digitalWrite(RELE,HIGH);
    delay(700);
    digitalWrite(RELE,LOW);
    delay(400);
    digitalWrite(RELE,HIGH);
    delay(900);
    digitalWrite(RELE,LOW); 
     delay(800);
     digitalWrite(RELE,HIGH);
    delay(900);
    digitalWrite(RELE,LOW);
    delay(300);
     digitalWrite(RELE,HIGH);
    delay(700);
    digitalWrite(RELE,LOW);
    delay(100);
    break;
    case 2:
    digitalWrite(RELE,HIGH);
    delay(1600);
    digitalWrite(RELE,LOW); 
     delay(400);
     digitalWrite(RELE,HIGH);
    delay(1400);
    digitalWrite(RELE,LOW);
    delay(300);
    digitalWrite(RELE,HIGH);
    delay(600);
    digitalWrite(RELE,LOW); 
     delay(800);
      digitalWrite(RELE,HIGH);
    delay(700);
    digitalWrite(RELE,LOW);
    delay(100);
    break;
    case 3:
    digitalWrite(RELE,HIGH);
    delay(1000);
    digitalWrite(RELE,LOW); 
     delay(1000);
     digitalWrite(RELE,HIGH);
    delay(1800);
    digitalWrite(RELE,LOW);
    delay(700);
    digitalWrite(RELE,HIGH);
    delay(1000);
    digitalWrite(RELE,LOW); 
     delay(800);
     digitalWrite(RELE,HIGH);
    delay(600);
    digitalWrite(RELE,LOW); 
     delay(900);
     digitalWrite(RELE,HIGH);
    delay(1100);
    digitalWrite(RELE,LOW);
    delay(100);  
    break;
      case 4:
   digitalWrite(RELE,HIGH);
    delay(1100);
    digitalWrite(RELE,LOW); 
     delay(200);
     digitalWrite(RELE,HIGH);
    delay(800);
    digitalWrite(RELE,LOW);
    delay(800);
    digitalWrite(RELE,HIGH);
    delay(900);
    digitalWrite(RELE,LOW); 
     delay(900);
     digitalWrite(RELE,HIGH);
    delay(1100);
    digitalWrite(RELE,LOW);
    delay(800);
    digitalWrite(RELE,HIGH);
    delay(1200);
    digitalWrite(RELE,LOW); 
     delay(400);
     digitalWrite(RELE,HIGH);
    delay(1600);
    digitalWrite(RELE,LOW);
    delay(100);
    break;
      case 5:
   digitalWrite(RELE,HIGH);
    delay(1400);
    digitalWrite(RELE,LOW); 
     delay(400);
     digitalWrite(RELE,HIGH);
    delay(1300);
    digitalWrite(RELE,LOW);
    delay(800);
    digitalWrite(RELE,HIGH);
    delay(700);
    digitalWrite(RELE,LOW); 
     delay(800);
     digitalWrite(RELE,HIGH);
    delay(1500);
    digitalWrite(RELE,LOW);
    delay(500);
    digitalWrite(RELE,HIGH);
    delay(900);
    digitalWrite(RELE,LOW); 
     delay(300);
     digitalWrite(RELE,HIGH);
    delay(900);
    digitalWrite(RELE,LOW);
    delay(300);
       digitalWrite(RELE,HIGH);
    delay(1200);
    digitalWrite(RELE,LOW);
    delay(500);
       digitalWrite(RELE,HIGH);
    delay(1800);
    digitalWrite(RELE,LOW);
    delay(100);
    break;
     case 6:
    digitalWrite(RELE,HIGH);
    delay(600);
    digitalWrite(RELE,LOW); 
     delay(600);
     digitalWrite(RELE,HIGH);
    delay(900);
    digitalWrite(RELE,LOW);
    delay(900);
    digitalWrite(RELE,HIGH);
    delay(2600);
    digitalWrite(RELE,LOW); 
     delay(1100);
    digitalWrite(RELE,HIGH);
    delay(700);
    digitalWrite(RELE,LOW); 
     delay(400);
        digitalWrite(RELE,HIGH);
    delay(1000);
    digitalWrite(RELE,LOW);
    delay(1000);
       digitalWrite(RELE,HIGH);
    delay(1000);
    digitalWrite(RELE,LOW);
    delay(1200);
    digitalWrite(RELE,HIGH);
    delay(900);
    digitalWrite(RELE,LOW);
    delay(100);
    break;
       case 7:
   digitalWrite(RELE,HIGH);
    delay(2000);
    digitalWrite(RELE,LOW); 
     delay(700);
     digitalWrite(RELE,HIGH);
    delay(1100);
    digitalWrite(RELE,LOW);
    delay(600);
    digitalWrite(RELE,HIGH);
    delay(1200);
    digitalWrite(RELE,LOW); 
     delay(500);
     digitalWrite(RELE,HIGH);
    delay(600);
    digitalWrite(RELE,LOW);
    delay(300);
    digitalWrite(RELE,HIGH);
    delay(800);
    digitalWrite(RELE,LOW); 
     delay(600);
     digitalWrite(RELE,HIGH);
    delay(800);
    digitalWrite(RELE,LOW);
    delay(400);
     digitalWrite(RELE,HIGH);
    delay(1100);
    digitalWrite(RELE,LOW);
    delay(900);
     digitalWrite(RELE,HIGH);
    delay(600);
    digitalWrite(RELE,LOW);
    delay(800);
     digitalWrite(RELE,HIGH);
    delay(1000);
    digitalWrite(RELE,LOW);
    delay(100);    
    break;
     }
    saundGryffindor ++;
    if(saundGryffindor  == 8){
      saundGryffindor  =1; 
    }
     }
     if(mySwitch.getReceivedValue()==753826 )
     { 
       mp3_play (saundSlytherin);
      delay(100);
      switch(saundSlytherin){
          case 8:
   digitalWrite(RELE,HIGH);
    delay(2300);
    digitalWrite(RELE,LOW); 
     delay(600);
     digitalWrite(RELE,HIGH);
    delay(900);
    digitalWrite(RELE,LOW);
    delay(600);
    digitalWrite(RELE,HIGH);
    delay(600);
    digitalWrite(RELE,LOW); 
     delay(100);
    
    break;
    case 9:
    digitalWrite(RELE,HIGH);
    delay(800);
    digitalWrite(RELE,LOW); 
     delay(400);
     digitalWrite(RELE,HIGH);
    delay(600);
    digitalWrite(RELE,LOW);
    delay(400);
     digitalWrite(RELE,HIGH);
    delay(700);
    digitalWrite(RELE,LOW); 
     delay(900);
     digitalWrite(RELE,HIGH);
    delay(1000);
    digitalWrite(RELE,LOW);
    delay(500);
    digitalWrite(RELE,HIGH);
    delay(1200);
    digitalWrite(RELE,LOW); 
     delay(700);
    digitalWrite(RELE,HIGH);
    delay(700);
    digitalWrite(RELE,LOW); 
     delay(400);
      digitalWrite(RELE,HIGH);
    delay(300);
    digitalWrite(RELE,LOW);
    delay(200);
     digitalWrite(RELE,HIGH);
    delay(500);
    digitalWrite(RELE,LOW);
    delay(700);
     digitalWrite(RELE,HIGH);
    delay(500);
    digitalWrite(RELE,LOW);
    delay(100);
    break;
    case 10:
     digitalWrite(RELE,HIGH);
    delay(1000);
    digitalWrite(RELE,LOW); 
     delay(1000);
     digitalWrite(RELE,HIGH);
    delay(800);
    digitalWrite(RELE,LOW);
    delay(900);
     digitalWrite(RELE,HIGH);
    delay(1000);
    digitalWrite(RELE,LOW); 
     delay(1100);
     digitalWrite(RELE,HIGH);
    delay(3000);
    digitalWrite(RELE,LOW);
    delay(900);
    digitalWrite(RELE,HIGH);
    delay(1500);
    digitalWrite(RELE,LOW); 
     delay(1000);
    digitalWrite(RELE,HIGH);
    delay(2100);
    digitalWrite(RELE,LOW); 
     delay(1000);
      digitalWrite(RELE,HIGH);
    delay(3000);
    digitalWrite(RELE,LOW);
    delay(100);
   
    break;
      case 11:
   digitalWrite(RELE,HIGH);
    delay(600);
    digitalWrite(RELE,LOW); 
     delay(300);
     digitalWrite(RELE,HIGH);
    delay(500);
    digitalWrite(RELE,LOW);
    delay(800);
    digitalWrite(RELE,HIGH);
    delay(1900);
    digitalWrite(RELE,LOW); 
     delay(500);
     digitalWrite(RELE,HIGH);
    delay(2100);
    digitalWrite(RELE,LOW);
    delay(400);
     digitalWrite(RELE,HIGH);
    delay(800);
    digitalWrite(RELE,LOW);
    delay(700);
     digitalWrite(RELE,HIGH);
    delay(1000);
    digitalWrite(RELE,LOW);
    delay(100);
    
    break;
      case 12:
   digitalWrite(RELE,HIGH);
    delay(1200);
    digitalWrite(RELE,LOW); 
     delay(400);
     digitalWrite(RELE,HIGH);
    delay(1000);
    digitalWrite(RELE,LOW);
    delay(400);
    digitalWrite(RELE,HIGH);
    delay(900);
    digitalWrite(RELE,LOW); 
     delay(400);
     digitalWrite(RELE,HIGH);
    delay(1000);
    digitalWrite(RELE,LOW);
    delay(800);
    digitalWrite(RELE,HIGH);
    delay(900);
    digitalWrite(RELE,LOW); 
     delay(300);
      digitalWrite(RELE,HIGH);
    delay(900);
    digitalWrite(RELE,LOW);
    delay(400);
     digitalWrite(RELE,HIGH);
    delay(1100);
    digitalWrite(RELE,LOW);
    delay(700);
     digitalWrite(RELE,HIGH);
    delay(800);
    digitalWrite(RELE,LOW);
    delay(100);
    
    break;
     case 13:
    digitalWrite(RELE,HIGH);
    delay(1000);
    digitalWrite(RELE,LOW); 
     delay(500);
     digitalWrite(RELE,HIGH);
    delay(3500);
    digitalWrite(RELE,LOW);
    delay(800);
    digitalWrite(RELE,HIGH);
    delay(2200);
    digitalWrite(RELE,LOW); 
     delay(600);
     digitalWrite(RELE,HIGH);
    delay(1700);
    digitalWrite(RELE,LOW); 
     delay(400);
      digitalWrite(RELE,HIGH);
    delay(1500);
    digitalWrite(RELE,LOW);
    delay(700);
     digitalWrite(RELE,HIGH);
    delay(500);
    digitalWrite(RELE,LOW);
    delay(400);
     digitalWrite(RELE,HIGH);
    delay(700);
    digitalWrite(RELE,LOW);
    delay(100);
    break;
       case 14:
   digitalWrite(RELE,HIGH);
    delay(3000);
    digitalWrite(RELE,LOW); 
     delay(400);
     digitalWrite(RELE,HIGH);
    delay(1300);
    digitalWrite(RELE,LOW);
    delay(400);
    digitalWrite(RELE,HIGH);
    delay(1600);
    digitalWrite(RELE,LOW); 
     delay(100);
   
    break;
     } 
       saundSlytherin ++; 
       if (saundSlytherin ==15){
        saundSlytherin =8;  
       }
     }
      if(mySwitch.getReceivedValue()== 753832 )
     { 
       mp3_play (saundStart);
     delay(100);
     switch(saundStart){
     case 15:
   digitalWrite(RELE,HIGH);
    delay(2800);
    digitalWrite(RELE,LOW); 
     delay(1100);
     digitalWrite(RELE,HIGH);
    delay(1200);
    digitalWrite(RELE,LOW);
    delay(900);
    digitalWrite(RELE,HIGH);
    delay(1000);
    digitalWrite(RELE,LOW); 
     delay(200);
     digitalWrite(RELE,HIGH);
    delay(1000);
    digitalWrite(RELE,LOW);
    delay(100);
   
     
    break;
    case 16:
    digitalWrite(RELE,HIGH);
    delay(2600);
    digitalWrite(RELE,LOW); 
     delay(500);
     digitalWrite(RELE,HIGH);
    delay(2000);
    digitalWrite(RELE,LOW);
    delay(700);
    digitalWrite(RELE,HIGH);
    delay(1700);
    digitalWrite(RELE,LOW); 
     delay(500);
      digitalWrite(RELE,HIGH);
    delay(2600);
    digitalWrite(RELE,LOW); 
     delay(400);
      digitalWrite(RELE,HIGH);
    delay(2400);
    digitalWrite(RELE,LOW); 
     delay(100);
    break;
     }
    saundStart ++; 
       if (saundStart ==17){
        saundStart =15;  
         }
     } 
      if(mySwitch.getReceivedValue()==753825 )
     { 
       mp3_play (saundFinish);
     delay(100);
     switch(saundFinish){
     case 17:
   digitalWrite(RELE,HIGH);
    delay(900);
    digitalWrite(RELE,LOW); 
     delay(500);
     digitalWrite(RELE,HIGH);
    delay(2300);
    digitalWrite(RELE,LOW);
    delay(800);
    digitalWrite(RELE,HIGH);
    delay(1200);
    digitalWrite(RELE,LOW); 
     delay(400);
     digitalWrite(RELE,HIGH);
    delay(800);
    digitalWrite(RELE,LOW);
    delay(400);
    digitalWrite(RELE,HIGH);
    delay(900);
    digitalWrite(RELE,LOW); 
     delay(300);
     digitalWrite(RELE,HIGH);
    delay(2300);
    digitalWrite(RELE,LOW);
    delay(600);
    break;
    case 18:
    digitalWrite(RELE,HIGH);
    delay(1800);
    digitalWrite(RELE,LOW); 
     delay(300);
     digitalWrite(RELE,HIGH);
    delay(2700);
    digitalWrite(RELE,LOW);
    delay(500);
    digitalWrite(RELE,HIGH);
    delay(1200);
    digitalWrite(RELE,LOW); 
     delay(300);
      digitalWrite(RELE,HIGH);
    delay(2100);
    digitalWrite(RELE,LOW); 
     delay(100);
    break;
     }
    saundFinish ++; 
       if (saundFinish ==19){
        saundFinish =17;  
         }
     } 
    mySwitch.resetAvailable();
  }
}
