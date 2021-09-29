

#include<TinyGPS++.h>
#include<SoftwareSerial.h>

#define shake_pin 9  //定义震动器，连接在D2
#define alarm  6     //定义轰鸣器，连接在D6
#define RXPin 4      //定义GPS的RX接口
#define TXPin 3      //定义GPS的TX接口
unsigned long lastMs=0;
unsigned long lastMs2=0;
unsigned long lastMs3=0;
unsigned long lastMs4=0;
boolean shake=0;
boolean shake_last=0;
int ForcedAlarmSwitch=0;

double lattitude; 
double lattitude_last;
double longtitude_last;//定义经纬度信息
double longtitude;
TinyGPSPlus gps;
SoftwareSerial ss(RXPin,TXPin);


void set(){
  boolean shake=digitalRead(shake_pin);

  if(ForcedAlarmSwitch==2){
    digitalWrite(alarm,LOW);
    return;
  }
  if(shake==1&ForcedAlarmSwitch!=2)   ForcedAlarmSwitch=1;
  //if(shake==0&&ForcedAlarmSwitch!=2)   ForcedAlarmSwitch=0;
  
  if(ForcedAlarmSwitch==0||ForcedAlarmSwitch==2){
    digitalWrite(alarm,LOW);
  }
  if(ForcedAlarmSwitch==1&&millis()-lastMs2<=20000){
    digitalWrite(alarm,HIGH);
    delay(250);
    digitalWrite(alarm,LOW);
    delay(250);
  }                                       
  else{
    lastMs2=millis();
    digitalWrite(alarm,LOW);
    ForcedAlarmSwitch=0;
  }
  
}


void GPS(){
  while(ss.available()>0){
    if(gps.encode(ss.read())){
      if(gps.location.isValid()){
        lattitude=gps.location.lat();
        longtitude=gps.location.lng();
      }
    }
  }
}


void GPS_send(){        //每隔5秒判断一次位置，如果位置发生变化就上传位置
  if(millis()-lastMs4>5000){
  if(lattitude!=lattitude_last||longtitude!=longtitude_last){
    Serial.println("AT+SEND=1,30,git:"+String(longtitude,6)+'#'+"lat:"+String(lattitude,6)+'#');
    lattitude_last=lattitude;
    longtitude_last=longtitude;
  }
  lastMs4=millis();
 
 }
}


void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  ss.begin(9600);
  pinMode(shake_pin,INPUT);
  pinMode(alarm,OUTPUT);
  
}


void loop() {
 // rcevLoRa();

   if(Serial.available()){
    String incomingString=Serial.readString();
   // Serial.println(incomingString);
  for(int i=0;i<=incomingString.length();i++){
    if(incomingString[i]=='d'&&incomingString[i+1]==':'){   //发送来的信息AT+SEND=2,21,Forced:2
      ForcedAlarmSwitch=incomingString[i+2]-48;
      Serial.println("FORCEDALARMSW");
      Serial.println(ForcedAlarmSwitch);
      if(ForcedAlarmSwitch==2){
        digitalWrite(alarm,HIGH);
        delay(100);
        digitalWrite(alarm,LOW);
        delay(50);
        digitalWrite(alarm,HIGH);
        delay(100);
        digitalWrite(alarm,LOW);
      }
    }
  }
  }
  
  set();
  GPS();
  GPS_send();
    if(ForcedAlarmSwitch!=shake_last){
      if(ForcedAlarmSwitch==1) Serial.println("AT+SEND=1,21,shake:1#");
      else if(ForcedAlarmSwitch==0)  Serial.println("AT+SEND=1,21,shake:0#");
      shake_last=ForcedAlarmSwitch;
    }  
}
    
