#define ENC_A 2
#define ENC_B 3

#define IN1 4
#define IN2 5
#define ENA 9

#define TRIG 6
#define ECHO 7

#define ZERO_SPEED 70
#define SLOW_SPEED 70
#define FAST_SPEED 150

volatile long position = 0;


// Считаем импульсы энкодера и определяем направление вращения
void encoderISR()
{
    if (digitalRead(ENC_B))
        position++;
    else
        position--;
}


// двигатель вправо
void motorRight(int speed)
{
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);
    analogWrite(ENA, speed);
}


// двигатель влево
void motorLeft(int speed)
{
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, HIGH);
    analogWrite(ENA, speed);
}


// Остановка
void motorStop()
{
    analogWrite(ENA, 0);
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, LOW);
}


// Поиск нулевого положения объектива, каждый раз вначале
void findZero()
{
    long oldPosition = position;

    unsigned long lastMove = millis();
    unsigned long startTime = millis();

    motorLeft(ZERO_SPEED);

    while (true)
    {
        if (position != oldPosition)
        {
            oldPosition = position;
            lastMove = millis();
        }

        // Если энкодер 300 мс не меняется,  дошли до стопора
        if (millis() - lastMove > 300)
        {
            motorStop();
            position = 0;
            return;
        }

        // Защита (?)
        if (millis() - startTime > 5000)
        {
            motorStop();
            return;
        }
    }
}


// Измерение расстояния 
float getDistance()
{
    digitalWrite(TRIG, LOW);
    delayMicroseconds(2);

    digitalWrite(TRIG, HIGH);
    delayMicroseconds(10);
    digitalWrite(TRIG, LOW);

    long t = pulseIn(ECHO, HIGH, 30000);

    return t * 0.0343 / 2;
}


// Здесь будет функция, когда мы ее найдем
long focusFunction(float d)
{
    return 0;
}


// Поиск фокуса и перемещение обьектива
void moveTo(long target)
{
    long error = target - position;
    long difference = abs(error);

    // Если уже достаточно близко, то остановка
    if (difference <= 5)
    {
        motorStop();
        return;
    }

    int speed;

    // Далеко, поэтому быстро
    if (difference > 100)
        speed = FAST_SPEED;
    else
        speed = SLOW_SPEED;

    if (error > 0)
        motorRight(speed);
    else
        motorLeft(speed);
}


void setup()
{
    pinMode(ENC_A, INPUT);
    pinMode(ENC_B, INPUT);

    pinMode(IN1, OUTPUT);
    pinMode(IN2, OUTPUT);
    pinMode(ENA, OUTPUT);

    pinMode(TRIG, OUTPUT);
    pinMode(ECHO, INPUT);

    Serial.begin(9600);

    // Прерывание вызывается при каждом импульсе энкодера
    attachInterrupt(digitalPinToInterrupt(ENC_A), encoderISR, RISING);

    // сначала находим ноль всегда
    findZero();
}


void loop()
{
    float d = getDistance();

    // Прогоняем через функцию фокуса от расстояния, находим колво градусов, шагов
    long target = focusFunction(d);

    moveTo(target);

    Serial.print("Distance: ");
    Serial.print(d);
    Serial.print(" cm   Target: ");
    Serial.print(target);
    Serial.print("   Position: ");
    Serial.println(position);

    delay(50);
}
