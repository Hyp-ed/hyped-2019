# Configuring HYPED OS

Our current OS enables certain networks and interfaces, such as CAN, SPI0, SPI1, etc. If curious about our configuration, look at this guide to learn more. Please get approval from the sensors team before reconfiguring your BBB and follow this guide when editing our OS.

#### HYPED Boot Script
- For edits, login using `sudo -i`

-- Our boot script is the reason the BBB functions the way we want it to and it runs every time the BBB powers on.
-- Our main boot script is manually flashed onto each BBB via the microSD port. Instructions for flashing later in this guide.
-- You can find our script here:
```
root@beaglebone:~# cat /etc/init.d/hyped
#!/bin/bash

## I2C
# open i2c bus for users
# ls /dev/i2c-0 -l | logger
# chmod a+rw /dev/i2c-0 | logger
# ls /dev/i2c-0 -l | logger
## SPI
# register SPI overlay
echo BB-SPI0-01 > /sys/devices/platform/bone_capemgr/slots | logger
echo BB-SPI1-01 > /sys/devices/platform/bone_capemgr/slots | logger
sleep 20
# open SPI bus for users
ls -l /dev/i2c-2 | logger
chmod a+rw /dev/spidev1.0 /dev/i2c-2
ls -l /dev/i2c-2 | logger

# ADC
echo BB-ADC > /sys/devices/platform/bone_capemgr/slots | logger
sleep 20

# Disable HDMI to enable GPIO pins P8.27-P8.46
echo cape-hdmi-disable > /sys/devices/platform/bone_capemgr/slots | logger
sleep 20

cp -r /home/hyped/.hyped.ssh /home/hyped/.ssh

ifconfig eth0 192.168.0.7

logger "finished HYPED init"

exit 0
```
-- As you can see here, we have several devices we use on the BBB enabled here, such as SPI and ADC.
- CAN0 is enabled in a different location `/boot/uEnv.txt'. Reference *The BBB Boot Script* for more information.

-- `BB_SPI0-01` , `BB-SPI1-01` and `BB-ADC` are *device tree overlays* and we use these to configure the hardware pins on these devices, i.e. `SPI1_SCLK`, `AIN0` , and even GPIO pins. `cape-hdmi-disable` and GPIO pins are discussed later in SECTION HERE!!!!
-- These overlays are downloaded onto the BBB by default, but we enable them through this script. Find these files here:
```
root@beaglebone:~# ls /lib/firmware
BB-ADC-00A0.dtbo
...
```
-- There will be a lot of overlays not used, but above is one example that we use. The *overlay_name* we use when enabling is the name of the .dtbo file **omitting** the `-00A0.dtbo`. So the *overlay_name* in the example above will be `BB-ADC`
-- To enable overlays and add to this `hyped` boot script, use this syntax:
```
echo <overlay_name> > /sys/devices/platform/bone_capemgr/slots | logger
```
- Note: The .dtbo file **must** to enable the device tree overlay. If it does not exist, reference section *GPIO Settings*.

#### The BBB Boot Script
- This is located at `/boot/uEnv.txt`. This script is different from the `hyped` OS boot script.

-- Usually, we try to put our HYPED OS configurations in `/etc/init.d/hyped` but some things need to be included here as well.
-- A few capes are enabled here. Here are important commands that have to do with our pin configuration, but there are more commands in the script itself. Make sure to comment additions:
```
##BeagleBone Black: HDMI (Audio/Video) disabled:
dtb=am335x-boneblack-emmc-overlay.dtb
#HDMI DISABLE for p8.27-p8.46
###Cape Universal Enable
enable_uboot_cape_universal=1
optargs=quiet capemgr.disable_partno=BB-BONELT-HDMI,BB-BONELT-HDMIN
cape_enable=capemgr.enable_partno=cape-hdmi-disable
# CAN numbering scheme is weird. This is CAN0 for our code
cape_enable=bone_capemgr.enable_partno=BB-CAN1
# note SPI is registered by script in /etc/init.d/HYPED
```
-- HDMI has to be disabled to use BBB pins P8.27-P8.46
-- This script follows a different format than the overlay slots in the `hyped` boot script


#### Downloading the BBB OS onto microSD
- This section outlines how to download the current BBB OS (including the `hyped` boot script and other script edits). See *Flashing the BBB* for the reverse process of this.

-- SSH to your BBB and `rm -rf` all local repositories, directories and files downloaded onto the BBB. The only file left in the home directory should be *README*
-- Plug in a microSD card into the port (bottom right underneath) and execute the following commands:
```
hyped@beaglebone:~# sudo -i
root@beaglebone:~# cd /opt/scripts/tools/eMMC/
root@beaglebone:~# ./beaglebone-black-make-microSD-flasher-from-eMMC.sh
```
-- Your BBB should initialise the download and the LEDs will change pattern. Leave your machine on and the BBB powered on until the BBB powers **off**
- **While the BBB is off**, unplug the microSD card. If you leave the microSD card in when you power the BBB again, it will flash the BBB with the OS on the microSD. This will waste your time since you cannot unplug the microSD whilst downloading/flashing or else you will need to re-flash the BBB.

-- Once the microSD is out, use the BBB as normal. Use the microSD to flash other BBBs

#### Flashing the BBB
- This section outlines how to flash the OS on a microSD onto the BBB. This process can be used to update the OS on a certain BBB or configure a new BBB with our HYPED OS.

-- Hold down the *RESET* button on the BBB. The button is located on the bottom left corner of the BBB on top of the microSD slot.
-- Insert the microSD with our up-to-date HYPED OS.
-- Plug in the BBB into your computer as normal (no need to SSH) while STILL holding down the reset button until the 4 LEDs change pattern to a continuous back-and-forth pattern.
-- Once the LEDs change pattern, leave the BBB powered on until the BBB powers **off**
- **While the BBB is off**, unplug the microSD card. If you leave the microSD card in when you power the BBB again, it will flash the BBB with the OS on the microSD. This will waste your time since you cannot unplug the microSD whilst downloading/flashing or else you will need to re-flash the BBB.

-- Once the microSD is out, use the BBB as normal. Use the microSD to flash other BBBs

#### GPIO Settings and Custom Device Tree Overlays
- Not all GPIO pins on the BBB are made equal. This section will outline some information regarding the low-level OS and device tree overlays for the GPIO pins related to sensors.

-- Go to the link [&lt;here&gt;](https://vadl.github.io/beagleboneblack/2016/07/29/setting-up-bbb-gpio)  for information regarding GPIO pin numbering and default modes. This section of the guide will outline the link, so you reference page for more information.
-- The pins on the BBB have a total of 7 different modes. The GPIO pins have 7 different modes as well, with mode 7 (0x07).
-- The gpio pins of the BBB are grouped into 3 groups of 32: `GPIO0`, `GPIO1`, and `GPIO2`. An individual pin can be refered to using the convention *GPIOX_Y* where X is its gpio register and Y is its number within that register. However, all references to a particular pin made in software instead uses its absolute pin number! A gpio’s absolute pin number is calculated in the following manner:
- `Z = 32*X + Y` where X is again the gpio register and Y is the position within that register.
- i.e. `GPIO2_24` is `32*2+24`, making it `GPIO_88`. If this pin were to be referenced anywhere in software, the user would use the number 88, not 24!
- The pin address for `GPIO2_24` would be `0x0E8`. Reference the pins using the offset value from the table, rather than the raw register.

-- The pins utilise something called *pinmuxing* to switch between modes. Reference this [&lt;link&gt;](http://www.ofitselfso.com/BeagleNotes/BeagleboneBlackPinMuxModes.php) for more information about *pinmuxing*
-- Certain pins are not default mode 7, so a device tree overlay must be disabled and/or a device tree overlay must be written to enable these pins for this setting. You can see which pins have a different default mode other than mode 7:
```
root@beaglebone:~# cat /sys/kernel/debug/pinctrl/44e10800.pinmux/pingroups
```
- Make sure you follow the pin numbering convention between pin number, gpio hardware number, and gpio software number.

-- Device trees are tricky stuff. Below is the code for the overlay `cape-hdmi-disable.dts`. Other BBB default overlays are located [&lt;here&gt;](https://github.com/VADL/bb.org-overlays/blob/master/src/arm/cape-univ-hdmi-00A0.dts) 
```
/dts-v1/;
/plugin/;

/ {
compatible = "ti,beaglebone", "ti,beaglebone-black";

/* identification */

part-number = "cape-hdmi-disable";
version = "00A0";

/* state the resources this cape uses */
exclusive-use =
           /* the pin header uses */
           "P8.27",
           "P8.28",
           "P8.29",
           "P8.30",
           "P8.31",
           "P8.32",
           "P8.33",
           "P8.34",
           "P8.35",
           "P8.36",
           "P8.37",
           "P8.38",
           "P8.39",
           "P8.40",
           "P8.41",
           "P8.42",
           "P8.43",
           "P8.44",
           "P8.45",
           "P8.46";
fragment@0 {
	target = <&am33xx_pinmux>;
	__overlay__ {
		bb_gpio_pins: pinmux_pins {
			pinctrl-single,pins = <
				0x0E0 0x07 /* P8_27 OUTPUT | MODE7 */
				0x0E8 0x07 /* P8_28 OUTPUT | MODE7 */
				0x0E4 0x07 /* P8_29 OUTPUT | MODE7 */
				0x0EC 0x07 /* P8_30 OUTPUT | MODE7 */	
				0x0D8 0x07 /* P8_31 OUTPUT | MODE7 */
				0x0DC 0x07 /* P8_32 OUTPUT | MODE7 */
				0x0D4 0x07 /* P8_33 OUTPUT | MODE7 */
				0x0CC 0x07 /* P8_34 OUTPUT | MODE7 */
				0x0D0 0x07 /* P8_35 OUTPUT | MODE7 */
				0x0C8 0x07 /* P8_36 OUTPUT | MODE7 */
				0x0C0 0x07 /* P8_37 OUTPUT | MODE7 */

		                0x0C4 0x07 /* P8_38 OUTPUT | MODE7 */
		                0x0B8 0x07 /* P8_39 OUTPUT | MODE7 */
		                0x0BC 0x07 /* P8_40 OUTPUT | MODE7 */
				0x0B0 0x07 /* P8_41 OUTPUT | MODE7 */
				0x0B4 0x07 /* P8_42 OUTPUT | MODE7 */
				0x0A8 0x07 /* P8_43 OUTPUT | MODE7 */
				0x0AC 0x07 /* P8_44 OUTPUT | MODE7 */
				0x0A0 0x07 /* P8_45 OUTPUT | MODE7 */
				0x0A4 0x07 /* P8_46 OUTPUT | MODE7 */
			>;
		};
	};
};

fragment@1 {
	target = <&ocp>;
	__overlay__ {
		test_helper: helper {
			compatible = "bone-pinmux-helper";
			pinctrl-names = "default";
			pinctrl-0 = <&bb_gpio_pins>;
			status = "okay";
		};
	};
};
};
```
-- `part-number` should be the name of the .dts file. This file will later be complied to a BBB-usable .dtbo file.
-- As you can see, the pins in list `bb_gpio_pins` are referenced by its offset memory address and set to mode `0x07` in `fragment@0`. These numbers were found off the first link above.
-- Make sure you make this file in `root`
-- Compile the file using the following command:
```
root@beaglebone:~# dtc -O dtb -o /lib/firmware/sample_overlay-00A0.dtbo -b 0 -@ sample_overlay.dts
```
-- Make sure the compile file name iks `-00A0.dtbo`, ignore the warnings
-- After compilation, copy the file into `/lib/firmware`
-- You can then delete the .dtbo and .dts files in the home directory.
-- Your device tree overlay is ready for use by adding it to the `hyped` boot script in `/etc/init.d/`. Reference the first section to enable this. 