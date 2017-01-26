# C Internet Chat

Betriebssysteme-Übung

Kompilieren:
- gcc client.c -o client.exe
- gcc server.c -o server.exe
- gcc ctest.c -lncurses -lpthread -o ctest.exe

Testen:

./ctest.exe 127.0.0.1 - Selbstgespräche

#TODO

(wenn man das benutzen wollen würde)

Generell:

- client.c und server.c in ctest.c einbinden -> Pipes reduzieren & insgesamt handlicher
- Makefile

UI:

- Zeilenumbrüche
- Scrolling

Brauchbarkeit:

- Relay & Identitätsserver

Cool:
- Verschlüsselung
