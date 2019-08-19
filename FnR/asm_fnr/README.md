# Flush and Reload implementation for various cases with asm inline

# To reduce the noise of CPUs

Check have many CPUs we have
$ grep -c proc /proc/cpuinfo
8

Add in Debian/Ubuntu in the file /etc/default/grub to the option GRUB_CMDLINE_LINUX:

GRUB_CMDLINE_LINUX="isolcpus=7"
(it is 7, because it starts in 0, and you have 8 cores)

sudo update-grub

Reboot the system.

Then start your process.

sudo taskset -c <cpu_number> <application>

# To make sure that the CPU 7 is isolated
Install dstat

if you only interested in core 3 and 7 then you could do

dstat -C 3,7
