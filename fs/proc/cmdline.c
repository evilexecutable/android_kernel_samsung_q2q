// SPDX-License-Identifier: GPL-2.0
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <asm/setup.h>

static char new_command_line[COMMAND_LINE_SIZE];

static int cmdline_proc_show(struct seq_file *m, void *v)
{
	seq_puts(m, new_command_line);
	seq_putc(m, '\n');
	return 0;
}

static void patch_flag(char *cmd, const char *flag, const char *val)
{
	size_t flag_len, val_len;
	char *start, *end;

	start = strstr(cmd, flag);
	if (!start)
		return;

	flag_len = strlen(flag);
	val_len = strlen(val);
	end = start + flag_len + strcspn(start + flag_len, " ");
	memmove(start + flag_len + val_len, end, strlen(end) + 1);
	memcpy(start + flag_len, val, val_len);
}

static void patch_safetynet_flags(char *cmd)
{
	patch_flag(cmd, "androidboot.flash.locked=", "1");
	patch_flag(cmd, "androidboot.verifiedbootstate=", "green");
	patch_flag(cmd, "androidboot.veritymode=", "enforcing");
	patch_flag(cmd, "androidboot.vbmeta.device_state=", "locked");
	patch_flag(cmd, "androidboot.hdm_status=", "NONE");
}

static int __init proc_cmdline_init(void)
{
	strcpy(new_command_line, saved_command_line);

	/*
	 * Patch various flags from command line seen by userspace in order to
	 * pass SafetyNet checks.
	 */
	patch_safetynet_flags(new_command_line);

	proc_create_single("cmdline", 0, NULL, cmdline_proc_show);
	return 0;
}
fs_initcall(proc_cmdline_init);
