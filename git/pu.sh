#!/bin/bash
if ./clear-ds.sh; then :; else echo -e '\nfailed to clear .DS_Store files. Exiting...\n'; exit 1; fi
cd run
if ./clear.sh; then :; else echo -e "\nfailed to clear output. Exiting...\n"; exit 1; fi
# ---------------------------------------------------------------------------------------------------
git add .
if git commit -m 'update'; then git push origin main; else echo -e "\nfailed to commit, push anyway?\n"
# if user input yes then push, else exit with 1
echo -e "\ngit pushed to main\n"
fi