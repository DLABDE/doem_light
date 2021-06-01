/*************
 * 宿舍灯光音响控制器
 * 使用NODEMCV控制器
 * 引脚使用说明：
 * D0--音响开关
 * D1--红外感应
 * D2--LED
 * D5--WIFI控制开关
 * D6--灯带信号
 * D7--OPEN ALL
 * 
**************/
#define BLINKER_WIFI
#include <Blinker.h>
#include "FastLED.h" 

//配置入网标识
//0066b30555f2
char auth[] = "0066b30555f2";
#define MAX_CHAR 25

//配置灯带信息       
#define NUM_LEDS 14             // LED灯珠数量
#define DATA_PIN 6              // Arduino输出控制信号引脚
#define LED_TYPE WS2812         // LED灯带型号
#define COLOR_ORDER GRB         // RGB灯珠中红色、绿色、蓝色LED的排列顺序
//uint8_t max_bright =0;       // LED亮度控制变量，可使用数值为 0 ～ 255
#define twinkleInterval  15    //  闪烁间隔时间 (毫秒)
#define twinkleChance 150        //  闪烁数量，数值越大闪烁越多（0-255）

CRGB leds[NUM_LEDS];            // 建立光带leds


//全局变量
bool light_call=0;//音响
unsigned char led_val=0;//LED
bool openpeocom=0;//红外检测开关
bool sleepok=0;
bool peoplehere=0;

///////////////////////WIFI控制
bool get_wifimes(){
  pinMode(D5, INPUT);
  return analogRead(D5);
}

void setup_wifimes(){
  pinMode(D5, OUTPUT);
  digitalWrite(D5, 0);
}

/////////////////////////////////////////////////音响
BlinkerButton light("btn-abc");
void light_callback(const String & state) {
  //light.icon("fas fa-lightbulb");
  if (state=="on"){
    digitalWrite(D0,1);
    light.text("音响关闭");
    light.color("#00FF00");////!!!注意!.print因该在最后
    light.print("on");
    light_call=1;
  }
  else if(state=="off"){
    digitalWrite(D0,0);
    light.text("音响打开");
    light.color("#000000");
    light.print("off");
    light_call=0;
  }
   //Blinker.vibrate();
}
void setup_light(){
  pinMode(D0, OUTPUT);
  digitalWrite(D0, 0);
  light.attach(light_callback);
  light_callback("off"); 
}


//////////////////////////////////////////////////////////////////////flash-led
BlinkerRGB led("col-rgb");
void led_callback(uint8_t r, uint8_t g, uint8_t b, uint8_t a){
  BLINKER_LOG("r ", r);
  BLINKER_LOG("g ", g);
  BLINKER_LOG("b ", b);
  BLINKER_LOG("a ", a);

  for(uint8_t countt=0;countt<NUM_LEDS;countt++){
    leds[countt].r = r;     
    leds[countt].g = g;    
    leds[countt].b = b;
    FastLED.setBrightness(a); 
    FastLED.show();
    }
}
void setup_led(){
  led.attach(led_callback);
}

/////////////////////////////////////////////////////////light-led
BlinkerSlider lightled("ran-oiv");
void lightled_callback(int32_t value)
{
    if(value<=250){
      BLINKER_LOG("value: ", value);
      analogWrite(D2,value);
      led_val=value;
    }
    else if(value==0){
      digitalWrite(D2, 0);
    }
    else{
      BLINKER_LOG("value: !0");
      digitalWrite(D2,1);
      led_val=255;
    }
}
void setup_ligthled(){
  pinMode(D2, OUTPUT);
  digitalWrite(D2, 0);
  lightled.attach(lightled_callback);
}


///////////////////////////////////////////////////////////people
BlinkerButton peocom("btn-3uv");
void peocom_callback(const String & state) {
  //light.icon("fas fa-lightbulb");
  if (state=="on"){
    openpeocom=1;
    peocom.text("关闭");
    peocom.color("#00FF00");
    peocom.print("on");
  }
  else if(state=="off"){
    openpeocom=0;
    peocom.text("打开");
    peocom.color("#000000");
    peocom.print("off");
  }
   //Blinker.vibrate();
}
void setup_peocom(){
  peocom.attach(peocom_callback);
}

////////////////////////////////////////////////////sleep
BlinkerButton ifsleep("btn-o7d");
void ifsleep_callback(const String & state) {
  //light.icon("fas fa-lightbulb");
  if (state=="on"){
    sleepok=1;
    ifsleep.text("关闭");
    ifsleep.color("#00FF00");
    ifsleep.print("on");
  }
  else if(state=="off"){
    sleepok=0;
    ifsleep.text("打开");
    ifsleep.color("#000000");
    ifsleep.print("off");
  }
   //Blinker.vibrate();
}
void setup_ifsleep(){
  ifsleep.attach(ifsleep_callback);
}

//////////////////////////////////////////////heartbeat
void heartbeat(){
  //getdist();
  lightled_callback(led_val);
  
  if(sleepok==1){
    ifsleep_callback("on");
  }
  else if(sleepok==0){
    ifsleep_callback("off");
  }
  
  if(light_call==1){
    light_callback("on");
  }
  else if(light_call==0){
    light_callback("off");
  }
  
  if(openpeocom==1){
    peocom_callback("on");
  }
  else if(openpeocom==0){
    peocom_callback("off");
  }
  
}
///////////////////////////////////////////////////////data
void dataread(const String & state){
  BLINKER_LOG("data is:",state);
}


void setup() {
  //配置入网信息
  char ssid[MAX_CHAR] = "LAPTOP-A44RPV5S 7536";
  char pswd[MAX_CHAR] = "01020100";
  char ssid_a[MAX_CHAR] = "HUAWEI nova 3e";
  char pswd_a[MAX_CHAR] = "01020300";
  //setup_wifimes();
  if(get_wifimes()==0){
    for(int i=0;i<MAX_CHAR;i++){
      ssid[i]=ssid_a[i];
      pswd[i]=pswd_a[i];
    }
  }
  else{
    light_callback("on");
  }
  
  //灯带初始化
  LEDS.addLeds<LED_TYPE, DATA_PIN, COLOR_ORDER>(leds, NUM_LEDS); 
  FastLED.setBrightness(0);
  
  //debug初始化
  Serial.begin(9600);
  BLINKER_DEBUG.stream(Serial);


  //用户初始化
  setup_peocom();
  setup_light();
  setup_led();
  setup_ligthled();
  setup_ifsleep();
  pinMode(D1,INPUT);//初始化人体检测信号端口
  pinMode(D7,INPUT);//初始化OPENALL信号端口
  
  if(digitalRead(D7)==1){
    fill_solid(leds, 14, CRGB::White);
    FastLED.setBrightness(0); 
    FastLED.show();
    analogWrite(D2,0);
  }
  else{
    fill_solid(leds, 14, CRGB::White);
    FastLED.setBrightness(100); 
    FastLED.show();
    analogWrite(D2,180);
  }

  if(get_wifimes()==0){
    light_callback("off");
    }
  else{
    light_callback("on");
  }

  //其他初始化
  Blinker.begin(auth, ssid, pswd);
  Blinker.attachData(dataread);
  Blinker.attachHeartbeat(heartbeat);
}

//用户循环
void user_make()
{ 
  //如果打开睡眠
  if(sleepok==1){
    light_callback("off");
    led_callback(0, 0, 0, 0);
    lightled_callback(0);
  }
  //如果打开people
  else if(openpeocom==1){
    if(digitalRead(D1)==1){
      peoplehere=1;
      //BLINKER_LOG("people in!!");
    }
    else if(digitalRead(D1)==0){
      peoplehere=0;
      light_callback("off");
      led_callback(0, 0, 0, 0);
      lightled_callback(0);
      //BLINKER_LOG("pppppppppeople out!!");
    }
  }
}

void loop() {
  user_make();
  Blinker.run();
}
