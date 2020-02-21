#include <Arduino.h>
#include <ESP32Servo.h>
#include "Shared.h"

#define MOTOR1_PIN 17
#define MOTOR2_PIN 16
#define MOTOR3_PIN 8

#define SMOOTHING_SCALE 0.0025

#define MAX_TIMEOUT 500

#define MAX_INPUT 500
#define MIN_INPUT -500

#define MAX_OUTPUT 2000
#define MIN_OUTPUT 1000

#define ROTATION_MAX 1750
#define ROTATION_MIN 1250

Servo motor1;
Servo motor2;
Servo motor3;

float xj, yj, zj;

void OnDataRecv(const uint8_t *mac_addr, const uint8_t *data, int data_len) {
    char macStr[18];
    snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
            mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
    // Serial.print("Last Packet Recv from: "); Serial.println(macStr);
    Serial.print("Last Packet Recv Data: "); Serial.println(*data);
    Serial.println();
}

/**
* calculates what speed all the wheels should move to get the robot moving
* in the correct direction at the target speed.
*/
void calcWheelOutput(){
  float wheel1 = constrain(xj+zj, MIN_INPUT, MAX_INPUT);
  float wheel2 = constrain((-xj/2+(0.866f*yj)+zj), MIN_INPUT, MAX_INPUT);
  float wheel3 = constrain((-xj/2-(0.866f*yj)+zj), MIN_INPUT, MAX_INPUT);
  
  wheel1 = map(wheel1, MIN_INPUT, MAX_INPUT, MIN_OUTPUT, MAX_OUTPUT);
  wheel2 = map(wheel2, MIN_INPUT, MAX_INPUT, MIN_OUTPUT, MAX_OUTPUT);
  wheel3 = map(wheel3, MIN_INPUT, MAX_INPUT, MIN_OUTPUT, MAX_OUTPUT);
  motor1.writeMicroseconds(wheel1);
  motor2.writeMicroseconds(wheel2);
  motor3.writeMicroseconds(wheel3);
}


void setup() {
  Serial.begin(115200);
  // pwm.attachPin(17, 1000, );
  motor1.attach(17, 1000, 2000);
  motor2.attach(16, 1000, 2000);
  motor3.attach(4,  1000, 2000);
  WiFi.disconnect();
  InitWiFi();

  InitESPNow();
  esp_now_register_recv_cb(OnDataRecv);
  AddPeer(DS_MAC);
  delay(3000);
  uint8_t packet[] = "I THINK";
  esp_now_send(peer.peer_addr, packet, sizeof(packet));
  // pinMode(17, OUTPUT);
  // put your setup code here, to run once:
}

void loop() {
  // while(Serial.available() > 0){
  //   xj = Serial.parseInt();
  //   yj = Serial.parseInt();
  //   zj = Serial.parseInt();
  // }

  calcWheelOutput();
  delay(50);
}