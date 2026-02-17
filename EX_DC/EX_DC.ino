#include <WiFi.h>
#include <WebServer.h>

// Motor A pins
const int motor1Pin1 = 27; 
const int motor1Pin2 = 26; 
const int enable1Pin = 14; 

// WiFi credentials
const char* ssid = "ESP";
const char* password = "12345678";

// WebServer instance
WebServer server(80);

// HTML page
const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
  <head>
    <title>ESP IOT DASHBOARD</title>
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <link rel="stylesheet" href="https://kit.fontawesome.com/0294e3a09e.css" crossorigin="anonymous">
    <style>
      html { font-family: Arial, Helvetica, sans-serif; text-align: center; background: #0A1128; color: white; }
      h1 { font-size: 1.8rem; margin-top: 20px; }
      .card { background: white; color: #034078; padding: 20px; margin: auto; width: 300px; border-radius: 10px; box-shadow: 2px 2px 12px rgba(0,0,0,0.3); }
      .card-title { font-size: 1.2rem; margin-bottom: 10px; }
      button {
        padding: 12px 25px; margin: 10px; font-size: 16px; border: none; border-radius: 5px; cursor: pointer;
        color: #fff;
      }
      .button-on { background: #034078; }
      .button-on:hover { background: #1282A2; }
      .button-off { background: #858585; }
      .button-off:hover { background: #252524; }
      .button-stop { background: #5e0f0f; width: 90%; }
      .button-stop:hover { background: #9b332c; }
    </style>
  </head>
  <body>
    <h1>CONTROL DC MOTOR</h1>
    <div class="card">
      <p class="card-title"><i class="fa-solid fa-gear"></i> DC Motor A</p>
      <p>
        <a href="/forward"><button class="button-on"><i class="fa-solid fa-arrow-up"></i> FORWARD</button></a>
        <a href="/backward"><button class="button-off"><i class="fa-solid fa-arrow-down"></i> BACKWARD</button></a>
      </p>
      <p>
        <a href="/stop"><button class="button-stop"><i class="fa-solid fa-stop"></i> STOP</button></a>
      </p>
    </div>
  </body>
</html>
)rawliteral";

// ========= Motor Functions =========
void moveForward() {
  Serial.println("Moving Forward");
  digitalWrite(enable1Pin, HIGH);
  digitalWrite(motor1Pin1, LOW);
  digitalWrite(motor1Pin2, HIGH);
}

void moveBackward() {
  Serial.println("Moving Backward");
  digitalWrite(enable1Pin, HIGH);
  digitalWrite(motor1Pin1, HIGH);
  digitalWrite(motor1Pin2, LOW);
}

void stopMotor() {
  Serial.println("Motor Stopped");
  digitalWrite(enable1Pin, LOW);
  digitalWrite(motor1Pin1, LOW);
  digitalWrite(motor1Pin2, LOW);
}

// ========= Handlers =========
void handleRoot() {
  server.send(200, "text/html", index_html);
}

void handleForward() {
  moveForward();
  server.send(200, "text/html", index_html);
}

void handleBackward() {
  moveBackward();
  server.send(200, "text/html", index_html);
}

void handleStop() {
  stopMotor();
  server.send(200, "text/html", index_html);
}

// ========= Setup =========
void setup() {
  Serial.begin(115200);
  pinMode(motor1Pin1, OUTPUT);
  pinMode(motor1Pin2, OUTPUT);
  pinMode(enable1Pin, OUTPUT);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected! IP address: ");
  Serial.println(WiFi.localIP());

  server.on("/", handleRoot);
  server.on("/forward", handleForward);
  server.on("/backward", handleBackward);
  server.on("/stop", handleStop);
  server.begin();
}

// ========= Loop =========
void loop() {
  server.handleClient();
}
