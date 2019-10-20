
#include <ArduinoBLE.h>
#include <Arduino_LSM9DS1.h>

const int ledPin = 22;
const int ledPin2 = 23;
const int ledPin3 = 24;

 // BLE Service
BLEService imuService("917649A0-D98E-11E5-9EEC-0002A5D5C51B"); // Custom UUID

// BLE Characteristic
BLECharacteristic imuCharacteristic("917649A1-D98E-11E5-9EEC-0002A5D5C51B", BLERead | BLENotify, 36);

long previousMillis = 0;  // last timechecked, in ms

void setup() {
  pinMode(22, OUTPUT);
pinMode(23, OUTPUT);
pinMode(24, OUTPUT);
  Serial.begin(9600);    // initialize serial communication

  pinMode(LED_BUILTIN, OUTPUT); // initialize the built-in LED pin to indicate when a central is connected

// begin initialization
  if (!IMU.begin()) {
    Serial.println("Failed to initialize IMU!");
    while (1);
  }

  Serial.print("Euler Angles sample rate = ");
  //Serial.print(IMU.eulerAnglesSampleRate());
  Serial.println(" Hz");
  Serial.println();
  Serial.println("Euler Angles in degrees");
  Serial.println("X\tY\tZ");
  
  if (!BLE.begin()) {
    Serial.println("starting BLE failed!");
    while (1);
  }

  // Setup bluetooth
  BLE.setLocalName("LIGAMED");
  BLE.setAdvertisedService(imuService); 
  imuService.addCharacteristic(imuCharacteristic);
  BLE.addService(imuService); 
  
  // start advertising
  BLE.advertise();
  Serial.println("Bluetooth device active, waiting for connections...");
}


// send IMU data
void sendSensorData() {
float eulers[5];
short output[5];
// read orientation x, y and z eulers

IMU.readAcceleration(eulers[0], eulers[1], eulers[2]);
IMU.readGyroscope(eulers[3], eulers[4], eulers[5]);

output[0] = round(eulers[0]*1000);
output[1] = round(eulers[1]*1000);
output[2] = round(eulers[2]*1000);
output[3] = round(eulers[3]);
output[4] = round(eulers[4]);
output[5] = round(eulers[5]);

for(int i=0;i<=5;i++){
  if (output[i]>32000){output[i] = 32000;}
  if (output[i]<-32000){output[i] = -32000;}
}


// Send 3x eulers over bluetooth as 1x byte array 
imuCharacteristic.setValue((byte *) &output, 36); 

} 

void loop() {
  // wait for a BLE central
  BLEDevice central = BLE.central();
  digitalWrite(ledPin, LOW);
     digitalWrite(ledPin2, LOW);
     digitalWrite(ledPin3, LOW);
  delay(1000);          
     digitalWrite(ledPin, LOW);
     digitalWrite(ledPin2, LOW);
     digitalWrite(ledPin3, HIGH);
  delay(1000);    
     digitalWrite(ledPin, LOW);
     digitalWrite(ledPin2, HIGH);
     digitalWrite(ledPin3, HIGH);
  delay(1000);  
       digitalWrite(ledPin, HIGH);
       digitalWrite(ledPin2, LOW);
       digitalWrite(ledPin3, LOW);
  delay(1000);  
       digitalWrite(ledPin, HIGH);
       digitalWrite(ledPin2, LOW);
       digitalWrite(ledPin3, HIGH);
  delay(1000);  
       digitalWrite(ledPin, HIGH);
       digitalWrite(ledPin2, HIGH);
       digitalWrite(ledPin3, LOW);
  delay(1000);  

         digitalWrite(ledPin, HIGH);
       digitalWrite(ledPin2, HIGH);
       digitalWrite(ledPin3, HIGH);
  delay(1000);  
  // if a BLE central is connected to the peripheral:
  if (central) {
    Serial.print("Connected to central: ");
    // print the central's BT address:
    Serial.println(central.address());
    // turn on the LED to indicate the connection:
    digitalWrite(LED_BUILTIN, HIGH);

    // while the central is connected:
    while (central.connected()) {
      long currentMillis = millis();

      if (currentMillis - previousMillis >= 50) {
          if (IMU.gyroscopeAvailable()) { // XX
        previousMillis = currentMillis;
        sendSensorData();
          }
      }
    }
    // when the central disconnects, turn off the LED:
    digitalWrite(LED_BUILTIN, LOW);
    Serial.print("Disconnected from central: ");
    Serial.println(central.address());
  }
}
