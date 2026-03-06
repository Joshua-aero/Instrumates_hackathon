#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <DHT.h>

LiquidCrystal_I2C lcd(0x27,16,2);

// -------- DHT SENSOR --------
#define DHTPIN 11
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

// -------- SENSOR PINS --------
const int gas = A0;
const int floatSwitch = 7;

// -------- ACTUATORS --------
const int buzzer = 8;
const int fan = 9;
const int pump = 10;

// -------- THRESHOLDS --------
const int gasThreshold = 140;
const float tempThreshold = 34.0;

void setup()
{
  Serial.begin(9600);

  pinMode(gas, INPUT);
  pinMode(floatSwitch, INPUT_PULLUP);

  pinMode(buzzer, OUTPUT);
  pinMode(fan, OUTPUT);
  pinMode(pump, OUTPUT);

  lcd.init();
  lcd.backlight();

  dht.begin();
}

void loop()
{
  // -------- SENSOR READINGS --------
  int gasValue = analogRead(gas);
  float humidity = dht.readHumidity();
  float temperature = dht.readTemperature();
  int waterLevel = digitalRead(floatSwitch);

  // -------- ALERT FLAGS --------
  bool tempAlert = false;
  bool gasAlert = false;
  bool waterAlert = false;

  if(temperature > tempThreshold)
    tempAlert = true;

  if(gasValue > gasThreshold)
    gasAlert = true;

  if(waterLevel == LOW)
    waterAlert = true;

  // -------- SERIAL MONITOR OUTPUT --------
  Serial.println("------------------------------------------------");

  Serial.print("Gas: ");
  Serial.print(gasValue);

  Serial.print("   Temp: ");
  Serial.print(temperature);
  Serial.print(" C");

  Serial.print("   Humidity: ");
  Serial.print(humidity);
  Serial.print(" %");

  Serial.print("   WaterLevel: ");
  Serial.println(waterLevel);

  if(tempAlert || gasAlert || waterAlert)
  {
    Serial.print("ALERT: ");

    if(tempAlert)
      Serial.print("Temperature Rising  ");

    if(gasAlert)
      Serial.print("Gas Leakage  ");

    if(waterAlert)
      Serial.print("Water Level High  ");

    Serial.println();
  }
  else
  {
    Serial.println("Status: System Normal");
  }

  Serial.println("------------------------------------------------");
  Serial.println();

  // -------- LCD SENSOR DISPLAY (SCROLL) --------
  lcd.clear();

  lcd.setCursor(16,0);
  lcd.print("Temp:");
  lcd.print(temperature);
  lcd.print("C ");

  lcd.print("Hum:");
  lcd.print(humidity);
  lcd.print("% ");

  lcd.setCursor(16,1);
  lcd.print("Gas:");
  lcd.print(gasValue);

  lcd.autoscroll();

  delay(800);   // reduced from 2000

  lcd.noAutoscroll();
  lcd.clear();

  // -------- ALERT DISPLAY + ACTUATORS --------

  if(tempAlert || gasAlert || waterAlert)
  {
    digitalWrite(buzzer,HIGH);
    digitalWrite(fan,HIGH);

    if(waterAlert)
      digitalWrite(pump,HIGH);

    lcd.setCursor(0,0);

    if(tempAlert)
      lcd.print("Temp Rising!");

    if(gasAlert)
      lcd.print("Gas Leakage!");

    lcd.setCursor(0,1);

    if(waterAlert)
      lcd.print("Water Level High");
  }
  else
  {
    digitalWrite(buzzer,LOW);
    digitalWrite(fan,LOW);
    digitalWrite(pump,LOW);

    lcd.setCursor(0,0);
    lcd.print("System Normal");
  }

  delay(600);   // reduced from 1500
}