#include "FirebaseESP8266.h" // 반드시 파이어베이스 라이브러리를 먼저 추가해야 합니다.
#include <ESP8266WiFi.h>
 
#define FIREBASE_HOST "iot-project-team7-default-rtdb.firebaseio.com" 
#define FIREBASE_AUTH "syPHwV3ZvHp25xKJTjkggv5o775mIUcVgP7JvwVr"
//#define WIFI_SSID "SK_WiFiGIGA248F" // 연결 가능한 wifi의 ssid
//#define WIFI_PASSWORD "1712035400" // wifi 비밀번호
#define WIFI_SSID "AP-104-1101"
#define WIFI_PASSWORD "3183a!69"

//STOP&SWITCH
#define pinSW1  D12
#define pinSW2  D13

#define pinBUSLED1  D9
#define pinBUSLED2  D10

#define pinBuzzer   D8   //D1 보드에서 UNO 보드의 버저를 울리기 위해 보내는 OUTPUT
#define pinFire     D11
#define pinFIRELED  D2

FirebaseData firebaseData;
FirebaseJson json;
 
void printResult(FirebaseData &data);
void setup() // wifi 접속 과정.
{
  Serial.begin(9600);
 
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.println();
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(500);
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();

  pinMode(pinSW1, INPUT);
  pinMode(pinSW2, INPUT);
  pinMode(pinFire, INPUT);
  pinMode(pinFIRELED, OUTPUT);
  pinMode(pinBUSLED1, OUTPUT);
  pinMode(pinBUSLED2, OUTPUT);
  pinMode(pinBuzzer, OUTPUT);
 
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  Firebase.reconnectWiFi(true);
 
  firebaseData.setBSSLBufferSize(1024, 1024);
  firebaseData.setResponseSize(1024);
  Firebase.setReadTimeout(firebaseData, 1000 * 60);
  Firebase.setwriteSizeLimit(firebaseData, "tiny");
  
}

// Switch_func 에 필요한 변수 선언
bool last_state_sw1 = false;
bool last_state_sw2 = false;
bool current_state_sw1 = false;
bool current_state_sw2 = false;
bool valLED1 = false;
bool valLED2 = false;

String  valA1_1 = "";
String  valA1_2 = "";

int int_valA1_1 = 0;
int int_valA1_2 = 0;

// Bus_arrived 에 필요한 변수 선언
String  Bus1_state  = "";   //버스 1_1의 위치
String  Bus2_state  = "";   //버스 2_1의 위치
String  Bus1_lstate = "";   //버스 1_1의 이전 위치
String  Bus2_lstate = "";   //버스 2_1의 이전 위치
String  Alarm1_state  = ""; //A1_1 값
String  Alarm2_state  = ""; //A1_2 값


void Fire(){
  bool fire = digitalRead(pinFire);
  if (fire == HIGH){
    Firebase.setBool(firebaseData, "Fire Situation", 1);
    digitalWrite(pinFIRELED, !digitalRead(pinFIRELED));
    delay(100);
  }
  else{
    Firebase.setBool(firebaseData, "Fire Situation", 0);
    digitalWrite(pinFIRELED, LOW);
  }
}


void Switch_func(){   // 정류장 내 스위치를 통한 승차 요청 Firebase에 전달, LED 제어
  
  current_state_sw1 = !digitalRead(pinSW1);
  current_state_sw2 = !digitalRead(pinSW2);
  
  delay(20);

  if (current_state_sw1 && (current_state_sw1 !=last_state_sw1)) {
    valLED1 = !valLED1;
    digitalWrite(pinBUSLED1, valLED1);
//    Firebase.setBool(firebaseData, "bus1", digitalRead(valLED1));
    Serial.print("SW1 Activated!  ");
    Serial.println(valLED1);
    Firebase.getString(firebaseData, "A1_1");
    valA1_1 = firebaseData.stringData();
    int_valA1_1 = valA1_1.toInt();
    
    if (valLED1){
      int_valA1_1 = int_valA1_1 +1;
    }
    else{
      int_valA1_1 = int_valA1_1 -1;
    }
    
    valA1_1 = String(int_valA1_1);
    Firebase.setString(firebaseData, "A1_1", valA1_1);
  }
  
  if (current_state_sw2 && (current_state_sw2 !=last_state_sw2)) {
    valLED2 = !valLED2;
    digitalWrite(pinBUSLED2, valLED2);
//    Firebase.setBool(firebaseData, "bus2", digitalRead(valLED2));
    Serial.print("SW2 Activated!  ");
    Serial.println(valLED2);
    Firebase.getString(firebaseData, "A1_2");
    valA1_2 = firebaseData.stringData();
    int_valA1_2 = valA1_2.toInt();
    
    if (valLED2){
      int_valA1_2 = int_valA1_2 +1;
    }
    else{
      int_valA1_2 = int_valA1_2 -1;
    }
    
    valA1_2 = String(int_valA1_2);
    Firebase.setString(firebaseData, "A1_2", valA1_2);
  }

  last_state_sw1 = current_state_sw1;
  last_state_sw2 = current_state_sw2;

}

void Bus_arrived(){   // 버스가 잠시 후 도착할 때, 버저를 울려 알림
                      // 버스 도착 시에 스위치를 통한 승차 요청 초기화

  //버스1_1, 2_1의 위치와 firebase의 A1_1, A1_2 값 저장
  Firebase.getString(firebaseData, "Bus1_1"); //  
  Bus1_state = firebaseData.stringData();
  Firebase.getString(firebaseData, "Bus2_1"); 
  Bus2_state = firebaseData.stringData();

  Firebase.getString(firebaseData, "A1_1");
  Alarm1_state = firebaseData.stringData();
  Firebase.getString(firebaseData, "A1_2");
  Alarm2_state = firebaseData.stringData();

  delay(50);

  if (Bus1_state != Bus1_lstate){
    if((Bus1_state == "DA") && (Alarm1_state != "0")){
      digitalWrite(pinBuzzer, !digitalRead(pinBuzzer));
      Serial.println("Buzzer requested!  for...Bus1_1");
      Serial.println(digitalRead(pinBuzzer));
    }
    if(valLED1 && (Bus1_state == "A1")){
      valLED1 = false;
      digitalWrite(pinBUSLED1, valLED1);
      Serial.println("SW1 Initiated!  ");
      
      Firebase.setString(firebaseData, "A1_1", "0");
    }
  }
  
  if (Bus2_state != Bus2_lstate){
    if((Bus2_state == "DA") && (Alarm2_state != "0")){
      digitalWrite(pinBuzzer, !digitalRead(pinBuzzer));
      Serial.println("Buzzer requested!  for...Bus2_1");
    }
    if(valLED2 && (Bus2_state == "A1")){
      valLED2 = false;
      digitalWrite(pinBUSLED2, valLED2);
      Serial.println("SW2 Initiated!  ");
      
      Firebase.setString(firebaseData, "A1_2", "0");
    }
  }
  delay(50);
  Bus1_lstate = Bus1_state;
  Bus2_lstate = Bus2_state;
}

void loop(){
//Switch&Bus loop
  Switch_func();
  Bus_arrived();
  Fire();

  delay(10);
}
