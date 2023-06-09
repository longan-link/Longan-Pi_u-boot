/* SPDX-License-Identifier: GPL-2.0+ */
/*
 * Copyright (c) 2013, Google Inc.
 */

#ifndef ARM_BOOTM_H
#define ARM_BOOTM_H

void bootm_announce_and_cleanup(void);

/**
 * boot_linux_kernel() - boot a linux kernel
 *
 * This boots a kernel image, either 32-bit or 64-bit. It will also work with
 * a self-extracting kernel, if you set @image_64bit to false.
 *
 * @setup_base:		Address of the setup.bin information for the kernel
 * @entry:		Address of the kernel entry point
 * @image_64bit:	true if the image is a raw 64-bit kernel, or a kernel
 * which supports booting in 64-bit mode; false if it is raw 32-bit or any type
 * of self-extracting kernel such as a bzImage.
 * Return: -ve error code. This function does not return if the kernel was
 * booted successfully.
 */
int boot_linux_kernel(ulong setup_base, ulong entry, bool image_64bit);

#endif
