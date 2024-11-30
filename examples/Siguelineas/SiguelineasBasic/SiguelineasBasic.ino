// ------------------------------------------------ ROBBUS ZEEGO ----------------------------------------------
//
// EJEMPLO DE SIGUE LINEAS CON MENU Y CONTROL PID
// Este ejemplo corre un algoritmo para seguir lineas, por medio de un control PID
// Autor: Rocket Launcher
// Fecha: 28 de noviembre de 2024
// Visita www.robbus.mx/zeego para mas informacion (proximamente).
// ------------------------------------------------------------------------------------------------------------
//
// MUY IMPORTANTE:
// La pantalla OLED utiliza los controladores de Adafruit, por lo que deberas instalar
// esas bibliotecas por separado, para hacerlo, descomprime la biblioteca
// RobbusZeego.h y busca en la ruta: ../RobbusZeego/externalLibraries las siguientes
// 3 bibliotecas en formato zip:
//
// - Adafruit_BusIO.zip
// - Adafruit_GFX_Library.zip
// - Adafruit_SSD1306.zip
//
// e instalarlas como lo hiciste con esta biblioteca. Si ya las tienes, puedes saltarte
// este paso.
//
// Con este programa, Zeego seguira una linea negra sobre un fondo blanco.
// Para empezar, dibuja una linea negra en un pliego de papel blanco usando un marcador grueso.
// Asegurate de que la linea tenga un grosor de aproximadamente 1 cm y evita hacer curvas
// demasiado cerradas o angulos rectos para que Zeego pueda seguirla facilmente.
//
// Como funciona Zeego?
// 1. Zeego tiene 5 sensores que miran hacia el suelo y detectan que tan oscuro o claro es.
// 2. Usa esos sensores para averiguar donde esta la linea: a la izquierda, a la derecha o al centro?
// 3. Si Zeego se desvia de la linea, ajusta las velocidades de sus motores para volver a ella.
// 4. Con este programa, puedes ajustar algunos parametros para que Zeego tome las curvas mejor
//    o se mueva mas rapido.
//
// Que es un algoritmo PID y como ayuda a Zeego a seguir la linea?
// 
// El PID es como un "cerebro matematico" que le dice a Zeego cuanto debe girar
// o moverse mas rapido para mantenerse sobre la linea. Para hacerlo, usa tres ideas principales:
//
// 1. **P** (Proporcional): 
//    Si Zeego esta muy lejos de la linea, debe girar mucho para regresar.
//    Si esta solo un poco lejos, gira menos. Esto es, como cuando llevas un carrito del
//    super y este comienza a irse de lado, entonces, tu le das un leve empujon en la
//    direccion contraria. Acabas de corregir el error proporcionalmente! Si le das mas
//    duro, la correccion sera mas brusca. Si solo guias el carrito de esta forma, notaras
//    como el carrito comienza a ir en zig-zag con cada empujon.
//
// 2. **I** (Integral): 
//    Si Zeego sigue desviandose del camino aunque intente regresar, este ajuste
//    le ayuda a "recordar" que debe corregir el rumbo mas rapido. En el ejmplo del carrito,
//    es como si despues del empujon inicial, notas que el carrito no regresa al centro, le
//    siguieras dando mas duro para que se acerque mas al centro.
//
// 3. **D** (Derivativo):
//    Este ajuste le dice a Zeego que no gire demasiado rapido para evitar que se pase
//    del otro lado de la linea. Si el carrito de la compra se comienza a acercar a la linea,
//    entonces tu deberias comenzar a reducir la velocidad de forma gradual para que no se
//    desvie ahora al lado contrario.
//
// Combinando estos tres ajustes, Zeego puede seguir la linea de manera suave, incluso 
// en curvas o en caminos complicados. El truco esta en encontrar los mejores valores para
// P, I y D!
//
// Para mover estas constantes, Zeego mostrara en su pantalla 6 variables diferentes:
//
// - Pd: El valor proporcional
// - Pi: El valor integral
// - Pd: El valor derivativo
// - Motores: ON - motores en movimiento / OFF: motores detenidos
// - maxSpeed: Velocidad maxima de los motores (-255 a 255)
// - minSpeed: Velocidad minima de los motores (-255 a 255)
//   Si la velocidad minima es negativa, el motor del lado de la curva no solo se parara
//   para corregir, sino que girara en sentido contrario para que Zeego pueda girar sobre
//   su eje y logre tomar esas curvas.
//
// El joystick de 5 posiciones se utiliza para navegar por el menu y ajustar valores:
//
// - JoystickUp: Mover hacia arriba en el menu.
// - JoystickDown: Mover hacia abajo en el menu.
// - JoystickLeft: Disminuir valores.
// - JoystickRight: Aumentar valores.
// - JoystickEnter: Cambiar el estado ON/OFF de los motores.
//
// Este programa utiliza la EEPROM para guardar los valores ajustados y los recupera al inicio.
//
// Con este ejemplo entenderas el funcionamiento de los sensores de linea, asi como el 
// algoritmo que hace posible a Zeego seguirlas, el joystick, los motores y la pantalla
// OLED. Este es un ejemplo avanzado, que requiere algunas formulas matematicas basicas
// y logica compleja. Te recomendamos que antes de estudiar este ejemplo, corras y analices
// todos los ejemplos disponibles.
// ------------------------------------------------------------------------------------------------------------

#include <Arduino.h>
#include <EEPROM.h>
#include <Adafruit_SSD1306.h>

#include <RobbusZeego.h>


// Inicializacion del objeto Robbus Zeego y pantalla OLED
RobbusZeego Zeego;
Adafruit_SSD1306 display(128, 64, &Wire, -1);

// Constantes y definiciones
#define NUM_SENSORS 5              // Numero de sensores de linea
#define MAX_SENSOR_VALUE 1023      // Valor maximo de los sensores de linea
#define POSITIVE 1                 // Bandera positiva
#define NEGATIVE 0                 // Bandera negativa

// Variables para el menu
uint64_t timeStamp_updateSpeed;
uint16_t sensors[5];              // Valores de los sensores de linea
bool flagSign = POSITIVE;

// Parametros del controlador PID
float Kp = 0.75;                   // Ganancia proporcional
float Ki = 0.2;                    // Ganancia integral
float Kd = 0.5;                    // Ganancia derivativa

// Direccion de memoria en la EEPROM para guardar parametros
bool flashInit;
uint16_t flashInitAddr = 0;        // Direccion para inicializar la memoria
uint16_t kpAddr = 50;              // Direccion para Kp
uint16_t kiAddr = 100;             // Direccion para Ki
uint16_t kdAddr = 150;             // Direccion para Kd
uint8_t lsAddr = 200;              // Direccion para velocidad izquierda
uint8_t rsAddr = 250;              // Direccion para velocidad derecha

// Variables del algoritmo PID
int setpoint = 0;                  // Punto de ajuste del PID
float lastError = 0;               // Error previo para calculo derivativo
float integral = 0;                // Termino integral acumulado
int64_t linePosition;              // Posicion de la linea detectada
float pidOutput;                   // Salida del controlador PID

// Velocidades de los motores
int16_t leftSpeed;
int16_t rightSpeed;

// Limites de velocidad de los motores
uint8_t maxSpeed = 100;
int16_t minSpeed = -100;

// Variables para el menu
uint8_t variableSelect = 0;        // Variable seleccionada en el menu
uint8_t page = 0;                  // Pagina actual del menu
bool wheelsStatus = false;         // Estado ON/OFF de los motores

// Funcion para mostrar el menu en la pantalla OLED
void showMenu() {
  display.clearDisplay();          // Limpiar la pantalla
  switch (page) {
    case 0:                        // Pagina 0: Configuracion de PID
      display.setCursor(15, 0);
      display.printf("Kp: %.2f", Kp);
      display.setCursor(15, 15);
      display.printf("Ki: %.2f", Ki);
      display.setCursor(15, 30);
      display.printf("Kd: %.2f", Kd);
      display.setCursor(15, 45);
      display.print("Motores: ");
      wheelsStatus ? display.print("ON") : display.print("OFF");

      // Indicar la variable seleccionada con una flecha
      switch (variableSelect) {
        case 0: display.setCursor(0, 0); display.print("->"); break;
        case 1: display.setCursor(0, 15); display.print("->"); break;
        case 2: display.setCursor(0, 30); display.print("->"); break;
        case 3: display.setCursor(0, 45); display.print("->"); break;
      }
      break;
    case 1:                        // Pagina 1: Configuracion de velocidad
      display.setCursor(15, 0);
      display.printf("maxSpeed: %u", maxSpeed);
      display.setCursor(15, 15);
      display.printf("minSpeed: %d", minSpeed);
      display.setCursor(15, 30);
      // Si quieres mostrar otra variable, cambia el - por tu variable
      display.printf("-");
      display.setCursor(15, 45);
      // Puedes mostrar dos variables sin necesidad de construir otra pagina
      display.print("-");
      switch (variableSelect) {
        case 0: display.setCursor(0, 0); display.print("->"); break;
        case 1: display.setCursor(0, 15); display.print("->"); break;
        case 2: display.setCursor(0, 30); display.print("->"); break;
        case 3: display.setCursor(0, 45); display.print("->"); break;
      }
      break;
    case 2:
      // Si quieres construir otra pagina, puedes copiar una existente y modificarla
      break;
    case 3:
      // Y otra mas...
      break;
  }
  display.display();   // Al terminar, mostramos el contenido en la pantalla
}

void setup() {
  EEPROM.begin(300);   // Inicializar la EEPROM

  // Leer valores guardados en EEPROM
  EEPROM.get(flashInitAddr, flashInit);
  if (flashInit == 128) {
    EEPROM.get(kpAddr, Kp);
    EEPROM.get(kiAddr, Ki);
    EEPROM.get(kdAddr, Kd);
  } else {
    flashInit = 128;
    EEPROM.put(flashInitAddr, flashInit);
    EEPROM.put(kpAddr, Kp);
    EEPROM.put(kiAddr, Ki);
    EEPROM.put(kdAddr, Kd);
    EEPROM.commit();
  }

  Zeego.begin();                   // Inicializar hardware de Robbus Zeego

  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    // Error en la inicializacion de la pantalla OLED
  }
  display.clearDisplay();          // Limpiar pantalla
  display.setTextSize(1);          // Tamano de texto
  display.setTextColor(SSD1306_WHITE, SSD1306_BLACK);
  showMenu();                      // Mostrar menu inicial
}

void loop() {
  // Array para almacenar las lecturas de los sensores de linea
  int sensors[NUM_SENSORS];
  linePosition = 0; // Inicializa la posicion de la linea en 0

  // Lee los valores de los sensores de linea y los asigna al array
  Zeego.LineSensor.read();
  sensors[0] = Zeego.LineSensor.sensor1;
  sensors[1] = Zeego.LineSensor.sensor2;
  sensors[2] = Zeego.LineSensor.sensor3;
  sensors[3] = Zeego.LineSensor.sensor4;
  sensors[4] = Zeego.LineSensor.sensor5;

  // Lee el estado del joystick hacia abajo
  if (Zeego.JoystickDown.read() == PRESSED) {
    // Navega hacia la siguiente variable o cambia de pagina en el menu
    if (variableSelect <= 2) 
      variableSelect++;
    else {
      page++;
      if (page == 2) page = 0; // Vuelve a la primera pagina si la actual es la ultima
      variableSelect = 0; // Reinicia la seleccion de variables
    }
    showMenu(); // Actualiza el menu en pantalla
  }
  // Lee el estado del joystick hacia arriba
  else if (Zeego.JoystickUp.read() == PRESSED) {
    // Navega hacia la variable anterior o cambia de pagina hacia atras
    if (variableSelect == 0) {
      if (page == 1) page = 0; // Cambia de pagina si es necesario
      else page = 1;
      variableSelect = 3; // Selecciona la ultima variable de la pagina anterior
    } else {
      variableSelect--; // Selecciona la variable anterior
    }
    showMenu(); // Actualiza el menu en pantalla
  }
  // Lee el estado del joystick hacia la derecha
  else if (Zeego.JoystickRight.read() == PRESSED) {
    // Incrementa el valor de la variable seleccionada segun la pagina actual
    switch (page) {
      case 0:
        switch (variableSelect) {
          case 0: // Incrementa Kp
            Kp += 0.1;
            EEPROM.put(kpAddr, Kp); // Guarda el valor en la EEPROM
            EEPROM.commit();
            break;
          case 1: // Incrementa Ki
            Ki += 0.01;
            EEPROM.put(kiAddr, Ki);
            EEPROM.commit();
            break;
          case 2: // Incrementa Kd
            Kd += 0.05;
            EEPROM.put(kdAddr, Kd);
            EEPROM.commit();
            break;
          case 3: // Activa las ruedas
            wheelsStatus = true;
            break;
        }
        break;
      case 1:
        switch (variableSelect) {
          case 0: // Incrementa la velocidad maxima
            if (maxSpeed < 255) maxSpeed += 5;
            break;
          case 1: // Incrementa la velocidad minima
            if (minSpeed < 0) minSpeed += 5;
            break;
          case 2:
            // Si quieres, puedes modificar otra variable aqui
          case 3:
            // Si quieres, puedes modificar otra variable aqui
            break;
        }
        break;
    }
    showMenu(); // Actualiza el menu en pantalla
  }
  // Lee el estado del joystick hacia la izquierda
  else if (Zeego.JoystickLeft.read() == PRESSED) {
    // Decrementa el valor de la variable seleccionada segun la pagina actual
    switch (page) {
      case 0:
        switch (variableSelect) {
          case 0: // Decrementa Kp
            Kp -= 0.1;
            EEPROM.put(kpAddr, Kp); // Guarda el valor en la EEPROM
            EEPROM.commit();
            break;
          case 1: // Decrementa Ki
            Ki -= 0.01;
            EEPROM.put(kiAddr, Ki);
            EEPROM.commit();
            break;
          case 2: // Decrementa Kd
            Kd -= 0.05;
            EEPROM.put(kdAddr, Kd);
            EEPROM.commit();
            break;
          case 3: // Desactiva las ruedas
            wheelsStatus = false;
            break;
        }
        break;
      case 1:
        switch (variableSelect) {
          case 0: // Decrementa la velocidad maxima
            if (maxSpeed > 0) maxSpeed -= 5;
            break;
          case 1: // Decrementa la velocidad minima
            if (minSpeed > -255) minSpeed -= 5;
            break;
          case 2:
          case 3:
            // Acciones especificas pueden agregarse aqui
            break;
        }
        break;
    }
    showMenu(); // Actualiza el menu en pantalla
  }
  // Lee el estado del boton Enter del joystick
  else if (Zeego.JoystickEnter.read() == PRESSED) {
    switch (page) {
      case 0:
        switch (variableSelect) {
          case 3:
            wheelsStatus = !wheelsStatus; // Alterna el estado de las ruedas
            break;
        }
        break;
    }
    showMenu(); // Actualiza el menu en pantalla
  }

  // Control de la velocidad de los motores basado en el PID
  if (millis() > timeStamp_updateSpeed + 50) {
    timeStamp_updateSpeed = millis(); // Actualiza el timestamp
    for (int i = 0; i < NUM_SENSORS; i++) {
      // Calcula la posicion de la linea usando los sensores
      if (i == 0 || i == NUM_SENSORS - 1) {
        linePosition += sensors[i] * (i - (NUM_SENSORS - 1) / 2) * 2;
      } else {
        linePosition += sensors[i] * (i - (NUM_SENSORS - 1) / 2);
      }
    }
    linePosition /= (NUM_SENSORS * 2);

    // Determina el signo de la posicion de la linea
    if (linePosition > 0) flagSign = POSITIVE;
    else if (linePosition < 0) flagSign = NEGATIVE;

    // Calcula el error y los terminos del PID
    int16_t error = linePosition - setpoint;
    integral += error; // Acumula el termino integral
    integral = constrain(integral, -255, 255); // Limita el termino integral
    float derivative = error - lastError; // Calcula el termino derivativo
    lastError = error;

    pidOutput = Kp * error + Ki * integral + Kd * derivative; // Calcula la salida del PID
    pidOutput = constrain(pidOutput, -255, 255); // Limita la salida del PID

    // Ajusta las velocidades de los motores
    leftSpeed = constrain(maxSpeed - pidOutput, minSpeed, maxSpeed);
    rightSpeed = constrain(maxSpeed + pidOutput, minSpeed, maxSpeed);

    // Controla los motores si las ruedas estan activas
    if (wheelsStatus) {
      Zeego.Move.MotorRight(rightSpeed);
      Zeego.Move.MotorLeft(leftSpeed);
    } else {
      Zeego.Move.stop(); // Detiene los motores si las ruedas estan desactivadas
    }
  }
}