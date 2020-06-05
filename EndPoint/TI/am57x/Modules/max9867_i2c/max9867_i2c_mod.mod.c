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
	{ 0x1c3e8b28, __VMLINUX_SYMBOL_STR(module_layout) },
	{ 0xcea7839f, __VMLINUX_SYMBOL_STR(i2c_del_driver) },
	{ 0x33c3f94f, __VMLINUX_SYMBOL_STR(i2c_register_driver) },
	{ 0xb6d0fd36, __VMLINUX_SYMBOL_STR(regmap_read) },
	{ 0xe7fe1d58, __VMLINUX_SYMBOL_STR(__devm_regmap_init_i2c) },
	{ 0x609035c0, __VMLINUX_SYMBOL_STR(devm_kmalloc) },
	{ 0xaffcd336, __VMLINUX_SYMBOL_STR(regmap_write) },
	{ 0x8e865d3c, __VMLINUX_SYMBOL_STR(arm_delay_ops) },
	{ 0x9c444c83, __VMLINUX_SYMBOL_STR(regmap_update_bits_base) },
	{ 0x27e1a049, __VMLINUX_SYMBOL_STR(printk) },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=";

MODULE_ALIAS("of:N*T*Cmaxim,max9867");
MODULE_ALIAS("of:N*T*Cmaxim,max9867C*");
MODULE_ALIAS("i2c:max9867");

MODULE_INFO(srcversion, "B47E32DCE16D51894056F64");
