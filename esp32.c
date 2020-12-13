#include<Wire.h>
#include<WiFi.h>
#include<ezTime.h>
#include<PubSubClient.h>
#include<DHTesp.h>
#include<HCSR04.h>
#include<LiquidCrystal_I2C.h>

const char* ssid = "TP-Link_A41A";
const char* pswrd = "vigneshmr@99";
const char* broker = "broker.hivemq.com";
String message = "ALRT 19.073933,72.862785 ";

WiFiClient wifi;

PubSubClient client(wifi);
bool lastmsg[3]={true, true, true};

int dhtPin = 17;
DHTesp dht;

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
    WiFi.begin(ssid, pswrd);
    
    Timezone timenow;
    timenow.setLocation(F("Asia/Kolkata"));
    
    client.setServer(broker, 1883);
    
    dht.setup(dhtPin, DHTesp::DHT11);
    
    lcd.init();
    lcd.backlight();
    
    lcd.createChar(1, left);
    lcd.createChar(2, mid_long);
    lcd.createChar(3, right);
    lcd.createChar(4, mid_short);
    
    lcd.clear();
  }

void reconnect()
  {
    while (!client.connected())
      {
        if (client.connect("FDU-62MFf1", "floodwatch/fdu/62MFf1/OFF", 2, false,"STOF")) 
          { 
            lcd.setCursor(14,1);
            client.publish("floodwatch/fdu/62MFf1/OFF","STON");
            lcd.print("RES200");
          }
        else 
          { 
            lcd.setCursor(14,1);
            lcd.print("ERR503");
            delay(2000);
          }
      }
  }

void loop() 
  {
    if(!client.connected()) { reconnect(); }
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
            if (distance<13 && distance>10 && lastmsg[0])
              {
                waitForSync();
                message = message + dateTime(ISO8601);
                const char* alert = message.c_str();
                client.publish("floodwatch/fdu/62MFf1", alert);
                lcd.setCursor(14,1);
                lcd.print("ALR1OK");
                lastmsg[0] = false;
              }
            else if (distance<10 && distance>7 && lastmsg[1])
              {
                waitForSync();
                message = message + dateTime(ISO8601);
                const char* alert = message.c_str();
                client.publish("floodwatch/fdu/62MFf1", alert);
                lcd.setCursor(14,1);
                lcd.print("ALR2OK");
                lastmsg[1] = false;
              }
            else if (distance<7 && distance>4 && lastmsg[2])
              {
                waitForSync();
                message = message + dateTime(ISO8601);
                const char* alert = message.c_str();
                client.publish("floodwatch/fdu/62MFf1", alert);
                lcd.setCursor(14,1);
                lcd.print("ALR3OK");
                lastmsg[2] = false;
              }
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
