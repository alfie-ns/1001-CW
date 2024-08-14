#!/bin/bash
if ./clear-ds.sh; then :; else echo -e '\nfailed to clear .DS_Store files. Exiting...\n'; exit 1; fi
cd ../run
if ./clear.sh; then :; else echo -e "\nfailed to clear output. Exiting...\n"; exit 1; fi
# ---------------------------------------------------------------------------------------------------
cd .. && git add .
if git commit -m 'update'; then git pull origin main; else echo -e "\nfailed to commit. Exiting...\n"; exit 1; fi
echo -e "\ngit pushed to main\n"
