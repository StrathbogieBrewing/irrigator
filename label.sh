#!/bin/bash

# Name      Printable px   Description
# 12         106           12mm endless
# 29         306           29mm endless
# 38         413           38mm endless
# 50         554           50mm endless
# 54         590           54mm endless
# 62         696           62mm endless
# 102       1164           102mm endless

convert -font courier -size 306x label:'Irrigator\nD0/1 - 4 hrs\nD2/3 - 2 hrs\nD4/5 - 1 hrs\nD6/7 - 30 m' label.png

# export BROTHER_QL_PRINTER=usb://0x04f9:0x2028/E8G784860
export BROTHER_QL_PRINTER=/dev/usb/lp0
export BROTHER_QL_MODEL=QL-570 
~/.local/bin/brother_ql -b linux_kernel print -r 90 -l 29 label.png