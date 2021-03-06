# kOferta
devel: [![CircleCI](https://circleci.com/gh/konserw/koferta/tree/devel.svg?style=svg)](https://circleci.com/gh/konserw/koferta/tree/devel)[![Build status](https://ci.appveyor.com/api/projects/status/at9gkx5uqx1c28hw/branch/devel?svg=true)](https://ci.appveyor.com/project/konserw/koferta/branch/devel)[![Coverage Status](https://coveralls.io/repos/github/konserw/koferta/badge.svg?branch=devel)](https://coveralls.io/github/konserw/koferta?branch=devel)

master: [![CircleCI](https://circleci.com/gh/konserw/koferta/tree/master.svg?style=svg)](https://circleci.com/gh/konserw/koferta/tree/master)[![Build status](https://ci.appveyor.com/api/projects/status/at9gkx5uqx1c28hw/branch/master?svg=true)](https://ci.appveyor.com/project/konserw/koferta/branch/master)[![Coverage Status](https://coveralls.io/repos/github/konserw/koferta/badge.svg?branch=master)](https://coveralls.io/github/konserw/koferta?branch=master)

[![Join the chat at https://gitter.im/konserw/koferta](https://badges.gitter.im/konserw/koferta.svg)](https://gitter.im/konserw/koferta?utm_source=badge&utm_medium=badge&utm_campaign=pr-badge&utm_content=badge)
## Wstęp

kOferta jest to kompletny system wspomagający tworzenie ofert handlowych.

## Co to jest oferta

Oferta jest to formalna propozycja dokonania transakcji. Zawiera listę i ilość
towarów ich ceny, dane klienta i sprzedającego. Dodatkowymi elementami mogą być
informacje o sposobie płatności, czy terminie dostawy.

## Cel i zakres projektu

Projekt kOferta ma ułatwić sprzedającemu tworzenie ofert handlowych poprzez dostęp do bazy danych z klientami i produktami.
System umożliwia również archiwizowanie stworzonych już wcześniej ofert.

Celem projektu jest stworzenie systemu składającego się z 2 zasadniczych części:

1. Serwer przechowujący dane klientów firmy, oferowanych towarów, zarchiwizowanych ofert i danych sprzedawców.

2. Aplikacja umożliwiająca szybkie tworzenie ofert w oparciu o lokalną bazę danych.

## To do
* Wszędzie gdzie się da przeniesienie SQLowych rzeczy do klasy bazy danych
* Dalszy i głębszy rozdział elementów systemu na moduły 
* Testy!
