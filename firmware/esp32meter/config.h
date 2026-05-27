#pragma once

// AP
#define AP_SSID_PREFIX  "ESP32Meter"
#define AP_PASSWORD     ""              // open network — no password needed

// Pins (ESP32-C3)
#define PIN_ADC_VOLTAGE   0             // voltage divider sense
#define PIN_ADC_RDIODE    1             // resistance / continuity / diode sense
#define PIN_ADC_CAP       2             // capacitance sense
#define PIN_REF_100R      3             // drives 100 Ω reference
#define PIN_REF_1K        4             // drives 1 kΩ reference
#define PIN_REF_10K       5             // drives 10 kΩ reference
#define PIN_CAP_CHARGE    6             // capacitor charge / discharge

// Physical resistor values — measure yours for best accuracy
#define R_REF_100     100.0f
#define R_REF_1K     1000.0f
#define R_REF_10K   10000.0f
#define R_CHARGE    10000.0f

// Voltage divider ratio: V_in = V_adc * VDIV_RATIO
// (180 kΩ + 20 kΩ) / 20 kΩ = 10
#define VDIV_RATIO      10.0f

// ADC reference (millivolts)
#define ADC_VREF_MV    3300.0f

// ADC oversampling
#define ADC_SAMPLES       64

// Continuity threshold
#define CONTINUITY_OHMS   50.0f

// Capacitance charge timeout (microseconds)
#define CAP_TIMEOUT_US  3000000UL
