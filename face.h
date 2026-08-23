#include <MD_MAX72xx.h>
#include <SPI.h>

#define HARDWARE_TYPE MD_MAX72XX::FC16_HW
#define MAX_DEVICES 4

#define DATA_PIN 11
#define CLK_PIN 13
#define CS_PIN 10

MD_MAX72XX mx = MD_MAX72XX(HARDWARE_TYPE, DATA_PIN, CLK_PIN, CS_PIN, MAX_DEVICES);

// ===== EYES =====
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

// looking left
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

// looking right
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

// angry eyes
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

// ===== MOUTH =====
uint8_t mouth_smile[8][2] = {
  {B00000000,B00000000},
  {B00000000,B00000000},
  {B00000000,B00000000},
  {B00000000,B00000000},
  {B10000001,B10000001},
  {B01000010,B01000010},
  {B00111100,B00111100},
  {B00000000,B00000000}
};

uint8_t mouth_sad[8][2] = {
  {B00000000,B00000000},
  {B00111100,B00111100},
  {B01000010,B01000010},
  {B10000001,B10000001},
  {B00000000,B00000000},
  {B00000000,B00000000},
  {B00000000,B00000000},
  {B00000000,B00000000}
};

// surprised (open mouth)
uint8_t mouth_surprised[8][2] = {
  {B00000000,B00000000},
  {B00111100,B00111100},
  {B01000010,B01000010},
  {B01000010,B01000010},
  {B01000010,B01000010},
  {B00111100,B00111100},
  {B00000000,B00000000},
  {B00000000,B00000000}
};

// flat (neutral)
uint8_t mouth_flat[8][2] = {
  {B00000000,B00000000},
  {B00000000,B00000000},
  {B00000000,B00000000},
  {B11111111,B11111111},
  {B00000000,B00000000},
  {B00000000,B00000000},
  {B00000000,B00000000},
  {B00000000,B00000000}
};

// ===== DRAW FUNCTIONS =====
void drawEye(int module, uint8_t pattern[8]) {
  for (int i = 0; i < 8; i++) {
    mx.setRow(module, i, pattern[i]);
  }
}

void drawMouth(uint8_t pattern[8][2]) {
  for (int i = 0; i < 8; i++) {
    mx.setRow(1, i, pattern[i][0]);
    mx.setRow(2, i, pattern[i][1]);
  }
}

// ===== EXPRESSIONS =====
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

// animation: look around
void lookAround() {
  drawEye(0, eye_left);
  drawEye(3, eye_left);
  delay(500);

  drawEye(0, eye_right);
  drawEye(3, eye_right);
  delay(500);
}

// animation: laugh
void laugh() {
  for (int i = 0; i < 3; i++) {
    drawMouth(mouth_smile);
    delay(200);
    drawMouth(mouth_surprised);
    delay(200);
  }
}

// blink
void blink() {
  drawEye(0, eye_closed);
  drawEye(3, eye_closed);
  delay(200);

  drawEye(0, eye_open);
  drawEye(3, eye_open);
}

// ===== SETUP =====
void setup() {
  mx.begin();
  mx.control(MD_MAX72XX::INTENSITY, 4);
  mx.clear();
}

// ===== LOOP =====
void loop() {

  happy();
  delay(2000);

  blink();

  lookAround();

  surprised();
  delay(1500);

  laugh();

  angry();
  delay(2000);

  sleepy();
  delay(2000);

  sad();
  delay(2000);
}
