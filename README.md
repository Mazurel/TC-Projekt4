# Projekt 4 z technik programowania

# Uwagi

Program korzysta z biblioteki sfml i języka programowania c++ w standarcie 11.

# Obsługa
Strzałki sterują chwytem dźwigu.
Spacja chwyta element o ile jest on kwadratem (zgodnie z zaleceniem). 
Lewym przyciskiem myszy umieszczany jest obiekt fizyczny.

# Kompilacja i instalacja

## Gnu/Linux

Pobierz i zainstaluj sfml, cmake, git i kompilator c++ (najlepiej gcc).
Np.:
- Ubuntu/Debian:
```bash
sudo apt install git cmake libsfml-dev
```
- Arch:
```bash
sudo pacman -S git cmake sfml
```

Instrukcja kompilacji:

```bash
git clone https://github.com/Mazurel/TC-Projekt4
cd TC-Projekt4
mkdir build && cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
make && ./Projekt4
```

## Windows

> Wersja opisana dla visual studio 2019

**Wersja dla visual studio z cmake**
1. Pobierz sfml dla Visual Studio C++ 15 (2017).
2. Zapisz wypakowany folder najlepiej jako C:\sfml-2.5.1\
3. Otwórz repozytorium https://github.com/Mazurel/TC-Projekt4 w visual studio.
4. (opcjonalnie) Zmodyfikuj ustawienie SFML_LOKALIZACJA w pliku CMakeLists.txt na inną lokzalizacje sfml.
5. Do folderu wyjściowego programu przekupiuj wszyskie biblioteki SFML z podfloderu bin (zwykle folder wyjściowy znajduje się w folderze out projektu).
5. Włącz program.

**Wersja dla cmake-gui**
1. Pobierz cmake oraz sfml dla Visual C++ 15 (2017).
2. Sfml zapisz w folderze najlepiej od razu na dysku, tj D:\sfml-2.5.1\
3. Pobierz repozytorium https://github.com/Mazurel/TC-Projekt4 .
4. Otwórz cmake-gui, jako projekt wybierz folder z repozytorium, miejsce budowania ustaw na gdziekolwiek.
5. Naciśnij konfiguruj, wybierz visual studio i kontynuuj.
6. Naciśnij generuj i zamknij program.
7. W folderze z wygenerowanym projektem, otwórz projekt sln w visual studio.
8. Usuń wszystkie podprojekty oprócz Projekt4.
9. Skonfiguruj Projekt4 zgodnie z https://www.sfml-dev.org/tutorials/2.5/start-vc.php#creating-and-configuring-a-sfml-project .
10. Skompiluj projekt4 (najlepiej w trybie release), a do folderu z plikiem exe przekopiuj biblioteki z folderu sfml/bin.
11. Włącz program.


