# SPDX-License-Identifier: GPL-2.0
# Copyright (c) 2016, NVIDIA CORPORATION. All rights reserved.

import getpass
import gzip
import os
import os.path
import pytest

import u_boot_utils
from tests import fs_helper

def mkdir_cond(dirname):
    """Create a directory if it doesn't already exist

    Args:
        dirname (str): Name of directory to create
    """
    if not os.path.exists(dirname):
        os.mkdir(dirname)

def setup_image(cons, mmc_dev, part_type, second_part=False):
    """Create a 20MB disk image with a single partition

    Args:
        cons (ConsoleBase): Console to use
        mmc_dev (int): MMC device number to use, e.g. 1
        part_type (int): Partition type, e.g. 0xc for FAT32
        second_part (bool): True to contain a small second partition

    Returns:
        tuple:
            str: Filename of MMC image
            str: Directory name of 'mnt' directory
    """
    fname = os.path.join(cons.config.source_dir, f'mmc{mmc_dev}.img')
    mnt = os.path.join(cons.config.persistent_data_dir, 'mnt')
    mkdir_cond(mnt)

    spec = f'type={part_type:x}, size=18M, bootable'
    if second_part:
        spec += '\ntype=c'

    u_boot_utils.run_and_log(cons, 'qemu-img create %s 20M' % fname)
    u_boot_utils.run_and_log(cons, 'sudo sfdisk %s' % fname,
                             stdin=spec.encode('utf-8'))
    return fname, mnt

def mount_image(cons, fname, mnt, fstype):
    """Create a filesystem and mount it on partition 1

    Args:
        cons (ConsoleBase): Console to use
        fname (str): Filename of MMC image
        mnt (str): Directory name of 'mnt' directory
        fstype (str): Filesystem type ('vfat' or 'ext4')

    Returns:
        str: Name of loop device used
    """
    out = u_boot_utils.run_and_log(cons, 'sudo losetup --show -f -P %s' % fname)
    loop = out.strip()
    part = f'{loop}p1'
    u_boot_utils.run_and_log(cons, f'sudo mkfs.{fstype} {part}')
    opts = ''
    if fstype == 'vfat':
         opts += f' -o uid={os.getuid()},gid={os.getgid()}'
    u_boot_utils.run_and_log(cons, f'sudo mount -o loop {part} {mnt}{opts}')
    u_boot_utils.run_and_log(cons, f'sudo chown {getpass.getuser()} {mnt}')
    return loop

def copy_prepared_image(cons, mmc_dev, fname):
    """Use a prepared image since we cannot create one

    Args:
        cons (ConsoleBase): Console touse
        mmc_dev (int): MMC device number
        fname (str): Filename of MMC image
    """
    infname = os.path.join(cons.config.source_dir,
                           f'test/py/tests/bootstd/mmc{mmc_dev}.img.xz')
    u_boot_utils.run_and_log(
        cons,
        ['sh', '-c', 'xz -dc %s >%s' % (infname, fname)])

def setup_bootmenu_image(cons):
    """Create a 20MB disk image with a single ext4 partition

    This is modelled on Armbian 22.08 Jammy
    """
    mmc_dev = 4
    fname, mnt = setup_image(cons, mmc_dev, 0x83)

    loop = None
    mounted = False
    complete = False
    try:
        loop = mount_image(cons, fname, mnt, 'ext4')
        mounted = True

        vmlinux = 'Image'
        initrd = 'uInitrd'
        dtbdir = 'dtb'
        script = '''# DO NOT EDIT THIS FILE
#
# Please edit /boot/armbianEnv.txt to set supported parameters
#

setenv load_addr "0x9000000"
setenv overlay_error "false"
# default values
setenv rootdev "/dev/mmcblk%dp1"
setenv verbosity "1"
setenv console "both"
setenv bootlogo "false"
setenv rootfstype "ext4"
setenv docker_optimizations "on"
setenv earlycon "off"

echo "Boot script loaded from ${devtype} ${devnum}"

if test -e ${devtype} ${devnum} ${prefix}armbianEnv.txt; then
	load ${devtype} ${devnum} ${load_addr} ${prefix}armbianEnv.txt
	env import -t ${load_addr} ${filesize}
fi

if test "${logo}" = "disabled"; then setenv logo "logo.nologo"; fi

if test "${console}" = "display" || test "${console}" = "both"; then setenv consoleargs "console=tty1"; fi
if test "${console}" = "serial" || test "${console}" = "both"; then setenv consoleargs "console=ttyS2,1500000 ${consoleargs}"; fi
if test "${earlycon}" = "on"; then setenv consoleargs "earlycon ${consoleargs}"; fi
if test "${bootlogo}" = "true"; then setenv consoleargs "bootsplash.bootfile=bootsplash.armbian ${consoleargs}"; fi

# get PARTUUID of first partition on SD/eMMC the boot script was loaded from
if test "${devtype}" = "mmc"; then part uuid mmc ${devnum}:1 partuuid; fi

setenv bootargs "root=${rootdev} rootwait rootfstype=${rootfstype} ${consoleargs} consoleblank=0 loglevel=${verbosity} ubootpart=${partuuid} usb-storage.quirks=${usbstoragequirks} ${extraargs} ${extraboardargs}"

if test "${docker_optimizations}" = "on"; then setenv bootargs "${bootargs} cgroup_enable=cpuset cgroup_memory=1 cgroup_enable=memory swapaccount=1"; fi

load ${devtype} ${devnum} ${ramdisk_addr_r} ${prefix}uInitrd
load ${devtype} ${devnum} ${kernel_addr_r} ${prefix}Image

load ${devtype} ${devnum} ${fdt_addr_r} ${prefix}dtb/${fdtfile}
fdt addr ${fdt_addr_r}
fdt resize 65536
for overlay_file in ${overlays}; do
	if load ${devtype} ${devnum} ${load_addr} ${prefix}dtb/rockchip/overlay/${overlay_prefix}-${overlay_file}.dtbo; then
		echo "Applying kernel provided DT overlay ${overlay_prefix}-${overlay_file}.dtbo"
		fdt apply ${load_addr} || setenv overlay_error "true"
	fi
done
for overlay_file in ${user_overlays}; do
	if load ${devtype} ${devnum} ${load_addr} ${prefix}overlay-user/${overlay_file}.dtbo; then
		echo "Applying user provided DT overlay ${overlay_file}.dtbo"
		fdt apply ${load_addr} || setenv overlay_error "true"
	fi
done
if test "${overlay_error}" = "true"; then
	echo "Error applying DT overlays, restoring original DT"
	load ${devtype} ${devnum} ${fdt_addr_r} ${prefix}dtb/${fdtfile}
else
	if load ${devtype} ${devnum} ${load_addr} ${prefix}dtb/rockchip/overlay/${overlay_prefix}-fixup.scr; then
		echo "Applying kernel provided DT fixup script (${overlay_prefix}-fixup.scr)"
		source ${load_addr}
	fi
	if test -e ${devtype} ${devnum} ${prefix}fixup.scr; then
		load ${devtype} ${devnum} ${load_addr} ${prefix}fixup.scr
		echo "Applying user provided fixup script (fixup.scr)"
		source ${load_addr}
	fi
fi
booti ${kernel_addr_r} ${ramdisk_addr_r} ${fdt_addr_r}

# Recompile with:
# mkimage -C none -A arm -T script -d /boot/boot.cmd /boot/boot.scr
''' % (mmc_dev)
        bootdir = os.path.join(mnt, 'boot')
        mkdir_cond(bootdir)
        cmd_fname = os.path.join(bootdir, 'boot.cmd')
        scr_fname = os.path.join(bootdir, 'boot.scr')
        with open(cmd_fname, 'w') as outf:
            print(script, file=outf)

        infname = os.path.join(cons.config.source_dir,
                               'test/py/tests/bootstd/armbian.bmp.xz')
        bmp_file = os.path.join(bootdir, 'boot.bmp')
        u_boot_utils.run_and_log(
            cons,
            ['sh', '-c', f'xz -dc {infname} >{bmp_file}'])

        u_boot_utils.run_and_log(
            cons, f'mkimage -C none -A arm -T script -d {cmd_fname} {scr_fname}')

        kernel = 'vmlinuz-5.15.63-rockchip64'
        target = os.path.join(bootdir, kernel)
        with open(target, 'wb') as outf:
            print('kernel', outf)

        symlink = os.path.join(bootdir, 'Image')
        if os.path.exists(symlink):
            os.remove(symlink)
        u_boot_utils.run_and_log(
            cons, f'echo here {kernel} {symlink}')
        os.symlink(kernel, symlink)

        u_boot_utils.run_and_log(
            cons, f'mkimage -C none -A arm -T script -d {cmd_fname} {scr_fname}')
        complete = True

    except ValueError as exc:
        print('Falled to create image, failing back to prepared copy: %s',
              str(exc))
    finally:
        if mounted:
            u_boot_utils.run_and_log(cons, 'sudo umount --lazy %s' % mnt)
        if loop:
            u_boot_utils.run_and_log(cons, 'sudo losetup -d %s' % loop)

    if not complete:
        copy_prepared_image(cons, mmc_dev, fname)

def setup_bootflow_image(cons):
    """Create a 20MB disk image with a single FAT partition"""
    mmc_dev = 1
    fname, mnt = setup_image(cons, mmc_dev, 0xc, second_part=True)

    loop = None
    mounted = False
    complete = False
    try:
        loop = mount_image(cons, fname, mnt, 'vfat')
        mounted = True

        vmlinux = 'vmlinuz-5.3.7-301.fc31.armv7hl'
        initrd = 'initramfs-5.3.7-301.fc31.armv7hl.img'
        dtbdir = 'dtb-5.3.7-301.fc31.armv7hl'
        script = '''# extlinux.conf generated by appliance-creator
ui menu.c32
menu autoboot Welcome to Fedora-Workstation-armhfp-31-1.9. Automatic boot in # second{,s}. Press a key for options.
menu title Fedora-Workstation-armhfp-31-1.9 Boot Options.
menu hidden
timeout 20
totaltimeout 600

label Fedora-Workstation-armhfp-31-1.9 (5.3.7-301.fc31.armv7hl)
        kernel /%s
        append ro root=UUID=9732b35b-4cd5-458b-9b91-80f7047e0b8a rhgb quiet LANG=en_US.UTF-8 cma=192MB cma=256MB
        fdtdir /%s/
        initrd /%s''' % (vmlinux, dtbdir, initrd)
        ext = os.path.join(mnt, 'extlinux')
        mkdir_cond(ext)

        with open(os.path.join(ext, 'extlinux.conf'), 'w') as fd:
            print(script, file=fd)

        inf = os.path.join(cons.config.persistent_data_dir, 'inf')
        with open(inf, 'wb') as fd:
            fd.write(gzip.compress(b'vmlinux'))
        u_boot_utils.run_and_log(cons, 'mkimage -f auto -d %s %s' %
                                 (inf, os.path.join(mnt, vmlinux)))

        with open(os.path.join(mnt, initrd), 'w') as fd:
            print('initrd', file=fd)

        mkdir_cond(os.path.join(mnt, dtbdir))

        dtb_file = os.path.join(mnt, '%s/sandbox.dtb' % dtbdir)
        u_boot_utils.run_and_log(
            cons, 'dtc -o %s' % dtb_file, stdin=b'/dts-v1/; / {};')
        complete = True
    except ValueError as exc:
        print('Falled to create image, failing back to prepared copy: %s',
              str(exc))
    finally:
        if mounted:
            u_boot_utils.run_and_log(cons, 'sudo umount --lazy %s' % mnt)
        if loop:
            u_boot_utils.run_and_log(cons, 'sudo losetup -d %s' % loop)

    if not complete:
        copy_prepared_image(cons, mmc_dev, fname)


@pytest.mark.buildconfigspec('ut_dm')
def test_ut_dm_init(u_boot_console):
    """Initialize data for ut dm tests."""

    fn = u_boot_console.config.source_dir + '/testflash.bin'
    if not os.path.exists(fn):
        data = b'this is a test'
        data += b'\x00' * ((4 * 1024 * 1024) - len(data))
        with open(fn, 'wb') as fh:
            fh.write(data)

    fn = u_boot_console.config.source_dir + '/spi.bin'
    if not os.path.exists(fn):
        data = b'\x00' * (2 * 1024 * 1024)
        with open(fn, 'wb') as fh:
            fh.write(data)

    # Create a file with a single partition
    fn = u_boot_console.config.source_dir + '/scsi.img'
    if not os.path.exists(fn):
        data = b'\x00' * (2 * 1024 * 1024)
        with open(fn, 'wb') as fh:
            fh.write(data)
        u_boot_utils.run_and_log(
            u_boot_console, f'sfdisk {fn}', stdin=b'type=83')

    fs_helper.mk_fs(u_boot_console.config, 'ext2', 0x200000, '2MB',
                    use_src_dir=True)
    fs_helper.mk_fs(u_boot_console.config, 'fat32', 0x100000, '1MB',
                    use_src_dir=True)

@pytest.mark.buildconfigspec('cmd_bootflow')
def test_ut_dm_init_bootstd(u_boot_console):
    """Initialise data for bootflow tests"""

    setup_bootflow_image(u_boot_console)
    setup_bootmenu_image(u_boot_console)

    # Restart so that the new mmc1.img is picked up
    u_boot_console.restart_uboot()


def test_ut(u_boot_console, ut_subtest):
    """Execute a "ut" subtest.

    The subtests are collected in function generate_ut_subtest() from linker
    generated lists by applying a regular expression to the lines of file
    u-boot.sym. The list entries are created using the C macro UNIT_TEST().

    Strict naming conventions have to be followed to match the regular
    expression. Use UNIT_TEST(foo_test_bar, _flags, foo_test) for a test bar in
    test suite foo that can be executed via command 'ut foo bar' and is
    implemented in C function foo_test_bar().

    Args:
        u_boot_console (ConsoleBase): U-Boot console
        ut_subtest (str): test to be executed via command ut, e.g 'foo bar' to
            execute command 'ut foo bar'
    """

    output = u_boot_console.run_command('ut ' + ut_subtest)
    assert output.endswith('Failures: 0')
