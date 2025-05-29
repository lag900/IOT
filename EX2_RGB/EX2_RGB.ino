#include <WiFi.h>
#include <WebServer.h>

const char* ssid = "LAG";
const char* password = "12345678";

const int redPin = 25;
const int greenPin = 26;
const int bluePin = 27;

WebServer server(80);

String htmlPage = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
    <title>ESP32 RGB Color Picker</title>
    <meta charset="UTF-8">
    <style>
        body { font-family: Arial; text-align: center; background: #1a1a1a; color: #fff; }
        canvas { border: 1px solid #ccc; }
    </style>
</head>
<body>
    <h1>RGB LED Color Picker</h1>
    <canvas id="colorWheel" width="300" height="300"></canvas>
    <br>
    <canvas id="brightnessSlider" width="300" height="50"></canvas>
    <br>
    <div id="colorDisplay" style="width: 50px; height: 50px; border: 1px solid #fff; margin: 10px auto; background-color: #000;"></div>
    <script>
        const canvas = document.getElementById('colorWheel');
        const ctx = canvas.getContext('2d');
        const slider = document.getElementById('brightnessSlider');
        const sliderCtx = slider.getContext('2d');
        const colorDisplay = document.getElementById('colorDisplay');

        let hue = 0, saturation = 1, brightness = 1;

        function drawColorWheel() {
            const centerX = canvas.width / 2;
            const centerY = canvas.height / 2;
            const radius = canvas.width / 2 - 10;

            for (let angle = 0; angle < 360; angle += 1) {
                for (let r = 0; r < radius; r++) {
                    let h = angle;
                    let s = r / radius;
                    let rgb = hsvToRgb(h, s, brightness);
                    ctx.fillStyle = `rgb(${rgb.r}, ${rgb.g}, ${rgb.b})`;
                    let x = centerX + r * Math.cos((angle - 90) * Math.PI / 180);
                    let y = centerY + r * Math.sin((angle - 90) * Math.PI / 180);
                    ctx.fillRect(x, y, 2, 2);
                }
            }
            ctx.beginPath();
            ctx.arc(centerX + radius * Math.cos((hue - 90) * Math.PI / 180) * saturation, 
                    centerY + radius * Math.sin((hue - 90) * Math.PI / 180) * saturation, 5, 0, 2 * Math.PI);
            ctx.stroke();
        }

        function drawBrightnessSlider() {
            for (let x = 0; x < slider.width; x++) {
                let b = 1 - x / slider.width;
                let rgb = hsvToRgb(hue, saturation, b);
                sliderCtx.fillStyle = `rgb(${rgb.r}, ${rgb.g}, ${rgb.b})`;
                sliderCtx.fillRect(x, 0, 1, slider.height);
            }
            sliderCtx.beginPath();
            sliderCtx.moveTo(brightness * slider.width, 0);
            sliderCtx.lineTo(brightness * slider.width, slider.height);
            sliderCtx.stroke();
        }

        function hsvToRgb(h, s, v) {
            let r, g, b;
            let i = Math.floor(h / 60) % 6;
            let f = h / 60 - i;
            let p = v * (1 - s);
            let q = v * (1 - f * s);
            let t = v * (1 - (1 - f) * s);
            switch (i) {
                case 0: r = v; g = t; b = p; break;
                case 1: r = q; g = v; b = p; break;
                case 2: r = p; g = v; b = t; break;
                case 3: r = p; g = q; b = v; break;
                case 4: r = t; g = p; b = v; break;
                case 5: r = v; g = p; b = q; break;
            }
            return { r: Math.round(r * 255), g: Math.round(g * 255), b: Math.round(b * 255) };
        }

        canvas.addEventListener('click', (e) => {
            const rect = canvas.getBoundingClientRect();
            const x = e.clientX - rect.left - canvas.width / 2;
            const y = e.clientY - rect.top - canvas.height / 2;
            const distance = Math.sqrt(x * x + y * y);
            if (distance <= canvas.width / 2 - 10) {
                hue = (Math.atan2(y, x) * 180 / Math.PI + 90 + 360) % 360;
                saturation = Math.min(distance / (canvas.width / 2 - 10), 1);
                updateColor();
            }
        });

        slider.addEventListener('click', (e) => {
            const rect = slider.getBoundingClientRect();
            brightness = (e.clientX - rect.left) / slider.width;
            updateColor();
        });

        function updateColor() {
            let rgb = hsvToRgb(hue, saturation, brightness);
            colorDisplay.style.backgroundColor = `rgb(${rgb.r}, ${rgb.g}, ${rgb.b})`;
            fetch(`/set?r=${rgb.r}&g=${rgb.g}&b=${rgb.b}`);
            drawColorWheel();
            drawBrightnessSlider();
        }

        drawColorWheel();
        drawBrightnessSlider();
    </script>
</body>
</html>
)rawliteral";

void setup() {
    Serial.begin(115200);
    pinMode(redPin, OUTPUT);
    pinMode(greenPin, OUTPUT);
    pinMode(bluePin, OUTPUT);

    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.println("Connecting to WiFi...");
    }
    Serial.println("Connected to WiFi");
    Serial.println(WiFi.localIP());

    server.on("/", handleRoot);
    server.on("/set", handleSet);
    server.begin();
}

void loop() {
    server.handleClient();
}

void handleRoot() {
    server.send(200, "text/html", htmlPage);
}

void handleSet() {
    if (server.hasArg("r") && server.hasArg("g") && server.hasArg("b")) {
        int r = server.arg("r").toInt();
        int g = server.arg("g").toInt();
        int b = server.arg("b").toInt();
        r = constrain(r, 0, 255);
        g = constrain(g, 0, 255);
        b = constrain(b, 0, 255);
        analogWrite(redPin, r);
        analogWrite(greenPin, g);
        analogWrite(bluePin, b);
        Serial.printf("Set RGB: %d, %d, %d\n", r, g, b);
    }
    server.send(200);
}