@echo off

rem ---------------- Run the program ----------------

rem 1. Clear previous run's output
rem --------------------------------

cd q3\q3
del /F image_processor.exe 2>nul
cd q3-images\output_images
del /F * 2>nul
cd ..\..\

rem 2. Compile program
rem -------------------
clang++ -std=c++17 q3b-mac.cpp -o image_processor.exe -O3 -lm 

rem 3. Run the program
rem -------------------
image_processor.exe q3-images\input_images q3-images\output_images