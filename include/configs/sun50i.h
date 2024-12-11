/* SPDX-License-Identifier: GPL-2.0+ */
/*
 * Placeholder wrapper to allow addressing Allwinner A64 (and later) sun50i
 * CPU based devices separately. Please do not add anything in here.
 */
#ifndef __CONFIG_H
#define __CONFIG_H

#ifdef CONFIG_TARGET_SUN50I_WIRENBOARD8
#include <configs/sun50i_wirenboard8.h>
#else
#include <configs/sunxi-common.h>
#endif

#endif /* __CONFIG_H */
