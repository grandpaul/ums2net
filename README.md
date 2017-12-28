USB Mass Storage to Network Proxy (ums2net)
======================================================

ums2net provides a way for a user to connect from a network connection to a
USB mass storage device.


## Build

 1. cmake .
 2. make

## How to use ums2net

 1. Insert the USB Mass Storage. Check /dev/disk/by-id/ for the unique path
    for that device.
 2. Create a config file base on the above path. Please see the config file
    format section.
 3. Run "ums2net -c <ConfigFile>". ums2net will become a daemon in the
    background. For debugging please add "-d" option to avoid detach.
 4. Use nc to write your image to the USB Mass Storage device. For example,
    "nc localhost:29543 < warp7.img"

## Config file

Each line in the config file maps a TCP port to a device. All the options are
seperated by space. The first argument is a number represents the TCP port.
And the rest of the arguments are in dd-style. For example,

A line in the config file:
~~~
"29543 of=/dev/disk/by-id/usb-Linux_UMS_disk_0_WaRP7-0x2c98b953000003b5-0:0 bs=4096"
~~~
It means TCP port 29543 is mapped to /dev/disk/by-id/usb-Linux_UMS_disk_0_WaRP7-0x2c98b953000003b5-0:0 and the block size is 4096.

Currently we only support "of" and "bs".