#include <SoftwareSerial.h>

class ParkingLot {
  // 필드 변수
  private :
    bool firstSensor; // 진입 센서
    bool secondSensor; // 주차 센서
    int sensor1; // 진입 센서 핀넘버
    int sensor2; // 주차 센서 핀넘버
    int led; // 인디케이터 핀넘버
    int secTimer = 0; // 주차 시간 저장 변수
  // 필드 함수
   public :
   // 생성자
    ParkingLot(int sensor1, int sensor2, int led) : sensor1(sensor1), sensor2(sensor2), led(led) {
      pinMode(led, OUTPUT); // 해당 인디케이터 핀넘버를 출력으로 설정
    }
    // 매 루프 호출하여 주차장의 상태를 체크하여 변수를 초기화 하는 함수
    void initLot() {
    if (digitalRead(sensor1) == 0) {
      firstSensor = true;
    } else {
      firstSensor = false;
      secTimer = 0;
    }
    if (digitalRead(sensor2) == 0) {
      secondSensor = true;
      secTimer ++;
    } else {
      secondSensor = false;
    }
  }
  // 변수를 계산하여 자리가 비어있는지 확인하는 함수
  bool isClear() {
    if ((!secondSensor && (secTimer > 5)) || (!firstSensor && !secondSensor)) { 
      // 차량이 완전히 주자되어있지 않고 타이머가 설정시간을 초과했으면 출차중
      // 진입중인 차량도 없고 주차되어 있는 차량도 없으면 비어있음
      digitalWrite(led, HIGH);
      return true;
    } else {
      digitalWrite(led, LOW);
      return false;
    }
  }
};

// 불 배열을 바이트로 변환하는 함수
static byte boolToByte(bool source[], int arraySize) {
  byte result = 0;
  for(int i = 0; i < arraySize; i ++) {
    if (source[i] == true) {
      result |=  (byte)(1 << i);
    }
  }
  return result;
}

static ParkingLot lotArray[5] = {
    ParkingLot(2, 3, 4), ParkingLot(5, 6, 7),
    ParkingLot(8, 9, 10), ParkingLot(11, 12, 13),
    ParkingLot(14, 15, 16)
};

int rxPin = 18; int txPin = 17;
SoftwareSerial bleMod(txPin, rxPin);
// 74278bda-b644-720eaf059935

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600); bleMod.begin(9600);
  Serial.println("System Setting Start, Type \'F\' To Finish");
  // 초기화를 위해서는 AT+RENEW AT+RESET AT+ADVI0 AT+IBEA1 AT+RESET 
  while (true) {
    if (bleMod.available()) {
      Serial.write(bleMod.read());
    }
    if (Serial.available()) {
      char input = Serial.read(); if (input == 'F') {
        break;
      }
      bleMod.write(input);
    }
  }
}

static bool result[8];
static byte data = 0;
static char hex[10]; // 헥사코드 데이터 배열

void loop() {
  // put your main code here, to run repeatedly:
  for (int i = 0; i < 5; i ++) {
    lotArray[i].initLot();
    result[i] = lotArray[i].isClear();
  }
  data = boolToByte(result, 8); sprintf(hex, "0x%04X", data);
  Serial.print(data); Serial.println(hex);
  bleMod.print("AT+MARJ"); bleMod.println(hex);
  delay(500);
  bleMod.println("AT+RESET");
  delay(1500);
}
