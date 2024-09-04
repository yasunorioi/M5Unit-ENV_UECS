/**
 * @file ENV.ino
 * @author SeanKwok (shaoxiang@m5stack.com)
 * @brief
 * @version 0.1
 * @date 2024-01-30
 *
 *
 * @Hardwares: M5Core + Unit ENV
 * @Platform Version: Arduino M5Stack Board Manager v2.1.0
 * @Dependent Library:
 * M5UnitENV: https://github.com/m5stack/M5Unit-ENV
 */

#include "M5UnitENV.h"
#include <M5Atom.h>
#include <WiFi.h>
#include <WiFiMulti.h>
#include <WiFiUdp.h>
#include "config.h"
DHT12 dht;
BMP280 bmp;
WiFiUDP udp;

const char *ssid     = SSID;
const char *password = PASSWORD;
const IPAddress ip(192, 168, 1, 74);
const IPAddress gateway(192, 168, 1, 70);
const IPAddress subnet(255, 255, 255, 0);
const IPAddress dns1(192, 168, 1, 70);
const String ccmname ="test";
const String ccmtype ="cMC";
const String room = "1";
const String region = "4";
const String order = "1";
const String priority = "2";
const String host = "192.168.1.70";
const int port = 16520;
const IPAddress castaddress(255,255,255,255);
const String dhtcTemp = "dhtcTemp";
const String dhthumidity = "dhthumidity";
const String bmpcTemp = "bmpcTemp";
const String bmppressure = "bmppressure";
const String bmpaltitude = "bmpaltitude";

WiFiMulti WiFiMulti;

void setup() {
    M5.begin(true, false, true);
    pinMode(0,OUTPUT);
    digitalWrite(0,LOW);
    Serial.begin(115200);
    WiFiMulti.addAP(
      ssid,
      password);
    if (!WiFi.config(ip,gateway,subnet,dns1)){
      Serial.println("Failed to configure!");
    }
    M5.dis.fillpix(0xff0000);
    while (WiFiMulti.run() !=
     WL_CONNECTED) {
        Serial.print(".");
        delay(300);
    }
    M5.dis.fillpix(0x00ff00);
    Serial.println("\nWiFi connected");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());

    if (!dht.begin(&Wire, DHT12_I2C_ADDR, 26, 32, 400000U)) {
        Serial.println("Couldn't find DHT12");
        while (1) delay(1);
    }
    if (!bmp.begin(&Wire, BMP280_I2C_ADDR, 26, 32, 400000U)) {
        Serial.println("Couldn't find BMP280");
        while (1) delay(1);
    }
    /* Default settings from datasheet. */
    bmp.setSampling(BMP280::MODE_NORMAL,     /* Operating Mode. */
                    BMP280::SAMPLING_X2,     /* Temp. oversampling */
                    BMP280::SAMPLING_X16,    /* Pressure oversampling */
                    BMP280::FILTER_X16,      /* Filtering. */
                    BMP280::STANDBY_MS_500); /* Standby time. */
}

void loop() {
    if (dht.update()) {
        CCM_send(dhtcTemp,dht.cTemp);
        CCM_send(dhthumidity,dht.humidity);
    }

    if (bmp.update()) {
        CCM_send(bmpcTemp,bmp.cTemp);
        CCM_send(bmppressure,bmp.pressure);
        CCM_send(bmpaltitude,bmp.altitude);
    }
    delay(1000);
}

int CCM_send (String sensorname,int val){
   String msg;
   msg  = "<?xml version=\"1.0\"?>\r\n<UECS ver=\"1.00-E10\">\r\n";
   msg += "<DATA type=\"" + ccmname + sensorname + "." + ccmtype + "\" ";
   msg += "room=\"" + room + "\" ";
   msg += "region=\"" + region + "\" ";
   msg += "order=\"" + order + "\" ";
   msg += "priority=\"" + priority + "\">\r\n";
   msg += val;
   msg += "\r\n</DATA>\r\n";
   msg += "<IP>"+host+"</IP></UECS>";
   Serial.println(msg);
   udp.beginPacket(castaddress,port);
   udp.println(msg);
   udp.endPacket();
   delay(1000);
  int n = Serial.println(msg);
return n;
}
