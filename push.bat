@echo off
rem Change to the q3/q3/q3-images directory
cd q3\q3\q3-images
if %errorlevel% neq 0 (
    echo Failed to change directory to q3\q3\q3-images
    exit /b %errorlevel%
)

rem Remove the output_images directory if it exists
if exist "output_images" (
    rmdir /s /q output_images
    if %errorlevel% neq 0 (
        echo Failed to remove output_images directory
        cd ..\..\..
        exit /b %errorlevel%
    )
    echo Removed output_images directory
)

rem Change back to the root directory
cd ..\..\..

rem Remove cached files in the output_images directory from git
git rm -r --cached q3/q3/q3-images/output_images

rem Add all changes to staging
git add .

rem Commit the changes with a message
git commit -m "update"

rem Push the changes to the main branch
git push origin main

echo.
powershell -command "Write-Host 'Git push' -NoNewline -ForegroundColor Cyan; Write-Host ' completed successfully' -ForegroundColor Green"