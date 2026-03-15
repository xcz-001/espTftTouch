//THIS EXAMPLE ALLOWS YOU TO WRITE YELLOW DOTS ON THE SCREEN UPON TOUCHE
// ALSO HAVE AN ERASE ALL BUTTON


#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>
#include <XPT2046_Touchscreen.h>

// Display pins
#define TFT_CS    5
#define TFT_RST   27
#define TFT_DC    21
#define TFT_MOSI  13
#define TFT_CLK   14
#define TFT_LED   25
#define TFT_MISO  12

// Touch pins
#define TOUCH_CS  26
#define TOUCH_DIN 23
#define TOUCH_DO  19
#define TOUCH_CLK 18

Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_MOSI, TFT_CLK, TFT_RST, TFT_MISO);
XPT2046_Touchscreen ts(TOUCH_CS);

#define ERASE_BTN_X 200
#define ERASE_BTN_Y 0
#define ERASE_BTN_W 120
#define ERASE_BTN_H 40

bool calibrated = false;
int xMinVal, xMaxVal, yMinVal, yMaxVal;

void drawCross(int x, int y) {
  tft.fillCircle(x, y, 8, ILI9341_RED);
  tft.drawLine(x - 15, y, x + 15, y, ILI9341_RED);
  tft.drawLine(x, y - 15, x, y + 15, ILI9341_RED);
  delay(500);
}

TS_Point getTouchPoint() {
  while (!ts.touched()) delay(10);
  TS_Point p = ts.getPoint();
  while (ts.touched()) delay(10);
  return p;
}

void calibrateScreen() {
  tft.fillScreen(ILI9341_BLACK);
  tft.setCursor(20, 140);
  tft.setTextColor(ILI9341_WHITE);
  tft.setTextSize(2);
  tft.print("Touch the corners");

  // Corner 1: top left
  drawCross(20, 20);
  TS_Point p = getTouchPoint();
  xMinVal = p.x;
  yMinVal = p.y;

  // Corner 2: top right
  drawCross(tft.width()-20, 20);
  p = getTouchPoint();
  xMaxVal = p.x;

  // Corner 3: bottom left
  drawCross(20, tft.height()-20);
  p = getTouchPoint();
  yMaxVal = p.y;

  // Maybe adjust minY & maxX depending on orientation
  tft.fillScreen(ILI9341_BLACK);
  calibrated = true;
}

void drawEraseBtn() {
  tft.fillRect(ERASE_BTN_X, ERASE_BTN_Y, ERASE_BTN_W, ERASE_BTN_H, ILI9341_BLUE);
  tft.setCursor(ERASE_BTN_X+10, ERASE_BTN_Y+10);
  tft.setTextColor(ILI9341_BLACK);
  tft.setTextSize(2);
  tft.print("ERASE ALL");
}

void setup() {
  Serial.begin(115200);
  pinMode(TFT_LED, OUTPUT);
  digitalWrite(TFT_LED, HIGH);

  tft.begin();
  ts.begin();
  ts.setRotation(1);
  tft.setRotation(1);

  drawEraseBtn();
  calibrateScreen();
}

void loop() {
  if (!ts.touched()) return;

  TS_Point raw = ts.getPoint();

  // Map raw to screen
  int sx = map(raw.x, xMinVal, xMaxVal, 0, tft.width());
  int sy = map(raw.y, yMinVal, yMaxVal, 0, tft.height());

  // Bound check
  if (sx < 0) sx = 0;
  if (sy < 0) sy = 0;
  if (sx > tft.width())  sx = tft.width();
  if (sy > tft.height()) sy = tft.height();

  // Erase button
  if (sx >= ERASE_BTN_X && sx <= ERASE_BTN_X+ERASE_BTN_W &&
      sy >= ERASE_BTN_Y && sy <= ERASE_BTN_Y+ERASE_BTN_H) {
    tft.fillScreen(ILI9341_BLACK);
    drawEraseBtn();
    delay(200);
    return;
  }

  // Draw point
  tft.fillCircle(sx, sy, 2, ILI9341_YELLOW);

  delay(10);
}