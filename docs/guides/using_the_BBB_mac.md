# Using the BBB for Mac

To complie your project onto the BeagleBone, you will need to learn how to connect the BBB to you Mac and mount the BBB file system onto your Mac as well. At the end of this guide, there will instructions for manipulating the bash script to avoid typing out command line functions repeatedly. 

#### Connecting to the BBB via SSH
- Username: `hyped`
- Password: `spacex`
1. Macs come preinstalled with SSH, but its default setting is disabled. To ensure it is enabled, run the following command in the command line:
```
$ sudo systemsetup -setremotelogin on
```
2. Connect the BBB via USB and allow ample time for your Mac to recognize the BBB. You can check this by opening *Finder* and waiting for *BEAGLEBONE_BLACK* to appear on the side.
3. In a new terminal window, type the following command:
```
$ ssh hyped@192.168.6.2
```
4. You will be prompted to type in the hyped password, which is `spacex`. If you are prompted with an error message, continue with the following step.
5. Type the following command to sshclean:
```
$ ssh-keygen -R 192.168.6.2
```
6. Repeat steps 3 and 4.
7. If there is still an error or the password you typed is not accepted, please give the BBB to member of the sensors team to reconfigure the BBB.

#### Mounting to the BBB
- You will be mounting the BBB file system onto your Mac, which will allow you to move the project directory and other various files onto the BBB.
- WARNING: when running and debugging the project on the BBB (when mounted), be aware which files you are editing as files edited on the BBB will be lost when turned off. Always save changes to the project saved on your Mac.
1. On the command line, navigate to the same directory with `hyped-2019`.
2. Create an empty directory called `mount`.
3. Mount the BBB onto your computer through that new directory.
4. Clone the up-to-date repo onto the BBB directly:
```
$ mkdir mount
$ sshfs hyped@192.168.6.2: <insert_path_here>/mount
$ cd mount
$ git clone <repo_URL>
```
- Note: 
    - For Mac: 192.168.6.2
    - For Windows/Linux: 192.168.7.2
- WARNING:
    - Make sure create a new branch for your feature changes on the repo and push your edits BEFORE disconneting the BBB or else your edits will be lost.


#### Futhermore
- Typing in these commands is a pain when you need to use the BBB repeatedly. This section will teach you how to manipulate bash script on your Mac to save these various commands of this guide to your Mac.
 1. Open a new terminal and navigate to your home directory:
```
$ cd
```
2. Type the command `ls -a` and a list of files will appear. There should be a file called `.bash_profile`. Open that file in terminal with nano:
```
$ ls -a
$ nano .bash_profile
```
3. Within this script, type the following lines below the preexisting text. Make sure to replace *<insert_path_here>* with the path to the mount directory you created earlier.
```
BBB=192.168.6.2

alias sshclean="ssh-keygen -R $BBB"
alias sshBBB="ssh hyped@$BBB"
alias sshfsBBB="sshfs hyped@$BBB: <insert_path_here>/mount"
```
4. To exit nano, press command+x, then hit enter twice when prompted to save the script
5. You need to make sure the script is sourced in terminal. To test this, close the terminal window and open a new terminal window to its home directory.
6. Type the following command to if it is sourced:
```
$ echo $BBB
```
7. The output should be `192.168.6.2`. If not, type the following command to source the `.bash_profile` script:
```
$ source ~/.bash_profile
```
8. Congratulations! You have successfully shortcut commands for the BBB. Next time you connect to the BBB, use `sshclean`, `sshBBB`, and `sshfsBBB` instead of their longer, equivalent commands you learned above.
 
