#include "RobbusZeego.h"

#ifndef _ROBBUSZEEGO_CPP_
#define _ROBBUSZEEGO_CPP_

// -------------------------------------------------------------
// Si tu cabecera antigua usa PWM_RESOUTION sin "L", crea alias
// -------------------------------------------------------------
#ifndef PWM_RESOLUTION
  #ifdef PWM_RESOUTION
    #define PWM_RESOLUTION PWM_RESOUTION
  #else
    #error "Define PWM_RESOLUTION en RobbusZeego.h antes de compilar"
  #endif
#endif

//------------- Definición única del strip Neopixel -------------
Adafruit_NeoPixel RobbusZeego::neop(1, 5 + NEO_KHZ800);

//----------------------------------------------------------------
//                             begin()
//----------------------------------------------------------------
void RobbusZeego::begin() {
  // --------------- Entradas de joystick ----------------
  pinMode(JOYSTICK_UP,     INPUT);
  pinMode(JOYSTICK_DOWN,   INPUT);
  pinMode(JOYSTICK_LEFT,   INPUT);
  pinMode(JOYSTICK_RIGHT,  INPUT);
  pinMode(JOYSTICK_ENTER,  INPUT);

  Wire.begin(23, 22);

  // --- Asigna pines a las estructuras de botones ---
  JoystickUp.pin     = JOYSTICK_UP;
  JoystickDown.pin   = JOYSTICK_DOWN;
  JoystickLeft.pin   = JOYSTICK_LEFT;
  JoystickRight.pin  = JOYSTICK_RIGHT;
  JoystickEnter.pin  = JOYSTICK_ENTER;

  // --------------- Salidas ----------------
  pinMode(BUZZER,        OUTPUT);
  pinMode(DCM_LEFT_IN1,  OUTPUT);
  pinMode(DCM_LEFT_IN2,  OUTPUT);
  pinMode(DCM_RIGHT_IN1, OUTPUT);
  pinMode(DCM_RIGHT_IN2, OUTPUT);
  pinMode(DCM_SLEEP,     OUTPUT);

  // --------- Configuración PWM con analogWrite ---------
  // Cada pin puede llevar su propia resolución y frecuencia
  const uint32_t FREQ = PWM_MOTOR_FREQ;

  analogWriteFrequency(DCM_LEFT_IN1,  FREQ);
  analogWriteResolution(DCM_LEFT_IN1, PWM_RESOLUTION);

  analogWriteFrequency(DCM_LEFT_IN2,  FREQ);
  analogWriteResolution(DCM_LEFT_IN2, PWM_RESOLUTION);

  analogWriteFrequency(DCM_RIGHT_IN1, FREQ);
  analogWriteResolution(DCM_RIGHT_IN1,PWM_RESOLUTION);

  analogWriteFrequency(DCM_RIGHT_IN2, FREQ);
  analogWriteResolution(DCM_RIGHT_IN2,PWM_RESOLUTION);

  // Buzzer: se ajusta sobre la marcha en playTone()
  analogWriteFrequency(BUZZER, 0);
  analogWriteResolution(BUZZER, PWM_RESOLUTION);
  analogWrite(BUZZER, 0);

  Move.enableMotors();

  // ------------------ Neopixel ------------------
  RobbusZeego::neop.begin();
  RobbusZeego::neop.show();        // LED apagado al inicio
}

/* --------------------------------------------------------------------------------
 *             IMPLEMENTACIÓN DE CLASES INTERNAS
 * --------------------------------------------------------------------------------
 */

// --------------------------- Buttons ---------------------------
uint8_t RobbusZeego::buttons::read() {
  new_state = digitalRead(pin);
  if (new_state == LOW  && old_state == LOW)  status = HOLD_PRESSED;
  if (new_state == LOW  && old_state == HIGH) status = PRESSED;
  if (new_state == HIGH && old_state == LOW)  status = RELEASED;
  if (new_state == HIGH && old_state == HIGH) status = HOLD_RELEASED;
  old_state = new_state;
  return status;
}

// --------------------------- LineSensor ---------------------------
void RobbusZeego::lineSensor::read() {
  sensor1 = analogRead(IR_SENSOR1);
  sensor2 = analogRead(IR_SENSOR2);
  sensor3 = analogRead(IR_SENSOR3);
  sensor4 = analogRead(IR_SENSOR4);
  sensor5 = analogRead(IR_SENSOR5);

  sensor1_oledMapped = map(sensor1, 0, 1000, 0, 64);
  sensor2_oledMapped = map(sensor2, 0, 1000, 0, 64);
  sensor3_oledMapped = map(sensor3, 0, 1000, 0, 64);
  sensor4_oledMapped = map(sensor4, 0, 1000, 0, 64);
  sensor5_oledMapped = map(sensor5, 0, 1000, 0, 64);
}

// --------------------------- Movement ---------------------------
void RobbusZeego::movement::enableMotors()  { digitalWrite(DCM_SLEEP, HIGH); }
void RobbusZeego::movement::disableMotors() { digitalWrite(DCM_SLEEP, LOW);  }

// Helper para fijar los cuatro pines con inversion duty/stop
static inline void _zeegoSetPWM(uint8_t in1, uint8_t in2, uint16_t duty) {
  analogWrite(in1, 255 - duty);  // canal activo
  analogWrite(in2, 255);         // canal inactivo
}

void RobbusZeego::movement::MotorLeft(int16_t vel) {
  vel = constrain(vel, -255, 255);
  direction = (vel > 0) ? FORWARD : (vel < 0) ? BACKWARD : STOP;
  speed     = abs(vel);

  switch (direction) {
    case FORWARD:  _zeegoSetPWM(DCM_LEFT_IN1,  DCM_LEFT_IN2,  speed); break;
    case BACKWARD: _zeegoSetPWM(DCM_LEFT_IN2,  DCM_LEFT_IN1,  speed); break;
    default:       stop();                                             break;
  }
}

void RobbusZeego::movement::MotorRight(int16_t vel) {
  vel = constrain(vel, -255, 255);
  direction = (vel > 0) ? FORWARD : (vel < 0) ? BACKWARD : STOP;
  speed     = abs(vel);

  switch (direction) {
    case FORWARD:  _zeegoSetPWM(DCM_RIGHT_IN1, DCM_RIGHT_IN2, speed); break;
    case BACKWARD: _zeegoSetPWM(DCM_RIGHT_IN2, DCM_RIGHT_IN1, speed); break;
    default:       stop();                                            break;
  }
}

void RobbusZeego::movement::forward(uint16_t vel) {
  vel = constrain(vel, 0u, 255u);
  speed = vel;
  if (speed) {
    _zeegoSetPWM(DCM_LEFT_IN1,  DCM_LEFT_IN2,  speed);
    _zeegoSetPWM(DCM_RIGHT_IN1, DCM_RIGHT_IN2, speed);
  } else stop();
}

void RobbusZeego::movement::backward(uint16_t vel) {
  vel = constrain(vel, 0u, 255u);
  speed = vel;
  if (speed) {
    _zeegoSetPWM(DCM_LEFT_IN2,  DCM_LEFT_IN1,  speed);
    _zeegoSetPWM(DCM_RIGHT_IN2, DCM_RIGHT_IN1, speed);
  } else stop();
}

void RobbusZeego::movement::turnLeft(uint16_t vel) {
  vel = constrain(vel, 0u, 255u);
  speed = vel;
  if (speed) {
    _zeegoSetPWM(DCM_RIGHT_IN1, DCM_RIGHT_IN2, speed); // rueda derecha avanza
    _zeegoSetPWM(DCM_LEFT_IN2,  DCM_LEFT_IN1,  speed); // rueda izquierda retrocede
  } else stop();
}

void RobbusZeego::movement::turnRight(uint16_t vel) {
  vel = constrain(vel, 0u, 255u);
  speed = vel;
  if (speed) {
    _zeegoSetPWM(DCM_LEFT_IN1,  DCM_LEFT_IN2,  speed); // rueda izquierda avanza
    _zeegoSetPWM(DCM_RIGHT_IN2, DCM_RIGHT_IN1, speed); // rueda derecha retrocede
  } else stop();
}

void RobbusZeego::movement::stop() {
  analogWrite(DCM_LEFT_IN1,  255);
  analogWrite(DCM_LEFT_IN2,  255);
  analogWrite(DCM_RIGHT_IN1, 255);
  analogWrite(DCM_RIGHT_IN2, 255);
}

// --------------------------- Buzzer ---------------------------
void RobbusZeego::Buzzer::playTone(int16_t frequency, uint16_t duration) {
  analogWriteFrequency(BUZZER, frequency);
  analogWrite(BUZZER, 128);           // 50 % duty
  delay(duration);
  analogWrite(BUZZER, 0);
}

void RobbusZeego::Buzzer::playTone(int16_t frequency) {
  if (old_frequency != frequency) {
    analogWriteFrequency(BUZZER, frequency);
    analogWrite(BUZZER, 128);
    old_frequency = frequency;
  }
}

void RobbusZeego::Buzzer::noTone() {
  analogWrite(BUZZER, 0);
  old_frequency = 0;
}

// --------------------------- Neopixel ---------------------------
// (funciones Neopixel sin cambios del original: color(), fadeInOut(), etc.)

#endif  // _ROBBUSZEEGO_CPP_
