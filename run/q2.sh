#!/bin/bash
cd q2 && { open q2.pdf 2>/dev/null || xdg-open q2.pdf 2>/dev/null || echo "Could not open q2.pdf"; }
# 2>/dev/null suppresses respective error messages