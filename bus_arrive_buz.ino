#include "FirebaseESP8266.h" // 반드시 파이어베이스 라이브러리를 먼저 추가해야 합니다.
#include <ESP8266WiFi.h>
 
#define FIREBASE_HOST "iot-project-team7-default-rtdb.firebaseio.com" 
#define FIREBASE_AUTH "syPHwV3ZvHp25xKJTjkggv5o775mIUcVgP7JvwVr"
#define WIFI_SSID "SK_WiFiGIGA248F" // 연결 가능한 wifi의 ssid
#define WIFI_PASSWORD "1712035400" // wifi 비밀번호

#define pinBUS11  D2
#define pinBUS12  D2
#define pinBUS21  D2
#define pinBUS22  D2
#define pinBUS31  D2
#define pinBUS32  D2
#define pinLED1 D5
#define pinLED2 D6

#define BusDelay  60 
 
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

  pinMode(pinBUS11, OUTPUT);
  pinMode(pinBUS12, OUTPUT);
  pinMode(pinBUS21, OUTPUT);
  pinMode(pinBUS22, OUTPUT);
  pinMode(pinBUS31, OUTPUT);
  pinMode(pinBUS32, OUTPUT);
  pinMode(pinLED1, OUTPUT);
  pinMode(pinLED2, OUTPUT);
     
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  Firebase.reconnectWiFi(true);
 
  firebaseData.setBSSLBufferSize(1024, 1024);
  firebaseData.setResponseSize(1024);
  Firebase.setReadTimeout(firebaseData, 1000 * 60);
  Firebase.setwriteSizeLimit(firebaseData, "tiny");
}


int i = 0;

int flag11 = 1;
int flag12 = 0;
int flag21 = 0;
int flag22 = 0;
int flag31 = 0;
int flag32 = 0;
int flag[] = {flag11, flag12, flag21, flag22, flag31, flag32};

String arrayBus11[] = {"A1", "AB", "B1", "BC", "C1", "CD", "D1", "DA"};
String arrayBus12[] = {"AD", "D2", "DC", "C2", "CB", "B2", "BA", "A2"};
String arrayBus21[] = {"A1", "AB", "B1", "BA"};
String arrayBus22[] = {"AB", "B2", "BA", "A2"};
String arrayBus31[] = {"C1", "CD", "D1", "DC"};
String arrayBus32[] = {"CD", "D2", "DC", "C2"};


String value1;
String value2;

String resetLED1;
String resetLED2;

String valLED1;
String valLED2;
int int_LED1;
int int_LED2;

void loop() {
  // put your main code here, to run repeatedly:
  if (i < 5){
    i = i +1;
  }
  else{
    i = 0;
  }           // 시간 기준이 되는 i의 변화 설정 (0~5까지 변화)

  switch(i){
    case 0: flag11++;
            break;
    case 1: flag12++;
            break;
    case 2: flag21++;
            break;
    case 3: flag22++;
            break;
    case 4: flag31++;
            break;
    case 5: flag32++;
            break;
    default: break;
  }

  if (flag11 == 8){
    flag11 = 0;
  }
  if (flag12 == 8){
    flag12 = 0;
  }
  if (flag21 == 4){
    flag21 = 0;
  }
  if (flag22 == 4){
    flag22 = 0;
  }
  if (flag31 == 4){
    flag31 = 0;
  }
  if (flag32 == 4){
    flag32 = 0;
  }

  Firebase.setString(firebaseData, "Bus1_1", arrayBus11[flag11]);
  Firebase.setString(firebaseData, "Bus1_2", arrayBus12[flag12]);
  Firebase.setString(firebaseData, "Bus2_1", arrayBus21[flag21]);
  Firebase.setString(firebaseData, "Bus2_2", arrayBus22[flag22]);
  Firebase.setString(firebaseData, "Bus3_1", arrayBus31[flag31]);
  Firebase.setString(firebaseData, "Bus3_2", arrayBus32[flag32]);

  Serial.print("i = ");
  Serial.println(i);
  Serial.print("Bus1_1 = ");
  Serial.println(flag11);
  Serial.print("Bus1_2 = ");
  Serial.println(flag12);
  Serial.print("Bus2_1 = ");
  Serial.println(flag21);
  Serial.print("Bus2_2 = ");
  Serial.println(flag22);
  Serial.print("Bus3_1 = ");
  Serial.println(flag31);
  Serial.print("Bus3_2 = ");
  Serial.println(flag32);
  
  delay(200);


  //버스정류장 A1의 누군가가 bus1(A1_1)을 눌렀는지 체크 
  if (Firebase.getString(firebaseData, "Bus1_1"))
  {
    value1 = firebaseData.stringData(); // 값을 문자열로 받아와서 value에
    Serial.print("Bus1_1의 위치 = ");
    Serial.println(value1); // 시리얼모니터에 값을 출력
    
    if (value1 == "DA")
    {

      Firebase.getString(firebaseData, "A1_1");
      valLED1 = firebaseData.stringData(); // 값을 문자열로 받아와서 value에
      Serial.println(valLED1); // 시리얼모니터에 값을 출력
      //#int_LED1=atoi(valLED1.c_str());
      if (valLED1 != "0"){
        digitalWrite(pinLED1, true);
      }
    }
  }
  else
  {
    Serial.println(firebaseData.errorReason()); // 에러 메시지 출력
  }


  //만약 bus1(A1_1) 버스가 A1를 지났으면 bus1(A1_1)값을 초기화합니다
  if (Firebase.getString(firebaseData, "Bus1_1"))
  {
    value1 = firebaseData.stringData(); // 값을 문자열로 받아와서 value에
    Serial.print("Bus1_1의 위치 = ");
    Serial.println(value1); // 시리얼모니터에 값을 출력
    
    if (value1 == "A1")
    {
      if (value1 == "A1"){
        resetLED1 = "0";
        Firebase.setString(firebaseData, "A1_1", resetLED1);
        digitalWrite(pinLED1, false);
      }
    }
  }
  else
  {
    Serial.println(firebaseData.errorReason()); // 에러 메시지 출력
  }  

  
  //버스정류장 A1의 누군가가 bus2(A2_1)을 눌렀는지 체크 
  if (Firebase.getString(firebaseData, "Bus2_1"))
  {
    value2 = firebaseData.stringData(); // 값을 문자열로 받아와서 value에
    Serial.print("Bus2의 위치 = ");
    Serial.println(value2); // 시리얼모니터에 값을 출력
    
    if (value2 == "BA")
    {

      Firebase.getString(firebaseData, "A2_1");
      valLED2 = firebaseData.stringData(); // 값을 문자열로 받아와서 value에
      Serial.println(valLED2); // 시리얼모니터에 값을 출력
      //#int_LED1=atoi(valLED1.c_str());
      if (valLED2 != "0"){
        digitalWrite(pinLED2, true);
      }
    }
  }
  else
  {
    Serial.println(firebaseData.errorReason()); // 에러 메시지 출력
  }

  
   //만약 bus2(A1_2)버스가 A1를 지났으면 bus2(A2_1)값을 초기화합니다
  if (Firebase.getString(firebaseData, "Bus2_1")) 
  {
    value2 = firebaseData.stringData(); // 값을 문자열로 받아와서 value에
    Serial.print("Bus2_1의 위치 = ");
    Serial.println(value2); // 시리얼모니터에 값을 출력
    
    if (value2 == "A1")
    {
      resetLED2 = "0";
      Firebase.setString(firebaseData, "A2_1", resetLED2);
      digitalWrite(pinLED2, false);
    }
  }
  else
  {
    Serial.println(firebaseData.errorReason()); // 에러 메시지 출력
  }


}
