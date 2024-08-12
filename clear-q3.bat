@echo off

echo.
echo Clearing output_images files inside the q3/q3/q3-images/output_images directory
echo.

rem 1. Change to the q3/q3/q3-images/output_images directory
cd q3\q3\q3-images\output_images

rem 2. Remove all files in the current directory, -Q is to suppress the confirmation prompt
del /Q * 

rem 3. Change back to the q3 and rm q3.exe
cd ..\..\
del q3.exe