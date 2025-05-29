  #include <WiFi.h>                      // دي المكتبة اللي بتخلي ESP32 يقدر يتصل بالواي فاي
  
  const char* ssid = "LAG";              // هنا بنكتب اسم شبكة الواي فاي
  const char* password = "A1442002a!";   // وهنا الباسورد بتاعها
  
  WiFiServer server(80);                 // بنعمل سيرفر على البورت 80 (ده البورت اللي المتصفح بيدخل عليه)
  
  String header;                         // متغير عشان نخزن فيه الطلب اللي جاي من المتصفح
  String LedPinState = "off";            // حالة الليد دلوقتي (مطفية في الأول)
 
 const int LedPin = 4;                  // الليد موصل على البن رقم 4
 
 unsigned long currentTime = millis();  // بنجيب الوقت الحالي بالملي ثانية
 unsigned long previousTime = 0;        // وده آخر وقت اتسجل فيه حركة
 const long timeoutTime = 2000;         // لو العميل ما ردش في خلال 2 ثانية، نقفل عليه
 
 void setup() {
   Serial.begin(115200);                // بنشغل السيريال مونيتور عشان نطبع عليه كلام
   pinMode(LedPin, OUTPUT);             // بنقول إن البن 4 هيبقى Output (يعني نطلع عليه كهربا)
   digitalWrite(LedPin, LOW);           // نطفي الليد في الأول
 
   Serial.print("Connecting to ");      // نطبع إننا بنحاول نتصل بالواي فاي
   Serial.println(ssid);
   WiFi.begin(ssid, password);          // نبدأ نحاول نتصل بالشبكة
   while (WiFi.status() != WL_CONNECTED) {
     delay(500);                        // كل نص ثانية نستنى شوية
     Serial.print(".");                 // ونطبع نقطة عشان نعرف إنه لسه بيحاول
   }
   Serial.println("");
   Serial.println("WiFi connected.");   // أول ما يتصل، نطبع إنه اتصل
   Serial.println("IP address: ");
   Serial.println(WiFi.localIP());      // نطبع الـ IP اللي أخده
   server.begin();                      // نشغل السيرفر
 }
 
 void loop(){
   WiFiClient client = server.available(); // نستنى لو في عميل دخل على السيرفر
 
   if (client) {                           // لو في عميل فعلاً
     currentTime = millis();              // نسجل الوقت الحالي
     previousTime = currentTime;
     Serial.println("New Client.");       // نطبع إنه في عميل جديد
     String currentLine = "";             // نخزن كل سطر بيكتبه العميل
     while (client.connected() && currentTime - previousTime <= timeoutTime) {
       currentTime = millis();            // نحدث الوقت الحالي كل شوية
       if (client.available()) {          // لو في بيانات وصلت من العميل
         char c = client.read();          // نقرأ الحرف اللي جاي
         Serial.write(c);                 // نطبعه على السيريال مونيتور
         header += c;                     // نحوشه في المتغير header
         if (c == '\n') {                 // لو الحرف ده سطر جديد
           if (currentLine.length() == 0) { // ولو السطر فاضي، يعني نهاية الطلب
             client.println("HTTP/1.1 200 OK");            // نرد على العميل إنه تمام
             client.println("Content-type:text/html");     // ونقول له إننا هنبعت HTML
             client.println("Connection: close");          // وقول له يقفل الاتصال بعد كده
             client.println();                             // سطر فاضي بيفصل الهيدرز عن الصفحة
 
             // لو العميل بعت طلب إنه يشغل البن 26 (بس إحنا موصلين على البن 4)
             if (header.indexOf("GET /26/on") >= 0) {
               Serial.println("GPIO 26 on");
               LedPinState = "on";                         // نحدث الحالة
               digitalWrite(LedPin, HIGH);                 // ونشغل الليد
             } else if (header.indexOf("GET /26/off") >= 0) {
               Serial.println("GPIO 26 off");
               LedPinState = "off";
               digitalWrite(LedPin, LOW);                  // نطفي الليد
             } 
 
             client.println("<!DOCTYPE html><html>");        // نبدأ صفحة HTML
             client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
             client.println("<link rel=\"icon\" href=\"data:,\">");
             client.println("<style>");
             client.println("@import url('https://fonts.googleapis.com/css2?family=Cairo&display=swap');");
             client.println("html { font-family: 'Cairo', sans-serif; display: inline-block; margin: 0px auto; text-align: center; }");
             client.println(".button { background-color: #4CAF50; border: none; color: white; padding: 16px 40px;");
             client.println("text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer; }");
             client.println(".button2 { background-color: #555555; }");
             client.println("</style></head>");
 
             client.println("<body><h2>Children's Technological University</h2>");
             client.println("<p>  Led " + LedPinState + "</p>");
             if (LedPinState=="off") {
               client.println("<p><a href=\"/26/on\"><button class=\"button\">ON</button></a></p>");
             } else {
               client.println("<p><a href=\"/26/off\"><button class=\"button button2\">OFF</button></a></p>");
             } 
             client.println("</body></html>");              // نهاية الصفحة
             client.println();                             // سطر فاضي ينهي الاستجابة
             break;                                        // نخرج من اللوب
           } else {
             currentLine = "";                             // نفضي السطر ونكمل
           }
         } else if (c != '\r') {
           currentLine += c;                               // نضيف الحرف للسطر
         }
       }
     }
     header = "";            // نفضي الهيدر بعد ما نخلص
     client.stop();          // نقفل الاتصال بالعميل
     Serial.println("Client disconnected.");
    Serial.println("");
  }
}
