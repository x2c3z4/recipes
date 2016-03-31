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
	{ 0x98397cc5, "module_layout" },
	{ 0xf0fdf6cb, "__stack_chk_fail" },
	{ 0x362ef408, "_copy_from_user" },
	{ 0x2f8794fc, "pid_task" },
	{ 0x5104d374, "find_vpid" },
	{ 0x91715312, "sprintf" },
	{ 0x50eedeb8, "printk" },
	{ 0x7e1805b7, "create_proc_entry" },
	{ 0x9331365d, "proc_mkdir" },
	{ 0x59528ec6, "remove_proc_entry" },
	{ 0xb4390f9a, "mcount" },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=";


MODULE_INFO(srcversion, "6B00B9B7548A9D026EA0994");
