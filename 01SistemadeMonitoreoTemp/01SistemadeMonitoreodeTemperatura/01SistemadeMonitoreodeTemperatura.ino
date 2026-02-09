#include <DHT.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// Pines
#define DHTPIN     4
#define DHTTYPE    DHT22
#define LED_VERDE  5
#define LED_ROJO   18
#define BUZZER     19
#define PIN_POT    34 

LiquidCrystal_I2C lcd(0x27, 16, 2); 

DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(115200);
  dht.begin();
  
  pinMode(LED_VERDE, OUTPUT);
  pinMode(LED_ROJO, OUTPUT);
  pinMode(BUZZER, OUTPUT);
  
  // NO pinMode para el potenciómetro → es analógico
  
  lcd.init();
  lcd.backlight();
  
  lcd.setCursor(0, 0);
  lcd.print("Estacion Iniciada");
  delay(2000);
  lcd.clear();
}

void loop() {
  // 1. Leer temperatura y humedad
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  
  if (isnan(h) || isnan(t)) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Error sensor!");
    Serial.println("Error lectura DHT!");
    delay(2000);
    return;
  }

  // 2. Leer el potenciómetro y calcular umbral variable
  int valorPot = analogRead(PIN_POT);                   // 0 → 4095
  float umbralAlerta = map(valorPot, 0, 4095, 200, 400); // mapa a 20.0 → 40.0
  umbralAlerta = umbralAlerta / 10.0;                   // convierte 200→20.0, 400→40.0

  // 3. Mostrar en LCD (intentamos que quepa todo en 16×2)
  lcd.clear();
  
  // Línea 1: Temperatura actual + umbral
  lcd.setCursor(0, 0);
  lcd.print("T:");
  lcd.print(t, 1);          // 1 decimal
  lcd.print(" U:");
  lcd.print(umbralAlerta, 1);

  // Línea 2: Humedad + estado
  lcd.setCursor(0, 1);
  lcd.print("H:");
  lcd.print(h, 0);
  lcd.print("% ");
  
  // 4. Lógica de alerta
  if (t > umbralAlerta) {
    digitalWrite(LED_VERDE, LOW);
    digitalWrite(LED_ROJO, HIGH);
    tone(BUZZER, 800, 500);
    
    lcd.print("ALERTA!");
  } 
  else {
    digitalWrite(LED_VERDE, HIGH);
    digitalWrite(LED_ROJO, LOW);
    noTone(BUZZER);
    
    lcd.print("Normal");
  }

  // Debug por Serial (opcional pero útil)
  Serial.print("T: "); Serial.print(t,1);
  Serial.print(" °C  H: "); Serial.print(h,0);
  Serial.print("%  Umbral: "); Serial.print(umbralAlerta,1);
  Serial.print(" °C  Pot: "); Serial.println(valorPot);

  delay(1500);   // Actualiza más rápido para que sientas el cambio del potenciómetro
}