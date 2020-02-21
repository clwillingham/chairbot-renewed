#include <Arduino.h>
#include "Shared.h"

#include <CmdBuffer.hpp>
#include <CmdCallback.hpp>
#include <CmdParser.hpp>

CmdCallback<1> cmdCallback;


void OnDataSend(const uint8_t *mac_addr, esp_now_send_status_t status){
    char macStr[18];
    snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
            mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
    Serial.print("Last Packet sent to: "); Serial.println(macStr);
    switch(status){
        case ESP_NOW_SEND_SUCCESS: Serial.println("Success!");break;
        case ESP_NOW_SEND_FAIL: Serial.println("FAILED!");break;
        default: Serial.println("Really Failed");break;
    }
}


void OnDataRecv(const uint8_t *mac_addr, const uint8_t *data, int data_len) {
    char macStr[18];
    snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
            mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
    // Serial.print("Last Packet Recv from: "); Serial.println(macStr);
    Serial.print("Last Packet Recv Data: "); Serial.println(*data);
    Serial.println();
}

void HandleMoveCmd(CmdParser *myParser){
    char* x = myParser->getCmdParam(1);
    char* y = myParser->getCmdParam(2);
    char* z = myParser->getCmdParam(3);
    Serial.print("Received: ");
    Serial.print(x);
    Serial.print(", ");
    Serial.print(y);
    Serial.print(", ");
    Serial.println(z);
}

void setup(){
    Serial.begin(115200);
    WiFi.disconnect();
    InitWiFi();
    Serial.println(WiFi.getMode());
    InitESPNow();

    WiFi.enableSTA(true);
    esp_wifi_set_mode(WIFI_MODE_STA);
    esp_wifi_set_channel(CHANNEL, WIFI_SECOND_CHAN_NONE);
    esp_wifi_start();
    Serial.println(WiFi.getMode());
    cmdCallback.addCmd("move", &HandleMoveCmd);


    esp_now_register_recv_cb(OnDataRecv);
    esp_now_register_send_cb(OnDataSend);
    AddPeer(RC_MAC);
}

void loop() {
    uint8_t packet[] = "Hello";
    CmdBuffer<32> myBuffer;
    CmdParser     myParser;

    cmdCallback.updateCmdProcessing(&myParser, &myBuffer, &Serial);

    esp_now_send(peer.peer_addr, packet, sizeof(packet));
    delay(500);
}