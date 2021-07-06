#include <U8glib.h>
#include <SoftwareSerial.h>
#include <RTClib.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <SimpleDHT.h>

#include "DHT.h"
#define DHTPIN 8
#define DHTTYPE DHT22 

LiquidCrystal_I2C lcd(0x27,16,2);

// for DHT11,
//      VCC: 5V or 3V
//      GND: GND
//      DATA: 2
//int pinDHT22 = 8;
//int pushButton = 2;
int pin_motor_coco = 2;
int pin_motor_arroz = 3;
int pin_motor_lana = 4;
int pin_motor_perlita = 5;
int pin_extractor_in = 6;
int pin_extractor_out = 7;
DHT dht(DHTPIN, DHTTYPE);
RTC_DS3231 rtc;      // crea objeto del tipo RTC_DS3231
int sustratoASensorPin = A0;
int sustratoASensorValue = 0;
int sustratoAPercentValue = 0;
int sustratoPSensorPin = A1;
int sustratoPSensorValue = 0;
int sustratoPPercentValue = 0;

//int sensor_arroz_pin = ;

#define SSID2 "CNT_FLIAGAIBOR" // "WiFiname" Bioinv_IOT
#define PASS "orion1974" // "Password" biosfera2josue
#define AUTH "sanaIi1SHwpWbWnVj8U6_aNrDJycYY_I"
#define IP "184.106.153.149" // thingspeak.com ip (do not change)
String msg = "GET /update?api_key=K7KM5YEVFG35JWUD"; //change it with your channel Write key
const int Time_S = 120; //Change this to your prefered sampling interval in seconds, exampe 2 min is 120 seconds
float Temp_C;
float Temp_F;
float Humidity;

///////////////////////////////////////////////////////////////////////////////////////////////
void relay_setup() {
  pinMode(pin_motor_coco, OUTPUT);
  digitalWrite(pin_motor_coco, HIGH);
  pinMode(pin_motor_arroz, OUTPUT);
  digitalWrite(pin_motor_arroz, HIGH);
  pinMode(pin_motor_lana, OUTPUT);
  digitalWrite(pin_motor_lana, HIGH);
  pinMode(pin_motor_perlita, OUTPUT);
  digitalWrite(pin_motor_perlita, HIGH);
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
  //rtc.adjust(DateTime(__DATE__,__TIME__));
  
  lc_setup();

  //pinMode(sensor_arroz_pin, INPUT);
}
///////////////////////////////////////////////////////////////////////////////////////////////
void loop() {
  DateTime fecha = rtc.now();
  
  // read the input pin:
  //int buttonState = digitalRead(pushButton);
  // delay in between reads for stability
  //delay(250);
  // read without samples.
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
  
  Serial.print(t); Serial.print(" *C, ");
  Serial.print(h); Serial.println(" H");
  
  sustratoASensorValue = analogRead(sustratoASensorPin);
  sustratoAPercentValue = map(sustratoASensorValue,1010,840,0,100);
  Serial.print("\nsustratoPercentValue: ");
  Serial.print(sustratoAPercentValue);
  //Serial.print(sustratoASensorValue);
  Serial.print("%");
  Serial.print("\n");

  sustratoPSensorValue = analogRead(sustratoPSensorPin);
  sustratoPPercentValue = map(sustratoPSensorValue,1020,880,0,100);
  Serial.print("\nsustratoPercentValue: ");
  //Serial.print(sustratoPPercentValue);
  Serial.print(sustratoPSensorValue);
  //Serial.print("%");
  Serial.print("\n");
  
  Temp_C = t;
  Humidity = h;

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
  lcd.print("HS:");
  lcd.print(sustratoPPercentValue);
  lcd.print("%");
  
  /////IF STATEMENTS /////////
  if (fecha.hour() == 6 && fecha.minute() == 10 && fecha.second() == 15) {
    Send();
    delay(1000);
    lcd.backlight();
    digitalWrite(pin_motor_coco, LOW);
    digitalWrite(pin_motor_lana, LOW);
  }

  if (fecha.hour() == 6 && fecha.minute() == 14 && fecha.second() == 30) {
    digitalWrite(pin_motor_coco, HIGH);
    digitalWrite(pin_motor_lana, HIGH);
  }

  if (fecha.hour() == 7 && fecha.minute() == 01 && fecha.second() == 15) {
    Send();
    delay(500);
    digitalWrite(pin_motor_coco, LOW);
    digitalWrite(pin_motor_lana, LOW);
  }

  if (fecha.hour() == 7 && fecha.minute() == 05 && fecha.second() == 15) {
    digitalWrite(pin_motor_coco, HIGH);
    digitalWrite(pin_motor_lana, HIGH);
  }

  if (fecha.hour() == 8 && fecha.minute() == 01 && fecha.second() == 15) {
    Send();
    delay(500);
    digitalWrite(pin_motor_coco, LOW);
    digitalWrite(pin_motor_lana, LOW);
  }

  if (fecha.hour() == 8 && fecha.minute() == 05 && fecha.second() == 15) {
    digitalWrite(pin_motor_coco, HIGH);
    digitalWrite(pin_motor_lana, HIGH);
  }

  if (fecha.hour() == 9 && fecha.minute() == 01 && fecha.second() == 15) {
    Send();
    delay(500);
    digitalWrite(pin_motor_coco, LOW);
    digitalWrite(pin_motor_lana, LOW);
  }

  if (fecha.hour() == 9 && fecha.minute() == 05 && fecha.second() == 15) {
    digitalWrite(pin_motor_coco, HIGH);
    digitalWrite(pin_motor_lana, HIGH);
  }

  if (fecha.hour() == 10 && fecha.minute() == 01 && fecha.second() == 15) {
    Send();
    delay(500);
    digitalWrite(pin_motor_coco, LOW);
    digitalWrite(pin_motor_lana, LOW);
  }

  if (fecha.hour() == 10 && fecha.minute() == 05 && fecha.second() == 15) {
    digitalWrite(pin_motor_coco, HIGH);
    digitalWrite(pin_motor_lana, HIGH);
  }

  if (fecha.hour() == 11 && fecha.minute() == 01 && fecha.second() == 15) {
    Send();
    delay(500);
    digitalWrite(pin_motor_coco, LOW);
    digitalWrite(pin_motor_lana, LOW);
  }

  if (fecha.hour() == 11 && fecha.minute() == 05 && fecha.second() == 15) {
    digitalWrite(pin_motor_coco, HIGH);
    digitalWrite(pin_motor_lana, HIGH);
  }

  if (fecha.hour() == 12 && fecha.minute() == 01 && fecha.second() == 15) {
    Send();
  }

  if (fecha.hour() == 12 && fecha.minute() == 10 && fecha.second() == 15) {
    digitalWrite(pin_motor_coco, LOW);
    digitalWrite(pin_motor_lana, LOW);
  }

  if (fecha.hour() == 12 && fecha.minute() == 14 && fecha.second() == 15) {
    digitalWrite(pin_motor_coco, HIGH);
    digitalWrite(pin_motor_lana, HIGH);
  }

  if (fecha.hour() == 13 && fecha.minute() == 01 && fecha.second() == 15) {
    Send();
    delay(500);
    digitalWrite(pin_motor_coco, LOW);
    digitalWrite(pin_motor_lana, LOW);
  }

  if (fecha.hour() == 13 && fecha.minute() == 05 && fecha.second() == 15) {
    digitalWrite(pin_motor_coco, HIGH);
    digitalWrite(pin_motor_lana, HIGH);
  }

  if (fecha.hour() == 14 && fecha.minute() == 01 && fecha.second() == 15) {
    Send();
    delay(500);
    digitalWrite(pin_motor_coco, LOW);
    digitalWrite(pin_motor_lana, LOW);
  }

  if (fecha.hour() == 14 && fecha.minute() == 05 && fecha.second() == 15) {
    digitalWrite(pin_motor_coco, HIGH);
    digitalWrite(pin_motor_lana, HIGH);
  }

  if (fecha.hour() == 15 && fecha.minute() == 01 && fecha.second() == 15) {
    Send();
    delay(500);
    digitalWrite(pin_motor_coco, LOW);
    digitalWrite(pin_motor_lana, LOW);
  }


  if (fecha.hour() == 15 && fecha.minute() == 05 && fecha.second() == 15) {
    digitalWrite(pin_motor_coco, HIGH);
    digitalWrite(pin_motor_lana, HIGH);
  }

  
  if (fecha.hour() == 16 && fecha.minute() == 01 && fecha.second() == 15) {
    Send();
    delay(500);
    digitalWrite(pin_motor_coco, LOW);
    digitalWrite(pin_motor_lana, LOW);
  }

  if (fecha.hour() == 16 && fecha.minute() == 05 && fecha.second() == 15) {
    digitalWrite(pin_motor_coco, HIGH);
    digitalWrite(pin_motor_lana, HIGH);
  }

  if (fecha.hour() == 17 && fecha.minute() == 01 && fecha.second() == 15) {
    Send();
    delay(500);
    digitalWrite(pin_motor_coco, LOW);
    digitalWrite(pin_motor_lana, LOW);
  }
  
  if (fecha.hour() == 17 && fecha.minute() == 05 && fecha.second() == 15) {
    digitalWrite(pin_motor_coco, HIGH);
    digitalWrite(pin_motor_lana, HIGH);
  }

  if (fecha.hour() == 18 && fecha.minute() == 01 && fecha.second() == 15) {
    Send();
    lcd.noBacklight();
  }

  if (fecha.hour() == 18 && fecha.minute() == 10 && fecha.second() == 15) {
    digitalWrite(pin_motor_coco, LOW);
    digitalWrite(pin_motor_lana, LOW);
  }
  if (fecha.hour() == 18 && fecha.minute() == 14 && fecha.second() == 15) {
    digitalWrite(pin_motor_coco, HIGH);
    digitalWrite(pin_motor_lana, HIGH);
  }

  if (fecha.hour() == 19 && fecha.minute() == 01 && fecha.second() == 15) {
    Send();
  }

  if (fecha.hour() == 20 && fecha.minute() == 01 && fecha.second() == 15) {
    Send();
  }

  if (fecha.hour() == 21 && fecha.minute() == 01 && fecha.second() == 15) {
    Send();

  }

  if (fecha.hour() == 22 && fecha.minute() == 01 && fecha.second() == 15) {
    Send();

  }

  if (fecha.hour() == 23 && fecha.minute() == 01 && fecha.second() == 15) {
    Send();

  }

  if (fecha.hour() == 00 && fecha.minute() == 01 && fecha.second() == 15) {
    Send();

  }
  
  if (fecha.hour() == 0 && fecha.minute() == 01 && fecha.second() == 15) {
    Send();
  }

  if (fecha.hour() == 1 && fecha.minute() == 01 && fecha.second() == 15) {
    Send();
  }

  if (fecha.hour() == 2 && fecha.minute() == 01 && fecha.second() == 15) {
    Send();
  }

  if (fecha.hour() == 3 && fecha.minute() == 01 && fecha.second() == 15) {
    Send();
  }
  
  if (fecha.hour() == 4 && fecha.minute() == 01 && fecha.second() == 15) {
    Send();
  }

  if (fecha.hour() == 5 && fecha.minute() == 01 && fecha.second() == 15) {
    Send();
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
  
  if(sustratoAPercentValue < 40){
    digitalWrite(pin_motor_arroz, LOW);
    delay(6000);
    digitalWrite(pin_motor_arroz, HIGH);
  }

  if(sustratoPPercentValue < 45){
    digitalWrite(pin_motor_perlita, LOW);
    delay(6000);
    digitalWrite(pin_motor_perlita, HIGH);
  }

  //if(digitalRead(sensor_arroz_pin) == HIGH){
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
  cmd += Humidity;
  cmd += "&field3="; //field
  cmd += sustratoAPercentValue;
  cmd += "&field4="; //field
  cmd += sustratoPPercentValue;
  //cmd += "&field5="; //field
  //cmd += 100;
  //cmd += "&field6="; //field
  //cmd += 1000;
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
