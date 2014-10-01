#!/bin/bash
echo "Parser Testing Script"
echo ""

echo "Expected success:"
./parser "1+2+3"
./parser "3-2-1"
./parser "1+2^3/4"
./parser "1+2*3^4*5/6"
./parser "4^3^2-1*5+5^6-7"

echo ""
echo "Expected failures:"
./parser "1+"
./parser "1+2*"
./parser "1^2^"
./parser "1-"