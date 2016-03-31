#include <linux/module.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

MODULE_INFO(vermagic, VERMAGIC_STRING);

struct module __this_module
__attribute__((section(".gnu.linkonce.this_module"))) = {
	.name = KBUILD_MODNAME,
	.init = init_module,
#ifdef CONFIG_MODULE_UNLOAD
	.exit = cleanup_module,
#endif
	.arch = MODULE_ARCH_INIT,
};

static const struct modversion_info ____versions[]
__used
__attribute__((section("__versions"))) = {
	{ 0x111ca498, "module_layout" },
	{ 0x6b06fdce, "delayed_work_timer_fn" },
	{ 0xfd6293c2, "boot_tvec_bases" },
	{ 0x687085cf, "single_release" },
	{ 0x99f924a3, "seq_read" },
	{ 0x9961d9ff, "seq_lseek" },
	{ 0x83117f87, "misc_deregister" },
	{ 0x8c03d20c, "destroy_workqueue" },
	{ 0x42160169, "flush_workqueue" },
	{ 0xf141bf26, "cancel_delayed_work" },
	{ 0xaba107bd, "debugfs_remove" },
	{ 0x27dfde63, "queue_delayed_work" },
	{ 0x3bd1b1f6, "msecs_to_jiffies" },
	{ 0x828d9add, "debugfs_create_file" },
	{ 0xdcbfa60, "debugfs_create_dir" },
	{ 0x43a53735, "__alloc_workqueue_key" },
	{ 0xaadcb107, "misc_register" },
	{ 0xa7e94a70, "single_open" },
	{ 0x50eedeb8, "printk" },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=";

