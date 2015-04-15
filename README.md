VS Wheel 2014-2015
=======
This is the main branch of the VS Wheel project.

This project is here to help out other DIY sim racers, who would like to create their own force feedback wheel.

======
How to compile and run our code

1) Clone our repository locally using a git client or by downloading the zip.

2) Download and install GCC ARM Embedded using the .exe 'https://launchpad.net/gcc-arm-embedded' - And don't forget to check 'Add to PATH' at the end of the installation.

3) In the root of our repo you will have a folder named 'make-tool'. Simply copy all the files from the folder to the 'GNU Tools ARM Embedded\4.8 2014q3\bin'. Your version might differ from ours, but it's still the same procedure. (The files were taken from mingw32, however 'mingw32-make.exe' was renamed to 'make.exe' to simplify the compiling process.)

4) Download and install the latest LPCXpresso IDE 'http://www.lpcware.com/lpcxpresso/downloads/windows'. After installing, activate the free version of the program [Launch LPCXpresso -> Help -> Activate -> Create serial number and register (Free Edition)]. You might need to create an account on their website to do this.

5) Press Windows + R on your keyboard and type 'sysdm.cpl'. After hitting enter you will be presented to the system properties window. Navigate to the Advanced tab and go into the Environment Variables. Under the 'User variables for Username' you will have a PATH variable. Select it and press Edit. At the end of the line add a semicolon and paste your LPCXpresso bin path. It should look something like this: 'C:\NXP\LPCXpresso_7.3.0_186\lpcxpresso\bin'. In your situation the path might differ.

6) Create a shortcut to the file 'bootLPCXpresso.cmd' located in your LPCXpresso directory: 'C:\NXP\LPCXpresso_7.3.0_186\lpcxpresso\bin\Scripts'. Name it 'boot-script' and place it in our project folder where the main.cpp is located.

-> You should now have finished setting up our programming environment and should be able to compile/boot/flash by opening a command prompt in the 'VSWheel' folder and running the 'make' command to build, the 'boot' command to initialize the LPC and the 'flash' command to program. Enjoy!

======

How to compile and run on Ubuntu 14.04

1) Clone our repository locally using a git client or by downloading the zip.

2) Follow these instructions: 'https://launchpad.net/~terry.guo/+archive/ubuntu/gcc-arm-embedded', if you are running on 14.04 or later do not forget to follow the additional procedure specified on the site or else it will not work correctly.

3) Download LPCXpresso IDE version 7.3 from here: 'http://www.lpcware.com/lpcxpresso/downloads/linux', we did run into some problems with the 7.7.2 version. Use the newer ones at your own risk, they may or may not work.

4) Extract the downloaded archive and change the permissions of the file using 'sudo chmod 770 filename' in a terminal.

5) Run & install using: 'sudo ./filename'. After installing, activate the free version of the program [Launch LPCXpresso -> Help -> Activate -> Create serial number and register (Free Edition)]. You might need to create an account on their website to do this.

6) Copy the 'flash.sh' into the root of the project.

7) In the 'flash.sh' file, on line 3 replace '/usr/local/lpcxpresso_7.3.0_186/lpcxpresso/bin' with your LPCXpresso path, on line 11 replace the filepath to the one pointing to your .bin file in the project folder.

-> You should now have finished setting up our linux programming environment and should be able to compile/boot/flash by opening a terminal in the 'VSWheel' folder and running the 'make' command to build, the 'sh flash.sh' command to boot & flash the LPC.

If you want an alternative to 'joy.cpl' from Windows, you can install jstest by typing: 'sudo apt-get install jstest-gtk' into a terminal.

* If you have running on a 64-bit OS install these packages using this command line:
	
	"sudo apt-get install libgtk2.0-0:i386 libxtst6:i386 libpangox-1.0-0:i386 libpangoxft-1.0-0:i386 libidn11:i386 libglu1-mesa:i386 libncurses5:i386 libudev1:i386 libusb-1.0:i386 libusb-0.1:i386 gtk2-engines-murrine:i386 libnss3-1d:i386"

======

Commands on Windows:
- clean
- make
- boot
- flash

Commands on Linux:
- make
- make clean
- sh flash.sh

We use:
- Windows 8.1
- Sublime Text 2
- LPCXpresso IDE
- GCC ARM Embedded

- LPCXpresso LPC1769 (http://www.embeddedartists.com/products/lpcxpresso/lpc1769_xpr.php)