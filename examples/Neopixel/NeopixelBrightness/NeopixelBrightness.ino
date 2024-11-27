// ------------------------------------------------ ROBBUS ZEEGO ------------------------------------------------
//
// EJEMPLO DE USO DEL NEOPIXEL CON REGULACION DE BRILLO
// Este ejemplo es de uso libre y esta pensado para dar una introduccion al hardware de Robbus Zeego.
// Autor: Rocket Launcher
// Fecha: 23 de noviembre de 2024
// --------------------------------------------------------------------------------------------------------------
// Si aun no has visto el ejemplo NeopixelSimple, te invitamos a probarlo primero.
// 
// Robbus Zeego cuenta con un LED en el centro de las flechas, capaz de mostrar una gran cantidad de colores.
// Este LED, llamado Neopixel es RGB, tiene en su interior 3 LEDs diferentes: Rojo, Verde y Azul, que al 
// combinarlos en distintas proporciones, muestran un nuevo color. Estos colores, a su vez pueden ser regulados
// para variar el brillo.
//
// Tu Zeego cuenta con 8 color predefinidos, los cuales pueden ser:
//
// BLACK (Apagado)
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
// 
// Esta funcion puede regular el brillo del Neopixel, tanto en color solido, como en los efectos, por ejemplo:
// Zeego.Neopixel.color(RED, 100);         // Mostrara el color rojo con un brillo de 100 (maximo 255)
//
// Tambien dispone de algunos interesantes efectos, como un latido:
// Zeego.Neopixel.heartBeat(GREEN, 50);   // Mostrara el efecto heartBeat con un brillo de 50 (maximo 255);
// --------------------------------------------------------------------------------------------------------------

#include<RobbusZeego.h>

RobbusZeego Zeego;      // Llama a Robbus Zeego

void setup() {
  Zeego.begin();        // Inicializa el hardware de Robbus Zeego
}

void loop() {
  Zeego.Neopixel.color(RED, 50);    // Muestra el color Rojo con brillo 50
  delay(1000);
  Zeego.Neopixel.color(GREEN, 50);  // Muestra el color Verde con brillo 50
  delay(1000);
  Zeego.Neopixel.color(BLUE, 50);   // Muestra el color Azul con brillo 50
  delay(1000);

  Zeego.Neopixel.heartBeat(CYAN, 50);    // Muestra latido en Cyan con brillo 50
  delay(1000);
  Zeego.Neopixel.heartBeat(MAGENTA, 100);  // Muestra latido en Magenta con brillo 100
  delay(1000);
  Zeego.Neopixel.heartBeat(YELLOW, 255);   // Muestra latido en Amarillo con brillo al maximo
  delay(1000);
}