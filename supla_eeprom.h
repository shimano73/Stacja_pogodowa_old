

#ifndef SUPLA_EEPROM_H
#define SUPLA_EEPROM_H

void Pokaz_zawartosc_eeprom();
void czyszczenieEepromAll(void);
void czyszczenieEeprom(void);

void save_wifi_ssid(String save);
void save_wifi_pass(String save);
void save_login(String save);
void save_login_pass(String save);
void save_supla_server(String save);
void save_supla_id(String save);
void save_supla_pass(String save);
void save_supla_hostname(String save);
void save_guid(void);
void save_supla_button_type(int nr, String save);
void save_supla_relay_flag(int nr, String save);
void save_supla_relay_state(int nr, String save);
void save_DS18b20_address(String save, int nr);
void save_DS18b20_name(String save, int nr);
void save_bme_elevation(int temp);
void save_sds011_cwp(int temp);
void save_sds011_norm_pm10(int temp);
void save_sds011_norm_pm25(int temp);
void save_One_rpm_speed(String save);
void save_aqi_eco_state(String save);
void save_aqi_eco_host(String save);
void save_aqi_eco_path(String save);
void save_air_monitor_state(String save);
void save_air_monitor_server(String save);
void save_air_monitor_latitude(String save);
void save_air_monitor_longitude(String save);
void save_rain_surface_area(int temp);
void save_rain_gauge_cup_capacity(String save);



String read_wifi_ssid(void);
String read_wifi_pass(void);
String read_login(void);
String read_login_pass(void);
String read_supla_server(void);
String read_supla_id(void);
String read_supla_pass(void);
String read_supla_hostname(void);
String read_guid(void);
int read_supla_button_type(int nr);
int read_supla_relay_flag(int nr);
int read_supla_relay_state(int nr);
String read_DS18b20_address(int nr);
String read_DS18b20_name(int nr);
int read_bme_elevation();
int read_sds011_cwp();
int read_sds011_norm_pm25();
int read_sds011_norm_pm10();
String read_One_rpm_speed(void);
int    read_aqi_eco_state();
String read_aqi_eco_host(void);
String read_aqi_eco_path(void);
int    read_air_monitor_state();
String read_air_monitor_server(void);
String read_air_monitor_latitude(void);
String read_air_monitor_longitude(void);
int read_rain_surface_area(void);
  
String read_rain_gauge_cup_capacity(void);
       
extern int CustomWorkingPeriod;
extern int norma_pm10;    
extern int norma_pm25;
extern char select_monitor_state;
extern double One_rpm_speed;
extern int rain_surface_area;
extern double rain_gauge_cup_capacity;
#endif //SUPLA_EEPROM_H
