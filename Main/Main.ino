#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <Adafruit_SSD1306.h>
#include "Adafruit_SGP30.h"
#include <DS3231.h>



#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET    -1  // Reset pin # 
#define SCREEN_ADDRESS 0xBC
#define SEALEVELPRESSURE_HPA (1013.25)
#define BUZZER_PIN 7  // Buzzer pin
#define LED_PIN_1 51
#define LED_PIN_2 53
#define LED_PIN_3 49

Adafruit_BME280 bme; // Create BME280 object
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
Adafruit_SGP30 sgp;
DS3231 rtc(SDA, SCL);




uint32_t getAbsoluteHumidity(float temperature, float humidity) {
    // approximation formula from Sensirion SGP30 Driver Integration chapter 3.15
    const float absoluteHumidity = 216.7f * ((humidity / 100.0f) * 6.112f * exp((17.62f * temperature) / (243.12f + temperature)) / (273.15f + temperature)); // [g/m^3]
    const uint32_t absoluteHumidityScaled = static_cast<uint32_t>(1000.0f * absoluteHumidity); // [mg/m^3]
    return absoluteHumidityScaled;
}


void setup() {
 rtc.begin();
 pinMode(BUZZER_PIN, OUTPUT);
 pinMode(LED_PIN_1, OUTPUT);
 pinMode(LED_PIN_2, OUTPUT);
 pinMode(LED_PIN_3, OUTPUT);

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

 if (Serial.available()) {
        String input = Serial.readStringUntil('\n');
        if (input == "test") {
            display.clearDisplay();
            display.setTextSize(1);
            display.setTextColor(SSD1306_WHITE);
            display.setCursor(0, 0);
            display.println("HIGH CO2 LEVELS");
            display.setCursor(0, 10);
            display.println("Detected");
            display.display();

            unsigned long previousMillis = 0;
            const long interval = 500;  // Blink Time
            const int lowToneFrequency = 200;    // Low frequency of buzzer
            const int highToneFrequency = 1000;  // High frequency of buzzer
            bool isHighTone = false;
            while (true) { 
                unsigned long currentMillis = millis();
                if (currentMillis - previousMillis >= interval) {
                    previousMillis = currentMillis;
                    digitalWrite(LED_PIN_1, !digitalRead(LED_PIN_1)); // Toggle LED state
                    digitalWrite(LED_PIN_2, !digitalRead(LED_PIN_2)); // Toggle LED state
                    digitalWrite(LED_PIN_3, !digitalRead(LED_PIN_3)); // Toggle LED state
                    isHighTone = !isHighTone; // Toggle between low and high tone
                    if (isHighTone) {
                        tone(BUZZER_PIN, highToneFrequency); // High tone
                    } else {
                        tone(BUZZER_PIN, lowToneFrequency); // Low tone
                    }
                }

                if (Serial.available()) {
                    String newInput = Serial.readStringUntil('\n');
                    if (newInput == "testover") {
                        noTone(BUZZER_PIN); // Turn off the buzzer
                        digitalWrite(LED_PIN_1, LOW); // Turn off LED
                        digitalWrite(LED_PIN_2, LOW); // Turn off LED
                        digitalWrite(LED_PIN_3, LOW); // Turn off LED
                        break; // Break out of the loop
                    }
                }
            }
        }
    }

  display.clearDisplay();

  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);

 float temperatureC = bme.readTemperature(); // Read temperature in Celsius
  float temperatureF = (temperatureC * 9.0 / 5.0) + 32; // Convert to Fahrenheit

  display.setCursor(0, 10);
  display.print("Temperature: ");
  display.print(temperatureF); // Display temperature in Fahrenheit
  display.println(" *F");

  display.setCursor(0, 20);
  display.print("Pressure: ");
  display.print(bme.readPressure() / 100.0F);
  display.println(" hPa");

  display.setCursor(0, 30);
  display.print("Humidity: ");
  display.print(bme.readHumidity());
  display.println(" %");

if (! sgp.IAQmeasure()) {
    Serial.println("Measurement failed");
    display.print("Measurement failed");
    return;
  }
  display.setCursor(0, 40);
  display.print("eCO2: ");
  display.print(sgp.eCO2);
  display.print(" ppm");

if (! sgp.IAQmeasure()) {
    Serial.println("Measurement failed");
    display.print("Measurement failed");
    return;
  }

  display.setCursor(0, 50);
  display.print("TVOC: ");
  display.print(sgp.TVOC);
  display.print(" ppb\\t");


  display.setCursor(0, 0);
  display.print(rtc.getDOWStr());
  display.print(" ");
  display.print(rtc.getTimeStr());
  
 // if (! sgp.IAQmeasureRaw()) {
   // Serial.println("Raw Measurement failed");
    //return;
  //}
  //display.setCursor(0, 60);
  //display.print("Raw Ethanol ");
  //display.print(sgp.rawEthanol);
  //display.print(" ");
    


  display.display();

  Serial.println("Data displayed on OLED");

  delay(500); // Pause for .5 seconds 
}
