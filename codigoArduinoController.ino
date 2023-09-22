#include <LedControl.h>

const int DIN_PIN = 8;
const int CS_PIN = 9;
const int CLK_PIN = 10;
const int BUTTON_PIN = A1; // Pin del botón

LedControl lc = LedControl(DIN_PIN, CLK_PIN, CS_PIN, 1);
bool buttonState = LOW; // Estado actual del botón
bool lastButtonState = LOW; // Estado anterior del botón

void setup() {
  lc.shutdown(0, false);
  lc.setIntensity(0, 8);
  lc.clearDisplay(0);
  pinMode(BUTTON_PIN, INPUT_PULLUP); // Configura el pin del botón con pull-up
  Serial.begin(9600);
}

void loop() {
  checkButton();

  if (Serial.available()) {
    char command = Serial.read();
    handleCommand(command);
  }
}

void checkButton() {
  buttonState = digitalRead(BUTTON_PIN);

  // Verifica si el botón ha cambiado de estado
  if (buttonState != lastButtonState) {
    delay(50); // Debounce
    lastButtonState = buttonState;

    if (buttonState == LOW) { // Si el botón está presionado
      Serial.write("P"); // Envía la señal para hacer play/pause
    }
  }
}

void handleCommand(char command) {
  // Limpiamos el display
  lc.clearDisplay(0);

  switch(command) {
    case 'A':
      for (int col = 0; col < 8; col++) {
        lc.setLed(0, 0, col, true);
      }
      break;
    case 'B':
      for (int col = 0; col < 8; col++) {
        lc.setLed(0, 1, col, true);
      }
      break;
    case 'C':
      for (int col = 0; col < 8; col++) {
        lc.setLed(0, 2, col, true);
      }
      break;
    case 'D':
      for (int col = 0; col < 8; col++) {
        lc.setLed(0, 3, col, true);
      }
      break;
    case 'E':
      for (int col = 0; col < 8; col++) {
        lc.setLed(0, 4, col, true);
      }
      break;
    case 'F':
      for (int col = 0; col < 8; col++) {
        lc.setLed(0, 5, col, true);
      }
      break;
    case 'G':
      for (int col = 0; col < 8; col++) {
        lc.setLed(0, 6, col, true);
      }
      break;
    case 'H':
      for (int col = 0; col < 8; col++) {
        lc.setLed(0, 7, col, true);
      }
      break;
    case 'X':
      lc.clearDisplay(0);
      break;
  }
}
