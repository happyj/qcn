sudo cp -r ExarUSBCDCACM.kext /System/Library/Extensions
sudo chmod -R 755 /System/LibraryExtensions/ExarUSBCDCACM.kext
sudo chown -R root:wheel /System/LibraryExtensions/ExarUSBCDCACM.kext
sudo rm -R /System/Library/Extensions.kextcache
sudo rm -R /System/Library/Extensions.mkext
sudo reboot
