#include <Wire.h>
#include <Adafruit_MLX90614.h>
#include "RTClib.h"
#include "WiFi.h"
#include <ThingerESP32.h>
#include <Nextion.h>
#include <NextionPage.h>
#include <NextionButton.h>
#include <NextionText.h>

#define USERNAME ""
#define DEVICE_ID ""
#define DEVICE_CREDENTIAL ""

ThingerESP32 thing(USERNAME, DEVICE_ID, DEVICE_CREDENTIAL);

#define SSID ""
#define WIFI_PASSWORD ""

Adafruit_MLX90614 mlx = Adafruit_MLX90614();
RTC_DS3231 rtc;

#define HMI_SERIAL Serial2
#define HMI_BAUD 256000

// pin adc untuk mq2 & uvtron
#define MQ2_PIN 35     // ESP32's pin GPIO34 connected to AO pin of the MQ2 sensor
#define UVTRON_PIN 34  // ESP32's pin GPIO35 connected to Active High UVTROn sensor

// pin output untuk alarm, pump dan indikator
#define ALARM 25
#define PUMP 26
#define LED_INDIKATOR_KUNING 18  // indikator suhu (led kuning)
#define LED_INDIKATOR_MERAH 19   // indikator gas (led merah)

// HMI
Nextion next(HMI_SERIAL);
// Page HMI
NextionPage pageMain(next, 0, 0, "main");
// Sensor value HMI
NextionText tempText(next, 0, 1, "temp");
NextionText mq2Text(next, 0, 2, "mq2");
NextionText uvtronText(next, 0, 8, "uvtron");
NextionText timeText(next, 0, 5, "time");

unsigned long previousMillis = 0;
const long interval = 200;  // Interval waktu dalam milidetik (ms)

int mq2_value;
float temp_value;
float voltageUVTRON = 0;
bool fire = false;
String rtc_value;
char charBuffer[10];
int gasValue;
float voltageMQ2;

int sumUVTRON = 0;
int countUVTRON = 0;

bool alarm_status = false;

void setup() {
  // initialize serial communication;
  pinMode(MQ2_PIN, INPUT);
  pinMode(UVTRON_PIN, INPUT);
  pinMode(ALARM, OUTPUT);
  pinMode(PUMP, OUTPUT);
  pinMode(LED_INDIKATOR_KUNING, OUTPUT);
  pinMode(LED_INDIKATOR_MERAH, OUTPUT);

  digitalWrite(ALARM, HIGH);
  digitalWrite(PUMP, HIGH);
  digitalWrite(LED_INDIKATOR_KUNING, HIGH);
  digitalWrite(LED_INDIKATOR_MERAH, HIGH);

  Serial.begin(115200);
  HMI_SERIAL.begin(HMI_BAUD);
  next.init();
  delay(200);
  pageMain.show();
  tempText.setText("");
  mq2Text.setText("");
  uvtronText.setText("");
  timeText.setText("");

  Serial.print("Connecting to ");
  Serial.println(SSID);
  thing.add_wifi(SSID, WIFI_PASSWORD);
  Serial.println("WiFi connected.");

  Serial.println("Sensors Testing");
  mlx.begin();

  if (!rtc.begin()) {
    Serial.println("Couldn't find RTC");
  }

  // Atur waktu RTC dengan waktu kompilasi kode
  // rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));

  thing["button"] << [](pson& in) {
    alarm_status = in;
  };

  thing["UVTRON"] >> [](pson& out) {
    out["value"] = fire;
    out["voltage"] = voltageUVTRON;
  };
  thing["MQ2"] >> [](pson& out) {
    out["value"] = mq2_value;
    out["ADC"] = gasValue;
    out["voltage"] = voltageMQ2;
  };
  thing["Temp"] >> [](pson& out) {
    out["value"] = temp_value;
  };
}

void loop() {
  next.poll();
  thing.handle();
  unsigned long currentMillis = millis();

  int value = analogRead(UVTRON_PIN);
  sumUVTRON += value;
  countUVTRON++;

  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    DateTime now = rtc.now();
    Serial.print("RTC : ");
    Serial.print(now.year(), DEC);
    Serial.print('/');
    Serial.print(now.month(), DEC);
    Serial.print('/');
    Serial.print(now.day(), DEC);
    Serial.print(" ");
    Serial.print(now.hour(), DEC);
    Serial.print(':');
    Serial.print(now.minute(), DEC);
    Serial.print(':');
    Serial.print(now.second(), DEC);
    Serial.println();
    rtc_value = String(formatDigits(now.hour()) + ":" + formatDigits(now.minute()) + " " + formatDigits(now.day()) + "/" + formatDigits(now.month()) + "/" + String(now.year()));

    gasValue = analogRead(MQ2_PIN);
    voltageMQ2 = (gasValue / 4095.0) * 3.3 * 1000;
    mq2_value = map(voltageMQ2, 0, 3300, 0, 1000) * 3;

    Serial.print("MQ2 sensor value: ");
    Serial.println(mq2_value);

    if (mq2_value > 400) {
      digitalWrite(LED_INDIKATOR_KUNING, LOW);
    } else {
      digitalWrite(LED_INDIKATOR_KUNING, HIGH);
    }

    // Serial.print("Ambient = ");
    // Serial.print(mlx.readAmbientTempC());
    temp_value = mlx.readObjectTempC();
    Serial.print("Temp = ");
    Serial.print(temp_value);
    Serial.println("°C");

    if (temp_value >= 40.0) {
      digitalWrite(LED_INDIKATOR_MERAH, LOW);
    } else {
      digitalWrite(LED_INDIKATOR_MERAH, HIGH);
    }

    if (countUVTRON > 0) {
      int averageUVTRON = sumUVTRON / countUVTRON;
      voltageUVTRON = (averageUVTRON / 4095.0) * 3.3 * 1000;
      Serial.print("UVTRON Value (mV) : ");
      Serial.println(voltageUVTRON);
      if (voltageUVTRON > 5) {
        fire = true;
        alarm_status = true;
        Serial.println("Terdeteksi Api!");
      } else {
        fire = false;
        Serial.println("Tidak Terdeteksi Api");
      }

      // Reset nilai sum dan count setelah menampilkan rata-rata
      sumUVTRON = 0;
      countUVTRON = 0;
    }

    if (alarm_status) {
      digitalWrite(PUMP, LOW);
      digitalWrite(ALARM, LOW);
    } else {
      digitalWrite(PUMP, HIGH);
      digitalWrite(ALARM, HIGH);
    }

    // send data to hmi
    dtostrf(temp_value, 5, 1, charBuffer);
    tempText.setText(charBuffer);

    itoa(mq2_value, charBuffer, 10);
    mq2Text.setText(charBuffer);

    char rtc_char[rtc_value.length() + 1];
    rtc_value.toCharArray(rtc_char, rtc_value.length() + 1);
    timeText.setText(rtc_char);
    if (fire) {
      uvtronText.setText("Terdeteksi Api !");
    } else {
      uvtronText.setText("Tidak Ada Api");
    }
  }
}

String formatDigits(int digits) {
  // Tambahkan nol di depan angka jika hanya satu digit
  if (digits < 10) {
    return "0" + String(digits);
  } else {
    return String(digits);
  }
}