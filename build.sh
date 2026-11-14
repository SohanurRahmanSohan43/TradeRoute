#!/bin/bash
echo "1. Compiling SQLite Engine..."
gcc -c src/sqlite3.c -o sqlite3.o -I include

echo "2. Compiling TradeRoute..."
g++ src/*.cpp sqlite3.o -o TradeRoute -I include

rm sqlite3.o
echo "[DONE] Build Successful."
./TradeRoute