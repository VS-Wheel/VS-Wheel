echo "Boot & Flash started"
CurrentDir="$(pwd)"
cd /usr/local/lpcxpresso_7.3.0_186/lpcxpresso/bin
lsusb | grep 0471:df55
if [ $? -eq 0 ]; then
   echo "Mounting device"
   ./dfu-util -d 0x471:0xdf55 -c 0 -t 2048 -R -D LPCXpressoWIN.enc
else
   echo "Device already mounted"
fi
./crt_emu_cm3_nxp -flash-load-exec "/home/alexander/VSWheel/VSWheel.bin" -g -2 -vendor=NXP -pLPC1769 -wire=winUSB -s250 -flash-driver=LPC175x_6x_512.cfx
echo "Boot & Flash ended"