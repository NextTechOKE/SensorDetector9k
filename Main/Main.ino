#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET    -1  // Reset pin # 
#define SCREEN_ADDRESS 0xBC

Adafruit_BME280 bme; // Create BME280 object
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#define SEALEVELPRESSURE_HPA (1013.25)

void setup() {
  Serial.begin(9600);
  
  if (!bme.begin()) {
    Serial.println("Could not find a valid BME280 sensor, check wiring!");
    while (1);
  }

  if(!display.begin(SSD1306_I2C, SCREEN_ADDRESS)) {
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

  display.setCursor(0, 30);
  display.print("Altitude: ");
  display.print(bme.readAltitude(SEALEVELPRESSURE_HPA));
  display.println(" m");

  display.display();

  Serial.println("Data displayed on OLED");

  delay(2000); // Pause for 2 seconds 
}
