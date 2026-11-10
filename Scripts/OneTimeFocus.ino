#include <Servo.h>
#include <math.h>

Servo motor;

const int SERVO_PIN = 9;
const int STOP_US = 1450;
const int ROTATE_US = 1000;
const float SPEED_DEG_S = 450.5;

const int MIN_DISTANCE = 38;
const int MAX_DISTANCE = 800;

bool readDistance(int &distance) {
  byte data[9];
  byte index = 0;

  unsigned long start = millis();

  while (millis() - start < 3000) {
    if (Serial.available() == 0) continue;

    byte b = Serial.read();

    if (index == 0) {
      if (b != 0x59) continue;
    } else if (index == 1) {
      if (b != 0x59) {
        index = 0;
        continue;
      }
    }

    data[index++] = b;

    if (index == 9) {
      index = 0;

      byte checksum = 0;
      for (int i = 0; i < 8; i++) {
        checksum += data[i];
      }

      if (checksum != data[8]) continue;

      int d = data[2] | (data[3] << 8);
      int strength = data[4] | (data[5] << 8);

      if (strength < 100 ||
          strength == 65535) continue;

      if (d < MIN_DISTANCE ||
          d > MAX_DISTANCE) continue;

      distance = d;
      return true;
    }
  }

  return false;
}

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);

  motor.attach(SERVO_PIN);
  motor.writeMicroseconds(STOP_US);

  // UART TF-Luna
  Serial.begin(115200);

  delay(2000);

  int distance;

  if (!readDistance(distance)) {
    return;
  }

  // Формула угла объектива
  float phi1 =
    383.01 * (1.0 - exp(-0.0262 * distance))
    - 205.37;

  float phi2 = phi1 / 0.296;

  if (phi2 <= 0) return;

  unsigned long rotationTime =
    (unsigned long)(
      phi2 / SPEED_DEG_S * 1000.0
    );

  motor.writeMicroseconds(ROTATE_US);
  delay(rotationTime);
  motor.writeMicroseconds(STOP_US);
  digitalWrite(LED_BUILTIN, HIGH);
}

void loop() {
  // Нет повторных поворотов
}
