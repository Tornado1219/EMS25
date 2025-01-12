#include "BluetoothSerial.h"

const int L = 27;
const int M = 14;
const int R = 13;

const int L2 = 32;
const int M2 = 33;
const int R2 = 26;

const int Poti = 26;
int PotiMode = 0;
int st = 0;

int FreqLaden = 430;
int ImpWLaden = 311;
int Freq = 65;
int ImpW = 250;
int Versatz = 100;
volatile int MHD = 0;
volatile int MLD = 0;
volatile int FreqDR = 0;
volatile int FreqDL = 0;

int FreqLaden2 = 430;
int ImpWLaden2 = 311;
int Freq2 = 65;
int ImpW2 = 250;

int Abstand = 5000;

const int AnzKeys = 5;
String keys[] = { "FQ1", "IW1", "VP1", "FL1", "IL1" };
int* Variable[] = { &Freq, &ImpW, &Versatz, &FreqLaden, &ImpWLaden };


volatile int MHD2 = 0;
volatile int MLD2 = 0;

volatile int FreqD = (1000000 / Freq) / 2;
volatile int FreqD2 = (1000000 / Freq) / 2;

volatile bool CycleR = true;
volatile bool CycleR2 = true;

volatile bool Strom = false;


hw_timer_t* Timer0_Cfg = NULL;
hw_timer_t* Timer1_Cfg = NULL;
hw_timer_t* Timer2_Cfg = NULL;
hw_timer_t* Timer3_Cfg = NULL;


BluetoothSerial SerialBT;

void IT0() {
  if (digitalRead(M)) {
    digitalWrite(M, LOW);
    timerAlarmWrite(Timer0_Cfg, MHD, true);
  } else {
    digitalWrite(M, HIGH);
    timerAlarmWrite(Timer0_Cfg, MLD, true);
  }


  timerAlarmEnable(Timer0_Cfg);
}



void PT1() {

  if (CycleR) {
    if (!digitalRead(R)) {
      digitalWrite(R, HIGH);
      timerAlarmWrite(Timer1_Cfg, FreqDR, true);
      CycleR = false;
      Strom = false;
    } else {
      if (!Strom) {
        digitalWrite(R, LOW);
        timerAlarmWrite(Timer1_Cfg, ImpW, true);
        Strom = true;
      } else {
        timerAlarmWrite(Timer1_Cfg, 2 * ImpW2, true);
      }
    }
  } else {
    if (!digitalRead(L)) {
      digitalWrite(L, HIGH);
      timerAlarmWrite(Timer1_Cfg, FreqDL, true);
      CycleR = true;
      Strom = false;
    } else {
      if (!Strom) {
        digitalWrite(L, LOW);
        timerAlarmWrite(Timer1_Cfg, ImpW, true);
        Strom = true;
      } else {
        timerAlarmWrite(Timer1_Cfg, 2 * ImpW2, true);
      }
    }
  }
}


void IT2() {
  if (digitalRead(M2)) {
    digitalWrite(M2, LOW);
    timerAlarmWrite(Timer2_Cfg, MHD2, true);
  } else {
    digitalWrite(M2, HIGH);
    timerAlarmWrite(Timer2_Cfg, MLD2, true);
  }


  timerAlarmEnable(Timer2_Cfg);
}




void PT3() {
  if (CycleR) {
    if (!digitalRead(R2)) {
      digitalWrite(R2, HIGH);
      timerAlarmWrite(Timer3_Cfg, FreqD2, true);
      CycleR2 = false;
      Strom = false;
    } else {
      if (!Strom) {
        digitalWrite(R2, LOW);
        timerAlarmWrite(Timer3_Cfg, ImpW2, true);
        Strom = true;
      } else {
        timerAlarmWrite(Timer3_Cfg, 2 * ImpW, true);
      }
    }
  } else {
    if (!digitalRead(L2)) {
      digitalWrite(L2, HIGH);
      timerAlarmWrite(Timer3_Cfg, FreqD2, true);
      CycleR2 = true;
      Strom = false;
    } else {
      if (!Strom) {
        digitalWrite(L2, LOW);
        timerAlarmWrite(Timer3_Cfg, ImpW2, true);
        Strom = true;
      } else {
        timerAlarmWrite(Timer3_Cfg, 2 * ImpW, true);
      }
    }
  }
}



void setup() {
  // put your setup code here, to run once:
  SerialBT.begin("EMS_Gerät");
  Serial.begin(115200);
  //Serial.begin(115200);
  pinMode(Poti, INPUT);
  pinMode(L, OUTPUT);
  pinMode(M, OUTPUT);
  pinMode(R, OUTPUT);
  DelayMberechnen();
  pinMode(L2, OUTPUT);
  pinMode(M2, OUTPUT);
  pinMode(R2, OUTPUT);
  DelayMberechnen2();
  FreqDberechnen();

  Timer0_Cfg = timerBegin(0, 80, true);
  timerAttachInterrupt(Timer0_Cfg, &IT0, true);
  timerAlarmWrite(Timer0_Cfg, MLD, true);
  timerAlarmEnable(Timer0_Cfg);

  Timer1_Cfg = timerBegin(1, 80, true);
  timerAttachInterrupt(Timer1_Cfg, &PT1, true);
  timerAlarmWrite(Timer1_Cfg, FreqD, true);
  timerAlarmEnable(Timer1_Cfg);


  Timer2_Cfg = timerBegin(2, 80, true);
  timerAttachInterrupt(Timer2_Cfg, &IT2, true);
  timerAlarmWrite(Timer2_Cfg, MLD2, true);
  timerAlarmEnable(Timer2_Cfg);

  Timer3_Cfg = timerBegin(3, 80, true);
  timerAttachInterrupt(Timer3_Cfg, &PT3, true);
  timerAlarmWrite(Timer3_Cfg, FreqD2, true);
  timerAlarmEnable(Timer3_Cfg);

  Serial.println(FreqDR);
  Serial.println(FreqDL);
}

void loop() {
  if (SerialBT.available()) {
    String txt = SerialBT.readString();
    for (int i = 0; i < AnzKeys; i++) {
      if (txt.startsWith(keys[i])) {
        *Variable[i] = txt.substring(3).toInt();
        FreqDberechnen();
        DelayMberechnen();
        SerialBT.println((String)keys[i] + *Variable[i] + '|');
        Serial.println((String)keys[i] + *Variable[i]);
        Serial.println(ImpW);
        Serial.println(FreqD);
      }
    }
    if (txt.startsWith("ALL")) {
      for (int i = 0; i < AnzKeys; i++) {
        SerialBT.println((String)keys[i] + *Variable[i] + '|');
      }
    }
  }
}

void loop2(void* pvParameters) {
  while (1) {
    digitalWrite(M, LOW);
    delayMicroseconds(MHD);
    digitalWrite(M, HIGH);
    delayMicroseconds(MLD);
  }
}

void loop3(void* pvParameters) {
  while (1) {
    digitalWrite(L, LOW);
    delayMicroseconds(ImpW);
    digitalWrite(L, HIGH);
    delayMicroseconds(FreqD);
    digitalWrite(R, LOW);
    delayMicroseconds(ImpW);
    digitalWrite(R, HIGH);
    delayMicroseconds(FreqD);
  }
}

void loop4(void* pvParameters) {
  while (1) {
    digitalWrite(M2, LOW);
    delayMicroseconds(MHD2);
    digitalWrite(M2, HIGH);
    delayMicroseconds(MLD2);
  }
}

void loop5(void* pvParameters) {
  while (1) {
    digitalWrite(L2, LOW);
    delayMicroseconds(ImpW2);
    digitalWrite(L2, HIGH);
    delayMicroseconds(FreqD2);
    digitalWrite(R2, LOW);
    delayMicroseconds(ImpW2);
    digitalWrite(R2, HIGH);
    delayMicroseconds(FreqD2);
  }
}


void FreqDberechnen() {
  FreqD = (1000000 / Freq) / 2;
  float Prozent = (Versatz / 100.0);
  FreqDR = ((1000000 / Freq) / 2) * (1.0 - Prozent);
  FreqDL = ((1000000 / Freq) / 2) * (1.0 + Prozent);

  //Serial.println(FreqD);
}



void DelayMberechnen() {
  MHD = (1000000.0 / FreqLaden);
  //Serial.println(MHD);
  float x = (ImpWLaden / 1000.0);
  //Serial.println(x);
  MHD = MHD * x;
  //Serial.println(MHD);


  //Serial.println("___________");

  MLD = (1000000.0 / FreqLaden);
  //Serial.println(MLD);
  x = 1 - x;
  //Serial.println(x);
  MLD = MLD * x;
  //Serial.println(MLD);
}


void FreqDberechnen2() {
  FreqD2 = (1000000 / Freq2) / 2;
  //Serial.println(FreqD2);
}



void DelayMberechnen2() {
  MHD2 = (1000000.0 / FreqLaden2);
  //Serial.println(MHD2);
  float x = (ImpWLaden2 / 1000.0);
  //Serial.println(x);
  MHD2 = MHD2 * x;
  //Serial.println(MHD2);


  //Serial.println("___________");

  MLD2 = (1000000.0 / FreqLaden2);
  //Serial.println(MLD2);
  x = 1 - x;
  //Serial.println(x);
  MLD2 = MLD2 * x;
  //Serial.println(MLD2);
}
