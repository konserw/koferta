#kOferta

##Wstęp

kOferta jest to kompletny system wspomagający tworzenie ofert handlowych

##Co to jest oferta

Oferta jest to formalna propozycja dokonania transakcji. Zawiera listę i ilość
towarów ich ceny, dane klienta i sprzedającego. Dodatkowymi elementami mogą być
informacje o sposobie płatności, czy terminie dostawy.

##Cel i zakres projektu

Projekt kOferta ma ułatwić sprzedającemu tworzenie ofert handlowych poprzez dostęp do bazy danych z klientami i produktami.
System umożliwia również archiwizowanie stworzonych już wcześniej ofert.

Celem projektu jest stworzenie systemu składającego się z trzech zasadniczych części:

1. Serwer przechowujący dane klientów firmy, oferowanych towarów, zarchiwizowanych ofert i danych sprzedawców.

2. kDatabase - program, którego głównym zadaniem jest synchronizacja danych między serwerem a lokalną bazą danych.
Umożliwia ona też łatwą edycję danych w bazie – zarówno dodawanie nowych rekordów jak i edycję istniejących Dodatkowo
możliwe jest szybkie uzupełnienie bazy importując dane klientów i towarów z pliku .csv.

3. Aplikacja umożliwiająca szybkie tworzenie ofert w oparciu o lokalną bazę danych.

##Wykorzystanie

System ten wykorzystywany jest aktualnie przez przedstawicieli handlowych firmy Marley.
W czasie projektowania, systemu przeprowadzane były konsultacje z jej pracownikami.

Podczas testów i wdrażania systemu wykorzystano issue tracker platformy bitbucklet.com

##To do
* zastąpienie makra EXEC_SILENT i funkcji insert_towar etc. odpowiednimi funkcjami w klasie bazy danych
* Rodzielenie oferty od MainWindow poprzez nową klasę dla oferty
* Klasa Klient zamiast sqlrecord
* Wszędzie gdzie się da przeniesienie SQLowych rzeczy do klasy bazy danych
