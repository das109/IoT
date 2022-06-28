#include "FirebaseESP8266.h" // 반드시 파이어베이스 라이브러리를 먼저 추가해야 합니다.
#include <ESP8266WiFi.h>
 
#define FIREBASE_HOST "iot-project-team7-default-rtdb.firebaseio.com" 
#define FIREBASE_AUTH "syPHwV3ZvHp25xKJTjkggv5o775mIUcVgP7JvwVr"
#define WIFI_SSID "SK_WiFiGIGA248F" // 연결 가능한 wifi의 ssid
#define WIFI_PASSWORD "1712035400" // wifi 비밀번호
#define pinLED1 D5
#define pinLED2 D6

 
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

  pinMode(pinLED1, OUTPUT);
  pinMode(pinLED2, OUTPUT);
   
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  Firebase.reconnectWiFi(true);
 
  firebaseData.setBSSLBufferSize(1024, 1024);
  firebaseData.setResponseSize(1024);
  Firebase.setReadTimeout(firebaseData, 1000 * 60);
  Firebase.setwriteSizeLimit(firebaseData, "tiny");
}



bool value1;
bool value2;

bool valLED1 = false;
bool valLED2 = false;

void loop() {
  // put your main code here, to run repeatedly:
  
  if (Firebase.getBool(firebaseData, "bus1")) // 해당하는 key가 있으면 (앱 인벤터에서는 tag)
  {
    value1 = firebaseData.boolData(); // 값을 문자열로 받아와서 value에
    Serial.print("bus1 = ");
    Serial.println(value1); // 시리얼모니터에 값을 출력
    
    digitalWrite(pinLED1, value1);
 
  }
  else
  {
    Serial.println(firebaseData.errorReason()); // 에러 메시지 출력
  }

  

  if (Firebase.getBool(firebaseData, "bus2")) 
  {
    value2 = firebaseData.boolData(); 
    Serial.print("bus2 = ");
    Serial.println(value2); 
    
    digitalWrite(pinLED2, value2);

  }
  else
  {
    Serial.println(firebaseData.errorReason()); 
  }


}
