## HASSOS mount NAS network share

I use HASSOS in a Proxmox VM.
To make it possible to use network share inside and permanently the following needs to be done:

### Prerequisit

- a generic ubuntu server image.

### Step1
The HASSOS VM is shut down before starting.
Note the ID of the HA and Ubuntu VMs in proxmox. Forex.:
- HA vm id: 100
- Ubuntu vm id: 204

Open the proxmox main node terminal and edit the ubuntu config:

```
sudo nano /etc/pve/qemu-server/204.conf
```
And add the following line:
```
scsi1: local-lvm:vm-100-disk-1
```
Start Ubuntu Container.

### Step2


You should see your HA disk and it’s partitions listed.
```sh
lsblk

NAME    MAJ:MIN RM   SIZE RO TYPE MOUNTPOINTS
loop0     7:0    0  63.3M  1 loop /snap/core20/1879
loop1     7:1    0    73M  1 loop /snap/core22/617
loop2     7:2    0 166.4M  1 loop /snap/lxd/24846
loop3     7:3    0  53.2M  1 loop /snap/snapd/18933
loop4     7:4    0  73.1M  1 loop /snap/core22/634
loop5     7:5    0 164.3M  1 loop /snap/lxd/24758
loop6     7:6    0  63.3M  1 loop /snap/core20/1852
loop7     7:7    0  53.2M  1 loop /snap/snapd/19122
sda       8:0    0    32G  0 disk
├─sda1    8:1    0  31.9G  0 part /
├─sda14   8:14   0     4M  0 part
└─sda15   8:15   0   106M  0 part /boot/efi
sdb       8:16   0    32G  0 disk
├─sdb1    8:17   0    32M  0 part
├─sdb2    8:18   0    24M  0 part
├─sdb3    8:19   0   256M  0 part
├─sdb4    8:20   0    24M  0 part
├─sdb5    8:21   0   256M  0 part
├─sdb6    8:22   0     8M  0 part
├─sdb7    8:23   0    96M  0 part
└─sdb8    8:24   0  31.3G  0 part
sr0      11:0    1     4M  0 rom
sr1      11:1    1  1024M  0 rom

```
**├─sdb3    8:19   0   256M  0 part**

**├─sdb5    8:21   0   256M  0 part**

These are the HASSOS root filesystems parttitions, that should modify.

The partitions are so called squashfs not regular ext or ntfs/fat. Now we will need to extract files from these partition

### Step3
#### sdb3

```sh
cd /mnt
sudo unsquashfs -i /dev/sdb3
```
New file was created under mnt/squashfs-root. Now
```sh
sudo nano /mnt/squashfs-root/etc/fstab
```
Add neccesery lines for SMB
```sh
//<NASIP>/<NASSHARE>  /mnt/data/supervisor/media  cifs  nofail,username=<SMBUSER>,password=<SMBPASS>,iocharset=utf8,x-systemd.after=network-online.target  0  0
```
Add neccesery lines for NFS
```sh
<NASIP>:/<NASSHARE>  /mnt/data/supervisor/media  nfs  auto,nofail,noatime,nolock,tcp,actimeo=1800,x-systemd.after=network-online.target  0  0
```
convert folder to squashfs and copy it to device partition
```
sudo mksquashfs squashfs-root/ squash.rootfs.1
sudo dd if=./squash.rootfs.1 of=/dev/sdb3
```
After finish
```sh
sudo rm -r /mnt/squashfs-root
sudo rm /mnt/squash.rootfs.1
```
#### sdb5

```sh
cd /mnt
sudo unsquashfs -i /dev/sdb5
```
New file was created under mnt/squashfs-root. Now
```sh
sudo nano /mnt/squashfs-root/etc/fstab
```
Add neccesery lines for SMB
```sh
//<NASIP>/<NASSHARE>  /mnt/data/supervisor/media  cifs  nofail,username=<SMBUSER>,password=<SMBPASS>,iocharset=utf8,x-systemd.after=network-online.target  0  0
```
Add neccesery lines for NFS
```sh
<NASIP>:/<NASSHARE>  /mnt/data/supervisor/media  nfs  auto,nofail,noatime,nolock,tcp,actimeo=1800,x-systemd.after=network-online.target  0  0
```
convert folder to squashfs and copy it to device partition
```
sudo mksquashfs squashfs-root/ squash.rootfs.1
sudo dd if=./squash.rootfs.1 of=/dev/sdb5
```
After finish
```sh
sudo rm -r /mnt/squashfs-root
sudo rm /mnt/squash.rootfs.1
```
### Step4

Once complete, remove the temporary files and shutdown the Ubuntu VM. 
```sh
sudo shutdown -h now
```
Start HASSOS Container and check if everything works:
```
dmesg | grep cifs
```
```
dmesg | grep nfs
```
If YES, then edit the Proxmox config file in **Step1** above to comment out the HA disk. Boot the HASSOS Container.
