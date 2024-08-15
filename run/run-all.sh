#!/bin/bash


if bash q1.sh; then echo -e '\nRunning q1...\n'; fi
if bash q2.sh; then echo -e '\nRunning q2...\n'; fi
if bash q3a.sh; then echo -e '\nRunning q3a...\n'; fi
# ---------------------------------------------------
read -p "Do you want to proceed to q3b? This will clear the screen (y/n): " answer
if [[ $answer == "y" ]]; then bash q3b.sh; fi
# if answer is anything else, skip q3b
: <<'END'

If all the listed scripts run successfully,
print respective success messages. -p flag is used
to prompt the user for input. 'answer' variable is
user input.

END
