

#include <ESP32Servo.h>
#include <WiFi.h>
#include <Firebase_ESP_Client.h>
#include "addons/TokenHelper.h"
#include "addons/RTDBHelper.h"

#define WiFI_SSID "vivo"
#define WiFi_PASSWORD "012345678"
#define API_KEY "AIzaSyBaHndPylidVRmZfd9aFitWl1Ll_eVVr8w"
#define DATABASE_URL "https://smart-last-day-default-rtdb.firebaseio.com/"

FirebaseData fbdo ,fbdo_s1, fbdo_s2 ,fbdo_s3 , fbdo_s4 , fbdo_s5 ,fbdo_s6 , fbdo_s7 ;   //handle any changes if any 
FirebaseAuth auth;   //for the authentecathion operation
FirebaseConfig config;  // required for configuratuon

unsigned long sendDataPrevMillis=0;   //for handle the time in unblocking way using millis function



bool signupOK= false;  //flag value sign up

bool safe_status;

#define tp T0
int touch =0;
#define PWMchannel1 0
#define led2 26    //for the pwm for the moter speed        **
#define led 13    // for the fan switch on and off
#define led3 15   //for the servo moter switch on and off 
#define led4 12   //for switch which case you wanna servo moter run in   
#define led5 25  //for the pwn you will get for zawya     *
#define PWMchannel2 1
#define moter 18
#define touchpin 23

#define RLed 19
#define WLed 22

float voltage =0.00;


bool key= true;    //switch on and off  dc moter [the fan]
bool key2= true;    // to switch on and off srvomoter
bool key3=false;     //to choose which case you want to apply the automatic rotating or the zawya mode

byte analog=150;     // for control the lighting      //this is the variable for controlling the speed of the fan

const int freq =5000;
const int resol=8;

const int freq2 =500;
const int resol2=8;

byte xanalog=0;
byte yanalog=0;

bool ledstatus=false;
bool ledstatus3=false;
bool ledstatus4=false;
bool ledstatus5=false;
bool ledstatus6=false;

Servo  myservo;
#define servopin 21
int servopos=0;       // for scale the value from 0 to 180
byte zawya=50;     


int pos =0;




void setup() {
  Serial.begin(115200);
pinMode (moter,OUTPUT);
pinMode (led,OUTPUT);
pinMode (led2,OUTPUT);


pinMode (led3,OUTPUT);
pinMode (led4,OUTPUT);
pinMode (led5,OUTPUT); 
pinMode (touchpin,OUTPUT);

pinMode (WLed,OUTPUT);
pinMode (RLed,OUTPUT);

ledcSetup(PWMchannel1,freq,resol);
ledcAttachPin (led2,PWMchannel1);

ledcSetup(PWMchannel2,freq2,resol2);
ledcAttachPin (led5,PWMchannel2);

WiFi.begin (WiFI_SSID, WiFi_PASSWORD);
 Serial.print ("Connecting to Wi-Fi"); 
 
 while (WiFi.status() != WL_CONNECTED)
  { 
    Serial.print("."); 
 delay (300);
}
Serial.println();
Serial.print ("Connected with IP: "); 
Serial.println (WiFi.localIP());
Serial.println();

config.api_key = API_KEY;
config.database_url = DATABASE_URL;

if (Firebase.signUp (&config, &auth, "", "") ) //for annumonous user signup in fire base setting
{
Serial.println("signUp OK"); 
signupOK = true;
}else
{
  Serial.printf ("%s\n", config.signer.signupError.message.c_str());
}

config.token_status_callback = tokenStatusCallback;
Firebase.begin (&config, &auth);
Firebase.reconnectWiFi (true );

  

   if (!Firebase.RTDB.beginStream (&fbdo_s1 , "/fan/analog"))    // this is a return function
  Serial.printf("stream 1 begin error, %s\n\n", fbdo_s1.errorReason () .c_str()); 

  if (!Firebase.RTDB.beginStream (&fbdo_s2 , "/fan/key"))    // this is a return function
  Serial.printf("stream 2 begin error, %s\n\n", fbdo_s2.errorReason () .c_str()); 

  if (!Firebase.RTDB.beginStream (&fbdo_s3 , "/servo/key2"))    // this is a return function
  Serial.printf("stream 3 begin error, %s\n\n", fbdo_s3.errorReason () .c_str()); 

  if (!Firebase.RTDB.beginStream (&fbdo_s4 , "/servo/key3"))    // this is a return function
  Serial.printf("stream 4 begin error, %s\n\n", fbdo_s4.errorReason () .c_str()); 

  if (!Firebase.RTDB.beginStream (&fbdo_s5 , "/servo/zawya"))   // this is a return function
  Serial.printf("stream 5 begin error, %s\n\n", fbdo_s5.errorReason () .c_str()); 

  if (!Firebase.RTDB.beginStream (&fbdo_s6 , "/output/LEDS/RedLed"))    // this is a return function
  Serial.printf("stream 6 begin error, %s\n\n", fbdo_s6.errorReason () .c_str());
 
  if (!Firebase.RTDB.beginStream (&fbdo_s7 , "/output/LEDS/whiteLed"))    // this is a return function
  Serial.printf("stream 7 begin error, %s\n\n", fbdo_s7.errorReason () .c_str());


}
void loop() {
  
touch =touchRead(tp);
  Serial.print ("touch =");
   Serial.println(touch);

   if (touch<40)
  {

    digitalWrite (touchpin,HIGH);
    Serial.print ("touch =");
    Serial.println("cautious some body tring to open the safe"); 
  safe_status=true;
     delay(1000);

  } else 
  {
    digitalWrite(touchpin,LOW);
    
    Serial.println("every thing is ok");
     safe_status=false;
delay(1000);
  }



key=digitalRead(led);


  if (key== false)
  {

analog = 0;
voltage= 0;

Serial.println("please run the fan");
analogWrite(moter,analog);

} else 
{
  analog=xanalog;
voltage= 3.30*analog /255.00;
Serial.print("the pin voltage =");
Serial.println(voltage);

Serial.print(" the fan is running at the speed of = ");
Serial.println(analog);
analogWrite(moter,analog);
 //delay (1000);
 
}

key2= digitalRead(led3);

if (key2==true){

   key3= digitalRead(led4);

    if (key3==false){         //the zawya mode




zawya= yanalog;
 
  pos =0;
    servopos =map  (zawya ,0,255, 0,160);
myservo.write(servopos);
Serial.print ("the ceta =");
Serial.println(servopos);
delay (100);
 }
  else   //for automatically rotating
  {
    
    servopos=0;
for ( pos=0; pos<=160 ;pos+=20)
{
myservo.write(pos);
  Serial.print("the ceta now is ==");
  Serial.println(pos);
delay (1000);
}
for ( pos=160; pos>=0;pos-=20)
{
  
  myservo.write(pos);
  Serial.print("the ceta now is ==");
  Serial.println(pos);
  delay (1000);
} 

  }  
 
} 

else 
  {
Serial.println("please open the servomoter");
  delay (1000);
   }

   




  if (Firebase.ready() && signupOK && (millis()-sendDataPrevMillis > 5000 || sendDataPrevMillis ==0))
  
   {
sendDataPrevMillis = millis();
 
//for knowing which voltage is on the pin
   
   if (Firebase. RTDB.setFloat (&fbdo, "Sensor/voltage", voltage)) 
{
   Serial.println(); Serial.print (voltage);
Serial.print("-successfully saved to:"+ fbdo.dataPath());
Serial.println(" (" + fbdo.dataType () + ")");
}
else
{
Serial.println("FAILED: " +fbdo.errorReason ());
}

// for sending the safe status

 if (Firebase. RTDB.setBool (&fbdo, "output/status", safe_status)) 
{
   Serial.println();
    Serial.print (safe_status);
Serial.print("-successfully saved to:"+ fbdo.dataPath());
Serial.println(" (" + fbdo.dataType () + ")");
}
else
{
Serial.println("FAILED: " +fbdo.errorReason ());
}

// for knowing if any one is near to the safe

if (Firebase. RTDB.setFloat (&fbdo, "output/touch", touch)) 
{
   Serial.println("the sequre reading is ="); 
   Serial.print (touch);
Serial.print("-successfully saved to:"+ fbdo.dataPath());
Serial.println(" (" + fbdo.dataType () + ")");
}
else
{
Serial.println("FAILED: " +fbdo.errorReason ());
}

   } 
  
   // for getting info from the firebase  
   //the motor speed

 //for the streaming intervales

  if (Firebase.ready() && signupOK) {



  if (!Firebase.RTDB.readStream(&fbdo_s1))
  Serial.printf("stream 1 read error, %s\n\n", fbdo_s1.errorReason () .c_str());
if  (fbdo_s1.streamAvailable())
    {
if (fbdo_s1.dataType() == "int")
{
xanalog = fbdo_s1.intData();
Serial.print("-successfully read from "+ fbdo_s1.dataPath());
Serial.println("Successful READ from "+fbdo_s1.dataPath() + ": " + xanalog + " (" +fbdo_s1. dataType() + ")");
ledcWrite(PWMchannel1 , xanalog);

Serial.print("the analog read is = ");
Serial.println(xanalog);
}
    }

// the key of the motor to switch on and off 

if (!Firebase.RTDB.readStream(&fbdo_s2))
   Serial.printf("stream 2 read error, %s\n\n", fbdo_s2.errorReason () .c_str());
if  (fbdo_s2.streamAvailable())
    {
if (fbdo_s2.dataType() == "boolean")
{
ledstatus= fbdo_s2.boolData();
Serial.print("-successfully read from "+ fbdo_s2.dataPath());
Serial.println("Successful READ from "+fbdo_s2.dataPath() + ": " + ledstatus +" (" +fbdo_s2. dataType() + ")");
digitalWrite(led,ledstatus);
Serial.print("the fan status is =");
Serial.println(ledstatus);
delay (1000);
}
   }
   
   // for the servo moter switch on and off

   if (!Firebase.RTDB.readStream(&fbdo_s3))
    Serial.printf("stream 3 read error, %s\n\n", fbdo_s3.errorReason () .c_str());
if  (fbdo_s3.streamAvailable())
    {
if (fbdo_s3.dataType() == "boolean")
{
ledstatus3= fbdo_s3.boolData();
Serial.print("-successfully read from "+ fbdo_s3.dataPath());
Serial.println("Successful READ from "+fbdo_s3.dataPath() + ": " + ledstatus3 +" (" +fbdo_s3. dataType() + ")");
digitalWrite(led3,ledstatus3);
Serial.print("the servo moter status is =");
Serial.println(ledstatus3);
delay (1000);
}
    }

   // for swithch the case between the servo moter cases automatically or by zawya

if (!Firebase.RTDB.readStream(&fbdo_s4))
   Serial.printf("stream 4 read error, %s\n\n", fbdo_s4.errorReason () .c_str());
if  (fbdo_s4.streamAvailable())
    {
if (fbdo_s4.dataType() == "boolean")
{
ledstatus4= fbdo_s4.boolData();
Serial.print("-successfully read from "+ fbdo_s4.dataPath());
Serial.println("Successful READ from "+fbdo_s4.dataPath() + ": " + ledstatus4 +" (" +fbdo_s4. dataType() + ")");
digitalWrite(led4,ledstatus4);
Serial.print("the servo moter case is on =");
Serial.println(ledstatus4);
delay (1000);
}
} 
 // for getting zawya for servo moter


if (!Firebase.RTDB.readStream(&fbdo_s5))
 Serial.printf("stream 5 read error, %s\n\n", fbdo_s5.errorReason () .c_str());
if  (fbdo_s5.streamAvailable())
    {
if (fbdo_s5.dataType() == "int")
{
yanalog = fbdo_s5.intData();
Serial.print("-successfully read from "+ fbdo_s5.dataPath());
Serial.println("Successful READ from "+fbdo_s5.dataPath() + ": " + yanalog + " (" +fbdo_s5. dataType() + ")");
ledcWrite(PWMchannel2 , yanalog);


Serial.print("servo ceta is =");
Serial.println(yanalog);

}
    }

   // for the white led on and off

if (!Firebase.RTDB.readStream(&fbdo_s6))
   Serial.printf("stream 6 read error, %s\n\n", fbdo_s6.errorReason () .c_str());
if  (fbdo_s6.streamAvailable())
    {
if (fbdo_s6.dataType() == "boolean")
{
ledstatus5= fbdo_s6.boolData();
Serial.print("-successfully read from "+ fbdo_s6.dataPath());
Serial.println("Successful READ from "+fbdo_s6.dataPath() + ": " + ledstatus5 +" (" +fbdo_s6.dataType() + ")");
digitalWrite(RLed,ledstatus5);
Serial.print("the red led status is =");
Serial.println(ledstatus5);
delay (1000);
}
   }
   
   // for the white led switch on and off

if (!Firebase.RTDB.readStream(&fbdo_s7))
    Serial.printf("stream 7 read error, %s\n\n", fbdo_s7.errorReason () .c_str());
if  (fbdo_s7.streamAvailable())
    {
if (fbdo_s7.dataType() == "boolean")
{
ledstatus6= fbdo_s7.boolData();
Serial.print("-successfully read from "+ fbdo_s7.dataPath());
Serial.println("Successful READ from "+fbdo_s7.dataPath() + ": " + ledstatus6 +" (" +fbdo_s7. dataType() + ")");
digitalWrite(WLed,ledstatus6);
Serial.print("the white led status is =");
Serial.println(ledstatus6);
delay (1000);
}
   }
   
  }
}

