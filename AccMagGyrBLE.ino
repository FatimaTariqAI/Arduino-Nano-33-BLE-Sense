#include <ArduinoBLE.h>
#include <Arduino_HTS221.h>
#include <Arduino_LPS22HB.h>
#include <Arduino_LSM9DS1.h>

String p, t, m;

BLEService customService("180C");
BLEStringCharacteristic ble_pressure("2A56", BLERead | BLENotify, 13);
BLEStringCharacteristic ble_temperature("2A57", BLERead | BLENotify, 13);
BLEStringCharacteristic ble_magnetic("2A58", BLERead | BLENotify, 20);

void readValues();

void setup() {
    HTS.begin();
    BARO.begin();
    IMU.begin();
    Serial.begin(9600);
    while (!Serial);
    
    if (!BLE.begin()) {
        Serial.println("BLE failed to Initiate");
        delay(500);
        while (1);
    }

    BLE.setLocalName("Arduino Environment Sensor");
    BLE.setAdvertisedService(customService);
    customService.addCharacteristic(ble_pressure);
    customService.addCharacteristic(ble_temperature);
    customService.addCharacteristic(ble_magnetic);
    BLE.addService(customService);

    BLE.advertise();
    Serial.println("Bluetooth device is now active, waiting for connections...");
}

void loop() {
    BLEDevice central = BLE.central();
    
    if (central) {
        Serial.print("Connected to central: ");
        Serial.println(central.address());
        
        while (central.connected()) {
            delay(5);
            readValues();
            ble_pressure.writeValue(p);
            ble_temperature.writeValue(t);
            ble_magnetic.writeValue(m);

            Serial.println("Reading Sensors");
            Serial.println(p);
            Serial.println(t);
            Serial.println(m);
            Serial.println("\n");

            // Check for a specific character received from the serial monitor
            if (Serial.available() > 0) {
                char command = Serial.read();
                
                // Assuming 'D' represents a command from the PC
                if (command == 'D') {
                    // Perform actions specific to the connection with the programmed PC
                    Serial.println("Command received from the programmed PC");
                }
            }

            delay(1000);
        }
    }
    
    Serial.print("Disconnected from central: ");
    Serial.println(central.address());
}

void readValues() {
    float x, y, z;
    float pressure = BARO.readPressure();
    float temperature = HTS.readTemperature();
    
    if (IMU.magneticFieldAvailable()) {
        IMU.readMagneticField(x, y, z);
        p = String(pressure) + " kPa";
        t = String(temperature) + " C";
        m = "X:" + String(x) + ", Y:" + String(y);
    }
}
