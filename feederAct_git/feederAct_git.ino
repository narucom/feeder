////git ver 0.10 ///
#include <SoftwareSerial.h>
#include <Servo.h>
#include <EEPROM.h>
#include <DS3231.h>

SoftwareSerial BTSerial(4, 5); // SoftwareSerial(TX, RX)
DS3231  rtc(SDA, SCL);
Time t;
Servo servo;
int i =1; // address
unsigned long pri_Time = 0;
String priFix = "";
String moreMsg = "";
String inputString = "";         
boolean stringComplete = false;  
boolean runring = false;
int readHour;
int readMin;
int dateTimes[6];   
int saved_hour_addr = 1;  
int saved_min_addr = 2;
int addrPossible = 1;
int addrMax = 10; 

void setup() {
  servo.attach(9);
  BTSerial.begin(9600);
  Serial.begin(9600);
  rtc.begin();
  servo.write(0);
  delay(500);
  inputString.reserve(10);   
  eepromRead();
} // setup end

void setTime(){
  rtc.setDOW(MONDAY);
  rtc.setTime(17, 45, 0); 
  rtc.setDate(10, 4, 2017);
}

void loop() {
  btSerialEvent();
  if (stringComplete) {
     subString(inputString);
     Serial.println("prifix:" + priFix);
    if(priFix == "z"){    
      int angleInt = intFromString(moreMsg);
      Serial.println("angle:" + moreMsg);
      angleAct(angleInt);
    }
    if(priFix == "p"){
      angleAct(0);
      delay(1000); 
      int angleInt = intFromString(moreMsg);
      Serial.println("angle:" + moreMsg);
      angleAct(angleInt);
      delay(500);
    }
    if(priFix == "o"){
      servo.attach(9);
      int angleInt = intFromString(moreMsg);
      Serial.println("angle:" + moreMsg);
      angleAct(angleInt);
    }
    if(priFix == "m"){
      int angleInt = intFromString(moreMsg);
      Serial.println("angle:" + moreMsg);
      angleAct(angleInt);
    }
    if(priFix == "a"){
      int angleInt = intFromString(moreMsg);
      Serial.println("angle:" + moreMsg);
      angleAct(angleInt);
    }
    if(priFix == "t"){
      int angleInt = intFromString(moreMsg);
      Serial.println("angle:" + moreMsg);
      angleAct(angleInt);
     
    }
    if(priFix == "s"){
      int angleInt = intFromString(moreMsg);
      eepromAngleSave(angleInt);
    }
     if(priFix == "v"){
      subTimeString(moreMsg);
      eepromAddTimeSave(dateTimes);
    }
     if(priFix == "d"){
      int add = moreMsg.toInt();
      Serial.println("deleate clected ");
      eepromDel(add); 
    }
     if(priFix == "c"){
      Serial.println("read time send:");
      delay(100);
      btSerialWrite();
    } 
    inputString = "";
    stringComplete = false;
  } // if end     
  readHour = EEPROM.read(saved_hour_addr);
  readMin = EEPROM.read(saved_min_addr);
   timeLoading();
   delay(1);
} // loop end

void servoDetach()
{
  servo.detach();
}

void testfor(){
  for(int i =0; i < 10; i++){
    Serial.println("index test :" + String(i));
    delay(1000);
  }
}

void subTimeString(String msg){
  String addAddr =  msg.substring(0, 1);  // address
  Serial.println("read send addAddr :" + addAddr);
  dateTimes[5] = intFromString(addAddr);
  String mHour =  msg.substring(1, 3);  // hour
  dateTimes[3] = intFromString(mHour);  // hour
  String mMin = msg.substring(3);  //min 
  dateTimes[4] =  intFromString(mMin);  //min
}

void timeLoading(){
  unsigned long current_Time = millis();
  int angle_fact = EEPROM.read(0); // 동작 하게될 저장되었던 각도
    t = rtc.getTime();
    int hourInt = t.hour;
    int minInt = t.min;
  if(current_Time - pri_Time > 60000) {
    for(int i=1; i <= 5; i++){
      int mHour = EEPROM.read(i*2-1); //시간
      int mMin = EEPROM.read(i*2);    // 분
      if(hourInt == 0 && minInt == 0){
        Serial.println("no data out setting");  
        pri_Time = current_Time;    
        break;
      }
      if(hourInt == mHour && minInt == mMin){
          Serial.println("time same.");    
          for(int m = 0; i < 8 ; i++){
          angleAct(angle_fact);
          delay(2000);
          angleAct(0);
          delay(50);
          angleAct(angle_fact/2);
          delay(100);
          angleAct(0);
          delay(50);
          }
          pri_Time = current_Time;  
          break;  
      }
   } // for 
  } //if
} //timeLoading end

void currentTimeRoading(){
  t = rtc.getTime();
  int yearInt = t.year;
  int monthInt = t.mon;
  int dateInt = t.date;
  int hourInt = t.hour;
  int minInt = t.min;
  Serial.println("  // Arduino current Time//");
   delay (1000*60);
}

void currentTimeView(){
  t = rtc.getTime();
  int yearInt = t.year;
  int monthInt = t.mon;
  int dateInt = t.date;
  int hourInt = t.hour;
  int minInt = t.min;
   Serial.print(yearInt);
  Serial.print("-");
  Serial.print(monthInt);
  Serial.print("-");
  Serial.print(dateInt);
  Serial.print('\n');
  Serial.print(hourInt);
  Serial.print("-");
  Serial.print(minInt);
  Serial.println("  //  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  - //");
   delay (1000*30);
}

void eepromAngleSave(int writeData){
  int addr =0;
  EEPROM.write(addr, writeData);
  delay(100);
  Serial.println("Writing end ...");
  Serial.println();         // 공백 줄
  int readData = EEPROM.read(addr);
  Serial.println("EEPROM Address : " + String(0) + "\t Value : " + readData);
  delay(100);
}

void eepromAddTimeSave(int *mTimeArr){
  Serial.println("Current mTimeArr :" + String(mTimeArr[5]));
  int hourAddr = mTimeArr[5]*2-1;
  int minAddr = mTimeArr[5]*2;
  Serial.println("EEPROM Address : " + String(hourAddr) + ", "  + "\t Value : " + mTimeArr[3]);
  EEPROM.write(hourAddr, mTimeArr[3]);
  EEPROM.write(minAddr, mTimeArr[4]);
}
void eepromTimeSave(int *mTimeArr){
  Serial.println("Current Address ..." + String(addrPossible));
  Serial.println();
  int addr_hour = addrPossible;
  int addr_min =addrPossible + 1;
  EEPROM.write(addr_hour, mTimeArr[3]);
  EEPROM.write(addr_min, mTimeArr[4]);
  if(addrPossible == addrMax -1){
    addrPossible = 1;
  }else{
  addrPossible = addrPossible + 2;    // 새 주소로 갱신
  }
  delay(100);
  Serial.println("Time Saveing end ...");
  Serial.println();         // 공백 줄
  int readHour = EEPROM.read(addr_hour);
  int readMin = EEPROM.read(addr_min);
  Serial.println("EEPROM Address : " + String(addr_hour) + "\t Value : " + readHour);
  Serial.println();         // 공백 줄
  Serial.println("EEPROM Address : " + String(addr_min) + "\t Value : " + readMin);
  delay(100);
}

void eepromClear(){
  for (int i = 0 ; i < EEPROM.length() ; i++) {
//    EEPROM.write(i, 255);
      EEPROM.write(i, 0);
  }
}

void eepromDel(int i){
      int address = i*2;
      EEPROM.write(address-1, 0);
//      EEPROM.clear(address-1);
      delay(50);
      EEPROM.write(address, 0);
//      EEPROM.clear(address-1);
      delay(50);
      eepromRead();
}

void eepromRead(){
  for(int i = 0; i <= 10 ; i++){
    int value = EEPROM.read(i);
    Serial.println("address:" + String(i) + "\t value = " + value);
    Serial.println("");
  }
}

void eepromPossible_addr(){
  int result = 1%2; 
  for(int i = 1; i <= 10 ; i++){
    int value = EEPROM.read(i);
    Serial.println("address:" + String(i) + "\t value = " + value);
    Serial.println("");
    int remainder = i%2; // 나머지 값
    if(value == 0 && remainder == 1){
      addrPossible = i;
      Serial.println("Possible address:" + String(addrPossible));
      Serial.println("");
      break;
    }
  }
}

int intFromString(String str){
    char angChar[4];
    str.toCharArray(angChar, 4);
    int angleInt = atoi(angChar);
    return angleInt;
}

void btSerialEvent() {
  while (BTSerial.available()) {
    char inChar = (char)BTSerial.read();
    inputString += inChar;
    if (inChar == '\n') {
      stringComplete = true;
    }
  }
}

void btSerialWrite_date() {
  t = rtc.getTime();
  int yearInt = t.year;
  int monthInt = t.mon;
  int dateInt = t.date;
  int hourInt = t.hour;
  int minInt = t.min;
  Serial.println("  // Arduino current Time//");
    int arrLan = 10; 
    byte byteV[arrLan];
    byteV[0] = 0;
    for(int j=1; j <= arrLan; j++){ 
      BTSerial.write(yearInt);
      BTSerial.print('/');
      BTSerial.print('e');
    }
delay(100);
}

void btSerialWrite() {
  t = rtc.getTime();
    int arrLan = 10; 
    byte byteV[arrLan];
    byteV[0] = 0;
    for(int i=1; i <= 10; i++){
    int value = EEPROM.read(i);
      byteV[i] = 0;
      byteV[i] = value;
      delay(50);
    }
    BTSerial.print(t.year);
    BTSerial.print('/');
    BTSerial.print(t.mon);
    BTSerial.print('/');
    BTSerial.print(t.date);
    BTSerial.print('/');
    BTSerial.print(t.hour);
    BTSerial.print('/');
    BTSerial.print(t.min);
    BTSerial.print('/'); 
    for(int j=1; j <= arrLan; j++){     
      BTSerial.print(EEPROM.read(j));
      if(j == arrLan){
        BTSerial.print('e');
        break;
      }
       BTSerial.print('/');
    }
}

void subString(String msg){
  priFix = msg.substring(0, 1);
  moreMsg = msg.substring(1);
}

void angleAct(int angle){
  servo.write(angle);
  delay(15);
}
