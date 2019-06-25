# Controller Configuration Troubleshooting
Hyped 2019 - Iain Macpherson

I came across various issues when configuring the controllers that were not explained in the data sheets or user manuals of the motors so I decided to write up this guide to detail some of the problems I had and solutions that I found for them.

### Specifics: 
* Controller: Emdrive500
* Feedback type: Hall effect sensors
* Motor: PMS motor

The configuration of the controller is walked through in the user manual for the controller.
First install the controller configurator software for windows from the emsiso/emdrive website. Note that unfortunately this software is only available for windows. Configuration data containing the parameters that will be set to configure various aspects of the controller, can be saved as .dcf files. They can be loaded and saved to the controller using the emdrive configurator software.

***Note:*** when installing the configurator it is usually unnecessary to install the drivers that the user manual recommends. It should be enough to just install the configurator. 

Once the software is installed ensure that the controller is correctly wired to the feedback system as well as that the CAN high, low and ground are correctly connected. 

Connect the Emsiso usb to CAN device to your laptop and follow the proceedure shown in the user manual to connect your laptop to the controller. If the can error light on the controller is flashing but the configurator can connect to the usb to CAN device then check that everything is correctly wired to the controller (check that the correct CAN termination resistors have been used). If all of this seems to be fine then try changing the CAN bitrate in the connection tab of the configurator software (the controller is likely to use 250kb/s). 

Once the connection is established continue following the user manual to use the motor auto-align mode.

***Note:*** in order to use the motor in cyclic sync velocity it is necessary to set up auto-align, cyclic sync torque and finally cyclic sync velocity. It will not work correctly if these steps are skipped.

## Auto-align
The auto-align mode is when any issues in the configuration will show themselves. Some common errors are:
* 0xFF0B (Feedback type error) - Check the feedback config registers and ensure that they are all appropriately set. (for hall effect sensors the feedback config electrical angle filter, i.e. sub-index 7, should be set to 30). Also the feedback direction may need to be inverted.
* 0xFF0C (DC Link Under Voltage) - Ensure that the motor rated current and voltage are set correctly and that the controller is not drawing too much current. If this error persists it is either that the supply cannot supply the current that the motor needs and the voltage drops or there is a wiring issue (loose connection etc).

Once everything is correctly configured the motor should spin slowly, first in one direction and then in the other before coming to a stop with no errors or warnings. Once this happens you can move on to tuning the current control parameters.

## Current Control
This mode is initially well described in the user manual but it is lacking on describing how to set the P and I gains. There appears to be no auto-tune feature for them so it depends largely on what kind of motor will be used. This is a decent proceedure to follow:
* Set both P gain and I gain to 0 (or near enough).
* Follow the method that the user manual describes and check the built-in oscilloscope output.
    * A rough guide to what the P and I gain numbers do would be that P gain controls the speed at which the torque will try to reach the target. So too low and it will never reach the target but too high and it will overshoot the target. The I gain number controls the accuracy of the final steady state. So too low and it will stablise below the target but too high and it will stablise above the target.
* Once suitable P and I gain values are obtained then continue on to setting the Velocity control parameters.

## Velocity Control
***Note:*** for velocity control it is important to be aware that the velocity values are encoded and this can be configured in the velocity encoder factor register (0x6094). If this value is not correct the velocity will not be set or read correctly.

Configuring the velocity control also requires setting the velocity P and I gains but be careful when configuring this as if the values are incorrect it is possible or the motor to reach an rpm that is much higher than intented, or even for it to spin up until the current limiter kicks in and the controller throws a DC Link Under voltage error. This can happen even when the target velocity is zero. It is important to start the values very low and work your way up until the motor is stable when the target velocity is set to zero before spinning it up to any set rpm.

With all of these settings configured it should be possible to supply a target velocity in rpm and measure the same rpm physically with some tape on the motor and a tachometer.