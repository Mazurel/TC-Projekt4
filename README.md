# TC-Projekt4

# Kompilacja

## Wymagania
- sfml
- cmake

## Linux
```bash
git clone https://github.com/Mazurel/TC-Projekt4
cd TC-Projekt4
mkdir build && cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
make && ./Projekt4
```

## Windows

> Wersja opisana dla visual stduio 2019

1. Pobierz cmake oraz sfml dla Visual C++ 15 (2017).
2. Sfml zapisz w folderze najlepiej od razu na dysku, tj D:\sfml-2.5.1\
3. Pobierz repozytorium https://github.com/Mazurel/TC-Projekt4 .
4. Otwórz cmake-gui, jakoi projekt wybierz folder z repozytorium, miejsce budowania gdziekolwiek.
5. Naciśnij konfiguruj, wybierz visual studio i kontynuuj.
6. Naciśnij generuj i zamknij program.
7. W folderze z wygenerowanym projektem, otwórz projekt sln w visual studio.
8. Usuń wszystkie podprojekty oprócz Projekt4.
9. Skonfiguruj Projekt4 zgodnie z https://www.sfml-dev.org/tutorials/2.5/start-vc.php#creating-and-configuring-a-sfml-project .
10. Skompiluj projekt4 (najlepiej w trybie release), a do folderu z plikiem exe przekopiuj biblioteki z folderu sfml/bin.
11. Włącz program.


