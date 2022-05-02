Commands used for bh1750 device driver
******************************************************************************************

1. $ lsmod: to  display the status of modules in the Linux kernel
2. $ sudo apt-get install raspberrypi-kernel-headers: used to create a build directory inside kernel header. 
3. $ i2cdetect –y 1:display slave address of sensor 
4. $ sudo make all
5. $ sudo depmod -a: used to generate a list of dependency description of kernel modules and its associated map files.
6. $ sudo insmod i2c_pro.ko chip.ko : insert module i2c_pro 
7. $ sudo modprobe industrialio : for iio devices
8. $ sudo modprobe chip: to insert loadable kernel module into linux kernel as an IIO device
9.  device file should be created under /sys/bus/iio/iio:deviceX/ and /dev/iio:deviceX so run
10. $ cd /sys/bus/iio
11. ls
12. cd devices /
13. ls
14. cd iio\: device0
15. ls
16. $cat /sys/bus/iio/devices/iio:device0/in_illuminance_raw to read the contents 
of the file in_illuminance_raw on the terminal to read the light intensity captured by sensor
17. cat can be used to read name power type of the sensor 
18. $rmmod i2c.ko chip.ko: remove the module 

