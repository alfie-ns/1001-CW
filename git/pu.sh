#!/bin/bash
if ./clear-ds.sh; then sleep 0.1; else echo -e '\nfailed to clear .DS_Store files\n'; fi
cd ../run
if ./clear.sh; then sleep 1; else echo -e "\nfailed to clear output. Exiting...\n"; exit 1; fi
# ---------------------------------------------------------------------------------------------------
cd ..
git add .
git commit -m 'update'
git push origin main
echo -e "\ngit pushed to main\n"
