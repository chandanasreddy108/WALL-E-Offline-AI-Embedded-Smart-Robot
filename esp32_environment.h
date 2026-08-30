#include <Wire.h>
#include <DHT.h>
#include <RTClib.h>
#include <TinyGPS++.h>

// Sensor Pin Definitions
#define DHTPIN        4
#define DHTTYPE       DHT22
#define GAS_PIN       34
#define RAIN_PIN      35

// Serial Port Configurations
#define GPS_RX_PIN    16
#define GPS_TX_PIN    17
#define ARDUINO_TX    18
#define ARDUINO_RX    19

DHT dht(DHTPIN, DHTTYPE);
RTC_DS3231 rtc;
TinyGPSPlus gps;

HardwareSerial SerialGPS(2);
HardwareSerial SerialToUno(1);

unsigned long lastSampleTime = 0;
const unsigned long sampleInterval = 2000; // Send telemetry every 2 seconds

void setup() {
  Serial.begin(115200);
  SerialGPS.begin(9600, SERIAL_8N1, GPS_RX_PIN, GPS_TX_PIN);
  SerialToUno.begin(9600, SERIAL_8N1, ARDUINO_RX, ARDUINO_TX);

  dht.begin();
  Wire.begin(21, 22);
  
  if (!rtc.begin()) {
    Serial.println("RTC initialization failed!");
  }
}

void loop() {
  // Continuously feed incoming NMEA sentences from the GPS module
  while (SerialGPS.available() > 0) {
    gps.encode(SerialGPS.read());
  }

  unsigned long currentMillis = millis();
  if (currentMillis - lastSampleTime >= sampleInterval) {
    lastSampleTime = currentMillis;

    // 1. Environmental Readings
    float temp = dht.readTemperature();
    float hum = dht.readHumidity();
    if (isnan(temp)) temp = 0.0;
    if (isnan(hum)) hum = 0.0;

    int gasVal = analogRead(GAS_PIN);
    int gasPercent = map(gasVal, 0, 4095, 0, 100);

    int rainVal = analogRead(RAIN_PIN);
    int rainPercent = map(rainVal, 4095, 0, 0, 100); // Inverted: lower analog = higher moisture

    // 2. Real-Time Clock Acquisition
    DateTime now = rtc.now();
    char timeBuffer[10];
    sprintf(timeBuffer, "%02d:%02d:%02d", now.hour(), now.minute(), now.second());

    // 3. GPS Coordinate Tracking
    float latitude = gps.location.isValid() ? gps.location.lat() : 0.00000;
    float longitude = gps.location.isValid() ? gps.location.lng() : 0.00000;

    // 4. Construct Delimited Data Packet: <TIME,TEMP,HUM,GAS,RAIN,LAT,LNG>
    String packet = "<" + String(timeBuffer) + "," +
                    String(temp, 1) + "," +
                    String(hum, 1) + "," +
                    String(gasPercent) + "," +
                    String(rainPercent) + "," +
                    String(latitude, 4) + "," +
                    String(longitude, 4) + ">";

    // Transmit to Arduino Uno & Debug Port
    SerialToUno.println(packet);
    Serial.println("Dispatched: " + packet);
  }
}
