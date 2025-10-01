#include <Arduino.h>
#include <WiFi.h>
#include <WiFiMulti.h>
#include <WiFiClientSecure.h>

const char* host = "api.line.me";
const int httpsPort = 443;

#define WIFI_SSID "***" // WiFi Name
#define WIFI_PASSWORD "********" //WiFipassword
// WiFi Password
const int TRIG_PIN = 27;
const int ECHO_PIN = 14; // ต้องผ่านตัวแบ่งแรงดัน!


WiFiClientSecure client; 

void setup() {
  Serial.begin(115200);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnected!");

  client.setInsecure(); //skip certificate validation for simplicity
  
}



void sendLineMessage(String message) {
  String accessToken = "****"; // Channel access token
  String userID = "****"; //userId / Group

  if (!client.connect(host, httpsPort)) {
    Serial.println("Connection failed!");
    return;
  }

  String url = "/v2/bot/message/push";
  String body = "{ \"to\": \"" + userID + "\", \"messages\":[{\"type\":\"text\",\"text\":\"" + message + "\"}]}";

  // Build HTTP request
  client.println("POST " + url + " HTTP/1.1");
  client.println("Host: " + String(host));
  client.println("Authorization: Bearer " + accessToken);
  client.println("Content-Type: application/json");
  client.print("Content-Length: ");
  client.println(body.length());
  client.println();
  client.println(body);

  // Read response
  while (client.connected()) {
    String line = client.readStringUntil('\n');
    if (line == "\r") break; // headers end
  }

  String response = client.readString();
  Serial.println("Response:");
  Serial.println(response);

  client.stop();
}

float readUltrasonic(int trigPin, int echoPin) {
  // Send 10µs pulse to trigger pin
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  // Read echo pulse
  long duration = pulseIn(echoPin, HIGH);

  // Convert to centimeters (speed of sound ~ 343 m/s)
  float distanceCm = duration * 0.0343 / 2.0;
  return distanceCm;
}

void loop() {
  float distanceCm = readUltrasonic(TRIG_PIN,ECHO_PIN);
  
  if( distanceCm < 50 )
  {
    sendLineMessage("*");//Notification message
    delay(10000);//10s
  }

}