#!/bin/bash
cd ../run
if ./clear.sh; then sleep 1; else echo -e "\nfailed to clear output; exiting\n"; exit 1; fi
# ---------------------------------------------------------------------------------------------------
cd ..
git add .
git commit -m 'update'
git push origin main
echo -e "\ngit pushed to main\n"
