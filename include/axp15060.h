/* SPDX-License-Identifier: GPL-2.0+ */
/*
 * (C) Copyright 2023 Nikita Maslov <nikita.maslov@wirenboard.ru>
 *
 * X-Powers AXP15060 Power Management IC driver
 */

#define AXP15060_OUTPUT_CTRL1	0x10
#define AXP15060_OUTPUT_CTRL1_DCDC1_EN	(1 << 0)
#define AXP15060_OUTPUT_CTRL1_DCDC2_EN	(1 << 1)
#define AXP15060_OUTPUT_CTRL1_DCDC3_EN	(1 << 2)
#define AXP15060_OUTPUT_CTRL1_DCDC4_EN	(1 << 3)
#define AXP15060_OUTPUT_CTRL1_DCDC5_EN	(1 << 4)
#define AXP15060_OUTPUT_CTRL1_DCDC6_EN	(1 << 5)

#define AXP15060_OUTPUT_CTRL2	0x11
#define AXP15060_OUTPUT_CTRL2_ALDO1_EN	(1 << 0)
#define AXP15060_OUTPUT_CTRL2_ALDO2_EN	(1 << 1)
#define AXP15060_OUTPUT_CTRL2_ALDO3_EN	(1 << 2)

#define AXP15060_OUTPUT_CTRL3	0x12
#define AXP15060_OUTPUT_CTRL3_SWOUT_EN	(1 << 7)

#define AXP15060_DCDC1_CTRL	0x13
#define AXP15060_DCDC2_CTRL	0x14
#define AXP15060_DCDC3_CTRL	0x15
#define AXP15060_DCDC4_CTRL	0x16
#define AXP15060_DCDC5_CTRL	0x17
#define AXP15060_ALDO1_CTRL	0x19
#define AXP15060_ALDO2_CTRL	0x20
#define AXP15060_ALDO3_CTRL	0x21
#define AXP15060_ALDO4_CTRL	0x22

/* For axp_gpio.c */
#ifdef CONFIG_AXP15060_POWER
#define AXP_POWER_STATUS		0x00
#define AXP_POWER_STATUS_ALDO_IN		BIT(0)

// bullshit there, just don't turn on gpio
#define AXP_VBUS_IPSOUT			0x30
#define AXP_VBUS_IPSOUT_DRIVEBUS		(1 << 2)
#define AXP_MISC_CTRL			0x8f
#define AXP_MISC_CTRL_N_VBUSEN_FUNC		(1 << 4)
#define AXP_GPIO0_CTRL			0x90
#define AXP_GPIO1_CTRL			0x92
#define AXP_GPIO_CTRL_OUTPUT_LOW	0x00 /* Drive pin low */
#define AXP_GPIO_CTRL_OUTPUT_HIGH	0x01 /* Drive pin high */
#define AXP_GPIO_CTRL_INPUT		0x02 /* Input */
#define AXP_GPIO_STATE			0x94
#define AXP_GPIO_STATE_OFFSET		0
#endif
