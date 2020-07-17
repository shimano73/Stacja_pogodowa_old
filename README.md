# Stacja_pogodowa
Program stacji pogodowej oparty o projekt SUPLA dokonujący pomiarów :
  * temperatury, wilgotności, ciśnienia (BME280)
  * zanieczyszczenia powietrza (SDS0111)
  * modułem HECA(NETTIGO) z czujnikiem  temperatury i wilgotności - SHT31D w celu zmniejszenia błędu pomiarów przy dużej wilgotności,
  * pomiar predkości wiatru, 
  * pomiar opadów deszczu ,
  * pomiar nasłonecznienia.
  * rejestracja  najniższej i najwyższej temperatury w ciągu doby, reset następuje wg ustalonego harmonogramu na supla.org
  * możliwość wysyłania pomiarów na serwer aqi.eco
  * możliwość wysyłania pomiarów na serwer air.monitor
  
Układ elektroniczny zbudowany w oparciu schemat i PCB od NETTIGO na wemos d1 mini, zamknięty w obudowie Kradex Z59.

Numer kanału i jego przeznaczenie. 

 0.  czujnik smogu SDS011;
 1.  ciśnienie atmosferyczne z czujnika BME280
 2.  temp. i wilgotność z czujnika BME280
 3.  temp min i max czujnika bme280 - wartości te można zresetować odpowiednio kanałem 8 i 9
 4.  temp i wilgotność z czujnika SHT31D z modułu Nettigo HECA
 5.  Nasłonecznienie z czujnika BH 1750;
 6.  prędkość wiatru w m/s
 7.  deszczomierz korytkowy 
 8.  reset temp minimalnej i wartości deszczomierza - kanał ten jest wyłączony  urządzeniach klienckich
 9.  rest temp. maksymalnej - kanał ten jest wyłączony  urządzeniach klienckich
 
Aktualizacja 2020.06.23 - Wprowadzono możliwość ustawienia danych pod własny deszczomierz korytkowy
