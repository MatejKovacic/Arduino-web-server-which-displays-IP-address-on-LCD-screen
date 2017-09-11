/* Arduino Nano based web server, which displays its IP address on 16x2 LCD display.

How does it work?
=================
 * 1. Arduino Nano with ethernet card gets its IP address via DHCP when ethernet cable is connected.
 * 2. When it gets IP address, IP address is displayed on LCD display.
 * 3. Arduino Nano is then running web server on that IP address.
 * 4. When client connects to webserver, it gets simple website showing values of analog readings from A0 and A1.
 *    Analog readings are from two MQ-x gas sensors.

Wiring instructions
===================
[LCD 16x2 (HD44780) - Arduino Nano]
 1. VSS Arduino GND
 2. VDD - Arduino 5V
 3. V0 (contrast) - 10k Ohm potentiometer (connect one wire to center pin and the other to left/right pin) - Arduino GND
 4. RS - Arduino D9
 5. RW - Arduino D8
 6. E (enable) - Arduino D7
 7. D0 (data 0) - not connected   
 8. D1 - not connected   
 9. D2 - not connected   
 10. D3 - not connected  
 11. D4 - Arduino D5
 12. D5 - Arduino D4
 13. D6 - Arduino D3
 14. D7 - Arduino D2
 15. A (backlight +) - 220 Ohm resistor (40 Ohm min.) - Arduino D6
 16. K (backlight GND) - Arduino GND

If there is no text displayed, LCD module may be out of sync with Arduino.
In that case press the reset button on your Arduino to synchronize (it may take a couple of resets).
Could be used for similar types of displays (for instance 20x4 LCD).

Source: https://www.hacktronics.com/Tutorials/arduino-character-lcd-tutorial.html


[Ethernet shield HanRun (HR911105A, 17/20) - Arduino Nano]
 1. V+5 - Arduino 5V
 2. GND - Arduino GND
 3. NSS - Arduino D10
 4. MO - Arduino D11
 5. SOK - Arduino D13
 6. MI - Arduino D12

Source: https://www.jdohnalek.com/arduino-nano-and-wiznet-w5100-red/


[MQx sensor - Arduino Nano]
 1. A0 - Arduino A0 (or A1 if you are using two sensors)
 2. D0 - not connected
 3. GND - Arduino GND
 4. VCC - Arduino 5V

*/

#include <LiquidCrystal.h>
#include <SPI.h>
#include <Ethernet.h>

byte mac[] = {
  0x00, 0xAA, 0xBB, 0xCC, 0xDE, 0x02  // Set MAC address.
};
EthernetServer server(80);  // Running web server on port 80.

LiquidCrystal lcd(9, 8, 7, 5, 4, 3, 2);  // Set which pins are used for LCD display.
int backLight = 6;  // Arduino pin D6 will control the backlight.

int hits = 0;  // Web hits counter.

void setup(){
  pinMode(backLight, OUTPUT);
  digitalWrite(backLight, HIGH); // Turn backlight on. Replace 'HIGH' with 'LOW' to turn it off.
  lcd.begin(16,2);               // Columns and rows.  use 16,2 for a 16x2 LCD.
  lcd.clear();                   // Start with a blank screen...
  lcd.setCursor(0,0);            // Set cursor to column 0, row 0 (the beginning of the first line).

  lcd.print("Connect cable!");   // Text to display where there is no ethernet cable connected.
  lcd.setCursor(0, 1);
  lcd.print("Web hits: ");
  lcd.print(hits);

  Ethernet.begin(mac);           // Get IP address from DHCP.
  lcd.setCursor(0,0);
  lcd.print(Ethernet.localIP()); // Print IP address on LCD display.
  server.begin();
}

void loop(){
  EthernetClient client = server.available();
  if (client) {
    while (client.connected()) {
      if (client.available()) {
        // Send a standard HTTP response header.
        client.println("HTTP/1.1 200 OK");
        client.println("Content-Type: text/html");
        client.println("Connection: close");  // The connection will be closed after completion of the response
        client.println("Refresh: 2");         // Refresh the web page automatically every 2 sec
        client.println();
        client.println("<!DOCTYPE HTML>");
        client.println("<HTML>");
        client.println("<h2>Arduino web server</h2><br />");
        // Print output the value of each analog input pin (0 and 1)
        for (int analogChannel = 0; analogChannel < 2; analogChannel++) {
          int sensorReading = analogRead(analogChannel);
          client.print("Analog input A");
          client.print(analogChannel);
          client.print(" has a value ");
          client.print(sensorReading);
          client.println("<br />");
        }
        client.println("<br /><small><center>(CC) Matej Kovacic</center></small><br />");
        client.println("</HTML>");

        hits = ++hits;

        lcd.setCursor(0, 1);
        lcd.print("Web hits: ");  // Print increased web hits counter on LCD display.
        lcd.print(hits);
        break;
      }
    }
    client.stop();
  }
}
