// Ref1. https://raspberrypi.mongonta.com/howto-start-m5stack-core2arduinoide/
// Ref2. https://note.com/katsushun89/n/nd2cbcb9d88a3
// Ref3. https://github.com/m5stack/m5-docs/blob/master/docs/ja/api/button.md
// Ref4. https://www.skyarch.net/blog/?p=18253
// Ref5. https://qiita.com/minwinmin/items/55ba92edac3e1ec9f602
// Ref6. https://garretlab.web.fc2.com/arduino/esp32/examples/WiFiClientSecure/WiFiClientSecure.html
// Ref7. https://qiita.com/lumbermill/items/2517c5f130384cced335
// Ref8. https://wak-tech.com/archives/1770

#include <Arduino.h>
#include <M5Core2.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>

#include <wifi_pass.h>

bool bath_status = false;     // true: in_use, false: available

void setup() {
  M5.begin(true, false, false, false);
  M5.Lcd.setTextSize(2);
  //M5.Lcd.print("Hello World!!");

  // WifiSetup
  WiFi.begin(WIFI_SSID, WIFI_PASS);

  // wait for WiFi connection
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }

  M5.Lcd.println("Success wifi connection");
  M5.Lcd.print("Connected to ");
  M5.Lcd.println(WIFI_SSID);
  M5.Lcd.println(WiFi.localIP());

  WiFiClientSecure https;
  https.setCACert(ca);
  //int https_connection_check = https.connect(hostname, 443);
  //if(https_connection_check > 0)
  if((https.connect(hostname, 443)))
  {
    // HTTP header has been send and Server response header has been handled
    M5.Lcd.printf("[HTTPS] connection success\n");
    //String target_page = "/rss/topics/top-picks.xml";
    String str1 = String("POST https://") + String(hostname) + String(target_page) + " HTTP/1.1\r\n";
           str1 += "Host: " + String(hostname) + "\r\n";
           str1 += "User-Agent: BuildFailureDetectorESP32\r\n";
           str1 += "Content-Type: application/x-www-form-urlencoded\r\n";
           str1 += "Content-Length: "+ String(message_wakeup.length()) +"\r\n\r\n";
           str1 += message_wakeup + "\r\n\r\n";
           str1 += "Connection: close\r\n\r\n"; //closeを使うと、サーバーの応答後に切断される。最後に空行必要
           str1 += "\0";
    https.print(str1); //client.println にしないこと。最後に改行コードをプラスして送ってしまう為
    https.flush(); //client出力が終わるまで待つ
    
    delay(100);

    while (https.connected()) {
      String line = https.readStringUntil('\n');
      if (line == "\r") {
        //Serial.println("headers received");
        M5.Lcd.println("headers received");
        break;
      }
    }
    // if there are incoming bytes available
    // from the server, read them and print them:
    while (https.available()) {
      char c = https.read();
      //Serial.write(c);
      M5.Lcd.print(c);
    }

    https.stop();
  }
  else
  {
    M5.Lcd.printf("[HTTPS] connection failed\n");
  }

  // Wait
  delay(100000);
  M5.Lcd.clear(BLACK);
}

void loop() {
  M5.update();
  M5.Lcd.setCursor(0, 0);
  M5.Lcd.print("Button A Status: ");
  M5.Lcd.println(M5.BtnA.isPressed());
  M5.Lcd.print("Button B Status: ");
  M5.Lcd.println(M5.BtnB.isPressed());
  M5.Lcd.print("Button C Status: ");
  M5.Lcd.println(M5.BtnC.isPressed());
  M5.Lcd.print("Touch Panel Status: ");
  M5.Lcd.println(M5.Touch.ispressed());     //include button A/B/C
}