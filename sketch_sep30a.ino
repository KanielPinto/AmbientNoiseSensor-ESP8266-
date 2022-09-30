#include <LiquidCrystal_I2C.h>

#include <ESP8266SSDP.h>
#include <ESP8266WiFi.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);


int num_Measure = 128 ; // Set the number of measurements
int pinSignal = A0; // pin connected to pin O module sound sensor
long Sound_signal;    // Store the value read Sound Sensor
long sum = 0 ; // Store the total value of n measurements
long level = 0 ; // Store the average value
int soundlow = 100;
int soundmedium = 140;
int error = 52;


String apiKey = "ZBB0UOZ4BRD5X8CE"; // Enter your Write API key from ThingSpeak
const char *ssid = "pintos_2";     // replace with your wifi ssid and wpa2 key
const char *pass = "9920295894";
const char* server = "184.106.153.149";

WiFiClient client;

void setup ()
{
  pinMode (pinSignal, INPUT); // Set the signal pin as input
  pinMode (D5, OUTPUT);
  pinMode (D6, OUTPUT);
  pinMode (D7, OUTPUT);

  Serial.begin (115200);
  lcd.begin();
  lcd.backlight();
  lcd.clear();
  delay(10);

  Serial.println("Connecting to ");
  Serial.println(ssid);

  lcd.setCursor(0, 0);
  lcd.print("Connecting to ");
  lcd.setCursor(0, 1);
  lcd.print(ssid);

  WiFi.begin(ssid, pass);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("WIFIconnected");
  delay(4000);
}


void loop ()
{

  // Performs 128 signal readings
  for ( int i = 0 ; i < num_Measure; i ++)
  {
    Sound_signal = analogRead (pinSignal);
    sum = sum + Sound_signal;
  }

  level = sum / num_Measure; // Calculate the average value

  Serial.print("\nSound Level: ");
  Serial.println (level - error);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.println("--Decibelmeter--");



  if ( (level - error) < soundlow)
  {

    Serial.print("Intensity= Low");
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Sound Level: ");
    lcd.println(level - error);

    lcd.setCursor(0, 1);
    lcd.print("Intensity: LOW");

    digitalWrite(D6, LOW);
    digitalWrite(D7, LOW);
    digitalWrite(D5, HIGH);
   }
  if ( ( (level - error) > soundlow ) && ( (level - error) < soundmedium )  )
  {

    Serial.print("Intensity=Medium");
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Sound Level: ");
    lcd.println(level - error);

    lcd.setCursor(0, 1);
    lcd.print("Intensity: MID");

    digitalWrite(D6, HIGH);
    digitalWrite(D7, LOW);
    digitalWrite(D5, LOW);
  }
  if ( (level - error) > soundmedium )
  {

    Serial.print("Intensity= High");
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Sound Level: ");
    lcd.println(level - error);

    lcd.setCursor(0, 1);
    lcd.print("Intensity: HIGH");

    digitalWrite(D6, LOW);
    digitalWrite(D7, HIGH);
    digitalWrite(D5, LOW);

  }
  sum = 0 ; // Reset the sum of the measurement values
  delay(200);

  if (client.connect(server, 80)) // "184.106.153.149" or api.thingspeak.com
  {
    String postStr = apiKey;
    postStr += "&field1=";
    postStr += String(level - error);
    postStr += "r\n";

    client.print("POST /update HTTP/1.1\n");
    client.print("Host: api.thingspeak.com\n");
    client.print("Connection: close\n");
    client.print("X-THINGSPEAKAPIKEY: " + apiKey + "\n");
    client.print("Content-Type: application/x-www-form-urlencoded\n");
    client.print("Content-Length: ");
    client.print(postStr.length());
    client.print("\n\n");
    client.print(postStr);

  }
  client.stop();

  delay(15000);      // thingspeak needs minimum 15 sec delay between updates.
}
