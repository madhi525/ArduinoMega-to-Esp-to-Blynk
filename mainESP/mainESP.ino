#define BLYNK_TEMPLATE_ID "BLYNK_TEMPLATE_ID_ANDA" // Ganti dengan Template ID Anda
#define BLYNK_TEMPLATE_NAME "BLYNK_TEMPLATE_NAME_ANDA" // Ganti dengan Template Name Anda
#define BLYNK_AUTH_TOKEN "BLYNK_AUTH_TOKEN_ANDA" // Ganti dengan Token autentikasi Blynk Anda

#include <SoftwareSerial.h>
#include <BlynkSimpleEsp8266.h>
#include <WiFiUdp.h>
#include <NTPClient.h>
#include <ArduinoJson.h>
#include <ESP8266HTTPClient.h>

char ssid[] = "SSID_wifi"; // Ganti dengan SSID Wi-Fi Anda
char pass[] = "pass_wifi"; // Ganti dengan password Wi-Fi Anda

#define RX_PIN 3
#define TX_PIN 2
#define D2_PIN 4
#define D1_PIN 5

SoftwareSerial mySerial(RX_PIN, TX_PIN);
SoftwareSerial espToMega(D2_PIN , D1_PIN);
WiFiUDP udp;
NTPClient timeClient(udp, "pool.ntp.org", 7 * 3600 , 60000); // Konfigurasi NTP WIB

unsigned long previousMillis = 0;
const long interval = 2000;
unsigned long currentMillis = millis();

void setup() {
  Serial.begin(9600);
  mySerial.begin(9600);
  espToMega.begin(9600);
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);

  while (WiFi.status() != WL_CONNECTED && millis() - currentMillis < 30000) {
    delay(1000);
    Serial.println("Menghubungkan ke Wi-Fi...");
  }
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("Terhubung ke Wi-Fi!");
  } else {
    Serial.println("Gagal terhubung ke Wi-Fi.");
  }

  timeClient.begin();
}

// Baca tombol pada BLYNK dengan Virtual Port 5
BLYNK_WRITE(V5) {
  int state = param.asInt();
  if (state == 1) {
    StaticJsonDocument<128> doc;
    doc["tombol"] = state;
    serializeJson(doc, espToMega);
  }
}

String sendDataToFlutter(String waktu, float pH, int turbidity, float suhu_DS18B20) {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin("http://<flutter_server_url>/data"); // Ganti dengan URL server Anda
    http.addHeader("Content-Type", "application/json");

    StaticJsonDocument<200> doc;
    doc["waktu"] = waktu;
    doc["pH"] = pH;
    doc["turbidity"] = turbidity;
    doc["suhu"] = suhu_DS18B20;

    String jsonData;
    serializeJson(doc, jsonData);

    int httpResponseCode = http.POST(jsonData);
    if (httpResponseCode > 0) {
      Serial.print("Response: ");
      Serial.println(httpResponseCode);

      if (httpResponseCode == 200) {
        http.end();
        return "sukses terkirim";
      } else {
        http.end();
        return "error sending data";
      }
    } else {
      Serial.print("Error sending data: ");
      Serial.println(httpResponseCode);
      http.end();
      return "error sending data";
    }
  } else {
    Serial.println("WiFi not connected");
    return "wifi not connected";
  }
}

void updateWaktu(){
  bool timeUpdated = timeClient.update();
  String waktu = "00:00";  // Default
  if (timeUpdated) {
    int jam = timeClient.getHours();
    int menit = timeClient.getMinutes();

    char formattedWaktu[6];  // 5 Karakter + null-terminator
    sprintf(formattedWaktu, "%02d:%02d", jam, menit); // Format waktu ke dalam bentuk ##:##

    waktu = String(formattedWaktu);

    StaticJsonDocument<512> doc;
    doc["waktu"] = waktu;
    serializeJson(doc, espToMega);
    Serial.println("Data dikirim ke mega");
  }
}

void loop() {
  Blynk.run();
  updateWaktu();

  // Variabel untuk mengupdate waktu ke blynk
  char formattedWaktuToBlynk[6];
  String waktuToBlynk;
  int jamToBlynk = timeClient.getHours();
  int menitToBlynk = timeClient.getMinutes();
  sprintf(formattedWaktuToBlynk, "%02d:%02d", jamToBlynk, menitToBlynk);
  waktuToBlynk = String(formattedWaktuToBlynk);

  // Handle data yang masuk dari Mega
  if (currentMillis - previousMillis >= 1500) {
    previousMillis = currentMillis;

    if (mySerial.available()) {
      char data[200];
      mySerial.readBytesUntil('\n', data, sizeof(data) - 1);
      data[sizeof(data) - 1] = '\0'; // Null-terminate string

      StaticJsonDocument<200> doc;
      DeserializationError error = deserializeJson(doc, data);

      if (!error) {
        // Ambil data dari JSON
        String stringpH = doc["pH"];
        String stringTurbidity = doc["turbidity"];
        String stringSuhu_DS18B20 = doc["suhu_DS18B20"];

        // Sterilisasi Data yang di terima
        float pH = stringpH.toFloat();
        int turbidity = stringTurbidity.toInt();
        float suhu_DS18B20 = stringSuhu_DS18B20.toFloat();

        // Kirim data ke Blynk
        Blynk.virtualWrite(V0, pH);   
        Blynk.virtualWrite(V1, turbidity);   
        Blynk.virtualWrite(V2, suhu_DS18B20); 
        Blynk.virtualWrite(V3, waktuToBlynk);            

        // Kirim data ke Flutter
        String statusData = sendDataToFlutter(waktuToBlynk, pH, turbidity, suhu_DS18B20);
        Serial.println("Status data yang di kirim ke flutter: " + statusData);

        // Debugging: tampilkan data di serial monitor
        Serial.print("pH: ");
        Serial.println(stringpH);
        Serial.print("Turbidity: ");
        Serial.println(stringTurbidity);
        Serial.print("Suhu DS18B20: ");
        Serial.println(stringSuhu_DS18B20);
        Serial.print("Waktu: ");
        Serial.println(waktuToBlynk);
      }
    }
  }
}
