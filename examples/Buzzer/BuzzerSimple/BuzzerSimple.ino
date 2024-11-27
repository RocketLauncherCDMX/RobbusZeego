// ------------------------------------------------ ROBBUS ZEEGO ----------------------------------------------
//
// EJEMPLO SIMPLE DE USO DEL BUZZER
// Este ejemplo es de uso libre y esta pensado para dar una introduccion al hardware del robot Robbus Zeego.
// Autor: Rocket Launcher
// Fecha: 23 de noviembre de 2024
// Visita www.robbus.mx/zeego para mas informacion
// ------------------------------------------------------------------------------------------------------------
//
// Robbus Zeego cuenta con 1 buzzer:
// - Buzzer
//
// El buzzer no es otra cosa que una mini bocina, con la que Robbus puede hacer sonidos
// Utilizar el Buzzer es muy sencillo, basta con emplear la funcion Buzzer.playTone().
//
// Kisdy.Buzzer.playTone(100, 50);    // Robbus reproduce un sonido grave durante 50 milisegundos
// Zeego.Buzzer.playTone(1000);       // Robbus reproduce un sonido agudo de forma indefinida
// 
// El odio humano puede escuchar sonidos en el rango de los 20Hz a los 20000Hz
// Los valores cercanos a 20 son sonidos graves, y los valores cercanos a 20000 son sonidos agudos, aunque
// generalmente sonidos mayores a 15000Hz ya son imperceptibles para muchas personas.
//
// Si usas la segunda forma, Robbus no guardara silencio, a menos que uses la funcion Zeego.Buzzer.noTone()
//
// Zeego.Buzzer.playTone(1000);                               // reproduce un sonido agudo de forma indefinida
// if(Zeego.ButtonA.read() == PRESSED) Zeego.Buzzer.noTone(); // guarda silencio al presionar el boton A
// ------------------------------------------------------------------------------------------------------------

#include<RobbusZeego.h>

RobbusZeego Zeego;      // Llama a Robbus Zeego

void setup() {
  Zeego.begin();        // Inicializa el hardware del Robbus Zeego

  Zeego.Buzzer.playTone(500);         // se mantiene sonando
  delay(1000);                        // espera 1 segundo
  Zeego.Buzzer.playTone(1000);        // se mantiene sonando
  delay(1000);                        // espera 1 segundo
  Zeego.Buzzer.playTone(1500);        // se mantiene sonando
  delay(1000);                        // espera 1 segundo
  Zeego.Buzzer.playTone(2000, 1000);  // suena durante 1 segundo
  Zeego.Buzzer.playTone(2500, 1000);  // suena durante 1 segundo
  Zeego.Buzzer.noTone();              // deja de sonar
}

void loop() {
  /* Para no aturdirte, el sonido solo se reproduce una vez en el setup, si quieres volver
     a escucharlo, basta que presiones el boton RESET.*/
}