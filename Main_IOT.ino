#include <U8glib.h>
#include <SoftwareSerial.h>
#include <RTClib.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <SimpleDHT.h>

#include "DHT.h"
#define DHTPIN 8
#define DHTTYPE DHT22 
#define DHT11PIN 7
#define DHT11TYPE DHT11

LiquidCrystal_I2C lcd(0x27,16,2);

// for DHT11,
//      VCC: 5V or 3V
//      GND: GND
//      DATA: 2
//int pinDHT22 = 8;
//int pushButton = 2;
int pin_motor_coco = 2;
int pin_motor_coco_2 = 5;
int pin_extractor_in = 3;
int pin_extractor_out = 4;
DHT dht(DHTPIN, DHTTYPE);
DHT dht_2(DHT11PIN, DHT11TYPE);
RTC_DS3231 rtc;      // crea objeto del tipo RTC_DS3231
int sustratoASensorPin = A0;
int sustratoASensorValue = 0;
int sustratoAPercentValue = 0;

#define SSID2 "BIOINV_IOT" // "WiFiname" BIOINV_IOT
#define PASS "bioinvdl2022" // "Password" bioinvdl2022
#define AUTH "sanaIi1SHwpWbWnVj8U6_aNrDJycYY_I"
#define IP "184.106.153.149" // thingspeak.com ip (do not change)
String msg = "GET /update?api_key=TPHQ9AYRMZAZAYE7"; //change it with your channel Write key
const int Time_S = 120; //Change this to your prefered sampling interval in seconds, exampe 2 min is 120 seconds
float Temp_C;
float Temp_F;
float Humidity;
float Temp_C_2;
float Humidity_2;

///////////////////////////////////////////////////////////////////////////////////////////////
void relay_setup() {
  pinMode(pin_motor_coco, OUTPUT);
  digitalWrite(pin_motor_coco, HIGH);
  pinMode(pin_motor_coco_2, OUTPUT);
  digitalWrite(pin_motor_coco_2, HIGH);
  pinMode(pin_extractor_in, OUTPUT);
  digitalWrite(pin_extractor_in, HIGH);
  pinMode(pin_extractor_out, OUTPUT);
  digitalWrite(pin_extractor_out, HIGH);
}

////////////////////////////////////////////////////////////////////////////////////////////////
void dh_setup() {
  Serial.println("Sample DHT22...");
  Serial.println("Temperature and Humidity Data");
  dht.begin();
  dht_2.begin();
}

////////////////////////////////////////////////////////////////////////////////////////////////
void lc_setup() {
  lcd.init();
  lcd.backlight();
  lcd.clear();
}
/////////////////////////////////////////////////////////////////////////////////////////////////
void setup() {
  Serial.begin(115200);
  // start working...
  Serial.println("AT");
  delay(5000);
  if (Serial.find("OK")) {
    connectWiFi();
  }
  relay_setup();
  dh_setup();
  // make the pushbutton's pin an input:
  //pinMode(pushButton, INPUT_PULLUP);

  if (! rtc.begin()) {        // si falla la inicializacion del modulo
    Serial.println("Modulo RTC no encontrado !");  // muestra mensaje de error
    while (1);         // bucle infinito que detiene ejecucion del programa
  }

  if(rtc.lostPower()){
    rtc.adjust(DateTime(__DATE__,__TIME__));
  }
  
  
  lc_setup();

  //pinMode(sensor_arroz_pin, INPUT);
}
///////////////////////////////////////////////////////////////////////////////////////////////
void loop() {
  DateTime fecha = rtc.now();

  byte temperature = 0;
  byte humidity = 0;

  int err = SimpleDHTErrSuccess;
  //if ((err = dht22.read(&temperature, &humidity, NULL)) != SimpleDHTErrSuccess) {
    //Serial.print("Read DHT11 failed, err="); Serial.print(SimpleDHTErrCode(err));
    //Serial.print(","); Serial.println(SimpleDHTErrDuration(err)); delay(1000);
    //return;
  //}
  
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  float h_2 = dht_2.readHumidity();
  float t_2 = dht_2.readTemperature();
  
  Serial.print(t); Serial.print(" *C, ");
  Serial.print(h); Serial.println(" H");
  
  sustratoASensorValue = analogRead(sustratoASensorPin);
  sustratoAPercentValue = map(sustratoASensorValue,1010,840,0,100);
  Serial.print("\nsustratoPercentValue: ");
  Serial.print(sustratoAPercentValue);
  //Serial.print(sustratoASensorValue);
  Serial.print("%");
  Serial.print("\n");
  
  Temp_C = t;
  Humidity = h;

  Temp_C_2 = t_2;
  Humidity_2 = h_2;

  lcd.setCursor(0, 0);
  lcd.print("T: ");
  lcd.print(t);
  lcd.print("C");
  lcd.setCursor(10, 0);
  lcd.print(fecha.hour());
  lcd.print(":");
  lcd.print(fecha.minute());
  lcd.setCursor(0, 1);
  lcd.print("H: ");
  lcd.print(h);
  lcd.print("%");
  lcd.setCursor(10, 1);
  lcd.print("TI:");
  lcd.print(t_2);
  lcd.print("C");
  
  /////IF STATEMENTS /////////
  if (fecha.minute() == 01 && fecha.second() == 15 && fecha.hour() >= 6 && fecha.hour() <= 18) {
    Send();
    delay(1000);
    digitalWrite(pin_motor_coco, LOW);
    digitalWrite(pin_motor_coco_2, LOW);
  }

  if (fecha.minute() == 05 && fecha.second() == 15 && fecha.hour() >= 6 && fecha.hour() <= 18) {
    digitalWrite(pin_motor_coco, HIGH);
    digitalWrite(pin_motor_coco_2, HIGH);
  }

  if (fecha.hour() == 6 && fecha.minute() == 10 && fecha.second() == 15) {
    lcd.backlight();
    delay(1000);
  }
  
  if (fecha.minute() == 05 && fecha.second() == 30 && fecha.hour() > 18) {
    Send();
    delay(1000);
  }

  if (fecha.hour() >= 6 && fecha.hour() <= 18) {
    if (Temp_C > 27) {
      digitalWrite(pin_extractor_out, LOW);
      digitalWrite(pin_extractor_in, LOW);
    }
    if (Temp_C < 27 && Temp_C > 12) {
      digitalWrite(pin_extractor_out, HIGH);
      digitalWrite(pin_extractor_in, HIGH);
    }
  }
  
  //if(sustratoAPercentValue < 40){
    //digitalWrite(pin_motor_arroz, LOW);
    //delay(6000);
    //digitalWrite(pin_motor_arroz, HIGH);
  //}
  delay(1000);
}

void Send() {
  String cmd = "AT+CIPSTART=\"TCP\",\"";
  cmd += IP;
  cmd += "\",80";
  Serial.println(cmd);
  delay(2500);
  cmd = msg ;
  cmd += "&field1="; //field 1 for test value
  cmd += Temp_C; //change to temp_F for Fahrenheit
  cmd += "&field2="; //field
  cmd += Temp_C_2;
  cmd += "&field3="; //field
  cmd += Temp_C_2 - Temp_C;
  cmd += "&field4="; //field
  cmd += Humidity;
  cmd += "&field5="; //field
  cmd += Humidity_2;
  cmd += "&field6="; //field
  cmd += Humidity_2 - Humidity;
  //cmd += "&field7="; //field
  //cmd += 100;
  //cmd += "&field8="; //field
  //cmd += 100;
  cmd += "\r\n";
  cmd += "\r\n";
  Serial.print("AT+CIPSEND=");
  Serial.println(cmd.length());
  if (Serial.find(">")) {
    Serial.print(cmd);
  } else {
    Serial.println("AT+CIPCLOSE");
  }
}

boolean connectWiFi() {
  Serial.println("AT+CWMODE=1");
  delay(2000);
  String cmd = "AT+CWJAP=\"";
  cmd += SSID2;
  cmd += "\",\"";
  cmd += PASS;
  cmd += "\"";
  Serial.println(cmd);
  delay(5000);
  if (Serial.find("OK")) {
    return true;
  } else {
    return false;
  }
}
