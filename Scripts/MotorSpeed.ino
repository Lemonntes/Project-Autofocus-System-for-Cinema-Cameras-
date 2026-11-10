#include <Servo.h>

Servo motor;
const int SERVO_PIN = 9;

const int STOP_US = 1450;
const int ROTATE_US = 2000;
const unsigned long TEST_TIME = 2000;

void setup() {
  motor.attach(SERVO_PIN);
  motor.writeMicroseconds(STOP_US);
  delay(3000);
  motor.writeMicroseconds(ROTATE_US);
  delay(TEST_TIME);
  motor.writeMicroseconds(STOP_US);
}

void loop() {
  // Повторного вращения нет
}
