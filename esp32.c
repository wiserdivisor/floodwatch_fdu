#include<Wire.h>
#inlclude<WiFi.h>
#inlclude<PubSubClient.h>
#include<DHTesp.h>
#include<HCSR04.h>
#include<LiquidCrystal_I2C.h>

int dhtPin = 17;
DHTesp dht;
MQTTClient 
int trigPin = 13;
int echoPin = 12;
UltraSonicDistanceSensor distanceSensor(trigPin, echoPin);

LiquidCrystal_I2C lcd(0x27, 20, 4);


byte left[8] =
  {
    0b10000,
    0b10000,
    0b10001,
    0b10001,
    0b10101,
    0b10101,
    0b10101,
    0b10101
  };

byte right[8] =
  {
    0b00001,
    0b00001,
    0b10001,
    0b10001,
    0b10101,
    0b10101,
    0b10101,
    0b10101
  };

byte mid_long[8] =
  {
    0b00100,
    0b00100,
    0b00100,
    0b00100,
    0b10101,
    0b10101,
    0b10101,
    0b10101
  };

byte mid_short[8] =
  {
    0b00000,
    0b00000,
    0b00100,
    0b00100,
    0b10101,
    0b10101,
    0b10101,
    0b10101
  };

void setup() 
  {     
    dht.setup(dhtPin, DHTesp::DHT11);
    
    lcd.init();
    lcd.backlight();
    
    lcd.createChar(1, left);
    lcd.createChar(2, mid_long);
    lcd.createChar(3, right);
    lcd.createChar(4, mid_short);
    
    lcd.clear();
  }

void loop() 
  {
    delay(2000);
    
    TempAndHumidity temphumid = dht.getTempAndHumidity();
    lcd.setCursor(0,0);
    lcd.print("Temp:"+String(temphumid.temperature,0));
    lcd.print("  ");
    lcd.print("Humidity:"+String(temphumid.humidity,0));

    double distance = distanceSensor.measureDistanceCm();
    lcd.setCursor(13,2);
    lcd.print("F-LEVEL");
    lcd.setCursor(13,3);
    lcd.print(distance);
    lcd.print((distance<=9)?"CM ":"CM");
    
    lcd.setCursor(0,1);
    for(int i=13; i>6; i=i-3)
      {
        if(distance-i<=0)
          {            
            lcd.print((i==7)?">>":">>>>");
          }
        else
          {
            lcd.print((i==7)?"  ":"    ");
          }
      }

    lcd.setCursor(0,2);
    for(int i=0; i<4; i++)
      {
        lcd.print(i);
        lcd.print((i==3)?" ":"  ");
      }
    
    lcd.setCursor(0,3);
    for(int i=0; i<2; i++)
      {
        lcd.write(byte(1));
        lcd.write(byte(2));
        lcd.write(byte(3));
        lcd.write(byte(4));
      }
      lcd.write(byte(1));
      lcd.write(byte(2));
      lcd.write(byte(3));    
  }
