#include <HCSR04.h>
#include <Servo.h>

// ----- PIN KONFIGURASI -----
#define TRIG_A 2
#define ECHO_A 12
#define TRIG_B 4
#define ECHO_B 13
#define PIN_SERVO 9
#define PIN_BUZZER 8

// ----- OBJEK SENSOR DAN SERVO -----
UltraSonicDistanceSensor sensorA(TRIG_A, ECHO_A);
UltraSonicDistanceSensor sensorB(TRIG_B, ECHO_B);
Servo palang;

// ----- KONSTANTA -----
const int SUDUT_BUKA   = 0;      // Palang buka
const int SUDUT_TUTUP  = 140;    // Palang tutup
const float JARAK_DETEKSI_CM = 50.0; // Batas deteksi (cm)
const unsigned long WAKTU_TIMEOUT = 5000; // 5 detik
const unsigned long BATAS_WAKTU_DETEKSI = 5000; // 5 detik untuk warning

// ----- VARIABEL -----
unsigned long waktuTerakhirA = 0;
unsigned long waktuTerakhirB = 0;
unsigned long waktuMulaiDeteksiA = 0;
bool buzzerAktif = false;
bool masihTerdeteksiA = false;
bool palangTerbuka = false;

// ----- FUNGSI BUZZER -----
void buzzerNyala() {
  if (!buzzerAktif) {
    digitalWrite(PIN_BUZZER, HIGH);
    buzzerAktif = true;
  }
}

void buzzerMati() {
  if (buzzerAktif) {
    digitalWrite(PIN_BUZZER, LOW);
    buzzerAktif = false;
  }
}

// ----- SETUP -----
void setup() {
  Serial.begin(9600);
  pinMode(PIN_BUZZER, OUTPUT);
  digitalWrite(PIN_BUZZER, LOW);

  palang.attach(PIN_SERVO);
  palang.write(SUDUT_TUTUP);

  Serial.println("Sistem palang siap.");
}

// ----- LOOP UTAMA -----
void loop() {
  float jarakA = sensorA.measureDistanceCm();
  delay(100);
  float jarakB = sensorB.measureDistanceCm();

  bool deteksiA = (jarakA > 0 && jarakA < JARAK_DETEKSI_CM);
  bool deteksiB = (jarakB > 0 && jarakB < JARAK_DETEKSI_CM);

  unsigned long sekarang = millis();

  // ---------------------------
  // Logika Sensor A (buka palang)
  // ---------------------------
  if (deteksiA) {
    if (waktuMulaiDeteksiA == 0) waktuMulaiDeteksiA = sekarang;

    palang.write(SUDUT_BUKA);
    palangTerbuka = true;
    buzzerMati();
    waktuTerakhirA = sekarang;
    Serial.println("Sensor A aktif → Palang dibuka");

    // Jika masih terdeteksi setelah 5 detik → buzzer ON
    if (sekarang - waktuMulaiDeteksiA >= BATAS_WAKTU_DETEKSI) {
      buzzerNyala();
      Serial.println("Sensor A masih aktif setelah 5 detik → Buzzer ON");
    }
  } 
  else {
    waktuMulaiDeteksiA = 0;
  }

  // ---------------------------
  // Logika Sensor B (tutup palang)
  // ---------------------------
  if (deteksiB) {
    palang.write(SUDUT_TUTUP);
    palangTerbuka = false;
    buzzerMati();
    waktuTerakhirB = sekarang;
    Serial.println("Sensor B aktif → Palang ditutup");
  }

  // ---------------------------
  // Jika tidak ada deteksi
  // ---------------------------
  if (!deteksiA && !deteksiB) {
    // Hanya beri peringatan kalau palang terbuka terlalu lama TANPA deteksi baru
    if (palangTerbuka && (sekarang - waktuTerakhirA > WAKTU_TIMEOUT)) {
      buzzerNyala();
      delay(2000);
      buzzerMati();
      palang.write(SUDUT_TUTUP);
      palangTerbuka = false;
      Serial.println("Tidak ada deteksi setelah 5 detik → Palang ditutup + Buzzer ON");
    } 
    else {
      // Jika palang sudah tertutup, jangan ada peringatan
      buzzerMati();
    }
  }

  // ---------------------------
  // Debug serial
  // ---------------------------
  Serial.print("Jarak A = ");
  Serial.print(jarakA);
  Serial.print(" cm | Jarak B = ");
  Serial.print(jarakB);
  Serial.print(" cm | Status Palang = ");
  Serial.println(palangTerbuka ? "TERBUKA" : "TERTUTUP");

  delay(200);
}
