#include <WiFi.h>
#include <WebServer.h>
#include "DHT.h"

// تعريف نوع الحساس
#define DHTTYPE DHT11  // DHT 11 فقط

// بيانات شبكة الواي فاي
const char* ssid = "LAG";     
const char* password = "12345678";    

// تحديد البورت
WebServer server(80);

// توصيل حساس DHT على البن رقم 4
uint8_t DHTPin = 4;
DHT dht(DHTPin, DHTTYPE);

float TemperatureC;
float Humidity;

void setup() {
  Serial.begin(115200);
  delay(100);

  pinMode(DHTPin, INPUT);
  dht.begin();

  Serial.println("Connecting to WiFi...");
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected!");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  // إعداد الصفحات
  server.on("/", handle_OnConnect);
  server.onNotFound(handle_NotFound);

  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
  server.handleClient();
}

void handle_OnConnect() {
  TemperatureC = dht.readTemperature(); // بالدرجة المئوية
  Humidity = dht.readHumidity();

  // إرسال صفحة HTML فيها البيانات
  server.send(200, "text/html", SendHTML(TemperatureC, TemperatureC * 9 / 5 + 32, Humidity)); 
}

void handle_NotFound() {
  server.send(404, "text/plain", "Not found");
}

// دالة إرسال صفحة HTML بتصميم أنيق
String SendHTML(float TempCstat, float TempFstat, float Humiditystat){
  String ptr = "<!DOCTYPE html> <html>\n";
  ptr +="<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0, user-scalable=no\">\n";
  ptr +="<link href=\"https://fonts.googleapis.com/css?family=Open+Sans:300,400,600\" rel=\"stylesheet\">\n";
  ptr +="<title>ESP32 Weather Report</title>\n";
  ptr +="<style>html { font-family: 'Open Sans', sans-serif; display: block; margin: 0px auto; text-align: center;color: #333333;}\n";
  ptr +="body{margin-top: 50px;}\n";
  ptr +="h1 {margin: 50px auto 30px;}\n";
  ptr +=".side-by-side{display: inline-block;vertical-align: middle;position: relative;}\n";
  ptr +=".humidity-icon{background-color: #3498db;width: 30px;height: 30px;border-radius: 50%;line-height: 36px;}\n";
  ptr +=".humidity-text{font-weight: 600;padding-left: 15px;font-size: 19px;width: 160px;text-align: left;}\n";
  ptr +=".humidity{font-weight: 300;font-size: 60px;color: #3498db;}\n";
  ptr +=".temperature-icon{background-color: #f39c12;width: 30px;height: 30px;border-radius: 50%;line-height: 40px;}\n";
  ptr +=".temperature-text{font-weight: 600;padding-left: 15px;font-size: 19px;width: 160px;text-align: left;}\n";
  ptr +=".temperature{font-weight: 300;font-size: 60px;color: #f39c12;}\n";
  ptr +=".superscript{font-size: 17px;font-weight: 600;position: absolute;right: -20px;top: 15px;}\n";
  ptr +=".data{padding: 10px;}\n";
  ptr +="</style>\n";
  ptr +="</head>\n";
  ptr +="<body>\n";
  ptr +="<div id=\"webpage\">\n";
  ptr +="<h1>ESP32 Weather Report</h1>\n";

  // Temperature Block
  ptr +="<div class=\"data\">\n";
  ptr +="<div class=\"side-by-side temperature-icon\">\n";
  ptr +="<svg version=\"1.1\" xmlns=\"http://www.w3.org/2000/svg\" width=\"9.915px\" height=\"22px\" viewBox=\"0 0 9.915 22\">\n";
  ptr +="<path fill=\"#FFFFFF\" d=\"M3.498,0.53c0.377-0.331,0.877-0.501,1.374-0.527C5.697-0.04,6.522,0.421,6.924,1.142c0.237,0.399,0.315,0.871,0.311,1.33C7.229,5.856,7.245,9.24,7.227,12.625c1.019,0.539,1.855,1.424,2.301,2.491c0.491,1.163,0.518,2.514,0.062,3.693c-0.414,1.102-1.24,2.038-2.276,2.594c-1.056,0.583-2.331,0.743-3.501,0.463c-1.417-0.323-2.659-1.314-3.3-2.617C0.014,18.26-0.115,17.104,0.1,16.022c0.296-1.443,1.274-2.717,2.58-3.394c0.013-3.44,0-6.881,0.007-10.322C2.674,1.634,2.974,0.955,3.498,0.53z\"/>\n";
  ptr +="</svg>\n";
  ptr +="</div>\n";
  ptr +="<div class=\"side-by-side temperature-text\">Temperature</div>\n";
  ptr +="<div class=\"side-by-side temperature\">";
  ptr +=(int)TempCstat;
  ptr +="<span class=\"superscript\">°C</span></div>\n";
  ptr +="</div>\n";

  // Humidity Block
  ptr +="<div class=\"data\">\n";
  ptr +="<div class=\"side-by-side humidity-icon\">\n";
  ptr +="<svg version=\"1.1\" xmlns=\"http://www.w3.org/2000/svg\" width=\"12px\" height=\"17.955px\" viewBox=\"0 0 13 17.955\">\n";
  ptr +="<path fill=\"#FFFFFF\" d=\"M1.819,6.217C3.139,4.064,6.5,0,6.5,0s3.363,4.064,4.681,6.217c1.793,2.926,2.133,5.05,1.571,7.057c-0.438,1.574-2.264,4.681-6.252,4.681c-3.988,0-5.813-3.107-6.252-4.681C-0.313,11.267,0.026,9.143,1.819,6.217\"></path>\n";
  ptr +="</svg>\n";
  ptr +="</div>\n";
  ptr +="<div class=\"side-by-side humidity-text\">Humidity</div>\n";
  ptr +="<div class=\"side-by-side humidity\">";
  ptr +=(int)Humiditystat;
  ptr +="<span class=\"superscript\">%</span></div>\n";
  ptr +="</div>\n";

  ptr +="</div>\n";
  ptr +="</body>\n";
  ptr +="</html>\n";
  return ptr;
}
