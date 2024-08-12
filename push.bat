@echo off
rem 1. Change to the q3/q3/q3-images directory
cd q3\q3\q3-images\output_images

rem 2. Remove all files in the current directory, -Q is to suppress the confirmation prompt
del /Q * 

rem 3. Change back to the root directory
cd ..\..\..

rem 4. Add all changes to staging
git add .

rem 5. Commit the changes with a message
git commit -m "update"

rem Final: Push the changes to the main branch
git push origin main

echo.
powershell -command "Write-Host 'Git push' -NoNewline -ForegroundColor Cyan; Write-Host ' completed successfully' -ForegroundColor Green"