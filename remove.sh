#!/bin/bash
if ./pu.sh; then
cd ..
rm -rf 1001-cw
echo -e "\nremoved\n"
fi
