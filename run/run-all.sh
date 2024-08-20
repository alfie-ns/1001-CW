#!/bin/bash

bash q1.sh
bash q2.sh
bash q3a.sh
# ---------------------------------------------------
read -p "Do you want to proceed to q3b? This will clear the screen (y/n): " answer
if [[ $answer == "y" ]]; then bash q3b.sh; fi
# if answer's anything else, will skip q3b
: <<'END'

If all the listed scripts run successfully,
print respective success messages. -p flag is used
to prompt the user for input. 'answer' variable is
user's input'

END
