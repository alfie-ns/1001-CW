@echo off
REM ---------------- Run the program ----------------

REM 1. Clear previous run's output
REM --------------------------------
if exist image_processor.exe del image_processor.exe
cd q3-images\output_images
for %%i in (*) do del "%%i"
cd ..\..

REM 2. Compile program
REM -------------------
g++ -std=c++17 q3b-windows.cpp -o image_processor.exe -O3

REM 3. Run the program
REM -------------------
image_processor.exe q3-images\input_images q3-images\output_images

pause
