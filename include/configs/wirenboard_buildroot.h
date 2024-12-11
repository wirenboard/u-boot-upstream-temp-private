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
        "else " \
            "if test \"${rauc_slot}\" = \"B\" -a \"${SLOT_B_FAILED}\" = \"1\"; then " \
                "echo Current slot B failed, switching...; " \
                "run switch_slot; " \
            "fi; " \
        "fi;\0" \
    "choose_rootfs=" \
        "if test \"${rauc_slot}\" = \"A\"; then " \
            "setenv root_part ${slot_a_part}; " \
            "setenv bootargs ${bootargs} ${bootargs_base}\" root=${slot_a_rootfs}\"; " \
        "else " \
            "setenv root_part ${slot_b_part}; " \
            "setenv bootargs ${bootargs} ${bootargs_base}\" root=${slot_b_rootfs}\"; " \
        "fi; " \
        "if test -n \"${factory_reset}\"; then " \
            "setenv bootargs ${bootargs} factory_reset=1; " \
        "fi;\0" \
    "retry_boot_on_other_slot=" \
        "echo Current load attempt failed. Trying again on the other slot.; " \
        "reset;\0" \
    "load_default_dt=" \
        "echo Loading default fdt_file ${fdt_file};" \
        "sqfsload mmc ${root_part} ${fdt_addr_r} ${fdt_file};" \
        "\0" \
	"load_fdt_with_overlays=mmc read ${fdtoverlay_addr_r} ${fdt_extra_overlay_offset} ${fdt_extra_overlay_size};" \
		"if fdt addr ${fdtoverlay_addr_r}; then " \
			"echo Reading factory fdt name from eMMC;" \
            "if fdt get value fdt_name_factory /fragment/__overlay__ factory-fdt; then " \
                "echo Loading factory fdt ${fdt_name_factory};" \
                "sqfsload mmc ${root_part} ${fdt_addr_r} ${fdt_dir}/${fdt_name_factory}.dtb;" \
            "else " \
                "run load_default_dt; "\
            "fi; " \
        "else " \
            "echo Failed to read factory fdt name from eMMC;" \
                "run load_default_dt; "\
		"fi;" \
		"fdt addr ${fdt_addr_r} ${fdt_maxsize};" \
		"\0" \
	"boot_linux=" \
        "setenv boot_in_progress 1; saveenv; " \
        "run load_fdt_with_overlays || run retry_boot_on_other_slot; " \
        "if sqfsload mmc ${root_part} ${kernel_addr_r} " WIRENBOARD_IMAGE_PATH "; then " \
            WIRENBOARD_BOOT_CMD " || run retry_boot_on_other_slot; " \
        "else " \
            "echo Failed to load Kernel (fallback); " \
            "run retry_boot_on_other_slot; " \
        "fi; \0" \
    "attempt_boot_script=" \
        "echo Trying to load boot.scr from slot ${rauc_slot}...; " \
        "if sqfsload mmc ${root_part} ${scriptaddr} /boot/boot.scr; then " \
            "echo boot.scr found, executing script...; " \
            "if source ${scriptaddr}; then " \
                "echo boot.scr returned success but did not run kernel, fallback to direct kernel loading; " \
                "run boot_linux; "\
            "else " \
                "echo boot.scr execution failed!; " \
                "run retry_boot_on_other_slot; " \
            "fi; " \
        "else " \
            "echo No boot.scr found, attempting default Kernel and DTB loading; "\
            "run boot_linux; "\
        "fi;\0" \
    \
    "check_button=if run read_button; then false; else true; fi\0" \
	"not_timeout=test 0x\\${fw_timeout} -gt 0\0" \
    "wait_for_button_countdown=" \
        "while run check_button && run not_timeout ; do " \
            "echo -n \".\"; " \
            "sleep 1 ; " \
            "setexpr fw_timeout \\${fw_timeout} - 1; " \
        "done;" \
        "echo ;" \
        "run not_timeout\0" \
	"beep=" \
		"run buzzer_init;" \
		"run buzzer_on;" \
		"sleep 0.1; " \
		"run buzzer_off;" \
		"\0" \
    "check_fw_reset=" \
        "if run check_button; then " \
			"run beep; " \
            "echo Detected FW button press. If you want to reset the system to factory settings, keep holding the button for 5 seconds. The system will reset after the countdown.; " \
            "setenv fw_timeout 5; " \
            "run wait_for_button_countdown; " \
            "if run check_button; then " \
                "echo System reset confirmed. Initiating factory reset process...; " \
                "run beep; " \
                "sleep 0.1; " \
                "run beep; " \
                "echo Setting factory reset flag for the kernel...; " \
                "setenv factory_reset 1; " \
                "saveenv; " \
                "echo Factory reset flag set. The system will reset partition 4 on the next boot.; " \
            "else " \
                "echo System reset not confirmed. Continuing normal system boot.; " \
            "fi; " \
        "fi;\0" \
    "distro_bootcmd=" \
        "mmc dev ${root_part};" \
        "mmc rescan;" \
        "run check_fw_reset; " \
        "if test \"${boot_in_progress}\" = \"1\"; then " \
            "echo Previous attempt was not confirmed, marking current slot as failed...; " \
            "run mark_current_slot_failed; " \
        "fi; " \
        "run check_both_failed; " \
        "run check_current_slot_failed; " \
        "run choose_rootfs; " \
        "run attempt_boot_script;\0"

