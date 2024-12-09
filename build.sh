#!/bin/bash

# Проверяем наличие MinGW
if ! command -v x86_64-w64-mingw32-gcc &> /dev/null; then
    echo "MinGW GCC (x86_64-w64-mingw32-gcc) не найден. Установите его, чтобы продолжить."
    exit 1
fi

# Создаем папку build, если она не существует
if [ ! -d "build" ]; then
    mkdir build
fi

# Компилируем программу
x86_64-w64-mingw32-gcc main.c definitions/azhypa_rpg.c -o build/azhypa_rpg.exe -lncurses

# Проверяем, успешно ли прошла компиляция
if [ $? -eq 0 ]; then
    echo "Сборка завершена успешно. Файл build/azhypa_rpg.exe готов."
else
    echo "Ошибка во время сборки. Проверьте код и зависимости."
    exit 1
fi

# Запуск программы
cd build
echo "Запускаем azhypa_rpg.exe..."
./azhypa_rpg.exe
