#include <WiFi.h>
#include <PubSubClient.h>
#include "time.h"

const char* ntpServer = "time.windows.com"; //NTP主機位址
const long  gmtOffset_sec = 28800; //時區（以秒為單位） 8*60*60  https://time.artjoey.com/
const int   daylightOffset_sec = 0; //定義夏令時的偏移量（以秒為單位）
char timeStringBuff[50];    //時間


unsigned long int previous_millis_A = 0; //之前時間
unsigned long int interval_A =  1 * 1000; //間隔時間1s

unsigned long int previous_millis_B = 0; //之前時間
unsigned long int interval_B =  10 * 1000; //間隔時間10s

unsigned long int previous_millis_C = 0; //之前時間
unsigned long int interval_C =  5 * 1000; //間隔時間5s

int counter = 0, client2counter = 0, client4counter = 0, conterrs = 0, conterrs2 = 0, conterrs3 = 0, conterrs4 = 0, conterrs5 = 0, conterrs6 = 0 ; //WIFI,client2,client4計時10秒初始化用,conterrs 继电器初始化


//要连接的WiFi的名字和密码
const char * wifi_network_ssid = "wifi ssid";
const char * wifi_network_password = "password";
//要esp32创建的ap的名字和密码
const char * soft_ap_ssid = "ap ssid";
const char * soft_ap_password = "password";


const IPAddress serverIP2(192, 168, 4, 2); //relay ch2 双路WiFirelay的IP地址
const IPAddress serverIP5(192, 168, 4, 10); //relay ch4  4路WiFirelay的IP地址

uint16_t serverPort = 8080; //relay端口号

WiFiClient client2; //訪問客戶端連接
WiFiClient client4; //訪問客戶端連接

// MQTT Broker
const char *mqtt_broker = "broker.emqx.io";
//const char *mqtt_broker = "broker.hivemq.com";//有限制
//const char *mqtt_broker = "test.mosquitto.org";//可以用
//const char *mqtt_broker = "mqtt.eclipseprojects.io";//有问题的


const char *mqtt_username = "esp32relay";
const char *mqtt_password = "relayesp32";
const int mqtt_port = 1883;

String message;


#define sub_esprestart "esp32restart" //订阅mqtt dash里on tap的重启esp32的指令

#define sub_appcontrolrelay "esp32MQTTrelay"//订阅mqtt dash手动控制开关的指令

//定时器时间并赋予初始化显示内容
String time1="Please Set";
String time2="Please Set";
String time3="Please Set";
String time4="Please Set";
String time5="Please Set";
String time6="Please Set";
String time7="Please Set";
String time8="Please Set";
String time9="Please Set";
String time10="Please Set";


String time1a;
String time2a;
String time3a;
String time4a;
String time5a;
String time6a;
String time7a;
String time8a;
String time9a;
String time10a;




int timeswitch1 = 0;
int timeswitch2 = 0;
int timeswitch3 = 0;
int timeswitch4 = 0;
int timeswitch5 = 0;
int timeswitch6 = 0;
int timeswitch7 = 0;
int timeswitch8 = 0;
int timeswitch9 = 0;
int timeswitch10 = 0;

//定时器执行选项并赋予初始化显示内容
String timer_option1="Please Select";
String timer_option2="Please Select";
String timer_option3="Please Select";
String timer_option4="Please Select";
String timer_option5="Please Select";
String timer_option6="Please Select";
String timer_option7="Please Select";
String timer_option8="Please Select";
String timer_option9="Please Select";
String timer_option10="Please Select";

//订阅定时器时间设置
#define sub_TIMER1_RELAY "esp32relaytimer1"
#define sub_TIMER2_RELAY "esp32relaytimer2"
#define sub_TIMER3_RELAY "esp32relaytimer3"
#define sub_TIMER4_RELAY "esp32relaytimer4"
#define sub_TIMER5_RELAY "esp32relaytimer5"
#define sub_TIMER6_RELAY "esp32relaytimer6"
#define sub_TIMER7_RELAY "esp32relaytimer7"
#define sub_TIMER8_RELAY "esp32relaytimer8"
#define sub_TIMER9_RELAY "esp32relaytimer9"
#define sub_TIMER10_RELAY "esp32relaytimer10"

//订阅定时器开或关
#define sub_TIMER1_RELAY_onoff "esp32relaytimer1_on/off"
#define sub_TIMER2_RELAY_onoff "esp32relaytimer2_on/off"
#define sub_TIMER3_RELAY_onoff "esp32relaytimer3_on/off"
#define sub_TIMER4_RELAY_onoff "esp32relaytimer4_on/off"
#define sub_TIMER5_RELAY_onoff "esp32relaytimer5_on/off"
#define sub_TIMER6_RELAY_onoff "esp32relaytimer6_on/off"
#define sub_TIMER7_RELAY_onoff "esp32relaytimer7_on/off"
#define sub_TIMER8_RELAY_onoff "esp32relaytimer8_on/off"
#define sub_TIMER9_RELAY_onoff "esp32relaytimer9_on/off"
#define sub_TIMER10_RELAY_onoff "esp32relaytimer10_on/off"

//订阅定时器要执行的选项
#define sub_TIMER_option1 "esp32timeroption1"
#define sub_TIMER_option2 "esp32timeroption2"
#define sub_TIMER_option3 "esp32timeroption3"
#define sub_TIMER_option4 "esp32timeroption4"
#define sub_TIMER_option5 "esp32timeroption5"
#define sub_TIMER_option6 "esp32timeroption6"
#define sub_TIMER_option7 "esp32timeroption7"
#define sub_TIMER_option8 "esp32timeroption8"
#define sub_TIMER_option9 "esp32timeroption9"
#define sub_TIMER_option10 "esp32timeroption10"



int timeswitchloop1;

String clientId = "ESP32Client-"; // Create a random client ID

WiFiClient espClient;
PubSubClient client(espClient);

const int b1 = 4;     // 按鈕的引腳
const int b2 = 16;    // 按鈕的引腳
const int b3 = 17;    // 按鈕的引腳
const int b4 = 5;     // 按鈕的引腳
const int b5 = 18;    // 按鈕的引腳
const int b6 = 19;    // 按鈕的引腳

int buttonStateb1;             // 輸入按鈕的當前讀數
int buttonStateb2;             // 輸入按鈕的當前讀數
int buttonStateb3;             // 輸入按鈕的當前讀數
int buttonStateb4;             // 輸入按鈕的當前讀數
int buttonStateb5;             // 輸入按鈕的當前讀數
int buttonStateb6;             // 輸入按鈕的當前讀數

int lastButtonStateb1 = LOW;   // 輸入按鈕的初始讀數
int lastButtonStateb2 = LOW;   // 輸入按鈕的初始讀數
int lastButtonStateb3 = LOW;   // 輸入按鈕的初始讀數
int lastButtonStateb4 = LOW;   // 輸入按鈕的初始讀數
int lastButtonStateb5 = LOW;   // 輸入按鈕的初始讀數
int lastButtonStateb6 = LOW;   // 輸入按鈕的初始讀數

unsigned long lastDebounceTimeb1 = 0;  //bounce的先前讀數
unsigned long lastDebounceTimeb2 = 0;  //bounce的先前讀數
unsigned long lastDebounceTimeb3 = 0;  //bounce的先前讀數
unsigned long lastDebounceTimeb4 = 0;  //bounce的先前讀數
unsigned long lastDebounceTimeb5 = 0;  //bounce的先前讀數
unsigned long lastDebounceTimeb6 = 0;  //bounce的先前讀數

unsigned long debounceDelay = 50;    // bounce去抖時間

int ch2r1 = 0, ch2r2 = 0, ch4r1 = 0, ch4r2 = 0, ch4r3 = 0, ch4r4 = 0; //初始化

char s; //create variable
String t;


//WiFi relay的hex控制指令
byte rel1ON[] = {
  0xA0,
  0x01,
  0x01,
  0xA2
};
//open relay1
byte rel1OFF[] = {
  0xA0,
  0x01,
  0x00,
  0xA1
};

//close relay2
byte rel2ON[] = {
  0xA0,
  0x02,
  0x01,
  0xA3
};
//open relay2
byte rel2OFF[] = {
  0xA0,
  0x02,
  0x00,
  0xA2
};

//close relay3
byte rel3ON[] = {
  0xA0,
  0x03,
  0x01,
  0xA4
};
//open relay3
byte rel3OFF[] = {
  0xA0,
  0x03,
  0x00,
  0xA3
};

//close relay4
byte rel4ON[] = {
  0xA0,
  0x04,
  0x01,
  0xA5
};
//open relay4
byte rel4OFF[] = {
  0xA0,
  0x04,
  0x00,
  0xA4
};

void setup() {


  // initialize the pushbutton pin as an pull-up input
  pinMode(b1, INPUT_PULLUP);
  pinMode(b2, INPUT_PULLUP);
  pinMode(b3, INPUT_PULLUP);
  pinMode(b4, INPUT_PULLUP);
  pinMode(b5, INPUT_PULLUP);
  pinMode(b6, INPUT_PULLUP);


  Serial.begin(115200);

  // put your setup code here, to run once:
  pinMode(2, OUTPUT); //内置LED

  WiFi.mode(WIFI_MODE_APSTA);

  WiFi.softAP(soft_ap_ssid, soft_ap_password);
  WiFi.begin(wifi_network_ssid, wifi_network_password);

  wifireconnect();      //WiFi断线esp32重启


  // 初始化並獲取時間
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);

  Serial.print("ESP32 IP as soft AP: ");
  Serial.println(WiFi.softAPIP());

  Serial.print("ESP32 IP on the WiFi network: ");
  Serial.println(WiFi.localIP());



  //connecting to a mqtt broker
  client.setServer(mqtt_broker, mqtt_port);
  client.setCallback(callback);
  while (!client.connected()) {
    clientId += String(random(0xffff), HEX);
    Serial.println(clientId);
    Serial.println("Connecting to public emqx mqtt broker.....");

    if (client.connect(clientId.c_str(), mqtt_username, mqtt_password)) {
      Serial.println("Public emqx mqtt broker connected");
    } else {
      Serial.print("failed with state ");
      Serial.print(client.state());
      delay(2000);
    }
    clientId="";
  }
  
  // publish and subscribe all topic
  client.publish("startdisplay", "esp32 start");

  client.subscribe(sub_esprestart);
  
  client.subscribe(sub_appcontrolrelay);


  client.subscribe(sub_TIMER1_RELAY);
  client.subscribe(sub_TIMER2_RELAY);
  client.subscribe(sub_TIMER3_RELAY);
  client.subscribe(sub_TIMER4_RELAY);
  client.subscribe(sub_TIMER5_RELAY);
  client.subscribe(sub_TIMER6_RELAY);
  client.subscribe(sub_TIMER7_RELAY);
  client.subscribe(sub_TIMER8_RELAY);
  client.subscribe(sub_TIMER9_RELAY);
  client.subscribe(sub_TIMER10_RELAY);



  client.subscribe(sub_TIMER1_RELAY_onoff);
  client.subscribe(sub_TIMER2_RELAY_onoff);
  client.subscribe(sub_TIMER3_RELAY_onoff);
  client.subscribe(sub_TIMER4_RELAY_onoff);
  client.subscribe(sub_TIMER5_RELAY_onoff);
  client.subscribe(sub_TIMER6_RELAY_onoff);
  client.subscribe(sub_TIMER7_RELAY_onoff);
  client.subscribe(sub_TIMER8_RELAY_onoff);
  client.subscribe(sub_TIMER9_RELAY_onoff);
  client.subscribe(sub_TIMER10_RELAY_onoff);



  client.subscribe(sub_TIMER_option1);
  client.subscribe(sub_TIMER_option2);
  client.subscribe(sub_TIMER_option3);
  client.subscribe(sub_TIMER_option4);
  client.subscribe(sub_TIMER_option5);
  client.subscribe(sub_TIMER_option6);
  client.subscribe(sub_TIMER_option7);
  client.subscribe(sub_TIMER_option8);
  client.subscribe(sub_TIMER_option9);
  client.subscribe(sub_TIMER_option10);



  client.publish("esp32relaytimer1_return","Please Set");
  client.publish("esp32relaytimer2_return","Please Set");
  client.publish("esp32relaytimer3_return","Please Set");
  client.publish("esp32relaytimer4_return","Please Set");
  client.publish("esp32relaytimer5_return","Please Set");
  client.publish("esp32relaytimer6_return","Please Set");
  client.publish("esp32relaytimer7_return","Please Set");
  client.publish("esp32relaytimer8_return","Please Set");
  client.publish("esp32relaytimer9_return","Please Set");
  client.publish("esp32relaytimer10_return","Please Set");
 


  client.publish("esp32timeroption1reset", "Please Select");
  client.publish("esp32timeroption2reset", "Please Select");
  client.publish("esp32timeroption3reset", "Please Select");
  client.publish("esp32timeroption4reset", "Please Select");
  client.publish("esp32timeroption5reset", "Please Select");
  client.publish("esp32timeroption6reset", "Please Select");
  client.publish("esp32timeroption7reset", "Please Select");
  client.publish("esp32timeroption8reset", "Please Select");
  client.publish("esp32timeroption9reset", "Please Select");;
  client.publish("esp32timeroption10reset", "Please Select");

  
  client.publish("esp32relaytimer1_on/offreset", "0");
  client.publish("esp32relaytimer2_on/offreset", "0");
  client.publish("esp32relaytimer3_on/offreset", "0");
  client.publish("esp32relaytimer4_on/offreset", "0");  
  client.publish("esp32relaytimer5_on/offreset", "0"); 
  client.publish("esp32relaytimer6_on/offreset", "0");
  client.publish("esp32relaytimer7_on/offreset", "0");
  client.publish("esp32relaytimer8_on/offreset", "0");
  client.publish("esp32relaytimer9_on/offreset", "0");  
  client.publish("esp32relaytimer10_on/offreset", "0"); 
  
   
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived in topic: ");
  Serial.println(topic);
  Serial.print("Message:");
  for (int i = 0; i < length; i++) {
    message = message + (char) payload[i];  // convert *byte to string
  }
  Serial.print(message);

  if (String(topic) == sub_esprestart) {
    if (message == "r") {
      Serial.print(message);
      ESP.restart();
    }
  }


  if (String(topic) == sub_appcontrolrelay) {
    if (message == "a") { //ch2r1 on
      led();
      s = 'a'; //为是赋值
      detect_s(s);//调用检测s，执行继电器开关hex指令
    }
    if (message == "b") {  // ch2r2 off
      led();
      s = 'b';//为是赋值
      detect_s(s);//调用检测s，执行继电器开关hex指令
    }


    if (message == "c") { //ch2r2 on
      led();
      s = 'c'; //为是赋值
      detect_s(s);//调用检测s，执行继电器开关hex指令
    }
    if (message == "d") {  // ch2r2 off
      led();
      s = 'd';//为是赋值
      detect_s(s);//调用检测s，执行继电器开关hex指令
    }


    if (message == "e") { //ch4r1 on
      led();
      s = 'e'; //为是赋值
      detect_s(s);//调用检测s，执行继电器开关hex指令
    }
    if (message == "f") {  // ch4r1 off
      led();
      s = 'f';//为是赋值
      detect_s(s);//调用检测s，执行继电器开关hex指令
    }


    if (message == "g") { //ch4r2 on
      led();
      s = 'g'; //为是赋值
      detect_s(s);//调用检测s，执行继电器开关hex指令
    }
    if (message == "h") {  // ch4r2 off
      led();
      s = 'h';//为是赋值
      detect_s(s);//调用检测s，执行继电器开关hex指令;
    }


    if (message == "i") { //ch4r3 on
      led();
      s = 'i'; //为是赋值
      detect_s(s);//调用检测s，执行继电器开关hex指令

    }
    if (message == "j") {  // ch4r3 off
      led();
      s = 'j';//为是赋值
      detect_s(s);//调用检测s，执行继电器开关hex指令

    }


    if (message == "k") { //ch4r4 on
      led();
      s = 'k'; //为是赋值
      detect_s(s);//调用检测s，执行继电器开关hex指令

    }
    if (message == "l") {  // ch4r4 off
      led();
      s = 'l';//为是赋值
      detect_s(s);//调用检测s，执行继电器开关hex指令

    }
    Serial.println("-----------------------");
  }

  if (String(topic) == sub_TIMER1_RELAY ) {
    time1 = message;
    time1a = message.substring(0,7) + String((message.substring(7,8).toInt()+1)%10);
    client.publish("esp32relaytimer1_return",time1.c_str());//mqtt发送
  }
  if (String(topic) == sub_TIMER1_RELAY_onoff ) {
    if (message.toInt() == 1) {
      timeswitch1 = message.toInt();
      client.publish("esp32relaytimer1_on/offreset", "1");
      Serial.println("timer1 ON");
    }
    if (message.toInt() == 0) {
      timeswitch1 = message.toInt();
      client.publish("esp32relaytimer1_on/offreset", "0");
      Serial.println("timer1 OFF");
    }
  }

  if (String(topic) == sub_TIMER2_RELAY ) {
    time2 = message;
    time2a = message.substring(0,7) + String((message.substring(7,8).toInt()+1)%10);
    client.publish("esp32relaytimer2_return",time2.c_str());//mqtt发送
  }
  if (String(topic) == sub_TIMER2_RELAY_onoff ) {
    if (message.toInt() == 1) {
      timeswitch2 = message.toInt();
      client.publish("esp32relaytimer2_on/offreset", "1");
      Serial.println("timer2 ON");
    }
    if (message.toInt() == 0) {
      timeswitch2 = message.toInt();
      client.publish("esp32relaytimer2_on/offreset", "0");
      Serial.println("timer2 OFF");
    }
  }

  if (String(topic) == sub_TIMER3_RELAY ) {
    time3 = message;
    time3a = message.substring(0,7) + String((message.substring(7,8).toInt()+1)%10);
    client.publish("esp32relaytimer3_return",time3.c_str());//mqtt发送
  }
  if (String(topic) == sub_TIMER3_RELAY_onoff ) {
    if (message.toInt() == 1) {
      timeswitch3 = message.toInt();
      client.publish("esp32relaytimer3_on/offreset", "1");
      Serial.println("timer3 ON");
    }
    if (message.toInt() == 0) {
      timeswitch3 = message.toInt();
      client.publish("esp32relaytimer3_on/offreset", "0");
      Serial.println("timer3 OFF");
    }
  }

  if (String(topic) == sub_TIMER4_RELAY ) {
    time4 = message;
    time4a = message.substring(0,7) + String((message.substring(7,8).toInt()+1)%10);
    client.publish("esp32relaytimer4_return",time4.c_str());//mqtt发送
  }
  if (String(topic) == sub_TIMER4_RELAY_onoff ) {
    if (message.toInt() == 1) {
      timeswitch4 = message.toInt();
      client.publish("esp32relaytimer4_on/offreset", "1");
      Serial.println("timer4 ON");
    }
    if (message.toInt() == 0) {
      timeswitch4 = message.toInt();
      client.publish("esp32relaytimer4_on/offreset", "0");
      Serial.println("timer4 OFF");
    }
  }

  if (String(topic) == sub_TIMER5_RELAY ) {
    time5 = message;
    time5a = message.substring(0,7) + String((message.substring(7,8).toInt()+1)%10);
    client.publish("esp32relaytimer5_return",time5.c_str());//mqtt发送
  }
  if (String(topic) == sub_TIMER5_RELAY_onoff ) {
    if (message.toInt() == 1) {
      timeswitch5 = message.toInt();
      client.publish("esp32relaytimer5_on/offreset", "1");
      Serial.println("timer5 ON");
    }
    if (message.toInt() == 0) {
      timeswitch5 = message.toInt();
      client.publish("esp32relaytimer5_on/offreset", "0");
      Serial.println("timer5 OFF");
    }
  }

  if (String(topic) == sub_TIMER6_RELAY ) {
    time6 = message;
    time6a = message.substring(0,7) + String((message.substring(7,8).toInt()+1)%10);
    client.publish("esp32relaytimer6_return",time6.c_str());//mqtt发送
  }
  if (String(topic) == sub_TIMER6_RELAY_onoff ) {
    if (message.toInt() == 1) {
      timeswitch6 = message.toInt();
      client.publish("esp32relaytimer6_on/offreset", "1");
      Serial.println("timer6 ON");
    }
    if (message.toInt() == 0) {
      timeswitch6 = message.toInt();
      client.publish("esp32relaytimer6_on/offreset", "0");
      Serial.println("timer6 OFF");
    }
  }

  if (String(topic) == sub_TIMER7_RELAY ) {
    time7 = message;
    time7a = message.substring(0,7) + String((message.substring(7,8).toInt()+1)%10);
    client.publish("esp32relaytimer7_return",time7.c_str());//mqtt发送
  }
  if (String(topic) == sub_TIMER7_RELAY_onoff ) {
    if (message.toInt() == 1) {
      timeswitch7 = message.toInt();
      client.publish("esp32relaytimer7_on/offreset", "1");
      Serial.println("timer7 ON");
    }
    if (message.toInt() == 0) {
      timeswitch7 = message.toInt();
      client.publish("esp32relaytimer7_on/offreset", "0");
      Serial.println("timer7 OFF");
    }
  }

  if (String(topic) == sub_TIMER8_RELAY ) {
    time8 = message;
    time8a = message.substring(0,7) + String((message.substring(7,8).toInt()+1)%10);
    client.publish("esp32relaytimer8_return",time8.c_str());//mqtt发送
  }
  if (String(topic) == sub_TIMER8_RELAY_onoff ) {
    if (message.toInt() == 1) {
      timeswitch8 = message.toInt();
      client.publish("esp32relaytimer8_on/offreset", "1");
      Serial.println("timer2 ON");
    }
    if (message.toInt() == 0) {
      timeswitch8 = message.toInt();
      client.publish("esp32relaytimer8_on/offreset", "0");
      Serial.println("timer8 OFF");
    }
  }

  if (String(topic) == sub_TIMER9_RELAY ) {
    time9 = message;
    time9a = message.substring(0,7) + String((message.substring(7,8).toInt()+1)%10);
    client.publish("esp32relaytimer9_return",time9.c_str());//mqtt发送
  }
  if (String(topic) == sub_TIMER9_RELAY_onoff ) {
    if (message.toInt() == 1) {
      timeswitch9 = message.toInt();
      client.publish("esp32relaytimer9_on/offreset", "1");
      Serial.println("timer9 ON");
    }
    if (message.toInt() == 0) {
      timeswitch9 = message.toInt();
      client.publish("esp32relaytimer9_on/offreset", "0");
      Serial.println("timer9 OFF");
    }
  }

  if (String(topic) == sub_TIMER10_RELAY ) {
    time10 = message;
    time10a = message.substring(0,7) + String((message.substring(7,8).toInt()+1)%10);
    client.publish("esp32relaytimer10_return",time10.c_str());//mqtt发送
  }
  if (String(topic) == sub_TIMER10_RELAY_onoff ) {
    if (message.toInt() == 1) {
      timeswitch10 = message.toInt();
      client.publish("esp32relaytimer10_on/offreset", "1");
      Serial.println("timer10 ON");
    }
    if (message.toInt() == 0) {
      timeswitch10 = message.toInt();
      client.publish("esp32relaytimer10_on/offreset", "0");
      Serial.println("timer10 OFF");
    }
  }






  if (String(topic) == sub_TIMER_option1 ) { //定时器选项,主题对message写入timer_option1
    timer_option1 = message;
    client.publish("esp32timeroption1reset", timer_option1.c_str());
  }
  if (String(topic) == sub_TIMER_option2 ) {
    timer_option2 = message;
    client.publish("esp32timeroption2reset", timer_option2.c_str());
  }
  if (String(topic) == sub_TIMER_option3 ) {
    timer_option3 = message;
    client.publish("esp32timeroption3reset", timer_option3.c_str());
  }
  if (String(topic) == sub_TIMER_option4 ) {
    timer_option4 = message;
    client.publish("esp32timeroption4reset", timer_option4.c_str());
  }
  if (String(topic) == sub_TIMER_option5 ) {
    timer_option5 = message;
    client.publish("esp32timeroption5reset", timer_option5.c_str());
  }
  if (String(topic) == sub_TIMER_option6 ) {
    timer_option6 = message;
    client.publish("esp32timeroption6reset", timer_option6.c_str());
  }
  if (String(topic) == sub_TIMER_option7 ) { //定时器选项,主题对message写入timer_option1
    timer_option7 = message;
    client.publish("esp32timeroption7reset", timer_option7.c_str());
  }
  if (String(topic) == sub_TIMER_option8 ) {
    timer_option8 = message;
    client.publish("esp32timeroption8reset", timer_option9.c_str());
  }
  if (String(topic) == sub_TIMER_option9 ) {
    timer_option9 = message;
    client.publish("esp32timeroption9reset", timer_option9.c_str());
  }
  if (String(topic) == sub_TIMER_option10 ) {
    timer_option10 = message;
    client.publish("esp32timeroption10reset", timer_option10.c_str());
  }

  message = "";
}




void loop() {
  if (!client2.connect(serverIP2, serverPort)) {//嘗試訪問目標地址 IP2
    Serial.println("訪問IP2 失敗");
    client.publish("client2status", "disconnect");
    delay(200);

    client2counter++;
    if (client2counter >= 10) { // 10 秒超時後重置板
      ESP.restart();
    }

    return;
  }

  if (!client4.connect(serverIP5, serverPort)) {//嘗試訪問目標地址 IP10
    Serial.println("訪問IP10 失敗");
    client.publish("client4status", "disconnect");
    delay(200);

    client4counter++;
    if (client4counter >= 10) { // 10 秒超時後重置板
      ESP.restart();
    }

    return;
  }



  wifireconnect();//WiFi断线esp32重启

  timing1s();
  timing10s();
  sysc_timing();//同步到mqtt dash


  client.loop();

  if (client2.connect(serverIP2, serverPort) && conterrs <= 20) {//重置继电器

    s = 'b';
    detect_s(s);
    conterrs ++;
    delay(10);
  }

  if (client2.connect(serverIP2, serverPort) && conterrs2 <= 20) {//重置继电器

    s = 'd';
    detect_s(s);
    conterrs2 ++;
    delay(10);
  }

  if (client4.connect(serverIP5, serverPort) && conterrs3 <= 20) {//重置继电器

    s = 'f';
    detect_s(s);
    conterrs3++;
    delay(10);
  }

  if (client4.connect(serverIP5, serverPort) && conterrs4 <= 20) {//重置继电器

    s = 'h';
    detect_s(s);
    conterrs4++;
    delay(10);
  }

  if (client4.connect(serverIP5, serverPort) && conterrs5 <= 20) {//重置继电器

    s = 'j';
    detect_s(s);
    conterrs5++;
    delay(10);
  }

  if (client4.connect(serverIP5, serverPort) && conterrs6 <= 20) {//重置继电器

    s = 'l';
    detect_s(s);
    conterrs6 ++;
    delay(10);
  }




  botton1();
  botton2();
  botton3();
  botton4();
  botton5();
  botton6();
  
  timer1();
  timer2();
  timer3();
  timer4();
  timer5();
  timer6();
  timer7();
  timer8();
  timer9();
  timer10();

  


}

//发送WiFi relay hex 控制指令的功能
void detect_s(char s) {
  

  if (s == 'a') {
    client2.write(rel1ON, sizeof(rel1ON));
    client.publish("esp32relaystatusch2r1", "ON");//mqtt发送
    ch2r1 = 1;
    Serial.println("client2 esp32relay 1 ON");
    //2ch r1
  } else if (s == 'b') {
    client2.write(rel1OFF, sizeof(rel1OFF));
    client.publish("esp32relaystatusch2r1", "off");//mqtt发送
    ch2r1 = 0;
    Serial.println("client2 esp32relay 1 OFF");

  } else if (s == 'c') {
    client2.write(rel2ON, sizeof(rel2ON));
    client.publish("esp32relaystatusch2r2", "ON");//mqtt发送
    ch2r2 = 1;
    Serial.println("client2 esp32relay 2 ON");
    //2ch r2
  } else if (s == 'd') {
    client2.write(rel2OFF, sizeof(rel2OFF));
    client.publish("esp32statusch2r2", "off");//mqtt发送
    ch2r2 = 0;
    Serial.println("client2 esp32relay 2 OFF");


  } else if (s == 'e') {
    client4.write(rel1ON, sizeof(rel1ON));
    client.publish("esp32relaystatusch4r1", "ON");//mqtt发送
    ch4r1 = 1;
    Serial.println("client4 esp32relay 1 ON");
    //4ch r1
  } else if (s == 'f') {
    client4.write(rel1OFF, sizeof(rel1OFF));
    client.publish("esp32relaystatusch4r1", "off");//mqtt发送
    ch4r1 = 0;
    Serial.println("client4 esp32relay 1 OFF");



  } else if (s == 'g') {
    client4.write(rel2ON, sizeof(rel2ON));
    client.publish("esp32relaystatusch4r2", "ON");//mqtt发送
    ch4r2 = 1;
    Serial.println("client4 esp32relay 2 ON");
    //4ch r2
  } else if (s == 'h') {
    client4.write(rel2OFF, sizeof(rel2OFF));
    client.publish("esp32relaystatusch4r2", "off");//mqtt发送
    ch4r2 = 0;
    Serial.println("client4 esp32relay 2 OFF");



  } else if (s == 'i') {
    client4.write(rel3ON, sizeof(rel3ON));
    client.publish("esp32relaystatusch4r3", "ON");//mqtt发送
    ch4r3 = 1;
    Serial.println("client4 esp32relay 3 ON");
    //4ch r3
  } else if (s == 'j') {
    client4.write(rel3OFF, sizeof(rel3OFF));
    client.publish("esp32relaystatusch4r3", "off");//mqtt发送
    ch4r3 = 0;
    Serial.println("client4 esp32relay 3 OFF");


  } else if (s == 'k') {
    client4.write(rel4ON, sizeof(rel4ON));
    client.publish("esp32relaystatusch4r4", "ON");//mqtt发送
    ch4r4 = 1;
    Serial.println("client4 esp32relay 4 ON");
    //4ch r4
  } else if (s == 'l') {
    client4.write(rel4OFF, sizeof(rel4OFF));
    client.publish("esp32relaystatusch4r4", "off");//mqtt发送
    ch4r4 = 0;
    Serial.println("client4 esp32relay 4 OFF");


  } else {
    Serial.println("error");
  }
}


//botton1到6的控制逻辑功能（手动自回弹按钮开关）
void botton1() {
  int reading = digitalRead(b1);//檢查是否只按了按鈕 （即輸入從HIGH變為LOW）
  if (reading != lastButtonStateb1) {
    lastDebounceTimeb1 = millis();
  }
  if ((millis() - lastDebounceTimeb1) > debounceDelay) {
    if (reading != buttonStateb1) {
      buttonStateb1 = reading;
      if (buttonStateb1 == LOW && ch2r1 == 1) { //如果按鍵按了
        Serial.println("b1有按下了");
        s = 'b'; //off
        detect_s(s);
        led();

      } else if (buttonStateb1 == LOW && ch2r1 == 0) {
        Serial.println("b1有按下了1");
        s = 'a';//on
        detect_s(s);
        led();

      } else {
        
      }
    }
  }
  lastButtonStateb1 = reading;
  return;
}

void botton2() {
  int reading = digitalRead(b2);//檢查是否只按了按鈕 （即輸入從HIGH變為LOW）
  if (reading != lastButtonStateb2) {
    lastDebounceTimeb2 = millis();
  }
  if ((millis() - lastDebounceTimeb2) > debounceDelay) {
    if (reading != buttonStateb2) {
      buttonStateb2 = reading;
      if (buttonStateb2 == LOW && ch2r2 == 1) {//如果按鍵按了
        Serial.println("b2有按下了");
        s = 'd'; //off
        detect_s(s);
        led();

      } else if (buttonStateb2 == LOW && ch2r2 == 0) {
        Serial.println("b2有按下了2");
        s = 'c';//on
        detect_s(s);
        led();
      } else {

      }
    }
  }
  lastButtonStateb2 = reading;
  return;
}

void botton3() {
  int reading = digitalRead(b3);//檢查是否只按了按鈕 （即輸入從HIGH變為LOW）
  if (reading != lastButtonStateb3) {
    lastDebounceTimeb3 = millis();
  }
  if ((millis() - lastDebounceTimeb3) > debounceDelay) {
    if (reading != buttonStateb3) {
      buttonStateb3 = reading;
      if (buttonStateb3 == LOW && ch4r1 == 1) {//如果按鍵按了
        Serial.println("b3有按下了");
        s = 'f'; //off
        detect_s(s);
        led();

      } else if (buttonStateb3 == LOW && ch4r1 == 0) {
        Serial.println("b3有按下了3");
        s = 'e';//on
        detect_s(s);
        led();

      } else {

      }
    }
  }
  lastButtonStateb3 = reading;
  return;
}

void botton4() {
  int reading = digitalRead(b4);//檢查是否只按了按鈕 （即輸入從HIGH變為LOW）
  if (reading != lastButtonStateb4) {
    lastDebounceTimeb4 = millis();
  }
  if ((millis() - lastDebounceTimeb4) > debounceDelay) {
    if (reading != buttonStateb4) {
      buttonStateb4 = reading;
      if (buttonStateb4 == LOW && ch4r2 == 1) {//如果按鍵按了
        Serial.println("b4有按下了");
        led();
        s = 'h'; //off
        detect_s(s);
        led();

      } else if (buttonStateb4 == LOW && ch4r2 == 0) {
        Serial.println("b4有按下了4");
        s = 'g';//on
        detect_s(s);

        led();

      } else {
        //Serial.println("botton1 error");

      }
    }
  }
  lastButtonStateb4 = reading;
  return;
}

void botton5() {
  int reading = digitalRead(b5);//檢查是否只按了按鈕 （即輸入從HIGH變為LOW）
  if (reading != lastButtonStateb5) {
    lastDebounceTimeb5 = millis();
  }
  if ((millis() - lastDebounceTimeb5) > debounceDelay) {
    if (reading != buttonStateb5) {
      buttonStateb5 = reading;
      if (buttonStateb5 == LOW && ch4r3 == 1) {//如果按鍵按了
        Serial.println("b5有按下了");
        s = 'j'; //off
        detect_s(s);
        led();

      } else if (buttonStateb5 == LOW && ch4r3 == 0) {
        Serial.println("b5有按下了5");
        s = 'i';//on
        detect_s(s);
        led();

      } else {
        //Serial.println("botton5 error");

      }
    }
  }
  lastButtonStateb5 = reading;
  return;
}

void botton6() {
  int reading = digitalRead(b6);//檢查是否只按了按鈕 （即輸入從HIGH變為LOW）
  if (reading != lastButtonStateb6) {
    lastDebounceTimeb6 = millis();
  }
  if ((millis() - lastDebounceTimeb6) > debounceDelay) {
    if (reading != buttonStateb6) {
      buttonStateb6 = reading;
      if (buttonStateb6 == LOW && ch4r4 == 1) {//如果按鍵按了
        Serial.println("b6有按下了");

        s = 'l'; //off
        detect_s(s);
        led();

      } else if (buttonStateb6 == LOW && ch4r4 == 0) {
        Serial.println("b6有按下了6");
        s = 'k';//on
        detect_s(s);
        led();

      } else {
        //Serial.println("botton6 error");

      }
    }
  }
  lastButtonStateb6 = reading;
  return;
}

void wifireconnect() { //WiFi断线esp32重启
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    counter++;
    if (counter >= 10) { // 10 秒超時後重置板
      ESP.restart();
    }
  }
}

void printLocalTime() {//從網路ntpServer獲取時間
  time_t rawtime;
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    Serial.println("未能獲得時間");
    return;
  }
  strftime(timeStringBuff, sizeof(timeStringBuff), "%H:%M:%S", &timeinfo);  //幾時:幾分:幾秒
  Serial.println("现在时间： " + String(timeStringBuff)); //顯示日期時間
  client.publish("timedisplay", timeStringBuff);
}

void timing1s() { //每隔一秒获取网络时间
  unsigned long int current_millis_A = millis(); //current_millis_D = 2000
  if ((current_millis_A - previous_millis_A) >= interval_A) //(2000 - 1000) >= 2000
  {
    /*要執行的程式放下面*/
    printLocalTime();

    previous_millis_A = current_millis_A;

  }
}

//给mqttdash发送WiFi relay连接状态的功能，10秒执行一次
void timing10s() {
  unsigned long int current_millis_B = millis(); //current_millis_D = 2000
  if ((current_millis_B - previous_millis_B) >= interval_B) { //(2000 - 1000) >= 2000

    /*要執行的程式放下面*/
    if (client2.connect(serverIP2, serverPort)) //嘗試訪問目標地址
    {
      client.publish("client2status", "Connected");
    }

    if (client4.connect(serverIP5, serverPort)) //嘗試訪問目標地址
    {
      client.publish("client4status", "Connected");
    }
    previous_millis_B = current_millis_B;
  }
}

//同步数据到所有MQTT dash用户端
void sysc_timing() {
  unsigned long int current_millis_C = millis(); //current_millis_D = 2000
  if ((current_millis_C - previous_millis_C) >= interval_C) { 

    /*要執行的程式放下面*/
if (ch2r1==0){
    client.publish("esp32relaystatusch2r1", "off");
}
if (ch2r1==1){
  client.publish("esp32relaystatusch2r1", "ON");
}

if (ch2r2==0){
    client.publish("esp32relaystatusch2r2", "off");
}
if (ch2r2==1){
  client.publish("esp32relaystatusch2r2", "ON");
}

if (ch4r1==0){
    client.publish("esp32relaystatusch4r1", "off");
}
if (ch4r1==1){
  client.publish("esp32relaystatusch4r1", "ON");
}

if (ch4r2==0){
    client.publish("esp32relaystatusch4r2", "off");
}
if (ch4r2==1){
  client.publish("esp32relaystatusch4r2", "ON");
}

if (ch4r3==0){
    client.publish("esp32relaystatusch4r3", "off");
}
if (ch4r3==1){
  client.publish("esp32relaystatusch4r3", "ON");
}

if (ch4r4==0){
    client.publish("esp32relaystatusch4r4", "off");
}
if (ch4r4==1){
  client.publish("esp32relaystatusch4r4", "ON");
}

client.publish("esp32relaytimer1_return",time1.c_str());
client.publish("esp32relaytimer2_return",time2.c_str());
client.publish("esp32relaytimer3_return",time3.c_str());
client.publish("esp32relaytimer4_return",time4.c_str());
client.publish("esp32relaytimer5_return",time5.c_str());
client.publish("esp32relaytimer6_return",time6.c_str());
client.publish("esp32relaytimer7_return",time7.c_str());
client.publish("esp32relaytimer8_return",time8.c_str());
client.publish("esp32relaytimer9_return",time9.c_str());
client.publish("esp32relaytimer10_return",time10.c_str());


client.publish("esp32timeroption1reset", timer_option1.c_str());
client.publish("esp32timeroption2reset", timer_option2.c_str());
client.publish("esp32timeroption3reset", timer_option3.c_str());
client.publish("esp32timeroption4reset", timer_option4.c_str());
client.publish("esp32timeroption5reset", timer_option5.c_str());
client.publish("esp32timeroption6reset", timer_option6.c_str());
client.publish("esp32timeroption7reset", timer_option7.c_str());
client.publish("esp32timeroption8reset", timer_option8.c_str());
client.publish("esp32timeroption9reset", timer_option9.c_str());
client.publish("esp32timeroption10reset", timer_option10.c_str());

client.publish("esp32relaytimer1_on/offreset", String(timeswitch1).c_str());
client.publish("esp32relaytimer2_on/offreset", String(timeswitch2).c_str());
client.publish("esp32relaytimer3_on/offreset", String(timeswitch3).c_str());
client.publish("esp32relaytimer4_on/offreset", String(timeswitch4).c_str());
client.publish("esp32relaytimer5_on/offreset", String(timeswitch5).c_str());
client.publish("esp32relaytimer6_on/offreset", String(timeswitch6).c_str());
client.publish("esp32relaytimer7_on/offreset", String(timeswitch7).c_str());
client.publish("esp32relaytimer8_on/offreset", String(timeswitch8).c_str());
client.publish("esp32relaytimer9_on/offreset", String(timeswitch9).c_str());
client.publish("esp32relaytimer10_on/offreset", String(timeswitch10).c_str());

    previous_millis_C = current_millis_C;
  }
}

//定时器时间检测并执行定时器选项的功能
void timer1() {
  if (time1 <= String(timeStringBuff) && time1a >= String(timeStringBuff) && timeswitch1 == 1) {
    timerOption1();
  }
}

void timer2() {
  if (time2 <= String(timeStringBuff) && time2a >= String(timeStringBuff) && timeswitch2 == 1) {
    timerOption2();
  }
}

void timer3() {
  if (time3 <= String(timeStringBuff) && time3a >= String(timeStringBuff) && timeswitch3 == 1) {
    timerOption3();
  }
}

void timer4() {
  if (time4 <= String(timeStringBuff) && time4a >= String(timeStringBuff) && timeswitch4 == 1) {
    timerOption4();
  }
}

void timer5() {
  if (time5 <= String(timeStringBuff) && time5a >= String(timeStringBuff) && timeswitch5 == 1) {
    timerOption5();
  }
}

void timer6() {
  if (time6 <= String(timeStringBuff) && time6a >= String(timeStringBuff) && timeswitch6 == 1) {
    timerOption6();
  }
}

void timer7() {
  if (time7 <= String(timeStringBuff) && time7a >= String(timeStringBuff) && timeswitch7 == 1) {
    timerOption7();
  }
}

void timer8() {
  if (time8 <= String(timeStringBuff) && time8a >= String(timeStringBuff) && timeswitch8 == 1) {
    timerOption8();
  }
}

void timer9() {
  if (time9 <= String(timeStringBuff) && time9a >= String(timeStringBuff) && timeswitch9 == 1) {
    timerOption9();
  }
}

void timer10() {
  if (time10 <= String(timeStringBuff) && time10a >= String(timeStringBuff) && timeswitch10 == 1) {
    timerOption10();
  }
}



//执行定时器选项的功能，用于发送WiFi relay的控制指令
void timerOption1() {
  if (timer_option1 == "a") {
    s = 'a'; //为是赋值
    detect_s(s);//调用检测s，执行继电器开关hex指令
    client.publish("esp32relaytimer1_return",time1.c_str());//mqtt发送
  }

  if (timer_option1 == "b") {
    s = 'b'; //为是赋值
    detect_s(s);//调用检测s，执行继电器开关hex指令
    client.publish("esp32relaytimer1_return",time1.c_str());//mqtt发送
  }

  if (timer_option1 == "c") {
    s = 'c'; //为是赋值
    detect_s(s);//调用检测s，执行继电器开关hex指令
    client.publish("esp32relaytimer1_return",time1.c_str());//mqtt发送
  }

  if (timer_option1 == "d") {
    s = 'd'; //为是赋值
    detect_s(s);//调用检测s，执行继电器开关hex指令
    client.publish("esp32relaytimer1_return",time1.c_str());//mqtt发送
  }

  if (timer_option1 == "e") {
    s = 'e'; //为是赋值
    detect_s(s);//调用检测s，执行继电器开关hex指令
    client.publish("esp32relaytimer1_return",time1.c_str());//mqtt发送
  }

  if (timer_option1 == "f") {
    s = 'f'; //为是赋值
    detect_s(s);//调用检测s，执行继电器开关hex指令
    client.publish("esp32relaytimer1_return",time1.c_str());//mqtt发送
  }

  if (timer_option1 == "g") {
    s = 'g'; //为是赋值
    detect_s(s);//调用检测s，执行继电器开关hex指令
    client.publish("esp32relaytimer1_return",time1.c_str());//mqtt发送
  }

  if (timer_option1 == "h") {
    s = 'h'; //为是赋值
    detect_s(s);//调用检测s，执行继电器开关hex指令
    client.publish("esp32relaytimer1_return",time1.c_str());//mqtt发送
  }

  if (timer_option1 == "i") {
    s = 'i'; //为是赋值
    detect_s(s);//调用检测s，执行继电器开关hex指令
    client.publish("esp32relaytimer1_return",time1.c_str());//mqtt发送
  }

  if (timer_option1 == "j") {
    s = 'j'; //为是赋值
    detect_s(s);//调用检测s，执行继电器开关hex指令
    client.publish("esp32relaytimer1_return",time1.c_str());//mqtt发送
  }

  if (timer_option1 == "k") {
    s = 'k'; //为是赋值
    detect_s(s);//调用检测s，执行继电器开关hex指令
    client.publish("esp32relaytimer1_return",time1.c_str());//mqtt发送
  }

  if (timer_option1 == "l") {
    s = 'l'; //为是赋值
    detect_s(s);//调用检测s，执行继电器开关hex指令
    client.publish("esp32relaytimer1_return",time1.c_str());//mqtt发送
  }
}


void timerOption2() {
  if (timer_option2 == "a") {
    s = 'a'; //为是赋值
    detect_s(s);//调用检测s，执行继电器开关hex指令
    client.publish("esp32relaytimer2_return",time2.c_str());//mqtt发送
  }

  if (timer_option2 == "b") {
    s = 'b'; //为是赋值
    detect_s(s);//调用检测s，执行继电器开关hex指令
    client.publish("esp32relaytimer2_return",time2.c_str());//mqtt发送
  }

  if (timer_option2 == "c") {
    s = 'c'; //为是赋值
    detect_s(s);//调用检测s，执行继电器开关hex指令
    client.publish("esp32relaytimer2_return",time2.c_str());//mqtt发送
  }

  if (timer_option2 == "d") {
    s = 'd'; //为是赋值
    detect_s(s);//调用检测s，执行继电器开关hex指令
    client.publish("esp32relaytimer2_return",time2.c_str());//mqtt发送
  }

  if (timer_option2 == "e") {
    s = 'e'; //为是赋值
    detect_s(s);//调用检测s，执行继电器开关hex指令
    client.publish("esp32relaytimer2_return",time2.c_str());//mqtt发送
  }

  if (timer_option2 == "f") {
    s = 'f'; //为是赋值
    detect_s(s);//调用检测s，执行继电器开关hex指令
    client.publish("esp32relaytimer2_return",time2.c_str());//mqtt发送
  }

  if (timer_option2 == "g") {
    s = 'g'; //为是赋值
    detect_s(s);//调用检测s，执行继电器开关hex指令
    client.publish("esp32relaytimer2_return",time2.c_str());//mqtt发送
  }

  if (timer_option2 == "h") {
    s = 'h'; //为是赋值
    detect_s(s);//调用检测s，执行继电器开关hex指令
    client.publish("esp32relaytimer2_return",time2.c_str());//mqtt发送
  }

  if (timer_option2 == "i") {
    s = 'i'; //为是赋值
    detect_s(s);//调用检测s，执行继电器开关hex指令
    client.publish("esp32relaytimer2_return",time2.c_str());//mqtt发送
  }

  if (timer_option2 == "j") {
    s = 'j'; //为是赋值
    detect_s(s);//调用检测s，执行继电器开关hex指令
    client.publish("esp32relaytimer2_return",time2.c_str());//mqtt发送
  }

  if (timer_option2 == "k") {
    s = 'k'; //为是赋值
    detect_s(s);//调用检测s，执行继电器开关hex指令
    client.publish("esp32relaytimer2_return",time2.c_str());//mqtt发送
  }

  if (timer_option2 == "l") {
    s = 'l'; //为是赋值
    detect_s(s);//调用检测s，执行继电器开关hex指令
    client.publish("esp32relaytimer2_return",time2.c_str());//mqtt发送
  }
}


void timerOption3() {
  if (timer_option3 == "a") {
    s = 'a'; //为是赋值
    detect_s(s);//调用检测s，执行继电器开关hex指令
    client.publish("esp32relaytimer3_return",time3.c_str());//mqtt发送
  }

  if (timer_option3 == "b") {
    s = 'b'; //为是赋值
    detect_s(s);//调用检测s，执行继电器开关hex指令
    client.publish("esp32relaytimer3_return",time3.c_str());//mqtt发送
  }

  if (timer_option3 == "c") {
    s = 'c'; //为是赋值
    detect_s(s);//调用检测s，执行继电器开关hex指令
    client.publish("esp32relaytimer3_return",time3.c_str());//mqtt发送
  }

  if (timer_option3 == "d") {
    s = 'd'; //为是赋值
    detect_s(s);//调用检测s，执行继电器开关hex指令
    client.publish("esp32relaytimer3_return",time3.c_str());//mqtt发送
  }

  if (timer_option3 == "e") {
    s = 'e'; //为是赋值
    detect_s(s);//调用检测s，执行继电器开关hex指令
    client.publish("esp32relaytimer3_return",time3.c_str());//mqtt发送
  }

  if (timer_option3 == "f") {
    s = 'f'; //为是赋值
    detect_s(s);//调用检测s，执行继电器开关hex指令
    client.publish("esp32relaytimer3_return",time3.c_str());//mqtt发送
  }

  if (timer_option3 == "g") {
    s = 'g'; //为是赋值
    detect_s(s);//调用检测s，执行继电器开关hex指令
    client.publish("esp32relaytimer3_return",time3.c_str());//mqtt发送
  }

  if (timer_option3 == "h") {
    s = 'h'; //为是赋值
    detect_s(s);//调用检测s，执行继电器开关hex指令
    client.publish("esp32relaytimer3_return",time3.c_str());//mqtt发送
  }

  if (timer_option3 == "i") {
    s = 'i'; //为是赋值
    detect_s(s);//调用检测s，执行继电器开关hex指令
    client.publish("esp32relaytimer3_return",time3.c_str());//mqtt发送
  }

  if (timer_option3 == "j") {
    s = 'j'; //为是赋值
    detect_s(s);//调用检测s，执行继电器开关hex指令
    client.publish("esp32relaytimer3_return",time3.c_str());//mqtt发送
  }

  if (timer_option3 == "k") {
    s = 'k'; //为是赋值
    detect_s(s);//调用检测s，执行继电器开关hex指令
    client.publish("esp32relaytimer3_return",time3.c_str());//mqtt发送
  }

  if (timer_option3 == "l") {
    s = 'l'; //为是赋值
    detect_s(s);//调用检测s，执行继电器开关hex指令
    client.publish("esp32relaytimer3_return",time3.c_str());//mqtt发送
  }
}


void timerOption4() {
  if (timer_option4 == "a") {
    s = 'a'; //为是赋值
    detect_s(s);//调用检测s，执行继电器开关hex指令
    client.publish("esp32relaytimer4_return",time4.c_str());//mqtt发送
  }

  if (timer_option4 == "b") {
    s = 'b'; //为是赋值
    detect_s(s);//调用检测s，执行继电器开关hex指令
    client.publish("esp32relaytimer4_return",time4.c_str());//mqtt发送
  }

  if (timer_option4 == "c") {
    s = 'c'; //为是赋值
    detect_s(s);//调用检测s，执行继电器开关hex指令
    client.publish("esp32relaytimer4_return",time4.c_str());//mqtt发送
  }

  if (timer_option4 == "d") {
    s = 'd'; //为是赋值
    detect_s(s);//调用检测s，执行继电器开关hex指令
    client.publish("esp32relaytimer4_return",time4.c_str());//mqtt发送
  }

  if (timer_option4 == "e") {
    s = 'e'; //为是赋值
    detect_s(s);//调用检测s，执行继电器开关hex指令
    client.publish("esp32relaytimer4_return",time4.c_str());//mqtt发送
  }

  if (timer_option4 == "f") {
    s = 'f'; //为是赋值
    detect_s(s);//调用检测s，执行继电器开关hex指令
    client.publish("esp32relaytimer4_return",time4.c_str());//mqtt发送
  }

  if (timer_option4 == "g") {
    s = 'g'; //为是赋值
    detect_s(s);//调用检测s，执行继电器开关hex指令
    client.publish("esp32relaytimer4_return",time4.c_str());//mqtt发送
  }

  if (timer_option4 == "h") {
    s = 'h'; //为是赋值
    detect_s(s);//调用检测s，执行继电器开关hex指令
    client.publish("esp32relaytimer4_return",time4.c_str());//mqtt发送
  }

  if (timer_option4 == "i") {
    s = 'i'; //为是赋值
    detect_s(s);//调用检测s，执行继电器开关hex指令
    client.publish("esp32relaytimer4_return",time4.c_str());//mqtt发送
  }

  if (timer_option4 == "j") {
    s = 'j'; //为是赋值
    detect_s(s);//调用检测s，执行继电器开关hex指令
    client.publish("esp32relaytimer4_return",time4.c_str());//mqtt发送
  }

  if (timer_option4 == "k") {
    s = 'k'; //为是赋值
    detect_s(s);//调用检测s，执行继电器开关hex指令
    client.publish("esp32relaytimer4_return",time4.c_str());//mqtt发送
  }

  if (timer_option4 == "l") {
    s = 'l'; //为是赋值
    detect_s(s);//调用检测s，执行继电器开关hex指令
    client.publish("esp32relaytimer4_return",time4.c_str());//mqtt发送
  }
}


void timerOption5() {
  if (timer_option5 == "a") {
    s = 'a'; //为是赋值
    detect_s(s);//调用检测s，执行继电器开关hex指令
    client.publish("esp32relaytimer5_return",time5.c_str());//mqtt发送
  }

  if (timer_option5 == "b") {
    s = 'b'; //为是赋值
    detect_s(s);//调用检测s，执行继电器开关hex指令
    client.publish("esp32relaytimer5_return",time5.c_str());//mqtt发送
  }

  if (timer_option5 == "c") {
    s = 'c'; //为是赋值
    detect_s(s);//调用检测s，执行继电器开关hex指令
    client.publish("esp32relaytimer5_return",time5.c_str());//mqtt发送
  }

  if (timer_option5 == "d") {
    s = 'd'; //为是赋值
    detect_s(s);//调用检测s，执行继电器开关hex指令
    client.publish("esp32relaytimer5_return",time5.c_str());//mqtt发送
  }

  if (timer_option5 == "e") {
    s = 'e'; //为是赋值
    detect_s(s);//调用检测s，执行继电器开关hex指令
    client.publish("esp32relaytimer5_return",time5.c_str());//mqtt发送
  }

  if (timer_option5 == "f") {
    s = 'f'; //为是赋值
    detect_s(s);//调用检测s，执行继电器开关hex指令
    client.publish("esp32relaytimer5_return",time5.c_str());//mqtt发送
  }

  if (timer_option5 == "g") {
    s = 'g'; //为是赋值
    detect_s(s);//调用检测s，执行继电器开关hex指令
    client.publish("esp32relaytimer5_return",time5.c_str());//mqtt发送
  }

  if (timer_option5 == "h") {
    s = 'h'; //为是赋值
    detect_s(s);//调用检测s，执行继电器开关hex指令
    client.publish("esp32relaytimer5_return",time5.c_str());//mqtt发送
  }

  if (timer_option5 == "i") {
    s = 'i'; //为是赋值
    detect_s(s);//调用检测s，执行继电器开关hex指令
    client.publish("esp32relaytimer5_return",time5.c_str());//mqtt发送
  }

  if (timer_option5 == "j") {
    s = 'j'; //为是赋值
    detect_s(s);//调用检测s，执行继电器开关hex指令
    client.publish("esp32relaytimer5_return",time5.c_str());//mqtt发送
  }

  if (timer_option5 == "k") {
    s = 'k'; //为是赋值
    detect_s(s);//调用检测s，执行继电器开关hex指令
    client.publish("esp32relaytimer5_return",time5.c_str());//mqtt发送
  }

  if (timer_option5 == "l") {
    s = 'l'; //为是赋值
    detect_s(s);//调用检测s，执行继电器开关hex指令
    client.publish("esp32relaytimer5_return",time5.c_str());//mqtt发送
  }
}


void timerOption6() {
  if (timer_option6 == "a") {
    s = 'a'; //为是赋值
    detect_s(s);//调用检测s，执行继电器开关hex指令
    client.publish("esp32relaytimer6_return",time6.c_str());//mqtt发送
  }

  if (timer_option6 == "b") {
    s = 'b'; //为是赋值
    detect_s(s);//调用检测s，执行继电器开关hex指令
    client.publish("esp32relaytimer6_return",time6.c_str());//mqtt发送
  }

  if (timer_option6 == "c") {
    s = 'c'; //为是赋值
    detect_s(s);//调用检测s，执行继电器开关hex指令
    client.publish("esp32relaytimer6_return",time6.c_str());//mqtt发送
  }

  if (timer_option6 == "d") {
    s = 'd'; //为是赋值
    detect_s(s);//调用检测s，执行继电器开关hex指令
    client.publish("esp32relaytimer6_return",time6.c_str());//mqtt发送
  }

  if (timer_option6 == "e") {
    s = 'e'; //为是赋值
    detect_s(s);//调用检测s，执行继电器开关hex指令
    client.publish("esp32relaytimer6_return",time6.c_str());//mqtt发送
  }

  if (timer_option6 == "f") {
    s = 'f'; //为是赋值
    detect_s(s);//调用检测s，执行继电器开关hex指令
    client.publish("esp32relaytimer6_return",time6.c_str());//mqtt发送
  }

  if (timer_option6 == "g") {
    s = 'g'; //为是赋值
    detect_s(s);//调用检测s，执行继电器开关hex指令
    client.publish("esp32relaytimer6_return",time6.c_str());//mqtt发送
  }

  if (timer_option6 == "h") {
    s = 'h'; //为是赋值
    detect_s(s);//调用检测s，执行继电器开关hex指令
    client.publish("esp32relaytimer6_return",time6.c_str());//mqtt发送
  }

  if (timer_option6 == "i") {
    s = 'i'; //为是赋值
    detect_s(s);//调用检测s，执行继电器开关hex指令
    client.publish("esp32relaytimer6_return",time6.c_str());//mqtt发送
  }

  if (timer_option6 == "j") {
    s = 'j'; //为是赋值
    detect_s(s);//调用检测s，执行继电器开关hex指令
    client.publish("esp32relaytimer6_return",time6.c_str());//mqtt发送
  }

  if (timer_option6 == "k") {
    s = 'k'; //为是赋值
    detect_s(s);//调用检测s，执行继电器开关hex指令
    client.publish("esp32relaytimer6_return",time6.c_str());//mqtt发送
  }

  if (timer_option6 == "l") {
    s = 'l'; //为是赋值
    detect_s(s);//调用检测s，执行继电器开关hex指令
    client.publish("esp32relaytimer6_return",time6.c_str());//mqtt发送
  }
}


void timerOption7() {
  if (timer_option7 == "a") {
    s = 'a'; //为是赋值
    detect_s(s);//调用检测s，执行继电器开关hex指令
    client.publish("esp32relaytimer7_return",time7.c_str());//mqtt发送
  }

  if (timer_option7 == "b") {
    s = 'b'; //为是赋值
    detect_s(s);//调用检测s，执行继电器开关hex指令
    client.publish("esp32relaytimer7_return",time7.c_str());//mqtt发送
  }

  if (timer_option7 == "c") {
    s = 'c'; //为是赋值
    detect_s(s);//调用检测s，执行继电器开关hex指令
    client.publish("esp32relaytimer7_return",time7.c_str());//mqtt发送
  }

  if (timer_option7 == "d") {
    s = 'd'; //为是赋值
    detect_s(s);//调用检测s，执行继电器开关hex指令
    client.publish("esp32relaytimer7_return",time7.c_str());//mqtt发送
  }

  if (timer_option7 == "e") {
    s = 'e'; //为是赋值
    detect_s(s);//调用检测s，执行继电器开关hex指令
    client.publish("esp32relaytimer7_return",time7.c_str());//mqtt发送
  }

  if (timer_option7 == "f") {
    s = 'f'; //为是赋值
    detect_s(s);//调用检测s，执行继电器开关hex指令
    client.publish("esp32relaytimer7_return",time7.c_str());//mqtt发送
  }

  if (timer_option7 == "g") {
    s = 'g'; //为是赋值
    detect_s(s);//调用检测s，执行继电器开关hex指令
    client.publish("esp32relaytimer7_return",time7.c_str());//mqtt发送
  }

  if (timer_option7 == "h") {
    s = 'h'; //为是赋值
    detect_s(s);//调用检测s，执行继电器开关hex指令
    client.publish("esp32relaytimer7_return",time7.c_str());//mqtt发送
  }

  if (timer_option7 == "i") {
    s = 'i'; //为是赋值
    detect_s(s);//调用检测s，执行继电器开关hex指令
    client.publish("esp32relaytimer7_return",time7.c_str());//mqtt发送
  }

  if (timer_option7 == "j") {
    s = 'j'; //为是赋值
    detect_s(s);//调用检测s，执行继电器开关hex指令
    client.publish("esp32relaytimer7_return",time7.c_str());//mqtt发送
  }

  if (timer_option7 == "k") {
    s = 'k'; //为是赋值
    detect_s(s);//调用检测s，执行继电器开关hex指令
    client.publish("esp32relaytimer7_return",time7.c_str());//mqtt发送
  }

  if (timer_option7 == "l") {
    s = 'l'; //为是赋值
    detect_s(s);//调用检测s，执行继电器开关hex指令
    client.publish("esp32relaytimer7_return",time7.c_str());//mqtt发送
  }
}


void timerOption8() {
  if (timer_option8 == "a") {
    s = 'a'; //为是赋值
    detect_s(s);//调用检测s，执行继电器开关hex指令
    client.publish("esp32relaytimer8_return",time8.c_str());//mqtt发送
  }

  if (timer_option8 == "b") {
    s = 'b'; //为是赋值
    detect_s(s);//调用检测s，执行继电器开关hex指令
    client.publish("esp32relaytimer8_return",time8.c_str());//mqtt发送
  }

  if (timer_option8 == "c") {
    s = 'c'; //为是赋值
    detect_s(s);//调用检测s，执行继电器开关hex指令
    client.publish("esp32relaytimer8_return",time8.c_str());//mqtt发送
  }

  if (timer_option8 == "d") {
    s = 'd'; //为是赋值
    detect_s(s);//调用检测s，执行继电器开关hex指令
    client.publish("esp32relaytimer8_return",time8.c_str());//mqtt发送
  }

  if (timer_option8 == "e") {
    s = 'e'; //为是赋值
    detect_s(s);//调用检测s，执行继电器开关hex指令
    client.publish("esp32relaytimer8_return",time8.c_str());//mqtt发送
  }

  if (timer_option8 == "f") {
    s = 'f'; //为是赋值
    detect_s(s);//调用检测s，执行继电器开关hex指令
    client.publish("esp32relaytimer8_return",time8.c_str());//mqtt发送
  }

  if (timer_option8 == "g") {
    s = 'g'; //为是赋值
    detect_s(s);//调用检测s，执行继电器开关hex指令
    client.publish("esp32relaytimer8_return",time8.c_str());//mqtt发送
  }

  if (timer_option8 == "h") {
    s = 'h'; //为是赋值
    detect_s(s);//调用检测s，执行继电器开关hex指令
    client.publish("esp32relaytimer8_return",time8.c_str());//mqtt发送
  }

  if (timer_option8 == "i") {
    s = 'i'; //为是赋值
    detect_s(s);//调用检测s，执行继电器开关hex指令
    client.publish("esp32relaytimer8_return",time8.c_str());//mqtt发送
  }

  if (timer_option8 == "j") {
    s = 'j'; //为是赋值
    detect_s(s);//调用检测s，执行继电器开关hex指令
    client.publish("esp32relaytimer8_return",time8.c_str());//mqtt发送
  }

  if (timer_option8 == "k") {
    s = 'k'; //为是赋值
    detect_s(s);//调用检测s，执行继电器开关hex指令
    client.publish("esp32relaytimer8_return",time8.c_str());//mqtt发送
  }

  if (timer_option8 == "l") {
    s = 'l'; //为是赋值
    detect_s(s);//调用检测s，执行继电器开关hex指令
    client.publish("esp32relaytimer8_return",time8.c_str());//mqtt发送
  }
}


void timerOption9() {
  if (timer_option9 == "a") {
    s = 'a'; //为是赋值
    detect_s(s);//调用检测s，执行继电器开关hex指令
    client.publish("esp32relaytimer9_return",time9.c_str());//mqtt发送
  }

  if (timer_option9 == "b") {
    s = 'b'; //为是赋值
    detect_s(s);//调用检测s，执行继电器开关hex指令
    client.publish("esp32relaytimer9_return",time9.c_str());//mqtt发送
  }

  if (timer_option9 == "c") {
    s = 'c'; //为是赋值
    detect_s(s);//调用检测s，执行继电器开关hex指令
    client.publish("esp32relaytimer9_return",time9.c_str());//mqtt发送
  }

  if (timer_option9 == "d") {
    s = 'd'; //为是赋值
    detect_s(s);//调用检测s，执行继电器开关hex指令
    client.publish("esp32relaytimer9_return",time9.c_str());//mqtt发送
  }

  if (timer_option9 == "e") {
    s = 'e'; //为是赋值
    detect_s(s);//调用检测s，执行继电器开关hex指令
    client.publish("esp32relaytimer9_return",time9.c_str());//mqtt发送
  }

  if (timer_option9 == "f") {
    s = 'f'; //为是赋值
    detect_s(s);//调用检测s，执行继电器开关hex指令
    client.publish("esp32relaytimer9_return",time9.c_str());//mqtt发送
  }

  if (timer_option9 == "g") {
    s = 'g'; //为是赋值
    detect_s(s);//调用检测s，执行继电器开关hex指令
    client.publish("esp32relaytimer9_return",time9.c_str());//mqtt发送
  }

  if (timer_option9 == "h") {
    s = 'h'; //为是赋值
    detect_s(s);//调用检测s，执行继电器开关hex指令
    client.publish("esp32relaytimer9_return",time9.c_str());//mqtt发送
  }

  if (timer_option9 == "i") {
    s = 'i'; //为是赋值
    detect_s(s);//调用检测s，执行继电器开关hex指令
    client.publish("esp32relaytimer9_return",time9.c_str());//mqtt发送
  }

  if (timer_option9 == "j") {
    s = 'j'; //为是赋值
    detect_s(s);//调用检测s，执行继电器开关hex指令
    client.publish("esp32relaytimer9_return",time9.c_str());//mqtt发送
  }

  if (timer_option9 == "k") {
    s = 'k'; //为是赋值
    detect_s(s);//调用检测s，执行继电器开关hex指令
    client.publish("esp32relaytimer9_return",time9.c_str());//mqtt发送
  }

  if (timer_option9 == "l") {
    s = 'l'; //为是赋值
    detect_s(s);//调用检测s，执行继电器开关hex指令
    client.publish("esp32relaytimer9_return",time9.c_str());//mqtt发送
  }
}


void timerOption10() {
  if (timer_option10 == "a") {
    s = 'a'; //为是赋值
    detect_s(s);//调用检测s，执行继电器开关hex指令
    client.publish("esp32relaytimer10_return",time10.c_str());//mqtt发送
  }

  if (timer_option10 == "b") {
    s = 'b'; //为是赋值
    detect_s(s);//调用检测s，执行继电器开关hex指令
    client.publish("esp32relaytimer10_return",time10.c_str());//mqtt发送
  }

  if (timer_option10 == "c") {
    s = 'c'; //为是赋值
    detect_s(s);//调用检测s，执行继电器开关hex指令
    client.publish("esp32relaytimer10_return",time10.c_str());//mqtt发送
  }

  if (timer_option10 == "d") {
    s = 'd'; //为是赋值
    detect_s(s);//调用检测s，执行继电器开关hex指令
    client.publish("esp32relaytimer10_return",time10.c_str());//mqtt发送
  }

  if (timer_option10 == "e") {
    s = 'e'; //为是赋值
    detect_s(s);//调用检测s，执行继电器开关hex指令
    client.publish("esp32relaytimer10_return",time10.c_str());//mqtt发送
  }

  if (timer_option10 == "f") {
    s = 'f'; //为是赋值
    detect_s(s);//调用检测s，执行继电器开关hex指令
    client.publish("esp32relaytimer10_return",time10.c_str());//mqtt发送
  }

  if (timer_option10 == "g") {
    s = 'g'; //为是赋值
    detect_s(s);//调用检测s，执行继电器开关hex指令
    client.publish("esp32relaytimer10_return",time10.c_str());//mqtt发送
  }

  if (timer_option10 == "h") {
    s = 'h'; //为是赋值
    detect_s(s);//调用检测s，执行继电器开关hex指令
    client.publish("esp32relaytimer10_return",time10.c_str());//mqtt发送
  }

  if (timer_option10 == "i") {
    s = 'i'; //为是赋值
    detect_s(s);//调用检测s，执行继电器开关hex指令
    client.publish("esp32relaytimer10_return",time10.c_str());//mqtt发送
  }

  if (timer_option10 == "j") {
    s = 'j'; //为是赋值
    detect_s(s);//调用检测s，执行继电器开关hex指令
    client.publish("esp32relaytimer10_return",time10.c_str());//mqtt发送
  }

  if (timer_option10 == "k") {
    s = 'k'; //为是赋值
    detect_s(s);//调用检测s，执行继电器开关hex指令
    client.publish("esp32relaytimer10_return",time10.c_str());//mqtt发送
  }

  if (timer_option10 == "l") {
    s = 'l'; //为是赋值
    detect_s(s);//调用检测s，执行继电器开关hex指令
    client.publish("esp32relaytimer10_return",time10.c_str());//mqtt发送
  }
}

//LED闪烁指示灯功能
void led() {

  digitalWrite(2, HIGH); //内置LED灯亮
  
  delay(50);

    digitalWrite(2, LOW); //内置LED灯
  }
