TARGETS = console-setup mountkernfs.sh alsa-utils ufw plymouth-log hostname.sh x11-common pppd-dns apparmor dns-clean udev keyboard-setup resolvconf mountdevsubfs.sh procps brltty hwclock.sh networking urandom checkroot.sh kmod mountall.sh checkfs.sh checkroot-bootclean.sh bootmisc.sh mountall-bootclean.sh mountnfs.sh mountnfs-bootclean.sh
INTERACTIVE = console-setup udev keyboard-setup checkroot.sh checkfs.sh
udev: mountkernfs.sh
keyboard-setup: mountkernfs.sh udev
resolvconf: dns-clean
mountdevsubfs.sh: mountkernfs.sh udev
procps: mountkernfs.sh udev
brltty: mountkernfs.sh udev
hwclock.sh: mountdevsubfs.sh
networking: resolvconf mountkernfs.sh urandom procps dns-clean
urandom: hwclock.sh
checkroot.sh: hwclock.sh keyboard-setup mountdevsubfs.sh hostname.sh
kmod: checkroot.sh
mountall.sh: checkfs.sh checkroot-bootclean.sh
checkfs.sh: checkroot.sh
checkroot-bootclean.sh: checkroot.sh
bootmisc.sh: udev checkroot-bootclean.sh mountall-bootclean.sh mountnfs-bootclean.sh
mountall-bootclean.sh: mountall.sh
mountnfs.sh: networking
mountnfs-bootclean.sh: mountnfs.sh
