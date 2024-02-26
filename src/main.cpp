#include <LiquidCrystal_I2C.h> // https://github.com/johnrickman/LiquidCrystal_I2C
#include <NTPClient.h>         // https://github.com/arduino-libraries/NTPClient
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include "DHTesp.h" // https://github.com/beegee-tokyo/DHTesp

#define buzzerPin 15          // define buzzer connected pin
#define dhtPin 2              // define dht connected pin
#define dhtType DHTesp::DHT11 // Define dht sensor type

const char *ssid = "TRN-2.4G";        // Wifi SSID
const char *password = "TRN.QWED303"; // Wifi Password

const long utcOffsetInSeconds = 10800; // Set your timezone 1 hour = 3600 second

bool executed = false;
bool Fahrenheit = false;                        // If you want change temperature unit to Fahrenheit set true
const char *ntpServerUrl = "2.tr.pool.ntp.org"; // Set npt server url
                                                // https://www.ntppool.org/en/

DHTesp dht;
WiFiUDP ntpUDP;
LiquidCrystal_I2C lcd(0x27, 16, 2);
NTPClient timeClient(ntpUDP, ntpServerUrl, utcOffsetInSeconds);

void lcdClockDate();
void TempHum();
void ringBuzzer();

void setup()
{
    pinMode(buzzerPin, OUTPUT);
    lcd.init();
    lcd.backlight();
    WiFi.begin(ssid, password);
    dht.setup(dhtPin, dhtType);
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        lcd.setCursor(0, 0);
        lcd.print("Connecting to");
        lcd.setCursor(3, 1);
        lcd.print(ssid);
    }
    lcd.clear();
    timeClient.begin();
}

void loop()
{
    timeClient.update();
    lcdClockDate();
    TempHum();
    ringBuzzer();
    delay(2000);
}
void lcdClockDate()
{
    time_t epochTime = timeClient.getEpochTime();
    String formattedTime = timeClient.getFormattedTime().substring(0, 5);
    struct tm *ptm = gmtime((time_t *)&epochTime);
    int monthDay = ptm->tm_mday;
    String currentMonth = (ptm->tm_mon + 1) < 10 ? "0" + String(ptm->tm_mon + 1) : String(ptm->tm_mon + 1);
    int currentYear = ptm->tm_year - 100;
    String CurrentDate = String(monthDay) + "/" + String(currentMonth) + "/" + String(currentYear);
    lcd.setCursor(0, 0);
    lcd.print(CurrentDate);
    lcd.setCursor(11, 0);
    lcd.print(formattedTime);
}
void TempHum()
{
    float humidity = dht.getHumidity();
    float temperature = dht.getTemperature();
    if (Fahrenheit)
    {
        lcd.setCursor(0, 1);
        lcd.print(String(dht.toFahrenheit(temperature), 1) + char(223) + "F");
        lcd.setCursor(11, 1);
        lcd.print("%" + String(humidity, 1));
    }
    else
    {

        lcd.setCursor(0, 1);
        lcd.print(String(temperature, 1) + char(223) + "C");
        lcd.setCursor(11, 1);
        lcd.print("%" + String(humidity, 1));
    }
}

void ringBuzzer() // This function activates the buzzer only once per hour
{
    int currentMinute = timeClient.getMinutes();
    if (currentMinute == 0 && executed == false)
    {
        digitalWrite(buzzerPin, HIGH);
        delay(200);
        digitalWrite(buzzerPin, LOW);
        executed = true;
    }
    if (currentMinute != 0)
    {
        executed = false;
    }
}