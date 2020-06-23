# Stacja_pogodowa
Program stacji pogodowej oparty o projekt SUPLA dokonujący pomiarów :
  * temperatury, wilgotności, ciśnienia (BME280)
  * zanieczyszczenia powietrza (SDS0111)
  * modułem HECA(NETTIGO) z czujnikiem  temperatury i wilgotności - SHT31D w celu zmniejszenia błędu pomiarów przy dużej wilgotności,
  * pomiar predkości wiatru, 
  * pomiar opadów deszczu ,
  * pomiar nasłonecznienia.
Doatkowo układ rejestruje  najniższą i najwyższą temperaturę w ciągu doby, reset następuje wg ustalonego harmonogramu na supla.org
Układ elektroniczny zbudowany w oparciu schemat i PCB od NETTIGO na wemos d1 mini, zamknięty w obudowie Kradex Z59.

Aktualizacja 2020.06.23 - Wprowadzono możliwość ustawienia danych pod własny deszczomierz korytkowy
