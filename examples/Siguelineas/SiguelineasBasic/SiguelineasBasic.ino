// ------------------------------------------------ ROBBUS ZEEGO ----------------------------------------------
//
// EJEMPLO DE USO DEL MENU Y CONTROL PID
// Este ejemplo demuestra el uso del menu de configuracion y la integracion de un controlador PID en Robbus Zeego.
// Autor: Rocket Launcher
// Fecha: 28 de noviembre de 2024
// Visita www.robbus.mx/zeego para mas informacion.
// ------------------------------------------------------------------------------------------------------------
//
// Este programa permite ajustar las constantes del PID (Kp, Ki, Kd) y controlar las velocidades maximas y minimas 
// del robot mediante un menu interactivo. Tambien implementa un algoritmo PID para seguir una linea negra.
// Nota importante: El joystick de 5 posiciones se utiliza para navegar por el menu y ajustar valores.
//
// Hardware requerido:
// - Robbus Zeego con sensores de linea y motores.
// - Pantalla OLED Adafruit SSD1306 para el menu.
//
// Joystick de 5 posiciones:
// - JoystickUp: Mover hacia arriba en el menu.
// - JoystickDown: Mover hacia abajo en el menu.
// - JoystickLeft: Disminuir valores.
// - JoystickRight: Aumentar valores.
// - JoystickEnter: Cambiar el estado ON/OFF de los motores.
//
// Este programa utiliza la EEPROM para guardar los valores ajustados y los recupera al inicio.
// Con este ejemplo entenderas el funcionamiento de los sensores de linea, el joystick,
// los motores y la pantalla OLED. Este es un ejemplo avanzado, que requiere algunas formulas
// matematicas basicas y logica compleja. Te recomendamos que antes de estudiar este ejemplo,
// corras y analices todos los ejemplos disponibles.
// ------------------------------------------------------------------------------------------------------------

#include <Arduino.h>
#include <EEPROM.h>
#include <Adafruit_GFX.h>
#include <Adafruit_I2CDevice.h>
#include <Adafruit_SSD1306.h>
#include <RobbusZeego.h>

// Inicialización del objeto Robbus Zeego y pantalla OLED
RobbusZeego Zeego;
Adafruit_SSD1306 display(128, 64, &Wire, -1);

// Constantes y definiciones
#define NUM_SENSORS 5              // Número de sensores de línea
#define MAX_SENSOR_VALUE 1023      // Valor máximo de los sensores de línea
#define POSITIVE 1                 // Bandera positiva
#define NEGATIVE 0                 // Bandera negativa

// Variables para el menú
uint64_t timeStamp_updateSpeed;
uint16_t sensors[5];              // Valores de los sensores de línea
bool flagSign = POSITIVE;

// Parámetros del controlador PID
float Kp = 0.75;                   // Ganancia proporcional
float Ki = 0.2;                    // Ganancia integral
float Kd = 0.5;                    // Ganancia derivativa

// Dirección de memoria en la EEPROM para guardar parámetros
bool flashInit;
uint16_t flashInitAddr = 0;        // Dirección para inicializar la memoria
uint16_t kpAddr = 50;              // Dirección para Kp
uint16_t kiAddr = 100;             // Dirección para Ki
uint16_t kdAddr = 150;             // Dirección para Kd
uint8_t lsAddr = 200;              // Dirección para velocidad izquierda
uint8_t rsAddr = 250;              // Dirección para velocidad derecha

// Variables del algoritmo PID
int setpoint = 0;                  // Punto de ajuste del PID
float lastError = 0;               // Error previo para cálculo derivativo
float integral = 0;                // Término integral acumulado
int64_t linePosition;              // Posición de la línea detectada
float pidOutput;                   // Salida del controlador PID

// Velocidades de los motores
int16_t leftSpeed;
int16_t rightSpeed;

// Límites de velocidad de los motores
uint8_t maxSpeed = 100;
int16_t minSpeed = -100;

// Variables para el menú
uint8_t variableSelect = 0;        // Variable seleccionada en el menú
uint8_t page = 0;                  // Página actual del menú
bool wheelsStatus = false;         // Estado ON/OFF de los motores

// Función para mostrar el menú en la pantalla OLED
void showMenu() {
  display.clearDisplay();          // Limpiar la pantalla
  switch (page) {
    case 0:                        // Página 0: Configuración de PID
      display.setCursor(15, 0);
      display.printf("Kp: %.2f", Kp);
      display.setCursor(15, 15);
      display.printf("Ki: %.2f", Ki);
      display.setCursor(15, 30);
      display.printf("Kd: %.2f", Kd);
      display.setCursor(15, 45);
      display.print("Status: ");
      wheelsStatus ? display.print("ON") : display.print("OFF");

      // Indicar la variable seleccionada con una flecha
      switch (variableSelect) {
        case 0: display.setCursor(0, 0); display.print("->"); break;
        case 1: display.setCursor(0, 15); display.print("->"); break;
        case 2: display.setCursor(0, 30); display.print("->"); break;
        case 3: display.setCursor(0, 45); display.print("->"); break;
      }
      break;
    case 1:                        // Página 1: Configuración de velocidad
      display.setCursor(15, 0);
      display.printf("maxSpeed: %u", maxSpeed);
      display.setCursor(15, 15);
      display.printf("minSpeed: %d", minSpeed);
      display.setCursor(15, 30);
      display.printf("-");
      display.setCursor(15, 45);
      display.print("-");
      switch (variableSelect) {
        case 0: display.setCursor(0, 0); display.print("->"); break;
        case 1: display.setCursor(0, 15); display.print("->"); break;
        case 2: display.setCursor(0, 30); display.print("->"); break;
        case 3: display.setCursor(0, 45); display.print("->"); break;
      }
      break;
    case 2:
      break;
    case 3:
      break;
  }
  display.display();               // Mostrar contenido en la pantalla
}

void setup() {
  Serial.begin(115200);            // Inicializar comunicación serial
  EEPROM.begin(300);               // Inicializar la EEPROM

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
    // Error en la inicialización de la pantalla OLED
  }
  display.clearDisplay();          // Limpiar pantalla
  display.setTextSize(1);          // Tamaño de texto
  display.setTextColor(SSD1306_WHITE);
  showMenu();                      // Mostrar menú inicial
}

void loop() {
  // Array para almacenar las lecturas de los sensores de línea
  int sensors[NUM_SENSORS];
  linePosition = 0; // Inicializa la posición de la línea en 0

  // Lee los valores de los sensores de línea y los asigna al array
  Zeego.LineSensor.read();
  sensors[0] = Zeego.LineSensor.sensor1;
  sensors[1] = Zeego.LineSensor.sensor2;
  sensors[2] = Zeego.LineSensor.sensor3;
  sensors[3] = Zeego.LineSensor.sensor4;
  sensors[4] = Zeego.LineSensor.sensor5;

  // Lee el estado del joystick hacia abajo
  if (Zeego.JoystickDown.read() == PRESSED) {
    // Navega hacia la siguiente variable o cambia de página en el menú
    if (variableSelect <= 2) 
      variableSelect++;
    else {
      page++;
      if (page == 2) page = 0; // Vuelve a la primera página si la actual es la última
      variableSelect = 0; // Reinicia la selección de variables
    }
    showMenu(); // Actualiza el menú en pantalla
  }
  // Lee el estado del joystick hacia arriba
  else if (Zeego.JoystickUp.read() == PRESSED) {
    // Navega hacia la variable anterior o cambia de página hacia atrás
    if (variableSelect == 0) {
      if (page == 1) page = 0; // Cambia de página si es necesario
      else page = 1;
      variableSelect = 3; // Selecciona la última variable de la página anterior
    } else {
      variableSelect--; // Selecciona la variable anterior
    }
    showMenu(); // Actualiza el menú en pantalla
  }
  // Lee el estado del joystick hacia la derecha
  else if (Zeego.JoystickRight.read() == PRESSED) {
    // Incrementa el valor de la variable seleccionada según la página actual
    switch (page) {
      case 0:
        switch (variableSelect) {
          case 0: // Incrementa Kp
            Kp += 0.01;
            EEPROM.put(kpAddr, Kp); // Guarda el valor en la EEPROM
            EEPROM.commit();
            break;
          case 1: // Incrementa Ki
            Ki += 0.01;
            EEPROM.put(kiAddr, Ki);
            EEPROM.commit();
            break;
          case 2: // Incrementa Kd
            Kd += 0.01;
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
          case 0: // Incrementa la velocidad máxima
            if (maxSpeed < 255) maxSpeed += 1;
            break;
          case 1: // Incrementa la velocidad mínima
            if (minSpeed < 0) minSpeed += 1;
            break;
          case 2:
          case 3:
            // Acciones específicas pueden agregarse aquí
            break;
        }
        break;
    }
    showMenu(); // Actualiza el menú en pantalla
  }
  // Lee el estado del joystick hacia la izquierda
  else if (Zeego.JoystickLeft.read() == PRESSED) {
    // Decrementa el valor de la variable seleccionada según la página actual
    switch (page) {
      case 0:
        switch (variableSelect) {
          case 0: // Decrementa Kp
            Kp -= 0.01;
            EEPROM.put(kpAddr, Kp); // Guarda el valor en la EEPROM
            EEPROM.commit();
            break;
          case 1: // Decrementa Ki
            Ki -= 0.01;
            EEPROM.put(kiAddr, Ki);
            EEPROM.commit();
            break;
          case 2: // Decrementa Kd
            Kd -= 0.01;
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
          case 0: // Decrementa la velocidad máxima
            if (maxSpeed > 0) maxSpeed -= 1;
            break;
          case 1: // Decrementa la velocidad mínima
            if (minSpeed > -255) minSpeed -= 1;
            break;
          case 2:
          case 3:
            // Acciones específicas pueden agregarse aquí
            break;
        }
        break;
    }
    showMenu(); // Actualiza el menú en pantalla
  }
  // Lee el estado del botón Enter del joystick
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
    showMenu(); // Actualiza el menú en pantalla
  }

  // Control de la velocidad de los motores basado en el PID
  if (millis() > timeStamp_updateSpeed + 50) {
    timeStamp_updateSpeed = millis(); // Actualiza el timestamp
    for (int i = 0; i < NUM_SENSORS; i++) {
      // Calcula la posición de la línea usando los sensores
      if (i == 0 || i == NUM_SENSORS - 1) {
        linePosition += sensors[i] * (i - (NUM_SENSORS - 1) / 2) * 2;
      } else {
        linePosition += sensors[i] * (i - (NUM_SENSORS - 1) / 2);
      }
    }
    linePosition /= (NUM_SENSORS * 2);

    // Determina el signo de la posición de la línea
    if (linePosition > 0) flagSign = POSITIVE;
    else if (linePosition < 0) flagSign = NEGATIVE;

    // Calcula el error y los términos del PID
    int16_t error = linePosition - setpoint;
    integral += error; // Acumula el término integral
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
