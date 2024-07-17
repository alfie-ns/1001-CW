@echo off
rem Remove cached files in the output_images directory
git rm -r --cached q3/q3/q3-images/output_images/*

rem Add all changes to staging
git add .

rem Commit the changes with a message
git commit -m "update"

rem Push the changes to the main branch
git push origin main