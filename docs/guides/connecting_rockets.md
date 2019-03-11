# Guide on how to connect Rocket M900's
***Read instructions below sequentially***
<br>
***Use [this video](https://www.youtube.com/watch?v=4zKNIveuCxM&feature=youtu.be) as reference***

<br>
<br>

###### Physical setup
- Plug in both rocket adapters into sockets
- Plug in ethernet cable from rocket into POE socket of adapter (for both rockets)
- Plug in ethernet cable from laptop into LAN socket of adapter (for both laptops)
- Screw in antenna (for both rockets)
- **Restart** both rockets (use pen to hold restart button for ten seconds) - *idk why but without a restart connections seems to stop working later*

###### On Access Point Computer
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
 
###### On Station Computer
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
- Open up port 9090 by running `sudo ufw allow 9090` - *command is for Ubuntu, use respective command for your OS, also not sure if this is really necessary, just do it in case :)*
