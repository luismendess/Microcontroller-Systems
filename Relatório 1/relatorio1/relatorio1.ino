const int segmentPins[7] = { 22, 24, 26, 28, 30, 32, 34 };
const int displayPins[2] = { 42, 40 };
const int potPin = A0;
const byte digitPatterns[10] = {
  B1111110,  // Digito 0
  B0110000,  // Digito 1
  B1101101,  // Digito 2
  B1111001,  // Digito 3
  B0110011,  // Digito 4
  B1011011,  // Digito 5
  B1011111,  // Digito 6
  B1110000,  // Digito 7
  B1111111,  // Digito 8
  B1111011   // Digito 9
};
void setup() {
  for (int i = 0; i < 7; i++) {
    pinMode(segmentPins[i], OUTPUT);
  }
  for (int i = 0; i < 2; i++) {
    pinMode(displayPins[i], OUTPUT);
    digitalWrite(displayPins[i], HIGH);
  }
  Serial.begin(9600);
}
void loop() {
  int potValue = analogRead(potPin);
  Serial.print("Valor do potenciometro: ");
  Serial.print(potValue);
  int displayValue = map(potValue, 0, 1023, 0, 99);
  Serial.print(" | Valor ajustado (0-99): ");
  Serial.println(displayValue);
  int tens = displayValue / 10;
  int units = displayValue % 10;
  for (int refresh = 0; refresh < 20; refresh++) {
    displayDigit(0, tens);
    delay(5);
    displayDigit(1, units);
    delay(5);
  }
}
void displayDigit(int displayIndex, int digit) {
  for (int i = 0; i < 2; i++) {
    digitalWrite(displayPins[i], HIGH);
  }
  for (int i = 0; i < 7; i++) {
    bool segmentOn = bitRead(digitPatterns[digit], 6 - i);
    digitalWrite(segmentPins[i], segmentOn ? HIGH
                                           : LOW);
  }
  digitalWrite(displayPins[displayIndex], LOW);
}