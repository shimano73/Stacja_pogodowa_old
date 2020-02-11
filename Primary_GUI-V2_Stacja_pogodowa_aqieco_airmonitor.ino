/* *************************************************************************

   Dzieki kolegom @wojtas567 i @Duch__ powstaÄąâ€ša ta wersja.


   Wszystkie potrzebne modyfikacja znajdujĂ„â€¦ siĂ„â„˘ w pliku "supla_settings.h"

 * *************************************************************************
*/


#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPUpdateServer.h>

#include <EEPROM.h>
#include <DoubleResetDetector.h> //Bilioteka by Stephen Denne

#define SUPLADEVICE_CPP
#include <SuplaDevice.h>

#include <OneWire.h>
#include <DallasTemperature.h>
#include <DHT.h>
#include "SdsDustSensor.h"
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <BH1750.h>  // support light sensor
#include "ClosedCube_SHT31D.h" // support for Nettigo Air Monitor HECA

#include <ArduinoJson.h> // 6.9.0 or later

#include "supla_settings.h"
#include "supla_eeprom.h"
#include "supla_web_server.h"
#include "supla_board_settings.h"
// #include "supla_oled.h"

extern "C" {
#include "user_interface.h"
}

#define DRD_TIMEOUT 5// Number of seconds after reset during which a subseqent reset will be considered a double reset.
#define DRD_ADDRESS 0 // RTC Memory Address for the DoubleResetDetector to use
DoubleResetDetector drd(DRD_TIMEOUT, DRD_ADDRESS);

int nr_button = 0;
int nr_relay = 0;
int invert = 0;
int nr_ds18b20 = 0;
int nr_dht = 0;
int nr_bme = 0;
int nr_sht = 0;
int nr_oled = 0;

_ds18b20_channel_t ds18b20_channel[MAX_DS18B20];
_relay_button_channel relay_button_channel[MAX_RELAY];
_bme_channel bme_channel;
_sht_channel sht_channel;
_dht_channel dht_channel[MAX_DHT];

double temp_html;
double humidity_html;

const char* Config_Wifi_name = CONFIG_WIFI_LOGIN;
const char* Config_Wifi_pass = CONFIG_WIFI_PASSWORD;

unsigned long check_delay_WiFi = 50000;
unsigned long wait_for_WiFi;

//CONFIG
int config_state = HIGH;
int last_config_state = HIGH;
unsigned long time_last_config_change;
long config_delay = 5000;

const char* www_username;
const char* www_password;
const char* update_path = UPDATE_PATH;

WiFiClient client;
ESP8266WebServer httpServer(80);
ESP8266HTTPUpdateServer httpUpdater;
ETSTimer led_timer;

// Setup a DHT instance
//DHT dht(DHTPIN, DHTTYPE);
DHT dht_sensor[MAX_DHT] = {
  { -1, -1 },
  { -1, -1 },
  { -1, -1 },
  { -1, -1 },
  { -1, -1 },
  { -1, -1 },
  { -1, -1 },
  { -1, -1 },
};

// Setup a DS18B20 instance
OneWire ds18x20[MAX_DS18B20] = 0;
//const int oneWireCount = sizeof(ds18x20) / sizeof(OneWire);
DallasTemperature sensor[MAX_DS18B20];
int ds18b20_channel_first = 0;
int dht_channel_first = 0;

int go = 0;
bool first = false;

//Luftdaten ******************************************************************************************************

bool LUFTDATEN_ON = false;

const char *luftdatenAPIHOST = "api.luftdaten.info";
const char *luftdatenAPIURL = "/v1/push-sensor-data/";
const uint16_t luftdatenAPIPort = 80;

const char *madavideAPIHOST = "api-rrd.madavi.de";
const char *madavideAPIURL = "/data.php";
const uint16_t madavideAPIPort = 80;
String  luftdatenChipId;

// AirMonitor ****************************************************************************************************


const char *airMonitorServerName = "api.airmonitor.pl";
const uint16_t airMonitorPort = 5000;
bool AIRMONITOR_ON = true;
bool AIRMONITOR_GRAPH_ON = true;
char LATITUDE[255]  = "50.084449";
char LONGITUDE[255] = "18.832895";


// aqieco**********************************************************************************************************


String aqiEcoChipId;
const char SOFTWAREVERSION[] = "2.7.3 build " __TIME__ " " __DATE__;
const char HARDWAREVERSION[] = "1.0 - ESP8266";
char THP_MODEL[255] = "BME280";
char DUST_MODEL[255] = "SDS011/21";
bool FREQUENTMEASUREMENT;
bool AQI_ECO_ON = true;
char AQI_ECO_HOST[25] = "api.aqi.eco";
char AQI_ECO_PATH[50] = "/update/f92fa5567b72d38c092209378a03cd54";
bool DEBUG = true;





//BME280***************************************************************************************************

Adafruit_BME280 bme;
 double tab_measurement[15];
 double temp_measurement;
 int nr_measurement = 1;
 int p;
 double temperature_min;
 double temperature_max;
 double sec;

// *************************** HECA (SHT30)*****************************************************************
ClosedCube_SHT31D heca;


//***************************  SDS011 **********************************************************************
int rxPin = 5;
int txPin = 4;
SdsDustSensor mySDS011(rxPin, txPin);
int CustomWorkingPeriod = 3;
double pomiar10;    // aktualny pomiar
double pomiar25;
int norma_pm25 = 25; 
int norma_pm10 = 50 ;

// ********************** BH1750 *************************************************************************** 
BH1750 myBH1750; 




//*********************** Wind Speed  **********************************************************************

#define WindImpuls 13 // GPIO 13
double diameter = 2.75;
double predkosc_mph; //Utworzenie zmiennej mile/godzinÄ‚â€žĂ‚â„˘
double predkosc_kmh; //Utowrzenie zmiennej km/h
double predkosc_ms; // Utworzenie zmiennej m/s 
double pomiar_wiatru[60];
int licz_pomiar;
int half_revolution_time = 0; //Utworzenie zmiennej przechowujÄ‚â€žĂ‚â€¦cej
double rpm = 0; //Utworzenie zmiennej RPM (obroty)
unsigned long lastmillis = 0; //Utworzenie zmiennej long lastmilis
unsigned long pm_lastmillis = 0;

//************************ Deszczomierz ********************************************************************
#define RainImpuls 14 // GPIO 14
const int interval = 500;
volatile unsigned long tiptime = millis();
double rainrate;
unsigned long curtime;




//SUPLA ****************************************************************************************************
char Supla_server[MAX_SUPLA_SERVER];
char Location_id[MAX_SUPLA_ID];
char Location_Pass[MAX_SUPLA_PASS];

//*********************************************************************************************************

void ICACHE_RAM_ATTR RpmFan();
void ICACHE_RAM_ATTR RainCount();
void ICACHE_RAM_ATTR Timer1_ISR();



//*********************************************************************************************************


void setup() {
  pinMode(WindImpuls, INPUT_PULLUP);   
  attachInterrupt(digitalPinToInterrupt(WindImpuls), RpmFan, FALLING); 

  pinMode(RainImpuls,INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(RainImpuls), RainCount, FALLING);

  noInterrupts();
  timer1_isr_init();
  timer1_attachInterrupt(Timer1_ISR);
  timer1_write(ESP.getCycleCount() + 80000000L); // 80MHz == 1sec
  interrupts();

  Serial.println(ESP.getChipId());
  aqiEcoChipId = String(ESP.getChipId());
  luftdatenChipId = String(ESP.getChipId());
  delay(50);
  Serial.begin(115200);
  delay(50);
  mySDS011.begin(9600);
  delay(50);
  CustomWorkingPeriod = read_sds011_cwp();
  delay(50);norma_pm25 = read_sds011_norm_pm25();
  norma_pm10 = read_sds011_norm_pm10();
  delay(50);
  Serial.println(mySDS011.queryFirmwareVersion().toString()); // prints firmware version
  Serial.println(mySDS011.setActiveReportingMode().toString()); // ensures sensor is in 'active' reporting mode
  Serial.println(mySDS011.setCustomWorkingPeriod(CustomWorkingPeriod).toString()); // sensor sends data every 3 minutes
  delay(50);
 
  EEPROM.begin(EEPROM_SIZE);

  client.setTimeout(500);

 

  
  if ('2' == char(EEPROM.read(EEPROM_SIZE - 1))) {
    czyszczenieEeprom();
    first_start();
  } else if ('1' != char(EEPROM.read(EEPROM_SIZE - 1))) {
    czyszczenieEepromAll();
    first_start();
    save_guid();
  }

  supla_board_configuration();

  supla_ds18b20_channel_start();
  supla_dht_start();  //
  supla_I2C_start();  // 
  supla_bme_start();  // czujnik temperatury i wilgotnoÄąâ€şci zewnĂ„â„˘trznej 
  supla_heca_start(); // heca - "podgrzewacz" powietrza zasysanego przez sds011
  supla_sensor_light_start();

  SuplaDevice.addWindSensor(); // czunjnik prĂ„â„˘dkoÄąâ€şci wiatru
  SuplaDevice.addRainSensor(); // czujnik odpadÄ‚Ĺ‚w deszczu

  add_Relay(100); // PrzeÄąâ€šĂ„â€¦cznik / "resetownik" temperatury min 
  add_Relay(101); // PrzeÄąâ€šĂ„â€¦cznik / "resetownik" temperatury max 

  temperature_min = bme.readTemperature();
  temperature_max = bme.readTemperature();
  if (drd.detectDoubleReset()) {
    drd.stop();
    gui_color = GUI_GREEN;
    Modul_tryb_konfiguracji = 2;
    Tryb_konfiguracji();
  }
  else gui_color = GUI_BLUE;

  delay(5000);
  drd.stop();

  String www_username1 = String(read_login().c_str());
  String www_password1 = String(read_login_pass().c_str());

  www_password = strcpy((char*)malloc(www_password1.length() + 1), www_password1.c_str());
  www_username = strcpy((char*)malloc(www_username1.length() + 1), www_username1.c_str());

  //  Pokaz_zawartosc_eeprom();
  read_guid();
  int Location_id = read_supla_id().toInt();
  
  strcpy(Supla_server, read_supla_server().c_str());
  strcpy(Location_Pass, read_supla_pass().c_str());

  if (String(read_wifi_ssid().c_str()) == 0
      || String(read_wifi_pass().c_str()) == 0
      || String(read_login().c_str()) == 0
      || String(read_login_pass().c_str()) == 0
      || String(read_supla_server().c_str()) == 0
      || String(read_supla_id().c_str()) == 0
      || String(read_supla_pass().c_str()) == 0
     ) {

    gui_color = GUI_GREEN;
    Modul_tryb_konfiguracji = 2;
    Tryb_konfiguracji();
  }

  read_guid();
  my_mac_adress();

  String supla_hostname = read_supla_hostname().c_str();
  supla_hostname.replace(" ", "-");
  
  
  WiFi.hostname(supla_hostname);
  // delete old config
  WiFi.disconnect(true);
  delay(1000);
  WiFi.onEvent(WiFiEvent);

  SuplaDevice.setStatusFuncImpl(&status_func);
  SuplaDevice.setDigitalReadFuncImpl(&supla_DigitalRead);
  SuplaDevice.setDigitalWriteFuncImpl(&supla_DigitalWrite);
  SuplaDevice.setTimerFuncImpl(&supla_timer);
  SuplaDevice.setName(read_supla_hostname().c_str());

  SuplaDevice.begin(GUID,              // Global Unique Identifier
                    mac,               // Ethernet MAC address
                    Supla_server,  // SUPLA server address
                    Location_id,                 // Location ID
                    Location_Pass);

  Serial.println("");
  Serial.println("Uruchamianie serwera...");

  createWebServer();

  httpUpdater.setup(&httpServer, UPDATE_PATH, www_username, www_password);
  httpServer.begin();
 
}

//*********************************** LOOP **********************************************************************

void loop() {
  
  if (millis() - pm_lastmillis >= 60000) {
        pm_lastmillis = millis();  
        read_from_sds011();
        int pomiar25_int = round(pomiar25);
        int pomiar10_int = round(pomiar10);
        if ((AQI_ECO_ON) and (first==false)) {
            sendDataToAqiEco(bme_channel.temp,bme_channel.pressure,bme_channel.humidity,0,pomiar25_int,0,pomiar10_int);
            Serial.println("WysyĹ‚anie danych do AQI.ECO");
            first=true;
        }
        if ((AIRMONITOR_ON) and (first==true)) {
             sendDataToAirMonitor(bme_channel.temp, bme_channel.pressure,bme_channel.humidity,0,pomiar25_int,0,pomiar10_int);
            Serial.println("WysyĹ‚anie danych do AirMonitor");
            first=false;
            
        }
        /*
        if (LUFTDATEN_ON){
            sendDataToLuftdaten(bme_channel.temp, bme_channel.pressure,bme_channel.humidity, 0, pomiar25_int, 0, pomiar10_int);
            Serial.println("Sending measurement data to the LuftDaten service!\n");    
            
        }
        */
  }  
 
    
  if (WiFi.status() != WL_CONNECTED && Modul_tryb_konfiguracji == 0) {
    WiFi_up();
  } else {
    httpServer.handleClient();
  }
 
  if (Modul_tryb_konfiguracji == 0) {
    SuplaDevice.iterate();
  }

  //supla_oled_timer();
  //configBTN();
  
}
//*********************************************************************************************************

// Supla.org ethernet layer
int supla_arduino_tcp_read(void *buf, int count) {
  _supla_int_t size = client.available();

  if ( size > 0 ) {
    if ( size > count ) size = count;
    return client.read((uint8_t *)buf, size);
  }

  return -1;
}

int supla_arduino_tcp_write(void *buf, int count) {
  return client.write((const uint8_t *)buf, count);
}

bool supla_arduino_svr_connect(const char *server, int port) {
  if (WiFi.status() == WL_CONNECTED) return client.connect(server, 2015); else return false;
}

bool supla_arduino_svr_connected(void) {
  return client.connected();
}

void supla_arduino_svr_disconnect(void) {
  client.stop();
}

void supla_arduino_eth_setup(uint8_t mac[6], IPAddress *ip) {
  WiFi_up();
}


// ***********************************************************************************

int supla_DigitalRead(int channelNumber, uint8_t pin) {

  int result = digitalRead(pin);
  /*Serial.print("Read(");
    Serial.print(pin);
    Serial.print("): ");
    Serial.println(result);*/
  return result;
}


// ***********************************************************************************

void supla_DigitalWrite(int channelNumber, uint8_t pin, uint8_t val) {

  /*Serial.print("Write(");
    Serial.print(pin);
    Serial.print(",");
    Serial.print(val);
    Serial.println(")");*/
 if ((pin == 100) or   (pin == 101)){
    if ((pin ==100) and (val == 1)) {
       temperature_min =  bme.readTemperature();      
    }
    if ((pin == 101) and (val ==1)) {
      temperature_max =  bme.readTemperature();      
    }
 }
 else
  digitalWrite(pin, val);
 
}






//******************************* Timer  *******************************************************************

void supla_timer() {
   
    if (millis() - lastmillis >= 1000) {
        //Aktualizuj co sekundÄ‚â€žĂ‚â„˘, bÄ‚â€žĂ‚â„˘dzie to rĂ„â€šÄąâ€šwnoznaczne z odczytem czÄ‚â€žĂ‚â„˘stotliwoĂ„Ä…Ă‚â€şci (Hz)

        lastmillis = millis();          // Aktualizacja lastmillis
     

        // -------------- odczyt danych wiatromierza ---------------------     
        noInterrupts();                   // W trakcie kalkulacji wyĂ„Ä…Ă‚â€šÄ‚â€žĂ‚â€¦cz obsĂ„Ä…Ă‚â€šugÄ‚â€žĂ‚â„˘ przerwaĂ„Ä…Ă‚â€ž
        if (half_revolution_time>0) rpm = (30000 / half_revolution_time) ;      
        interrupts() ; //PrzywrĂ„â€šÄąâ€šÄ‚â€žĂ‚â€ˇ przerwania
        if (rpm>0) {
            predkosc_mph = diameter / 12 * 3.14 * rpm * 60 / 5280;//Odczyt prÄ‚â€žĂ‚â„˘dkoĂ„Ä…Ă‚â€şci wiatru w milach/godzinÄ‚â€žĂ‚â„˘
            predkosc_mph = predkosc_mph * 3.5; // Kalibracja bĂ„Ä…Ă‚â€šÄ‚â€žĂ‚â„˘du odczytu, wartoĂ„Ä…Ă‚â€şÄ‚â€žĂ‚â€ˇ naleĂ„Ä…ÄąĹźy dobraÄ‚â€žĂ‚â€ˇ we wĂ„Ä…Ă‚â€šasnym zakresie
            predkosc_kmh = predkosc_mph * 1.609;// Zamiana mil/godzinÄ‚â€žĂ‚â„˘ na km/h
            predkosc_ms = predkosc_kmh / 3.6;     
     
        }
        pomiar_wiatru[licz_pomiar]=predkosc_ms;  
        double suma_predkosci_wiatru=0;
        for (int a=1; a<=60; a++) {
          suma_predkosci_wiatru = suma_predkosci_wiatru + pomiar_wiatru[a];
        }
        pomiar_wiatru[0] = suma_predkosci_wiatru / 60;
        licz_pomiar++;    
        if (licz_pomiar==60) licz_pomiar=1;        
    }

   
}

SuplaDeviceCallbacks supla_arduino_get_callbacks(void) {
  SuplaDeviceCallbacks cb;

  cb.tcp_read = &supla_arduino_tcp_read;
  cb.tcp_write = &supla_arduino_tcp_write;
  cb.eth_setup = &supla_arduino_eth_setup;
  cb.svr_connected = &supla_arduino_svr_connected;
  cb.svr_connect = &supla_arduino_svr_connect;
  cb.svr_disconnect = &supla_arduino_svr_disconnect;
  cb.get_temperature = &get_temperature;
  cb.get_pressure = &get_pressure;
  cb.get_temperature_and_humidity = &get_temperature_and_humidity;
  cb.get_rgbw_value = NULL;
  cb.set_rgbw_value = NULL;
  cb.get_wind=&get_wind;  
  cb.get_rain = get_rain;
  cb.read_supla_relay_state = &read_supla_relay_state;
  cb.save_supla_relay_state = &save_supla_relay_state;

  return cb;
}
//*********************************************************************************************************

void createWebServer() {

  httpServer.on("/", []() {
    if (Modul_tryb_konfiguracji == 0) {
      if (!httpServer.authenticate(www_username, www_password))
        return httpServer.requestAuthentication();
    }
    httpServer.send(200, "text/html", supla_webpage_start(0));
  });

  httpServer.on("/set0", []() {
    if (Modul_tryb_konfiguracji == 0) {
      if (!httpServer.authenticate(www_username, www_password))
        return httpServer.requestAuthentication();
    }

    save_wifi_ssid(httpServer.arg("wifi_ssid"));
    save_wifi_pass(httpServer.arg("wifi_pass"));
    save_login( httpServer.arg("modul_login"));
    save_login_pass(httpServer.arg("modul_pass"));
    save_supla_server(httpServer.arg("supla_server"));
    save_supla_hostname(httpServer.arg("supla_hostname"));
    save_supla_id(httpServer.arg("supla_id"));
    save_supla_pass(httpServer.arg("supla_pass"));
    if (nr_button > 0) {
      for (int i = 1; i <= nr_button; ++i) {
        String button = "button_set";
        button += i;
        save_supla_button_type(i, httpServer.arg(button));
      }
    }
    if (nr_relay > 0) {
      for (int i = 1; i <= nr_relay; ++i) {
        String relay = "relay_set";
        relay += i;
        save_supla_relay_flag(i, httpServer.arg(relay));
      }
    }
    if (nr_ds18b20 > 0) {
      for (int i = 0; i < nr_ds18b20; i++) {
        String ds_name = "ds18b20_name_id_";
        ds_name += i;
        String name = httpServer.arg(ds_name);
        save_DS18b20_name(name, i);
        ds18b20_channel[i].name = name;
        if (ds18b20_channel[i].type == 1) {
          String ds_address = "ds18b20_channel_id_";
          ds_address += i;
          String address = httpServer.arg(ds_address);
          save_DS18b20_address(address, i);
          ds18b20_channel[i].address = address;
          read_DS18b20_address(i);
        }
      }
    }
    if (nr_bme > 0) {
      bme_channel.elevation = httpServer.arg("supla_bme_elevation").toInt();
      save_bme_elevation(bme_channel.elevation);
    }
    CustomWorkingPeriod =  httpServer.arg("SDS011_CWP").toInt();    
    save_sds011_cwp(CustomWorkingPeriod);
    
    norma_pm25 = httpServer.arg("SDS011_NORM25").toInt();    
    Serial.print("norma 2.5 = ");Serial.println(norma_pm25);
    save_sds011_norm_pm25(norma_pm25);

    norma_pm10 = httpServer.arg("SDS011_NORM10").toInt();          
    Serial.print("norma 10 = ");Serial.println(norma_pm10);
    save_sds011_norm_pm10(norma_pm10);
    
    httpServer.send(200, "text/html", supla_webpage_start(1));
  });

  //************************************************************

  httpServer.on("/firmware_up", []() {
    if (Modul_tryb_konfiguracji == 0) {
      if (!httpServer.authenticate(www_username, www_password))
        return httpServer.requestAuthentication();
    }
    httpServer.send(200, "text/html", supla_webpage_upddate());
  });

  //****************************************************************************************************************************************
  httpServer.on("/reboot", []() {
    if (Modul_tryb_konfiguracji == 0) {
      if (!httpServer.authenticate(www_username, www_password))
        return httpServer.requestAuthentication();
    }
    httpServer.send(200, "text/html", supla_webpage_start(2));
    delay(100);
    resetESP();
  });
  httpServer.on("/setup", []() {
    if (Modul_tryb_konfiguracji == 0) {
      if (!httpServer.authenticate(www_username, www_password))
        return httpServer.requestAuthentication();
    }
    SetupDS18B20Multi();
    httpServer.send(200, "text/html", supla_webpage_search(1));
  });

 
  httpServer.on("/search", []() {
    if (Modul_tryb_konfiguracji == 0) {
      if (!httpServer.authenticate(www_username, www_password))
        return httpServer.requestAuthentication();
    }
    httpServer.send(200, "text/html", supla_webpage_search(0));
  });
  httpServer.on("/eeprom", []() {
    if (Modul_tryb_konfiguracji == 0) {
      if (!httpServer.authenticate(www_username, www_password))
        return httpServer.requestAuthentication();
    }
    czyszczenieEeprom();
    httpServer.send(200, "text/html", supla_webpage_start(3));
  });
}

//*********************************** TRYB_KONFIGURACJI *****************************************************************************************************
void Tryb_konfiguracji() {
  supla_led_blinking(LED_CONFIG_PIN, 100);
  my_mac_adress();
  Serial.print("Tryb konfiguracji: ");
  Serial.println(Modul_tryb_konfiguracji);

  WiFi.softAPdisconnect(true);
  delay(100);
  WiFi.disconnect(true);
  delay(100);
  WiFi.mode(WIFI_AP_STA);
  delay(100);
  WiFi.softAP(Config_Wifi_name, Config_Wifi_pass);
  delay(100);
  Serial.println("Tryb AP");
  createWebServer();
  httpServer.begin();
  Serial.println("Start Serwera");

  if (Modul_tryb_konfiguracji == 2) {
    supla_ds18b20_channel_start();
    supla_dht_start();

    while (1) {
      httpServer.handleClient();
      //supla_oled_timer();
      //    SuplaDevice.iterate();
    }
  }
}

//***************************** WIFI_UP*********************************************************


void WiFi_up() {
  if ( WiFi.status() != WL_CONNECTED
       && millis() >= wait_for_WiFi ) {

    supla_led_blinking(LED_CONFIG_PIN, 500);
    WiFi.disconnect(true);
    String esid = String(read_wifi_ssid().c_str());
    String epass = String(read_wifi_pass().c_str());
    Serial.println("WiFi init ");
    Serial.print("SSID: ");
    Serial.println(esid);
    Serial.print("PASSWORD: ");
    Serial.println(epass);

    WiFi.mode(WIFI_STA);
    WiFi.begin(esid.c_str(), epass.c_str());

    wait_for_WiFi = millis() + check_delay_WiFi;
  }
}

//*************************** WIFIEVENT ***********************************************

void WiFiEvent(WiFiEvent_t event) {
  switch (event) {
    case WIFI_EVENT_STAMODE_GOT_IP:
      //String esid = String(read_wifi_ssid().c_str());
      Serial.print("WiFi connected SSID: ");
      Serial.println(String(read_wifi_ssid().c_str()));
      Serial.print("localIP: ");
      Serial.println(WiFi.localIP());
      Serial.print("subnetMask: ");
      Serial.println(WiFi.subnetMask());
      Serial.print("gatewayIP: ");
      Serial.println(WiFi.gatewayIP());
      Serial.print("siÄąâ€ša sygnaÄąâ€šu (RSSI): ");
      Serial.print(WiFi.RSSI());
      Serial.println(" dBm");
      break;
    case WIFI_EVENT_STAMODE_DISCONNECTED:
      Serial.println("WiFi lost connection");
      break;
  }
}

void first_start(void) {
  EEPROM.begin(EEPROM_SIZE);
  delay(100);
  EEPROM.write(EEPROM_SIZE - 1, '1');
  EEPROM.end();
  delay(100);
  save_login(DEFAULT_LOGIN);
  save_login_pass(DEFAULT_PASSWORD);
  save_supla_hostname(DEFAULT_HOSTNAME);
  save_bme_elevation(120);
  save_sds011_cwp(10);
  save_sds011_norm_pm10(11);
  save_sds011_norm_pm25(11);
}

String read_rssi(void) {
  int32_t rssi = WiFi.RSSI();
  return String(rssi) ;
}

int32_t read_rssi_oled (void) {
  int32_t rssi = WiFi.RSSI();
  if (WiFi.status() != WL_CONNECTED) return -1;
  if (rssi <= -100) return 0;
  if (rssi >= -50) return 100;
  return (2 * (rssi + 100) );
}

//**************************************GET_TEMPERATURE_AND_HUMIDITY******************************************************

void get_temperature_and_humidity(int channelNumber, double *temp, double *humidity) {
   switch (channelNumber)
        { 
          case 0:         
                 
                *temp = (pomiar25 * 100)/norma_pm25;
                Serial.print("SDS011 PM2.5 = ");
                Serial.print(*temp);
                Serial.print("%");
    
                *humidity = (pomiar10 * 100) /norma_pm10;
                Serial.print(" PM10 = ");
                Serial.print(*humidity);            
                Serial.println("%");    
          break; 
         case 2:         
                if (nr_measurement <=13)
                    {
                      nr_measurement++;
                    }
                else { 
                      nr_measurement=1;
                    }
           
                tab_measurement[nr_measurement] = bme.readTemperature();

                p = 0;
                while (p == 0)
                {
                  p = 1;
                  for (int sort = 1; sort <= 13; sort++)
                  {
                    if (tab_measurement[sort] > tab_measurement[sort + 1])
                      {
                        temp_measurement = tab_measurement[sort];
                        tab_measurement[sort] = tab_measurement[sort + 1];
                        tab_measurement[sort + 1] = temp_measurement;
                        p = 0;
                      }     
                  }
                }
          
                tab_measurement[0] = (tab_measurement[5] + tab_measurement[6] + tab_measurement[7] + tab_measurement[8] + tab_measurement[9]) / 5;
                tab_measurement[0] = bme.readTemperature(); // bez wyliczania uÄąâ€şrednionego pomiaru 
                
                bme_channel.temp = tab_measurement[0];
                *temp=tab_measurement[0];                
                Serial.print("BME 280 Temperature = ");
                Serial.print(tab_measurement[0]);
                Serial.print("Ă‚Â°C ");
                
                 bme_channel.humidity = bme.readHumidity();
                 *humidity = bme_channel.humidity;
                Serial.print(" Humidity = ");
                Serial.print(bme.readHumidity());
                Serial.println(" %");
                if ( isnan(*temp) || isnan(*humidity) ) {
                    *temp = -275;
                    *humidity = -1;
                }
                break;
         case 3:
                if (tab_measurement[0] < temperature_min ) temperature_min = tab_measurement[0];
                if (tab_measurement[0] > temperature_max ) temperature_max = tab_measurement[0];
                *temp = temperature_min;
                *humidity = temperature_max;
         break; 
         case 4:
             SHT31D result = heca.periodicFetchData();
               if (result.error == SHT3XD_NO_ERROR) {
                    *temp = result.t;
                    Serial.print("SHT31D  Temperature = ");
                    Serial.print(*temp);
                    Serial.print("Ă‚Â°C  ") ;
                    
                    *humidity = result.rh;
                    Serial.print("Humidity = ");
                    Serial.print(*humidity); 
                    Serial.println("%");
               } else {
                    *temp = +128;
                    *humidity = -1;
               }
         break;
        
        }
}


//************************************ GET_PRESSURE ********************************************************

double get_pressure(int channelNumber, double last_val) {
  double pressure = -275;
  double pressure_sea = -275;

  pressure = bme.readPressure();
  pressure_sea = pressure / pow(2.718281828, - (bme_channel.elevation / ((273.15 + bme_channel.temp) * 29.263))) / 100.0F;

  bme_channel.pressure = pressure / 100.0F;
  Serial.print("BME 280 Pressure = ");
  Serial.print(bme_channel.pressure);
  
  bme_channel.pressure_sea = pressure_sea;
  Serial.print("nPa   ");
  Serial.print("Preseure sea = ");
  Serial.print(bme_channel.pressure_sea);
  
  Serial.print("hPa   ");
  Serial.print("Elevation  = ");
  Serial.print(bme_channel.elevation);
  Serial.println("npm   ");
  
  return  pressure_sea;
}

//********************************************************************************************
double get_wind(int channelNumber, double last_val){
  //double wind_speed;  
  //wind_speed = pomiar_wiatru[0];
  //wind_speed = predkosc_ms;
  Serial.print("Wind meter = ");
  Serial.print(pomiar_wiatru[0]);
  Serial.println(" m/s"); 
  return pomiar_wiatru[0];
}

//********************************************************************************************

double get_temperature(int channelNumber, double last_val) {
  double t = -275;

  int i = channelNumber - ds18b20_channel_first;
  // if ( sensor[i].getDeviceCount() > 0 ) {
  if ( ds18b20_channel[i].address == "FFFFFFFFFFFFFFFF" ) return -275;
  if ( millis() - ds18b20_channel[i].lastTemperatureRequest < 0) {
    ds18b20_channel[i].lastTemperatureRequest = millis();
  }

  if (ds18b20_channel[i].TemperatureRequestInProgress == false) {
    sensor[i].requestTemperaturesByAddress(ds18b20_channel[i].deviceAddress);
    ds18b20_channel[i].TemperatureRequestInProgress = true;
  }

  if ( millis() - ds18b20_channel[i].lastTemperatureRequest > 1000) {
    if ( ds18b20_channel[i].type == 0 ) {
      sensor[i].requestTemperatures();
      t = sensor[i].getTempCByIndex(0);
    } else {
      t = sensor[i].getTempC(ds18b20_channel[i].deviceAddress);
    }
    if (t == -127) t = -275;
    ds18b20_channel[i].last_val = t;
    ds18b20_channel[i].lastTemperatureRequest = millis();
    ds18b20_channel[i].TemperatureRequestInProgress = false;
  }

  switch (channelNumber)
        {
   
        case 5:
         t =  myBH1750.readLightLevel();
          Serial.print("Light Level : ");
         Serial.println(t,2);
        break;
   
    }         
  // }
  return t;
}

void supla_led_blinking_func(void *timer_arg) {
  int val = digitalRead(LED_CONFIG_PIN);
  digitalWrite(LED_CONFIG_PIN, val == HIGH ? 0 : 1);
}

void supla_led_blinking(int led, int time) {

  os_timer_disarm(&led_timer);
  os_timer_setfn(&led_timer, supla_led_blinking_func, NULL);
  os_timer_arm (&led_timer, time, true);

}

void supla_led_blinking_stop(void) {
  os_timer_disarm(&led_timer);
  digitalWrite(LED_CONFIG_PIN, 1);
}

void supla_led_set(int ledPin) {
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, 1);
}

void supla_ds18b20_channel_start(void) {
  if (nr_ds18b20 > 0 ) {
    Serial.println("DS18B2 init");
    Serial.print("Parasite power is: ");
    if ( sensor[0].isParasitePowerMode() ) {
      Serial.println("ON");
    } else {
      Serial.println("OFF");
    }
    for (int i = 0; i < nr_ds18b20; i++) {
      sensor[i].setOneWire(&ds18x20[i]);
      sensor[i].begin();
      if (ds18b20_channel[i].type == 1) {
        sensor[i].setResolution(ds18b20_channel[i].deviceAddress, TEMPERATURE_PRECISION);
      } else {
        if (sensor[i].getAddress(ds18b20_channel[i].deviceAddress, 0)) sensor[i].setResolution(ds18b20_channel[i].deviceAddress, TEMPERATURE_PRECISION);
      }
    }
  }
}

void supla_dht_start(void) {
  if (nr_dht > 0 ) {
    for (int i = 0; i < nr_dht; i++) {
      dht_sensor[i].begin();
    }
  }
}


void supla_I2C_start(){
   Wire.begin(SDA, SCL);
}

void supla_bme_start(void) {
  if (nr_bme > 0) {
    // Inicjalizacja BME280
    Wire.begin(SDA, SCL);
    if (!bme.begin(0x76)) { //0x76
      Serial.println("Nie znaleleziono czujnika BME280, sprawdz poprawnoÄąâ€şĂ„â€ˇ podÄąâ€šĂ„â€¦czenia i okablowanie!");
      //while (1);
    }
  }
}

void supla_heca_start(void) {
  if (!initHECA()) Serial.println("Nie znaleleziono moduÄąâ€šu HECA, sprawdz poprawnoÄąâ€şĂ„â€ˇ podÄąâ€šĂ„â€¦czenia i okablowanie!");        
}

void supla_sensor_light_start(void) {
  myBH1750.begin(); 
}

void add_Sensor(int sensor) {
  SuplaDevice.addSensorNO(sensor);
}

void add_Roller_Shutter_Buttons(int channel, int button1, int button2) {
  SuplaDevice.setRollerShutterButtons(channel, button1, button2);
}

void add_Roller_Shutter_Relays(int relay1, int relay2) {
  SuplaDevice.addRollerShutterRelays(relay1, relay2);
  //  SuplaDevice.setRollerShutterFuncImpl(&supla_rs_SavePosition, &supla_rs_LoadPosition, &supla_rs_SaveSettings, &supla_rs_LoadSettings);
}

void add_Led_Config(int led) {
  supla_led_set(led);
}

void add_Config(int pin) {
  pinMode(pin, INPUT);
}

void add_Relay(int relay) {
  relay_button_channel[nr_relay].relay = relay;
  relay_button_channel[nr_relay].invert = 0;
  nr_relay++;
  //SuplaDevice.addRelay(relay);
  SuplaDevice.addRelayButton(relay, -1, 0, read_supla_relay_flag(nr_relay));
}

void add_Relay_Invert(int relay) {
  relay_button_channel[nr_relay].relay = relay;
  relay_button_channel[nr_relay].invert = 1;
  nr_relay++;
  //SuplaDevice.addRelay(relay, true);
  SuplaDevice.addRelayButton(relay, -1, 0, read_supla_relay_flag(nr_relay), true);
}

void add_DHT11_Thermometer(int thermpin) {
  int channel = SuplaDevice.addDHT11();
  if (nr_dht == 0) dht_channel_first = channel;

  dht_sensor[nr_dht] = { thermpin, DHT11 };
  dht_channel[nr_dht].channel = channel;
  nr_dht++;
}

void add_DHT22_Thermometer(int thermpin) {
  int channel = SuplaDevice.addDHT22();
  if (nr_dht == 0) dht_channel_first = channel;

  dht_sensor[nr_dht] = { thermpin, DHT22 };
  dht_channel[nr_dht].channel = channel;
  nr_dht++;
}

void add_DS18B20_Thermometer(int thermpin) {
  int channel = SuplaDevice.addDS18B20Thermometer();
  if (ds18b20_channel_first == 0) ds18b20_channel_first = channel;

  ds18x20[nr_ds18b20] = thermpin;
  ds18b20_channel[nr_ds18b20].pin = thermpin;
  ds18b20_channel[nr_ds18b20].channel = channel;
  ds18b20_channel[nr_ds18b20].type = 0;
  ds18b20_channel[nr_ds18b20].name = read_DS18b20_name(nr_ds18b20);
  nr_ds18b20++;
}

void add_DS18B20Multi_Thermometer(int thermpin) {
  for (int i = 0; i < MAX_DS18B20; i++) {
    int channel = SuplaDevice.addDS18B20Thermometer();
    if (i == 0) ds18b20_channel_first = channel;

    ds18x20[nr_ds18b20] = thermpin;
    ds18b20_channel[nr_ds18b20].pin = thermpin;
    ds18b20_channel[nr_ds18b20].channel = channel;
    ds18b20_channel[nr_ds18b20].type = 1;
    ds18b20_channel[nr_ds18b20].address = read_DS18b20_address(i);
    ds18b20_channel[nr_ds18b20].name = read_DS18b20_name(i);
    nr_ds18b20++;
  }
}

void add_BME280_Sensor() {
  bme_channel.pressure_channel = SuplaDevice.addPressureSensor();
  bme_channel.temperature_channel = SuplaDevice.addDHT22();

  bme_channel.elevation = read_bme_elevation();
  nr_bme++;
}
void add_SHT_Sensor() {
  sht_channel.temperature_channel = SuplaDevice.addDHT22();
  nr_sht++;
}



void add_Relay_Button(int relay, int button, int type) {
  return add_Relay_Button(relay, button, type, 0);
}

void add_Relay_Button_Invert(int relay, int button, int type) {
  return add_Relay_Button_Invert(relay, button, type, 0);
}

void add_Relay_Button(int relay, int button, int type, int DurationMS) {
  relay_button_channel[nr_relay].relay = relay;
  relay_button_channel[nr_relay].invert = 0;
  nr_button++;
  nr_relay++;
  if (type == CHOICE_TYPE) {
    int select_button = read_supla_button_type(nr_button);
    type = select_button;
  }

  SuplaDevice.addRelayButton(relay, button, type, read_supla_relay_flag(nr_relay), DurationMS);
}

void add_Relay_Button_Invert(int relay, int button, int type, int DurationMS) {
  relay_button_channel[nr_relay].relay = relay;
  relay_button_channel[nr_relay].invert = 1;
  nr_button++;
  nr_relay++;
  if (type == CHOICE_TYPE) {
    int select_button = read_supla_button_type(nr_button);
    type = select_button;
  }

  SuplaDevice.addRelayButton(relay, button, type, read_supla_relay_flag(nr_relay), true, DurationMS);
}
/*
void add_Oled() {
  supla_oled_start();
  nr_oled ++;
}
*/
//Convert device id to String
String GetAddressToString(DeviceAddress deviceAddress) {
  String str = "";
  for (uint8_t i = 0; i < 8; i++) {
    if ( deviceAddress[i] < 16 ) str += String(0, HEX);
    str += String(deviceAddress[i], HEX);
  }
  return str;
}

void SetupDS18B20Multi() {
  DeviceAddress devAddr[MAX_DS18B20];  //An array device temperature sensors
  int numberOfDevices; //Number of temperature devices found
  numberOfDevices = sensor[0].getDeviceCount();
  // Loop through each device, print out address
  for (int i = 0; i < numberOfDevices; i++) {
    sensor[i].requestTemperatures();
    // Search the wire for address
    if ( sensor[i].getAddress(devAddr[i], i) ) {
      Serial.print("Found device ");
      Serial.println(i, DEC);
      Serial.println("with address: " + GetAddressToString(devAddr[i]));
      Serial.println();
      save_DS18b20_address(GetAddressToString(devAddr[i]), i);
      ds18b20_channel[i].address = read_DS18b20_address(i);
    } else {
      Serial.print("Not Found device");
      Serial.print(i, DEC);
      // save_DS18b20_address("", i);
    }
    //Get resolution of DS18b20
    Serial.print("Resolution: ");
    Serial.print(sensor[i].getResolution( devAddr[i] ));
    Serial.println();

    //Read temperature from DS18b20
    float tempC = sensor[i].getTempC( devAddr[i] );
    Serial.print("Temp C: ");
    Serial.println(tempC);
  }
}

void resetESP() {
  WiFi.forceSleepBegin();
  wdt_reset();
  ESP.restart();
  while (1)wdt_reset();
}

void configBTN() {
  //CONFIG ****************************************************************************************************
  int config_read = digitalRead(CONFIG_PIN);
  if (config_read != last_config_state) {
    time_last_config_change = millis();
  }
  if ((millis() - time_last_config_change) > config_delay) {
    if (config_read != config_state) {
      Serial.println("Triger sate changed");
      config_state = config_read;
      if (config_state == LOW && Modul_tryb_konfiguracji != 1) {
        gui_color = GUI_GREEN;
        Modul_tryb_konfiguracji = 1;
        Tryb_konfiguracji();
        client.stop();
      } else if (config_state == LOW && Modul_tryb_konfiguracji == 1) {
        resetESP();
      }
    }
  }
  last_config_state = config_read;
}


//******************************* HECA init *******************************************************************
 
 bool initHECA() {

  heca.begin(0x44);
  //heca.begin(addr);
  if (heca.periodicStart(SHT3XD_REPEATABILITY_HIGH, SHT3XD_FREQUENCY_1HZ) != SHT3XD_NO_ERROR) {
    return false;
  } else {
    // temperature set, temperature clear, humidity set, humidity clear
    if (heca.writeAlertHigh(120, 119, 63, 60) != SHT3XD_NO_ERROR) {
      Serial.println(" [HECA ERROR] Cannot set Alert HIGH");
    }
    if (heca.writeAlertLow(-5, 5, 0, 1) != SHT3XD_NO_ERROR) {
      Serial.println(" [HECA ERROR] Cannot set Alert LOW");      
    }
    if (heca.clearAll() != SHT3XD_NO_ERROR) {
      Serial.println(" [HECA ERROR] Cannot clear register");
    }
    return true;
  }
}


//*************************************** RPM_FAN ***********************************************************************

void RpmFan() {
  unsigned long static last_event = 0;
  if (millis() - last_event < 5) {   //debouncing
    return;
  }
  half_revolution_time = (millis() - last_event);
  last_event = millis();
 
}

//*****************************OPADY_DESZCZ*******************************************

double get_rain(int channelNumber, double t) {


//double t = -275;  
    t = rainrate*100;
    return t;
}  
  

//**********************************WYLICZ OPADY****************************************
void RainCount() {
  float de;
  // Grab the current ms count for common calculations
  unsigned long curtime = millis();
  
  // Make sure we don't record bounces
  if ((curtime - tiptime) < interval) {
    return;
  }
// How long since the last tip?
  unsigned long tipcount = curtime - tiptime;
  tiptime = curtime;
  
  // Calculate mm/hr from period between cup tips
  rainrate = 914400.0 / tipcount;
  
  Serial.print("Cup tip: ");
  Serial.print(tipcount);
  Serial.println("ms");  
  
  Serial.print("Opady deszczu: ");
  Serial.print(rainrate/10);
  Serial.println("l/m2");  
  Serial.println("mm/h");  

}


void Timer1_ISR (void) {

  timer1_write(1000); // 80MHz == 1sec 
}


 //**************************************** SDS011 **************************************************************************

 void read_from_sds011() {
   
      PmResult pm = mySDS011.readPm();
      Serial.print("Data request from SDS011 -> ");
      if (pm.isOk()) {
          Serial.println("Valid data received");
          pomiar25 = double(pm.pm25);
          pomiar10 = double(pm.pm10);        
       }                     
       else {
         Serial.print("Could not read values from sensor, reason : ");
         Serial.println(pm.statusToString());
       }      
 }

 //******************************************** AqiEco *****************************************************************

void sendDataToAqiEco(double currentTemperature, double currentPressure, double currentHumidity, int averagePM1, int averagePM25, int averagePM4, int averagePM10) {
  if (!(AQI_ECO_ON)) {
    return;
  }
  Serial.print("Typ czujnika smogu : ");
  Serial.print(DUST_MODEL); Serial.print(" Typ czujnika THP : ");
   Serial.println(THP_MODEL);
  StaticJsonDocument<768> jsonBuffer;
  JsonObject json = jsonBuffer.to<JsonObject>();
  json["esp8266id"] = aqiEcoChipId;
  json["software_version"] = "Smogomierz_" + String(SOFTWAREVERSION);
  JsonArray sensordatavalues = json.createNestedArray("sensordatavalues");

  if (!strcmp(DUST_MODEL, "PMS7003")) {
    JsonObject P0 = sensordatavalues.createNestedObject();
    P0["value_type"] = "PMS_P0";
    P0["value"] = averagePM1;
    JsonObject P1 = sensordatavalues.createNestedObject();
    P1["value_type"] = "PMS_P1";
    P1["value"] = averagePM10;
    JsonObject P2 = sensordatavalues.createNestedObject();
    P2["value_type"] = "PMS_P2";
    P2["value"] = averagePM25;
  } else if (!strcmp(DUST_MODEL, "SDS011/21")) {
    JsonObject P1 = sensordatavalues.createNestedObject();
    P1["value_type"] = "SDS_P1";
    P1["value"] = averagePM10;
    JsonObject P2 = sensordatavalues.createNestedObject();
    P2["value_type"] = "SDS_P2";
    P2["value"] = averagePM25;
    Serial.print("Przygotowanie danych SDS011");
  } else if (!strcmp(DUST_MODEL, "HPMA115S0")) {
    JsonObject P1 = sensordatavalues.createNestedObject();
    P1["value_type"] = "HPM_P1";
    P1["value"] = averagePM10;
    JsonObject P2 = sensordatavalues.createNestedObject();
    P2["value_type"] = "HPM_P2";
    P2["value"] = averagePM25;
  }

  if (!strcmp(THP_MODEL, "BME280")) {
    JsonObject temperature = sensordatavalues.createNestedObject();
    temperature["value_type"] = "BME280_temperature";
    temperature["value"] = String(currentTemperature);
    JsonObject humidity = sensordatavalues.createNestedObject();
    humidity["value_type"] = "BME280_humidity";
    humidity["value"] = String(currentHumidity);
    JsonObject pressure = sensordatavalues.createNestedObject();
    pressure["value_type"] = "BME280_pressure";
    pressure["value"] = String(currentPressure * 100); //hPa -> Pa
     Serial.print("Przygotowanie danych BME280");
   
  } else if (!strcmp(THP_MODEL, "BMP280")) {
    JsonObject temperature = sensordatavalues.createNestedObject();
    temperature["value_type"] = "BMP280_temperature";
    temperature["value"] = String(currentTemperature);
    JsonObject pressure = sensordatavalues.createNestedObject();
    pressure["value_type"] = "BMP280_pressure";
    pressure["value"] = String(currentPressure * 100); //hPa -> Pa
  } else if (!strcmp(THP_MODEL, "HTU21")) {
    JsonObject temperature = sensordatavalues.createNestedObject();
    temperature["value_type"] = "HTU21_temperature";
    temperature["value"] = String(currentTemperature);
    JsonObject humidity = sensordatavalues.createNestedObject();
    humidity["value_type"] = "HTU21_humidity";
    humidity["value"] = String(currentHumidity);
  } else if (!strcmp(THP_MODEL, "DHT22")) {
    JsonObject temperature = sensordatavalues.createNestedObject();
    temperature["value_type"] = "DHT22_temperature";
    temperature["value"] = String(currentTemperature);
    JsonObject humidity = sensordatavalues.createNestedObject();
    humidity["value_type"] = "DHT22_humidity";
    humidity["value"] = String(currentHumidity);
  } else if (!strcmp(THP_MODEL, "SHT1x")) {
    JsonObject temperature = sensordatavalues.createNestedObject();
    temperature["value_type"] = "SHT1x_temperature";
    temperature["value"] = String(currentTemperature);
    JsonObject humidity = sensordatavalues.createNestedObject();
    humidity["value_type"] = "SHT1x_humidity";
    humidity["value"] = String(currentHumidity);
  }

/*
    JsonObject heater_temperature = sensordatavalues.createNestedObject();
    heater_temperature["value_type"] = "heater_temperature";
    heater_temperature["value"] = String("5.99");
    JsonObject heater_humidity = sensordatavalues.createNestedObject();
    heater_humidity["value_type"] = "heater_Humidity";
    heater_humidity["value"] = String("50.99");
  */
  WiFiClient client;

  Serial.print("\nconnecting to ");
  Serial.println(AQI_ECO_HOST);

  if (!client.connect(AQI_ECO_HOST, 80)) {
    Serial.println("connection failed");
    delay(1000);
    return;
  }
  delay(100);

  client.print("POST ");
  client.print(AQI_ECO_PATH);
  client.println(" HTTP/1.1");
  client.print("Host: ");
  client.println(AQI_ECO_HOST);
  client.println("Content-Type: application/json");
  client.println("X-PIN: 1");
  client.print("X-Sensor: smogomierz-");
  client.println(aqiEcoChipId);
  client.print("Content-Length: ");
  client.println(measureJson(json));
  client.println("Connection: close");
  client.println();
  serializeJson(json, client);

  String line = client.readStringUntil('\r');
  Serial.println(line);
  // TODO: Support wrong error (!= 200)

  if (DEBUG) {
    Serial.println();
    Serial.print("POST ");
    Serial.print(AQI_ECO_PATH);
    Serial.println(" HTTP/1.1");
    Serial.print("Host: ");
    Serial.println(AQI_ECO_HOST);
    Serial.println("Content-Type: application/json");
    Serial.println("X-PIN: 1");
    Serial.print("X-Sensor: smogomierz-");
    Serial.println(aqiEcoChipId);
    Serial.print("Content-Length: ");
    Serial.println(measureJson(json));
    Serial.println("Connection: close");
    Serial.println();
    serializeJsonPretty(json, Serial);
    Serial.println("\n");
    Serial.println(line);
  }
  client.stop();
}


void sendJson(JsonObject& json) {
    WiFiClient client;
    Serial.print("\nconnecting to ");
    Serial.println(airMonitorServerName);

    if (!client.connect(airMonitorServerName, airMonitorPort)) {
        Serial.println("connection failed");
        Serial.println("wait ...\n");
        delay(1000);
        return;
    }

    delay(100); 

    client.println("POST /api HTTP/1.1");
    client.println("Content-Type: application/json");
    client.print("Content-Length: ");
  client.println(measureJson(json));
    client.println();
  serializeJson(json, client);

    String line = client.readStringUntil('\r');
    // TODO: Support wrong error (!= 200)

    if (DEBUG) {
        Serial.print("Length:");
    Serial.println(measureJson(json));
    serializeJsonPretty(json, Serial);
        Serial.println(line);
    }
    client.stop();
}

void sendDUSTData(int averagePM1, int averagePM25, int averagePM10) {
  if (strcmp(DUST_MODEL, "Non")) {
  StaticJsonDocument<400> jsonBuffer;
  JsonObject json = jsonBuffer.to<JsonObject>();
    json["lat"] = String(LATITUDE);
    json["long"] = String(LONGITUDE);
    json["pm1"] = averagePM1;
    json["pm25"] = averagePM25;
    json["pm10"] = averagePM10;
  if (!strcmp(DUST_MODEL, "PMS7003")) {
    json["sensor"] = "PMS7003";
  }
  if (!strcmp(DUST_MODEL, "HPMA115S0")) {
    json["sensor"] = "HPMA115S0";
  }
  if (!strcmp(DUST_MODEL, "SDS011/21")) {
    json["sensor"] = "SDS011";
  }
  if (!strcmp(DUST_MODEL, "SPS30")) {
    json["sensor"] = "SPS30";
  }
    sendJson(json);
  }
}

void sendTHPData(double currentTemperature, double currentPressure, double currentHumidity) {
  if (strcmp(THP_MODEL, "Non")) {
  StaticJsonDocument<400> jsonBuffer;
  JsonObject json = jsonBuffer.to<JsonObject>();
    json["lat"] = String(LATITUDE);
    json["long"] = String(LONGITUDE);
  if (!strcmp(THP_MODEL, "BME280")) {
      json["pressure"] = currentPressure;
      json["temperature"] = currentTemperature;
      json["humidity"] = currentHumidity;
    json["sensor"] = "BME280";
  } else if (!strcmp(THP_MODEL, "BMP280")) {
      json["pressure"] = currentPressure;
      json["temperature"] = currentTemperature;
    json["sensor"] = "BMP280";
  } else if (!strcmp(THP_MODEL, "HTU21")) {
      json["temperature"] = currentTemperature;
      json["humidity"] = currentHumidity;
    json["sensor"] = "HTU21";
  } else if (!strcmp(THP_MODEL, "DHT22")) {
      json["temperature"] = currentTemperature;
      json["humidity"] = currentHumidity;
    json["sensor"] = "DHT22";
  } else if (!strcmp(THP_MODEL, "SHT1x")) {
      json["temperature"] = currentTemperature;
      json["humidity"] = currentHumidity;
    json["sensor"] = "SHT1x";
  }
    sendJson(json);
  }
}

void sendDataToAirMonitor(float currentTemperature, float currentPressure, float currentHumidity, int averagePM1, int averagePM25, int averagePM4, int averagePM10) {
    if (!(AIRMONITOR_ON)) {
        return;
    }

    sendDUSTData(averagePM1, averagePM25, averagePM10);
    sendTHPData(currentTemperature, currentPressure, currentHumidity);
}

/*       *     *
         *     * 
         *     *U F T D A T E N
         *  
         ****                                                                          */
void sendDUSTLuftdatenJson(JsonObject& json) {
  WiFiClient client;
  Serial.print("\nconnecting to ");
  Serial.println(luftdatenAPIHOST);

  if (!client.connect(luftdatenAPIHOST, luftdatenAPIPort)) {
    Serial.println("connection failed");
    delay(1000);
    return;
  }
  delay(100);
  client.println("POST " + String(luftdatenAPIURL) + " HTTP/1.1");
  client.println("Host: " + String(luftdatenAPIHOST));
  client.println("Content-Type: application/json");
  client.println("X-PIN: 1");
  client.println("X-Sensor: smogomierz-" + luftdatenChipId);
  client.print("Content-Length: ");
  client.println(measureJson(json));
  client.println("Connection: close");
  client.println();
  serializeJson(json, client);

  String line = client.readStringUntil('\r');
  // TODO: Support wrong error (!= 200)

  if (DEBUG) {
    Serial.println();
    Serial.println("POST " + String(luftdatenAPIURL) + " HTTP/1.1");
    Serial.println("Host: " + String(luftdatenAPIHOST));
    Serial.println("Content-Type: application/json");
    Serial.println("X-PIN: 1");
    Serial.println("X-Sensor: smogomierz-" + luftdatenChipId);
    Serial.print("Content-Length: ");
    Serial.println(measureJson(json));
    Serial.println("Connection: close");
    Serial.println();
    serializeJsonPretty(json, Serial);
    Serial.println("\n");
    Serial.println(line);
  }
  client.stop();
}

void sendTHPLuftdatenJson(JsonObject& json) {
  WiFiClient client;
  Serial.print("\nconnecting to ");
  Serial.println(luftdatenAPIHOST);

  if (!client.connect(luftdatenAPIHOST, luftdatenAPIPort)) {
    Serial.println("connection failed");
    delay(1000);
    return;
  }
  delay(100);
  client.println("POST " + String(luftdatenAPIURL) + " HTTP/1.1");
  client.println("Host: " + String(luftdatenAPIHOST));
  client.println("Content-Type: application/json");
  if (!strcmp(THP_MODEL, "BME280")) {
    client.println("X-PIN: 11");
  } else if (!strcmp(THP_MODEL, "BMP280")) {
    client.println("X-PIN: 3");
  } else if (!strcmp(THP_MODEL, "HTU21")) {
    client.println("X-PIN: 7");
  } else if (!strcmp(THP_MODEL, "DHT22")) {
    client.println("X-PIN: 7");
  } else if (!strcmp(THP_MODEL, "SHT1x")) {
    client.println("X-PIN: 12");
  }
  client.println("X-Sensor: smogomierz-" + luftdatenChipId);
  client.print("Content-Length: ");
  client.println(measureJson(json));
  client.println("Connection: close");
  client.println();

  serializeJson(json, client);

  String line = client.readStringUntil('\r');
  // TODO: Support wrong error (!= 200)

  if (DEBUG) {
    Serial.println();
    Serial.println("POST " + String(luftdatenAPIURL) + " HTTP/1.1");
    Serial.println("Host: " + String(luftdatenAPIHOST));
    Serial.println("Content-Type: application/json");
    if (!strcmp(THP_MODEL, "BME280")) {
      Serial.println("X-PIN: 11");
    } else if (!strcmp(THP_MODEL, "BMP280")) {
      Serial.println("X-PIN: 3");
    } else if (!strcmp(THP_MODEL, "HTU21")) {
      Serial.println("X-PIN: 7");
    } else if (!strcmp(THP_MODEL, "DHT22")) {
      Serial.println("X-PIN: 7");
    } else if (!strcmp(THP_MODEL, "SHT1x")) {
      Serial.println("X-PIN: 12");
    }
    Serial.println("X-Sensor: smogomierz-" + luftdatenChipId);
    Serial.print("Content-Length: ");
    Serial.println(measureJson(json));
    Serial.println("Connection: close");
    Serial.println();
    serializeJsonPretty(json, Serial);
    Serial.println("\n");
    Serial.println(line);
  }
  client.stop();
}

void sendDUSTDatatoLuftdaten(int averagePM1, int averagePM25, int averagePM10) {
  if (strcmp(DUST_MODEL, "Non")) {
    StaticJsonDocument<600> jsonBuffer;
    JsonObject json = jsonBuffer.to<JsonObject>();
    json["software_version"] = "Smogomierz_" + String(SOFTWAREVERSION);
    JsonArray sensordatavalues = json.createNestedArray("sensordatavalues");
    if (!strcmp(DUST_MODEL, "PMS7003")) {
      JsonObject P0 = sensordatavalues.createNestedObject();
      P0["value_type"] = "P0";
      P0["value"] = averagePM1;
      JsonObject P1 = sensordatavalues.createNestedObject();
      P1["value_type"] = "P1";
      P1["value"] = averagePM10;
      JsonObject P2 = sensordatavalues.createNestedObject();
      P2["value_type"] = "P2";
      P2["value"] = averagePM25;
    } else {
      JsonObject P1 = sensordatavalues.createNestedObject();
      P1["value_type"] = "P1";
      P1["value"] = averagePM10;
      JsonObject P2 = sensordatavalues.createNestedObject();
      P2["value_type"] = "P2";
      P2["value"] = averagePM25;
    }
    sendDUSTLuftdatenJson(json);
  }
}

void sendTHPDatatoLuftdaten(double currentTemperature, double currentPressure, double currentHumidity) {
  if (strcmp(THP_MODEL, "Non")) {
    StaticJsonDocument<600> jsonBuffer;
    JsonObject json = jsonBuffer.to<JsonObject>();
    json["software_version"] = "Smogomierz_" + String(SOFTWAREVERSION);
    JsonArray sensordatavalues = json.createNestedArray("sensordatavalues");
    if (!strcmp(THP_MODEL, "BME280")) {
      JsonObject temperature = sensordatavalues.createNestedObject();
      temperature["value_type"] = "temperature";
      temperature["value"] = String(currentTemperature);
      JsonObject humidity = sensordatavalues.createNestedObject();
      humidity["value_type"] = "humidity";
      humidity["value"] = String(currentHumidity);
      JsonObject pressure = sensordatavalues.createNestedObject();
      pressure["value_type"] = "pressure";
      pressure["value"] = String(currentPressure * 100); //hPa -> Pa
    } else if (!strcmp(THP_MODEL, "BMP280")) {
      JsonObject temperature = sensordatavalues.createNestedObject();
      temperature["value_type"] = "temperature";
      temperature["value"] = String(currentTemperature);
      JsonObject pressure = sensordatavalues.createNestedObject();
      pressure["value_type"] = "pressure";
      pressure["value"] = String(currentPressure * 100); //hPa -> Pa
    } else if (!strcmp(THP_MODEL, "HTU21")) {
      JsonObject temperature = sensordatavalues.createNestedObject();
      temperature["value_type"] = "temperature";
      temperature["value"] = String(currentTemperature);
      JsonObject humidity = sensordatavalues.createNestedObject();
      humidity["value_type"] = "humidity";
      humidity["value"] = String(currentHumidity);
    } else if (!strcmp(THP_MODEL, "DHT22")) {
      JsonObject temperature = sensordatavalues.createNestedObject();
      temperature["value_type"] = "temperature";
      temperature["value"] = String(currentTemperature);
      JsonObject humidity = sensordatavalues.createNestedObject();
      humidity["value_type"] = "humidity";
      humidity["value"] = String(currentHumidity);
    } else if (!strcmp(THP_MODEL, "SHT1x")) {
      JsonObject temperature = sensordatavalues.createNestedObject();
      temperature["value_type"] = "temperature";
      temperature["value"] = String(currentTemperature);
      JsonObject humidity = sensordatavalues.createNestedObject();
      humidity["value_type"] = "humidity";
      humidity["value"] = String(currentHumidity);
    }
    sendTHPLuftdatenJson(json);
  }
}


void sendDUSTMadavideJson(JsonObject& json) {
  WiFiClient client;
  Serial.print("\nconnecting to ");
  Serial.println(madavideAPIHOST);

  if (!client.connect(madavideAPIHOST, madavideAPIPort)) {
    Serial.println("connection failed");
    delay(1000);
    return;
  }
  delay(100);
  client.println("POST " + String(madavideAPIURL) + " HTTP/1.1");
  client.println("Host: " + String(madavideAPIHOST));
  client.println("Content-Type: application/json");
  client.println("X-PIN: 1");
  client.println("X-Sensor: smogomierz-" + luftdatenChipId);
  client.print("Content-Length: ");
  client.println(measureJson(json));
  client.println("Connection: close");
  client.println();

  serializeJson(json, client);

  String line = client.readStringUntil('\r');
  // TODO: Support wrong error (!= 200)

  if (DEBUG) {
    Serial.println();
    Serial.println("POST " + String(madavideAPIURL) + " HTTP/1.1");
    Serial.println("Host: " + String(madavideAPIHOST));
    Serial.println("Content-Type: application/json");
    Serial.println("X-PIN: 1");
    Serial.println("X-Sensor: smogomierz-" + luftdatenChipId);
    Serial.print("Content-Length: ");
    Serial.println(measureJson(json));
    Serial.println("Connection: close");
    Serial.println();
    serializeJsonPretty(json, Serial);
    Serial.println("\n");
    Serial.println(line);
  }
  client.stop();
}

void sendTHPMadavideJson(JsonObject& json) {
  WiFiClient client;
  Serial.print("\nconnecting to ");
  Serial.println(madavideAPIHOST);

  if (!client.connect(madavideAPIHOST, madavideAPIPort)) {
    Serial.println("connection failed");
    delay(1000);
    return;
  }
  delay(100);
  client.println("POST " + String(madavideAPIURL) + " HTTP/1.1");
  client.println("Host: " + String(madavideAPIHOST));
  client.println("Content-Type: application/json");
  if (!strcmp(THP_MODEL, "BME280")) {
    client.println("X-PIN: 11");
  } else if (!strcmp(THP_MODEL, "BMP280")) {
    client.println("X-PIN: 3");
  } else if (!strcmp(THP_MODEL, "HTU21")) {
    client.println("X-PIN: 7");
  } else if (!strcmp(THP_MODEL, "DHT22")) {
    client.println("X-PIN: 7");
  } else if (!strcmp(THP_MODEL, "SHT1x")) {
    client.println("X-PIN: 12");
  }
  client.println("X-Sensor: smogomierz-" + luftdatenChipId);
  client.print("Content-Length: ");
  client.println(measureJson(json));
  client.println("Connection: close");
  client.println();

  serializeJson(json, client);

  String line = client.readStringUntil('\r');
  // TODO: Support wrong error (!= 200)

  if (DEBUG) {
    Serial.println();
    Serial.println("POST " + String(madavideAPIURL) + " HTTP/1.1");
    Serial.println("Host: " + String(madavideAPIHOST));
    Serial.println("Content-Type: application/json");
    if (!strcmp(THP_MODEL, "BME280")) {
      Serial.println("X-PIN: 11");
    } else if (!strcmp(THP_MODEL, "BMP280")) {
      Serial.println("X-PIN: 3");
    } else if (!strcmp(THP_MODEL, "HTU21")) {
      Serial.println("X-PIN: 7");
    } else if (!strcmp(THP_MODEL, "DHT22")) {
      Serial.println("X-PIN: 7");
    } else if (!strcmp(THP_MODEL, "SHT1x")) {
      Serial.println("X-PIN: 12");
    }
    Serial.println("X-Sensor: smogomierz-" + luftdatenChipId);
    Serial.print("Content-Length: ");
    Serial.println(measureJson(json));
    Serial.println("Connection: close");
    Serial.println();
    serializeJsonPretty(json, Serial);
    Serial.println("\n");
    Serial.println(line);
  }
  client.stop();
}

void sendDUSTDatatoMadavide(int averagePM1, int averagePM25, int averagePM10) {
  if (strcmp(DUST_MODEL, "Non")) {
    StaticJsonDocument<600> jsonBuffer;
    JsonObject json = jsonBuffer.to<JsonObject>();
    json["software_version"] = "Smogomierz_" + String(SOFTWAREVERSION);
    JsonArray sensordatavalues = json.createNestedArray("sensordatavalues");
    if (!strcmp(DUST_MODEL, "PMS7003")) {
      JsonObject P0 = sensordatavalues.createNestedObject();
      P0["value_type"] = "PMS_P0";
      P0["value"] = averagePM1;
      JsonObject P1 = sensordatavalues.createNestedObject();
      P1["value_type"] = "PMS_P1";
      P1["value"] = averagePM10;
      JsonObject P2 = sensordatavalues.createNestedObject();
      P2["value_type"] = "PMS_P2";
      P2["value"] = averagePM25;
    } else if (!strcmp(DUST_MODEL, "SDS011/21")) {
      JsonObject P1 = sensordatavalues.createNestedObject();
      P1["value_type"] = "SDS_P1";
      P1["value"] = averagePM10;
      JsonObject P2 = sensordatavalues.createNestedObject();
      P2["value_type"] = "SDS_P2";
      P2["value"] = averagePM25;
    } else if (!strcmp(DUST_MODEL, "HPMA115S0")) {
      JsonObject P1 = sensordatavalues.createNestedObject();
      P1["value_type"] = "HPM_P1";
      P1["value"] = averagePM10;
      JsonObject P2 = sensordatavalues.createNestedObject();
      P2["value_type"] = "HPM_P2";
      P2["value"] = averagePM25;
    }
    sendDUSTMadavideJson(json);
  }
}

void sendTHPDatatoMadavide(float currentTemperature, float currentPressure, float currentHumidity) {
  if (strcmp(THP_MODEL, "Non")) {
    StaticJsonDocument<600> jsonBuffer;
    JsonObject json = jsonBuffer.to<JsonObject>();
    json["software_version"] = "Smogomierz_" + String(SOFTWAREVERSION);
    JsonArray sensordatavalues = json.createNestedArray("sensordatavalues");
    if (!strcmp(THP_MODEL, "BME280")) {
      JsonObject temperature = sensordatavalues.createNestedObject();
      temperature["value_type"] = "temperature";
      temperature["value"] = String(currentTemperature);
      JsonObject humidity = sensordatavalues.createNestedObject();
      humidity["value_type"] = "humidity";
      humidity["value"] = String(currentHumidity);
      JsonObject pressure = sensordatavalues.createNestedObject();
      pressure["value_type"] = "pressure";
      pressure["value"] = String(currentPressure * 100); //hPa -> Pa
    } else if (!strcmp(THP_MODEL, "BMP280")) {
      JsonObject temperature = sensordatavalues.createNestedObject();
      temperature["value_type"] = "temperature";
      temperature["value"] = String(currentTemperature);
      JsonObject pressure = sensordatavalues.createNestedObject();
      pressure["value_type"] = "pressure";
      pressure["value"] = String(currentPressure * 100); //hPa -> Pa
    } else if (!strcmp(THP_MODEL, "HTU21")) {
      JsonObject temperature = sensordatavalues.createNestedObject();
      temperature["value_type"] = "temperature";
      temperature["value"] = String(currentTemperature);
      JsonObject humidity = sensordatavalues.createNestedObject();
      humidity["value_type"] = "humidity";
      humidity["value"] = String(currentHumidity);
    } else if (!strcmp(THP_MODEL, "DHT22")) {
      JsonObject temperature = sensordatavalues.createNestedObject();
      temperature["value_type"] = "temperature";
      temperature["value"] = String(currentTemperature);
      JsonObject humidity = sensordatavalues.createNestedObject();
      humidity["value_type"] = "humidity";
      humidity["value"] = String(currentHumidity);
    } else if (!strcmp(THP_MODEL, "SHT1x")) {
      JsonObject temperature = sensordatavalues.createNestedObject();
      temperature["value_type"] = "temperature";
      temperature["value"] = String(currentTemperature);
      JsonObject humidity = sensordatavalues.createNestedObject();
      humidity["value_type"] = "humidity";
      humidity["value"] = String(currentHumidity);
    }
    sendTHPMadavideJson(json);
  }
}



void sendDataToLuftdaten(float currentTemperature, float currentPressure, float currentHumidity, int averagePM1, int averagePM25, int averagePM4, int averagePM10) {
  if (!(LUFTDATEN_ON)) {
    return;
  }  
  sendDUSTDatatoLuftdaten(averagePM1, averagePM25, averagePM10);
  sendTHPDatatoLuftdaten(currentTemperature, currentPressure, currentHumidity);
  delay(10);
  sendDUSTDatatoMadavide(averagePM1, averagePM25, averagePM10);
  sendTHPDatatoMadavide(currentTemperature, currentPressure, currentHumidity);
}

