const byte ledPin   = 11;       // pino PWM para o LED
const byte potPin   = A0;       // entrada analógica do potenciômetro
const unsigned long cycle = 600; // 600 ms para um ciclo completo (300ms subir + 300ms descer)
unsigned long startTime;

void setup() {
  pinMode(ledPin, OUTPUT);
  Serial.begin(9600);
  startTime = millis();
}

void loop() {
  unsigned long now = millis();
  unsigned long t = (now - startTime) % cycle;

  // Cálculo do PWM do LED usando map():
  int pwm;
  if (t < cycle/2) {
    // 0 → 255 em 300ms
    pwm = map(t, 0, cycle/2, 0, 255);
  } else {
    // 255 → 0 em 300ms
    pwm = map(t, cycle/2, cycle, 255, 0);
  }
  analogWrite(ledPin, pwm);

  // Leitura do potenciômetro e envio pela Serial
  int potValue = analogRead(potPin); 
  Serial.println(potValue);

  // Pequena pausa para não inundar a Serial (opcional)
  delay(100);
}
