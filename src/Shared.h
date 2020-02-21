#ifndef __SHARED_H
#define __SHARED_H

#include <Arduino.h>
#include <WiFi.h>
#include <esp_now.h>
#include <esp_wifi.h>
#include <esp_pm.h>

#define FIRE_CONFIRM_BYTE 123

#define PACKET_TYPE_PING 0
#define PACKET_TYPE_STATUS 1 //response to ping from commander should be a status report (this should be continuity, armed and battery voltage)
#define PACKET_TYPE_SET_ARM 2 // command from cmdr sets weather or not the igniter is armed, must be set to true every second to keep the device armed
#define PACKET_TYPE_FIRE 3 // command from cmdr to fire an igniter

#define CHANNEL 1
#define DS_MAC "24:6F:28:B1:43:14"
#define RC_MAC "98:F4:AB:68:F8:40"

struct DSPacket {
    float xj;
    float yj;
    float zj;
};

union UDSPacket {
    DSPacket data;
    uint8_t raw[sizeof(DSPacket)];
};

extern esp_now_peer_info_t peer;
extern unsigned long connection_timeout;
extern bool connected;

void InitWiFi();
void InitESPNow();
bool AddPeer(String BSSIDstr);
bool ParseResult(esp_err_t result);


#endif