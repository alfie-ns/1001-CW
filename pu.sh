#!/bin/bash
bash clear-output.sh
sleep 1
# ------------------------------
git add .
git commit -m 'update'
git push origin main
echo -e "\ngit pushed to main\n"
