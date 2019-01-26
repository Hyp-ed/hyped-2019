## Using the BeagleBone Black on Linux:

**Note:** To connect to the BeagleBone from your computer the BeagleBone must have its operating system installed already. If this is not the case then please follow the guide for that first before continuing.

#### Summary of the guide:

User: _hyped_ 

Password: _spacex_

BeagleBone IP Address: _192.168.7.2_

To connect via ssh:

```bash
ssh hyped@192.168.7.2
```

To mount the BeagleBone:

```bash
sshfs hyped@192.168.7.2: /own_mount_point/
```

To safely unmount:

```bash
fusermount -uz /own_mount_point/
```



### Step 1: SSH

To begin with connect the BeagleBone to your computer using the USB cable. This should cause the BeagleBone to power on and for a bunch of lights to flash. It may take a couple of minutes for your computer to detect that it is connected. 

To communicate with the BeagleBone from your laptop we will use SSH. If SSH is not installed on your computer you can install it by running the following command (for ubuntu or similar distros) in your terminal: 

```bash
sudo apt-get install ssh
```

With SSH installed we can connect to the BeagleBone by opening terminal and running the _ssh_ command. _ssh_ has the following syntax:

```bash
ssh user@address
```

In our case the _user_ is _hyped_ and the _address_ is the IP address of the BeagleBone: _192.168.7.2_ 

So your command should look like this:

```bash
ssh hyped@192.168.7.2
```

You should then be prompted to enter a password, which for the user hyped is: _spacex_

If this is your first time connecting to the BeagleBone you should get a warning message which is normal, just continue and your computer should be connected.

If you get some other output saying that you can't connect to the BeagleBone then see the **Troubleshooting** heading below.

### Step 2: Mounting the file system

To edit and copy files across more easily you can mount the BeagleBone's file system on your computer.

To do this open a new terminal (on your computer, not connected to the BeagleBone) and decide where you want to put the BeagleBone. For example on my computer it is in: _/home/iain/hyped/BeagleBone/_

You will want to create a directory/folder to hold the mounted file system. Change directory to where you want it to be. For example on my computer:

```bash
cd /home/iain/hyped
```

Then create the directory:

```bash
mkdir BeagleBone
```

Once you have that done you can use the _sshfs_ command to mount the BeagleBone to this directory. If _sshfs_ is not installed on your computer you will need to install it using the same command used to install ssh (just replace _ssh_ with _sshfs_). The syntax for _sshfs_ is as follows: 

```bash
sshfs user@address: /own_mount_point/
```

So I would run:

```bash
sshfs hyped@192.168.7.2: /home/iain/hyped/BeagleBone/
```

After entering the password you should not get any other output. You can now edit and view the files in your own text editor like you would files on your own computer as well as drag and drop files onto the BeagleBone.

If you get other output saying that you can't connect to the BeagleBone then see the **Troubleshooting** heading below.

**Note:** The mount point must be either the absolute file path to the directory in which you want to mount the BeagleBone or the relative file path from your current directory.

**Note:** Remember that the files that you mounted are still only stored on the BeagleBone and not on your computer so as soon as you power off the BeagleBone the files that were mounted there will disappear.



### Step 3: Disconnecting the BeagleBone

To safely disconnect the BeagleBone don't unplug it right away. If you have mounted its file system then you can run the following command to safely disconnect it:

```bash
fusermount -uz /own_mount_point/
```

Then press the labelled power button above the usb cable to turn it off before unplugging.



#### Troubleshooting:

If you have connected to a different BeagleBone before and try connecting again your computer might not let you. 

The solution for this is to enter the following command in the terminal:

```bash
ssh-keygen -R 192.168.7.2
```

This will reset your computer's memory of previous devices at that IP address and allow the connection to the new device.

