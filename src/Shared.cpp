#include "shared.h"
#include "esp_log.h"
esp_now_peer_info_t peer;
bool connected = false;
unsigned long connection_timeout;

void InitWiFi(){
    tcpip_adapter_init();
    // ESP_ERROR_CHECK( esp_event_loop_init(example_event_handler, NULL) );
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK( esp_wifi_init(&cfg) );
    ESP_ERROR_CHECK( esp_wifi_set_storage(WIFI_STORAGE_RAM) );
    ESP_ERROR_CHECK( esp_wifi_set_mode(WIFI_MODE_STA) );
    ESP_ERROR_CHECK( esp_wifi_start());

    /* In order to simplify example, channel is set after WiFi started.
     * This is not necessary in real application if the two devices have
     * been already on the same channel.
     */
    ESP_ERROR_CHECK( esp_wifi_set_channel(CHANNEL, WIFI_SECOND_CHAN_NONE) );

    // WiFi.begin(NULL, NULL, CHANNEL, NULL, false);
    // WiFi.mode(WIFI_STA);
    // WiFi.enableSTA(true);
    // WiFi.begin();
    Serial.println(WiFi.macAddress());
    // esp_wifi_set_ps(WIFI_PS_MAX_MODEM);
}

// Init ESP Now with fallback
void InitESPNow() {
    WiFi.disconnect();
    if (esp_now_init() == ESP_OK) {
        Serial.println("ESPNow Init Success");
    }
    else {
        Serial.println("ESPNow Init Failed");
        // Retry InitESPNow, add a counte and then restart?
        // InitESPNow();
        // or Simply Restart
        ESP.restart();
    }
}

bool ParseResult(esp_err_t result){
    if (result == ESP_OK) {
        // Pair success
        Serial.println("Pair success");
        return true;
    } else if (result == ESP_ERR_ESPNOW_NOT_INIT) {
        // How did we get so far!!
        Serial.println("ESPNOW Not Init");
        return false;
    } else if (result == ESP_ERR_ESPNOW_ARG) {
        Serial.println("Invalid Argument");
        return false;
    } else if (result == ESP_ERR_ESPNOW_FULL) {
        Serial.println("Peer list full");
        return false;
    } else if (result == ESP_ERR_ESPNOW_NO_MEM) {
        Serial.println("Out of memory");
        return false;
    } else if (result == ESP_ERR_ESPNOW_EXIST) {
        Serial.println("Peer Exists");
        return true;
    } else  if(result == ESP_ERR_ESPNOW_NOT_FOUND){
        Serial.println("404 Peer Not Found");
        return false;
    } else if(result == ESP_ERR_ESPNOW_IF){
        Serial.println("current WiFi interface doesn’t match that of peer");
        return false;
    } else {
        Serial.println("You screwed up");
        return false;
    }
}


bool AddPeer(String BSSIDstr){
    int mac[6];
    if ( 6 == sscanf(BSSIDstr.c_str(), "%x:%x:%x:%x:%x:%x",  &mac[0], &mac[1], &mac[2], &mac[3], &mac[4], &mac[5] ) ) { //TODO: had %c at end, not sure why
        for (int ii = 0; ii < 6; ++ii ) {
            peer.peer_addr[ii] = (uint8_t) mac[ii];
        }
    }
    peer.channel = CHANNEL; // pick a channel
    peer.encrypt = 0; // no encryption
    peer.ifidx = WIFI_IF_STA;
    if(!esp_now_is_peer_exist(peer.peer_addr))
        return ParseResult(esp_now_add_peer(&peer));
    else
        return true;
}
