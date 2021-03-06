#define NUM_LEDS 60
#define DATA_PIN 4
#define UPDATES_PER_SECOND 10
#define GET_VARIABLE_NAME(Variable) (#Variable).cstr()
#define ESPNAME "INFINITY-0001"

#ifndef DEBUG_PRINT
#ifdef DEBUG
#define DEBUG_PRINT(x) Serial.println(x)
#else
#define DEBUG_PRINT(x)
#endif
#endif

//void colorwaves(CRGB *ledarray, uint16_t numleds, CRGBPalette16 &palette);
void effects();
void showTime(int hr, int mn, int sec);

const TProgmemRGBGradientPalettePtr gGradientPalettes[] = {
    es_emerald_dragon_08_gp,
    Magenta_Evening_gp,
    blues_gp,
    ib04_gp,
    Sunset_Real_gp,
    fire_gp,
    /*lava_gp,
  Fuschia_7_gp,
  es_ocean_breeze_036_gp,
  es_pinksplash_07_gp,
  Analogous_1_gp,
  rgi_15_gp*/
};
// Count of how many cpt-city gradients are defined:
const uint8_t gGradientPaletteCount =
    sizeof(gGradientPalettes) / sizeof(TProgmemRGBGradientPalettePtr);
// Current palette number from the 'playlist' of color palettes
uint8_t gCurrentPaletteNumber = 1;

CRGBPalette16 gCurrentPalette(gGradientPalettes[gCurrentPaletteNumber]);

CRGBArray<NUM_LEDS> leds;
//CRGB tempLEDs[NUM_LEDS];
CRGB minutes, hours, seconds, bg, lines;
//int light_low, light_high;
boolean missed = 0, ledState = 1, multieffects = 0;
//byte  rain;

boolean autoupdate = false;

struct strConfig
{
  int light_low;
  int light_high;
  int rain;
  int gCurrentPaletteNumber;
  //String ntpServerName;
  int Update_Time_Via_NTP_Every;
  int timezoneoffset;
  int autoTimezone;
  int daylight;
  int switch_off;
  int switch_on;
} config;

bool saveDefaults()
{
  { 

    seconds.r = 0;
    seconds.g = 0;
    seconds.b = 0;
    minutes.r = 10;
    minutes.g = 44;
    minutes.b = 53;
    hours.r = 210;
    hours.g = 45;
    hours.b = 0;
    bg.r = 0;
    bg.g = 0;
    bg.b = 0;
    lines.r = 64;
    lines.g = 64;
    lines.b = 50;
    config.light_low = 0;
    config.light_high = 65;
    config.rain = 30;
    config.gCurrentPaletteNumber = 2;
    config.switch_off = 22;
    config.switch_on = 7;
    //IPGeolocation IPG(IPGeoKey);
    //IPGeo I;
    //IPG.updateStatus(&I);
    //config.timezoneoffset = (int)(I.offset * 3600);
    /*
    EEPROM.write(0, 0); // Seconds Colour
    EEPROM.write(1, 0);
    EEPROM.write(2, 0);
    EEPROM.write(3, 10); // Minutes Colour
    EEPROM.write(4, 44);
    EEPROM.write(5, 53);
    EEPROM.write(6, 210); // Hours Colour
    EEPROM.write(7, 45);
    EEPROM.write(8, 0);
    EEPROM.write(9, 0); // BG Colour
    EEPROM.write(10, 0);
    EEPROM.write(11, 0);
    EEPROM.write(12, 0);  // Light sensitivity - low
    EEPROM.write(13, 65); // Light sensitivity - high
    EEPROM.write(14, 30); // Minutes for each rainbow
    EEPROM.write(15, 2);  // Current Palette
    EEPROM.write(16, 22); // Switch Off
    EEPROM.write(17, 7);  // Switch On
    EEPROM.write(18, 64); //lines colour
    EEPROM.write(19, 64);
    EEPROM.write(20, 50);
    EEPROM.put(25,config.timezoneoffset);
    EEPROM.write(109, 6);
    EEPROM.commit();*/
  }
  return true;
}

/*bool loadDefaults()
{
  seconds.r = EEPROM.read(0);
  seconds.g = EEPROM.read(1);
  seconds.b = EEPROM.read(2);
  minutes.r = EEPROM.read(3);
  minutes.g = EEPROM.read(4);
  minutes.b = EEPROM.read(5);
  hours.r = EEPROM.read(6);
  hours.g = EEPROM.read(7);
  hours.b = EEPROM.read(8);
  bg.r = EEPROM.read(9);
  bg.g = EEPROM.read(10);
  bg.b = EEPROM.read(11);
  lines.r = EEPROM.read(18);
  lines.g = EEPROM.read(19);
  lines.b = EEPROM.read(20);
  config.light_low = EEPROM.read(12);
  config.light_high = EEPROM.read(13);
  config.rain = EEPROM.read(14);
  config.gCurrentPaletteNumber = EEPROM.read(15);
  config.switch_off = EEPROM.read(16);
  config.switch_on = EEPROM.read(17);
  EEPROM.get(25,config.timezoneoffset);
  return true;
}*/


// FastLED colorwaves

void colorwaves(CRGB *ledarray, uint16_t numleds, CRGBPalette16 &palette)
{
  static uint16_t sPseudotime = 0;
  static uint16_t sLastMillis = 0;
  static uint16_t sHue16 = 0;

  //uint8_t sat8 = beatsin88( 87, 220, 250);
  uint8_t brightdepth = beatsin88(341, 96, 224);
  uint16_t brightnessthetainc16 = beatsin88(203, (25 * 256), (40 * 256));
  uint8_t msmultiplier = beatsin88(147, 23, 60);

  uint16_t hue16 = sHue16; //gHue * 256;
  uint16_t hueinc16 = beatsin88(113, 300, 1500);

  uint16_t ms = millis();
  uint16_t deltams = ms - sLastMillis;
  sLastMillis = ms;
  sPseudotime += deltams * msmultiplier;
  sHue16 += deltams * beatsin88(400, 5, 9);
  uint16_t brightnesstheta16 = sPseudotime;

  for (uint16_t i = 0; i < numleds; i++)
  {
    hue16 += hueinc16;
    uint8_t hue8 = hue16 / 256;
    uint16_t h16_128 = hue16 >> 7;
    if (h16_128 & 0x100)
    {
      hue8 = 255 - (h16_128 >> 1);
    }
    else
    {
      hue8 = h16_128 >> 1;
    }

    brightnesstheta16 += brightnessthetainc16;
    uint16_t b16 = sin16(brightnesstheta16) + 32768;

    uint16_t bri16 = (uint32_t)((uint32_t)b16 * (uint32_t)b16) / 65536;
    uint8_t bri8 = (uint32_t)(((uint32_t)bri16) * brightdepth) / 65536;
    bri8 += (255 - brightdepth);

    uint8_t index = hue8;
    //index = triwave8( index);
    index = scale8(index, 240);

    CRGB newcolor = ColorFromPalette(palette, index, bri8);

    uint16_t pixelnumber = i;
    pixelnumber = (numleds - 1) - pixelnumber;

    nblend(ledarray[pixelnumber], newcolor, 128);
  }
}
