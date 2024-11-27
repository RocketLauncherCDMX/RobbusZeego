// ------------------------------------------------ ROBBUS ZEEGO ----------------------------------------------
//
// EJEMPLO DE USO DE LA SENTENCIA CONDICIONAL IF, USADO EN PROGRAMACION EN LENGUAJE C
// Este ejemplo es de uso libre y esta pensado para dar una introduccion al software de Robbus Zeego.
// Autor: Rocket Launcher
// Fecha: 23 de noviembre de 2024
// ------------------------------------------------------------------------------------------------------------
//
// Decisiones... de eso se trata la vida. Los robots tambien las pueden hacer y Robbus no es la excepcion.
// Una decision puede cambiar, detener o ralentizar el flujo de un programa. Imagina que vas en un autobus
// y a lo lejos vez como el semaforo cambia de verde a rojo y el autobus comienza a frenar hasta detenerse,
// entonces piensas... "cambia a verde para que podamos continuar", pues esta decision la puede tomar un robot
// con la ayuda de la condicional if.
//
// if sirve para evaluar una variable, ya sea una cantidad, estado o entrada, como por ejemplo el boton A de 
// tu robot, y su forma seria:
//
// if(Zeego.JoystickUp.read() == HOLD_PRESSED) { Serial.println("Presionado"); }
// Examinando sus partes:
//
// if(                                    ) {}  <- Cuerpo de la condicion if
//    Zeego.JoystickUp.read()                      <- Variable a evaluar
//                         ==                   <- Tipo de operacion
//                            HOLD_PRESSED      <- Valor con el que sera comparado
//
// De esta forma, para que lo que esta dentro de las llaves pueda ejecutarse, el resultado de la
// condicion if debe ser verdadero, es decir, el valor leido del boton debe ser igual a PRESSED, el 
// boton debe presionarse para que el LED encienda.
//
// Los tipos de operacion que pueden ser evaluados en un if son:
// ==  igual que (signo igual 2 veces, no confundir con un solo signo de igual)
// <   menor que
// >   mayor que
// <=  menor o igual
// >=  mayor o igual
// !=  diferente
//
// Ahora bien, te preguntaras, ¿Y que pasa si quiero que el LED se apague al soltar el boton?
// Pues para eso existe la otra parte de un if, else.
// else es el complemento del if, y la expresion completa seria:
//
// if(Zeego.JoystickUp.read() == HOLD_PRESSED) Serial.println("presionado");
// else Serial.println("-");
//
// "SI el boton esta presionado, entonces imprime "presionado", SI NO, imprime "-"
//
// Y con estas dos palabras ya podemos tomar una decision en funcion de una entrada. Veamos como se
// puede sacar provecho de esta condicional en tu Robbus Zeego para imprimir el estado de Up
// dependiendo de si se presiona o no.
// ------------------------------------------------------------------------------------------------------------

#include<RobbusZeego.h>

RobbusZeego Zeego;    // Llama a Robus Zeego

void setup() {
  Serial.begin(115200);
  Zeego.begin();      // Inicializa el hardware del Robbus Zeego
}

void loop() {
  int buttonUp = Zeego.JoystickUp.read();
  
  if(buttonUp == HOLD_PRESSED) {
    Serial.println("Boton Up presionado");
  } else {
    Serial.println("-");
  }
}