/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Copyright (C) Marvell International Ltd. and its affiliates
 */

#ifndef _SYS_ENV_LIB_H
#define _SYS_ENV_LIB_H

#include "../../../drivers/ddr/marvell/a38x/ddr3_init.h"

/* Serdes definitions */
#define COMMON_PHY_BASE_ADDR		0x18300

#define DEVICE_CONFIGURATION_REG0	0x18284
#define DEVICE_CONFIGURATION_REG1	0x18288
#define COMMON_PHY_CONFIGURATION1_REG	0x18300
#define COMMON_PHY_CONFIGURATION2_REG	0x18304
#define COMMON_PHY_CONFIGURATION4_REG	0x1830c
#define COMMON_PHY_STATUS1_REG		0x18318
#define COMMON_PHYS_SELECTORS_REG	0x183fc
#define SOC_CONTROL_REG1		0x18204
#define GENERAL_PURPOSE_RESERVED0_REG	0x182e0
#define GBE_CONFIGURATION_REG		0x18460
#define DEVICE_SAMPLE_AT_RESET1_REG	0x18600
#define DEVICE_SAMPLE_AT_RESET2_REG	0x18604
#define DEV_ID_REG			0x18238

#define CORE_PLL_PARAMETERS_REG		0xe42e0
#define CORE_PLL_CONFIG_REG		0xe42e4

#define QSGMII_CONTROL_REG1		0x18494

#define DEV_ID_REG_DEVICE_ID_OFFS	16
#define DEV_ID_REG_DEVICE_ID_MASK	0xffff0000

#define SAR_FREQ_OFFSET			10
#define SAR_FREQ_MASK			0x1f
#define SAR_DEV_ID_OFFS			27
#define SAR_DEV_ID_MASK			0x7

#define POWER_AND_PLL_CTRL_REG		0xa0004
#define CALIBRATION_CTRL_REG		0xa0008
#define DFE_REG0			0xa001c
#define DFE_REG3			0xa0028
#define RESET_DFE_REG			0xa0148
#define LOOPBACK_REG			0xa008c
#define SYNC_PATTERN_REG		0xa0090
#define INTERFACE_REG			0xa0094
#define ISOLATE_REG			0xa0098
#define MISC_REG			0xa013c
#define GLUE_REG			0xa0140
#define GENERATION_DIVIDER_FORCE_REG	0xa0144
#define PLLINTP_REG1			0xa0150
#define PCIE_REG0			0xa0120
#define LANE_ALIGN_REG0			0xa0124
#define SQUELCH_FFE_SETTING_REG		0xa0018
#define G1_SETTINGS_0_REG		0xa0034
#define G1_SETTINGS_1_REG		0xa0038
#define G1_SETTINGS_3_REG		0xa0440
#define G1_SETTINGS_4_REG		0xa0444
#define G2_SETTINGS_0_REG		0xa003c
#define G2_SETTINGS_1_REG		0xa0040
#define G2_SETTINGS_2_REG		0xa00f8
#define G2_SETTINGS_3_REG		0xa0448
#define G2_SETTINGS_4_REG		0xa044c
#define G3_SETTINGS_0_REG		0xa0044
#define G3_SETTINGS_1_REG		0xa0048
#define G3_SETTINGS_3_REG		0xa0450
#define G3_SETTINGS_4_REG		0xa0454
#define VTHIMPCAL_CTRL_REG		0xa0104
#define REF_REG0			0xa0134
#define CAL_REG6			0xa0168
#define RX_REG2				0xa0184
#define RX_REG3				0xa0188
#define PCIE_REG1			0xa0288
#define PCIE_REG3			0xa0290
#define LANE_CFG0_REG			0xa0600
#define LANE_CFG1_REG			0xa0604
#define LANE_CFG4_REG			0xa0620
#define LANE_CFG5_REG			0xa0624
#define GLOBAL_CLK_CTRL			0xa0704
#define GLOBAL_TEST_CTRL		0xa0708
#define GLOBAL_MISC_CTRL		0xa0718
#define GLOBAL_CLK_SRC_HI		0xa0710

#define GLOBAL_CLK_CTRL			0xa0704
#define GLOBAL_MISC_CTRL		0xa0718
#define GLOBAL_PM_CTRL			0xa0740

/* SATA registers */
#define SATA_CTRL_REG_IND_ADDR		0xa80a0
#define SATA_CTRL_REG_IND_DATA		0xa80a4

#define SATA_VENDOR_PORT_0_REG_ADDR	0xa8178
#define SATA_VENDOR_PORT_1_REG_ADDR	0xa81f8
#define SATA_VENDOR_PORT_0_REG_DATA	0xa817c
#define SATA_VENDOR_PORT_1_REG_DATA	0xa81fc

/* Reference clock values and mask */
#define POWER_AND_PLL_CTRL_REG_100MHZ_VAL	0x0
#define POWER_AND_PLL_CTRL_REG_25MHZ_VAL_1	0x1
#define POWER_AND_PLL_CTRL_REG_25MHZ_VAL_2	0x2
#define POWER_AND_PLL_CTRL_REG_40MHZ_VAL	0x3
#define GLOBAL_PM_CTRL_REG_25MHZ_VAL		0x7
#define GLOBAL_PM_CTRL_REG_40MHZ_VAL		0xc
#define LANE_CFG4_REG_25MHZ_VAL			0x200
#define LANE_CFG4_REG_40MHZ_VAL			0x300

#define POWER_AND_PLL_CTRL_REG_MASK		(~(0x1f))
#define GLOBAL_PM_CTRL_REG_MASK			(~(0xff))
#define LANE_CFG4_REG_MASK			(~(0x1f00))

#define REF_CLK_SELECTOR_VAL_PEX0(reg_val)	(reg_val >> 2) & 0x1
#define REF_CLK_SELECTOR_VAL_PEX1(reg_val)	(reg_val >> 3) & 0x1
#define REF_CLK_SELECTOR_VAL_PEX2(reg_val)	(reg_val >> 30) & 0x1
#define REF_CLK_SELECTOR_VAL_PEX3(reg_val)	(reg_val >> 31) & 0x1
#define REF_CLK_SELECTOR_VAL(reg_val)		(reg_val & 0x1)

#define MAX_SELECTOR_VAL			10

/* TWSI addresses */
/* starting from A38x A0, i2c address of EEPROM is 0x57 */
#define EEPROM_I2C_ADDR			(sys_env_device_rev_get() == \
					 MV_88F68XX_Z1_ID ? 0x50 : 0x57)
#define RD_GET_MODE_ADDR		0x4c
#define DB_GET_MODE_SLM1363_ADDR	0x25
#define DB_GET_MODE_SLM1364_ADDR	0x24
#define DB381_GET_MODE_SLM1426_1427_ADDR 0x56

/* DB-BP Board 'SatR' mapping */
#define SATR_DB_LANE1_MAX_OPTIONS	7
#define SATR_DB_LANE1_CFG_MASK		0x7
#define SATR_DB_LANE1_CFG_OFFSET	0
#define SATR_DB_LANE2_MAX_OPTIONS	4
#define SATR_DB_LANE2_CFG_MASK		0x38
#define SATR_DB_LANE2_CFG_OFFSET	3

/* GP Board 'SatR' mapping */
#define SATR_GP_LANE1_CFG_MASK		0x4
#define SATR_GP_LANE1_CFG_OFFSET	2
#define SATR_GP_LANE2_CFG_MASK		0x8
#define SATR_GP_LANE2_CFG_OFFSET	3

/* For setting MPP2 and MPP3 to be TWSI mode and MPP 0,1 to UART mode */
#define MPP_CTRL_REG			0x18000
#define MPP_SET_MASK			(~(0xffff))
#define MPP_SET_DATA			(0x1111)
#define MPP_UART1_SET_MASK		(~(0xff000))
#define MPP_UART1_SET_DATA		(0x66000)

#define DFX_PIPE_SELECT_PIPE0_ACTIVE_OFFS	0
/* DFX_PIPE_SELECT_XBAR_CLIENT_SEL_OFFS: Since address completion in 14bit
 * address mode, and given that [14:8] => [19:13], the 2 lower bits [9:8] =>
 * [14:13] are dismissed. hence field offset is also shifted to 10
 */
#define DFX_PIPE_SELECT_XBAR_CLIENT_SEL_OFFS	10

#define RTC_MEMORY_CTRL_REG_BASE	0xE6000
#define RTC_MEMORY_WRAPPER_COUNT	8
#define RTC_MEMORY_WRAPPER_REG(i)	(RTC_MEMORY_CTRL_REG_BASE + ((i) * 0x40))
#define RTC_MEMORY_CTRL_PDLVMC_FIELD_OFFS	6
#define RTC_MEMORY_WRAPPER_CTRL_VAL	(0x1 << RTC_MEMORY_CTRL_PDLVMC_FIELD_OFFS)

#define AVS_DEBUG_CNTR_REG		0xe4124
#define AVS_DEBUG_CNTR_DEFAULT_VALUE	0x08008073

#define AVS_ENABLED_CONTROL		0xe4130
#define AVS_LOW_VDD_LIMIT_OFFS		4
#define AVS_LOW_VDD_LIMIT_MASK		(0xff << AVS_LOW_VDD_LIMIT_OFFS)
#define AVS_LOW_VDD_LIMIT_VAL		(0x27 << AVS_LOW_VDD_LIMIT_OFFS)
#define AVS_LOW_VDD_SLOW_VAL		(0x23 << AVS_LOW_VDD_LIMIT_OFFS)

#define AVS_HIGH_VDD_LIMIT_OFFS		12
#define AVS_HIGH_VDD_LIMIT_MASK		(0xff << AVS_HIGH_VDD_LIMIT_OFFS)
#define AVS_HIGH_VDD_LIMIT_VAL		(0x27 << AVS_HIGH_VDD_LIMIT_OFFS)
#define AVS_HIGH_VDD_SLOW_VAL		(0x23 << AVS_HIGH_VDD_LIMIT_OFFS)

/* Board ID numbers */
#define MARVELL_BOARD_ID_MASK		0x10
/* Customer boards for A38x */
#define A38X_CUSTOMER_BOARD_ID_BASE	0x0
#define A38X_CUSTOMER_BOARD_ID0		(A38X_CUSTOMER_BOARD_ID_BASE + 0)
#define A38X_CUSTOMER_BOARD_ID1		(A38X_CUSTOMER_BOARD_ID_BASE + 1)
#define A38X_MV_MAX_CUSTOMER_BOARD_ID	(A38X_CUSTOMER_BOARD_ID_BASE + 2)
#define A38X_MV_CUSTOMER_BOARD_NUM	(A38X_MV_MAX_CUSTOMER_BOARD_ID - \
					 A38X_CUSTOMER_BOARD_ID_BASE)

/* Marvell boards for A38x */
#define A38X_MARVELL_BOARD_ID_BASE	0x10
#define RD_NAS_68XX_ID			(A38X_MARVELL_BOARD_ID_BASE + 0)
#define DB_68XX_ID			(A38X_MARVELL_BOARD_ID_BASE + 1)
#define RD_AP_68XX_ID			(A38X_MARVELL_BOARD_ID_BASE + 2)
#define DB_AP_68XX_ID			(A38X_MARVELL_BOARD_ID_BASE + 3)
#define DB_GP_68XX_ID			(A38X_MARVELL_BOARD_ID_BASE + 4)
#define DB_BP_6821_ID			(A38X_MARVELL_BOARD_ID_BASE + 5)
#define DB_AMC_6820_ID			(A38X_MARVELL_BOARD_ID_BASE + 6)
#define A38X_MV_MAX_MARVELL_BOARD_ID	(A38X_MARVELL_BOARD_ID_BASE + 7)
#define A38X_MV_MARVELL_BOARD_NUM	(A38X_MV_MAX_MARVELL_BOARD_ID - \
					 A38X_MARVELL_BOARD_ID_BASE)

#define CUTOMER_BOARD_ID_BASE		A38X_CUSTOMER_BOARD_ID_BASE
#define CUSTOMER_BOARD_ID0		A38X_CUSTOMER_BOARD_ID0
#define CUSTOMER_BOARD_ID1		A38X_CUSTOMER_BOARD_ID1
#define MV_MAX_CUSTOMER_BOARD_ID	A38X_MV_MAX_CUSTOMER_BOARD_ID
#define MV_CUSTOMER_BOARD_NUM		A38X_MV_CUSTOMER_BOARD_NUM
#define MARVELL_BOARD_ID_BASE		A38X_MARVELL_BOARD_ID_BASE
#define MV_MAX_MARVELL_BOARD_ID		A38X_MV_MAX_MARVELL_BOARD_ID
#define MV_MARVELL_BOARD_NUM		A38X_MV_MARVELL_BOARD_NUM
#define MV_DEFAULT_BOARD_ID		DB_68XX_ID
#define MV_DEFAULT_DEVICE_ID		MV_6811

#define MV_INVALID_BOARD_ID		0xffffffff

/* device revesion */
#define DEV_VERSION_ID_REG		0x1823c
#define REVISON_ID_OFFS			8
#define REVISON_ID_MASK			0xf00

/* A38x revisions */
#define MV_88F68XX_Z1_ID		0x0
#define MV_88F68XX_A0_ID		0x4
#define MV_88F68XX_B0_ID		0xa

#define MPP_CONTROL_REG(id)		(0x18000 + (id * 4))
#define GPP_DATA_OUT_REG(grp)		(MV_GPP_REGS_BASE(grp) + 0x00)
#define GPP_DATA_OUT_EN_REG(grp)	(MV_GPP_REGS_BASE(grp) + 0x04)
#define GPP_DATA_IN_REG(grp)		(MV_GPP_REGS_BASE(grp) + 0x10)
#define MV_GPP_REGS_BASE(unit)		(0x18100 + ((unit) * 0x40))

#define MPP_REG_NUM(GPIO_NUM)		(GPIO_NUM / 8)
#define MPP_MASK(GPIO_NUM)		(0xf << 4 * (GPIO_NUM - \
					(MPP_REG_NUM(GPIO_NUM) * 8)));
#define GPP_REG_NUM(GPIO_NUM)		(GPIO_NUM / 32)
#define GPP_MASK(GPIO_NUM)		(1 << GPIO_NUM % 32)

/* device ID */
/* Armada 38x Family */
#define MV_6810_DEV_ID		0x6810
#define MV_6811_DEV_ID		0x6811
#define MV_6820_DEV_ID		0x6820
#define MV_6828_DEV_ID		0x6828

enum {
	MV_6810,
	MV_6820,
	MV_6811,
	MV_6828,
};

#define MV_6820_INDEX			0
#define MV_6810_INDEX			1
#define MV_6811_INDEX			2
#define MV_6828_INDEX			3

#define MAX_DEV_ID_NUM			4

#define MV_6820_INDEX			0
#define MV_6810_INDEX			1
#define MV_6811_INDEX			2
#define MV_6828_INDEX			3

enum unit_id {
	PEX_UNIT_ID,
	ETH_GIG_UNIT_ID,
	USB3H_UNIT_ID,
	USB3D_UNIT_ID,
	SATA_UNIT_ID,
	QSGMII_UNIT_ID,
	XAUI_UNIT_ID,
	RXAUI_UNIT_ID,
	MAX_UNITS_ID
};

struct board_wakeup_gpio {
	u32 board_id;
	int gpio_num;
};

enum suspend_wakeup_status {
	SUSPEND_WAKEUP_DISABLED,
	SUSPEND_WAKEUP_ENABLED,
	SUSPEND_WAKEUP_ENABLED_GPIO_DETECTED,
};

/*
 * GPIO status indication for Suspend Wakeup:
 * If suspend to RAM is supported and GPIO inidcation is implemented,
 * set the gpio number
 * If suspend to RAM is supported but GPIO indication is not implemented
 * set '-2'
 * If suspend to RAM is not supported set '-1'
 */
#ifdef CONFIG_CUSTOMER_BOARD_SUPPORT
#define MV_BOARD_WAKEUP_GPIO_INFO {		\
	{A38X_CUSTOMER_BOARD_ID0,	-1 },	\
	{A38X_CUSTOMER_BOARD_ID0,	-1 },	\
};

#else

#define MV_BOARD_WAKEUP_GPIO_INFO {	\
	{RD_NAS_68XX_ID, -2 },		\
	{DB_68XX_ID,	 -1 },		\
	{RD_AP_68XX_ID,	 -2 },		\
	{DB_AP_68XX_ID,	 -2 },		\
	{DB_GP_68XX_ID,	 -2 },		\
	{DB_BP_6821_ID,	 -2 },		\
	{DB_AMC_6820_ID, -2 },		\
};
#endif /* CONFIG_CUSTOMER_BOARD_SUPPORT */

u32 mv_board_tclk_get(void);
u32 mv_board_id_get(void);
u32 mv_board_id_index_get(u32 board_id);
u32 sys_env_unit_max_num_get(enum unit_id unit);
enum suspend_wakeup_status sys_env_suspend_wakeup_check(void);
u8 sys_env_device_rev_get(void);
u32 sys_env_device_id_get(void);
u16 sys_env_model_get(void);
struct dlb_config *sys_env_dlb_config_ptr_get(void);
u32 sys_env_get_cs_ena_from_reg(void);

#endif /* _SYS_ENV_LIB_H */
