#!/bin/bash
git rm -r --cached q3/q3/q3-images/output_images/*
git add .
git commit -m 'update'
git push origin main
echo -e "\ngit pushed to main\n"
