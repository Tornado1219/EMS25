#include "BluetoothSerial.h"
#include <Arduino.h>


const int Touchpin = 13;
const int TouchSchwelle = 20;
const int LEDpin = 12;
const int Relaispin = 14;
const int AnaPin = 26;

int st = 0;

bool GREG = true;

BluetoothSerial SerialBT;




void ohneEMS() {
  int i = millis() - st;
  touchDetachInterrupt(Touchpin);
  digitalWrite(LEDpin, LOW);
  String s = "";
  s += i;
  s += "ms";
  SerialBT.println(s);
  Serial.println(i);
}

void mitEMS() {
  int i = millis() - st;
  touchDetachInterrupt(Touchpin);
  digitalWrite(LEDpin, LOW);
  digitalWrite(Relaispin, LOW);
  
  String s = "";
  s += i;
  s += "ms";
  SerialBT.println(s);
  Serial.println(i);
}



void FingerSteuern() {
  SerialBT.println("FS Aktiv");
  while (true) {
        if (touchRead(Touchpin) < TouchSchwelle) {
          digitalWrite(Relaispin, HIGH);
          digitalWrite(LEDpin, HIGH);
        } else {
          digitalWrite(Relaispin, LOW);
          digitalWrite(LEDpin, LOW);
                  }
        if (SerialBT.available()) {
          
          //Serial.println(txt);
          if (SerialBT.read() == 0x02) {
            Serial.println("--ms");
            break;
          }
        }
      }
}





long st1 = 0;
void GREGloop() {
  
  while (GREG) {
    if( (millis() - st1) > 150) {
    switch (Serial.read()) {
    case 0x02: digitalWrite(Relaispin, LOW); st = millis(); break;
    case 0x03: digitalWrite(Relaispin, HIGH); break;
    case 0x04: GREG = false; break;
  }
    }

  static int st = 0;
  if((millis() - st) > 200) {
    digitalWrite(LEDpin, !digitalRead(LEDpin));
    st = millis();
  }
  if(SerialBT.available()) {
    if(SerialBT.read() == 0x09){
      break;
    } 
  }
  }
}


void GREGoff() {
  GREG = false;
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  SerialBT.begin("Reaktionszeitmesser");
  pinMode(LEDpin, OUTPUT);
  pinMode(Relaispin, OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  if (SerialBT.available()) {
    int stpeak;
    //Serial.println(i);
    switch(SerialBT.read()) {
      case 0x02 :  SerialBT.println("...");
      delay(random(500, 2500));
                touchAttachInterrupt(Touchpin, ohneEMS, TouchSchwelle); 
                digitalWrite(LEDpin, HIGH);
                st = millis(); 
                break;
      
      case 0x03:  SerialBT.println("....");
                delay(random(500, 2500));
                while(analogRead(AnaPin) > 100){}
                Serial.println("1");
                while(analogRead(AnaPin) < 100) {
                  Serial.println(analogRead(AnaPin));
                }
                Serial.println("2");
                stpeak = micros();
                while(analogRead(AnaPin) > 100) {
      
                }
                Serial.println("3");
                while(analogRead(AnaPin) < 100) {
      
                }
                Serial.println("4");
                Serial.println(micros() - stpeak);
                Serial.println((micros() - stpeak) + micros());
                Serial.println(((micros() - stpeak) + micros()) - 1000);
                Serial.println(micros());
                stpeak = (((micros() - stpeak) + micros()) - 5000);
                while(stpeak > micros()) {
                  Serial.println(micros());
                  delay(2);
                }
                touchAttachInterrupt(Touchpin, mitEMS, TouchSchwelle); 
                digitalWrite(LEDpin, HIGH);
                digitalWrite(Relaispin, HIGH);
                st = millis(); 
                break;
      case 0x04: FingerSteuern(); break;
      case 0x05: touchDetachInterrupt(Touchpin);
                digitalWrite(LEDpin, LOW);
                digitalWrite(Relaispin, LOW);
                SerialBT.println("STOP");
                break;
      case 0x07: digitalWrite(Relaispin, HIGH); break;
      case 0x08: digitalWrite(Relaispin, LOW); break;
      case 0x09: SerialBT.println("GREG");
                  GREG = true;
                  touchAttachInterrupt(Touchpin, GREGoff, TouchSchwelle);
                  GREGloop();
                  digitalWrite(Relaispin, LOW);
                  digitalWrite(LEDpin, LOW);
                  SerialBT.println("");
                  break;
                
    }
  }
}

