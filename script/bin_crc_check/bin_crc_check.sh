#!/bin/bash 
echo "*****************************************************"
tc32-elf-objcopy -v -O binary $2.elf  $1.bin

../script/bin_crc_check/bin_crc_check_test.exe $1.bin 
if [ $? == 0 ]
then
echo "$1.bin crc calculate finish !"
fi
echo "*****************************************************"