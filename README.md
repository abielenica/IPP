# Wielomiany

Rozwiązanie części pierwszej dużego zadania z IPP, która polegała na zaimplementowaniu operacji na wielomianach rzadkich
wielu zmiennych o współczynnikach całkowitych. Opisy zaimplementowanych funkcji znajdują się w pliku `poly.h` w
formacie komentarzy.

### Definicja wielomianu

Zmienne wielomianu oznaczamy x<sub>0</sub>, x<sub>1</sub>, x<sub>2</sub> itd. Definicja wielomianu jest rekurencyjna.
Wielomian jest sumą jednomianów postaci px<sub>0</sub><sup>n</sup>, gdzie n jest wykładnikiem tego jednomianu będącym
nieujemną liczbą całkowitą, a p jest współczynnikiem, który jest wielomianem. Współczynnik w jednomianie zmiennej x<sub>
i</sub> jest sumą jednomianów zmiennej x<sub>i+1</sub>. Rekurencja kończy się, gdy współczynnik jest liczbą (czyli
wielomianem stałym), a nie sumą kolejnych jednomianów. Wykładniki jednomianów w każdej z rozważanych sum są parami
różne. Wielomiany są rzadkie, co oznacza, że stopień wielomianu może być znacznie większy niż liczba składowych
jednomianów.

### Zawartość projektu

* `src/poly.h` – deklaracja interfejsu biblioteki wraz z jej dokumentacją w formacie `doxygen`,
* `src/poly.c` - implementacja biblioteki wraz z dokumentacją w formacie `doxygen`,
* `src/memory.h` - biblioteka służąca do bezpiecznej alokacji pamięci,
* `src/poly_example.c` – przykłady użycia biblioteki,
* `CMakeLists.txt` – plik konfiguracyjny programu `cmake`,
* `Doxyfile.in` – plik konfiguracyjny programu `doxygen`,
* `MainPage.dox` – strona główna dokumentacji w formacie `doxygen`

### Kompilacja

Projekt korzysta z CMakeLists i kompiluje się w dwóch wersjach: release oraz debug. Wersję release kompiluje się za
pomocą sekwencji poleceń:

```asm
mkdir release
cd release
cmake ..
make
make doc
```

Wersję debug kompiluje się za pomocą sekwencji poleceń:

```asm
mkdir debug
cd debug
cmake -D CMAKE_BUILD_TYPE=Debug ..
make
make doc
```

W wyniku kompilacji odpowiednio w katalogu release lub debug powstaje plik wykonywalny poly oraz dokumentacja
wygenerowana przez `doxygen`.