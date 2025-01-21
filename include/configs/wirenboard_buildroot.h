#define BOOTSCR_MMC_PART 1
#define SYS_MMC_PART 2
#define OVERLAY_MMC_PART 4


#define WIRENBOARD_COMMON_ENV_SETTINGS \
    "fdt_addr_r=0x4FA00000\0" \
    "bootscr_addr=0x4FC00000\0" \
    "fdtoverlay_addr_r=0x4FE00000\0" \
    \
    "root_part=" __stringify(CONFIG_SYS_MMC_ENV_DEV) ":" __stringify(SYS_MMC_PART) "\0" \
    "bootargs=console=ttyS0,115200 ro rootfstype=squashfs rootwait net.ifnames=0 init=/linuxrc root=/dev/mmcblk0p" __stringify(SYS_MMC_PART) "\0" \
    "kernel_addr_r=" KERNEL_ADDR_R "\0" \
    "load_default_dt=" \
        "echo Loading default fdt_file ${fdt_file};" \
        "sqfsload mmc ${root_part} ${fdt_addr_r} ${fdt_file};" \
        "\0" \
    \
	"load_fdt_with_overlays=mmc read ${fdtoverlay_addr_r} " __stringify(CONFIG_OF_OVERLAY_OFFSET_BLKS) " " __stringify(CONFIG_OF_OVERLAY_SIZE_BLKS) "; " \
		"if fdt addr ${fdtoverlay_addr_r}; then " \
			"echo Reading factory fdt name from eMMC;" \
            "if fdt get value fdt_name_factory /fragment/__overlay__ factory-fdt; then " \
                "echo Loading factory fdt ${fdt_name_factory};" \
                "sqfsload mmc ${root_part} ${fdt_addr_r} /boot/dtbs/${fdt_name_factory}.dtb;" \
            "else " \
                "run load_default_dt; "\
            "fi; " \
        "else " \
            "echo Failed to read factory fdt name from eMMC;" \
            "run load_default_dt; "\
		"fi;" \
		"fdt addr ${fdt_addr_r} ${fdt_maxsize};" \
        "if fdt apply ${fdtoverlay_addr_r}; then true; else " \
            "echo Failed to apply extra overlay! No /proc/device-tree/wirenboard/ will be available;" \
        "fi\0" \
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
    "check_fw_reset=" \
        "if run check_button; then " \
            "run buzzer_init;" \
            "run buzzer_on;" \
            "sleep 0.1; " \
            "run buzzer_off;" \
            "echo Detected FW button press. If you want to reset the system to factory settings, keep holding the button for 5 seconds. The system will reset after the countdown.; " \
            "setenv fw_timeout 5; " \
            "run wait_for_button_countdown; " \
            "if run check_button; then " \
                "echo System reset confirmed. Initiating factory reset process...; " \
                "run buzzer_on;" \
                "sleep 0.5; " \
                "run buzzer_off;" \
                "echo Break Overlay FS...; " \
                "run breaking_ext4; " \
                "echo The Overlay FS is broken - it will be formatted the next time Linux boots.; " \
            "else " \
                "echo System reset not confirmed. Continuing normal system boot.; " \
            "fi; " \
        "fi;\0" \
    "breaking_ext4=" \
        "part start mmc " __stringify(CONFIG_SYS_MMC_ENV_DEV) " " __stringify(OVERLAY_MMC_PART)  " start_sector; " \
        "for offset in 0 32768 98304 163840 229376 294912 819200 1605632;  do "\
            "setexpr sector ${start_sector} + ${offset}; " \
            "mmc write 0x0 ${sector} 10; "\
        "done;\0" \
    "distro_bootcmd=" \
        "mmc dev ${root_part};" \
        "mmc rescan;" \
        "run check_fw_reset; " \
        "echo Trying to load boot.scr ...; " \
        "if fatload mmc " __stringify(CONFIG_SYS_MMC_ENV_DEV) ":" __stringify(BOOTSCR_MMC_PART)  " ${bootscr_addr} /boot.scr; then " \
            "echo boot.scr found, executing script...; " \
            "if source ${bootscr_addr}; then " \
                "echo boot.scr returned success but did not run kernel, fallback to direct kernel loading; " \
            "else " \
                "echo boot.scr execution failed!; " \
            "fi; " \
        "else " \
            "echo No boot.scr found, attempting default Kernel and DTB loading; "\
        "fi; " \
        "run load_fdt_with_overlays; " \
        "sqfsload mmc ${root_part} ${kernel_addr_r} " WIRENBOARD_IMAGE_PATH "; " \
        WIRENBOARD_BOOT_CMD "; " \
        "\0" \

