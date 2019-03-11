# Guide on how to connect Rocket M900's
***Read instructions below sequentially***
<br>
***Use [this video](https://www.youtube.com/watch?v=4zKNIveuCxM&feature=youtu.be) as reference***

<br>
<br>

- Plug in both rocket adapters into sockets
- Plug in ethernet cable from rocket into POE socket of adapter (for both rockets)
- Plug in ethernet cable from laptop into LAN socket of adapter (for both laptops)
- **Restart** both rockets (use pen to hold restart button for ten seconds) - *idk why but without a restart connections seems to stop working later*

###### On Access Point Computer
- Configure static IP address for ethernet interface
  - This will depend on your operating system, but typically under network settings somewhere (watch video for reference)
  - Set IP address to `192.168.1.50`
  - Set Subnet Masm to `255.255.255.0`
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
  - Set `Frequency` to `912` - *not sure why, seems to work better when this is set*
  - Set `Security` to `WPA2-AES`
  - Set `WPA Authentication` to `PSK`
  - Set `WPA Preshared Key` to `password1234` - *can be what you want, but we use password1234 just to keep things simple*
  - Click `Change` button in bottom right, and apply changes
- On AirOS page, click on `Network` tab
 
