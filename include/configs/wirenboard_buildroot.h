#define SYS_MMC_PART_SLOT_A 2
#define SYS_MMC_PART_SLOT_B 3


#define WIRENBOARD_COMMON_ENV_SETTINGS \
    "fdt_addr_r=0x4FA00000\0" \
    "scriptaddr=0x4FC00000\0" \
    "fdtoverlay_addr_r=0x4FE00000\0" \
	"fdt_dir=/boot/dtbs\0" \
	"fdt_extra_overlay_size="__stringify(CONFIG_OF_OVERLAY_SIZE_BLKS)"\0" \
	"fdt_extra_overlay_offset="__stringify(CONFIG_OF_OVERLAY_OFFSET_BLKS)"\0" \
    \
    "rauc_slot=A\0" \
    "SLOT_A_FAILED=0\0" \
    "SLOT_B_FAILED=0\0" \
    "boot_in_progress=0\0" \
    "slot_a_part=" __stringify(CONFIG_SYS_MMC_ENV_DEV) ":" __stringify(SYS_MMC_PART_SLOT_A) "\0" \
    "slot_b_part=" __stringify(CONFIG_SYS_MMC_ENV_DEV) ":" __stringify(SYS_MMC_PART_SLOT_B) "\0" \
    "slot_a_rootfs=/dev/mmcblk0p" __stringify(SYS_MMC_PART_SLOT_A) "\0" \
    "slot_b_rootfs=/dev/mmcblk0p" __stringify(SYS_MMC_PART_SLOT_B) "\0" \
    "bootargs_base=console=ttyS0,115200 ro rootfstype=squashfs rootwait net.ifnames=0 init=/linuxrc\0" \
    "kernel_addr_r=" KERNEL_ADDR_R "\0" \
    "switch_slot=" \
       "if test \"${rauc_slot}\" = \"A\"; then " \
         "echo Switching to slot B...; " \
         "setenv rauc_slot B; " \
       "else " \
         "echo Switching to slot A...; " \
         "setenv rauc_slot A; " \
       "fi; " \
       "saveenv\0" \
    "check_both_failed=" \
       "if test \"${SLOT_A_FAILED}\" = \"1\" -a \"${SLOT_B_FAILED}\" = \"1\"; then " \
         "echo Both slots failed, resetting flags...; " \
         "setenv SLOT_A_FAILED 0; " \
         "setenv SLOT_B_FAILED 0; " \
         "setenv rauc_slot A; " \
         "saveenv; " \
       "fi;\0" \
    "mark_current_slot_failed=" \
       "if test \"${rauc_slot}\" = \"A\"; then " \
         "echo Marking slot A as failed...; " \
         "setenv SLOT_A_FAILED 1; " \
       "else " \
         "echo Marking slot B as failed...; " \
         "setenv SLOT_B_FAILED 1; " \
       "fi; " \
       "saveenv; \0" \
    "check_current_slot_failed=" \
       "if test \"${rauc_slot}\" = \"A\" -a \"${SLOT_A_FAILED}\" = \"1\"; then " \
         "echo Current slot A failed, switching...; " \
         "run switch_slot; " \
       "fi; " \
       "if test \"${rauc_slot}\" = \"B\" -a \"${SLOT_B_FAILED}\" = \"1\"; then " \
         "echo Current slot B failed, switching...; " \
         "run switch_slot; " \
       "fi;\0" \
    "choose_rootfs=" \
       "if test \"${rauc_slot}\" = \"A\"; then " \
         "setenv root_part ${slot_a_part}; " \
         "setenv bootargs ${bootargs_base}\" root=${slot_a_rootfs}\"; " \
       "else " \
         "setenv root_part ${slot_b_part}; " \
         "setenv bootargs ${bootargs_base}\" root=${slot_b_rootfs}\"; " \
       "fi;\0" \
    "fail_fallback=" \
       "echo Fallback load failed!; " \
       "run mark_current_slot_failed; " \
       "run check_both_failed; " \
       "run switch_slot;\0" \
    \
	"loadfdt=mmc read ${fdtoverlay_addr_r} ${fdt_extra_overlay_offset} ${fdt_extra_overlay_size};" \
		"if fdt addr ${fdtoverlay_addr_r}; then " \
			"echo Reading factory fdt name from eMMC;" \
			"fdt get value fdt_name_factory /fragment/__overlay__ factory-fdt;" \
        "else " \
			"echo Failed to read factory fdt name from eMMC;" \
		"fi;" \
		"run loadmainfdt;" \
		"fdt addr ${fdt_addr_r} ${fdt_maxsize};" \
		"\0" \
	"loadmainfdt=" \
        "if test -n \"${fdt_name_factory}\" && "\
            "echo Loading factory fdt ${fdt_name_factory} && "\
            "sqfsload mmc ${root_part} ${fdt_addr_r} ${fdt_dir}/${fdt_name_factory}.dtb;"\
        "then true; else " \
            "echo Loading fdt_file ${fdt_file};" \
            "sqfsload mmc ${root_part} ${fdt_addr_r} ${fdt_file};" \
		"fi;\0" \
    "try_boot_scr=" \
       "echo Trying to load boot.scr from slot ${rauc_slot}...; " \
       "if sqfsload mmc ${root_part} ${scriptaddr} /boot/boot.scr; then " \
         "echo boot.scr found, executing script...; " \
         "if source ${scriptaddr}; then " \
           "echo boot.scr returned success but did not run kernel, fallback to direct kernel loading; " \
           "setenv boot_in_progress 1; saveenv; " \
           "run loadfdt || run fail_fallback; " \
           "if sqfsload mmc ${root_part} ${kernel_addr_r} " WIRENBOARD_IMAGE_PATH "; then " \
             WIRENBOARD_BOOT_CMD " || run fail_fallback; " \
           "else " \
             "echo Failed to load Kernel (fallback); " \
             "run fail_fallback; " \
           "fi; " \
         "else " \
           "echo boot.scr execution failed!; " \
           "run mark_current_slot_failed; " \
           "run check_both_failed; " \
           "run switch_slot; " \
         "fi; " \
       "else " \
         "echo No boot.scr found, fallback to loading default Kernel and DTB; " \
         "setenv boot_in_progress 1; saveenv; " \
         "run loadfdt || run fail_fallback; " \
         "if sqfsload mmc ${root_part} ${kernel_addr_r} " WIRENBOARD_IMAGE_PATH "; then " \
           WIRENBOARD_BOOT_CMD " || run fail_fallback; " \
         "else " \
           "echo Failed to load Kernel; " \
           "run fail_fallback; " \
         "fi; " \
       "fi;\0" \
    "distro_bootcmd=" \
	   "mmc dev ${root_part};" \
	   "mmc rescan;" \
       "if test \"${boot_in_progress}\" = \"1\"; then " \
         "echo Previous attempt was not confirmed, marking current slot as failed...; " \
         "run mark_current_slot_failed; " \
       "fi; " \
       "run check_both_failed; " \
       "run check_current_slot_failed; " \
       "run choose_rootfs; " \
       "run try_boot_scr;\0"

