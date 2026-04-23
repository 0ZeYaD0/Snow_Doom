@echo off
g++ main.cpp glad.c -o main -I../include -L../lib -lglfw3dll -lopengl32 -lgdi32
if %errorlevel% neq 0 (
    echo Compilation failed!
    pause
    exit /b 1
)
echo Compilation successful!