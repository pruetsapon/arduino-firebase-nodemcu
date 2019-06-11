
#include <Firebase.h>
#include <FirebaseArduino.h>
#include <FirebaseCloudMessaging.h>
#include <FirebaseError.h>
#include <FirebaseHttpClient.h>
#include <FirebaseObject.h>

#include <Time.h>
#include <Stdio.h>
#include <ESP8266WiFi.h>

#include <SoftwareSerial.h>

#include <DallasTemperature.h>
#include <OneWire.h>

/* config wifi */
const char* ssid     = "xxxxxxx";
const char* password = "xxxxxxx";
/* config firebase */
const String firebaseName = "cross-accounting.firebaseio.com";
const String firebaseKey = "64k2IWB8PR2hCjp7PAjWSi5YrSQ9ien5i9aKfFBg";
/* config time */
const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = 0;
/* thailand * 7 */
const int   daylightOffset_sec = 7 * 3600;
/* config arduino*/
int tMinute = 5;
int cAmount;
bool runLog = 0;

/*
  %a Abbreviated weekday name 
  %A Full weekday name 
  %b Abbreviated month name 
  %B Full month name 
  %c Date and time representation for your locale 
  %d Day of month as a decimal number (01-31) 
  %H Hour in 24-hour format (00-23) 
  %I Hour in 12-hour format (01-12) 
  %j Day of year as decimal number (001-366) 
  %m Month as decimal number (01-12) 
  %M Minute as decimal number (00-59) 
  %p Current locale's A.M./P.M. indicator for 12-hour clock 
  %S Second as decimal number (00-59) 
  %U Week of year as decimal number,  Sunday as first day of week (00-51) 
  %w Weekday as decimal number (0-6; Sunday is 0) 
  %W Week of year as decimal number, Monday as first day of week (00-51) 
  %x Date representation for current locale 
  %X Time representation for current locale 
  %y Year without century, as decimal number (00-99) 
  %Y Year with century, as decimal number 
  %z %Z Time-zone name or abbreviation, (no characters if time zone is unknown) 
  %% Percent sign 
  You can include text literals (such as spaces and colons) to make a neater display or for padding between adjoining columns. 
  You can suppress the display of leading zeroes  by using the "#" character  (%#d, %#H, %#I, %#j, %#m, %#M, %#S, %#U, %#w, %#W, %#y, %#Y) 
*/
#define ONE_WIRE_BUS D2                          //D2 pin of nodemcu

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

SoftwareSerial chat(D5, D6); // RX | TX

char* getLocalTime(char* format)
{
  char buffer[80];
  time_t rawtime;
  struct tm * timeinfo;
  time (&rawtime);
  timeinfo = localtime (&rawtime);
  strftime (buffer,80,format,timeinfo);
  return buffer;
}

void setup() 
{
  pinMode(D5, INPUT);
  pinMode(D6, OUTPUT);
  
  Serial.begin(9600);
  sensors.begin();
  /* connect wifi */
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) 
  {
    delay(250);
  }
  Serial.println("WiFi CONNECTED!!");
  /* set current date */
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  /* set up firebase */
  Firebase.begin(firebaseName, firebaseKey);

  chat.begin(4800);

  delay(10000);
}

void loop()
{
  setSetting();
  setSystemLog();
  checkSetting();
}

void checkSetting()
{
  for(int i = 0; i < (tMinute * 6); i++)
  {
    setSetting();
    delay(9700);
  }
}

void setSetting()
{
  FirebaseObject setting = getSetting();
  int amount = setting.getInt("amount");
  Serial.print("Amount : ");
  Serial.println(amount);
  runLog = setting.getBool("running");
  Serial.print("Status : ");
  Serial.println(runLog);
  tMinute = setting.getInt("time");
  Serial.print("Minute : ");
  Serial.println(tMinute);
  if(cAmount != amount)
  {
    cAmount = amount;
    Serial.println("update amount");
    Serial.println(amount);
    chat.write('#');
    chat.print(amount);
    chat.write(';');

  }
}

void setSystemLog()
{
  Serial.print("runLog : ");
  Serial.println(runLog);
  if(runLog)
  {
    sensors.requestTemperatures();                // Send the command to get temperatures  
    int temp = sensors.getTempCByIndex(0);
    Serial.print("Temp real : ");
    Serial.println(temp);
    StaticJsonBuffer<200> jsonBuffer;
    JsonObject& data = jsonBuffer.createObject();
    data["temperature"] = temp;
    data["amount"] = cAmount;
    data["date"] = getLocalTime("%Y-%m-%d %H:%M:%S");
    String ndate = String(getLocalTime("%Y%m%d"));
    data["ndate"] = ndate.toInt();
    //data["ndate"] = getLocalTime("%Y%m%d");
  
    String tPath = "systemlogs/" + String(getLocalTime("%Y%m%d")) + String(getLocalTime("%H%M%S"));
    Firebase.set(tPath , data);
    bool isFailed = Firebase.failed();
    if(isFailed) {
  
      Serial.println("error : set system logs");
      
    }
  }
}

FirebaseObject getSetting()
{
  FirebaseObject setting = Firebase.get("setting");
  bool isFailed = Firebase.failed();
  if(isFailed) {
    Serial.println("error : get setting");
  }
  return setting;
}
