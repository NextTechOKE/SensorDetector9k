#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <Adafruit_SSD1306.h>
#include "Adafruit_SGP30.h"


#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET    -1  // Reset pin # 
#define SCREEN_ADDRESS 0xBC
#define SEALEVELPRESSURE_HPA (1013.25)


Adafruit_BME280 bme; // Create BME280 object
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
Adafruit_SGP30 sgp;


uint32_t getAbsoluteHumidity(float temperature, float humidity) {
    // approximation formula from Sensirion SGP30 Driver Integration chapter 3.15
    const float absoluteHumidity = 216.7f * ((humidity / 100.0f) * 6.112f * exp((17.62f * temperature) / (243.12f + temperature)) / (273.15f + temperature)); // [g/m^3]
    const uint32_t absoluteHumidityScaled = static_cast<uint32_t>(1000.0f * absoluteHumidity); // [mg/m^3]
    return absoluteHumidityScaled;
}


void setup() {
  Serial.begin(9600);

  Serial.println("SGP30 test");

  if (! sgp.begin()){
    Serial.println("Sensor not found :(");
    while (1);
  }
  Serial.print("Found SGP30 serial #");
  Serial.print(sgp.serialnumber[0], HEX);
  Serial.print(sgp.serialnumber[1], HEX);
  Serial.println(sgp.serialnumber[2], HEX);

  
  if (!bme.begin()) {
    Serial.println("Could not find a valid BME280 sensor, check wiring!");
    while (1);
  }

  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }

  display.display();
  delay(2000);
  display.clearDisplay();
}



void loop() {
  display.clearDisplay();

  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);

  display.setCursor(0, 0);
  display.print("Temperature: ");
  display.print(bme.readTemperature());
  display.println(" *C");

  display.setCursor(0, 10);
  display.print("Pressure: ");
  display.print(bme.readPressure() / 100.0F);
  display.println(" hPa");

  display.setCursor(0, 20);
  display.print("Humidity: ");
  display.print(bme.readHumidity());
  display.println(" %");

if (! sgp.IAQmeasure()) {
    Serial.println("Measurement failed");
    display.print("Measurement failed");
    return;
  }
  display.setCursor(0, 30);
  display.print("eCO2: ");
  display.print(sgp.eCO2);
  display.print(" ppm");

if (! sgp.IAQmeasure()) {
    Serial.println("Measurement failed");
    display.print("Measurement failed");
    return;
  }

  display.setCursor(0, 40);
  display.print("TVOC: ");
  display.print(sgp.TVOC);
  display.print(" ppb\\t");



  display.setCursor(0, 50);
  display.print("test"); 



  display.display();

  Serial.println("Data displayed on OLED");

  delay(2000); // Pause for 2 seconds 
}
