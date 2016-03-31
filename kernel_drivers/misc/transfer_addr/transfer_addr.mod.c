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
	{ 0x91715312, "sprintf" },
	{ 0xb4390f9a, "mcount" },
	{ 0xe914e41e, "strcpy" },
	{ 0x3744cf36, "vmalloc_to_pfn" },
	{ 0xd6ee688f, "vmalloc" },
	{ 0x93fca811, "__get_free_pages" },
	{ 0x7e1805b7, "create_proc_entry" },
	{ 0x9331365d, "proc_mkdir" },
	{ 0x59528ec6, "remove_proc_entry" },
	{ 0x4302d0eb, "free_pages" },
	{ 0x906ae829, "mem_map" },
	{ 0x50eedeb8, "printk" },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=";


MODULE_INFO(srcversion, "A9E38E3CD84B7E95919E274");
