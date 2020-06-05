#include <linux/module.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

MODULE_INFO(vermagic, VERMAGIC_STRING);

__visible struct module __this_module
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
	{ 0x5066274b, __VMLINUX_SYMBOL_STR(module_layout) },
	{ 0xa48cf212, __VMLINUX_SYMBOL_STR(i2c_del_driver) },
	{ 0x10be9198, __VMLINUX_SYMBOL_STR(i2c_register_driver) },
	{ 0x85bf81d6, __VMLINUX_SYMBOL_STR(regmap_read) },
	{ 0x6355b4c9, __VMLINUX_SYMBOL_STR(__devm_regmap_init_i2c) },
	{ 0x2597fa90, __VMLINUX_SYMBOL_STR(devm_kmalloc) },
	{ 0x82d8eaa0, __VMLINUX_SYMBOL_STR(regmap_write) },
	{ 0x8e865d3c, __VMLINUX_SYMBOL_STR(arm_delay_ops) },
	{ 0x250b2123, __VMLINUX_SYMBOL_STR(regmap_update_bits_base) },
	{ 0x27e1a049, __VMLINUX_SYMBOL_STR(printk) },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=";

MODULE_ALIAS("of:N*T*Cmaxim,max9867");
MODULE_ALIAS("of:N*T*Cmaxim,max9867C*");
MODULE_ALIAS("i2c:max9867");

MODULE_INFO(srcversion, "9DB8222699B094DA3AD03E8");
