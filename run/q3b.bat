@echo off
cd ../q3/q3
rem Create output_images if it doesn't already exist
if not exist "q3-images\output_images" (
    mkdir "q3-images\output_images"
    if %errorlevel% neq 0 (
        echo Failed to create output directory
        exit /b %errorlevel%
    )
    echo Created output directory: q3-images\output_images
)

g++ q3b.cpp -o q3 -O3 -lm
if %errorlevel% neq 0 (
    echo Compilation failed
    exit /b %errorlevel%
)

q3 .\q3-images\input_images\ .\q3-images\output_images
if %errorlevel% neq 0 (
    echo Program execution failed
    exit /b %errorlevel%
)

echo.
powershell -command "Write-Host '../q3/q3/q3b.cpp' -NoNewline -ForegroundColor Cyan; Write-Host ' compiled and executed successfully' -ForegroundColor Green"

cd ..\..
rem back to the root directory