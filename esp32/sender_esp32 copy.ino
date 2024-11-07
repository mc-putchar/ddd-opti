#include <esp_now.h>
#include <esp_wifi.h>
#include <WiFi.h>

esp_now_peer_info_t peerInfo;
char buffer[1024];

uint8_t senderMacAdd[6] = {
  0x08, 0xB6, 0x1F, 0xBC, 0x8E, 0x9A
};

uint8_t broadcastAddresses[][6] = {
  { 0x08, 0xB6, 0x1F, 0xBC, 0x8E, 0x9B },
  { 0x08, 0xB6, 0x1F, 0xBC, 0x8E, 0x9C },
  { 0x08, 0xB6, 0x1F, 0xBC, 0x8E, 0x9D },
  { 0x08, 0xB6, 0x1F, 0xBC, 0x8E, 0x9E },
};

void data_recv_cb(const esp_now_recv_info_t *info, const uint8_t *incomingData, int len) {
	// Serial.println("data received");
	Serial.write(incomingData, len);
}

void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) { // callback when data is sent
	// Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}

void registerPeer(size_t drone) {
	memcpy(peerInfo.peer_addr, broadcastAddresses[drone], 6);
	peerInfo.channel = 0;
	peerInfo.encrypt = false;
	if (esp_now_add_peer(&peerInfo) != ESP_OK) {
		Serial.println("Failed to add peer");
	}
}

void readMacAddress(){
	uint8_t baseMac[6];
	esp_err_t ret = esp_wifi_get_mac(WIFI_IF_STA, baseMac);
	if (ret == ESP_OK) {
		Serial.printf("%02x:%02x:%02x:%02x:%02x:%02x\n",
					baseMac[0], baseMac[1], baseMac[2],
					baseMac[3], baseMac[4], baseMac[5]);
	} else {
		Serial.println("Failed to read MAC address");
	}
}

void setup() {

	Serial.begin(115200); // Init Serial Monitor
	// Serial.begin(1000000);

	// Set device as a Wi-Fi Station
	wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
	esp_wifi_init(&cfg);
	esp_wifi_set_mode(WIFI_MODE_STA);
	esp_wifi_set_mac(WIFI_IF_STA, senderMacAdd);
	esp_wifi_set_bandwidth(WIFI_IF_STA, WIFI_BW_HT20);
	esp_wifi_set_storage(WIFI_STORAGE_RAM);
	esp_wifi_set_ps(WIFI_PS_NONE);
	//esp_wifi_set_channel(1, WIFI_SECOND_CHAN_NONE);
	esp_wifi_start();
	if (esp_now_init() != ESP_OK) { // Init ESP-NOW
		Serial.println("Error initializing ESP-NOW");
		return;
	}
	esp_wifi_config_espnow_rate(WIFI_IF_STA, WIFI_PHY_RATE_24M);
	esp_wifi_start();
	esp_now_register_send_cb(OnDataSent); // reguster the call Back for on Data sent.
	esp_now_register_recv_cb(data_recv_cb);

	// Register peer drone 00
	registerPeer(0);
	registerPeer(1);
	readMacAddress();
}

void loop() {
	int availableBytes = Serial.available();
	if (availableBytes) {
		// int droneIndex = Serial.read() - '0';
		Serial.readBytes(buffer, availableBytes);
		buffer[availableBytes] = '\0';
		// Serial.print(buffer);
		esp_err_t result = esp_now_send(broadcastAddresses[0], (uint8_t *)&buffer, strlen(buffer) + 1);
		if (result) {
			Serial.println(esp_err_to_name(result));
		}
		esp_err_t result = esp_now_send(broadcastAddresses[1], (uint8_t *)&buffer, strlen(buffer) + 1);
		if (result) {
			Serial.println(esp_err_to_name(result));
		}
	}
	else {
		yield();
	}
}