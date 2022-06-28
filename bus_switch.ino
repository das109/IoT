
#include "FirebaseESP8266.h" // 반드시 파이어베이스 라이브러리를 먼저 추가해야 합니다.
#include <ESP8266WiFi.h>
 
#define FIREBASE_HOST "iot-project-team7-default-rtdb.firebaseio.com" 
#define FIREBASE_AUTH "syPHwV3ZvHp25xKJTjkggv5o775mIUcVgP7JvwVr"
#define WIFI_SSID "SK_WiFiGIGA248F" // 연결 가능한 wifi의 ssid
#define WIFI_PASSWORD "1712035400" // wifi 비밀번호

#define pinSW1  D2
#define pinSW2  D3
#define pinLED1 D5
#define pinLED2 D6

#define PIN_BUZ  D10


int length = 7;

char notes[] = "ccggaag"; 
int beats[] = { 1, 1, 1, 1, 1, 1, 2};
int tempo = 300;

void playTone(int tone, int duration) {
  for (long i = 0; i < duration * 1000L; i += tone * 2) {
    digitalWrite(PIN_BUZ, HIGH);
    delayMicroseconds(tone);
    digitalWrite(PIN_BUZ, LOW);
    delayMicroseconds(tone);
  }
}

void playNote(char note, int duration) {
  char names[] = { 'c', 'd', 'e', 'f', 'g', 'a', 'b', 'C' };
  int tones[] = { 1915, 1700, 1519, 1432, 1275, 1136, 1014, 956 };
  
  for (int i = 0; i < 8; i++) {
    if (names[i] == note) {
      playTone(tones[i], duration);
    }
  }
}

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
  pinMode(pinLED1, OUTPUT);
  pinMode(pinLED2, OUTPUT);
  pinMode(PIN_BUZ, OUTPUT);
 
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  Firebase.reconnectWiFi(true);
 
  firebaseData.setBSSLBufferSize(1024, 1024);
  firebaseData.setResponseSize(1024);
  Firebase.setReadTimeout(firebaseData, 1000 * 60);
  Firebase.setwriteSizeLimit(firebaseData, "tiny");
}

int i = 0;
bool check_led1 = false;
bool check_led2 = false;

String last_state1 = "A1";
String last_state2 = "A1";
String current_state1; 
String current_state2;

String value1;
String value2;
bool check_Bus1_1 = false;
bool check_Bus2_1 = false;

String valBus1;
String valBus2;
int int_valBus1;
int int_valBus2;


String reset1;
String reset2;
String valSW1;
String valSW2;
bool check_state_1 = false;
bool check_state_2 = false;

void loop()
{
  
  //스위치1 누르면 값이 바뀜
  if (digitalRead(pinSW1)){
    delay(100);
    check_led1 = !check_led1; //true인지 false인지
    Firebase.getString(firebaseData, "A1_1");
    valBus1 = firebaseData.stringData();
    int_valBus1 = valBus1.toInt();
    
    if (check_led1) { //true일 경우 불을 키고 +1
      digitalWrite(pinLED1, true);
      int_valBus1 = int_valBus1 + 1;
    }
    else { //false일 경우 불을 끄고 -1
      digitalWrite(pinLED1, false);
      int_valBus1 = int_valBus1 - 1;
    }
    valBus1 = String(int_valBus1);
    Firebase.setString(firebaseData, "A1_1", valBus1);
  }
  
  //스위치2 누르면 값이 바뀜
  if (digitalRead(pinSW2)){
    delay(20);
      
    check_led2 = !check_led2; //true인지 false인지
    Firebase.getString(firebaseData, "A2_1");
    valBus2 = firebaseData.stringData();
    int_valBus2 = valBus2.toInt();
    
    if (check_led2) { //true일 경우 불을 키고 +1
      digitalWrite(pinLED2, true);
      int_valBus2 = int_valBus2 + 1;
    }
    else { //false일 경우 불을 끄고 -1
      digitalWrite(pinLED2, false);
      int_valBus2 = int_valBus2 - 1;
    }
    
    valBus2 = String(int_valBus2);
    Firebase.setString(firebaseData, "A2_1", valBus2);
    //valBus2 = "1";
    //Firebase.setString(firebaseData, "A2_1", valBus1);
  }



  //버스 위치값이 바뀌었는지 확인
  if (Firebase.getString(firebaseData, "Bus1_1")) 
  {
    current_state1 = firebaseData.stringData(); 
    Serial.print("Bus1_1의 위치 = ");
    Serial.println(current_state1); // 시리얼모니터에 값을 출력
  }
  else
  {
    Serial.println(firebaseData.errorReason()); // 에러 메시지 출력
  }    

  if (current_state1 != last_state1) {
    check_Bus1_1 = true;
  }

  last_state1 = current_state1;

  //버스 호출값이 0이 아닌지 확인
  Firebase.getString(firebaseData, "A1_1");
  valSW1 = firebaseData.stringData();
  if (valSW1 != "0"){
     check_state_1 = true;
  }
  
  //버스 위치값이 바뀌었고 그 값이 DA라면 버저가 울림
  if ((current_state1 == "DA") && check_Bus1_1 && check_state_1) 
  {
    for (int i = 0; i < length; i++) {
      if (notes[i] == ' ') {
        delay(beats[i] * tempo);
      } else {
        playNote(notes[i], beats[i] * tempo);
      }
        delay(tempo / 2); 
    }
  }

  //버스 위치값이 A1이면 초기화
  if ((current_state1 == "A1") && check_Bus1_1 && check_state_1)
    {
      reset1 = "0";
      check_led1 = false;
      Firebase.setString(firebaseData, "A1_1", reset1);
      digitalWrite(pinLED1, false);
    }

  check_Bus1_1 = false;
  check_state_1 = false;

  //버스 위치값이 바뀌었는지 확인
  if (Firebase.getString(firebaseData, "Bus2_1")) 
  {
    current_state2 = firebaseData.stringData(); 
    Serial.print("Bus2_1의 위치 = ");
    Serial.println(current_state2); // 시리얼모니터에 값을 출력
  }
  else
  {
    Serial.println(firebaseData.errorReason()); // 에러 메시지 출력
  }    

  if (current_state2 != last_state2) {
    check_Bus2_1 = true;
  }

  last_state2 = current_state2;

  //버스 호출값이 0이 아닌지 확인
  Firebase.getString(firebaseData, "A2_1");
  valSW2 = firebaseData.stringData();
  if (valSW2 != "0"){
     check_state_2 = true;
  }
  
  //버스 위치값이 바뀌었고 그 값이 BA라면 버저가 울림
  if ((current_state2 == "BA") && check_Bus2_1 && check_state_2) 
  {
    for (int i = 0; i < length; i++) {
      if (notes[i] == ' ') {
        delay(beats[i] * tempo);
      } else {
        playNote(notes[i], beats[i] * tempo);
      }
        delay(tempo / 2); 
    }
  }

    //버스 위치값이 A1이면 초기화
  if ((current_state2 == "A1") && check_Bus2_1 && check_state_2) 
    {
      reset2 = "0";
      check_led2 = false;
      Firebase.setString(firebaseData, "A2_1", reset2);
      digitalWrite(pinLED2, false);
    }
  
  check_Bus2_1 = false;
  check_state_2 = false;
}
