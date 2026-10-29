// ===== EXERCISE 1: Hello world by serial =====
/*
void setup() {
  Serial.begin(115200);
}

void loop() {
  Serial.println("Hello world");
  delay(1000);
}
*/


// ===== EXERCISE 2: Read temperature sensor =====
/*
void setup() {
  Serial.begin(115200);
}

void loop() {
  float temp = temperatureRead();
  Serial.print("Temperature: ");
  Serial.println(temp);
  delay(1000);
}
*/


// ===== EXERCISE 3: Sine graph (open Serial Plotter) =====
/*
void setup() {
  Serial.begin(115200);
}

void loop() {
  for (int i = 0; i < 360; i++) {
    float sine_1 = 1 * sin(i * M_PI / 180);
    float sine_2 = 2 * sin((i + 90) * M_PI / 180);
    float sine_3 = 5 * sin((i + 180) * M_PI / 180);

    Serial.print(sine_1);
    Serial.print(",");
    Serial.print(sine_2);
    Serial.print(",");
    Serial.println(sine_3);

    delay(10);
  }
}
*/


// ===== EXERCISE 4: Blink built-in LED =====
// the task says pin 9 but on my board pin 9 is for the flash memory
// and the board crashes, so i use pin 2
/*
#define LED 2

void setup() {
  pinMode(LED, OUTPUT);
}

void loop() {
  digitalWrite(LED, HIGH);
  delay(500);
  digitalWrite(LED, LOW);
  delay(500);
}
*/


// ===== EXERCISE 5: Read touch pin =====
// touch pin T0 is GPIO4, touch it with the finger
/*
void setup() {
  Serial.begin(115200);
}

void loop() {
  int touch = touchRead(T0);
  Serial.print("Touch: ");
  Serial.println(touch);
  delay(200);
}
*/


// ===== EXERCISE 6 and 7: Deep sleep + wake up with touchpad =====
// the ESP32 goes to sleep after 5 seconds
// touch GPIO4 to wake it up

// #define THRESHOLD 900

// RTC_DATA_ATTR int bootCount = 0;

// void callback() {
//   // nothing here, its only needed for the touch wakeup
// }

// void setup() {
//   Serial.begin(115200);
//   delay(1000);

//   bootCount++;
//   Serial.print("Boot number: ");
//   Serial.println(bootCount);

//   if (esp_sleep_get_wakeup_cause() == ESP_SLEEP_WAKEUP_TOUCHPAD) {
//     Serial.println("Woken up by touchpad!");
//   } else {
//     Serial.println("Normal start");
//   }

//   // configure touchpad wakeup
//   touchAttachInterrupt(T0, callback, THRESHOLD);
//   esp_sleep_enable_touchpad_wakeup();

//   Serial.println("Going to sleep in 5 seconds...");
//   delay(5000);

//   Serial.println("Sleeping now, touch GPIO4 to wake up");
//   Serial.flush();
//   esp_deep_sleep_start();
// }

// void loop() {
//   // never runs because the ESP32 goes to sleep in setup
// }