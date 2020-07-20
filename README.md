# Stacja_pogodowa
Program stacji pogodowej oparty o projekt SUPLA dokonujący pomiarów :
  * temperatury, wilgotności, ciśnienia (BME280)
  * zanieczyszczenia powietrza (SDS0111)
  * modułem HECA(NETTIGO) z grzałką PTC , czujnikiem  temperatury i wilgotności - SHT31D w celu zmniejszenia błędu pomiarów przy dużej wilgotności,
  * pomiar predkości wiatru, 
  * pomiar opadów deszczu ,
  * pomiar nasłonecznienia.
  * rejestracja  najniższej i najwyższej temperatury w ciągu doby, reset następuje wg ustalonego harmonogramu na supla.org
  * możliwość wysyłania pomiarów na serwer aqi.eco
  * możliwość wysyłania pomiarów na serwer air.monitor
  
Układ elektroniczny zbudowany w oparciu schemat i PCB od NETTIGO na wemos d1 mini, zamknięty w obudowie Kradex Z59.
Schemat i projekt płytki PCB dostępny tu https://easyeda.com/nettigo/Nettigo-Air-Monitor-0.3

Wykorzystane GPIO Wemos D1 mini.

D1 - GPIO5 SDS011 TXD.
D2 - GPIO4 SDS011 RXD.
D3 - GPIO0 I2C SDA.
             BME280, BH1750, SHT31D.
D4 - GPIO2 I2C SCL.
D5 - GPIO14 - Czujnik deszczu.
D7 - GPIO13 - Czujnik wiatru.


Numer kanału i jego przeznaczenie. 

 0.  czujnik smogu SDS011;
 1.  ciśnienie atmosferyczne z czujnika BME280
 2.  temp. i wilgotność z czujnika BME280
 3.  temp min i max czujnika bme280 - wartości te można zresetować odpowiednio kanałem 8 i 9
 4.  temp i wilgotność z czujnika SHT31D z modułu Nettigo HECA
 5.  Nasłonecznienie z czujnika BH 1750;
 6.  prędkość wiatru w m/s
 7.  deszczomierz korytkowy 
 8.  reset temp minimalnej i wartości deszczomierza - kanał ten może być  urządzeniach klienckich
 9.  rest temp. maksymalnej - kanał ten może być wyłączony  urządzeniach klienckich
 
Aktualizacja 2020.06.23 - Wprowadzono możliwość ustawienia danych pod własny deszczomierz korytkowy
