// ESP32 Smart Lock - web dashboard (UI only for now)
// based on the web server example from class
//
// Pages:
//   /            -> dashboard (reloads every 2 s)
//   /lock        -> lock the door
//   /reset       -> reset failed attempts
//   /change      -> page with the form to change the PIN
//   /setpin?old=1234&new=5678 -> saves the new PIN
//   /sleep       -> deep sleep (added later)

#include <WiFi.h>

const char* ssid = "Iphone (Kuba)";
const char* password = "12345678";

WiFiServer server(80);
String header;

unsigned long currentTime = millis();
unsigned long previousTime = 0;
const long timeoutTime = 2000;

// ===== lock state =====
String pin = "1234";            // later saved in EEPROM
bool locked = true;             // true = locked, false = open
String enteredDigits = "";      // digits typed with the remote
int failedAttempts = 0;         // wrong PIN counter
int wakeCount = 0;              // times woken up from deep sleep
String lastKey = "-";           // last key pressed on the remote
String message = "System started";   // last event, shown on the page

// read a value from the URL, e.g. "new" from "GET /setpin?old=1234&new=5678"
String getParam(String request, String name) {
  int start = request.indexOf(name + "=");
  if (start < 0) {
    return "";
  }
  start = start + name.length() + 1;
  int end = start;
  while (end < request.length() && request[end] != '&' && request[end] != ' ') {
    end++;
  }
  return request.substring(start, end);
}

// beginning of every page: HTTP header + style
void sendStart(WiFiClient& client, bool autoRefresh) {
  client.println("HTTP/1.1 200 OK");
  client.println("Content-type:text/html; charset=utf-8");
  client.println("Connection: close");
  client.println();

  client.println("<!DOCTYPE html><html><head>");
  client.println("<meta name='viewport' content='width=device-width, initial-scale=1'>");
  client.println("<link rel='icon' href='data:,'>");
  if (autoRefresh) {
    client.println("<meta http-equiv='refresh' content='2; url=/'>");   // reload every 2 s
  }
  client.println("<style>");
  client.println("body { background:#0f1420; color:#e8ecf3; font-family:Arial; text-align:center; padding:16px; }");
  client.println(".card { background:#1a2130; border-radius:16px; padding:16px; margin:12px auto; max-width:420px; }");
  client.println(".locked { color:#e74c3c; font-size:30px; font-weight:bold; }");
  client.println(".open { color:#2ecc71; font-size:30px; font-weight:bold; }");
  client.println(".button { display:block; background:#4f8cff; color:white; padding:14px; margin:8px 0;");
  client.println("  border:none; border-radius:10px; font-size:18px; text-decoration:none; width:100%; box-sizing: border-box; }");
  client.println(".red { background:#e74c3c; } .grey { background:#3a4560; }");
  client.println("input { width:100%; padding:12px; font-size:18px; border-radius:10px; border:none; margin:6px 0; box-sizing:border-box; }");
  client.println("</style></head><body>");
  client.println("<h1>ESP32 Smart Lock</h1>");
}

// main page
void sendDashboard(WiFiClient& client) {
  sendStart(client, true);

  // lock state
  client.println("<div class='card'>");
  if (locked) {
    client.println("<p class='locked'>&#128274; LOCKED</p>");
  } else {
    client.println("<p class='open'>&#128275; OPEN</p>");
  }
  // typed digits shown as * * * _
  String dots = "";
  for (int i = 0; i < pin.length(); i++) {
    if (i < enteredDigits.length()) {
      dots += "* ";
    } else {
      dots += "_ ";
    }
  }
  client.println("<p style='font-size:28px'>" + dots + "</p>");
  client.println("<p>" + message + "</p>");
  client.println("</div>");

  // statistics
  client.println("<div class='card'>");
  client.println("<p>Failed attempts: <b>" + String(failedAttempts) + "</b></p>");
  client.println("<p>Wake ups: <b>" + String(wakeCount) + "</b></p>");
  client.println("<p>Last key: <b>" + lastKey + "</b></p>");
  client.println("<p>Uptime: <b>" + String(millis() / 1000) + " s</b></p>");
  client.println("</div>");

  // buttons
  client.println("<div class='card'>");
  client.println("<a class='button red' href='/lock'>Lock now</a>");
  client.println("<a class='button' href='/change'>Change PIN</a>");
  client.println("<a class='button grey' href='/reset'>Reset counter</a>");
  client.println("<a class='button grey' href='/sleep'>Sleep</a>");
  client.println("</div>");

  client.println("</body></html>");
}

// page with the form to change the PIN (no auto refresh, so you can type)
void sendChangePage(WiFiClient& client) {
  sendStart(client, false);

  client.println("<div class='card'>");
  client.println("<h2>Change PIN</h2>");
  client.println("<form action='/setpin'>");
  client.println("<input name='old' type='password' inputmode='numeric' placeholder='Current PIN'>");
  client.println("<input name='new' type='password' inputmode='numeric' placeholder='New PIN (4-8 digits)'>");
  client.println("<input class='button' type='submit' value='Save'>");
  client.println("</form>");
  client.println("<a class='button grey' href='/'>Back</a>");
  client.println("</div>");

  client.println("</body></html>");
}

void setup() {
  Serial.begin(115200);

  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.print("Open: http://");
  Serial.println(WiFi.localIP());
  server.begin();
}

void loop() {
  WiFiClient client = server.available();

  if (client) {
    currentTime = millis();
    previousTime = currentTime;
    String currentLine = "";

    while (client.connected() && currentTime - previousTime <= timeoutTime) {
      currentTime = millis();
      if (client.available()) {
        char c = client.read();
        header += c;
        if (c == '\n') {
          if (currentLine.length() == 0) {

            // ===== what did the user click? =====
            if (header.indexOf("GET /lock") >= 0) {
              locked = true;
              enteredDigits = "";
              message = "Locked from the web";
              sendDashboard(client);
            }
            else if (header.indexOf("GET /reset") >= 0) {
              failedAttempts = 0;
              message = "Counter reset";
              sendDashboard(client);
            }
            else if (header.indexOf("GET /change") >= 0) {
              sendChangePage(client);
            }
            else if (header.indexOf("GET /setpin") >= 0) {
              String oldPin = getParam(header, "old");
              String newPin = getParam(header, "new");
              if (oldPin != pin) {
                message = "Wrong current PIN";
              } else if (newPin.length() < 4 || newPin.length() > 8) {
                message = "New PIN must have 4-8 digits";
              } else {
                pin = newPin;                // later also save in EEPROM
                message = "PIN changed";
              }
              Serial.println(message);
              sendDashboard(client);
            }
            else if (header.indexOf("GET /sleep") >= 0) {
              message = "Sleep not added yet";   // later: deep sleep
              sendDashboard(client);
            }
            else {
              sendDashboard(client);         // main page
            }

            break;
          } else {
            currentLine = "";
          }
        } else if (c != '\r') {
          currentLine += c;
        }
      }
    }

    header = "";
    client.stop();
  }
}
