// ------------------------------------------------ ROBBUS ZEEGO ------------------------------------------------
//
// EJEMPLO DE USO DEL NEOPIXEL
// Este ejemplo es de uso libre y esta pensado para dar una introduccion al hardware de Robbus Zeego.
// Autor: Rocket Launcher
// Fecha: 23 de noviembre de 2024
// --------------------------------------------------------------------------------------------------------------
//
// Antes de probar este programa, te invitamos a mirar primero el ejemplo NeopixelSimple.
//
// Robbus Zeego cuenta con un LED RGB (Red, Green, Blue o rojo, verde, azul), capaz de mostrar una gama amplia
// de colores, dependiendo de los valores de cada uno de sus componentes.
// Un LED RGB tiene en su interior 3 LEDs diferentes de cada color primario, y al prenderlos con diferente
// intensidad, se logra el cambio de color.
// Como los Neopixels son de 8 bits, quiere decir que cada color puede tener un valor entre 0 (apagado) y
// 255 (encendido al maximo), por lo que 256*256*256 = ¡16,777,216 colores diferentes!
//
// El Neopixel tiene las siguientes funciones
// funciones basicas:
//   color(red,green,blue);               // componentes rojo, azul y verde, con valores de 0 a 255
//   color(color, brightness);            // nombre del color con un brillo entre 1 y 255
//   color(color);                        // nombre del color al maximo brillo
//   off();                               // apaga el neopixel
//
// asi como efectos:
//   fadeIn(color, speed);                // enciende el neopixel lentamente en un color, con intervalos de ms
//   fadeOut(color, speed);               // apaga el neopixel lentamente en un color, con intervalos de ms
//   fadeInOut(color,speed,between,end);  // enciende el led en un color, espera un tiempo, apaga, espera otro tiempo
//   heartBeat(color);                    // efecto de corazon latiendo en un color
//   heartBeat(color, brightness);        // efecto de corazon latiendo en un color, con un brillo maximo
//
// Tambien cuenta con 8 colores predefinidos, enumerados del 0 al 7
//
// BLACK         0 (Apagado)
// RED           1
// GREEN         2
// BLUE          3
// YELLOW        4
// CYAN          5
// MAGENTA       6
// WHITE         7
// 
// Nota importante: Todos los efectos del Neopixel tardan un tiempo en completarse, durante el cual, el programa
// esperara a que termine, no pudiendo hacer otra cosa.
//
// Cuando se presione Joystick Up, el LED hara un latido en rojo.
// Al presionar el Joystick Down, el LED encendera en color azul.
// Con el boton Joystick Left, El Neopixel apagara y encendera lentamente en magenta.
// Con el boton Joystick Right, El Neopixel apagara y encendera lentamente en cyan.
// Cuando se presione Joystick Enter, el LED hara un latido en verde.
// --------------------------------------------------------------------------------------------------------------

#include<RobbusZeego.h>

RobbusZeego Zeego;     // Llama a Robbus Zeego

void setup() {
  Zeego.begin();       // Inicializa el hardware de Robbus Zeego
}

void loop() {
  // Si se presiona Joystick Up, efecto latido en rojo
  if(Zeego.JoystickUp.read() == PRESSED) {
    Zeego.Neopixel.heartBeat(RED);        // efecto de latido en rojo
  }

  // Para evaluar en varios estados posible un boton, debemos leerlo primero y luego
  // consultar la variable status 
  Zeego.JoystickDown.read();    // solo lee el boton JoystickDown

  // Si se presiona Joystick Down, enciende en azul
  if(Zeego.JoystickDown.status == PRESSED) {  // se evalua la variable status a presionado
    Zeego.Neopixel.color(BLUE);           // enciende en azul
  }  
  // Si se suelta Joystick Down, apaga
  if(Zeego.JoystickDown.status == RELEASED) { // se evalua la variable status a suelto
    Zeego.Neopixel.color(OFF);           // se apaga
  }

  // Si se presiona Joystick Left, efecto de encender y apagar lentamente en Magenta
  if(Zeego.JoystickLeft.read() == PRESSED) {
    Zeego.Neopixel.fadeInOut(MAGENTA);    // enciende y apaga el neopixel lentamente en magenta
  }
  
  // Si se presiona Joystick Right, efecto de encender y apagar lentamente en Cyan
  if(Zeego.JoystickRight.read() == PRESSED) {
    Zeego.Neopixel.fadeInOut(CYAN);    // enciende y apaga el neopixel lentamente en cyan
  }

  // Si se presiona Joystick Enter, efecto latido en verde
  if(Zeego.JoystickEnter.read() == PRESSED) {
    Zeego.Neopixel.heartBeat(GREEN);        // efecto de latido en verde
  }
}