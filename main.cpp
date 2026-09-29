#include <Wire.h>
#include <MPU6500_WE.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <TinyGPSPlus.h>

#define MPU6500_ADDR 0x68

//---------------- OLED ----------------
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
MPU6500_WE mpu = MPU6500_WE(MPU6500_ADDR);

//---------------- GSM -----------------
HardwareSerial sim800(2);

// SIM900A
#define GSM_RX 16
#define GSM_TX 17

//---------------- GPS -----------------
HardwareSerial gpsSerial(1);

#define GPS_RX 32
#define GPS_TX 27

//---------------- Helmet Detection ----------------
#define hall 34
#define ir   35
#define buzzer 18

int a, b;
String helmetStatus = "OFF";

TinyGPSPlus gps;

//---------------- PHONE ----------------
String phone1 = "+919363987016";
String phone2 = "+918072863386";

//---------------- DEFAULT LOCATION ----------------
#define DEFAULT_LAT 12.600000
#define DEFAULT_LON 80.080000

double Latitude = DEFAULT_LAT;
double Longitude = DEFAULT_LON;

//---------------- FLAGS ----------------
bool emergency = false;
bool alertSent = false;
unsigned long normalTimer = 0;

//------------------------------------------------
// Send SMS
//------------------------------------------------
void sendSMS(String number, String message)
{
  sim800.println("AT+CMGF=1");
  delay(1000);

  sim800.print("AT+CMGS=\"");
  sim800.print(number);
  sim800.println("\"");
  delay(1000);

  sim800.print(message);
  delay(500);

  sim800.write(26);     // CTRL + Z
  delay(5000);
}

//------------------------------------------------
// Make Call
//------------------------------------------------
void makeCall(String number)
{
  sim800.print("ATD");
  sim800.print(number);
  sim800.println(";");

  delay(20000);         // Ring for 20 seconds

  sim800.println("ATH");

  delay(2000);
}

//------------------------------------------------
// Setup
//------------------------------------------------
void setup()
{
  Serial.begin(115200);

  Wire.begin(21, 22);
  pinMode(hall, INPUT);
  pinMode(ir, INPUT);
  pinMode(buzzer, OUTPUT);

  // GSM
  sim800.begin(9600, SERIAL_8N1, GSM_RX, GSM_TX);

  // GPS
  gpsSerial.begin(9600, SERIAL_8N1, GPS_RX, GPS_TX);

  delay(3000);

  // OLED
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C))
  {
    Serial.println("OLED Failed");
    while (1);
  }

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);

  display.setCursor(10, 25);
  display.println("Initializing...");
  display.display();

  // MPU6500
  if (!mpu.init())
  {
    Serial.println("MPU6500 Failed");

    display.clearDisplay();
    display.setCursor(0, 25);
    display.println("MPU6500 FAILED");
    display.display();

    while (1);
  }

  Serial.println("MPU6500 Ready");

  display.clearDisplay();
  display.setCursor(20, 25);
  display.println("MPU6500 READY");
  display.display();

  delay(1500);
}
void loop()
{
  // ---------- Read GPS ----------
  while (gpsSerial.available())
  {
    gps.encode(gpsSerial.read());
  }

  // ---------- Read MPU6500 ----------
  xyzFloat acc = mpu.getGValues();

  float x = acc.x;
  float y = acc.y;
  float z = acc.z;

  // Accident Condition
  bool alert = (y > 0.80 || z < 0.70);

  // ---------- Serial ----------
  Serial.print("X: ");
  Serial.print(x, 2);

  Serial.print("  Y: ");
  Serial.print(y, 2);

  Serial.print("  Z: ");
  Serial.print(z, 2);

  if (alert)
    Serial.println("  EMERGENCY");
  else
    Serial.println("  NORMAL");

  // ---------- Helmet Detection ----------
  a = digitalRead(hall);
  b = digitalRead(ir);

  Serial.print("Hall: ");
  Serial.print(a);
  Serial.print("  IR: ");
  Serial.println(b);

  if ((a == 0) && (b == 0))
  {
    helmetStatus = "HELMET DETECT";
  }
  else if ((a == 1) && (b == 0))
  {
    helmetStatus = "NO HELMET";
  }
  else
  {
    helmetStatus = "OFF";
  }

  Serial.println(helmetStatus);

  // ---------- Buzzer Control ----------
if (alert)
{
  // Accident: Beep 1 sec ON, 1 sec OFF
  digitalWrite(buzzer, HIGH);
  delay(1000);
  digitalWrite(buzzer, LOW);
  delay(1000);
}
else if (helmetStatus == "NO HELMET")
{
  // No Helmet: Beep 1 sec ON, 1 sec OFF
  digitalWrite(buzzer, HIGH);
  delay(1000);
  digitalWrite(buzzer, LOW);
  delay(1000);
}
else
{
  digitalWrite(buzzer, LOW);
}

  // ---------- OLED ----------

  display.clearDisplay();
  display.setTextSize(1);

  display.setCursor(0, 0);
  display.println("Helmet Monitor");

  display.setCursor(0, 16);
  display.print("Helmet:");
  display.println(helmetStatus);

  // X removed

  display.setCursor(0, 30);
  display.print("Y: ");
  display.println(y, 2);

  display.setCursor(0, 44);
  display.print("Z: ");
  display.println(z, 2);
  //=============== EMERGENCY ==================

  if (alert)
  {
    emergency = true;

    display.setCursor(0, 56);
    display.print("Status: EMERGENCY");
    display.display();

    if (!alertSent)
    {
      // ---------- GPS ----------
      if (gps.location.isValid())
      {
        Latitude = gps.location.lat();
        Longitude = gps.location.lng();

        Serial.println("GPS FIXED");
      }
      else
      {
        Latitude = DEFAULT_LAT;
        Longitude = DEFAULT_LON;

        Serial.println("GPS NOT AVAILABLE");
      }

      // ---------- Google Maps ----------
      String mapLink =
        "https://maps.google.com/?q=" +
        String(Latitude, 6) + "," +
        String(Longitude, 6);

      // ---------- SMS ----------
      String message =
        "EMERGENCY ALERT!\n"
        "Accident Detected.\n\n"
        "Location:\n" +
        mapLink;

      Serial.println("Sending SMS...");

      sendSMS(phone1, message);
      delay(3000);

      sendSMS(phone2, message);
      delay(3000);

      Serial.println("SMS Sent");

      // ---------- Calls ----------
      Serial.println("Calling Phone 1...");
      makeCall(phone1);

      delay(3000);

      Serial.println("Calling Phone 2...");
      makeCall(phone2);

      Serial.println("Emergency Alert Sent");

      alertSent = true;
      normalTimer = millis();
    }
  }
  else
  {
    // Wait 5 seconds before resetting
    if (alertSent)
    {
      if (millis() - normalTimer > 5000)
      {
        alertSent = false;
        emergency = false;
      }
    }

    display.setCursor(0, 56);

    if (emergency)
      display.print("Status: EMERGENCY");
    else
      display.print("Status: NORMAL");

    display.display();
  }

  delay(500);
}