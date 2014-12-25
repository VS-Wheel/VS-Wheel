@echo off
cls
set "CURRENTPATH=%cd%"
crt_emu_cm3_nxp -flash-load-exec "%CURRENTPATH%/VSWheel.bin" -g -2 -vendor=NXP -pLPC1769 -wire=winUSB -s250 -flash-driver=LPC175x_6x_512.cfx