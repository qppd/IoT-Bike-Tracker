// PinConfig.h
// Centralized pin definitions for BikeTracker project

#ifndef PINCONFIG_H
#define PINCONFIG_H

// NodeMCU v3 1.0: Define pins for software serial ports
// GPS: RX=D3, TX=D2; GSM: RX=D6, TX=D5
#define GPS_RX_PIN D3
#define GPS_TX_PIN D2
#define GSM_RX_PIN D6
#define GSM_TX_PIN D5

// Additional sensor pins
#define BUZZER_PIN D7
#define LED_STATUS_PIN D8

#endif // PINCONFIG_H
