#include <AltSoftSerial.h>
#include <Tin
#include <AltSoftSerial.h>
#include <TinyGPS++.h>

#include <SoftwareSerial.h>
#include <math.h>

#include<Wire.h>

#include<LiquidCrystal.h>
LiquidCrystal lcd(7, 8, 9, 10, 11, 12);



//--------------------------------------------------------------
//emergency phone number with country code
const String EMERGENCY_PHONE =  "+917620554318";
const String EMERGENCY_PHONE1 = "+919021546914";
const String EMERGENCY_PHONE2 = "+918010858994";
const String EMERGENCY_PHONE3 = "+91";
const String EMERGENCY_PHONE4 = "+91";
//--------------------------------------------------------------
//GSM Module RX pin to Arduino 3
//GSM Module TX pin to Arduino 2
#define rxPin 2
#define txPin 3
SoftwareSerial sim800(rxPin,txPin);
//--------------------------------------------------------------
//GPS Module RX pin to Arduino 4
//GPS Module TX pin to Arduino 5
#define rxPin1 4
#define txPin1 5
AltSoftSerial neogps (rxPin1,txPin1);
TinyGPSPlus gps;
//--------------------------------------------------------------
String sms_status,sender_number,received_date,msg;
String latitude, longitude;
//--------------------------------------------------------------
#define BUZZER 13
#define BUTTON 6
#define LED A2
#define RELAY A7

#define ALCOHOL A1
#define VIBRATION A0

//--------------------------------------------------------------
#define xPin A5
#define yPin A4
#define zPin A3
#define Thres_Val 260
//--------------------------------------------------------------

byte updateflag;
int value1;
int xaxis = 0, yaxis = 0, zaxis = 0;
int deltx = 0, delty = 0, deltz = 0;
int vibration = 2, devibrate = 75;
int magnitude = 0;
int sensitivity = 20;
double angle;
boolean impact_detected = false;
//Used to run impact routine every 2mS.
unsigned long time1;
unsigned long impact_time;
unsigned long alert_delay = 10000; //10 seconds
//--------------------------------------------------------------
char Incoming_value = 0;



/*****************************************************************************************
 * setup() function
 *****************************************************************************************/
void setup()
{
  //--------------------------------------------------------------
  //Serial.println("Arduino serial initialize");
  Serial.begin(9600);
  //--------------------------------------------------------------
  //Serial.println("SIM800L serial initialize");
  sim800.begin(9600);
  //--------------------------------------------------------------
  //Serial.println("NEO6M serial initialize");
  neogps.begin(9600);
  //--------------------------------------------------------------
  pinMode(BUZZER, OUTPUT);
  pinMode(RELAY, OUTPUT);
  pinMode(LED, OUTPUT);
 
  pinMode(BUTTON, INPUT_PULLUP);

  pinMode(xPin, INPUT_PULLUP);

  pinMode(yPin, INPUT_PULLUP);

  pinMode(zPin, INPUT_PULLUP);

 
  pinMode(VIBRATION, INPUT_PULLUP);
 
  //--------------------------------------------------------------
  lcd.begin(16,2);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Iot Based Smart");
  lcd.setCursor(0, 1);
  lcd.print("School Bus monitoring");
  delay(2000);


  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Work Mode ...");
  lcd.setCursor(0, 1);
  lcd.print("Monitoring Bus");
  delay(2000);

 
  //--------------------------------------------------------------
  sms_status = "";
  sender_number="";
  received_date="";
  msg="";
  //--------------------------------------------------------------
  sim800.println("AT"); //Check GSM Module
  delay(1000);
  //SendAT("AT", "OK", 2000); //Check GSM Module
  sim800.println("ATE1"); //Echo ON
  delay(1000);
  //SendAT("ATE1", "OK", 2000); //Echo ON
  sim800.println("AT+CPIN?"); //Check SIM ready
  delay(1000);
  //SendAT("AT+CPIN?", "READY", 2000); //Check SIM ready
  sim800.println("AT+CMGF=1"); //SMS text mode
  delay(1000);
  //SendAT("AT+CMGF=1", "OK", 2000); //SMS text mode
  sim800.println("AT+CNMI=1,1,0,0,0"); /// Decides how newly arrived SMS should be handled
  delay(1000);
  //SendAT("AT+CNMI=1,1,0,0,0", "OK", 2000); //set sms received format
  //AT +CNMI = 2,1,0,0,0 - AT +CNMI = 2,2,0,0,0 (both are same)
  //--------------------------------------------------------------
  time1 = micros();
  //Serial.print("time1 = "); Serial.println(time1);
  //--------------------------------------------------------------
  //read calibrated values. otherwise false impact will trigger
  //when you reset your Arduino. (By pressing reset button)
  xaxis = analogRead(xPin);
  yaxis = analogRead(yPin);
  zaxis = analogRead(zPin);
  //--------------------------------------------------------------
}



/*****************************************************************************************
 * loop() function
 *****************************************************************************************/
void loop()
{

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Waiting For Student");
  lcd.setCursor(0, 1);
  lcd.print("Monitoring Bus");
  delay(2000);

 
  if(digitalRead(xPin) == LOW)
  {
    delay(200);
    digitalWrite(BUZZER, LOW);
    impact_detected = false;
    impact_time = 0;

      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Student 1 On Board");
      lcd.setCursor(0, 1);
      lcd.print("Sending Location");
      delay(2000);

      getGps();

      delay(1000);
      sendAlert();
      delay(10000);
     
     
     
   
  }

  else if(digitalRead(yPin) == LOW)
  {
    delay(200);
    digitalWrite(BUZZER, LOW);
    impact_detected = false;
    impact_time = 0;

      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Student 2 On Board");
      lcd.setCursor(0, 1);
      lcd.print("Sending Location");
      delay(2000);

      getGps();

     
      delay(1000);
      sendAlert1();
      delay(10000);
     
     
   
  }


  if(digitalRead(zPin) == LOW)
  {
    delay(200);
    digitalWrite(BUZZER, LOW);
    impact_detected = false;
    impact_time = 0;

      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Student 3 On Board");
      lcd.setCursor(0, 1);
      lcd.print("Sending Location");
      delay(2000);

      getGps();

     
     
      delay(1000);
      sendAlert2();
      delay(10000);
     
   
  }
 
  //--------------------------------------------------------------
  while(sim800.available())
  {
    parseData(sim800.readString());
  }
  //--------------------------------------------------------------
  //while(Serial.available())  
  //{
    //sim800.println(Serial.readString());
  //}
  //--------------------------------------------------------------

  value1 = analogRead(ALCOHOL);
  if ( value1 > Thres_Val )
   {
     digitalWrite ( LED , HIGH );    //digitalWrite(Buzzer,HIGH);
    tone(BUZZER, 1000);
    digitalWrite(RELAY, LOW);
   getGps();
   Serial.print(value1);   // send a capital A
   Serial.print("|");   // send a capital A
   Serial.print(Thres_Val);   // send a capital A
   Serial.print("|");   // send a capital A
   Serial.print(latitude);   // send a capital A
   Serial.print("|");   // send a capital A
   Serial.print(longitude);   // send a capital A
 }
 else
 {
   digitalWrite(LED, LOW);
   //digitalWrite(Buzzer,LOW);
   delay(1000);
   noTone(BUZZER);
 
   getGps();
   Serial.print(value1);   // send a capital A
   Serial.print("|");   // send a capital A
   Serial.print(Thres_Val);   // send a capital A
   Serial.print("|");   // send a capital A
   Serial.print(latitude);   // send a capital A
   Serial.print("|");   // send a capital A
   Serial.print(longitude);   // send a capital A
   
 }



}





/*****************************************************************************************
 * Impact() function
 *****************************************************************************************/
void Impact()
{
  //--------------------------------------------------------------
  time1 = micros(); // resets time value
  //--------------------------------------------------------------
  //int oldx = xaxis; //store previous axis readings for comparison
  //int oldy = yaxis;
  //int oldz = zaxis;


 

  xaxis = analogRead(xPin);
  yaxis = analogRead(yPin);
  zaxis = analogRead(zPin);
 
  //--------------------------------------------------------------
  //loop counter prevents false triggering. Vibration resets if there is an impact. Don't detect new changes until that "time" has passed.
  //vibration--;
  //Serial.print("Vibration = "); Serial.println(vibration);
  ///if(vibration < 0) vibration = 0;                                
  //Serial.println("Vibration Reset!");
 
  ///if(vibration > 0) return;
  //--------------------------------------------------------------
  ///deltx = xaxis - oldx;                                          
  ///delty = yaxis - oldy;
  //deltz = zaxis - oldz;
 
  //Magnitude to calculate force of impact.
  //magnitude = sqrt(sq(deltx) + sq(delty) + sq(deltz));
  //NNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNN
  //if (magnitude >= sensitivity) //impact detected

  if(digitalRead(VIBRATION) == LOW)
  {
    updateflag=1;
    // reset anti-vibration counter
    vibration = devibrate;
    magnitude = 50;
  }
  //NNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNN
  else
  {
    //if (magnitude > 15)
      //Serial.println(magnitude);
    //reset magnitude of impact to 0
    magnitude=0;
  }
  //NNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNN
}





/*****************************************************************************************
 * parseData() function
 *****************************************************************************************/
void parseData(String buff){
  //Serial.println(buff);

  unsigned int len, index;
  //--------------------------------------------------------------
  //Remove sent "AT Command" from the response string.
  index = buff.indexOf("\r");
  buff.remove(0, index+2);
  buff.trim();
  //NNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNN
  if(buff != "OK"){
    //--------------------------------------------------------------
    index = buff.indexOf(":");
    String cmd = buff.substring(0, index);
    cmd.trim();
   
    buff.remove(0, index+2);
    //Serial.println(buff);
    //--------------------------------------------------------------
    if(cmd == "+CMTI"){
      //get newly arrived memory location and store it in temp
      //temp = 4
      index = buff.indexOf(",");
      String temp = buff.substring(index+1, buff.length());
      temp = "AT+CMGR=" + temp + "\r";
      //AT+CMGR=4 i.e. get message stored at memory location 4
      sim800.println(temp);
    }
    //--------------------------------------------------------------
    else if(cmd == "+CMGR"){
      //extractSms(buff);
      //Serial.println(buff.indexOf(EMERGENCY_PHONE));
      if(buff.indexOf(EMERGENCY_PHONE) > 1){
        buff.toLowerCase();
        //Serial.println(buff.indexOf("get gps"));
        if(buff.indexOf("get gps") > 1){
          getGps();
          String sms_data;
          sms_data = "GPS Location Data\r";
          sms_data += "http://maps.google.com/maps?q=loc:";
          sms_data += latitude + "," + longitude;
       
          sendSms(sms_data);

           Serial.print(value1);   // send a capital A
           Serial.print("|");   // send a capital A
           Serial.print(Thres_Val);   // send a capital A
           Serial.print("|");   // send a capital A
           Serial.print(latitude);   // send a capital A
           Serial.print("|");   // send a capital A
           Serial.print(longitude);   // send a capital A
   
        }
      }
    }
    //--------------------------------------------------------------
  }
  else{
  //The result of AT Command is "OK"
  }
  //NNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNN
}


/*****************************************************************************************
 * getGps() Function
*****************************************************************************************/
void getGps()
{
  // Can take up to 60 seconds
  boolean newData = false;
  for (unsigned long start = millis(); millis() - start < 2000;){
    while (neogps.available()){
      if (gps.encode(neogps.read())){
        newData = true;
        break;
      }
    }
  }
 
  if (newData) //If newData is true
  {
    latitude = String(gps.location.lat(), 6);
    longitude = String(gps.location.lng(), 6);
    newData = false;
  }
  else {
    //Serial.println("No GPS data is available");
    latitude = "20.924708";
    longitude = "74.779572";
  }

  //Serial.print("Latitude= "); Serial.println(latitude);
  //Serial.print("Lngitude= "); Serial.println(longitude);
}




/*****************************************************************************************
* sendAlert() function
*****************************************************************************************/
void sendAlert()
{
  String sms_data;
  sms_data = "Student On Board Alert!!\r";
  sms_data += "http://maps.google.com/maps?q=loc:";
  sms_data += latitude + "," + longitude;

  sendSms(sms_data);
}

void sendAlert1()
{
  String sms_data;
  sms_data = "Student On Board Alert!!\r";
  sms_data += "http://maps.google.com/maps?q=loc:";
  sms_data += latitude + "," + longitude;

  sendSms1(sms_data);
}


void sendAlert2()
{
  String sms_data;
  sms_data = "Student On Board Alert!!\r";
  sms_data += "http://maps.google.com/maps?q=loc:";
  sms_data += latitude + "," + longitude;

  sendSms2(sms_data);
}


void sendAlert3()
{
  String sms_data;
  sms_data = "Student On Board Alert!!\r";
  sms_data += "http://maps.google.com/maps?q=loc:";
  sms_data += latitude + "," + longitude;

  sendSms3(sms_data);
}


void sendAlert4()
{
  String sms_data;
  sms_data = "Student On Board Alert!!\r";
  sms_data += "http://maps.google.com/maps?q=loc:";
  sms_data += latitude + "," + longitude;

  sendSms4(sms_data);
}




/*****************************************************************************************
* makeCall() function
*****************************************************************************************/
void makeCall()
{
  //Serial.println("calling....");
  sim800.println("ATD"+EMERGENCY_PHONE+";");
  delay(20000); //20 sec delay
  sim800.println("ATH");
  delay(1000); //1 sec delay
}


void makeCall1()
{
  //Serial.println("calling....");
  sim800.println("ATD"+EMERGENCY_PHONE1+";");
  delay(20000); //20 sec delay
  sim800.println("ATH");
  delay(1000); //1 sec delay
}

void makeCall2()
{
  //Serial.println("calling....");
  sim800.println("ATD"+EMERGENCY_PHONE2+";");
  delay(20000); //20 sec delay
  sim800.println("ATH");
  delay(1000); //1 sec delay
}

void makeCall3()
{
  //Serial.println("calling....");
  sim800.println("ATD"+EMERGENCY_PHONE3+";");
  delay(20000); //20 sec delay
  sim800.println("ATH");
  delay(1000); //1 sec delay
}

void makeCall4()
{
  //Serial.println("calling....");
  sim800.println("ATD"+EMERGENCY_PHONE4+";");
  delay(20000); //20 sec delay
  sim800.println("ATH");
  delay(1000); //1 sec delay
}




/*****************************************************************************************
 * sendSms() function
 *****************************************************************************************/
 void sendSms(String text)
{
  //return;
  sim800.print("AT+CMGF=1\r");
  delay(1000);
  sim800.print("AT+CMGS=\""+EMERGENCY_PHONE+"\"\r");
  delay(1000);
  sim800.print(text);
  delay(100);
  sim800.write(0x1A); //ascii code for ctrl-26 //sim800.println((char)26); //ascii code for ctrl-26
  delay(1000);
  //Serial.println("SMS Sent Successfully.");
}


 void sendSms1(String text)
{
  //return;
  sim800.print("AT+CMGF=1\r");
  delay(1000);
  sim800.print("AT+CMGS=\""+EMERGENCY_PHONE1+"\"\r");
  delay(1000);
  sim800.print(text);
  delay(100);
  sim800.write(0x1A); //ascii code for ctrl-26 //sim800.println((char)26); //ascii code for ctrl-26
  delay(1000);
  //Serial.println("SMS Sent Successfully.");
}


 void sendSms2(String text)
{
  //return;
  sim800.print("AT+CMGF=1\r");
  delay(1000);
  sim800.print("AT+CMGS=\""+EMERGENCY_PHONE2+"\"\r");
  delay(1000);
  sim800.print(text);
  delay(100);
  sim800.write(0x1A); //ascii code for ctrl-26 //sim800.println((char)26); //ascii code for ctrl-26
  delay(1000);
  //Serial.println("SMS Sent Successfully.");
}


 void sendSms3(String text)
{
  //return;
  sim800.print("AT+CMGF=1\r");
  delay(1000);
  sim800.print("AT+CMGS=\""+EMERGENCY_PHONE3+"\"\r");
  delay(1000);
  sim800.print(text);
  delay(100);
  sim800.write(0x1A); //ascii code for ctrl-26 //sim800.println((char)26); //ascii code for ctrl-26
  delay(1000);
  //Serial.println("SMS Sent Successfully.");
}


 void sendSms4(String text)
{
  //return;
  sim800.print("AT+CMGF=1\r");
  delay(1000);
  sim800.print("AT+CMGS=\""+EMERGENCY_PHONE4+"\"\r");
  delay(1000);
  sim800.print(text);
  delay(100);
  sim800.write(0x1A); //ascii code for ctrl-26 //sim800.println((char)26); //ascii code for ctrl-26
  delay(1000);
  //Serial.println("SMS Sent Successfully.");
}






/*****************************************************************************************
 * SendAT() function
 *****************************************************************************************/
boolean SendAT(String at_command, String expected_answer, unsigned int timeout){

    uint8_t x=0;
    boolean answer=0;
    String response;
    unsigned long previous;
   
    //Clean the input buffer
    while( sim800.available() > 0) sim800.read();

    sim800.println(at_command);
   
    x = 0;
    previous = millis();

    //this loop waits for the answer with time out
    do{
        //if there are data in the UART input buffer, reads it and checks for the asnwer
        if(sim800.available() != 0){
            response += sim800.read();
            x++;
            // check if the desired answer (OK) is in the response of the module
            if(response.indexOf(expected_answer) > 0){
                answer = 1;
                break;
            }
        }
    }while((answer == 0) && ((millis() - previous) < timeout));

  //Serial.println(response);
  return answer;
}yGPS
#include <AltSoftSerial.h>
#include <TinyGPS++.h>

#include <SoftwareSerial.h>
#include <math.h>

#include<Wire.h>

#include<LiquidCrystal.h>
LiquidCrystal lcd(7, 8, 9, 10, 11, 12);



//--------------------------------------------------------------
//emergency phone number with country code
const String EMERGENCY_PHONE =  "+917620554318";
const String EMERGENCY_PHONE1 = "+919021546914";
const String EMERGENCY_PHONE2 = "+918010858994";
const String EMERGENCY_PHONE3 = "+91";
const String EMERGENCY_PHONE4 = "+91";
//--------------------------------------------------------------
//GSM Module RX pin to Arduino 3
//GSM Module TX pin to Arduino 2
#define rxPin 2
#define txPin 3
SoftwareSerial sim800(rxPin,txPin);
//--------------------------------------------------------------
//GPS Module RX pin to Arduino 4
//GPS Module TX pin to Arduino 5
#define rxPin1 4
#define txPin1 5
AltSoftSerial neogps (rxPin1,txPin1);
TinyGPSPlus gps;
//--------------------------------------------------------------
String sms_status,sender_number,received_date,msg;
String latitude, longitude;
//--------------------------------------------------------------
#define BUZZER 13
#define BUTTON 6
#define LED A2
#define RELAY A7

#define ALCOHOL A1
#define VIBRATION A0

//--------------------------------------------------------------
#define xPin A5
#define yPin A4
#define zPin A3
#define Thres_Val 260
//--------------------------------------------------------------

byte updateflag;
int value1;
int xaxis = 0, yaxis = 0, zaxis = 0;
int deltx = 0, delty = 0, deltz = 0;
int vibration = 2, devibrate = 75;
int magnitude = 0;
int sensitivity = 20;
double angle;
boolean impact_detected = false;
//Used to run impact routine every 2mS.
unsigned long time1;
unsigned long impact_time;
unsigned long alert_delay = 10000; //10 seconds
//--------------------------------------------------------------
char Incoming_value = 0;



/*****************************************************************************************
 * setup() function
 *****************************************************************************************/
void setup()
{
  //--------------------------------------------------------------
  //Serial.println("Arduino serial initialize");
  Serial.begin(9600);
  //--------------------------------------------------------------
  //Serial.println("SIM800L serial initialize");
  sim800.begin(9600);
  //--------------------------------------------------------------
  //Serial.println("NEO6M serial initialize");
  neogps.begin(9600);
  //--------------------------------------------------------------
  pinMode(BUZZER, OUTPUT);
  pinMode(RELAY, OUTPUT);
  pinMode(LED, OUTPUT);
 
  pinMode(BUTTON, INPUT_PULLUP);

  pinMode(xPin, INPUT_PULLUP);

  pinMode(yPin, INPUT_PULLUP);

  pinMode(zPin, INPUT_PULLUP);

 
  pinMode(VIBRATION, INPUT_PULLUP);
 
  //--------------------------------------------------------------
  lcd.begin(16,2);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Iot Based Smart");
  lcd.setCursor(0, 1);
  lcd.print("School Bus monitoring");
  delay(2000);


  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Work Mode ...");
  lcd.setCursor(0, 1);
  lcd.print("Monitoring Bus");
  delay(2000);

 
  //--------------------------------------------------------------
  sms_status = "";
  sender_number="";
  received_date="";
  msg="";
  //--------------------------------------------------------------
  sim800.println("AT"); //Check GSM Module
  delay(1000);
  //SendAT("AT", "OK", 2000); //Check GSM Module
  sim800.println("ATE1"); //Echo ON
  delay(1000);
  //SendAT("ATE1", "OK", 2000); //Echo ON
  sim800.println("AT+CPIN?"); //Check SIM ready
  delay(1000);
  //SendAT("AT+CPIN?", "READY", 2000); //Check SIM ready
  sim800.println("AT+CMGF=1"); //SMS text mode
  delay(1000);
  //SendAT("AT+CMGF=1", "OK", 2000); //SMS text mode
  sim800.println("AT+CNMI=1,1,0,0,0"); /// Decides how newly arrived SMS should be handled
  delay(1000);
  //SendAT("AT+CNMI=1,1,0,0,0", "OK", 2000); //set sms received format
  //AT +CNMI = 2,1,0,0,0 - AT +CNMI = 2,2,0,0,0 (both are same)
  //--------------------------------------------------------------
  time1 = micros();
  //Serial.print("time1 = "); Serial.println(time1);
  //--------------------------------------------------------------
  //read calibrated values. otherwise false impact will trigger
  //when you reset your Arduino. (By pressing reset button)
  xaxis = analogRead(xPin);
  yaxis = analogRead(yPin);
  zaxis = analogRead(zPin);
  //--------------------------------------------------------------
}



/*****************************************************************************************
 * loop() function
 *****************************************************************************************/
void loop()
{

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Waiting For Student");
  lcd.setCursor(0, 1);
  lcd.print("Monitoring Bus");
  delay(2000);

 
  if(digitalRead(xPin) == LOW)
  {
    delay(200);
    digitalWrite(BUZZER, LOW);
    impact_detected = false;
    impact_time = 0;

      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Student 1 On Board");
      lcd.setCursor(0, 1);
      lcd.print("Sending Location");
      delay(2000);

      getGps();

      delay(1000);
      sendAlert();
      delay(10000);
     
     
     
   
  }

  else if(digitalRead(yPin) == LOW)
  {
    delay(200);
    digitalWrite(BUZZER, LOW);
    impact_detected = false;
    impact_time = 0;

      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Student 2 On Board");
      lcd.setCursor(0, 1);
      lcd.print("Sending Location");
      delay(2000);

      getGps();

     
      delay(1000);
      sendAlert1();
      delay(10000);
     
     
   
  }


  if(digitalRead(zPin) == LOW)
  {
    delay(200);
    digitalWrite(BUZZER, LOW);
    impact_detected = false;
    impact_time = 0;

      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Student 3 On Board");
      lcd.setCursor(0, 1);
      lcd.print("Sending Location");
      delay(2000);

      getGps();

     
     
      delay(1000);
      sendAlert2();
      delay(10000);
     
   
  }
 
  //--------------------------------------------------------------
  while(sim800.available())
  {
    parseData(sim800.readString());
  }
  //--------------------------------------------------------------
  //while(Serial.available())  
  //{
    //sim800.println(Serial.readString());
  //}
  //--------------------------------------------------------------

  value1 = analogRead(ALCOHOL);
  if ( value1 > Thres_Val )
   {
     digitalWrite ( LED , HIGH );    //digitalWrite(Buzzer,HIGH);
    tone(BUZZER, 1000);
    digitalWrite(RELAY, LOW);
   getGps();
   Serial.print(value1);   // send a capital A
   Serial.print("|");   // send a capital A
   Serial.print(Thres_Val);   // send a capital A
   Serial.print("|");   // send a capital A
   Serial.print(latitude);   // send a capital A
   Serial.print("|");   // send a capital A
   Serial.print(longitude);   // send a capital A
 }
 else
 {
   digitalWrite(LED, LOW);
   //digitalWrite(Buzzer,LOW);
   delay(1000);
   noTone(BUZZER);
 
   getGps();
   Serial.print(value1);   // send a capital A
   Serial.print("|");   // send a capital A
   Serial.print(Thres_Val);   // send a capital A
   Serial.print("|");   // send a capital A
   Serial.print(latitude);   // send a capital A
   Serial.print("|");   // send a capital A
   Serial.print(longitude);   // send a capital A
   
 }



}





/*****************************************************************************************
 * Impact() function
 *****************************************************************************************/
void Impact()
{
  //--------------------------------------------------------------
  time1 = micros(); // resets time value
  //--------------------------------------------------------------
  //int oldx = xaxis; //store previous axis readings for comparison
  //int oldy = yaxis;
  //int oldz = zaxis;


 

  xaxis = analogRead(xPin);
  yaxis = analogRead(yPin);
  zaxis = analogRead(zPin);
 
  //--------------------------------------------------------------
  //loop counter prevents false triggering. Vibration resets if there is an impact. Don't detect new changes until that "time" has passed.
  //vibration--;
  //Serial.print("Vibration = "); Serial.println(vibration);
  ///if(vibration < 0) vibration = 0;                                
  //Serial.println("Vibration Reset!");
 
  ///if(vibration > 0) return;
  //--------------------------------------------------------------
  ///deltx = xaxis - oldx;                                          
  ///delty = yaxis - oldy;
  //deltz = zaxis - oldz;
 
  //Magnitude to calculate force of impact.
  //magnitude = sqrt(sq(deltx) + sq(delty) + sq(deltz));
  //NNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNN
  //if (magnitude >= sensitivity) //impact detected

  if(digitalRead(VIBRATION) == LOW)
  {
    updateflag=1;
    // reset anti-vibration counter
    vibration = devibrate;
    magnitude = 50;
  }
  //NNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNN
  else
  {
    //if (magnitude > 15)
      //Serial.println(magnitude);
    //reset magnitude of impact to 0
    magnitude=0;
  }
  //NNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNN
}





/*****************************************************************************************
 * parseData() function
 *****************************************************************************************/
void parseData(String buff){
  //Serial.println(buff);

  unsigned int len, index;
  //--------------------------------------------------------------
  //Remove sent "AT Command" from the response string.
  index = buff.indexOf("\r");
  buff.remove(0, index+2);
  buff.trim();
  //NNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNN
  if(buff != "OK"){
    //--------------------------------------------------------------
    index = buff.indexOf(":");
    String cmd = buff.substring(0, index);
    cmd.trim();
   
    buff.remove(0, index+2);
    //Serial.println(buff);
    //--------------------------------------------------------------
    if(cmd == "+CMTI"){
      //get newly arrived memory location and store it in temp
      //temp = 4
      index = buff.indexOf(",");
      String temp = buff.substring(index+1, buff.length());
      temp = "AT+CMGR=" + temp + "\r";
      //AT+CMGR=4 i.e. get message stored at memory location 4
      sim800.println(temp);
    }
    //--------------------------------------------------------------
    else if(cmd == "+CMGR"){
      //extractSms(buff);
      //Serial.println(buff.indexOf(EMERGENCY_PHONE));
      if(buff.indexOf(EMERGENCY_PHONE) > 1){
        buff.toLowerCase();
        //Serial.println(buff.indexOf("get gps"));
        if(buff.indexOf("get gps") > 1){
          getGps();
          String sms_data;
          sms_data = "GPS Location Data\r";
          sms_data += "http://maps.google.com/maps?q=loc:";
          sms_data += latitude + "," + longitude;
       
          sendSms(sms_data);

           Serial.print(value1);   // send a capital A
           Serial.print("|");   // send a capital A
           Serial.print(Thres_Val);   // send a capital A
           Serial.print("|");   // send a capital A
           Serial.print(latitude);   // send a capital A
           Serial.print("|");   // send a capital A
           Serial.print(longitude);   // send a capital A
   
        }
      }
    }
    //--------------------------------------------------------------
  }
  else{
  //The result of AT Command is "OK"
  }
  //NNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNN
}


/*****************************************************************************************
 * getGps() Function
*****************************************************************************************/
void getGps()
{
  // Can take up to 60 seconds
  boolean newData = false;
  for (unsigned long start = millis(); millis() - start < 2000;){
    while (neogps.available()){
      if (gps.encode(neogps.read())){
        newData = true;
        break;
      }
    }
  }
 
  if (newData) //If newData is true
  {
    latitude = String(gps.location.lat(), 6);
    longitude = String(gps.location.lng(), 6);
    newData = false;
  }
  else {
    //Serial.println("No GPS data is available");
    latitude = "20.924708";
    longitude = "74.779572";
  }

  //Serial.print("Latitude= "); Serial.println(latitude);
  //Serial.print("Lngitude= "); Serial.println(longitude);
}




/*****************************************************************************************
* sendAlert() function
*****************************************************************************************/
void sendAlert()
{
  String sms_data;
  sms_data = "Student On Board Alert!!\r";
  sms_data += "http://maps.google.com/maps?q=loc:";
  sms_data += latitude + "," + longitude;

  sendSms(sms_data);
}

void sendAlert1()
{
  String sms_data;
  sms_data = "Student On Board Alert!!\r";
  sms_data += "http://maps.google.com/maps?q=loc:";
  sms_data += latitude + "," + longitude;

  sendSms1(sms_data);
}


void sendAlert2()
{
  String sms_data;
  sms_data = "Student On Board Alert!!\r";
  sms_data += "http://maps.google.com/maps?q=loc:";
  sms_data += latitude + "," + longitude;

  sendSms2(sms_data);
}


void sendAlert3()
{
  String sms_data;
  sms_data = "Student On Board Alert!!\r";
  sms_data += "http://maps.google.com/maps?q=loc:";
  sms_data += latitude + "," + longitude;

  sendSms3(sms_data);
}


void sendAlert4()
{
  String sms_data;
  sms_data = "Student On Board Alert!!\r";
  sms_data += "http://maps.google.com/maps?q=loc:";
  sms_data += latitude + "," + longitude;

  sendSms4(sms_data);
}




/*****************************************************************************************
* makeCall() function
*****************************************************************************************/
void makeCall()
{
  //Serial.println("calling....");
  sim800.println("ATD"+EMERGENCY_PHONE+";");
  delay(20000); //20 sec delay
  sim800.println("ATH");
  delay(1000); //1 sec delay
}


void makeCall1()
{
  //Serial.println("calling....");
  sim800.println("ATD"+EMERGENCY_PHONE1+";");
  delay(20000); //20 sec delay
  sim800.println("ATH");
  delay(1000); //1 sec delay
}

void makeCall2()
{
  //Serial.println("calling....");
  sim800.println("ATD"+EMERGENCY_PHONE2+";");
  delay(20000); //20 sec delay
  sim800.println("ATH");
  delay(1000); //1 sec delay
}

void makeCall3()
{
  //Serial.println("calling....");
  sim800.println("ATD"+EMERGENCY_PHONE3+";");
  delay(20000); //20 sec delay
  sim800.println("ATH");
  delay(1000); //1 sec delay
}

void makeCall4()
{
  //Serial.println("calling....");
  sim800.println("ATD"+EMERGENCY_PHONE4+";");
  delay(20000); //20 sec delay
  sim800.println("ATH");
  delay(1000); //1 sec delay
}




/*****************************************************************************************
 * sendSms() function
 *****************************************************************************************/
 void sendSms(String text)
{
  //return;
  sim800.print("AT+CMGF=1\r");
  delay(1000);
  sim800.print("AT+CMGS=\""+EMERGENCY_PHONE+"\"\r");
  delay(1000);
  sim800.print(text);
  delay(100);
  sim800.write(0x1A); //ascii code for ctrl-26 //sim800.println((char)26); //ascii code for ctrl-26
  delay(1000);
  //Serial.println("SMS Sent Successfully.");
}


 void sendSms1(String text)
{
  //return;
  sim800.print("AT+CMGF=1\r");
  delay(1000);
  sim800.print("AT+CMGS=\""+EMERGENCY_PHONE1+"\"\r");
  delay(1000);
  sim800.print(text);
  delay(100);
  sim800.write(0x1A); //ascii code for ctrl-26 //sim800.println((char)26); //ascii code for ctrl-26
  delay(1000);
  //Serial.println("SMS Sent Successfully.");
}


 void sendSms2(String text)
{
  //return;
  sim800.print("AT+CMGF=1\r");
  delay(1000);
  sim800.print("AT+CMGS=\""+EMERGENCY_PHONE2+"\"\r");
  delay(1000);
  sim800.print(text);
  delay(100);
  sim800.write(0x1A); //ascii code for ctrl-26 //sim800.println((char)26); //ascii code for ctrl-26
  delay(1000);
  //Serial.println("SMS Sent Successfully.");
}


 void sendSms3(String text)
{
  //return;
  sim800.print("AT+CMGF=1\r");
  delay(1000);
  sim800.print("AT+CMGS=\""+EMERGENCY_PHONE3+"\"\r");
  delay(1000);
  sim800.print(text);
  delay(100);
  sim800.write(0x1A); //ascii code for ctrl-26 //sim800.println((char)26); //ascii code for ctrl-26
  delay(1000);
  //Serial.println("SMS Sent Successfully.");
}


 void sendSms4(String text)
{
  //return;
  sim800.print("AT+CMGF=1\r");
  delay(1000);
  sim800.print("AT+CMGS=\""+EMERGENCY_PHONE4+"\"\r");
  delay(1000);
  sim800.print(text);
  delay(100);
  sim800.write(0x1A); //ascii code for ctrl-26 //sim800.println((char)26); //ascii code for ctrl-26
  delay(1000);
  //Serial.println("SMS Sent Successfully.");
}






/*****************************************************************************************
 * SendAT() function
 *****************************************************************************************/
boolean SendAT(String at_command, String expected_answer, unsigned int timeout){

    uint8_t x=0;
    boolean answer=0;
    String response;
    unsigned long previous;
   
    //Clean the input buffer
    while( sim800.available() > 0) sim800.read();

    sim800.println(at_command);
   
    x = 0;
    previous = millis();

    //this loop waits for the answer with time out
    do{
        //if there are data in the UART input buffer, reads it and checks for the asnwer
        if(sim800.available() != 0){
            response += sim800.read();
            x++;
            // check if the desired answer (OK) is in the response of the module
            if(response.indexOf(expected_answer) > 0){
                answer = 1;
                break;
            }
        }
    }while((answer == 0) && ((millis() - previous) < timeout));

  //Serial.println(response);
  return answer;
}++.h>

#include <SoftwareSerial.h>
#include <math.h>

#include<Wire.h>

#include<LiquidCrystal.h>
LiquidCrystal lcd(7, 8, 9, 10, 11, 12);



//--------------------------------------------------------------
//emergency phone number with country code
const String EMERGENCY_PHONE =  "+917620554318";
const String EMERGENCY_PHONE1 = "+919021546914";
const String EMERGENCY_PHONE2 = "+918010858994";
const String EMERGENCY_PHONE3 = "+91";
const String EMERGENCY_PHONE4 = "+91";
//--------------------------------------------------------------
//GSM Module RX pin to Arduino 3
//GSM Module TX pin to Arduino 2
#define rxPin 2
#define txPin 3
SoftwareSerial sim800(rxPin,txPin);
//--------------------------------------------------------------
//GPS Module RX pin to Arduino 4
//GPS Module TX pin to Arduino 5
#define rxPin1 4
#define txPin1 5
AltSoftSerial neogps (rxPin1,txPin1);
TinyGPSPlus gps;
//--------------------------------------------------------------
String sms_status,sender_number,received_date,msg;
String latitude, longitude;
//--------------------------------------------------------------
#define BUZZER 13
#define BUTTON 6
#define LED A2
#define RELAY A7

#define ALCOHOL A1
#define VIBRATION A0

//--------------------------------------------------------------
#define xPin A5
#define yPin A4
#define zPin A3
#define Thres_Val 260
//--------------------------------------------------------------

byte updateflag;
int value1;
int xaxis = 0, yaxis = 0, zaxis = 0;
int deltx = 0, delty = 0, deltz = 0;
int vibration = 2, devibrate = 75;
int magnitude = 0;
int sensitivity = 20;
double angle;
boolean impact_detected = false;
//Used to run impact routine every 2mS.
unsigned long time1;
unsigned long impact_time;
unsigned long alert_delay = 10000; //10 seconds
//--------------------------------------------------------------
char Incoming_value = 0;



/*****************************************************************************************
 * setup() function
 *****************************************************************************************/
void setup()
{
  //--------------------------------------------------------------
  //Serial.println("Arduino serial initialize");
  Serial.begin(9600);
  //--------------------------------------------------------------
  //Serial.println("SIM800L serial initialize");
  sim800.begin(9600);
  //--------------------------------------------------------------
  //Serial.println("NEO6M serial initialize");
  neogps.begin(9600);
  //--------------------------------------------------------------
  pinMode(BUZZER, OUTPUT);
  pinMode(RELAY, OUTPUT);
  pinMode(LED, OUTPUT);
 
  pinMode(BUTTON, INPUT_PULLUP);

  pinMode(xPin, INPUT_PULLUP);

  pinMode(yPin, INPUT_PULLUP);

  pinMode(zPin, INPUT_PULLUP);

 
  pinMode(VIBRATION, INPUT_PULLUP);
 
  //--------------------------------------------------------------
  lcd.begin(16,2);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Iot Based Smart");
  lcd.setCursor(0, 1);
  lcd.print("School Bus monitoring");
  delay(2000);


  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Work Mode ...");
  lcd.setCursor(0, 1);
  lcd.print("Monitoring Bus");
  delay(2000);

 
  //--------------------------------------------------------------
  sms_status = "";
  sender_number="";
  received_date="";
  msg="";
  //--------------------------------------------------------------
  sim800.println("AT"); //Check GSM Module
  delay(1000);
  //SendAT("AT", "OK", 2000); //Check GSM Module
  sim800.println("ATE1"); //Echo ON
  delay(1000);
  //SendAT("ATE1", "OK", 2000); //Echo ON
  sim800.println("AT+CPIN?"); //Check SIM ready
  delay(1000);
  //SendAT("AT+CPIN?", "READY", 2000); //Check SIM ready
  sim800.println("AT+CMGF=1"); //SMS text mode
  delay(1000);
  //SendAT("AT+CMGF=1", "OK", 2000); //SMS text mode
  sim800.println("AT+CNMI=1,1,0,0,0"); /// Decides how newly arrived SMS should be handled
  delay(1000);
  //SendAT("AT+CNMI=1,1,0,0,0", "OK", 2000); //set sms received format
  //AT +CNMI = 2,1,0,0,0 - AT +CNMI = 2,2,0,0,0 (both are same)
  //--------------------------------------------------------------
  time1 = micros();
  //Serial.print("time1 = "); Serial.println(time1);
  //--------------------------------------------------------------
  //read calibrated values. otherwise false impact will trigger
  //when you reset your Arduino. (By pressing reset button)
  xaxis = analogRead(xPin);
  yaxis = analogRead(yPin);
  zaxis = analogRead(zPin);
  //--------------------------------------------------------------
}



/*****************************************************************************************
 * loop() function
 *****************************************************************************************/
void loop()
{

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Waiting For Student");
  lcd.setCursor(0, 1);
  lcd.print("Monitoring Bus");
  delay(2000);

 
  if(digitalRead(xPin) == LOW)
  {
    delay(200);
    digitalWrite(BUZZER, LOW);
    impact_detected = false;
    impact_time = 0;

      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Student 1 On Board");
      lcd.setCursor(0, 1);
      lcd.print("Sending Location");
      delay(2000);

      getGps();

      delay(1000);
      sendAlert();
      delay(10000);
     
     
     
   
  }

  else if(digitalRead(yPin) == LOW)
  {
    delay(200);
    digitalWrite(BUZZER, LOW);
    impact_detected = false;
    impact_time = 0;

      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Student 2 On Board");
      lcd.setCursor(0, 1);
      lcd.print("Sending Location");
      delay(2000);

      getGps();

     
      delay(1000);
      sendAlert1();
      delay(10000);
     
     
   
  }


  if(digitalRead(zPin) == LOW)
  {
    delay(200);
    digitalWrite(BUZZER, LOW);
    impact_detected = false;
    impact_time = 0;

      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Student 3 On Board");
      lcd.setCursor(0, 1);
      lcd.print("Sending Location");
      delay(2000);

      getGps();

     
     
      delay(1000);
      sendAlert2();
      delay(10000);
     
   
  }
 
  //--------------------------------------------------------------
  while(sim800.available())
  {
    parseData(sim800.readString());
  }
  //--------------------------------------------------------------
  //while(Serial.available())  
  //{
    //sim800.println(Serial.readString());
  //}
  //--------------------------------------------------------------

  value1 = analogRead(ALCOHOL);
  if ( value1 > Thres_Val )
   {
     digitalWrite ( LED , HIGH );    //digitalWrite(Buzzer,HIGH);
    tone(BUZZER, 1000);
    digitalWrite(RELAY, LOW);
   getGps();
   Serial.print(value1);   // send a capital A
   Serial.print("|");   // send a capital A
   Serial.print(Thres_Val);   // send a capital A
   Serial.print("|");   // send a capital A
   Serial.print(latitude);   // send a capital A
   Serial.print("|");   // send a capital A
   Serial.print(longitude);   // send a capital A
 }
 else
 {
   digitalWrite(LED, LOW);
   //digitalWrite(Buzzer,LOW);
   delay(1000);
   noTone(BUZZER);
 
   getGps();
   Serial.print(value1);   // send a capital A
   Serial.print("|");   // send a capital A
   Serial.print(Thres_Val);   // send a capital A
   Serial.print("|");   // send a capital A
   Serial.print(latitude);   // send a capital A
   Serial.print("|");   // send a capital A
   Serial.print(longitude);   // send a capital A
   
 }



}





/*****************************************************************************************
 * Impact() function
 *****************************************************************************************/
void Impact()
{
  //--------------------------------------------------------------
  time1 = micros(); // resets time value
  //--------------------------------------------------------------
  //int oldx = xaxis; //store previous axis readings for comparison
  //int oldy = yaxis;
  //int oldz = zaxis;


 

  xaxis = analogRead(xPin);
  yaxis = analogRead(yPin);
  zaxis = analogRead(zPin);
 
  //--------------------------------------------------------------
  //loop counter prevents false triggering. Vibration resets if there is an impact. Don't detect new changes until that "time" has passed.
  //vibration--;
  //Serial.print("Vibration = "); Serial.println(vibration);
  ///if(vibration < 0) vibration = 0;                                
  //Serial.println("Vibration Reset!");
 
  ///if(vibration > 0) return;
  //--------------------------------------------------------------
  ///deltx = xaxis - oldx;                                          
  ///delty = yaxis - oldy;
  //deltz = zaxis - oldz;
 
  //Magnitude to calculate force of impact.
  //magnitude = sqrt(sq(deltx) + sq(delty) + sq(deltz));
  //NNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNN
  //if (magnitude >= sensitivity) //impact detected

  if(digitalRead(VIBRATION) == LOW)
  {
    updateflag=1;
    // reset anti-vibration counter
    vibration = devibrate;
    magnitude = 50;
  }
  //NNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNN
  else
  {
    //if (magnitude > 15)
      //Serial.println(magnitude);
    //reset magnitude of impact to 0
    magnitude=0;
  }
  //NNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNN
}





/*****************************************************************************************
 * parseData() function
 *****************************************************************************************/
void parseData(String buff){
  //Serial.println(buff);

  unsigned int len, index;
  //--------------------------------------------------------------
  //Remove sent "AT Command" from the response string.
  index = buff.indexOf("\r");
  buff.remove(0, index+2);
  buff.trim();
  //NNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNN
  if(buff != "OK"){
    //--------------------------------------------------------------
    index = buff.indexOf(":");
    String cmd = buff.substring(0, index);
    cmd.trim();
   
    buff.remove(0, index+2);
    //Serial.println(buff);
    //--------------------------------------------------------------
    if(cmd == "+CMTI"){
      //get newly arrived memory location and store it in temp
      //temp = 4
      index = buff.indexOf(",");
      String temp = buff.substring(index+1, buff.length());
      temp = "AT+CMGR=" + temp + "\r";
      //AT+CMGR=4 i.e. get message stored at memory location 4
      sim800.println(temp);
    }
    //--------------------------------------------------------------
    else if(cmd == "+CMGR"){
      //extractSms(buff);
      //Serial.println(buff.indexOf(EMERGENCY_PHONE));
      if(buff.indexOf(EMERGENCY_PHONE) > 1){
        buff.toLowerCase();
        //Serial.println(buff.indexOf("get gps"));
        if(buff.indexOf("get gps") > 1){
          getGps();
          String sms_data;
          sms_data = "GPS Location Data\r";
          sms_data += "http://maps.google.com/maps?q=loc:";
          sms_data += latitude + "," + longitude;
       
          sendSms(sms_data);

           Serial.print(value1);   // send a capital A
           Serial.print("|");   // send a capital A
           Serial.print(Thres_Val);   // send a capital A
           Serial.print("|");   // send a capital A
           Serial.print(latitude);   // send a capital A
           Serial.print("|");   // send a capital A
           Serial.print(longitude);   // send a capital A
   
        }
      }
    }
    //--------------------------------------------------------------
  }
  else{
  //The result of AT Command is "OK"
  }
  //NNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNN
}


/*****************************************************************************************
 * getGps() Function
*****************************************************************************************/
void getGps()
{
  // Can take up to 60 seconds
  boolean newData = false;
  for (unsigned long start = millis(); millis() - start < 2000;){
    while (neogps.available()){
      if (gps.encode(neogps.read())){
        newData = true;
        break;
      }
    }
  }
 
  if (newData) //If newData is true
  {
    latitude = String(gps.location.lat(), 6);
    longitude = String(gps.location.lng(), 6);
    newData = false;
  }
  else {
    //Serial.println("No GPS data is available");
    latitude = "20.924708";
    longitude = "74.779572";
  }

  //Serial.print("Latitude= "); Serial.println(latitude);
  //Serial.print("Lngitude= "); Serial.println(longitude);
}




/*****************************************************************************************
* sendAlert() function
*****************************************************************************************/
void sendAlert()
{
  String sms_data;
  sms_data = "Student On Board Alert!!\r";
  sms_data += "http://maps.google.com/maps?q=loc:";
  sms_data += latitude + "," + longitude;

  sendSms(sms_data);
}

void sendAlert1()
{
  String sms_data;
  sms_data = "Student On Board Alert!!\r";
  sms_data += "http://maps.google.com/maps?q=loc:";
  sms_data += latitude + "," + longitude;

  sendSms1(sms_data);
}


void sendAlert2()
{
  String sms_data;
  sms_data = "Student On Board Alert!!\r";
  sms_data += "http://maps.google.com/maps?q=loc:";
  sms_data += latitude + "," + longitude;

  sendSms2(sms_data);
}


void sendAlert3()
{
  String sms_data;
  sms_data = "Student On Board Alert!!\r";
  sms_data += "http://maps.google.com/maps?q=loc:";
  sms_data += latitude + "," + longitude;

  sendSms3(sms_data);
}


void sendAlert4()
{
  String sms_data;
  sms_data = "Student On Board Alert!!\r";
  sms_data += "http://maps.google.com/maps?q=loc:";
  sms_data += latitude + "," + longitude;

  sendSms4(sms_data);
}




/*****************************************************************************************
* makeCall() function
*****************************************************************************************/
void makeCall()
{
  //Serial.println("calling....");
  sim800.println("ATD"+EMERGENCY_PHONE+";");
  delay(20000); //20 sec delay
  sim800.println("ATH");
  delay(1000); //1 sec delay
}


void makeCall1()
{
  //Serial.println("calling....");
  sim800.println("ATD"+EMERGENCY_PHONE1+";");
  delay(20000); //20 sec delay
  sim800.println("ATH");
  delay(1000); //1 sec delay
}

void makeCall2()
{
  //Serial.println("calling....");
  sim800.println("ATD"+EMERGENCY_PHONE2+";");
  delay(20000); //20 sec delay
  sim800.println("ATH");
  delay(1000); //1 sec delay
}

void makeCall3()
{
  //Serial.println("calling....");
  sim800.println("ATD"+EMERGENCY_PHONE3+";");
  delay(20000); //20 sec delay
  sim800.println("ATH");
  delay(1000); //1 sec delay
}

void makeCall4()
{
  //Serial.println("calling....");
  sim800.println("ATD"+EMERGENCY_PHONE4+";");
  delay(20000); //20 sec delay
  sim800.println("ATH");
  delay(1000); //1 sec delay
}




/*****************************************************************************************
 * sendSms() function
 *****************************************************************************************/
 void sendSms(String text)
{
  //return;
  sim800.print("AT+CMGF=1\r");
  delay(1000);
  sim800.print("AT+CMGS=\""+EMERGENCY_PHONE+"\"\r");
  delay(1000);
  sim800.print(text);
  delay(100);
  sim800.write(0x1A); //ascii code for ctrl-26 //sim800.println((char)26); //ascii code for ctrl-26
  delay(1000);
  //Serial.println("SMS Sent Successfully.");
}


 void sendSms1(String text)
{
  //return;
  sim800.print("AT+CMGF=1\r");
  delay(1000);
  sim800.print("AT+CMGS=\""+EMERGENCY_PHONE1+"\"\r");
  delay(1000);
  sim800.print(text);
  delay(100);
  sim800.write(0x1A); //ascii code for ctrl-26 //sim800.println((char)26); //ascii code for ctrl-26
  delay(1000);
  //Serial.println("SMS Sent Successfully.");
}


 void sendSms2(String text)
{
  //return;
  sim800.print("AT+CMGF=1\r");
  delay(1000);
  sim800.print("AT+CMGS=\""+EMERGENCY_PHONE2+"\"\r");
  delay(1000);
  sim800.print(text);
  delay(100);
  sim800.write(0x1A); //ascii code for ctrl-26 //sim800.println((char)26); //ascii code for ctrl-26
  delay(1000);
  //Serial.println("SMS Sent Successfully.");
}


 void sendSms3(String text)
{
  //return;
  sim800.print("AT+CMGF=1\r");
  delay(1000);
  sim800.print("AT+CMGS=\""+EMERGENCY_PHONE3+"\"\r");
  delay(1000);
  sim800.print(text);
  delay(100);
  sim800.write(0x1A); //ascii code for ctrl-26 //sim800.println((char)26); //ascii code for ctrl-26
  delay(1000);
  //Serial.println("SMS Sent Successfully.");
}


 void sendSms4(String text)
{
  //return;
  sim800.print("AT+CMGF=1\r");
  delay(1000);
  sim800.print("AT+CMGS=\""+EMERGENCY_PHONE4+"\"\r");
  delay(1000);
  sim800.print(text);
  delay(100);
  sim800.write(0x1A); //ascii code for ctrl-26 //sim800.println((char)26); //ascii code for ctrl-26
  delay(1000);
  //Serial.println("SMS Sent Successfully.");
}






/*****************************************************************************************
 * SendAT() function
 *****************************************************************************************/
boolean SendAT(String at_command, String expected_answer, unsigned int timeout){

    uint8_t x=0;
    boolean answer=0;
    String response;
    unsigned long previous;
   
    //Clean the input buffer
    while( sim800.available() > 0) sim800.read();

    sim800.println(at_command);
   
    x = 0;
    previous = millis();

    //this loop waits for the answer with time out
    do{
        //if there are data in the UART input buffer, reads it and checks for the asnwer
        if(sim800.available() != 0){
            response += sim800.read();
            x++;
            // check if the desired answer (OK) is in the response of the module
            if(response.indexOf(expected_answer) > 0){
                answer = 1;
                break;
            }
        }
    }while((answer == 0) && ((millis() - previous) < timeout));

  //Serial.println(response);
  return answer;
}