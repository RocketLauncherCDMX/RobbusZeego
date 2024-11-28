#include <RobbusZeego.h>

#ifndef _ROBBUSZEEGO_CPP_
#define _ROBBUSZEEGO_CPP_

rmt_data_t led_data[24];
rmt_obj_t* rmt_send = NULL;

void RobbusZeego :: begin() {
  // IOs
  pinMode(JOYSTICK_UP, INPUT);
  pinMode(JOYSTICK_DOWN, INPUT);
  pinMode(JOYSTICK_LEFT, INPUT);
  pinMode(JOYSTICK_RIGHT, INPUT);
  pinMode(JOYSTICK_ENTER, INPUT);

  Wire.begin(23,22);

  // BUTTONS
  JoystickUp.pin = JOYSTICK_UP;
  JoystickDown.pin = JOYSTICK_DOWN;
  JoystickLeft.pin = JOYSTICK_LEFT;
  JoystickRight.pin = JOYSTICK_RIGHT;
  JoystickEnter.pin = JOYSTICK_ENTER;

  // ---------------------------------------------------------------
  // IOs
  pinMode(BUZZER, OUTPUT);
  pinMode(DCM_LEFT_IN1, OUTPUT);
  pinMode(DCM_LEFT_IN2, OUTPUT);
  pinMode(DCM_RIGHT_IN1, OUTPUT);
  pinMode(DCM_RIGHT_IN2, OUTPUT);
  pinMode(DCM_SLEEP, OUTPUT);

  ledcSetup(PWM_CHANNEL_LEFT_IN1, PWM_MOTOR_FREQ, PWM_RESOUTION);
  ledcAttachPin(DCM_LEFT_IN1, PWM_CHANNEL_LEFT_IN1);
  ledcSetup(PWM_CHANNEL_LEFT_IN2, PWM_MOTOR_FREQ, PWM_RESOUTION);
  ledcAttachPin(DCM_LEFT_IN2, PWM_CHANNEL_LEFT_IN2);

  ledcSetup(PWM_CHANNEL_RIGHT_IN1, PWM_MOTOR_FREQ, PWM_RESOUTION);
  ledcAttachPin(DCM_RIGHT_IN1, PWM_CHANNEL_RIGHT_IN1);
  ledcSetup(PWM_CHANNEL_RIGHT_IN2, PWM_MOTOR_FREQ, PWM_RESOUTION);
  ledcAttachPin(DCM_RIGHT_IN2, PWM_CHANNEL_RIGHT_IN2);

  ledcSetup(PWM_CHANNEL_BUZZER, 0, PWM_RESOUTION);
  ledcAttachPin(BUZZER, PWM_CHANNEL_BUZZER);
  ledcWrite(BUZZER, 128);

  Move.enableMotors();

   // NEOPIXEL INITIALIZATION
  rmt_send = rmtInit(5, true, RMT_MEM_64);
  rmtSetTick(rmt_send, 100);

  Neopixel.off();
  delay(1);

  Neopixel.off();
  delay(1);

  // ---------------------------------------------------------------------------

  
}

uint8_t RobbusZeego :: buttons :: read() {
  new_state = digitalRead(pin);
  if(new_state == LOW && old_state == LOW) status = HOLD_PRESSED;    // button remains released
  if(new_state == LOW && old_state == HIGH) status = PRESSED;          // button is pressed once
  if(new_state == HIGH && old_state == LOW) status = RELEASED;         // button is released once
  if(new_state == HIGH && old_state == HIGH) status = HOLD_RELEASED;     // button remains pressed
  old_state = new_state;                                               // the last state gets old
  return(status);
}

void RobbusZeego :: lineSensor :: read() {
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

void RobbusZeego :: lineSensor :: screenPlot() {
  /*display.clearDisplay();
  display.fillRect(4, 64-sensor1_oledMapped, 23, 64, WHITE);
  display.fillRect(29, 64-sensor2_oledMapped, 23, 64, WHITE);
  display.fillRect(54, 64-sensor3_oledMapped, 23, 64, WHITE);
  display.fillRect(79, 64-sensor4_oledMapped, 23, 64, WHITE);
  display.fillRect(104, 64-sensor5_oledMapped, 23, 64, WHITE);
  display.display();*/
}

void RobbusZeego :: lineSensor :: printValues() {
  if(sensor1 < 10) Serial.print(" ");
  if(sensor1 < 100) Serial.print(" ");
  if(sensor1 < 1000) Serial.print(" ");
  Serial.print(String(sensor1) + ",");
  if(sensor2 < 10) Serial.print(" ");
  if(sensor2 < 100) Serial.print(" ");
  if(sensor2 < 1000) Serial.print(" ");
  Serial.print(String(sensor2) + ",");
  if(sensor3 < 10) Serial.print(" ");
  if(sensor3 < 100) Serial.print(" ");
  if(sensor3 < 1000) Serial.print(" ");
  Serial.print(String(sensor3) + ",");
  if(sensor4 < 10) Serial.print(" ");
  if(sensor4 < 100) Serial.print(" ");
  if(sensor4 < 1000) Serial.print(" ");
  Serial.print(String(sensor4) + ",");
  if(sensor5 < 10) Serial.print(" ");
  if(sensor5 < 100) Serial.print(" ");
  if(sensor5 < 1000) Serial.print(" ");
  Serial.println(String(sensor5) + ",");
  
}

void RobbusZeego :: movement :: enableMotors() {
  digitalWrite(DCM_SLEEP, HIGH);
}

void RobbusZeego :: movement :: disableMotors() {
  digitalWrite(DCM_SLEEP, LOW);
}

void RobbusZeego :: movement :: MotorLeft(int16_t vel) {
  if(vel > 255) vel = 255;
  else if(vel < -255) vel = -255;
  if(vel > 0) direction = FORWARD;
  else if(vel < 0) direction = BACKWARD;
  else direction = STOP;
  speed = abs(vel);
  if(direction == FORWARD) {
    adjusted_leftSpeed = map(speed, 0, 255, 50, top_leftSpeed);
    //ledcWrite(PWM_CHANNEL_LEFT_IN1, 255 - adjusted_leftSpeed);

    ledcWrite(PWM_CHANNEL_LEFT_IN1, 255 - speed);
    ledcWrite(PWM_CHANNEL_LEFT_IN2, 255);
  }
  else if(direction == BACKWARD){
    ledcWrite(PWM_CHANNEL_LEFT_IN1, 255);
    ledcWrite(PWM_CHANNEL_LEFT_IN2, 255 - speed);
  }
  else stop();
}

void RobbusZeego :: movement :: MotorRight(int16_t vel) {
  if(vel > 255) vel = 255;
  else if(vel < -255) vel = -255;
  if(vel > 0) direction = FORWARD;
  else if(vel < 0) direction = BACKWARD;
  else direction = STOP;
  speed = abs(vel);
  if(direction == FORWARD) {
    adjusted_rightSpeed = map(speed, 0, 255, 50, top_rightSpeed);
    ledcWrite(PWM_CHANNEL_RIGHT_IN1, 255 - speed);
    ledcWrite(PWM_CHANNEL_RIGHT_IN2, 255);
  }
  else if(direction == BACKWARD){
    adjusted_rightSpeed = map(speed, 0, 255, 50, top_rightSpeed);
    ledcWrite(PWM_CHANNEL_RIGHT_IN1, 255);
    ledcWrite(PWM_CHANNEL_RIGHT_IN2, 255 - speed);
  }
  else stop();
}

void RobbusZeego :: movement :: forward(uint16_t vel) {
  if(vel > 255) vel = 255;
  else if(vel < 0) vel = 0;
  speed = vel;
  if(speed > 0) {
    adjusted_leftSpeed = map(speed, 0, 255, 50, top_leftSpeed);
    ledcWrite(PWM_CHANNEL_LEFT_IN1, 255 - speed);
    ledcWrite(PWM_CHANNEL_LEFT_IN2, 255);
    adjusted_rightSpeed = map(speed, 0, 255, 50, top_rightSpeed);
    ledcWrite(PWM_CHANNEL_RIGHT_IN1, 255 - speed);
    ledcWrite(PWM_CHANNEL_RIGHT_IN2, 255);
  }
  else stop();
}

void RobbusZeego :: movement :: backward(uint16_t vel) {
  if(vel > 255) vel = 255;
  else if(vel < 0) vel = 0;
  speed = vel;
  if(speed > 0) {
    adjusted_leftSpeed = map(speed, 0, 255, 50, top_leftSpeed);
    ledcWrite(PWM_CHANNEL_LEFT_IN1, 255);
    ledcWrite(PWM_CHANNEL_LEFT_IN2, 255 - speed);
    adjusted_rightSpeed = map(speed, 0, 255, 50, top_rightSpeed);
    ledcWrite(PWM_CHANNEL_RIGHT_IN1, 255);
    ledcWrite(PWM_CHANNEL_RIGHT_IN2, 255 - speed);
  }
  else stop();
}

void RobbusZeego :: movement :: turnLeft(uint16_t vel) {
  if(vel > 255) vel = 255;
  else if(vel < 0) vel = 0;
  speed = vel;
  if(speed > 0) {
    adjusted_rightSpeed = map(speed, 0, 255, 50, top_rightSpeed);
    ledcWrite(PWM_CHANNEL_RIGHT_IN1, 255 - speed);
    ledcWrite(PWM_CHANNEL_RIGHT_IN2, 255);
    adjusted_leftSpeed = map(speed, 0, 255, 50, top_leftSpeed);
    ledcWrite(PWM_CHANNEL_LEFT_IN1, 255);
    ledcWrite(PWM_CHANNEL_LEFT_IN2, 255 - speed);
  }
  else stop();
}

void RobbusZeego :: movement :: turnRight(uint16_t vel) {
  if(vel > 255) vel = 255;
  else if(vel < 0) vel = 0;
  speed = vel;
  if(speed > 0) {
    adjusted_leftSpeed = map(speed, 0, 255, 50, top_leftSpeed);
    ledcWrite(PWM_CHANNEL_LEFT_IN1, 255 - speed);
    ledcWrite(PWM_CHANNEL_LEFT_IN2, 255);
    adjusted_rightSpeed = map(speed, 0, 255, 50, top_rightSpeed);
    ledcWrite(PWM_CHANNEL_RIGHT_IN1, 255);
    ledcWrite(PWM_CHANNEL_RIGHT_IN2, 255 - speed);
  }
  else stop();
}

void RobbusZeego :: movement :: stop() {
  ledcWrite(PWM_CHANNEL_RIGHT_IN1, 255);
  ledcWrite(PWM_CHANNEL_RIGHT_IN2, 255);
  ledcWrite(PWM_CHANNEL_LEFT_IN1, 255);
  ledcWrite(PWM_CHANNEL_LEFT_IN2, 255);
}

void RobbusZeego :: Buzzer :: playTone(int16_t frequency, uint16_t duration) {
  ledcSetup(PWM_CHANNEL_BUZZER, frequency, PWM_RESOUTION);
  ledcAttachPin(BUZZER, PWM_CHANNEL_BUZZER);
  ledcWrite(PWM_CHANNEL_BUZZER, 128);
  delay(duration);
  ledcWrite(PWM_CHANNEL_BUZZER, 0);
}

void RobbusZeego :: Buzzer :: playTone(int16_t frequency) {
  if(old_frequency != frequency) {
    ledcSetup(PWM_CHANNEL_BUZZER, frequency, PWM_RESOUTION);
    ledcAttachPin(BUZZER, PWM_CHANNEL_BUZZER);
    ledcWrite(PWM_CHANNEL_BUZZER, 128);
  }
  old_frequency = frequency;
}


void RobbusZeego :: Buzzer :: noTone() {
  ledcSetup(PWM_CHANNEL_BUZZER, 0, PWM_RESOUTION);
  ledcAttachPin(BUZZER, PWM_CHANNEL_BUZZER);
  ledcWrite(PWM_CHANNEL_BUZZER, 0);
  old_frequency = 0;
}

void RobbusZeego :: Buzzer :: playNote(char note, uint16_t duration) {
  
}

void RobbusZeego :: Buzzer :: r2d2(uint16_t time) {
  for(int i=0; i<time/75; i++) {
    playTone(random(50, 5000), 75);
  }
  noTone();
}

void RobbusZeego :: Neopixel :: color(uint8_t red, uint8_t green, uint8_t blue) {
  value[0] = green;
  value[1] = red;
  value[2] = blue;
  i=0;

  for (col=0; col<3; col++ ) {
    for (bit=0; bit<8; bit++){
      if ( (value[col] & (1<<(7-bit)))) {
        led_data[i].level0 = 1;
        led_data[i].duration0 = 8;
        led_data[i].level1 = 0;
        led_data[i].duration1 = 4;
      } else {
        led_data[i].level0 = 1;
        led_data[i].duration0 = 4;
        led_data[i].level1 = 0;
        led_data[i].duration1 = 8;
      }
      i++;
    }
  }
  // Send the data
  rmtWrite(rmt_send, led_data, 24);
}

void RobbusZeego :: Neopixel :: color(uint8_t colorName) {
  switch(colorName) {
    case OFF:       color(0,0,0);        break;
    case RED:       color(255,0,0);      break;
    case GREEN:     color(0,255,0);      break;
    case BLUE:      color(0,0,255);      break;
    case YELLOW:    color(255,255,0);    break;
    case CYAN:      color(0,255,255);    break;
    case MAGENTA:   color(255,0,255);    break;
    case WHITE:     color(255,255,255);  break;
  }
}

void RobbusZeego :: Neopixel :: color(uint8_t colorName, uint8_t brightness) {
  if(brightness < 1) brightness = 1;
  else if(brightness > 255) brightness = 255;
  switch(colorName) {
    case BLACK:   off();                                     break;
    case RED:     color(brightness, 0, 0);                   break;
    case GREEN:   color(0, brightness, 0);                   break;
    case BLUE:    color(0, 0, brightness);                   break;
    case YELLOW:  color(brightness, brightness, 0);          break;
    case CYAN:    color(0, brightness, brightness);          break;
    case MAGENTA: color(brightness, 0, brightness);          break;
    case WHITE:   color(brightness, brightness, brightness); break;
  }
}

void RobbusZeego :: Neopixel :: fadeInOut(uint8_t colorName, uint16_t speed) {
  for(int i=0; i<=255; i++) {
    switch(colorName) {
      case RED:     color(i,0,0); break;
      case GREEN:   color(0,i,0); break;
      case BLUE:    color(0,0,i); break;
      case YELLOW:  color(i,i,0); break;
      case CYAN:    color(0,i,i); break;
      case MAGENTA: color(i,0,i); break;
      case WHITE:   color(i,i,i); break;
    }
    delayMicroseconds(speed);
  }
  for(int i=255; i>=0; i--) {
    switch(colorName) {
      case RED:     color(i,0,0); break;
      case GREEN:   color(0,i,0); break;
      case BLUE:    color(0,0,i); break;
      case YELLOW:  color(i,i,0); break;
      case CYAN:    color(0,i,i); break;
      case MAGENTA: color(i,0,i); break;
      case WHITE:   color(i,i,i); break;
    }
    delayMicroseconds(speed);
  }
}

void RobbusZeego :: Neopixel :: fadeInOut(uint8_t colorName) {
  for(int i=0; i<=255; i++) {
    switch(colorName) {
      case RED:     color(i,0,0); break;
      case GREEN:   color(0,i,0); break;
      case BLUE:    color(0,0,i); break;
      case YELLOW:  color(i,i,0); break;
      case CYAN:    color(0,i,i); break;
      case MAGENTA: color(i,0,i); break;
      case WHITE:   color(i,i,i); break;
    }
    delay(1);
  }
  for(int i=255; i>=0; i--) {
    switch(colorName) {
      case RED:     color(i,0,0); break;
      case GREEN:   color(0,i,0); break;
      case BLUE:    color(0,0,i); break;
      case YELLOW:  color(i,i,0); break;
      case CYAN:    color(0,i,i); break;
      case MAGENTA: color(i,0,i); break;
      case WHITE:   color(i,i,i); break;
    }
    delay(1);
  }
}

void RobbusZeego :: Neopixel :: heartBeat(uint8_t colorName) {
  fadeInOut(colorName, 750);
  delay(100);
  fadeInOut(colorName, 750);
}

void RobbusZeego :: Neopixel :: heartBeat(uint8_t colorName, uint8_t brightness) {
  if(brightness >= 255) brightness = 254;
  else if(brightness < 10) brightness = 10;
  for(uint8_t j=0; j<2; j++) {
    for(uint8_t i=0; i<=brightness; i++) {
      switch(colorName) {
        case RED:     color(i,0,0); break;
        case GREEN:   color(0,i,0); break;
        case BLUE:    color(0,0,i); break;
        case YELLOW:  color(i,i,0); break;
        case CYAN:    color(0,i,i); break;
        case MAGENTA: color(i,0,i); break;
        case WHITE:   color(i,i,i); break;
      }
      delay((uint8_t)255/brightness);
    }
    for(int i=brightness; i>=0; i--) {
      switch(colorName) {
        case RED:     color(i,0,0); break;
        case GREEN:   color(0,i,0); break;
        case BLUE:    color(0,0,i); break;
        case YELLOW:  color(i,i,0); break;
        case CYAN:    color(0,i,i); break;
        case MAGENTA: color(i,0,i); break;
        case WHITE:   color(i,i,i); break;
      }
      delay((uint8_t)255/brightness);
    }
    delay(100);
  }
}

void RobbusZeego :: Neopixel :: off() {
  color(0,0,0);
}

#endif