// ------------------------------------------------ ROBBUS ZEEGO ------------------------------------------------
//
// EJEMPLO DE CONTROL DE ROBBUS KIDSY DESDE EL SMARTPHONE POR MEDIO DE LA APLICACION DABBLE
// Este ejemplo es de uso libre y esta pensado para dar una introduccion al hardware de Robbus Zeego.
// Autor: Rocket Launcher
// Fecha: 12 de diciembre de 2021
// Visita www.robbus.mx/zeego para mas informacion
// --------------------------------------------------------------------------------------------------------------
//
//   Este ejemplo crea una conexion basica entre Robbus Zeego y un telefono, por bluetooth 4.2.
//   El presente ejemplo funciona hasta el dia 23/10/2024 con la aplicacion Dabble para telefonos Android. 
//   Android:   https://play.google.com/store/apps/details?id=io.dabbleapp&hl=es_MX
//   iOS:       https://apps.apple.com/mx/app/dabble-bluetooth-controller/id1472734455
//
//   NOTA: Esta aplicacion es de un tercero y su uso puede variar, no obstante, cualquier cambio se vera reflejado
//         en futuras actualizaciones. Rocket Launcher trabajara arduamente para tener su aplicacion propia en un
//         futuro cercano. Apoyanos comprando nuestros productos :)
//   
//   Pasos para probar este ejemplo:
//
//   1.- Compilar y subir el ejemplo a tu Robbus Zeego.
//   2.- Serciorarse que tu Robbus Zeego tenga el interruptor en posicion ON.
//   3.- Instalar la aplicacion Dabble (enlace arriba).
//   4.- Abrir la aplicacion. En la pantalla principal encontraras diversas funciones, selecciona Gamepad.
//   5.- En la pantalla del Gamepad, en la parte superior derecha encontraras 2 iconos, el primero es para
//       conectar a tu dispositivo, el segundo es para cambiar de tipo de gamepad. Toca el primero (Conectar).
//   6.- La siguiente pantalla es Seleccione el Dispositivo, donde, si Zeego ya tiene el programa y se encuentra
//       encendido, debera aparecer en esta lista. Seleccionalo. Si ya tiene el programa, esta encendido, y aun
//       asi no aparece en la lista, presiona el boton RESET de Zeego o vuelve a seleccionar Conectar en la app.
//   7.- Una vez conectado, ya podras manejar a Zeego con las flechas y botones. Tambien podras ocupar el pad
//       analogo o el acelerometro, tocando el icono de la derecha.
//   8.- Puedes ver la funcionalidad de cada boton, por medio del puerto Serial de Arduino. (115200 bauds).
// ------------------------------------------------------------------------------------------------------------------

#define CUSTOM_SETTINGS
#define INCLUDE_GAMEPAD_MODULE
#include <DabbleESP32.h>
#include <RobbusZeego.h>
#include "ZeegoDabbleJoystick.h"

RobbusZeego Zeego;    // Llama a Robbus Zeego

char ble_name[20] = "ROBBUS ZEEGO";     // Este es el nombre del dispositivo para su conexion

bool flag_direction_buttons = false;  // bandera para saber si algun boton de direccion se pulso o no

int max_speed_left = 100;   // velocidad llanta izquierda
int max_speed_right = 100;  // velocidad llanta derecha

void setup() {
  Serial.begin(115200);           // Asegurate de que tu Monitor Serial este configurado con esta velocidad.
  Dabble.begin(ble_name);         // Ingresa la velocidad del bluetooth. Conecta bluetooth en el puerto correspondiente en evive.
  Zeego.begin();                  // Inicializa el hardware del Robbus Zeego
  Zeego.Move.enableMotors();      // Habilita los motores
}

void loop() {
  Dabble.processInput();          // Esta funcion se usa para refrescar los datos obtenidos del smartphone. Es obligatoria para recibir datos correctamente.
  buttonsProcess();               // Procesa los botones. Esta funcion no es parte de la biblioteca, solo es una funcion de apoyo para compatibilidad entre Dabble y Zeego
  Zeego.LineSensor.read();        // Lee el valor de los 5 sensores de linea
  Zeego.LineSensor.screenPlot();  // Muestra la lectura de los sensores con barras en la pantalla del Zeego
  
  int new_angle = GamePad.getAngle();
  int b = GamePad.getRadius();
  
  // Procesa las flechas en la aplicacion Dabble
  // --------------------------------------------------------------------------------
  if(button_triangle == PRESSED) {
    Serial.println("Triangle");
    Zeego.Neopixel.color(MAGENTA);
    Zeego.Buzzer.playTone(500, 50);
  }
  if(button_cross == PRESSED) {
    Serial.println("Cross");
    Zeego.Neopixel.color(GREEN);
    Zeego.Buzzer.playTone(550, 50);
  }
  if(button_square == PRESSED) {
    Serial.println("Square");
    Zeego.Neopixel.color(YELLOW);
    Zeego.Buzzer.playTone(600, 50);
  }
  if(button_circle == PRESSED) {
    Serial.println("Circle");
    Zeego.Neopixel.color(BLUE);
    Zeego.Buzzer.playTone(650, 50);
  }
  if(button_select == PRESSED) {
    Serial.println("Select");
    Zeego.Neopixel.color(OFF);
  }
  if(button_start == PRESSED) {
    Serial.println("Start");
    Zeego.Neopixel.color(RED);
  }
  // --------------------------------------------------------------------------------

  // Procesa los botones de direccion o el joystick (no ambos)
  // --------------------------------------------------------------------------------
  // Aqui se evaluan los diferentes casos para mover el Zeego en funcion de los botones y el joystick.
  // Se prioriza el uso de los botones si estan presionados, desactivando los motores cuando se sueltan.
  // Si no hay botones activos, se utiliza el joystick para controlar direccion y velocidad.
  // --------------------------------------------------------------------------------
  if(button_forward == PRESSED) {
    Serial.println("Up");
    flag_direction_buttons = true;
    Zeego.Move.MotorLeft(max_speed_left);
    Zeego.Move.MotorRight(max_speed_right);
  }
  else if(button_forward == RELEASED) {
    flag_direction_buttons = false;
    Zeego.Move.MotorLeft(0);
    Zeego.Move.MotorRight(0);
  }
  else if(button_backward == PRESSED) {
    Serial.println("Down");
    flag_direction_buttons = true;
    Zeego.Move.MotorLeft(-max_speed_left);
    Zeego.Move.MotorRight(-max_speed_right);
  }
  else if(button_backward == RELEASED) {
    flag_direction_buttons = false;
    Zeego.Move.MotorLeft(0);
    Zeego.Move.MotorRight(0);
  }
  else if(button_left == PRESSED) {
    Serial.println("Left");
    flag_direction_buttons = true;
    Zeego.Move.MotorLeft(-max_speed_left);
    Zeego.Move.MotorRight(max_speed_right);
  }
  else if(button_left == RELEASED) {
    flag_direction_buttons = false;
    Zeego.Move.MotorLeft(0);
    Zeego.Move.MotorRight(0);
  }
  else if(button_right == PRESSED) {
    Serial.println("Right");
    flag_direction_buttons = true;
    Zeego.Move.MotorLeft(max_speed_left);
    Zeego.Move.MotorRight(-max_speed_right);
  }
  else if(button_right == RELEASED) {
    flag_direction_buttons = false;
    Zeego.Move.MotorLeft(0);
    Zeego.Move.MotorRight(0);
  }
  else if(b != 0 && new_angle != 0) {
    if(new_angle != old_angle) {
      Serial.print("Angulo: " + String(new_angle) + "°, Radio: " + String(b));
      if(new_angle >= 60 && new_angle < 120) {
        Serial.println(" - Frente");
        Zeego.Move.MotorLeft(max_speed_left/SPEED_STEP_NUMBER*b);
        Zeego.Move.MotorRight(max_speed_right/SPEED_STEP_NUMBER*b);
      }
      else if(new_angle >= 120 && new_angle < 150) {
        Serial.println(" - Vuelta ligera a la Izquierda");
        Zeego.Move.MotorLeft(max_speed_left/(SPEED_STEP_NUMBER*2)*b);
        Zeego.Move.MotorRight(max_speed_right/SPEED_STEP_NUMBER*b);
      }
      else if(new_angle <= 60 && new_angle > 30) {
        Serial.println(" - Vuelta ligera a la Derecha");
        Zeego.Move.MotorLeft(max_speed_left/SPEED_STEP_NUMBER*b);
        Zeego.Move.MotorRight(max_speed_right/(SPEED_STEP_NUMBER*2)*b);
      }
      else if(new_angle >= 150 && new_angle < 210) {
        Serial.println(" - Vuelta pronunciada a la Izquierda");
        Zeego.Move.MotorLeft(-max_speed_left/SPEED_STEP_NUMBER*b);
        Zeego.Move.MotorRight(max_speed_right/SPEED_STEP_NUMBER*b);
      }
      else if(new_angle <= 30 || new_angle > 330) {
        Serial.println(" - Vuelta pronunciada a la Derecha");
        Zeego.Move.MotorLeft(max_speed_left/SPEED_STEP_NUMBER*b);
        Zeego.Move.MotorRight(-max_speed_right/SPEED_STEP_NUMBER*b);
      }
      else if(new_angle >= 210 && new_angle < 240) {
        Serial.println(" - Reversa a la Izquierda");
        Zeego.Move.MotorLeft(-max_speed_left/(SPEED_STEP_NUMBER*2)*b);
        Zeego.Move.MotorRight(-max_speed_right/SPEED_STEP_NUMBER*b);
      }
      else if(new_angle <= 330 && new_angle > 300) {
        Serial.println(" - Reversa a la Derecha");
        Zeego.Move.MotorLeft(-max_speed_left/SPEED_STEP_NUMBER*b);
        Zeego.Move.MotorRight(-max_speed_right/(SPEED_STEP_NUMBER*2)*b);
      }
      else if(new_angle >= 240 && new_angle < 300) {
        Serial.println(" - Reversa");
        Zeego.Move.MotorLeft(-max_speed_left/SPEED_STEP_NUMBER*b);
        Zeego.Move.MotorRight(-max_speed_right/SPEED_STEP_NUMBER*b);
      }
    }  
    else {
      Zeego.Move.MotorLeft(0);
      Zeego.Move.MotorRight(0);
    }
  }
}

