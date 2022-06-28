#include "FirebaseESP8266.h" // 반드시 파이어베이스 라이브러리를 먼저 추가해야 합니다.
#include <ESP8266WiFi.h>

#define FIREBASE_HOST "smart-bus-stop-c7dce-default-rtdb.firebaseio.com"
#define FIREBASE_AUTH "PDWHSTw75pTfqguIFxduLSbmInTeiwf7QRdO5tPz"
#define WIFI_SSID "AP-104-1101" // 연결 가능한 wifi의 ssid
#define WIFI_PASSWORD "3183a!69" // wifi 비밀번호

#define pinBUS11  D2
#define pinBUS12  D2
#define pinBUS21  D2
#define pinBUS22  D2
#define pinBUS31  D2
#define pinBUS32  D2

#define BusDelay  60   // 몇 분마다 버스가 이동할 것인 지 설정

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

void loop(){
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
  
  delay(200);     //1000 일때 6초마다 버스 이동
}
