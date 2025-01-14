#include <DHT.h>
#include <Wire.h>
#include <Servo.h>
#include <LiquidCrystal_I2C.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <ArduinoJson.h>

#define PH_PIN A0
#define TURBIDITY_PIN A1
#define SERVO_PIN 9
#define ONE_WIRE_BUS 7

Servo servoMotor;
LiquidCrystal_I2C lcd(0x27, 16, 2);
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

void setup() {
  Serial.begin(9600);
  Serial1.begin(9600);     // Inisialisasi komunikasi serial dengan ESP8266 di Serial1 (untuk mengirim data ke ESP)
  Serial2.begin(9600);     // Inisialisasi komunikasi serial dengan ESP8266 di Serial2 (untuk Menerima data dari ESP)
  Serial2.setTimeout(200);
  
  lcd.begin(16, 2);
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Inisialisasi...");

  servoMotor.attach(SERVO_PIN);
  sensors.begin();

  delay(1000);
}

void cekTombolDitekan(){
  if(Serial2.available() > 0){
    String dataDiterima = Serial2.readStringUntil('\n');
    StaticJsonDocument<256> doc;
    DeserializationError error = deserializeJson(doc, dataDiterima);

    if(!error){
      if(doc.containsKey("tombol")){
        const char* tombolkey = doc["tombol"];
        Serial.println(tombolkey);
        servoMotor.write(360);
      } else {
        servoMotor.write(0);
        const char* result = doc["waktu"];
        Serial.println(result);
        lcd.setCursor(0, 0);
        String waktu = "Waktu: " + String(result);
        lcd.print(waktu);
      }
    } else {
      Serial.println("tidak ada data yang di terima");
    }
  }
}

void kirimDataKeEsp(float pH, float suhu, int turbidity){
  // Kirim data ke Serial1 dalam format JSON
  if (!isnan(pH) && !isnan(turbidity)) {
    char pHStr[6];
    dtostrf(pH, 4, 2, pHStr);
    StaticJsonDocument<200> doc;
    doc["pH"] = pH;
    doc["turbidity"] = turbidity;
    doc["suhu_DS18B20"] = suhu;

    // Kirim data ke ESP8266 melalui Serial1
    String output;
    serializeJson(doc, output);  // Serialize objek JSON menjadi string
    output += "\n";  // Menambahkan newline untuk memisahkan pesan
    Serial1.print(output);

    // Debugging: tampilkan data yang dikirimkan ke ESP
    Serial.println("Data yang dikirim ke ESP: " + output);
  }
}

void loop() {
  servoMotor.write(0);
  cekTombolDitekan();
  
  int pHValue = analogRead(PH_PIN);
  int turbidityValue = analogRead(TURBIDITY_PIN);
  float pH = map(pHValue, 0, 1023, 0, 14);  // Konversi nilai pH menjadi nilai pH yang lebih logis (penyesuaian skala)

  // Baca suhu dari sensor DS18B20
  sensors.requestTemperatures();
  float suhuDS18B20 = sensors.getTempCByIndex(0);

  kirimDataKeEsp(pH, suhuDS18B20, turbidityValue);

  // Menampilkan data pada LCD
  lcd.setCursor(0, 1);
  lcd.print("Turbidity: ");
  lcd.print(turbidityValue);
  lcd.setCursor(0, 2);
  lcd.print("Suhu   : ");
  lcd.print(suhuDS18B20);
  lcd.print(" C");

  delay(1000);
}
