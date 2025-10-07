#include <HCSR04.h>
#include <Servo.h>

// ----- PIN KONFIGURASI -----
#define TRIG_A 2
#define ECHO_A 12
#define TRIG_B 4
#define ECHO_B 13
#define PIN_SERVO 9
#define PIN_BUZZER 8
#define LED_HIJAU 6
#define LED_MERAH 7

// ----- OBJEK SENSOR DAN SERVO -----
UltraSonicDistanceSensor sensorA(TRIG_A, ECHO_A);
UltraSonicDistanceSensor sensorB(TRIG_B, ECHO_B);
Servo palang;

// ----- KONSTANTA -----
const int SUDUT_BUKA   = 0;      // Palang buka
const int SUDUT_TUTUP  = 140;    // Palang tutup
const float JARAK_DETEKSI_CM = 50.0; // Batas deteksi (cm)
const unsigned long WAKTU_TIMEOUT = 5000; // 5 detik tanpa deteksi
const unsigned long BATAS_WAKTU_DETEKSI = 5000; // 5 detik untuk peringatan buzzer

// ----- VARIABEL -----
unsigned long waktuMulaiDeteksiA = 0;
unsigned long waktuMulaiDeteksiB = 0;
unsigned long waktuTerakhirA = 0;
unsigned long waktuTerakhirB = 0;
bool buzzerAktif = false;
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

// ----- FUNGSI LED -----
void updateLED() {
  if (palangTerbuka) {
    digitalWrite(LED_HIJAU, HIGH);
    digitalWrite(LED_MERAH, LOW);
  } else {
    digitalWrite(LED_HIJAU, LOW);
    digitalWrite(LED_MERAH, HIGH);
  }
}

// ----- SETUP -----
void setup() {
  Serial.begin(9600);
  pinMode(PIN_BUZZER, OUTPUT);
  pinMode(LED_HIJAU, OUTPUT);
  pinMode(LED_MERAH, OUTPUT);
  digitalWrite(PIN_BUZZER, LOW);
  digitalWrite(LED_HIJAU, LOW);
  digitalWrite(LED_MERAH, HIGH); // awalnya palang tertutup → merah ON

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

  // ===================================================
  // CASE 1: Kedua sensor aktif → palang tetap terbuka
  // ===================================================
  if (deteksiA && deteksiB) {
    if (waktuMulaiDeteksiA == 0) waktuMulaiDeteksiA = sekarang;

    palang.write(SUDUT_BUKA);
    palangTerbuka = true;
    waktuTerakhirA = sekarang;
    Serial.println("Kedua sensor aktif → Palang tetap terbuka");

    // Jika kedua sensor aktif lebih dari 5 detik → buzzer ON
    if (sekarang - waktuMulaiDeteksiA >= BATAS_WAKTU_DETEKSI) {
      buzzerNyala();
      Serial.println("Kedua sensor aktif > 5 detik → Buzzer ON (palang tetap terbuka)");
    } else {
      buzzerMati();
    }
  }

  // ===================================================
  // CASE 2: Hanya sensor A aktif → buka palang
  // ===================================================
  else if (deteksiA) {
    if (waktuMulaiDeteksiA == 0) waktuMulaiDeteksiA = sekarang;

    palang.write(SUDUT_BUKA);
    palangTerbuka = true;
    waktuTerakhirA = sekarang;
    Serial.println("Sensor A aktif → Palang dibuka");

    // Jika sensor A masih aktif setelah 5 detik → buzzer ON
    if (sekarang - waktuMulaiDeteksiA >= BATAS_WAKTU_DETEKSI) {
      buzzerNyala();
      Serial.println("Sensor A masih aktif > 5 detik → Buzzer ON");
    } else {
      buzzerMati();
    }
  }

  // ===================================================
  // CASE 3: Hanya sensor B aktif → tutup palang
  // ===================================================
  else if (deteksiB) {
    if (waktuMulaiDeteksiB == 0) waktuMulaiDeteksiB = sekarang;

    palang.write(SUDUT_TUTUP);
    palangTerbuka = false;
    waktuTerakhirB = sekarang;
    Serial.println("Sensor B aktif → Palang ditutup");

    // Jika sensor B aktif lebih dari 5 detik → buzzer ON
    if (sekarang - waktuMulaiDeteksiB >= BATAS_WAKTU_DETEKSI) {
      buzzerNyala();
      Serial.println("Sensor B aktif > 5 detik → Buzzer ON");
    } else {
      buzzerMati();
    }
  }

  // ===================================================
  // CASE 4: Tidak ada deteksi sama sekali
  // ===================================================
  else {
    waktuMulaiDeteksiA = 0;
    waktuMulaiDeteksiB = 0;

    if (palangTerbuka && (sekarang - waktuTerakhirA > WAKTU_TIMEOUT)) {
      buzzerNyala();
      delay(2000);
      buzzerMati();
      palang.write(SUDUT_TUTUP);
      palangTerbuka = false;
      Serial.println("Tidak ada deteksi selama 5 detik → Palang ditutup + Buzzer ON");
    } else {
      buzzerMati();
    }
  }

  // ===================================================
  // Update LED sesuai status palang
  // ===================================================
  updateLED();

  // ===================================================
  // Debug Serial
  // ===================================================
  Serial.print("Jarak A = ");
  Serial.print(jarakA);
  Serial.print(" cm | Jarak B = ");
  Serial.print(jarakB);
  Serial.print(" cm | Status Palang = ");
  Serial.println(palangTerbuka ? "TERBUKA" : "TERTUTUP");

  delay(200);
}
