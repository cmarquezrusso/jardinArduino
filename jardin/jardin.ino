/**
 * Indoor Automation - Cristian Marquez Russo
 * DH11 Pin 2 Digital with resistence 10K
 * GroundHumedity: A0
 * Water Bomb = D36
 * Light = D37
 * Ground Humedity = D41
 * Water Temp = D40
 * RTC SDA = A4
 * RTC SDC = A5
 **/

#include <LiquidCrystal.h>
#include <OneWire.h>
#include "DHT.h"
#include "Wire.h"

#define DS1307_ADDRESS 0x68
#define DHTPIN 2     // what pin we're connected to
#define DHTTYPE DHT11   // DHT 11 

#undef int
#undef abs
#undef double
#undef float
#undef round

int watterBombPin = 36;
int lightPin = 37 ;
int groundHumedityPin=41;
int watterTempPin = 40;
int RTCSDAPin = A4;
int RTCSCLPin = A5;

LiquidCrystal lcd(7, 8, 9, 10, 11 , 12);
DHT dht(DHTPIN, DHTTYPE);
OneWire ds(watterTempPin);  // on pin 10 (a 4.7K resistor is necessary)

static float celsius;

//parameters for schedule the lights
int lightHourOn = 6;
int lightHourOff = 12;

//parameters for watter
boolean alreadyWatering=false;

//parameters for delay
int delayLCD = 1500;

//Variables for rtc and date

int second;
int minute;
int hour;
int weekDay; 
int monthDay ;
int month;
int year;

void setup() {           
  Serial.begin(9600); //Serial debug messages
  pinMode(watterBombPin, OUTPUT); //PinMode for the watter dispenser
  pinMode(lightPin, OUTPUT); //PinMode for the Sodium light
  pinMode(groundHumedityPin, INPUT_PULLUP);
  pinMode(13, OUTPUT);
  digitalWrite(watterBombPin, HIGH); //HIGH is OFF for the relee
  digitalWrite(lightPin, HIGH);   //HIGH is OFF for the relee
  Wire.begin();  //Watter temperature library
  dht.begin(); //DHT library
  lcd.begin(16, 2); //LCD library
  startUpInfo(); //LCD and Serial messages
}

void startUpInfo()
{
  Serial.println("########################################");
  Serial.println("## Arduino Indoor by Cristian Marquez ##");
  Serial.println("########################################");
  Serial.println("Loading sensors...");
  Serial.println("Loading misc components... ");
  Serial.println("Getting time...");
  cursorBlink();
  lcd.print("Arduino Indoor");
  lcd.setCursor(0,1);
  lcd.print("Cristian Marquez");
  cursorBlink();
  lcd.clear();
  lcd.print("Auspician:");
  lcd.setCursor(0,1);
  lcd.print("Joaquin Galvan");
  delay(delayLCD);
  lcd.clear();
  lcd.print("Auspician:");
  lcd.setCursor(0,1);
  lcd.print("Rodrigo Acuna");
  delay(delayLCD);
  lcd.clear();
  lcd.print("Auspician:");
  lcd.setCursor(0,1);
  lcd.print("Negro Rami");
}

void cursorBlink()
{
  for(int i=0;i<3;i++)
  {
    lcd.noBlink();
    delay(300);
    // Turn on the blinking cursor:
    lcd.blink();
    delay(300);
  }
}

void sensorsInfo()
{
  Serial.print("Temp: ");
  Serial.print(getTemp());
  Serial.println("C");
  Serial.print("Humidity: ");
  Serial.print(getHumidity());
  Serial.println("%");
  Serial.print("Ground Humidity: ");
  Serial.print(getGroundHumidity());
  Serial.println("%");
  Serial.print("Watter Temperature: ");
  Serial.print(getWatterTemperature());
  Serial.println("C");
  Serial.print("Date:");
  Serial.print(month);
  Serial.print("/");
  Serial.print(monthDay);
  Serial.print("/");
  Serial.print(year);
  Serial.print(" ");
  Serial.print(hour);
  Serial.print(":");
  Serial.print(minute);
  Serial.print(":");
  Serial.println(second);
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Humedad:");
  lcd.print(getHumidity());
  lcd.print(" %"); 
  lcd.setCursor(0,1);
  lcd.print("Temperatura:");
  lcd.print(getTemp());
  lcd.print(" *C");  
  delay(delayLCD);
  updateDate();
  lcd.clear();
  lcd.print("Fecha: ");
  lcd.print(monthDay);
  lcd.print("/");
  lcd.print(month);
  lcd.print("/");
  lcd.print(year);
  lcd.setCursor(0,1);
  lcd.print("Hora: ");
  lcd.print(hour);
  lcd.print(":");
  lcd.print(minute);
  lcd.print(":");
  lcd.print(second);
  delay(delayLCD);
  lcd.clear();
  lcd.print("Temp Agua: ");
  lcd.print(getWatterTemperature());
  lcd.print("*C");
  lcd.setCursor(0,1);
  lcd.print("Humedad Tierra: ");
  lcd.print(getGroundHumidity()); //0 is good. 1 is bad
  lcd.print("%");
}

void loop() {
  sensorsInfo();
  delay(delayLCD);
  if(hour>=lightHourOff && hour < lightHourOn )
  {
    Serial.println("Lights OFF");
    lcd.clear();
    lcd.print("Evento:");
    lcd.setCursor(0,1);
    lcd.print("Luces OFF");
    turnLights(false);  
  }
  else
  {
    Serial.println("Lights ON");
    lcd.clear();
    lcd.print("Evento:");
    lcd.setCursor(0,1);
    lcd.print("Luces ON");
    turnLights(true);
  }
  delay(delayLCD);
  //Fix: Watter in 5 days
  if(monthDay % 5 == 0 && alreadyWatering == false)
  {
    alreadyWatering = true; //this is to avoid watering more than 1 time
    Serial.println("Watter ON");
    lcd.clear();
    lcd.print("Evento:");
    lcd.setCursor(0,1);
    lcd.print("Agua ON");
    turnWatterBomb(true);
    delay(20000);
    turnWatterBomb(false);
    delay(40000);
  }
  else
  {
    alreadyWatering = false;
    Serial.println("Watter OFF");
    lcd.clear();
    lcd.print("Evento:");
    lcd.setCursor(0,1);
    lcd.print("Agua OFF");
    turnWatterBomb(false);
  }
  delay(delayLCD);
  if(getTemp()<=20)
  {
    Serial.println("The room temperature is too low, sending an alarm");
    lcd.clear();
    lcd.print("Evento:");
    lcd.setCursor(0,1);
    lcd.print("Temp BAJA");  
  }
  else
  {
    Serial.println("The room temperature is OK");
    lcd.clear();
    lcd.print("Evento:");
    lcd.setCursor(0,1);
    lcd.print("Temperatura OK");  
  }
  delay(delayLCD);

  if(getWatterTemperature() < 20)
  {
    Serial.println("Watter is cold");
    lcd.clear();
    lcd.print("Evento:");
    lcd.setCursor(0,1);
    lcd.print("Agua FRIA");
  }
  else
  {
    Serial.println("Watter is OK");
    lcd.clear();
    lcd.print("Evento:");
    lcd.setCursor(0,1);
    lcd.print("Agua TEMPLADA");
  }
  delay(delayLCD);
  if(getGroundHumidity()==0)
  {
    lcd.clear();
    lcd.print("Evento:");
    lcd.setCursor(0,1);
    lcd.print("Tengo Agua");
  }
  else
  {
    lcd.clear();
    lcd.print("Evento:");
    lcd.setCursor(0,1);
    lcd.print("Me Falta Agua");
  }
  delay(delayLCD);
}
void turnWatterBomb(boolean t)
{
  if(t)
  {
    digitalWrite(watterBombPin, LOW); 
  }
  else
  {
    digitalWrite(watterBombPin, HIGH); 
  }
}

void turnLights(boolean t)
{
  if(t)
  {
    digitalWrite(lightPin, LOW); 
  }
  else
  {
    digitalWrite(lightPin, HIGH); 
  }

}
int getTemp()
{
  return dht.readTemperature();
}

int getHumidity()
{
  return dht.readHumidity();
}

int getGroundHumidity()
{
  int sensorValue = digitalRead(groundHumedityPin);
  return sensorValue;  
}

int getWatterTemperature()
{
  getWatter();
  return celsius;
}

void getWatter()
{
  byte i;
  byte present = 0;
  byte type_s;
  byte data[12];
  byte addr[8];
  if ( !ds.search(addr)) {
    ds.reset_search();
    delay(250);
    return;
  }


  if (OneWire::crc8(addr, 7) != addr[7]) {
    return;
  }

  ds.reset();
  ds.select(addr);
  ds.write(0x44, 1);        // start conversion, with parasite power on at the end

  delay(1000);     // maybe 750ms is enough, maybe not
  // we might do a ds.depower() here, but the reset will take care of it.

  present = ds.reset();
  ds.select(addr);    
  ds.write(0xBE);         // Read Scratchpad

  for ( i = 0; i < 9; i++) {           // we need 9 bytes
    data[i] = ds.read();
  }

  // Convert the data to actual temperature
  // because the result is a 16 bit signed integer, it should
  // be stored to an "int16_t" type, which is always 16 bits
  // even when compiled on a 32 bit processor.
  int16_t raw = (data[1] << 8) | data[0];
  if (type_s) {
    raw = raw << 3; // 9 bit resolution default
    if (data[7] == 0x10) {
      // "count remain" gives full 12 bit resolution
      raw = (raw & 0xFFF0) + 12 - data[6];
    }
  } 
  else {
    byte cfg = (data[4] & 0x60);
    // at lower res, the low bits are undefined, so let's zero them
    if (cfg == 0x00) raw = raw & ~7;  // 9 bit resolution, 93.75 ms
    else if (cfg == 0x20) raw = raw & ~3; // 10 bit res, 187.5 ms
    else if (cfg == 0x40) raw = raw & ~1; // 11 bit res, 375 ms
    //// default is 12 bit resolution, 750 ms conversion time
  }
  celsius = (float)raw / 16.0;  
  Serial.print(celsius);
}

byte bcdToDec(byte val)  {
  // Convert binary coded decimal to normal decimal numbers
  return ( (val/16*10) + (val%16) );
}

void updateDate(){

  // Reset the register pointer
  Wire.beginTransmission(DS1307_ADDRESS);

  byte zero = 0x00;
  Wire.write(zero);
  Wire.endTransmission();

  Wire.requestFrom(DS1307_ADDRESS, 7);

  second = bcdToDec(Wire.read());
  minute = bcdToDec(Wire.read());
  hour = bcdToDec(Wire.read() & 0b111111); //24 hour time
  weekDay = bcdToDec(Wire.read()); //0-6 -> sunday - Saturday
  monthDay = bcdToDec(Wire.read());
  month = bcdToDec(Wire.read());
  year = bcdToDec(Wire.read());
}
