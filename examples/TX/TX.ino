/*
   F4GMU PAGER bi-directional
   december 2017
   TX
*/

#include <Arduino.h>
#include <cc1101.h>
#include <ccpacket.h>
//OLED
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// Attach CC1101 pins to their corresponding SPI pins
// Uno pins:
// CSN (SS) => 10
// MOSI => 11
// MISO => 12
// SCK => 13
// GD0 => 2

#define CC1101Interrupt 0 // Pin 2
#define CC1101_GDO0 2

#define OLED_RESET 4
Adafruit_SSD1306 display(OLED_RESET);

CC1101 radio;

byte syncWord[2] = {199, 10};
bool packetWaiting;

unsigned long lastSend = 0;
unsigned int sendDelay = 5000; // send text every 5 secondes

void messageReceived() {
  packetWaiting = true;
}

void setup() {
  Serial.begin(9600);
  radio.init();
  radio.setSyncWord(syncWord);
  radio.setCarrierFreq(CFREQ_433);
  radio.disableAddressCheck();
  radio.setTxPowerAmp(PA_LongDistance);

  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // initialize with the I2C addr 0x3C (for the 128x64)

  Serial.print(F("CC1101_PARTNUM "));
  Serial.println(radio.readReg(CC1101_PARTNUM, CC1101_STATUS_REGISTER));
  Serial.print(F("CC1101_VERSION "));
  Serial.println(radio.readReg(CC1101_VERSION, CC1101_STATUS_REGISTER));
  Serial.print(F("CC1101_MARCSTATE "));
  Serial.println(radio.readReg(CC1101_MARCSTATE, CC1101_STATUS_REGISTER) & 0x1f);

  Serial.println(F("CC1101 radio initialized."));
  attachInterrupt(CC1101Interrupt, messageReceived, FALLING);
}

// Get signal strength indicator in dBm.
// See: http://www.ti.com/lit/an/swra114d/swra114d.pdf
int rssi(char raw) {
  uint8_t rssi_dec;
  // This rssi_offset is dependent on baud and Freq: this is for 38.4kbps and 433 MHz.
  uint8_t rssi_offset = 74;
  rssi_dec = (uint8_t) raw;
  if (rssi_dec >= 128)
    return ((int)( rssi_dec - 256) / 2) - rssi_offset;
  else
    return (rssi_dec / 2) - rssi_offset;
}

// Get link quality.
int lqi(char raw) {
  return 0x3F - raw;
}

void loop() {
  display.display(); //display logo screen
  delay(1000);
  display.clearDisplay();
  display.setCursor(0, 0);
  display.println("*F4GMU PAGER CC1101*");
  if (packetWaiting) {
    detachInterrupt(CC1101Interrupt);
    packetWaiting = false;
    CCPACKET packet;
    if (radio.receiveData(&packet) > 0) {
      Serial.println(F("Received packet..."));
      if (!packet.crc_ok) {
        Serial.println(F("crc not ok"));
      }
      Serial.print(F("lqi: "));
      Serial.println(lqi(packet.lqi));
      Serial.print(F("rssi: "));
      Serial.print(rssi(packet.rssi));
      Serial.println(F("dBm"));

      if (packet.crc_ok && packet.length > 0) {
        Serial.print(F("packet: len "));
        Serial.println(packet.length);
        Serial.println(F("data: "));
        Serial.println((const char *) packet.data);
        display.setTextSize(1);
        display.setTextColor(WHITE);
        display.setCursor(20, 20);
        display.print("RSSI : ");
        display.print(rssi(packet.rssi));
        display.println("dBm");
        display.setCursor(0, 30);
        display.println((const char *) packet.data);
        display.display();
        delay(5000); //delay text display
      }
    }

    attachInterrupt(CC1101Interrupt, messageReceived, FALLING);
  }
  unsigned long now = millis();
  if (now > lastSend + sendDelay) {
    detachInterrupt(CC1101Interrupt);

    lastSend = now;
    const char *message = "HELLO WORLD, ceci est un test de transmission AT328 + CC1101"; //text to send. TODO : Serial.read
    CCPACKET packet;
    // We also need to include the 0 byte at the end of the string
    packet.length = strlen(message)  + 1;
    strncpy((char *) packet.data, message, packet.length);

    radio.sendData(packet);
    Serial.println(F("Sent packet..."));

    attachInterrupt(CC1101Interrupt, messageReceived, FALLING);
  }
}
