#!/bin/bash
cd ../run
bash clear-output.sh
sleep 1 # ensure deletion process complete
# ----------------------------------------
cd ..
git add .
git commit -m 'update'
git push origin main
echo -e "\ngit pushed to main\n"
