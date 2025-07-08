#include <SPI.h>
#include <MFRC522.h>
#include <Servo.h>
#include <LiquidCrystal.h>

// --- Protótipos ---
void atualizarDisplayPrincipal();
void verificarEntradaPorTag();
void verificarSaida();
void acionarCancelaComPulso(int ledPin);

// --- PINOS ---
#define RFID_SS_PIN      10
#define RFID_RST_PIN     9
#define SERVO_PIN        A0
#define SENSOR_SAIDA_PIN 2

#define LCD_RS 6
#define LCD_E  5
#define LCD_D4 4
#define LCD_D5 3
#define LCD_D6 8
#define LCD_D7 7

#define LED_ENTRADA A1
#define LED_SAIDA   A2

// --- CONFIGURAÇÕES ---
const int TOTAL_VAGAS        = 10;
const unsigned long DEBOUNCE = 2500;
const unsigned long PULSO_MS = 2000;
const unsigned long MOV_MS   = 1500;

String UID_AUTORIZADA = "BC 09 0A 4A";
int vagasDisponiveis   = TOTAL_VAGAS;
unsigned long lastRFID = 0;
String lastUID        = "";

MFRC522 rfid(RFID_SS_PIN, RFID_RST_PIN);
Servo cancela;
LiquidCrystal lcd(LCD_RS, LCD_E, LCD_D4, LCD_D5, LCD_D6, LCD_D7);

void setup() {
  Serial.begin(9600);
  SPI.begin();
  rfid.PCD_Init();

  // LCD
  lcd.begin(16, 2);
  lcd.clear();
  lcd.print("Estacionamento");
  lcd.setCursor(0, 1);
  lcd.print("Iniciando...");
  delay(500);

  // Servo fechado
  cancela.attach(SERVO_PIN);
  cancela.write(0);

  // LEDs start OFF (pino HIGH -> LED apaga no seu esquema)
  pinMode(LED_ENTRADA, OUTPUT);
  pinMode(LED_SAIDA,   OUTPUT);
  digitalWrite(LED_ENTRADA, HIGH);
  digitalWrite(LED_SAIDA,   HIGH);

  // Sensor
  pinMode(SENSOR_SAIDA_PIN, INPUT);

  atualizarDisplayPrincipal();
  Serial.println("Sistema Pronto!");
}

void loop() {
  verificarSaida();
  verificarEntradaPorTag();
}

// ENTRADA por RFID
void verificarEntradaPorTag() {
  if (!rfid.PICC_IsNewCardPresent() || !rfid.PICC_ReadCardSerial()) return;

  // monta string UID
  String uid = "";
  for (byte i=0; i<rfid.uid.size; i++) {
    uid += (rfid.uid.uidByte[i]<0x10 ? " 0" : " ");
    uid += String(rfid.uid.uidByte[i], HEX);
  }
  uid.toUpperCase();
  uid.remove(0, 1);

  unsigned long now = millis();
  if (uid==lastUID && now-lastRFID<DEBOUNCE) {
    rfid.PICC_HaltA();
    rfid.PCD_StopCrypto1();
    return;
  }
  lastRFID = now;
  lastUID  = uid;

  Serial.print("Tag detectada: ["); Serial.print(uid); Serial.println("]");

  if (uid==UID_AUTORIZADA && vagasDisponiveis > 0) {
    Serial.println("Acesso Liberado");
    lcd.clear();
    lcd.print("Acesso Liberado!");
    vagasDisponiveis--;
    acionarCancelaComPulso(LED_ENTRADA);
    atualizarDisplayPrincipal();
  }
  else if (uid==UID_AUTORIZADA) {
    Serial.println("Lotado");
    lcd.clear();
    lcd.print("Lotado!");
    atualizarDisplayPrincipal();
  }
  else {
    Serial.println("Negado");
    lcd.clear();
    lcd.print("Acesso Negado!");
    delay(2000);
    atualizarDisplayPrincipal();  
  }

  rfid.PICC_HaltA();
  rfid.PCD_StopCrypto1();
}

// SAÍDA por sensor
void verificarSaida() {
  if (digitalRead(SENSOR_SAIDA_PIN)!=LOW) return;

  //lcd.clear();
  if (vagasDisponiveis<TOTAL_VAGAS) {
    vagasDisponiveis++;
    Serial.println("Carro saiu.");
    lcd.clear();
    lcd.print("Saida Liberada!");
    lcd.setCursor(0, 1);
    lcd.print("Volte Sempre!");

    acionarCancelaComPulso(LED_SAIDA);
    atualizarDisplayPrincipal();
  }
  delay(800); // debounce
}

// Mostra vagas no LCD
void atualizarDisplayPrincipal() {
  lcd.clear();
  lcd.print("Vagas Livres:");
  lcd.setCursor(0, 1);
  if (vagasDisponiveis<10) lcd.print('0');
  lcd.print(vagasDisponiveis);
}

// Substitui movimento + LED em uma mesma rotina bloqueante,
// mas garante que servo e LED partam juntos
void acionarCancelaComPulso(int ledPin) {
  // Liga LED (no seu wiring, LOW -> acende)
  digitalWrite(ledPin, LOW);

  // Movimento suave até 90° e volta em MOV_MS total
  int passos = 90;
  int dir    = 1;  // sobe primeiro
  int delayStep = (MOV_MS/2) / passos;
  for (int ang=0; ang<=90; ang++) {
    cancela.write(ang);
    delay(delayStep);
  }
  // segura no topo o restante do tempo
  delay(PULSO_MS - (MOV_MS/2));

  // volta (outros MOV_MS/2)
  for (int ang=90; ang>=0; ang--) {
    cancela.write(ang);
    delay(delayStep);
  }

  // Desliga LED
  digitalWrite(ledPin, HIGH);
}