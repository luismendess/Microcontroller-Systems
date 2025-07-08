const int led1 = 13;
const int led2 = 12;
const int led3 = 11;
const int intervalo = 200;
const int tempoAumento = 300;
const int tempoReducao = 300;
const int tempoCiclo = tempoAumento + tempoReducao;
void setup() {
  pinMode(led1, OUTPUT);
  pinMode(led2, OUTPUT);
  pinMode(led3, OUTPUT);
}
void loop() {
  unsigned long tempo = millis();
  controlarFadeLED(led1, tempo, 0);
  controlarFadeLED(led2, tempo, intervalo);
  controlarFadeLED(led3, tempo, 2 * intervalo);
}
void controlarFadeLED(int pin, unsigned long tempoAtual, int atrasoInicio) {

  unsigned long tempoRelativo = (tempoAtual - atrasoInicio) % (3 * intervalo);

  int valorPWM = 0;
  if (tempoRelativo < tempoAumento) {
    // Fase de subida
    valorPWM = map(tempoRelativo, 0,
                   tempoAumento, 0, 255);
  } else if (tempoRelativo < tempoCiclo) {
    // Fase de descida
    valorPWM = map(tempoRelativo - tempoAumento,
                   0, tempoReducao, 255, 0);
  } else {
    valorPWM = 0;
  }
  analogWrite(pin, valorPWM);
}