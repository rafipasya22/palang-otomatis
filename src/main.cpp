#include <Servo.h>
#include <Arduino.h>

#define TRIG1 9
#define ECHO1 10

#define TRIG2 6
#define ECHO2 7

// === Servo ===
Servo myServo;

long getDistance(int trigPin, int echoPin);

void setup() {
  Serial.begin(9600);

  pinMode(TRIG1, OUTPUT);
  pinMode(ECHO1, INPUT);

  pinMode(TRIG2, OUTPUT);
  pinMode(ECHO2, INPUT);

  myServo.attach(3);
  myServo.write(90);
}

void loop() {
  long distance1 = getDistance(TRIG1, ECHO1);
  long distance2 = getDistance(TRIG2, ECHO2);

  Serial.print("Sensor1: ");
  Serial.print(distance1);
  Serial.print(" cm | Sensor2: ");
  Serial.print(distance2);
  Serial.println(" cm");

  if (distance1 < 20) {
    myServo.write(0);
  }
  else if (distance2 < 20) {
    myServo.write(90);
  }

  delay(300);
}

  long getDistance(int trigPin, int echoPin) {
    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);

    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);

    long duration = pulseIn(echoPin, HIGH, 30000); // timeout 30ms
    long distance = duration * 0.034 / 2; // konversi ke cm
    return distance;
  }