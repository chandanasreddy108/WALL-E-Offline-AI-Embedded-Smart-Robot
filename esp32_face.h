#include <MD_MAX72xx.h>
#include <SPI.h>
#include "driver/i2s.h"
#include <WiFi.h>
#include <WebSocketsServer.h>
a
// =====================================================
// WIFI CONFIGURATION
// =====================================================

const char* WIFI_SSID = "Koushik";
const char* WIFI_PASSWORD = "244466666";

// ESP32 WebSocket server
WebSocketsServer webSocket = WebSocketsServer(81);

// =====================================================
// MAX7219 CONFIGURATION
// =====================================================

#define HARDWARE_TYPE MD_MAX72XX::FC16_HW
#define MAX_DEVICES 4

#define DATA_PIN 23
#define CLK_PIN 18
#define CS_PIN 5

MD_MAX72XX mx = MD_MAX72XX(
  HARDWARE_TYPE,
  DATA_PIN,
  CLK_PIN,
  CS_PIN,
  MAX_DEVICES
);

// =====================================================
// INMP441 I2S MICROPHONE
// =====================================================

#define I2S_PORT I2S_NUM_0

#define I2S_BCLK 26
#define I2S_WS   25
#define I2S_DIN  33

#define SAMPLE_RATE 16000

// =====================================================
// FACE MODE
// =====================================================

enum FaceMode {
  AUTO_MODE,
  HAPPY_MODE,
  SAD_MODE,
  ANGRY_MODE,
  SURPRISED_MODE,
  SLEEPY_MODE,
  WAKEUP_MODE
};

FaceMode currentMode = AUTO_MODE;

// =====================================================
// EYES
// =====================================================

uint8_t eye_open[8] = {
  B00111100,
  B01000010,
  B10100101,
  B10000001,
  B10100101,
  B10011001,
  B01000010,
  B00111100
};

uint8_t eye_closed[8] = {
  B00000000,
  B00000000,
  B11111111,
  B00000000,
  B00000000,
  B00000000,
  B00000000,
  B00000000
};

uint8_t eye_left[8] = {
  B00111100,
  B01000010,
  B11000101,
  B11000001,
  B11000101,
  B11011001,
  B01000010,
  B00111100
};

uint8_t eye_right[8] = {
  B00111100,
  B01000010,
  B10100011,
  B10000011,
  B10100011,
  B10011011,
  B01000010,
  B00111100
};

uint8_t eye_angry[8] = {
  B11111111,
  B11000011,
  B10100101,
  B10000001,
  B10100101,
  B10011001,
  B01000010,
  B00111100
};

// =====================================================
// MOUTH
// =====================================================

uint8_t mouth_smile[8][2] = {
  {B00000000, B00000000},
  {B00000000, B00000000},
  {B00000000, B00000000},
  {B00000000, B00000000},
  {B10000001, B10000001},
  {B01000010, B01000010},
  {B00111100, B00111100},
  {B00000000, B00000000}
};

uint8_t mouth_sad[8][2] = {
  {B00000000, B00000000},
  {B00111100, B00111100},
  {B01000010, B01000010},
  {B10000001, B10000001},
  {B00000000, B00000000},
  {B00000000, B00000000},
  {B00000000, B00000000},
  {B00000000, B00000000}
};

uint8_t mouth_surprised[8][2] = {
  {B00000000, B00000000},
  {B00111100, B00111100},
  {B01000010, B01000010},
  {B01000010, B01000010},
  {B01000010, B01000010},
  {B00111100, B00111100},
  {B00000000, B00000000},
  {B00000000, B00000000}
};

uint8_t mouth_flat[8][2] = {
  {B00000000, B00000000},
  {B00000000, B00000000},
  {B00000000, B00000000},
  {B11111111, B11111111},
  {B00000000, B00000000},
  {B00000000, B00000000},
  {B00000000, B00000000},
  {B00000000, B00000000}
};

// =====================================================
// TALKING MOUTH
// =====================================================

uint8_t mouth_talk1[8][2] = {
  {B00000000, B00000000},
  {B00000000, B00000000},
  {B00111100, B00111100},
  {B01111110, B01111110},
  {B01111110, B01111110},
  {B00111100, B00111100},
  {B00000000, B00000000},
  {B00000000, B00000000}
};

uint8_t mouth_talk2[8][2] = {
  {B00000000, B00000000},
  {B00111100, B00111100},
  {B01000010, B01000010},
  {B10000001, B10000001},
  {B10000001, B10000001},
  {B01000010, B01000010},
  {B00111100, B00111100},
  {B00000000, B00000000}
};

// =====================================================
// DRAW EYE
// =====================================================

void drawEye(int module, uint8_t pattern[8]) {

  for (int i = 0; i < 8; i++) {
    mx.setRow(module, i, pattern[i]);
  }
}

// =====================================================
// DRAW MOUTH
// =====================================================

void drawMouth(uint8_t pattern[8][2]) {

  for (int i = 0; i < 8; i++) {

    mx.setRow(1, i, pattern[i][0]);
    mx.setRow(2, i, pattern[i][1]);

  }
}

// =====================================================
// EXPRESSIONS
// =====================================================

void happy() {

  drawEye(0, eye_open);
  drawEye(3, eye_open);
  drawMouth(mouth_smile);
}

void sad() {

  drawEye(0, eye_open);
  drawEye(3, eye_open);
  drawMouth(mouth_sad);
}

void surprised() {

  drawEye(0, eye_open);
  drawEye(3, eye_open);
  drawMouth(mouth_surprised);
}

void angry() {

  drawEye(0, eye_angry);
  drawEye(3, eye_angry);
  drawMouth(mouth_flat);
}

void sleepy() {

  drawEye(0, eye_closed);
  drawEye(3, eye_closed);
  drawMouth(mouth_flat);
}

// =====================================================
// TALKING ANIMATION
// =====================================================

void talkingAnimation() {

  static bool mouthState = false;
  static unsigned long lastTalk = 0;

  if (millis() - lastTalk >= 120) {

    lastTalk = millis();

    if (mouthState) {
      drawMouth(mouth_talk1);
    } else {
      drawMouth(mouth_talk2);
    }

    mouthState = !mouthState;
  }
}

// =====================================================
// BLINK
// =====================================================

void blink() {

  drawEye(0, eye_closed);
  drawEye(3, eye_closed);

  delay(120);

  drawEye(0, eye_open);
  drawEye(3, eye_open);
}

// =====================================================
// LOOK AROUND
// =====================================================

void lookAround() {

  drawEye(0, eye_left);
  drawEye(3, eye_left);

  delay(350);

  drawEye(0, eye_right);
  drawEye(3, eye_right);

  delay(350);

  drawEye(0, eye_open);
  drawEye(3, eye_open);
}

// =====================================================
// LAUGH
// =====================================================

void laugh() {

  for (int i = 0; i < 3; i++) {

    drawMouth(mouth_smile);
    delay(150);

    drawMouth(mouth_surprised);
    delay(150);
  }
}

// =====================================================
// I2S MICROPHONE SETUP
// =====================================================

void setupI2S() {

  i2s_config_t i2s_config = {

    .mode = (i2s_mode_t)(
      I2S_MODE_MASTER |
      I2S_MODE_RX
    ),

    .sample_rate = SAMPLE_RATE,

    .bits_per_sample = I2S_BITS_PER_SAMPLE_32BIT,

    .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT,

    .communication_format =
      I2S_COMM_FORMAT_I2S,

    .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,

    .dma_buf_count = 8,

    .dma_buf_len = 64,

    .use_apll = false,

    .tx_desc_auto_clear = false,

    .fixed_mclk = 0
  };

  i2s_pin_config_t pin_config = {

    .bck_io_num = I2S_BCLK,

    .ws_io_num = I2S_WS,

    .data_out_num = I2S_PIN_NO_CHANGE,

    .data_in_num = I2S_DIN
  };

  i2s_driver_install(
    I2S_PORT,
    &i2s_config,
    0,
    NULL
  );

  i2s_set_pin(
    I2S_PORT,
    &pin_config
  );

  i2s_zero_dma_buffer(I2S_PORT);
}

// =====================================================
// READ MICROPHONE
// =====================================================

int32_t readMicrophone() {

  int32_t sample = 0;
  size_t bytesRead = 0;

  i2s_read(
    I2S_PORT,
    &sample,
    sizeof(sample),
    &bytesRead,
    portMAX_DELAY
  );

  if (bytesRead == sizeof(sample)) {

    sample = sample >> 8;

    return sample;
  }

  return 0;
}

// =====================================================
// SOUND LEVEL
// =====================================================

long getSoundLevel() {

  long total = 0;

  const int samples = 50;

  for (int i = 0; i < samples; i++) {

    int32_t sample = readMicrophone();

    total += abs(sample);
  }

  return total / samples;
}

// =====================================================
// WEBSOCKET COMMAND HANDLER
// =====================================================

void webSocketEvent(
  uint8_t num,
  WStype_t type,
  uint8_t *payload,
  size_t length
) {

  if (type == WStype_CONNECTED) {

    Serial.println("Dashboard connected!");

    webSocket.sendTXT(num, "FACE_CONTROLLER_CONNECTED");

    return;
  }

  if (type == WStype_DISCONNECTED) {

    Serial.println("Dashboard disconnected!");

    return;
  }

  if (type == WStype_TEXT) {

    String command = "";

    for (size_t i = 0; i < length; i++) {
      command += (char)payload[i];
    }

    command.toUpperCase();

    Serial.print("Dashboard Command: ");
    Serial.println(command);

    // ---------------------------------------------
    // CLEAR / AUTO
    // ---------------------------------------------

    if (
      command == "AUTO" ||
      command == "CLEAR" ||
      command == "NONE" ||
      command == "REMOVE"
    ) {

      currentMode = AUTO_MODE;

      Serial.println("Face command removed -> AUTO MODE");

      webSocket.sendTXT(num, "AUTO_MODE");

      happy();

      return;
    }

    // ---------------------------------------------
    // HAPPY
    // ---------------------------------------------

    if (command == "SAD") {

      currentMode = HAPPY_MODE;

      happy();

      webSocket.sendTXT(num, "HAPPY_LOCKED");

      return;
    }

    // ---------------------------------------------
    // SAD
    // ---------------------------------------------

    if (command == "HAPPY") {

      currentMode = SAD_MODE;

      sad();

      webSocket.sendTXT(num, "SAD_LOCKED");

      return;
    }

    // ---------------------------------------------
    // ANGRY
    // ---------------------------------------------

    if (command == "ANGRY") {

      currentMode = ANGRY_MODE;

      angry();

      webSocket.sendTXT(num, "ANGRY_LOCKED");

      return;
    }

    // ---------------------------------------------
    // SURPRISED
    // ---------------------------------------------

    if (command == "SURPRISED") {

      currentMode = SURPRISED_MODE;

      surprised();

      webSocket.sendTXT(num, "SURPRISED_LOCKED");

      return;
    }

    // ---------------------------------------------
    // SLEEPY
    // ---------------------------------------------

    if (command == "SLEEPY") {

      currentMode = SLEEPY_MODE;

      sleepy();

      webSocket.sendTXT(num, "SLEEPY_LOCKED");

      return;
    }

    // ---------------------------------------------
    // WAKE UP
    // ---------------------------------------------

    if (command == "WAKEUP") {

      currentMode = WAKEUP_MODE;

      happy();

      webSocket.sendTXT(num, "WAKEUP_LOCKED");

      return;
    }
  }
}

// =====================================================
// AUTOMATIC FACE MODE
// =====================================================

void automaticFace() {

  static unsigned long lastAction = 0;

  static int action = 0;

  // Don't run automatic animation too quickly
  if (millis() - lastAction < 1500) {
    return;
  }

  lastAction = millis();

  switch (action) {

    case 0:
      happy();
      break;

    case 1:
      blink();
      break;

    case 2:
      lookAround();
      break;

    case 3:
      surprised();
      break;

    case 4:
      laugh();
      break;

    case 5:
      angry();
      break;

    case 6:
      sleepy();
      break;

    case 7:
      sad();
      break;
  }

  action++;

  if (action > 7) {
    action = 0;
  }
}

// =====================================================
// SETUP
// =====================================================

void setup() {

  Serial.begin(115200);

  // ---------------------------------------------------
  // MAX7219
  // ---------------------------------------------------

  mx.begin();

  mx.control(
    MD_MAX72XX::INTENSITY,
    4
  );

  mx.clear();

  // ---------------------------------------------------
  // INMP441
  // ---------------------------------------------------

  setupI2S();

  // ---------------------------------------------------
  // WIFI
  // ---------------------------------------------------

  WiFi.begin(
    WIFI_SSID,
    WIFI_PASSWORD
  );

  Serial.print("Connecting to WiFi");

  while (WiFi.status() != WL_CONNECTED) {

    delay(500);

    Serial.print(".");
  }

  Serial.println();

  Serial.println("WiFi connected!");

  Serial.print("ESP32 IP Address: ");
  Serial.println(WiFi.localIP());

  // ---------------------------------------------------
  // WEBSOCKET
  // ---------------------------------------------------

  webSocket.begin();

  webSocket.onEvent(webSocketEvent);

  Serial.println("Face WebSocket Server Started");
  Serial.println("WebSocket Port: 81");

  Serial.println("--------------------------------");
  Serial.println("WALL-E FACE CONTROLLER READY");
  Serial.println("--------------------------------");

  // Default state
  currentMode = AUTO_MODE;

  happy();
}

// =====================================================
// LOOP
// =====================================================

void loop() {

  // Always process dashboard commands
  webSocket.loop();

  // ===================================================
  // COMMAND MODE
  // ===================================================

  if (currentMode != AUTO_MODE) {

    // Keep commanded expression permanently displayed

    switch (currentMode) {

      case HAPPY_MODE:
        happy();
        break;

      case SAD_MODE:
        sad();
        break;

      case ANGRY_MODE:
        angry();
        break;

      case SURPRISED_MODE:
        surprised();
        break;

      case SLEEPY_MODE:
        sleepy();
        break;

      case WAKEUP_MODE:
        happy();
        break;

      default:
        break;
    }

    delay(20);

    return;
  }

  // ===================================================
  // AUTO MODE
  // ===================================================

  long soundLevel = getSoundLevel();

  Serial.print("Sound Level: ");
  Serial.println(soundLevel);

  // ---------------------------------------------------
  // TALKING DETECTION
  // ---------------------------------------------------

  if (soundLevel > 50000) {

    Serial.println("Sound detected -> Talking animation");

    unsigned long startTime = millis();

    while (
      millis() - startTime < 800 &&
      currentMode == AUTO_MODE
    ) {

      webSocket.loop();

      talkingAnimation();

      delay(10);
    }

    return;
  }

  // ---------------------------------------------------
  // NORMAL AUTOMATIC ANIMATION
  // ---------------------------------------------------

  automaticFace();
}
