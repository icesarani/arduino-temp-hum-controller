#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include "DHT.h"

#define DHTTYPE DHT11
#define DHTPIN 2
#define LEDTOFF 8
#define LEDTON 9
#define LEDHOFF 10
#define LEDHON 11

char buff[20];
char buff2[20];
float hum, temp;
float humP = 0, tempP = 0;
long tant; //declaramos como bigint o long la variable tant
char buffrx [20], *pointer;//estoy declarando un array y luego le declaro un puntero

DHT dht(DHTPIN, DHTTYPE);
LiquidCrystal_I2C lcd(0x27, 16, 2);

void setup() {
    // initialize serial communication:
    Serial.begin(9600);
    pinMode(LEDTOFF, OUTPUT);
    pinMode(LEDTON, OUTPUT);
    pinMode(LEDHOFF, OUTPUT);
    pinMode(LEDHON, OUTPUT);
    dht.begin();
    lcd.begin(); //inicializa el lcd
    lcd.backlight(); //prende las luces del display
    pointer = buffrx;
}

void loop() {
    showActualTempHum();
    showAndSetTempHum();
}

void showAndSetTempHum(void) {
    if (Serial.available()) {
        *pointer = Serial.read();
        if (*pointer == '\r' || *pointer == '\n') {
            *pointer = 0;
            sprintf(buff, "%s     ", buffrx); //%s escribe un string
            lcd.setCursor(0, 0);
            lcd.print(buff);
            Serial.println(buff);
            pointer = buffrx;
        }
        else {
            if (pointer < &buffrx[20]) pointer++;
            else pointer = buffrx;
        }
        switch (buffrx[0]) {
            case 'T':
                tempP = atoi(&buffrx[1]); //atoi significa ASCII to INTEGER
                break;
            case 'H':
                humP = atoi(&buffrx[1]);
                break;
            default:
                break;
        }
        Serial.println(tempP);
        Serial.println(humP);

        printPreSetValues();
        compareHumTemp();
    }
}

void showActualTempHum(void) {
    if (millis() - tant > 500) {
        temp = dht.readTemperature();
        hum = dht.readHumidity();
        tant = millis();

        sprintf(buff, "HUM: %02d TEMP: %02d", (int)hum, (int)temp);
        lcd.setCursor(0, 0);
        lcd.print(buff);
    }
}

void compareHumTemp(void) {
    if ((int)tempP > (int)temp) {
        digitalWrite(LEDTOFF, 1);
        digitalWrite(LEDTON, 0);
    }
    if ((int)tempP < (int)temp) {
        digitalWrite(LEDTOFF, 0);
        digitalWrite(LEDTON, 1);
    }
    if ((int)humP > (int)hum) {
        digitalWrite(LEDHOFF, 1);
        digitalWrite(LEDHON, 0);
    }
    if ((int)humP < (int)hum) {
        digitalWrite(LEDHOFF, 0);
        digitalWrite(LEDHON, 1);
    }
}

void printPreSetValues(void) {
    lcd.setCursor(0, 1);
    sprintf(buff2, "PreSet H:%02d T:%02d", (int)humP, (int)tempP);
    lcd.print(buff2);
}
