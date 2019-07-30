# Guides on Telemetry Stuff
- [Running the telemetry module](#running-the-telemetry-module)
- [Building the protobufs library (required for running telemetry code)](#building-protobufs)
- [Connecting the Rockets](#connecting-the-rockets)
<br>

## Running the telemetry module
If you're running this on your laptop, make sure you've followed the [guide on building the protobufs library](#building-protobufs). Programs that use the telemetry module will not work unless protobufs is installed. If running on a beaglebone however, protobufs should already be installed.

*In the past telemetry files had to manually be added to `Source.files`, this is now automated!*

First, generate the protobuf files:
```
$ make protoc
```

Now, whenever you want to compile a program that uses the telemetry module, make sure to use the `PROTOBUF=1` flag. For the main program run:
```
$ make PROTOBUF=1
```
The linker will run into errors if this is not specified.

If you're running this locally on your laptop, make sure `config.txt` has the `IP` specified as `localhost` and `Port` as `9090`. If using the Rockets, `Port` will be the same but `IP` will now be the static ip address of the access point computer (if you've followed the guide below word for word it should be `192.168.1.40`).

Since the telemetry module will stop the program if it cannot connect to the base-station, make sure you've followed the instructions [here](https://github.com/Hyp-ed/base-station-2019/blob/master/README.md) on how to start the base-station. The base-station needs to be started before you run `hyped`.

Now you can run `./hyped` and it should work!

## Building Protobufs
***Refer to [this readme file](https://github.com/protocolbuffers/protobuf/blob/master/src/README.md) from the protobufs library***.

**Note for Windows users:**
Unfortunately I haven't tried building the library on Windows, and the process seems to be quite different (unless you are using cygwin or similar) so I suggest following the instructions [here](https://github.com/protocolbuffers/protobuf/blob/master/src/README.md#c-installation---windows). Just make sure you download **protoc-3.6.1-win32.zip** from [here](https://github.com/protocolbuffers/protobuf/releases/tag/v3.6.1) (pretty sure using a later release is fine since they're backwards-compatible, but use 3.6.1 just to avoid any potential conflicts).

On Unix machines, make sure the following are installed:
- autoconf
- automake
- libtool
- make
- g++
- unzip

Chances are they're already installed. If not, on Ubuntu/Debian run:
```
$ sudo apt-get install autoconf automake libtool curl make g++ unzip
```

On Mac's they will also likely be installed (xcode command line tools are also required, but this will almost certainly be already installed as it's a requirement for `g++` anyways).

Now, download the source files for protobufs from [here](https://github.com/protocolbuffers/protobuf/releases/tag/v3.6.1). Make sure you choose **protobuf-java-3.6.1.zip** (the java parts of the library are only necessary if you want to work on the telemetry base-station and you could install the pure c++ version if you wanted, but I'd recommend downloading it anyways). Place these files where you want.

**Warning: the following will likely take around an hour (sad reaccs), so only proceed when you have the time for it**

Navigate to the root of the source protobufs directory. Now build and install the library by running the following:
```
$ ./configure
$ make
$ make check
$ sudo make install
$ sudo ldconfig
```
On Mac's [you can ignore](https://github.com/protocolbuffers/protobuf/issues/2570#issuecomment-271358087) `sudo ldconfig`.

Check if protobufs was installed:
```
$ protoc --version
libprotoc 3.6.1
```

Now if you want to run code that uses the telemetry module refer to [this guide](#running-the-telemetry-module).

## Connecting the Rockets
***Follow instructions below only if you want to configure the rockets manually or for the first time, otherwise just flash the config files provided by SpaceX which can be found in our Drive [here](https://drive.google.com/drive/folders/1DEZkNo7ZnE4ZAlGTfbYi_xrVpMejJWNj?usp=sharing).***

<br>
<br>

*Read instructions below sequentially*
<br>
*Use [this video](https://www.youtube.com/watch?v=4zKNIveuCxM&feature=youtu.be) as reference*
<br>
*Just to clarify Access Point Computer refers to the computer with the designated Access Point Rocket plugged in; Station Computer refers to the computer with the designated Station Rocket plugged in*
<br>
**Station computer is NOT same computer as where the base-station will run! Base-station computer is in fact the access point computer**
<br>

### Physical setup
- Plug in both rocket adapters into sockets
- Plug in ethernet cable from rocket into POE socket of adapter (for both rockets)
- Plug in ethernet cable from laptop into LAN socket of adapter (for both laptops)
- Screw in antenna (for both rockets)
- **Restart** both rockets (use pen to hold restart button for ten seconds) - *idk why but without a restart connections seems to stop working later*

### On Access Point Computer
- Configure static IP address for ethernet interface
  - This will depend on your operating system, but typically under network settings somewhere (watch video for reference)
  - Set IP address to `192.168.1.40`
  - Set Subnet Mask to `255.255.255.0`
- Try `ping 192.168.1.20`
  - If request times out or other error, restart rocket and try again
- If ping above works, go to `192.168.1.20` in browser, where AirOS login screen should show up
- Login
  - *Username*: `ubnt`
  - *Password*: should be `ubnt` as well, but if not try `password1234`
- On AirOS page, click on `Wireless` tab
  - Set `Wireless Mode` to `Access Point`
  - Check `WDS (Transparent Bridge Mode)` box
  - Set `SSID` to `ubnt-bridge`
  - Set `Frequency` to `917` - *not sure why, seems to work better when this is set, feel free to mess around with this*
  - Set `Security` to `WPA2-AES`
  - Set `WPA Authentication` to `PSK`
  - Set `WPA Preshared Key` to `password1234` - *can be what you want, but we use password1234 just to keep things simple*
  - Click `Change` button in bottom right, and apply changes
- On AirOS page, click on `Network` tab
  - Set `IP Address` to `192.168.1.159`
  - Set `Subnet Mask` to `255.255.255.0`
  - Click `Change` button in bottom right, and apply changes
 
### On Station Computer
- Configure static IP address for ethernet interface
  - This will depend on your operating system, but typically under network settings somewhere (watch video for reference)
  - Set IP address to `192.168.1.50`
  - Set Subnet Mask to `255.255.255.0`
- Try `ping 192.168.1.20`
  - If request times out or other error, restart rocket and try again
- If ping above works, go to `192.168.1.20` in browser, where AirOS login screen should show up
- Login
  - *Username*: `ubnt`
  - *Password*: should be `ubnt` as well, but if not try `password1234`
- On AirOS page, click on `Wireless` tab
  - Set `Wireless Mode` to `Station`
  - Check `WDS (Transparent Bridge Mode)` box
  - Click `Select...` button next to `SSID` text box to start site survey
    - Select `ubnt-bridge` (this is our access point Rocket M900)
    - Click `Lock to AP`
  - Set `Security` to `WPA2-AES`
  - Set `WPA Authentication` to `PSK`
  - Set `WPA Preshared Key` to the **same password** we set for our access point Rocket (should be `password1234`)!!
  - Click `Change` button in bottom right, and apply changes
- On AirOS page, click on `Network` tab
  - Set `IP Address` to `192.168.1.160`
  - Set `Subnet Mask` to `255.255.255.0`
  - Click `Change` button in bottom right, and apply changes

### Beaglebone
- Now unplug the ethernet cable from the station computer and plug it into the beaglebone
- Assign the beaglebone ethernet network interface a static ip:
  - `sudo ifconfig eth0 192.168.1.40 netmask 255.255.255.0 up`
  - This is the same address and netmask that we used for the station computer
  - This command must be run everytime the beaglebone is rebooted
- Test connection to the access point computer
  - `ping 192.168.1.40`
