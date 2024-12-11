#ifndef __CONFIGS_WIRENBOARD_COMMON_H
#define __CONFIGS_WIRENBOARD_COMMON_H

#define CONFIG_SYS_MMC_IMG_LOAD_PART 2
#define SYS_MMC_DATA_PART     6

#define WIRENBOARD_COMMON_ENV_SETTINGS \
	"loadaddr=" KERNEL_ADDR_R "\0" \
	"fdt_extra_overlay_size="__stringify(CONFIG_OF_OVERLAY_SIZE_BLKS)"\0" \
	"fdt_extra_overlay_offset="__stringify(CONFIG_OF_OVERLAY_OFFSET_BLKS)"\0" \
	"fdt_dir=/boot/dtbs\0" \
	"fdt_overlays_dir=/boot/overlays\0" \
	"mmcdev="__stringify(CONFIG_SYS_MMC_ENV_DEV)"\0" \
	"kernel_mmcdev=0\0" \
	"mmcpart=" __stringify(CONFIG_SYS_MMC_IMG_LOAD_PART) "\0" \
	"mmcdatapart=" __stringify(SYS_MMC_DATA_PART) "\0" \
	"setbootargs=setenv bootargs console=${console} ${optargs}\0 " \
	"mmcargs=setenv optargs ${optargs} " \
		"root=/dev/mmcblk${kernel_mmcdev}p${mmcpart} rootwait ro; " \
		"run setbootargs\0" \
	"loadbootenv=load mmc ${mmcdev}:${mmcpart} ${loadaddr} /boot/uEnv.txt\0" \
	"bootenv=echo Importing environment from ${loadaddr}...; " \
		"env import -t ${loadaddr} ${filesize};" \
		"echo Checking if uenvcmd is set ...;" \
		"if test -n \"${uenvcmd}\"; then " \
			"echo Running uenvcmd ...;" \
			"run uenvcmd;" \
		"fi;\0" \
	"loadzimage=load mmc ${mmcdev}:${mmcpart} ${kernel_addr_r} " WIRENBOARD_IMAGE_PATH "\0" \
	"loadmainfdt=" \
		"if test -n \"${fdt_file_override}\" && "\
			"echo Loading fdt_file_override ${fdt_file_override} && "\
			"load mmc ${mmcdev}:${mmcpart} ${fdt_addr_r} ${fdt_file_override}; "\
		"then true; else " \
			"if test -n \"${fdt_name_factory}\" && "\
				"echo Loading factory fdt ${fdt_name_factory} && "\
				"load mmc ${mmcdev}:${mmcpart} ${fdt_addr_r} ${fdt_dir}/${fdt_name_factory}.dtb;"\
			"then true; else " \
				"echo Loading fdt_file ${fdt_file};" \
				"load mmc ${mmcdev}:${mmcpart} ${fdt_addr_r} ${fdt_file};" \
			"fi;" \
		"fi;\0" \
	"addoverlay=if ${fdt_load_continue} && load mmc ${mmcdev}:${mmcpart} ${fdtoverlay_addr_r} ${fdt_overlays_dir}/${ovl}.dtbo; then " \
				"echo Applying DT overlay ${ovl};" \
				"if fdt apply ${fdtoverlay_addr_r}; then true; else " \
					"echo Failed to apply overlay ${ovl};" \
					"setenv fdt_load_continue false;" \
				"fi;" \
			"fi\0" \
	"loadfdt=mmc read ${fdtoverlay_addr_r} ${fdt_extra_overlay_offset} ${fdt_extra_overlay_size};" \
		"if fdt addr ${fdtoverlay_addr_r}; then " \
			"setenv fdt_extra_overlay_addr ${fdtoverlay_addr_r};" \
			"setexpr fdt_extra_overlay_size_bytes ${fdt_extra_overlay_size} * 0x200;" \
			"setexpr fdtoverlay_addr_r ${fdtoverlay_addr_r} + ${fdt_extra_overlay_size_bytes};" \
			"echo Reading overlays list from eMMC;" \
			"fdt get value fdt_overlays /fragment/__overlay__ overlays;" \
			"echo Reading factory fdt name from eMMC;" \
			"fdt get value fdt_name_factory /fragment/__overlay__ factory-fdt;" \
		"fi;" \
		"run loadmainfdt;" \
		"fdt addr ${fdt_addr_r} ${fdt_maxsize};" \
		"setenv fdt_load_continue true;" \
		"for ovl in ${fdt_overlays}; do " \
			"run addoverlay;" \
		"done;" \
		"if ${fdt_load_continue} && test -n \"${fdt_extra_overlay_addr}\"; then " \
			"echo Applying extra overlay;" \
			"if fdt apply ${fdt_extra_overlay_addr}; then true; else " \
				"echo Failed to apply extra overlay;" \
				"setenv fdt_load_continue false;" \
			"fi;" \
		"fi;" \
		"if ${fdt_load_continue}; then true; else " \
			"echo Some overlays are not applied, booting in failsafe;" \
			"run loadmainfdt;" \
			"fdt addr ${fdt_addr_r} ${fdt_maxsize};" \
			"fdt set / wirenboard-boot-failsafe;" \
		"fi;\0" \
	"mmcboot=echo Booting from mmc ...; " \
		"run mmcargs; " \
		WIRENBOARD_BOOT_CMD "\0" \
	"distro_bootcmd=" \
		WIRENBOARD_PREBOOT_CMD \
		"mmc dev ${mmcdev};" \
		"if mmc rescan; then " \
			"echo eMMC found on device ${mmcdev} ;" \
			"if run loadbootenv; then " \
				"run bootenv; " \
			"fi;" \
			"echo Running default loadzimage ...;" \
			"if run loadzimage; then " \
				"run loadfdt; " \
				"run mmcboot; " \
			"fi;" \
		"fi\0"
#endif
