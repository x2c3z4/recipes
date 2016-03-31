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
	{ 0x41086e, "module_layout" },
	{ 0x4302d0eb, "free_pages" },
	{ 0x93fca811, "__get_free_pages" },
	{ 0x70b2af54, "create_proc_entry" },
	{ 0xf6d62bd3, "dev_set_drvdata" },
	{ 0xbcec4698, "device_create_file" },
	{ 0x173b7186, "device_create" },
	{ 0x8fe748ab, "__class_create" },
	{ 0x40c497a1, "cdev_add" },
	{ 0xf28b622f, "cdev_init" },
	{ 0x884f145, "kmem_cache_alloc_trace" },
	{ 0xc288f8ce, "malloc_sizes" },
	{ 0x29537c9e, "alloc_chrdev_region" },
	{ 0x2da418b5, "copy_to_user" },
	{ 0xf2a644fb, "copy_from_user" },
	{ 0x701d0ebd, "snprintf" },
	{ 0x93b38b05, "dev_get_drvdata" },
	{ 0x3f1899f1, "up" },
	{ 0xfc4f55f3, "down_interruptible" },
	{ 0xb742fd7, "simple_strtol" },
	{ 0x7485e15e, "unregister_chrdev_region" },
	{ 0x37a0cba, "kfree" },
	{ 0xbef9fe65, "cdev_del" },
	{ 0x85440935, "class_destroy" },
	{ 0x96e2ed60, "device_destroy" },
	{ 0x8d8aacb9, "remove_proc_entry" },
	{ 0xb72397d5, "printk" },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=";


MODULE_INFO(srcversion, "0BD1A7D52E54CCCC5FFBFEE");

static const struct rheldata _rheldata __used
__attribute__((section(".rheldata"))) = {
	.rhel_major = 6,
	.rhel_minor = 4,
};
