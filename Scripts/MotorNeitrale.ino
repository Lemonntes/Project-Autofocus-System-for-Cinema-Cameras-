#include <Servo.h>

Servo myServo;

void setup() {
  Serial.begin(115200);
  Serial.println("Введите число от 80 до 100:");
  myServo.attach(9);
}

void loop() {
  if (Serial.available() > 0) {
    int value = Serial.parseInt();
    if (value >= 0 && value <= 180) {
      myServo.write(value);
      Serial.print("Установлено: ");
      Serial.println(value);
    }
  }
}
