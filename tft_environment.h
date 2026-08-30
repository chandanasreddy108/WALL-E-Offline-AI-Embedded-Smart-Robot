#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>
#include <SPI.h>

#define TFT_CS   10
#define TFT_RST  8
#define TFT_DC   9

Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);

String inputBuffer = "";
bool receivingData = false;

void drawDashboardLayout() {
  tft.fillScreen(ST77XX_BLACK);
  tft.setTextColor(ST77XX_CYAN);
  tft.setTextSize(1);
  tft.setCursor(10, 5);
  tft.println("ENVIRONMENT MONITOR");
  tft.drawFastHLine(0, 16, 128, ST77XX_WHITE);

  tft.setTextColor(ST77XX_YELLOW);
  tft.setCursor(5, 22);  tft.print("Time: ");
  tft.setCursor(5, 36);  tft.print("Temp: ");
  tft.setCursor(5, 50);  tft.print("Hum : ");
  tft.setCursor(5, 64);  tft.print("Gas : ");
  tft.setCursor(5, 78);  tft.print("Rain: ");
  tft.setCursor(5, 96);  tft.print("Lat : ");
  tft.setCursor(5, 110); tft.print("Lng : ");
  tft.drawFastHLine(0, 90, 128, ST77XX_DARKGREY);
}

void renderTelemetry(String timeStr, String temp, String hum, String gas, String rain, String lat, String lng) {
  tft.setTextSize(1);
  tft.setTextColor(ST77XX_WHITE, ST77XX_BLACK);

  tft.setCursor(42, 22);  tft.print(timeStr + "   ");
  tft.setCursor(42, 36);  tft.print(temp + " C   ");
  tft.setCursor(42, 50);  tft.print(hum + " %   ");
  tft.setCursor(42, 64);  tft.print(gas + " %   ");
  tft.setCursor(42, 78);  tft.print(rain + " %   ");
  tft.setCursor(42, 96);  tft.print(lat + "  ");
  tft.setCursor(42, 110); tft.print(lng + "  ");
}

void parseAndRender(String rawData) {
  // Extract values between comma delimiters
  int idx1 = rawData.indexOf(',');
  int idx2 = rawData.indexOf(',', idx1 + 1);
  int idx3 = rawData.indexOf(',', idx2 + 1);
  int idx4 = rawData.indexOf(',', idx3 + 1);
  int idx5 = rawData.indexOf(',', idx4 + 1);
  int idx6 = rawData.indexOf(',', idx5 + 1);

  if (idx1 > 0 && idx2 > 0 && idx3 > 0 && idx4 > 0 && idx5 > 0 && idx6 > 0) {
    String timeVal = rawData.substring(0, idx1);
    String tempVal = rawData.substring(idx1 + 1, idx2);
    String humVal  = rawData.substring(idx2 + 1, idx3);
    String gasVal  = rawData.substring(idx3 + 1, idx4);
    String rainVal = rawData.substring(idx4 + 1, idx5);
    String latVal  = rawData.substring(idx5 + 1, idx6);
    String lngVal  = rawData.substring(idx6 + 1);

    renderTelemetry(timeVal, tempVal, humVal, gasVal, rainVal, latVal, lngVal);
  }
}

void setup() {
  Serial.begin(9600);
  tft.initR(INITR_BLACKTAB); // Initialize ST7735 128x160 display
  tft.setRotation(1);
  drawDashboardLayout();
}

void loop() {
  // UART packet receiver (<...>)
  while (Serial.available() > 0) {
    char inChar = (char)Serial.read();

    if (inChar == '<') {
      inputBuffer = "";
      receivingData = true;
    } else if (inChar == '>') {
      receivingData = false;
      parseAndRender(inputBuffer);
    } else if (receivingData) {
      inputBuffer += inChar;
    }
  }
}
