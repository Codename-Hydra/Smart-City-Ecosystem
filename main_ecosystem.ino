 /*************************************************************
  Smart City with Gas Detection, Automatic Light, Fire Detection
 *************************************************************/
#define BLYNK_TEMPLATE_ID "TMPL6lAXQsImD"
#define BLYNK_TEMPLATE_NAME "Project P5 kelas XII 12"
#define BLYNK_AUTH_TOKEN "BL7onBGBZD4p9_YytS7RwlKJM4DSWUhX"
#define BLYNK_PRINT Serial

#include <ESP8266_Lib.h>
#include <BlynkSimpleShieldEsp8266.h>
#include <DHT.h>

// WiFi Credentials
char ssid[] = "HARISBIKE";
char pass[] = "naura2017";

// Hardware Serial
#include <SoftwareSerial.h>
SoftwareSerial EspSerial(3, 2); // RX, TX
#define ESP8266_BAUD 115200
ESP8266 wifi(&EspSerial);

// Pin Definitions
#define DHT_PIN A0
#define MQ2_PIN A1
#define LDR_PIN A2
#define BUZZER_PIN 6
#define LED_PIN 5

// Sensor Definitions
#define DHTTYPE DHT22
DHT dht(DHT_PIN, DHTTYPE);
BlynkTimer timer;

// Global Variables
int MQ2_Val = 0;
int ldrValue = 0;

void triggerAlarmTone() {
  for (int i = 1000; i < 2000; i++) { // naikkan frekuensi nada
    tone(BUZZER_PIN, i);
    delay(1);
  }
  for (int i = 2000; i > 1000; i--) { // turunkan frekuensi nada
    tone(BUZZER_PIN, i);
    delay(1);
  }
}

void sendSensor()
{
  // Baca Suhu dan Kelembaban
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  
  if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  // Kirim Data ke Blynk
  Blynk.virtualWrite(V5, h);
  Blynk.virtualWrite(V6, t);

  // Pembacaan Sensor Gas MQ2
  MQ2_Val = analogRead(MQ2_PIN);
  for (int x = 0; x < 100; x++) {
    MQ2_Val = MQ2_Val + analogRead(MQ2_PIN);
  }
  MQ2_Val = MQ2_Val / 100.0;
  Blynk.virtualWrite(V0, MQ2_Val);
  Blynk.virtualWrite(V2, MQ2_Val);

  // Pembacaan Sensor Cahaya LDR
  ldrValue = analogRead(LDR_PIN);
  Blynk.virtualWrite(V1, ldrValue);

  // Kontrol Lampu Berdasarkan Intensitas Cahaya
  if (ldrValue <= 300) {
    Blynk.logEvent("malam");
  } else {
    Blynk.logEvent("siang");
  }

  // Deteksi Gas
  if (MQ2_Val > 900) {
    Blynk.logEvent("gas_detected");
    triggerAlarmTone();
  }

  // Deteksi Kebakaran
  if (t > 40) {
    Blynk.logEvent("fire_detected");
    triggerAlarmTone();
    digitalWrite(LED_PIN, LOW);  // Matikan LED secara total
  }

  // Debug Serial
  //Serial.print("MQ2 Level: ");
  //Serial.println(MQ2_Val);
  //Serial.print("Temperature: ");
  //Serial.println(t);
  //Serial.print("LDR Value: ");
  //Serial.println(ldrValue);
}

void setup()
{
  // Inisialisasi Serial
  Serial.begin(115200);
  EspSerial.begin(ESP8266_BAUD);
  delay(10);

  // Konfigurasi Pin
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(LED_PIN, OUTPUT);

  // Koneksi Blynk
  Blynk.begin(BLYNK_AUTH_TOKEN, wifi, ssid, pass, "blynk.cloud", 80);
  
  // Inisialisasi Sensor
  dht.begin();
  
  // Setup Timer
  timer.setInterval(1000L, sendSensor);
}

void loop()
{
  Blynk.run();
  timer.run();
}
