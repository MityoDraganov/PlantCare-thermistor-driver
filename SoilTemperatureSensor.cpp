#include "SoilTemperatureSensor.h"
#include <Arduino.h>
#include <cmath>

// Static instance of the TemperatureSensor
static TemperatureSensor temperatureSensor;

// Constants for thermistor and resistor
const float TemperatureSensor::R1 = 10000;       // 10KΩ resistor value
const float TemperatureSensor::BETA = 3435;      // Beta coefficient for the thermistor
const float TemperatureSensor::T0 = 298.15;      // Room temperature in Kelvin (25°C = 298.15K)
const float TemperatureSensor::R0 = 10000;       // Resistance of the thermistor at 25°C (10KΩ)

TemperatureSensor::TemperatureSensor() : readIndex(0), total(0), average(0) {
    Serial.println("TemperatureSensor constructor called");
    SensorManager::registerSensor(this);  // Register with the SensorManager
    std::fill(std::begin(readings), std::end(readings), 0);  // Initialize readings array
}

void TemperatureSensor::init() {
    Serial.println("Initializing Temperature Sensor...");
    // ADC Configuration: Set width and attenuation
    adc1_config_width(ADC_WIDTH_BIT_12);  // Set to 12-bit resolution (0-4095)
    adc1_config_channel_atten(ADC_CHANNEL, ADC_ATTEN_DB_11);  // Set attenuation for full-scale voltage (0-3.3V)
}

int TemperatureSensor::readValue() {
    // Average multiple readings to smooth the value
    total = 0;
    for (int i = 0; i < NUM_READINGS; i++) {
        readings[i] = readADC();
        total += readings[i];
    }
    average = total / NUM_READINGS;

    // Calculate the temperature in Celsius from the averaged ADC value
    float temperatureC = calculateTemperature(average);

    // Convert the temperature to an integer value (Celsius)
    return static_cast<int>(temperatureC);
}

int TemperatureSensor::readADC() {
    // Read the ADC value from the temperature sensor (GPIO34)
    return adc1_get_raw(ADC_CHANNEL);  // Read the raw ADC value from channel 6 (GPIO34)
}

float TemperatureSensor::calculateTemperature(int adcValue) {
    // Convert the ADC value to a voltage (assuming 12-bit ADC, 3.3V reference)
    float voltage = adcValue * (3.3 / 4095.0);

    // Calculate the resistance of the thermistor
    float resistance = (3.3 / voltage - 1) * R1;

    // Calculate the temperature in Kelvin using the Beta equation
    float temperatureK = 1 / (1 / T0 + log(resistance / R0) / BETA);

    // Convert Kelvin to Celsius
    return temperatureK - 273.15;
}

const char* TemperatureSensor::getType() {
    return "TemperatureSensor";  // Return the sensor type as a string
}
