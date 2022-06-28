

//IR SENSOR
#define ADC_PORT_IR 0 //HEXT IR Sensor Board 번호
#define IR_delay 100 //대기자 유무 신호 지연 시간 = IR_delay * (  ) ms

//Fan
#define pinFAN  7

//CDS_Sensor
#define pinCDS_SEN A4 
#define pinCDS_LED1 5
#define pinCDS_LED2 6
#define MAX_PWM 255



//Fire_revision
#define pinTEMP A2  
#define pinBusBuzzer 8   //D1 보드에서 받는 부저 input 값
#define pinBuzzerCall 9  //UNO 보드에서 buzzer 센서로 내보내는 ouput 값
#define pinFire 11

int PORT_IR = ADC_PORT_IR; // 변수에 포트번호 할당
int PIN_IR; // IR 핀 선언

float TEMP = pinTEMP;
boolean Fire_State = true;

void setup() // wifi 접속 과정.
{
  Serial.begin(9600);
 
  
 
// IR_Sensor 셋업
  Serial.println();
  Serial.println("HexT IR Sensor Test");
  if (PIN_IR==0)
  {
    PIN_IR = A0;
  }
  else
  {
    Serial.println("Please connecting IR Sensor to ADC1 Port");
  }

// Fan 셋업
  pinMode(pinFAN, OUTPUT);

// Fire_revision 셋업
  pinMode(pinBusBuzzer, INPUT);
  pinMode(pinBuzzerCall, OUTPUT);
  pinMode(pinFire, OUTPUT);
  
}

// IR_Sensor 에 필요한 변수 선언
int IR_i = 0;
int IR_k = 0;
int IR_0 = 0;
bool IR_State = 0;

// LED 에 필요한 변수 선언
int LED_i = 0;

// Fan 에 필요한 변수 선언
bool FAN_LState = false;
bool FAN_CState = false;
bool fan = false;

//Buzzer 에 필요한 변수 선언
bool BusBuzzer  = false;
bool lBusBuzzer = false;





//버스 승객 buzzer loop
void Buzzer(){
  BusBuzzer = digitalRead(pinBusBuzzer);
  delay(20);
  if(BusBuzzer == lBusBuzzer){
    Serial.print("@@@@@@@@@@@@@@@@@@@@@@@@@@@@Buzzer pin :  ");
    Serial.println(digitalRead(pinBusBuzzer));
    noTone(pinBuzzerCall);
  }else{
    Serial.println("Buzzer Requested!!");
    tone(pinBuzzerCall, 400);
    delay(1000);
    noTone(pinBuzzerCall);
    delay(1000);
    tone(pinBuzzerCall, 400);
    delay(1000);
    noTone(pinBuzzerCall);    
  }
  lBusBuzzer = BusBuzzer;
}


//IR_Sensor loop
void IR_Sensor(){  
  int ValADC = analogRead(PIN_IR);
//  analogWrite(D13, 255);

  Serial.print("IR_Sensor: ");
  Serial.println(ValADC);

  if(IR_0 == 0){
  }else if(IR_0 + 120< ValADC && IR_k ==0){
    IR_k+= 1;
    IR_i = 0;
  }else if(IR_0 + 120< ValADC && IR_k > 0){
    IR_i = 0;
  }
  IR_0 = ValADC;
  
  IR_i += +1;

  if(IR_k == 0 || IR_i >= IR_delay){
    IR_State = 0;
  }
  else if(IR_k == 1 && 0 < IR_i <IR_delay){
    IR_State = 1;
  }

  if(IR_i >= IR_delay +1){
    IR_k = 0;
    IR_i = 0;
  }
  Serial.print("IR_State: ");
  Serial.println(IR_State);
}


//LED loop
void LED(){
  int valLIGHT1 = analogRead(pinCDS_SEN);
  if(500 > valLIGHT1){
    digitalWrite(pinCDS_LED1, LOW);
    digitalWrite(pinCDS_LED2, LOW);
  }else if(600 > valLIGHT1){
  digitalWrite(pinCDS_LED1, HIGH);
   digitalWrite(pinCDS_LED2, LOW);
   analogWrite(pinCDS_LED1, LED_i= 100);  // 밝기 조금 덜 밝음
   }
   else if(IR_State == 1) // 600 이상 LED 모두 점등
  {
   digitalWrite(pinCDS_LED1, HIGH);
   digitalWrite(pinCDS_LED2, HIGH);
   analogWrite(pinCDS_LED1, LED_i= MAX_PWM); // 최대 밝기
   analogWrite(pinCDS_LED2, LED_i= MAX_PWM);
  }
  Serial.print("CDS Value: ");
  Serial.println(valLIGHT1);
}


//Fan loop
void FAN(){
  FAN_CState = IR_State ;

  if(FAN_CState ==0){
    fan = true;
    
  }
  else{
    fan = false;
  }
  digitalWrite(pinFAN, fan);
//  FAN_LState = FAN_CState; // last State 적외선 센서 1 : update
}


//Fire_revision loop
void Fire_revision(){
  float valTemp = analogRead(pinTEMP);
//  valTemp = valTemp *330 / 1023-50;
    valTemp = valTemp*0.8 -100;
  
//  Firebase.setInt(firebaseData, "Temp", valTemp);

  if(valTemp > 32){
    Fire_State = true;
    tone(pinBuzzerCall, 800);
//    Firebase.setInt(firebaseData, "Fire Situation", Fire_State);
    digitalWrite(pinFire, HIGH);
  }
  else{
    Fire_State = false;
    noTone(pinBuzzerCall);
    //Firebase.setInt(firebaseData, "Fire Situation", Fire_State);
    digitalWrite(pinFire, LOW);
  }
  Serial.print("Temp Value: ");
  Serial.println(valTemp);
  delay(10);
}

void loop(){
  Buzzer();
  IR_Sensor();
  LED();
  FAN();
  Fire_revision();
  delay(500);
}
