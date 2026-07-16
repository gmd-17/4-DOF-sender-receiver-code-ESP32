#include <esp_now.h>
#include <WiFi.h>
#include <ESP32Servo.h> 

// --- Servo Object Declarations ---
Servo X1_servo, Y1_servo;
Servo X2_servo, Y2_servo;        


const int X1_PIN = 12; 
const int Y1_PIN = 13;
const int X2_PIN = 14;
const int Y2_PIN = 27;

// --- ESP-NOW Structure (Matches Transmitter) ---
typedef struct struct_message {
  int joy1X;
  int joy1Y;
  int joy2X;
  int joy2Y;
  bool joy1Button; 
  bool joy2Button; 
} struct_message;

struct_message incomingData;
bool newDataAvailable = false;

// --- Speed and Step Calculations ---
int speed = 0;      // increase speed variable for faster servo movements
int stepSize = speed + 1;

// --- Servo Angle Tracking Variables ---
int currentAngleX1 = 90;
int currentAngleY1 = 90;
int currentAngleX2 = 90;         
int currentAngleY2 = 90;         

// --- Raw Input Storage Variables (Defaults to center 2048) ---
int raw_X1 = 2048; 
int raw_Y1 = 2048;
int raw_X2 = 2048;
int raw_Y2 = 2048;
bool R3_1 = false;
bool R3_2 = false;

// FIXED: Allocating proper memory buffer size prevents memory crashes
char buff[150];                  

void OnDataRecv(const uint8_t * mac, const uint8_t *incomingReadings, int len) {
  memcpy(&incomingData, incomingReadings, sizeof(incomingData));
  
  raw_X1 = incomingData.joy1X;
  raw_Y1 = incomingData.joy1Y;
  raw_X2 = incomingData.joy2X;
  raw_Y2 = incomingData.joy2Y;
  
  R3_1 = incomingData.joy1Button;
  R3_2 = incomingData.joy2Button;
  
  newDataAvailable = true;
}

void setup() {
  Serial.begin(115200);          
  WiFi.mode(WIFI_STA);

  // Attach servos to ESP32 pins
  X1_servo.attach(X1_PIN);
  Y1_servo.attach(Y1_PIN);
  X2_servo.attach(X2_PIN);             
  Y2_servo.attach(Y2_PIN);             

  X1_servo.write(currentAngleX1);
  Y1_servo.write(currentAngleY1);
  X2_servo.write(currentAngleX2);
  Y2_servo.write(currentAngleY2);

  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  
  esp_now_register_recv_cb(OnDataRecv);
  delay(500);
  Serial.println("Receiver Ready. Awaiting data...");
}

void loop() {
  // --- Process Joystick 1 (Deadzone: 1400 to 3000) ---
  if(R3_1 == true) { 
    currentAngleX1 = 90;
    currentAngleY1 = 90;
  }
  else {
    if(raw_X1 > 3000){
      currentAngleX1 -= map(raw_X1, 3000, 4095, 1, stepSize);
    }
    else if(raw_X1 < 1400){
      currentAngleX1 += map(raw_X1, 0, 1400, stepSize, 1);
    }
  
    if(raw_Y1 > 3000){
      currentAngleY1 += map(raw_Y1, 3000, 4095, 1, stepSize);
    }
    else if(raw_Y1 < 1400){
      currentAngleY1 -= map(raw_Y1, 0, 1400, stepSize, 1);
    }
  }

  // --- Process Joystick 2 (Deadzone: 1400 to 3000) ---
  if(R3_2 == true) { 
    currentAngleX2 = 90;
    currentAngleY2 = 90;
  }
  else {
    if(raw_X2 > 3000){
      currentAngleX2 -= map(raw_X2, 3000, 4095, 1, stepSize);
    }
    else if(raw_X2 < 1400){
      currentAngleX2 += map(raw_X2, 0, 1400, stepSize, 1);
    }
  
    if(raw_Y2 > 3000){
      currentAngleY2 += map(raw_Y2, 3000, 4095, 1, stepSize);
    } 
    else if(raw_Y2 < 1400){
      currentAngleY2 -= map(raw_Y2, 0, 1400, stepSize, 1);
    }
  }

  // Bound constraints
  currentAngleX1 = constrain(currentAngleX1, 0, 180);
  currentAngleY1 = constrain(currentAngleY1, 90, 180);
  currentAngleX2 = constrain(currentAngleX2, 0, 180);
  currentAngleY2 = constrain(currentAngleY2, 90, 180);
  
  // Update hardware positions
  X1_servo.write(currentAngleX1);
  Y1_servo.write(currentAngleY1);
  X2_servo.write(currentAngleX2);
  Y2_servo.write(currentAngleY2);

  // Print tracking telemetry data stream
  if (newDataAvailable) {
    sprintf(buff, "J1_X:%4d Y:%4d | J2_X:%4d Y:%4d | Angles: %3d %3d %3d %3d", 
            raw_X1, raw_Y1, raw_X2, raw_Y2, currentAngleX1, currentAngleY1, currentAngleX2, currentAngleY2);
    Serial.println(buff);
    newDataAvailable = false;
  }

  delay(20); 
}


// Use the code below to find the MAC Address of the receiver board
// #include <WiFi.h>

// void setup() {
//   Serial.begin(115200); // Start talking to the computer
//   WiFi.mode(WIFI_STA);  // Turn on Wi-Fi
//   delay(1000);
  
//   Serial.print("ESP32 MAC Address: ");
//   Serial.println(WiFi.macAddress()); // Get and print the address
// }

// void loop() {
//   // Do nothing
// }
