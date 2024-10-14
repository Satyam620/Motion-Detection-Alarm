#include <ESP8266WiFi.h>
#include <UniversalTelegramBot.h>
#include <WiFiClientSecure.h>
#include <ArduinoJson.h>

const char* ssid = "xxxxxxxxxx";  // Replace with your WiFi name
const char* password = "xxxxxxxxxx";          // Replace with your WiFi password

#define botToken "xxxxxxxxxx:xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"  // Replace with your Telegram bot token
#define chatID "xxxxxxxxxx"                                        // Replace with your chat ID

X509List cert(TELEGRAM_CERTIFICATE_ROOT);

WiFiClientSecure client;
UniversalTelegramBot bot(botToken, client);

const int pirPin = D5;       // PIR sensor output pin D5
const int redLedPin = D1;    // Red LED pin
const int buzzerPin = D2;    // Buzzer pin
const int greenLedPin = D3;  // Green LED pin

const int timeSeconds = 5; //Replace with your personal time prefernce.
bool motionDetected = false;
unsigned long now = millis();
unsigned long lastTrigger = 0;


void setup() {
  Serial.begin(115200);

  // Configure time and security
  configTime(0, 0, "pool.ntp.org");  // Get UTC time via NTP
  client.setTrustAnchors(&cert);

  pinMode(redLedPin, OUTPUT);     // RED LED
  pinMode(buzzerPin, OUTPUT);     // BUZZER
  pinMode(greenLedPin, OUTPUT);   // LED GREEN
  pinMode(pirPin, INPUT_PULLUP);  // PIR SENSOR OUTPUT PIN D5

  // Connect to WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("Connected to WiFi");
  bot.sendMessage(chatID, "Bot started up", "");
  checkSensor();  // Initial sensor check
}

IRAM_ATTR void detectsMovement() {
  Serial.println("MOTION DETECTED!!!");
  motionDetected = true;
  lastTrigger = millis();
}


void checkSensor() {
  Serial.println("System is ON");
  attachInterrupt(digitalPinToInterrupt(pirPin), detectsMovement, RISING);
  if (motionDetected) {
    digitalWrite(redLedPin, HIGH);   // Turn on red LED
    digitalWrite(buzzerPin, HIGH);   // Turn on buzzer
    digitalWrite(greenLedPin, LOW);  // Turn off green LED
    if (WiFi.status() == WL_CONNECTED) {
      String message = "WARNING! Please check your security system";
      bot.sendMessage(chatID, message, "");
    }
  } else {
    digitalWrite(redLedPin, LOW);     // Turn off red LED
    digitalWrite(buzzerPin, LOW);     // Turn off buzzer
    digitalWrite(greenLedPin, HIGH);  // Turn on green LED
  }
}

void loop() {
  checkSensor();  // Check sensor status
  now = millis();
  if (motionDetected && (now - lastTrigger > (timeSeconds * 1000))) {
    digitalWrite(redLedPin, LOW);     // Turn off red LED
    digitalWrite(buzzerPin, LOW);     // Turn off buzzer
    digitalWrite(greenLedPin, HIGH);  // Turn on green LED
    motionDetected = false;
  }
}
