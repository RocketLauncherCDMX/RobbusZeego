// ------------------------------------------------ ROBBUS ZEEGO ----------------------------------------------------
//
// EJEMPLO DE CONTROL DE ROBBUS Zeego POR MEDIO DE LA APLICACION BLUEFRUIT
// Este ejemplo es de uso libre y esta pensado para dar una introduccion al hardware de Robbus Zeego.
// Autor: Rocket Launcher
// Fecha: 12 de diciembre de 2021
// Visita www.robbus.mx/zeego para mas informacion
// ------------------------------------------------------------------------------------------------------------------
//
//   Este ejemplo crea una conexion basica entre Robbus Zeego y un telefono, por bluetooth 4.2.
//   El presente ejemplo funciona hasta el dia 12/12/2021 con la aplicacion blueFruit de Adafruit
//   para telefonos Android. https://play.google.com/store/apps/details?id=com.adafruit.bluefruit.le.connect&hl=es
//
//   NOTA: Esta aplicacion es de un tercero y su uso puede variar, no obstante, cualquier cambio se vera reflejado
//         en futuras actualizaciones. Rocket Launcher trabajara arduamente para tener su aplicacion propia en un
//         futuro cercano. Apoyanos comprando nuestros productos :)
//   
//   Pasos para probar este ejemplo:
//
//   1.- Compilar y subir el ejemplo.
//   2.- Serciorarse que tu Robbus Zeego tenga el interruptor en posicion ON.
//   3.- Instalar la aplicacion bluefruit (enlace arriba).
//   4.- Abrir la aplicacion. Aparecera la pantalla Select Device, escoger Robbus Zeego.
//   5.- El Neopixel se encendera en verde, indicando que la conexion fue exitosa. Si no se enciende o marca error
//       volver a intentar.
//   6.- Seleccionar Controller en la siguiente pantalla.
//   7.- Dentro de Available Pins, seleccionar Control Pad.
// ------------------------------------------------------------------------------------------------------------------

#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>
#include <RobbusZeego.h>

RobbusZeego Zeego;

BLEServer *pServer = NULL;
BLECharacteristic * pTxCharacteristic;
bool deviceConnected = false;
bool oldDeviceConnected = false;
uint8_t txValue = 0;

// See the following for generating UUIDs:
// https://www.uuidgenerator.net/

#define SERVICE_UUID           "6E400001-B5A3-F393-E0A9-E50E24DCCA9E" // UART service UUID
#define CHARACTERISTIC_UUID_RX "6E400002-B5A3-F393-E0A9-E50E24DCCA9E"
#define CHARACTERISTIC_UUID_TX "6E400003-B5A3-F393-E0A9-E50E24DCCA9E"

class MyServerCallbacks: public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) {
      deviceConnected = true;
      Zeego.Neopixel.color(GREEN);
    };

    void onDisconnect(BLEServer* pServer) {
      deviceConnected = false;
      Zeego.Neopixel.color(RED);
    }
};

class MyCallbacks: public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic *pCharacteristic) {
    std::string rxValue = pCharacteristic->getValue();

    if (rxValue.length() > 0) {
      Serial.println("*********");
      Serial.print("Received Value: ");
      for (int i = 0; i < rxValue.length(); i++)
        Serial.print(rxValue[i]);

      Serial.println();
      Serial.println("*********");

      if (rxValue == "!B516") Zeego.Move.forward(100);
      else if (rxValue == "!B507") Zeego.Move.stop();
      if (rxValue == "!B615") Zeego.Move.backward(100);
      else if (rxValue == "!B606") Zeego.Move.stop();
      if (rxValue == "!B714") Zeego.Move.turnLeft(150);
      else if (rxValue == "!B705") Zeego.Move.stop();
      if (rxValue == "!B813") Zeego.Move.turnRight(150);
      else if (rxValue == "!B804") Zeego.Move.stop();

      if (rxValue == "!B11:") {
        Zeego.Buzzer.playTone(2000, 50);
        Zeego.Neopixel.color(BLUE);
      }
      else if (rxValue == "!B219") {
        Zeego.Buzzer.playTone(2500, 50);
        Zeego.Neopixel.color(YELLOW);
      }
      else if (rxValue == "!B318") {
        Zeego.Buzzer.playTone(3000, 50);
        Zeego.Neopixel.color(CYAN);
      }
      else if (rxValue == "!B417") {
        Zeego.Buzzer.playTone(3500, 50);
        Zeego.Neopixel.color(MAGENTA);
      }
    }
  }
};

void setup() {
  Serial.begin(115200);
  Zeego.begin();
  Zeego.Move.enableMotors();

  // Create the BLE Device
  BLEDevice::init("Robbus Zeego");

  // Create the BLE Server
  pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());

  // Create the BLE Service
  BLEService *pService = pServer->createService(SERVICE_UUID);

  // Create a BLE Characteristic
  pTxCharacteristic = pService->createCharacteristic(
                    CHARACTERISTIC_UUID_TX,
                    BLECharacteristic::PROPERTY_NOTIFY
                  );
                      
  pTxCharacteristic->addDescriptor(new BLE2902());

  BLECharacteristic * pRxCharacteristic = pService->createCharacteristic(
                       CHARACTERISTIC_UUID_RX,
                      BLECharacteristic::PROPERTY_WRITE
                    );

  pRxCharacteristic->setCallbacks(new MyCallbacks());

  // Start the service
  pService->start();

  // Start advertising
  pServer->getAdvertising()->start();
  Serial.println("Waiting a client connection to notify...");
}

void sendString(String string) {
  for(int i=0; i<string.length(); i++) {
    txValue = (uint8_t)string.charAt(i);
    pTxCharacteristic->setValue(&txValue, 1);
    pTxCharacteristic->notify();
  }
  txValue = '\n';
  pTxCharacteristic->setValue(&txValue, 1);
  pTxCharacteristic->notify();
}

void loop() {
    if (deviceConnected) {
      delay(100); // bluetooth stack will go into congestion, if too many packets are sent
    }

    // disconnecting
    if (!deviceConnected && oldDeviceConnected) {
        delay(500); // give the bluetooth stack the chance to get things ready
        pServer->startAdvertising(); // restart advertising
        Serial.println("start advertising");
        oldDeviceConnected = deviceConnected;
    }
    // connecting
    if (deviceConnected && !oldDeviceConnected) {
    // do stuff here on connecting
        oldDeviceConnected = deviceConnected;
    }
}