#include <WiFiS3.h>
#include <DHT.h>

#define DHTPIN 2 // Pin DATA konek ke pin 2 Arduino
#define DHTTYPE DHT11 // Tipe sensor DHT11
DHT dht(DHTPIN, DHTTYPE); // Set sensor + koneksi pin

const int MQ6 = A0;
int nilaiSensor;
float humi, temp;

// Ganti dengan SSID dan Password WiFi Anda
const char* ssid = "Berkah";
const char* password = "ruangan240";

const char* server = "202.125.83.143"; // URL server tanpa "http://"
const int port = 80; // Port HTTP

void setup() {
  dht.begin();
  Serial.begin(115200); // Baud rate
  delay(10);

  // Mulai koneksi WiFi
  WiFi.begin(ssid, password);

  // Tunggu hingga terhubung
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Menghubungkan ke WiFi...");
  }

  Serial.println("Terhubung ke WiFi");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  nilaiSensor = analogRead(MQ6);
  Serial.print("MQ-6 =====>>> Nilai Sensor: ");
  Serial.println(nilaiSensor);
  delay(1000);

  humi = dht.readHumidity(); // Baca kelembaban
  temp = dht.readTemperature(); // Baca suhu

  if (isnan(humi) || isnan(temp)) { // Jika tidak ada hasil
    Serial.println("DHT11 tidak terbaca... !");
    return;
  } else { // Jika ada hasilnya
    Serial.print("Suhu=");
    Serial.print(temp);
    Serial.println("C");
    Serial.print("Kelembapan=");
    Serial.print(humi);
    Serial.println("%RH");
  }

  // Kirim data ke server
  if (WiFi.status() == WL_CONNECTED) { // Jika terhubung ke WiFi
    WiFiClient client;

    if (client.connect(server, port)) {
      String url = "/insert_data.php";
      url += "?suhu=";
      url += temp;
      url += "&kelembapan=";
      url += humi;
      url += "&nilaiSensor=";
      url += nilaiSensor;

      client.print(String("GET ") + url + " HTTP/1.1\r\n" +
                   "Host: " + server + "\r\n" +
                   "Connection: close\r\n\r\n");

      // Tunggu respon dari server
      unsigned long timeout = millis();
      while (client.connected() && millis() - timeout < 5000) {
        if (client.available()) {
          String line = client.readStringUntil('\r');
          Serial.print(line);
        }
      }

      // Tutup koneksi
      client.stop();
      Serial.println("\n[Connection closed]");
    } else {
      Serial.println("Koneksi ke server gagal");
    }
  } else {
    Serial.println("WiFi Disconnected");
  }

  delay(10000); // Tunda 10 detik untuk pembacaan berikutnya
}
