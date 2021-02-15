#define FASTLED_INTERNAL
#define FASTLED_ESP8266_RAW_PIN_ORDER
//#define FASTLED_ESP8266_D1_PIN_ORDER
#define FASTLED_ALLOW_INTERRUPTS 0
#include "FastLED.h"

#include "palette.h"
#include "DA-config.h"

#include <H4.h>
#include <H4Plugins.h>

#define LocalTZO 330  // Number of minutes that your local time is ahead (+) or behind (-) UTC/GMT.  Do not use FP maths here, just pre-calculate the number of minutes.
#define NTP1 "192.168.78.2"  // First NTP server to query time information from.
//#define NTP3 "asia.pool.ntp.org"  // Second NTP server to query time information from.
#define NTP2 "time.google.com"

void onWiFiConnect( void );
void onWiFiDisconnect( void );

H4_USE_PLUGINS(115200,H4_Q_CAPACITY,false) // Serial baud rate, Q size, SerialCmd autostop

H4P_WiFi h4wifi("DUSHYANT", "ahuja987", "NTPClock", onWiFiConnect, onWiFiDisconnect);  // Configure and actyivate the WiFi Interface.
H4P_Timekeeper h4tk(NTP1, NTP2, LocalTZO);  // Time support.

boolean ClockValid = false;  // Flag indicating whether to display the clock or not.
boolean WiFiValid = false;  // Flag indicating a valid WiFi Connection is active.
int effectsCounter = 0;

//******************************************************************************
//*** Start displaying the time once we're sync'd.
//******************************************************************************
void onRTC(){

  Serial.println("Clock valid!");
  
  ClockValid = true;
  Serial.printf("\nReceived NTP time: %s (UTC)\n\n", CSTR(h4tk.strfDateTime( "%a %Y-%m-%d %H:%M:%S", h4tk.clockEPOCHLocal()+ h4tk.DST_EU( h4tk.clockEPOCHLocal() )) ));

//Serial.print("TZO = ");
//Serial.println(h4tk._tzo);

}

//******************************************************************************
//*** WiFi Connection Established.
//******************************************************************************
void onWiFiConnect( void ) {

  Serial.println("WiFi Connected!");
  WiFiValid = true;

  Serial.printf( "IP4 Address = " );
  Serial.println( WiFi.localIP() );
  
}

//******************************************************************************
//*** WiFi Connection Lost.
//******************************************************************************
void onWiFiDisconnect( void ) {

  Serial.println("WiFi Disconnected!");
  WiFiValid = false;

}

void ShowTime( void ) {

  //**** Dsiplay the current time.
  if (ClockValid) {
    //Serial.printf("\nEPOCH Time: %d = %s (UTC)\n", h4tk.clockEPOCH(), CSTR(h4tk.strfDateTime( "%a %Y-%m-%d %H:%M:%S", h4tk.clockEPOCH() )) );
    //Serial.printf(  "Local Time: %d = %s\n", h4tk.clockEPOCHLocal(), CSTR(h4tk.strfDateTime( "%a %Y-%m-%d %H:%M:%S", h4tk.clockEPOCHLocal() )) );
      uint32_t t=h4tk.clockEPOCHLocal()/1000;
      showTime((t%86400)/3600,(t/60)%60,t%60);
//    Serial.printf("Clock time: %d - %d - %s - %s\n", h4tk.clockEPOCH(), h4tk.clockEPOCHLocal(), CSTR(h4tk.strfDateTime( "%a %Y-%m-%d %H:%M:%S", h4tk.clockEPOCHLocal()) ), CSTR(h4tk.strfDateTime( "%H:%M:%S", h4tk.clockEPOCHLocal()+ h4tk.DST_EU( h4tk.clockEPOCHLocal() )) ));
  } else {
    //Serial.println("No NTP!");
  }

}


//******************************************************************************
//*** Show Time on LEDs
//******************************************************************************

void showTime(int hr, int mn, int sec)  
{
  if ((mn % config.rain == 0 && sec <= 5))
  {
    effects();
  }
  else  if (mn == 0)
    fill_solid(leds, NUM_LEDS, bg);
  colorwaves(leds, mn, gCurrentPalette);
  leds[hr % 12 * 5] = hours;
  leds[hr % 12 * 5 + 1] = hours;
  if (hr % 12 * 5 - 1 > 0)
    leds[hr % 12 * 5 - 1] = hours;
  else
    leds[59] = hours;
  for (byte i = 0; i < 60; i += 5)
  {
    leds[i] = lines;
  }
  leds[mn] = minutes;
  if (hr < config.switch_on || hr >= config.switch_off)
    LEDS.setBrightness(constrain(0, config.light_low, 50)); // Set brightness to light_low during night - cools down LEDs and power supplies.
  else
    LEDS.setBrightness(constrain(config.light_high, 10, 255));
}

void effects()
{
  //for (int j = 0; j < 300; j++)
  //{
    fadeToBlackBy(leds, NUM_LEDS, 20);
    byte dothue = 0;
    for (int i = 0; i < 8; i++)
    {
      leds[beatsin16(i + 7, 0, NUM_LEDS)] |= CHSV(dothue, 200, 255);
      dothue += 32;
    }
    //FastLED.show();
    //FastLED.delay(1000 / UPDATES_PER_SECOND);
  //}
  if (++effectsCounter >=300 )  {
    fill_solid(leds, NUM_LEDS, bg);
    effectsCounter = 0;
  }
}

//******************************************************************************
//*** Main setup routine.
//******************************************************************************

void h4setup( void ) {
  saveDefaults();
   //**** Activate a task to display the time.
  h4.every(25, [](){ 
    ShowTime();
    FastLED.show();
  });
}