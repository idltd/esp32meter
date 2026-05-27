#pragma once
#include <Arduino.h>

struct RResult {
    float        ohms;
    bool         continuity;
    bool         openCircuit;
    const char*  range;
};

struct DiodeResult {
    float        vf;
    const char*  type;
};

struct CapResult {
    float  farads;
    bool   timeout;
    bool   tooSmall;
};

float       measureVoltage();
RResult     measureResistance();
DiodeResult measureDiode();
CapResult   measureCapacitance();
String      formatCapacitance(const CapResult& c);
