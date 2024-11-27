// ------------------------------------------------ ROBBUS ZEEGO ----------------------------------------------
//
// EJEMPLO DE USO DEL JOYSTICK DE 5 POSICIONES
// Este ejemplo es de uso libre y esta pensado para dar una introduccion al hardware del robot Robbus Zeego.
// Autor: Rocket Launcher
// Fecha: 23 de noviembre de 2024
// Visita www.robbus.mx/zeego para mas informacion
// ------------------------------------------------------------------------------------------------------------
//
// Robbus Kidsy cuenta con un Joystick de 5 posiciones:
// - JoystickUp
// - JoystickDown
// - JoystickLeft
// - JoystickRight
// - JoystickEnter
// Nota importante: El boton RESET sirve para reiniciar el programa de tu Robbus Zeego y no se puede programar
// 
// Con este ejemplo entenderas el funcionamiento de los botones de usuario de tu Robot
// Simplemente usa la funcion Zeego.JoystickX.read() donde X es Up, Down, Left, Right, Enter.
// ------------------------------------------------------------------------------------------------------------

#include<RobbusZeego.h>

RobbusZeego Zeego;        // Llama a Robbus Zeego

void setup() {
  Serial.begin(115200);   // Inicializa comunicacion serial a 115200 baudios
  Zeego.begin();          // Inicializa el hardware de Robbus Zeego
}
void loop() {
  if(Zeego.JoystickUp.read() == PRESSED) Serial.println("Joystick Up Presionado");
  if(Zeego.JoystickDown.read() == PRESSED) Serial.println("Joystick Down Presionado");
  if(Zeego.JoystickLeft.read() == PRESSED) Serial.println("Joystick Left Presionado");
  if(Zeego.JoystickRight.read() == PRESSED) Serial.println("Joystick Right Presionado");
  if(Zeego.JoystickEnter.read() == PRESSED) Serial.println("Joystick Enter Presionado");
} 
