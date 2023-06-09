// SPDX-License-Identifier: GPL-2.0+
/*
 * Test for bootdev functions. All start with 'bootdev'
 *
 * Copyright 2021 Google LLC
 * Written by Simon Glass <sjg@chromium.org>
 */

#include <common.h>
#include <bootdev.h>
#include <bootflow.h>
#include <bootmeth.h>
#include <bootstd.h>
#include <cli.h>
#include <dm.h>
#include <expo.h>
#ifdef CONFIG_SANDBOX
#include <asm/test.h>
#endif
#include <dm/device-internal.h>
#include <dm/lists.h>
#include <test/suites.h>
#include <test/ut.h>
#include "bootstd_common.h"
#include "../../boot/bootflow_internal.h"
#include "../../boot/scene_internal.h"

DECLARE_GLOBAL_DATA_PTR;

extern U_BOOT_DRIVER(bootmeth_script);

static int inject_response(struct unit_test_state *uts)
{
	/*
	 * The image being booted presents a menu of options:
	 *
	 * Fedora-Workstation-armhfp-31-1.9 Boot Options.
	 * 1:   Fedora-Workstation-armhfp-31-1.9 (5.3.7-301.fc31.armv7hl)
	 * Enter choice:
	 *
	 * Provide input for this, to avoid waiting two seconds for a timeout.
	 */
	ut_asserteq(2, console_in_puts("1\n"));

	return 0;
}

/* Check 'bootflow scan/list' commands */
static int bootflow_cmd(struct unit_test_state *uts)
{
	console_record_reset_enable();
	ut_assertok(run_command("bootdev select 1", 0));
	ut_assert_console_end();
	ut_assertok(run_command("bootflow scan -lH", 0));
	ut_assert_nextline("Scanning for bootflows in bootdev 'mmc1.bootdev'");
	ut_assert_nextline("Seq  Method       State   Uclass    Part  Name                      Filename");
	ut_assert_nextlinen("---");
	ut_assert_nextline("Scanning bootdev 'mmc2.bootdev':");
	ut_assert_nextline("Scanning bootdev 'mmc1.bootdev':");
	ut_assert_nextline("  0  extlinux     ready   mmc          1  mmc1.bootdev.part_1       /extlinux/extlinux.conf");
	ut_assert_nextline("No more bootdevs");
	ut_assert_nextlinen("---");
	ut_assert_nextline("(1 bootflow, 1 valid)");
	ut_assert_console_end();

	ut_assertok(run_command("bootflow list", 0));
	ut_assert_nextline("Showing bootflows for bootdev 'mmc1.bootdev'");
	ut_assert_nextline("Seq  Method       State   Uclass    Part  Name                      Filename");
	ut_assert_nextlinen("---");
	ut_assert_nextline("  0  extlinux     ready   mmc          1  mmc1.bootdev.part_1       /extlinux/extlinux.conf");
	ut_assert_nextlinen("---");
	ut_assert_nextline("(1 bootflow, 1 valid)");
	ut_assert_console_end();

	return 0;
}
BOOTSTD_TEST(bootflow_cmd, UT_TESTF_DM | UT_TESTF_SCAN_FDT);

/* Check 'bootflow scan' with a label / seq */
static int bootflow_cmd_label(struct unit_test_state *uts)
{
	test_set_eth_enable(false);

	console_record_reset_enable();
	ut_assertok(run_command("bootflow scan -lH mmc1", 0));
	ut_assert_nextline("Scanning for bootflows with label 'mmc1'");
	ut_assert_skip_to_line("(1 bootflow, 1 valid)");
	ut_assert_console_end();

	ut_assertok(run_command("bootflow scan -lH 0", 0));
	ut_assert_nextline("Scanning for bootflows with label '0'");
	ut_assert_skip_to_line("(0 bootflows, 0 valid)");
	ut_assert_console_end();

	/*
	 * with ethernet enabled we have 8 devices ahead of the mmc ones:
	 *
	 * ut_assertok(run_command("bootdev list", 0));
	 * Seq  Probed  Status  Uclass    Name
	 * ---  ------  ------  --------  ------------------
	 * 0   [ + ]      OK  ethernet  eth@10002000.bootdev
	 * 1   [   ]      OK  ethernet  eth@10003000.bootdev
	 * 2   [   ]      OK  ethernet  sbe5.bootdev
	 * 3   [   ]      OK  ethernet  eth@10004000.bootdev
	 * 4   [   ]      OK  ethernet  phy-test-eth.bootdev
	 * 5   [   ]      OK  ethernet  dsa-test-eth.bootdev
	 * 6   [   ]      OK  ethernet  dsa-test@0.bootdev
	 * 7   [   ]      OK  ethernet  dsa-test@1.bootdev
	 * 8   [   ]      OK  mmc       mmc2.bootdev
	 * 9   [ + ]      OK  mmc       mmc1.bootdev
	 * a   [   ]      OK  mmc       mmc0.bootdev
	 */
	ut_assertok(run_command("bootflow scan -lH 9", 0));
	ut_assert_nextline("Scanning for bootflows with label '9'");
	ut_assert_skip_to_line("(1 bootflow, 1 valid)");

	ut_assertok(run_command("bootflow scan -lH 0", 0));
	ut_assert_nextline("Scanning for bootflows with label '0'");
	ut_assert_skip_to_line("(0 bootflows, 0 valid)");
	ut_assert_console_end();

	return 0;
}
BOOTSTD_TEST(bootflow_cmd_label, UT_TESTF_DM | UT_TESTF_SCAN_FDT |
	     UT_TESTF_ETH_BOOTDEV);

/* Check 'bootflow scan/list' commands using all bootdevs */
static int bootflow_cmd_glob(struct unit_test_state *uts)
{
	ut_assertok(bootstd_test_drop_bootdev_order(uts));

	console_record_reset_enable();
	ut_assertok(run_command("bootflow scan -lGH", 0));
	ut_assert_nextline("Scanning for bootflows in all bootdevs");
	ut_assert_nextline("Seq  Method       State   Uclass    Part  Name                      Filename");
	ut_assert_nextlinen("---");
	ut_assert_nextline("Scanning bootdev 'mmc2.bootdev':");
	ut_assert_nextline("Scanning bootdev 'mmc1.bootdev':");
	ut_assert_nextline("  0  extlinux     ready   mmc          1  mmc1.bootdev.part_1       /extlinux/extlinux.conf");
	ut_assert_nextline("Scanning bootdev 'mmc0.bootdev':");
	ut_assert_nextline("No more bootdevs");
	ut_assert_nextlinen("---");
	ut_assert_nextline("(1 bootflow, 1 valid)");
	ut_assert_console_end();

	ut_assertok(run_command("bootflow list", 0));
	ut_assert_nextline("Showing all bootflows");
	ut_assert_nextline("Seq  Method       State   Uclass    Part  Name                      Filename");
	ut_assert_nextlinen("---");
	ut_assert_nextline("  0  extlinux     ready   mmc          1  mmc1.bootdev.part_1       /extlinux/extlinux.conf");
	ut_assert_nextlinen("---");
	ut_assert_nextline("(1 bootflow, 1 valid)");
	ut_assert_console_end();

	return 0;
}
BOOTSTD_TEST(bootflow_cmd_glob, UT_TESTF_DM | UT_TESTF_SCAN_FDT);

/* Check 'bootflow scan -e' */
static int bootflow_cmd_scan_e(struct unit_test_state *uts)
{
	ut_assertok(bootstd_test_drop_bootdev_order(uts));

	console_record_reset_enable();
	ut_assertok(run_command("bootflow scan -aleGH", 0));
	ut_assert_nextline("Scanning for bootflows in all bootdevs");
	ut_assert_nextline("Seq  Method       State   Uclass    Part  Name                      Filename");
	ut_assert_nextlinen("---");
	ut_assert_nextline("Scanning bootdev 'mmc2.bootdev':");
	ut_assert_nextline("  0  extlinux     media   mmc          0  mmc2.bootdev.whole        <NULL>");
	ut_assert_nextline("     ** No partition found, err=-93: Protocol not supported");
	ut_assert_nextline("  1  efi          media   mmc          0  mmc2.bootdev.whole        <NULL>");
	ut_assert_nextline("     ** No partition found, err=-93: Protocol not supported");

	ut_assert_nextline("Scanning bootdev 'mmc1.bootdev':");
	ut_assert_nextline("  2  extlinux     media   mmc          0  mmc1.bootdev.whole        <NULL>");
	ut_assert_nextline("     ** No partition found, err=-2: No such file or directory");
	ut_assert_nextline("  3  efi          media   mmc          0  mmc1.bootdev.whole        <NULL>");
	ut_assert_nextline("     ** No partition found, err=-2: No such file or directory");
	ut_assert_nextline("  4  extlinux     ready   mmc          1  mmc1.bootdev.part_1       /extlinux/extlinux.conf");
	ut_assert_nextline("  5  efi          fs      mmc          1  mmc1.bootdev.part_1       efi/boot/bootsbox.efi");

	ut_assert_skip_to_line("Scanning bootdev 'mmc0.bootdev':");
	ut_assert_skip_to_line(" 3f  efi          media   mmc          0  mmc0.bootdev.whole        <NULL>");
	ut_assert_nextline("     ** No partition found, err=-93: Protocol not supported");
	ut_assert_nextline("No more bootdevs");
	ut_assert_nextlinen("---");
	ut_assert_nextline("(64 bootflows, 1 valid)");
	ut_assert_console_end();

	ut_assertok(run_command("bootflow list", 0));
	ut_assert_nextline("Showing all bootflows");
	ut_assert_nextline("Seq  Method       State   Uclass    Part  Name                      Filename");
	ut_assert_nextlinen("---");
	ut_assert_nextline("  0  extlinux     media   mmc          0  mmc2.bootdev.whole        <NULL>");
	ut_assert_nextline("  1  efi          media   mmc          0  mmc2.bootdev.whole        <NULL>");
	ut_assert_skip_to_line("  4  extlinux     ready   mmc          1  mmc1.bootdev.part_1       /extlinux/extlinux.conf");
	ut_assert_skip_to_line(" 3f  efi          media   mmc          0  mmc0.bootdev.whole        <NULL>");
	ut_assert_nextlinen("---");
	ut_assert_nextline("(64 bootflows, 1 valid)");
	ut_assert_console_end();

	return 0;
}
BOOTSTD_TEST(bootflow_cmd_scan_e, UT_TESTF_DM | UT_TESTF_SCAN_FDT);

/* Check 'bootflow info' */
static int bootflow_cmd_info(struct unit_test_state *uts)
{
	console_record_reset_enable();
	ut_assertok(run_command("bootdev select 1", 0));
	ut_assert_console_end();
	ut_assertok(run_command("bootflow scan", 0));
	ut_assert_console_end();
	ut_assertok(run_command("bootflow select 0", 0));
	ut_assert_console_end();
	ut_assertok(run_command("bootflow info", 0));
	ut_assert_nextline("Name:      mmc1.bootdev.part_1");
	ut_assert_nextline("Device:    mmc1.bootdev");
	ut_assert_nextline("Block dev: mmc1.blk");
	ut_assert_nextline("Method:    extlinux");
	ut_assert_nextline("State:     ready");
	ut_assert_nextline("Partition: 1");
	ut_assert_nextline("Subdir:    (none)");
	ut_assert_nextline("Filename:  /extlinux/extlinux.conf");
	ut_assert_nextlinen("Buffer:    ");
	ut_assert_nextline("Size:      253 (595 bytes)");
	ut_assert_nextline("OS:        Fedora-Workstation-armhfp-31-1.9 (5.3.7-301.fc31.armv7hl)");
	ut_assert_nextline("Logo:      (none)");
	ut_assert_nextline("FDT:       <NULL>");
	ut_assert_nextline("Error:     0");
	ut_assert_console_end();

	ut_assertok(run_command("bootflow info -d", 0));
	ut_assert_nextline("Name:      mmc1.bootdev.part_1");
	ut_assert_skip_to_line("Error:     0");
	ut_assert_nextline("Contents:");
	ut_assert_nextline("%s", "");
	ut_assert_nextline("# extlinux.conf generated by appliance-creator");
	ut_assert_skip_to_line("        initrd /initramfs-5.3.7-301.fc31.armv7hl.img");
	ut_assert_console_end();

	return 0;
}
BOOTSTD_TEST(bootflow_cmd_info, UT_TESTF_DM | UT_TESTF_SCAN_FDT);

/* Check 'bootflow scan -b' to boot the first available bootdev */
static int bootflow_scan_boot(struct unit_test_state *uts)
{
	console_record_reset_enable();
	ut_assertok(inject_response(uts));
	ut_assertok(run_command("bootflow scan -b", 0));
	ut_assert_nextline(
		"** Booting bootflow 'mmc1.bootdev.part_1' with extlinux");
	ut_assert_nextline("Ignoring unknown command: ui");

	/*
	 * We expect it to get through to boot although sandbox always returns
	 * -EFAULT as it cannot actually boot the kernel
	 */
	ut_assert_skip_to_line("sandbox: continuing, as we cannot run Linux");
	ut_assert_nextline("Boot failed (err=-14)");
	ut_assert_console_end();

	return 0;
}
BOOTSTD_TEST(bootflow_scan_boot, UT_TESTF_DM | UT_TESTF_SCAN_FDT);

/* Check iterating through available bootflows */
static int bootflow_iter(struct unit_test_state *uts)
{
	struct bootflow_iter iter;
	struct bootflow bflow;

	bootstd_clear_glob();

	/* The first device is mmc2.bootdev which has no media */
	ut_asserteq(-EPROTONOSUPPORT,
		    bootflow_scan_first(NULL, NULL, &iter,
					BOOTFLOWIF_ALL | BOOTFLOWIF_SKIP_GLOBAL, &bflow));
	ut_asserteq(2, iter.num_methods);
	ut_asserteq(0, iter.cur_method);
	ut_asserteq(0, iter.part);
	ut_asserteq(0, iter.max_part);
	ut_asserteq_str("extlinux", iter.method->name);
	ut_asserteq(0, bflow.err);

	/*
	 * This shows MEDIA even though there is none, since in
	 * bootdev_find_in_blk() we call part_get_info() which returns
	 * -EPROTONOSUPPORT. Ideally it would return -EEOPNOTSUPP and we would
	 * know.
	 */
	ut_asserteq(BOOTFLOWST_MEDIA, bflow.state);

	ut_asserteq(-EPROTONOSUPPORT, bootflow_scan_next(&iter, &bflow));
	ut_asserteq(2, iter.num_methods);
	ut_asserteq(1, iter.cur_method);
	ut_asserteq(0, iter.part);
	ut_asserteq(0, iter.max_part);
	ut_asserteq_str("efi", iter.method->name);
	ut_asserteq(0, bflow.err);
	ut_asserteq(BOOTFLOWST_MEDIA, bflow.state);
	bootflow_free(&bflow);

	/* The next device is mmc1.bootdev - at first we use the whole device */
	ut_asserteq(-ENOENT, bootflow_scan_next(&iter, &bflow));
	ut_asserteq(2, iter.num_methods);
	ut_asserteq(0, iter.cur_method);
	ut_asserteq(0, iter.part);
	ut_asserteq(0x1e, iter.max_part);
	ut_asserteq_str("extlinux", iter.method->name);
	ut_asserteq(0, bflow.err);
	ut_asserteq(BOOTFLOWST_MEDIA, bflow.state);
	bootflow_free(&bflow);

	ut_asserteq(-ENOENT, bootflow_scan_next(&iter, &bflow));
	ut_asserteq(2, iter.num_methods);
	ut_asserteq(1, iter.cur_method);
	ut_asserteq(0, iter.part);
	ut_asserteq(0x1e, iter.max_part);
	ut_asserteq_str("efi", iter.method->name);
	ut_asserteq(0, bflow.err);
	ut_asserteq(BOOTFLOWST_MEDIA, bflow.state);
	bootflow_free(&bflow);

	/* Then more to partition 1 where we find something */
	ut_assertok(bootflow_scan_next(&iter, &bflow));
	ut_asserteq(2, iter.num_methods);
	ut_asserteq(0, iter.cur_method);
	ut_asserteq(1, iter.part);
	ut_asserteq(0x1e, iter.max_part);
	ut_asserteq_str("extlinux", iter.method->name);
	ut_asserteq(0, bflow.err);
	ut_asserteq(BOOTFLOWST_READY, bflow.state);
	bootflow_free(&bflow);

	ut_asserteq(-ENOENT, bootflow_scan_next(&iter, &bflow));
	ut_asserteq(2, iter.num_methods);
	ut_asserteq(1, iter.cur_method);
	ut_asserteq(1, iter.part);
	ut_asserteq(0x1e, iter.max_part);
	ut_asserteq_str("efi", iter.method->name);
	ut_asserteq(0, bflow.err);
	ut_asserteq(BOOTFLOWST_FS, bflow.state);
	bootflow_free(&bflow);

	/* Then more to partition 2 which exists but is not bootable */
	ut_asserteq(-EINVAL, bootflow_scan_next(&iter, &bflow));
	ut_asserteq(2, iter.num_methods);
	ut_asserteq(0, iter.cur_method);
	ut_asserteq(2, iter.part);
	ut_asserteq(0x1e, iter.max_part);
	ut_asserteq_str("extlinux", iter.method->name);
	ut_asserteq(0, bflow.err);
	ut_asserteq(BOOTFLOWST_MEDIA, bflow.state);
	bootflow_free(&bflow);

	bootflow_iter_uninit(&iter);

	ut_assert_console_end();

	return 0;
}
BOOTSTD_TEST(bootflow_iter, UT_TESTF_DM | UT_TESTF_SCAN_FDT);

#if defined(CONFIG_SANDBOX) && defined(CONFIG_BOOTMETH_GLOBAL)
/* Check using the system bootdev */
static int bootflow_system(struct unit_test_state *uts)
{
	struct udevice *bootstd, *dev;

	if (!IS_ENABLED(CONFIG_CMD_BOOTEFI_BOOTMGR))
		return -EAGAIN;
	ut_assertok(uclass_first_device_err(UCLASS_BOOTSTD, &bootstd));
	ut_assertok(device_bind(bootstd, DM_DRIVER_GET(bootmeth_efi_mgr),
				"efi_mgr", 0, ofnode_null(), &dev));
	ut_assertok(device_probe(dev));
	sandbox_set_fake_efi_mgr_dev(dev, true);

	/* We should get a single 'bootmgr' method right at the end */
	bootstd_clear_glob();
	console_record_reset_enable();
	ut_assertok(run_command("bootflow scan -lH", 0));
	ut_assert_skip_to_line(
		"  0  efi_mgr      ready   (none)       0  <NULL>                    <NULL>");
	ut_assert_skip_to_line("No more bootdevs");
	ut_assert_skip_to_line("(2 bootflows, 2 valid)");
	ut_assert_console_end();

	return 0;
}
BOOTSTD_TEST(bootflow_system, UT_TESTF_DM | UT_TESTF_SCAN_PDATA |
	     UT_TESTF_SCAN_FDT);
#endif

/* Check disabling a bootmethod if it requests it */
static int bootflow_iter_disable(struct unit_test_state *uts)
{
	struct udevice *bootstd, *dev;
	struct bootflow_iter iter;
	struct bootflow bflow;
	int i;

	/* Add the EFI bootmgr driver */
	ut_assertok(uclass_first_device_err(UCLASS_BOOTSTD, &bootstd));
	ut_assertok(device_bind_driver(bootstd, "bootmeth_sandbox", "sandbox",
				       &dev));

	ut_assertok(bootstd_test_drop_bootdev_order(uts));

	bootstd_clear_glob();
	console_record_reset_enable();
	ut_assertok(inject_response(uts));
	ut_assertok(run_command("bootflow scan -lbH", 0));

	/* Try to boot the bootmgr flow, which will fail */
	console_record_reset_enable();
	ut_assertok(bootflow_scan_first(NULL, NULL, &iter, 0, &bflow));
	ut_asserteq(3, iter.num_methods);
	ut_asserteq_str("sandbox", iter.method->name);
	ut_assertok(inject_response(uts));
	ut_asserteq(-ENOTSUPP, bootflow_run_boot(&iter, &bflow));

	ut_assert_skip_to_line("Boot method 'sandbox' failed and will not be retried");
	ut_assert_console_end();

	/* Check that the sandbox bootmeth has been removed */
	ut_asserteq(2, iter.num_methods);
	for (i = 0; i < iter.num_methods; i++)
		ut_assert(strcmp("sandbox", iter.method_order[i]->name));

	return 0;
}
BOOTSTD_TEST(bootflow_iter_disable, UT_TESTF_DM | UT_TESTF_SCAN_FDT);

/* Check 'bootflow scan' with a bootmeth ordering including a global bootmeth */
static int bootflow_scan_glob_bootmeth(struct unit_test_state *uts)
{
	if (!IS_ENABLED(CONFIG_BOOTMETH_GLOBAL))
		return -EAGAIN;

	ut_assertok(bootstd_test_drop_bootdev_order(uts));

	/*
	 * Make sure that the -G flag makes the scan fail, since this is not
	 * supported when an ordering is provided
	 */
	console_record_reset_enable();
	ut_assertok(bootmeth_set_order("efi firmware0"));
	ut_assertok(run_command("bootflow scan -lGH", 0));
	ut_assert_nextline("Scanning for bootflows in all bootdevs");
	ut_assert_nextline(
		"Seq  Method       State   Uclass    Part  Name                      Filename");
	ut_assert_nextlinen("---");
	ut_assert_nextlinen("---");
	ut_assert_nextline("(0 bootflows, 0 valid)");
	ut_assert_console_end();

	ut_assertok(run_command("bootflow scan -lH", 0));
	ut_assert_nextline("Scanning for bootflows in all bootdevs");
	ut_assert_nextline(
		"Seq  Method       State   Uclass    Part  Name                      Filename");
	ut_assert_nextlinen("---");
	ut_assert_nextline("Scanning global bootmeth 'firmware0':");
	ut_assert_nextline("Scanning bootdev 'mmc2.bootdev':");
	ut_assert_nextline("Scanning bootdev 'mmc1.bootdev':");
	ut_assert_nextline("Scanning bootdev 'mmc0.bootdev':");
	ut_assert_nextline("No more bootdevs");
	ut_assert_nextlinen("---");
	ut_assert_nextline("(0 bootflows, 0 valid)");
	ut_assert_console_end();

	return 0;
}
BOOTSTD_TEST(bootflow_scan_glob_bootmeth, UT_TESTF_DM | UT_TESTF_SCAN_FDT);

/* Check 'bootflow boot' to boot a selected bootflow */
static int bootflow_cmd_boot(struct unit_test_state *uts)
{
	console_record_reset_enable();
	ut_assertok(run_command("bootdev select 1", 0));
	ut_assert_console_end();
	ut_assertok(run_command("bootflow scan", 0));
	ut_assert_console_end();
	ut_assertok(run_command("bootflow select 0", 0));
	ut_assert_console_end();

	ut_assertok(inject_response(uts));
	ut_asserteq(1, run_command("bootflow boot", 0));
	ut_assert_nextline(
		"** Booting bootflow 'mmc1.bootdev.part_1' with extlinux");
	ut_assert_nextline("Ignoring unknown command: ui");

	/*
	 * We expect it to get through to boot although sandbox always returns
	 * -EFAULT as it cannot actually boot the kernel
	 */
	ut_assert_skip_to_line("sandbox: continuing, as we cannot run Linux");
	ut_assert_nextline("Boot failed (err=-14)");
	ut_assert_console_end();

	return 0;
}
BOOTSTD_TEST(bootflow_cmd_boot, UT_TESTF_DM | UT_TESTF_SCAN_FDT);

/**
 * prep_mmc4_bootdev() - Set up the mmc4 bootdev so we can access a fake Armbian
 *
 * @uts: Unit test state
 * Returns 0 on success, -ve on failure
 */
static int prep_mmc4_bootdev(struct unit_test_state *uts)
{
	static const char *order[] = {"mmc2", "mmc1", "mmc4", NULL};
	struct udevice *dev, *bootstd;
	struct bootstd_priv *std;
	const char **old_order;
	ofnode node;

	/* Enable the mmc4 node since we need a second bootflow */
	node = ofnode_path("/mmc4");
	ut_assertok(lists_bind_fdt(gd->dm_root, node, &dev, NULL, false));

	/* Enable the script bootmeth too */
	ut_assertok(uclass_first_device_err(UCLASS_BOOTSTD, &bootstd));
	ut_assertok(device_bind(bootstd, DM_DRIVER_REF(bootmeth_script),
				"bootmeth_script", 0, ofnode_null(), &dev));

	/* Change the order to include mmc4 */
	std = dev_get_priv(bootstd);
	old_order = std->bootdev_order;
	std->bootdev_order = order;

	console_record_reset_enable();
	ut_assertok(run_command("bootflow scan", 0));
	ut_assert_console_end();

	/* Restore the order used by the device tree */
	std->bootdev_order = old_order;

	return 0;
}

/* Check 'bootflow menu' to select a bootflow */
static int bootflow_cmd_menu(struct unit_test_state *uts)
{
	char prev[3];

	ut_assertok(prep_mmc4_bootdev(uts));

	/* Add keypresses to move to and select the second one in the list */
	prev[0] = CTL_CH('n');
	prev[1] = '\r';
	prev[2] = '\0';
	ut_asserteq(2, console_in_puts(prev));

	ut_assertok(run_command("bootflow menu", 0));
	ut_assert_nextline("Selected: Armbian");
	ut_assert_console_end();

	return 0;
}
BOOTSTD_TEST(bootflow_cmd_menu, UT_TESTF_DM | UT_TESTF_SCAN_FDT);

/* Check searching for a single bootdev using the hunters */
static int bootflow_cmd_hunt_single(struct unit_test_state *uts)
{
	struct bootstd_priv *std;

	/* get access to the used hunters */
	ut_assertok(bootstd_get_priv(&std));

	ut_assertok(bootstd_test_drop_bootdev_order(uts));

	console_record_reset_enable();
	ut_assertok(run_command("bootflow scan -l mmc1", 0));
	ut_assert_nextline("Scanning for bootflows with label 'mmc1'");
	ut_assert_skip_to_line("(1 bootflow, 1 valid)");
	ut_assert_console_end();

	/* check that the hunter was used */
	ut_asserteq(BIT(MMC_HUNTER) | BIT(1), std->hunters_used);

	return 0;
}
BOOTSTD_TEST(bootflow_cmd_hunt_single, UT_TESTF_DM | UT_TESTF_SCAN_FDT);

/* Check searching for a uclass label using the hunters */
static int bootflow_cmd_hunt_label(struct unit_test_state *uts)
{
	struct bootstd_priv *std;

	/* get access to the used hunters */
	ut_assertok(bootstd_get_priv(&std));

	test_set_skip_delays(true);
	test_set_eth_enable(false);
	ut_assertok(bootstd_test_drop_bootdev_order(uts));

	console_record_reset_enable();
	ut_assertok(run_command("bootflow scan -l mmc", 0));

	/* check that the hunter was used */
	ut_asserteq(BIT(MMC_HUNTER) | BIT(1), std->hunters_used);

	/* check that we got the mmc1 bootflow */
	ut_assert_nextline("Scanning for bootflows with label 'mmc'");
	ut_assert_nextlinen("Seq");
	ut_assert_nextlinen("---");
	ut_assert_nextline("Hunting with: simple_bus");
	ut_assert_nextline("Found 2 extension board(s).");
	ut_assert_nextline("Hunting with: mmc");
	ut_assert_nextline("Scanning bootdev 'mmc2.bootdev':");
	ut_assert_nextline("Scanning bootdev 'mmc1.bootdev':");
	ut_assert_nextline(
		"  0  extlinux     ready   mmc          1  mmc1.bootdev.part_1       /extlinux/extlinux.conf");
	ut_assert_nextline("Scanning bootdev 'mmc0.bootdev':");
	ut_assert_skip_to_line("(1 bootflow, 1 valid)");
	ut_assert_console_end();

	return 0;
}
BOOTSTD_TEST(bootflow_cmd_hunt_label, UT_TESTF_DM | UT_TESTF_SCAN_FDT);

/**
 * check_font() - Check that the font size for an item matches expectations
 *
 * @uts: Unit test state
 * @scn: Scene containing the text object
 * @id: ID of the text object
 * Returns 0 on success, -ve on failure
 */
static int check_font(struct unit_test_state *uts, struct scene *scn, uint id,
		      int font_size)
{
	struct scene_obj_txt *txt;

	txt = scene_obj_find(scn, id, SCENEOBJT_TEXT);
	ut_assertnonnull(txt);

	ut_asserteq(font_size, txt->font_size);

	return 0;
}

/* Check themes work with a bootflow menu */
static int bootflow_menu_theme(struct unit_test_state *uts)
{
	const int font_size = 30;
	struct scene *scn;
	struct expo *exp;
	ofnode node;
	int i;

	ut_assertok(prep_mmc4_bootdev(uts));

	ut_assertok(bootflow_menu_new(&exp));
	node = ofnode_path("/bootstd/theme");
	ut_assert(ofnode_valid(node));
	ut_assertok(bootflow_menu_apply_theme(exp, node));

	scn = expo_lookup_scene_id(exp, MAIN);
	ut_assertnonnull(scn);

	/*
	 * Check that the txt objects have the correct font size from the
	 * device tree node: bootstd/theme
	 *
	 * Check both menu items, since there are two bootflows
	 */
	ut_assertok(check_font(uts, scn, OBJ_PROMPT, font_size));
	ut_assertok(check_font(uts, scn, OBJ_POINTER, font_size));
	for (i = 0; i < 2; i++) {
		ut_assertok(check_font(uts, scn, ITEM_DESC + i, font_size));
		ut_assertok(check_font(uts, scn, ITEM_KEY + i, font_size));
		ut_assertok(check_font(uts, scn, ITEM_LABEL + i, font_size));
	}

	expo_destroy(exp);

	return 0;
}
BOOTSTD_TEST(bootflow_menu_theme, UT_TESTF_DM | UT_TESTF_SCAN_FDT);
