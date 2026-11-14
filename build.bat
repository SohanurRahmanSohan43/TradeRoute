@echo off
echo 1. Compiling SQLite Engine...
:: Added '-I include' so sqlite3.c can find sqlite3.h
gcc -c src/sqlite3.c -o sqlite3.o -I include

echo 2. Compiling TradeRoute...
:: Added '-I include' and simplified source selection to 'src/*.cpp'
:: This automatically finds ALL cpp files in src/ so you don't have to list them.
g++ src/*.cpp sqlite3.o -o TradeRoute.exe -I include

del sqlite3.o
echo [DONE] Build Successful.
TradeRoute.exe