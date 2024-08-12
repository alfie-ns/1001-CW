@echo off
:: 'Q' flag: Quiet mode, suppresses confirmation prompt for faster deletion
:: Only displays results of commands, not the commands themselves

del /Q "../q3/q3/q3-images\output_images\*"
del /Q "image_processor"
del /Q "q3.exe"