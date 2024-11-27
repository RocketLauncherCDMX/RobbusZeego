// ------------------------------------------------ ROBBUS Zeego ----------------------------------------------
//
// EJEMPLO DE USO DE LOS BOTONES DE USUARIO A, B y C CON PULSACION LARGA Y MANTENIDA
// Este ejemplo es de uso libre y esta pensado para dar una introduccion al hardware de Robbus Zeego.
// Autor: Rocket Launcher
// Fecha: 23 de noviembre de 2024
// Visita www.robbus.mx/Zeego para mas informacion
// ------------------------------------------------------------------------------------------------------------
// Robbus Zeego cuenta con un joystick de 5 posiciones:
// - JoystickUp
// - JoystickDown
// - JoystickLeft
// - JoystickRight
// - JoystickEnter
//
// Con este ejemplo entenderas los diferentes usos que se le pueden dar a los botones de tu Robot.
// Todos los botones del Joystick pueden regresar 4 estados diferentes:
//
// 0 - RELEASED       - El boton paso de estar presionado a liberarse
// 1 - PRESSED        - El boton paso de estar en en reposo a presionarse
// 2 - HOLD_PRESSED   - Se mantiene presionado
// 3 - HOLD_RELEASED  - Se mantiene sin preisonar
//
// Nota importante: El boton RESET sirve para reiniciar el programa de tu robot y no se puede programar
// ------------------------------------------------------------------------------------------------------------

#include<RobbusZeego.h>

RobbusZeego Zeego;        // Llama a Robbus Zeego

int times_up;
int times_down;
int times_left;
int times_right;
int times_enter;              // Cuenta las pulsaciones de los botones

void setup() {
  Serial.begin(115200);   // Inicializa comunicacion serial a 115200 baudios
  Zeego.begin();          // Inicializa el hardware del Robbus Zeego
}

void loop() {
  Zeego.JoystickUp.read();   // Lee el ultimo estado del boton Up
  Zeego.JoystickDown.read();   // Lee el ultimo estado del boton Down
  Zeego.JoystickLeft.read();   // Lee el ultimo estado del boton Left
  Zeego.JoystickRight.read();   // Lee el ultimo estado del boton Right
  Zeego.JoystickEnter.read();   // Lee el ultimo estado del boton Enter

  // Para que el codigo permanezca corto, solo se leera el Joystick Up, pero tu puedes
  // agregar los otros 4 botones siguiente la misma logica
  switch(Zeego.JoystickUp.status) {  
    case PRESSED:
      Serial.print("Joystick Up Presionado ");
      Serial.print(++times_up);    // aumenta el contador y lo imprime
      Serial.println(" veces");
      break;
    case RELEASED:
      Serial.println("Joystick Up suelto");
      break;
    case HOLD_PRESSED:
      // mientras el boton permanezca presionado, entrara en este case
      //Serial.println("Joystick Up permanece presionado");
      break;
    case HOLD_RELEASED:
      // mientras el boton permanezca suelto, entrara en este case
      //Serial.println("Joystick Up permanece suelto");
      break;
  }
} 