#define _TIMERINTERRUPT_LOGLEVEL_     4
#define USE_WIFI_NINA         false
#define USE_WIFI101           true



#include <WiFiWebServer.h>
#include <WiFiHttpClient.h>
#include <ArduinoJson.h>
#include <SAMDTimerInterrupt.h>
//#include <ArduinoHttpClient.h>

int status = WL_IDLE_STATUS;
const char ssid[] = "linmo111";
const char pass[] = "20030807";
char serverAddress[] = "192.168.43.196";  // server address
int port = 8080;
WiFiClient           client;
WiFiWebSocketClient  wsClient(client, serverAddress, port);


int count = 0;
//StaticJsonBuffer<300> JSONBuffer;

const byte CFPin=1;
const byte IRPin=2;
const byte RFPin=13;
//const byte MGPin=A0;



SAMDTimer RadioTimer(TIMER_TC3);
SAMDTimer IRTimer(TIMER_TC4);
//SAMDTimer MagneticTimer(TIMER_TC5);
SAMDTimer CarrierFreqTimer(TIMER_TCC);
//SAMDTimer WSTimer(TIMER_TCC1);
SAMDTimer WSsendTimer(TIMER_TCC2);

bool IR_active=false;
int IR_count=0;
int IR_timeCount=0;
bool RF_active=false;
int RF_count=0;
int RF_timeCount=0;
//int MG_count=0;
//int MG_total=0;
int CF_count=0;
int CF_total=0;
int CF_TrueCount=0;

volatile int LED=0;
volatile int MotorState=0;
volatile double CarrierFreq=0;
volatile double Infrared=0;
//volatile double Magnetic=0;
volatile double Radio=0;
volatile bool CarrierFreqChanged=0;
volatile bool InfraredChanged=0;
//volatile bool MagneticChanged=0;
volatile bool RadioChanged=0;

const int MotorLIN1= 9;
const int MotorLIN2= 8;
const int MotorRIN1= 11;
const int MotorRIN2= 12;




void ledON()
{
  digitalWrite(LED_BUILTIN,1);

}
void ledOFF()
{
  digitalWrite(LED_BUILTIN,0);

}

void Forward(){
   digitalWrite(MotorLIN1,LOW);
   digitalWrite(MotorLIN2,HIGH);
   digitalWrite(MotorRIN1,LOW);
   digitalWrite(MotorRIN2,HIGH);
  
}
void Backward(){
  digitalWrite(MotorRIN1,HIGH);
  digitalWrite(MotorRIN2,LOW);
  digitalWrite(MotorLIN1,HIGH);
  digitalWrite(MotorLIN2,LOW);

  
}
void Stop(){
   digitalWrite(MotorRIN1,LOW);
   digitalWrite(MotorRIN2,LOW);
   digitalWrite(MotorLIN1,LOW);
   digitalWrite(MotorLIN2,LOW);
  
}

void Left(){
   digitalWrite(MotorRIN1,HIGH);
   digitalWrite(MotorRIN2,LOW);
   digitalWrite(MotorLIN1,LOW);
   digitalWrite(MotorLIN2,HIGH);
  
}
void Right(){
   digitalWrite(MotorRIN1,LOW);
   digitalWrite(MotorRIN2,HIGH);
   digitalWrite(MotorLIN1,HIGH);
   digitalWrite(MotorLIN2,LOW);
  
}

void LEDControl(){

  if(LED==0){
    ledOFF();
  }
  else if(LED==1){
    ledON();
  }
}
void MotorControl(){
 
  if(MotorState==0){
    Stop();
  }
  else if(MotorState==1){
    Forward();
  }
  else if(MotorState==2){
    Backward();
  }
   else if(MotorState==3){
    Left();
  }
   else if(MotorState==4){
    Right();
    
  }
}

void IRfound(){
  IR_active=true;
}
void RFfound(){
  RF_active=true;
}

void UpdateValues(JsonObject parsed){
  LED=parsed["LED"];
  MotorState=parsed["MotorState"];

}
JsonObject CreateJson(){
  DynamicJsonDocument jBuffer(1024);
//  DynamicJsonBuffer jBuffer;
  JsonObject root=jBuffer.createNestedObject();
  root["Infrared"]=Infrared;
  root["Radio"]=Radio;
  //root["Magnetic"]=Magnetic;
  root["CarrierFreq"]=CarrierFreq;
  return root;
}




void RadioTimerHandler()
{
   if(RF_active){
    RF_count++;
    RF_active=false;
  }
  RF_timeCount++;
  if(RF_timeCount==1000){
    RF_timeCount=0;
    Radio=RF_count-4;
    RF_count=0;
    RadioChanged=1;
  }
  
  //Radio+=1;
  //RadioChanged=1;
  
  
}
void IRTimerHandler(){
  if(IR_active){
    IR_count++;
    IR_active=false;
  }
  IR_timeCount++;
  if(IR_timeCount==1000){
    IR_timeCount=0;
    Infrared=IR_count-4;
    IR_count=0;
    InfraredChanged=1;
  }
  
  //Infrared+=1;
  //InfraredChanged=1;
}
/*
void MagneticTimerHandler(){
  MG_count++;
  MG_total+=analogRead(MGPin);
  if (MG_count==500){
    Magnetic=MG_total/500;
    MG_total=0;
    MagneticChanged=1;
    MG_count=0;
    
  }
  

  
 // Magnetic+=1;
  //MagneticChanged=1;
  
}*/
/*
void CarrierFreqTimerHandler(){
  CF_count++;
  
  CF_total+=analogRead(CFPin);
  //CF_TrueCount++;
  
  
  if (CF_count==500){
   // if(CF_TrueCount==0){
   //    CF_TrueCount=1;
 //    }
//  CarrierFreq=CF_total/CF_TrueCount;
 CarrierFreq=CF_total/CF_count;
  //  CF_TrueCount=0;
    CF_total=0;
    CarrierFreqChanged=1;
    CF_count=0;
    
  }
  //Ultrasonic+=1;
  //UltrasonicChanged=1;
}
*/
bool CF_active=false;
int CF_timeCount=0;

void CFFound(){
  CF_active=true;
}
void CarrierFreqTimerHandler(){
  if(CF_active){
    CF_count++;
    CF_active=false;
  }
  CF_timeCount++;
  if(CF_timeCount==1000){
    CF_timeCount=0;
   CarrierFreq=CF_count-4;
    CF_count=0;
    CarrierFreqChanged=1;
  }
  
}
void WSTimerHandler(){
  if(wsClient.connected()){
    //Serial.println("recieving");
  int messageSize = wsClient.parseMessage();
  if (messageSize > 0) {
    Serial.println("Received a message:");
    DynamicJsonDocument parsed(300);
     // JsonObject parsed;
      String recieved=wsClient.readString();
      DeserializationError error = deserializeJson(parsed, recieved);
      if (error){
         return;}
      JsonObject root = parsed.as<JsonObject>();

      
      UpdateValues(root);
      
      Serial.println(recieved);
      Serial.println(LED);
      LEDControl();
      MotorControl();
    
  }
  }
  else{
    //Serial.println("wsClient Not Connected");
  }
  
}
void WSsendTimerHandler(){
  if(RadioChanged && InfraredChanged && CarrierFreqChanged){
    //Serial.println("sending a message");
    
    JsonObject JsonMessage=CreateJson();
    String output;
    serializeJson(JsonMessage, output);
    wsClient.beginMessage(TYPE_TEXT);
  
 
    wsClient.print(output);
   // wsClient.print("1");
    
    wsClient.endMessage();
    RadioChanged=0;
    //MagneticChanged=0;
    CarrierFreqChanged=0;
    InfraredChanged=0;
   // Serial.println("sent a message"+output);
    }
  
}









void setup() {
  Serial.begin(115200);
  pinMode(IRPin, INPUT_PULLUP);
  pinMode(RFPin, INPUT_PULLUP);
 // pinMode(MGPin, INPUT);
  pinMode(CFPin, INPUT_PULLUP);
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(MotorRIN1, OUTPUT);
  pinMode(MotorRIN2, OUTPUT);
  pinMode(MotorLIN1, OUTPUT);
  pinMode(MotorLIN2, OUTPUT);
  while (!Serial && millis() < 5000);

  Serial.print(F("\nStarting SimpleWebSocket ")); 

  Stop();
  Serial.print(F("Connecting to SSID: "));
  Serial.println(ssid);
  
  status = WiFi.begin(ssid, pass);
    while ( status != WL_CONNECTED) {
      
      status=WiFi.begin(ssid, pass);
      Serial.print("Attempting to connect to Network named: ");
      Serial.println(ssid);                   // print the network name (SSID);
      delay(1000);
    }
  
  Serial.println("wifi connected"); 
  
  

  Serial.println("Starting WebSocket client");
  
  wsClient.begin();
  RadioTimer.attachInterruptInterval_MS(1,RadioTimerHandler);
  IRTimer.attachInterruptInterval_MS(1,IRTimerHandler);
  //MagneticTimer.attachInterruptInterval_MS(1,MagneticTimerHandler);
  CarrierFreqTimer.attachInterruptInterval_MS(1,CarrierFreqTimerHandler);
 // WSTimer.attachInterruptInterval_MS(50,WSTimerHandler);
  WSsendTimer.attachInterruptInterval_MS(1000,WSsendTimerHandler);
  attachInterrupt(digitalPinToInterrupt(IRPin), IRfound, FALLING);
  attachInterrupt(digitalPinToInterrupt(RFPin), RFfound, FALLING);
  attachInterrupt(digitalPinToInterrupt(CFPin), CFFound, FALLING);
  

}




void loop() {
  if(wsClient.connected()){
    //Serial.println("recieving");
  int messageSize = wsClient.parseMessage();
  if (messageSize > 0) {
    Serial.println("Received a message:");
    DynamicJsonDocument parsed(300);
     // JsonObject parsed;
      String recieved=wsClient.readString();
      DeserializationError error = deserializeJson(parsed, recieved);
      if (error){
         return;}
      JsonObject root = parsed.as<JsonObject>();

      
      UpdateValues(root);
      
      Serial.println(recieved);
      Serial.println(LED);
      LEDControl();
      MotorControl();
    
  }
  wsClient.flush();
  }
  else{
   // wsClient.begin();
  }

}
