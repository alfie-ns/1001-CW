#!/bin/bash

echo -e '\nRunning q1...\n'
if bash q1.sh; then sleep 3; fi

echo -e '\nRunning q2...\n'
if bash q2.sh; then sleep 4; fi

echo -e '\nRunning q3a...\n'
if bash q3a.sh; then sleep 3; fi

echo -e '\nRunning q3b...\n'
if bash q3b.sh; then echo -e '\nall questions ran succesfully\n'; fi
