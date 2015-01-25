VS Wheel 2014-2015
=======
This is the main branch of the VS Wheel project.

This project is here to help out other DIY sim racers, who would like to create their own force feedback wheel.

======
How to compile and run our code

1) Clone our repository locally.

2) Download and install GCC ARM Embedded using the .exe 'https://launchpad.net/gcc-arm-embedded' - And don't forget to check 'Add to PATH' at the end of the installation.

3) In the root of our repo you will have a folder named 'make-tool'. Simply copy all the files from the folder to the 'GNU Tools ARM Embedded\4.8 2014q3\bin'. Your version might differ from ours, but it's still the same procedure.

4) Download and install the latest LPCXpresso IDE 'http://www.lpcware.com/lpcxpresso/downloads/windows'. After installing, activate the free version of the program. You might need to create an account on their website to do this.

5) Press Windows + R on your keyboard and type 'sysdm.cpl'. After hitting enter you will be presented to the system properties window. Navigate to the Advanced tab and go into the Environment Variables. Under the 'User variables for Username' you will have a PATH variable. Select it and press Edit. At the end of the line add a semicolon and paste your LPCXpresso bin path. It should look something like this: 'C:\NXP\LPCXpresso_7.3.0_186\lpcxpresso\bin'. In your situation the path might differ.

6) Create a shortcut to the file 'bootLPCXpresso.cmd' located in your LPCXpresso directory: 'C:\NXP\LPCXpresso_7.3.0_186\lpcxpresso\bin\Scripts'. Name it 'boot-script' and place it in our project folder where the main.cpp is located.

-> You should now have finished setting up our programming environment and should be able to compile/boot/flash by opening a command prompt in the 'VSWheel' folder and running the 'make' command to build, the 'boot' command to initialize the LPC and the 'flash' command to program. Enjoy!

======

We use:
- Windows 8.1
- Sublime Text 2
- LPCXpresso IDE
- GCC ARM Embedded

- LPCXpresso LPC1769 (http://www.embeddedartists.com/products/lpcxpresso/lpc1769_xpr.php)