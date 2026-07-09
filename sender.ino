// #include <esp_now.h>
// #include <WiFi.h>

// // REPLACE WITH YOUR RECEIVER'S MAC ADDRESS
// uint8_t receiverAddress[] = {0xcc, 0xdb, 0xa7, 0x98, 0x74, 0xb8};

// // Joystick Button Pin Definitions
// const int JOY1_BUTTON_PIN = 25; 
// const int JOY2_BUTTON_PIN = 26;

// // Structure to hold joystick values and button states
// typedef struct struct_message {
//   int joy1X;
//   int joy1Y;
//   int joy2X;
//   int joy2Y;
//   bool joy1Button; // Stores true (pressed) or false (released)
//   bool joy2Button; // Stores true (pressed) or false (released)
// } struct_message;

// struct_message myData;
// esp_now_peer_info_t peerInfo;

// void setup() {
//   Serial.begin(115200);
//   WiFi.mode(WIFI_STA);

//   // Configure button pins with internal pull-up resistors
//   pinMode(JOY1_BUTTON_PIN, INPUT_PULLUP);
//   pinMode(JOY2_BUTTON_PIN, INPUT_PULLUP);

//   if (esp_now_init() != ESP_OK) {
//     Serial.println("Error initializing ESP-NOW");
//     return;
//   }

//   // Register peer
//   memcpy(peerInfo.peer_addr, receiverAddress, 6);
//   peerInfo.channel = 0;  
//   peerInfo.encrypt = false;
  
//   if (esp_now_add_peer(&peerInfo) != ESP_OK) {
//     Serial.println("Failed to add peer");
//     return;
//   }
// }

// void loop() {
//   // Read Joysticks (ADC1 pins - Safe to use with Wi-Fi)
//   myData.joy1X = analogRead(32);
//   myData.joy1Y = analogRead(33);
//   myData.joy2X = analogRead(34);
//   myData.joy2Y = analogRead(35);

//   // Read Buttons (Invert logic: LOW = Pressed = true)
//   myData.joy1Button = (digitalRead(JOY1_BUTTON_PIN) == LOW);
//   myData.joy2Button = (digitalRead(JOY2_BUTTON_PIN) == LOW);

//   // Send message via ESP-NOW
//   esp_err_t result = esp_now_send(receiverAddress, (uint8_t *) &myData, sizeof(myData));
  
//   delay(20); // 50Hz update rate
// }



#include <esp_now.h>
#include <WiFi.h>

// DOUBLE CHECK THIS MAC ADDRESS MATCHES YOUR RECEIVER
uint8_t receiverAddress[] = {0xe0, 0x8c, 0xfe, 0x57, 0x99, 0x48};

const int JOY1_X_PIN = 32; 
const int JOY1_Y_PIN = 33;
const int JOY2_X_PIN = 34;
const int JOY2_Y_PIN = 35;
const int JOY1_BUTTON_PIN = 25; 
const int JOY2_BUTTON_PIN = 26;

typedef struct struct_message {
  int joy1X;
  int joy1Y;
  int joy2X;
  int joy2Y;
  bool joy1Button; 
  bool joy2Button; 
} struct_message;

struct_message myData;
esp_now_peer_info_t peerInfo;

// DEBUG FUNCTION: Runs automatically every time data is sent
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("Delivery Status: ");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "DELIVERED SUCCESS" : "DELIVERY FAILED (Wrong MAC or Off)");
}

void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);

  pinMode(JOY1_BUTTON_PIN, INPUT_PULLUP);
  pinMode(JOY2_BUTTON_PIN, INPUT_PULLUP);

  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // Register the send callback debug function
  esp_now_register_send_cb(OnDataSent);

  memcpy(peerInfo.peer_addr, receiverAddress, 6);
  peerInfo.channel = 0;  
  peerInfo.encrypt = false;
  
  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("Failed to add peer");
    return;
  }
}

void loop() {
  myData.joy1X = analogRead(JOY1_X_PIN);
  myData.joy1Y = analogRead(JOY1_Y_PIN);
  myData.joy2X = analogRead(JOY2_X_PIN);
  myData.joy2Y = analogRead(JOY2_Y_PIN);
  myData.joy1Button = (digitalRead(JOY1_BUTTON_PIN) == LOW);
  myData.joy2Button = (digitalRead(JOY2_BUTTON_PIN) == LOW);

  esp_err_t result = esp_now_send(receiverAddress, (uint8_t *) &myData, sizeof(myData));
  
  delay(100); // Slower delay just for readable debugging
}
