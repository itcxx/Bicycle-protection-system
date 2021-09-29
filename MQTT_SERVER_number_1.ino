//https://a1209i5zgasxeggi.vapp.cloudhost.link/preview/page/334599?token=9ecafd880e920c3ef700c1f7cde5f4b4


#include<ESP8266WiFi.h>
#include<PubSubClient.h>
#include<ESP8266HTTPClient.h>
#define MQTT_MAX_PACKT_SIZE 1028
#define MQTT_KEEPALIVE 61
#define ALINK_TOPIC_PROP_POST     "/sys/a1JeohhEkQj/number_1/thing/event/property/post"
#define ALINK_BODY_FORMAT         "{\"id\":\"123\",\"version\":\"1.0\",\"method\":\"thing.event.property.post\",\"params\":%s}"
#define ALINK_TOPIC_PROP_SET        "/sys/a1JeohhEkQj/number_1/thing/event/property/post_reply"

unsigned long lastMs = 0;
const char* ssid="xxxx";
const char* password="xxxx";
const char* mqttServer="a1JeohhEkQj.iot-as-mqtt.cn-shanghai.aliyuncs.com";
const int mqttPort=1883;
const char* mqttUser="xxxxxxxxxx";
const char* mqttPassword="xxxxxxxxxxxxxxx";

char shake='0';           //保存震动器的信息，1为震动0为禁止
int ForcedAlarmSwitch=0;   //轰鸣器的信息，0,1,2，关，开，禁言

WiFiClient espClient;
PubSubClient client(espClient);



//发送消息
void mqttIntervalPost(){
  char param[32];
  char jsonBuf[128];
  if(shake=='1'&&ForcedAlarmSwitch!=2){  //判断震动器为震动且轰鸣器不是禁言状态
    ForcedAlarmSwitch=1;
  }
  if(shake=='0'&&ForcedAlarmSwitch!=2){     //震动器为禁止时轰鸣器为0；
    ForcedAlarmSwitch=0;
  }


  sprintf(param,"{\"ForcedAlarmSwitch\":%d,\"ActPos\":%d}",ForcedAlarmSwitch,2454655);
  sprintf(jsonBuf,ALINK_BODY_FORMAT,param);
 // Serial.println(jsonBuf);
  boolean d=client.publish(ALINK_TOPIC_PROP_POST,jsonBuf);   //发布
  
  client.subscribe(ALINK_TOPIC_PROP_SET);                     //订阅
 // Serial.print("publish:0 fail; 1 succesful");
 // Serial.println(d);
  
}
//接受消息


void callback(char *topic, byte *payload, unsigned int length)
{
    String jsonBuf;
    jsonBuf=(String)(char*)payload;

    for(int i=0;i<=length;i++){
      if(jsonBuf[i]=='i'&&jsonBuf[i+1]=='t'&&jsonBuf[i+2]=='c'&&jsonBuf[i+3]=='h'){  //搜索出ForceAlarmSwitch后的值
       if(jsonBuf[i+6]=='2') {ForcedAlarmSwitch=2;
        Serial.println("AT+SEND=2,21,2");  //把值2发送给设备address 2;
        Serial.println("AT+SEND=2,21,2");
        Serial.println("AT+SEND=2,21,2");
    
      }
      if(jsonBuf[i+6]=='1') {ForcedAlarmSwitch=1;
        Serial.println("AT+SEND=2,21,1");  //把值1发送给设备address 2;
        Serial.println("AT+SEND=2,21,1");
        Serial.println("AT+SEND=2,21,1");
    
      }
      if(jsonBuf[i+6]=='0') {ForcedAlarmSwitch=0;
        Serial.println("AT+SEND=2,21,0");  //把值0发送给设备address 2;
        Serial.println("AT+SEND=2,21,0");
        Serial.println("AT+SEND=2,21,0");
    
      }
       
      

}
}
}




//确认mqtt是否断开，断开重新连接

void mqttCheckConnect()
{
    while (!client.connected())
    {
        //Serial.println("Connecting to MQTT Server ...");
 
        if (client.connect("chuxin|securemode=3,timestamp=999,signmethod=hmacsha1|",mqttUser,mqttPassword))

        {

          //  Serial.println("MQTT Connected!");

        }
        else
        {
           // Serial.print("MQTT Connect err:");
          //  Serial.println(client.state());
            delay(1000);
        }
    }
}





void setup() {
  // put your setup code here, to run once:
  lastMs=millis();
  Serial.begin(115200);
  WiFi.begin(ssid,password);
  while(WiFi.status()!=WL_CONNECTED){
    delay(500);
    //Serial.println("Connecting to WIFI..");
    
  }
 // Serial.println("Connected to the WiFi network");
  
  client.setServer(mqttServer,mqttPort);     //Mqtt服务器连接
  client.setCallback(callback);             //回调函数

  
  while(!client.connected()){
   // Serial.println("connecting to MQTT..");
    if(client.connect("chuxin|securemode=3,timestamp=999,signmethod=hmacsha1|",mqttUser,mqttPassword)){
     // Serial.println("connected");
    }
    else{
     // Serial.print("failed with state");
     // Serial.print(client.state());
      //delay(2000);
    }
  }


}

void loop() {
  
       
       //rcevLoRa();
       if(Serial.available()){
    String incomingString=Serial.readString();
    Serial.println(incomingString);
    if(incomingString[1]=='R'&&incomingString[2]=='C'){   //判断是否有数据接收
      shake=incomingString[10];        //接受震动器的状态信息
      Serial.print("shake=");
      Serial.println(shake);
      
    }
  }
       client.loop();
       
        if(millis()-lastMs>=10000){
        mqttCheckConnect();
        mqttIntervalPost();
        //mqttIntervalSet();
        lastMs=millis();

        
        }

       
      
}
