// ESP32 Meter — WiFi-served multimeter for beginners
// Measures DC voltage, resistance, continuity, diode Vf, capacitance.
// No configuration needed: flashes up its own WiFi access point (ESP32Meter_XXXX).
// Connect a phone or laptop to that network and open http://192.168.4.1/

#include <WiFi.h>
#include <WebServer.h>
#include <DNSServer.h>

#include "config.h"
#include "measure.h"
#include "webui.h"

WebServer server(80);
DNSServer dnsServer;

void setup() {
    Serial.begin(115200);

    // All reference resistor pins start high-impedance
    pinMode(PIN_REF_100R,   INPUT);
    pinMode(PIN_REF_1K,     INPUT);
    pinMode(PIN_REF_10K,    INPUT);
    pinMode(PIN_CAP_CHARGE, OUTPUT);
    digitalWrite(PIN_CAP_CHARGE, LOW);

    // Full-range ADC (0–3.3 V) with factory calibration enabled
    analogSetAttenuation(ADC_11db);

    // Build AP name from last two bytes of MAC — e.g. "ESP32Meter_A3F2"
    uint8_t mac[6];
    WiFi.macAddress(mac);
    char apName[20];
    snprintf(apName, sizeof(apName), "%s_%02X%02X", AP_SSID_PREFIX, mac[4], mac[5]);

    WiFi.mode(WIFI_AP);
    WiFi.softAP(apName, AP_PASSWORD);

    // Captive portal: answer all DNS queries with the AP's own IP
    dnsServer.start(53, "*", WiFi.softAPIP());

    setupWebServer(server);
    server.begin();

    Serial.printf("[ESP32 Meter] AP: %s\n", apName);
    Serial.printf("[ESP32 Meter] Open: http://%s/\n", WiFi.softAPIP().toString().c_str());
}

void loop() {
    dnsServer.processNextRequest();
    server.handleClient();
}
