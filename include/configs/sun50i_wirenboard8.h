#ifndef __CONFIGS_SUN8I_WIRENBOARD8_H
#define __CONFIGS_SUN8I_WIRENBOARD8_H

#include <configs/sunxi-common.h>

/* add special check for FEL boot script
 * to use single U-boot image both on devices and in initenv*/
#define WIRENBOARD_PREBOOT_CMD "run bootcmd_fel;"

#define WIRENBOARD_IMAGE_PATH "/boot/Image.gz"
#define WIRENBOARD_BOOT_CMD "booti ${kernel_addr_r} - ${fdtaddr};"

/* Device capabilities DT overlay
 * 2016 (0x730) is 32 (0x20) blocks before first partition (2048)
 */
#define CONFIG_OF_OVERLAY_OFFSET_BLKS    0x7e0
#define CONFIG_OF_OVERLAY_SIZE_BLKS      0x20

#include <configs/wirenboard_common.h>

#ifdef CFG_EXTRA_ENV_SETTINGS
#undef CFG_EXTRA_ENV_SETTINGS
#endif

#define CFG_EXTRA_ENV_SETTINGS \
	MEM_LAYOUT_ENV_SETTINGS \
	MEM_LAYOUT_ENV_EXTRA_SETTINGS \
	"console=ttyS0,115200\0" \
	"fdt_file=/boot/dtbs/allwinner/sun50i-h616-wirenboard8xx.dtb\0" \
	"fdt_maxsize=0x20000\0" \
	"sd_fw_dev=\"0:1\"\0" \
	"read_button=if gpio qi PH10; then false; else true; fi\0" \
	/* Set PWM1 clock prescaler, period and duty cycle. pinctrl is handled by board code */ \
	/* 3kHz, 50% square wave */ \
	"buzzer_init=" \
        "sspi 0:0 112 00D0000000000000000000000BB8;" \
        "sspi 0:0 112 00D1000000000000000000000032; \0" \
	"buzzer_on=sspi 0:0 112 00D2000000000000000000000001;\0" \
	"buzzer_off=sspi 0:0 112 00D2000000000000000000000000;\0" \
	"bootcmd_fel=" \
		"if test -n ${fel_booted} && test -n ${fel_scriptaddr}; then " \
			"echo '(FEL boot)'; " \
			"source ${fel_scriptaddr}; " \
		"fi\0" \
    WIRENBOARD_COMMON_ENV_SETTINGS

#endif

