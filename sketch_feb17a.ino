// Konfigurasi Blynk
#define BLYNK_TEMPLATE_ID "TMPL6j-zkDURO"
#define BLYNK_TEMPLATE_NAME "suhusensor"
#define BLYNK_AUTH_TOKEN "Vg3-qzYO4b0KRE1c1aj8HAVuXP6QSNZi"

#include <WiFi.h>
#include <Firebase_ESP_Client.h>
#include <DHT.h>
#include <BlynkSimpleEsp32.h> // Tambahkan library Blynk

// Konfigurasi DHT22
#define DHTPIN 4
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);

// Konfigurasi WiFi
#define WIFI_SSID "Undiksha FTK"
#define WIFI_PASSWORD "JarEnterprise24#"

// Konfigurasi Firebase
#define FIREBASE_API_KEY "AIzaSyC4XPiGuvw8sYC--Q4WmUgb3Shbivp0bm8"
#define FIREBASE_URL "https://suhubaru-9a2d7-default-rtdb.asia-southeast1.firebasedatabase.app/"

// Objek Firebase
FirebaseData firebaseData;
FirebaseAuth firebaseAuth;
FirebaseConfig firebaseConfig;

void setup() {
  Serial.begin(115200);

  // Inisialisasi DHT22
  Serial.println("Inisialisasi DHT22...");
  dht.begin();

  // Koneksi WiFi
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Menghubungkan ke WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println("\nWiFi Terhubung!");

  // Konfigurasi Firebase
  firebaseConfig.api_key = FIREBASE_API_KEY;
  firebaseConfig.database_url = FIREBASE_URL;
  Firebase.begin(&firebaseConfig, &firebaseAuth);
  Firebase.reconnectWiFi(true);

  // Gunakan Autentikasi Anonymous
  if (Firebase.signUp(&firebaseConfig, &firebaseAuth, "", "")) {
    Serial.println("Firebase SIAP!");
  } else {
    Serial.println("Firebase GAGAL tersambung!");
    Serial.println(firebaseData.errorReason());
  }

  // Konfigurasi Blynk
  Blynk.begin(BLYNK_AUTH_TOKEN, WIFI_SSID, WIFI_PASSWORD);
  Serial.println("Blynk SIAP!");
}

void checkFirebaseConnection() {
  if (!Firebase.ready()) {
    Serial.println("Firebase tidak tersambung, mencoba reconnect...");
    Firebase.begin(&firebaseConfig, &firebaseAuth);
    Firebase.reconnectWiFi(true);
  }
}

void loop() {
  // Jalankan Blynk
  Blynk.run();

  // Periksa koneksi WiFi
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("Kehilangan koneksi WiFi, mencoba reconnect...");
    WiFi.reconnect();
    delay(5000);
    return;
  }

  // Periksa koneksi Firebase
  checkFirebaseConnection();

  // Baca data dari DHT22
  float humidity = dht.readHumidity();
  float temperature = dht.readTemperature();
  if (isnan(humidity) || isnan(temperature)) {
    Serial.println("Gagal membaca sensor!");
    return;
  }

  Serial.print("Kelembaban: ");
  Serial.print(humidity);
  Serial.println(" %");
  Serial.print("Suhu: ");
  Serial.print(temperature);
  Serial.println(" °C");

  // Kirim data ke Firebase
  if (Firebase.RTDB.setFloat(&firebaseData, "/Humidity", humidity)) {
    Serial.println("Data Kelembaban berhasil dikirim!");
  } else {
    Serial.println("Gagal mengirim data Kelembaban!");
    Serial.println(firebaseData.errorReason());
  }

  if (Firebase.RTDB.setFloat(&firebaseData, "/Temperature", temperature)) {
    Serial.println("Data Suhu berhasil dikirim!");
  } else {
    Serial.println("Gagal mengirim data Suhu!");
    Serial.println(firebaseData.errorReason());
  }

  // Kirim data ke Blynk
  Blynk.virtualWrite(V1, temperature); // Kirim suhu ke Virtual Pin V1
  Blynk.virtualWrite(V2, humidity);   // Kirim kelembaban ke Virtual Pin V2

  delay(5000); // Tunggu 5 detik sebelum membaca ulang
}