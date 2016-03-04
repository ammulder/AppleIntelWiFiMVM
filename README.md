# AppleIntelWiFiMVM
The goal of this project is to create an OS X driver for those Intel WiFi cards based on the MVM firmware (basically, the current and previous generations).

## Supported Hardware
The following Intel WiFi cards/chips are supported:
* Intel&reg; Wireless 3160
* Intel&reg; Wireless 3165
* Intel&reg; Wireless 4165
* Intel&reg; Wireless 7260
* Intel&reg; Wireless 7265
* Intel&reg; Wireless 8260
* NUC on-board wireless for NUC 5i\*RY\*
* NUC on-board wireless for NUC 6i\*SY\*

## Driver Status
This driver is still in the "can it be done?" stage.

As of this writing the driver recognizes the appropriate hardware and loads a matching firmware file, but does not yet send the firmware to the hardware.

Now I'm in the process of trying to get all the Linux driver code to build on Mac OS X.  This will be a very long process.  Work is currently happening on the "Porting" branch, but only a small subset of the code even compiles right now.

Long story short, __this driver does not yet actually connect to wireless networks__.

If you're interested in helping, it would be great to have more people try test builds just to ensure it accurately identifies all the hardware I don't have on hand to try.

## Development Status

There are notes on [the project wiki](https://github.com/ammulder/AppleIntelWiFiMVM/wiki).  If you're interested in helping out at the code level, that would be super.

## Loading/Installation
Typical third-party driver kexts should be installed to /Library/Extensions, but that's not recommended for this driver yet.  Even in this initial work I've managed to craft a bug that crashed my machine when the driver loaded.  If the driver was installed system-wide, that would happen on every boot, which seems rather unfortunate.  So the manual loading process is recommended for now.

#### Manual Testing
1. Download the latest AppleIntelWiFiMVM.kext.zip (or build from source)
2. Unzip if needed
3. From Terminal, go to where the kext is and run:
```Shell
chown -R root:wheel AppleIntelWiFiMVM.kext
sudo kextload AppleIntelWiFiMVM.kext
sudo kextunload AppleIntelWiFiMVM.kext
```
4. Run Console.app and scroll to the bottom of the system log (usually displayed by default) or search it for AppleIntelWiFiMVM.  You should see output such as:
```Text
2/12/16 11:16:51.000 PM kernel[0]: AppleIntelWiFiMVM loading for device Intel(R) Dual Band Wireless AC 7265
2/12/16 11:16:51.000 PM kernel[0]: AppleIntelWiFiMVM LOADED firmware file iwlwifi-8000C-16.ucode
```

#### System-wide Installation
You've been warned!

But to install system-wide, use your favorite kext installer tool to install the kext to /Library/Extensions.  It should load automatically, and generate the same output as above that you can view in Console.app

## Credits

In building this driver, I'm relying on:
* The iwlwifi Linux driver
* The Intel firmware releases for this hardware (probably largely due to the Linux drivers)
* Mieze's IntelMausiEthernet OS X driver for wired Ethernet adapters
* RehabMan and the-darkvoid's BrcmPatchRAM OS X driver for USB Bluetooth adapters

## License

This driver (including any code I used in it from the projects above) is covered by the [GPL v2](http://www.gnu.org/licenses/old-licenses/gpl-2.0.en.html)

The Intel firmware is covered by the [firmware license](http://git.kernel.org/?p=linux/kernel/git/firmware/linux-firmware.git;a=blob_plain;f=LICENCE.iwlwifi_firmware;hb=HEAD)
