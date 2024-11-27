// ------------------------------------------------ ROBBUS ZEEGO ------------------------------------------------
//
// EJEMPLO DE USO DEL NEOPIXEL
// Este ejemplo es de uso libre y esta pensado para dar una introduccion al hardware de Robbus Zeego.
// Autor: Rocket Launcher
// Fecha: 23 de noviembre de 2024
// --------------------------------------------------------------------------------------------------------------
//
// Robbus Zeego cuenta con un LED en el centro de las flechas, capaz de mostrar una gran cantidad de colores.
// Este LED, llamado Neopixel es RGB, tiene en su interior 3 LEDs diferentes: Rojo, Verde y Azul, que al 
// combinarlos en distintas proporciones, muestran un nuevo color.
//
// Tu Zeego cuenta con 8 color predefinidos, los cuales pueden ser:
//
// BLACK
// RED
// GREEN
// BLUE
// YELLOW
// CYAN
// MAGENTA
// WHITE
//
// Y los cuales se muestran por medio de la funcion:
// Zeego.Neopixel.color();
// --------------------------------------------------------------------------------------------------------------


#include<RobbusZeego.h>

RobbusZeego Zeego;      // Llama a Robbus Kidsy

void setup() {
  Zeego.begin();        // Inicializa el hardware de Robbus Zeego
}

void loop() {
  Zeego.Neopixel.color(RED);    // Muestra el color Rojo
  delay(1000);
  Zeego.Neopixel.color(GREEN);  // Muestra el color Verde
  delay(1000);
  Zeego.Neopixel.color(BLUE);   // Muestra el color Azul
  delay(1000);
}