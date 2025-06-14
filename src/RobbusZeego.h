#ifndef _ROBBUSZEEGO_H_
#define _ROBBUSZEEGO_H_

#include <Arduino.h>
#include <Wire.h>
#include <pitches.h>
#include <Adafruit_NeoPixel.h>


// -------------------------------------------------------------------------------------------------------------
// RobbusZeego Classes
class RobbusZeego {
public:
  //------------- Neopixel strip (estático, accesible como RobbusZeego::neop) -------------
  static Adafruit_NeoPixel neop;                 // 1 LED en el pin 5, modo por defecto

private:
  #define SCREEN_ADDRESS        0x3C
  #define PWM_MOTOR_FREQ        30000   // this variable is used to define the time period 
  #define PWM_RESOLUTION         8       // this will define the resolution of the signal which is 8 in this case

public:
  void begin();
  #define FORWARD       1     
  #define BACKWARD      2
  #define LEFT          3
  #define RIGHT         4
  #define ON            1
  #define OFF           0
  #define NONE          0

  #define BLACK         0
  #define RED           1
  #define GREEN         2
  #define BLUE          3
  #define YELLOW        4
  #define CYAN          5
  #define MAGENTA       6
  #define WHITE         7

  class buttons {
  private:
    bool new_state = HIGH;
    bool old_state = HIGH;
  public:
    #define PRESSED           1
    #define RELEASED          0
    #define HOLD_RELEASED     2
    #define HOLD_PRESSED      3
    
    // Version 1.4
    #define JOYSTICK_UP       21
    #define JOYSTICK_DOWN     26
    #define JOYSTICK_LEFT     27
    #define JOYSTICK_RIGHT    32
    #define JOYSTICK_ENTER    19
    
    uint8_t pin;      
    uint8_t status;
    uint8_t read();
    
  } JoystickUp, JoystickDown, JoystickLeft, JoystickRight, JoystickEnter;

  class lineSensor {
  private:
    uint8_t sensor1_oledMapped;
    uint8_t sensor2_oledMapped;
    uint8_t sensor3_oledMapped;
    uint8_t sensor4_oledMapped;
    uint8_t sensor5_oledMapped;

  public:
    uint16_t sensor1;
    uint16_t sensor2;
    uint16_t sensor3;
    uint16_t sensor4;
    uint16_t sensor5;
    #define IR_SENSOR1              36
    #define IR_SENSOR2              39
    #define IR_SENSOR3              34
    #define IR_SENSOR4              35
    #define IR_SENSOR5              33
    
    void read();
    void screenPlot();
    void printValues();

  } LineSensor;

  class movement {
  private:
    #define DCM_LEFT_IN1            18
    #define DCM_LEFT_IN2            17
    #define DCM_RIGHT_IN1           4
    #define DCM_RIGHT_IN2           2
    #define DCM_SLEEP               14
    
    #define PWM_CHANNEL_LEFT_IN1    0
    #define PWM_CHANNEL_LEFT_IN2    1
    #define PWM_CHANNEL_RIGHT_IN1   2
    #define PWM_CHANNEL_RIGHT_IN2   3

    uint8_t adjusted_leftSpeed = 150;
    uint8_t adjusted_rightSpeed = 150;
    uint8_t speed = 0;
    uint8_t direction;

  public:
    #define STOP   0
    uint8_t top_leftSpeed = 255;
    uint8_t top_rightSpeed = 255;

    void disableMotors();
    void enableMotors();
    void MotorLeft(int16_t);
    void MotorRight(int16_t);
    void forward(uint16_t);
    void backward(uint16_t);
    void turnLeft(uint16_t);
    void turnRight(uint16_t);
    void stop();

  } Move;

  class Buzzer {
  private:
    uint16_t old_frequency = 0;
  public:
    #define BUZZER  25
    #define PWM_CHANNEL_BUZZER  4

    void playTone(int16_t, uint16_t);
    void playTone(int16_t);
    void playTone(int16_t, uint16_t, uint8_t);
    void noTone();
    void playNote(char, uint16_t);
    void r2d2(uint16_t);
  } Buzzer;

  class Neopixel {
  private:
    uint8_t value[3];  // GRB value
    uint8_t col, bit;
    uint8_t i;
  public:
    int tempo;

    void color(uint8_t,uint8_t,uint8_t);
    void color(uint8_t,uint8_t);
    void color(uint8_t);
    void fadeInOut(uint8_t, uint16_t);
    void fadeInOut(uint8_t);
    void heartBeat(uint8_t);
    void heartBeat(uint8_t, uint8_t);
    void off(void);

  } Neopixel;
};

#endif  // _ROBBUSZEEGO_H_
