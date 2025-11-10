#include <HCSR04.h>
#include <Servo.h>

// ----- PIN KONFIGURASI -----
#define TRIG_A 3
#define ECHO_A 4
#define TRIG_B 7
#define ECHO_B 6
#define PIN_SERVO 5
#define LED_KUNING 9   
#define LED_HIJAU 10
#define LED_MERAH 8

UltraSonicDistanceSensor sensorA(TRIG_A, ECHO_A);
UltraSonicDistanceSensor sensorB(TRIG_B, ECHO_B);
Servo palang;

const int SUDUT_BUKA   = 0;      
const int SUDUT_TUTUP  = 130;   
const float JARAK_DETEKSI_CM = 10.0; 
const unsigned long WAKTU_TIMEOUT = 5000;
const unsigned long BATAS_WAKTU_DETEKSI = 5000; 

// ----- VARIABEL -----
unsigned long waktuMulaiDeteksiA = 0;
unsigned long waktuMulaiDeteksiB = 0;
unsigned long waktuTerakhirA = 0;
unsigned long waktuTerakhirB = 0;
bool palangTerbuka = false;

void setup() {
  Serial.begin(9600);
  pinMode(LED_KUNING, OUTPUT);
  pinMode(LED_HIJAU, OUTPUT);
  pinMode(LED_MERAH, OUTPUT);

  digitalWrite(LED_KUNING, LOW);
  digitalWrite(LED_HIJAU, HIGH);
  digitalWrite(LED_MERAH, LOW);

  palang.attach(PIN_SERVO);
  palang.write(SUDUT_TUTUP);

  Serial.println("Sistem palang siap.");
}

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

    digitalWrite(LED_HIJAU, HIGH);
    digitalWrite(LED_MERAH, LOW);
    digitalWrite(LED_KUNING, LOW);
    palang.write(SUDUT_BUKA);
    palangTerbuka = true;
    waktuTerakhirA = sekarang;

    Serial.println("Kedua sensor aktif → Palang tetap terbuka");
  }

  // ===================================================
  // CASE 2: Hanya sensor A aktif → buka palang
  // ===================================================
  else if (deteksiA) {
    if (waktuMulaiDeteksiA == 0) waktuMulaiDeteksiA = sekarang;

    digitalWrite(LED_HIJAU, HIGH);
    digitalWrite(LED_MERAH, LOW);
    digitalWrite(LED_KUNING, LOW);
    palang.write(SUDUT_BUKA);
    palangTerbuka = true;
    waktuTerakhirA = sekarang;

    Serial.println("Sensor A aktif → Palang dibuka");
  }

  // ===================================================
  // CASE 3: Hanya sensor B aktif → lampu kuning 2 detik → tutup palang
  // ===================================================
  else if (deteksiB) {
    if (waktuMulaiDeteksiB == 0) waktuMulaiDeteksiB = sekarang;

    palang.write(SUDUT_TUTUP);
    palangTerbuka = false;
    digitalWrite(LED_MERAH, HIGH);
    digitalWrite(LED_HIJAU, LOW);
    digitalWrite(LED_KUNING, LOW);
    delay(5000);
    digitalWrite(LED_MERAH, LOW);
    digitalWrite(LED_HIJAU, LOW);
    digitalWrite(LED_KUNING, HIGH);
    Serial.println("Sensor B aktif → Lampu kuning nyala 2 detik");
    delay(3000);
    digitalWrite(LED_KUNING, LOW);


    digitalWrite(LED_HIJAU, HIGH);
    digitalWrite(LED_MERAH, LOW);

    waktuTerakhirB = sekarang;
    Serial.println("Palang ditutup → Lampu hijau nyala");
  }

  // ===================================================
  // CASE 4: Tidak ada deteksi sama sekali
  // ===================================================
  else {
    waktuMulaiDeteksiA = 0;
    waktuMulaiDeteksiB = 0;

    if (palangTerbuka && (sekarang - waktuTerakhirA > WAKTU_TIMEOUT)) {
      palang.write(SUDUT_TUTUP);
      palangTerbuka = false;
      
      digitalWrite(LED_HIJAU, LOW);
      digitalWrite(LED_MERAH, LOW);
      digitalWrite(LED_KUNING, HIGH);
      delay(2000);
      digitalWrite(LED_KUNING, LOW);
      digitalWrite(LED_HIJAU, HIGH);
      digitalWrite(LED_MERAH, LOW);
      Serial.println("Tidak ada deteksi 5 detik → Palang ditutup + Lampu kuning 2 detik");
    }
  }

  Serial.print("Jarak A = ");
  Serial.print(jarakA);
  Serial.print(" cm | Jarak B = ");
  Serial.print(jarakB);
  Serial.print(" cm | Status Palang = ");
  Serial.println(palangTerbuka ? "TERBUKA" : "TERTUTUP");

  delay(200);
}
