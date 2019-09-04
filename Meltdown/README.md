# Meltdown playground

1- First, disable KPTI and KASLR

2- Go to grub file located in /etc/default/

3- Open Grub file with sudo permission

4- Change
	GRUB_CMDLINE_LINUX_DEFAULT="quiet"
	line to
	GRUB_CMDLINE_LINUX_DEFAULT="quiet nokaslr nopti"
5- 'sudo update-grub'
6- Reboot
7- test by 'cat /proc/cmdline'

8- To disable ASLR
echo 0 | sudo tee /proc/sys/kernel/randomize_va_space


9- For better performance, run it on a seperate core


sudo taskset -c <cpu_number> <application>

Example usage:

sudo taskset -c 7 ./meltdown 0

