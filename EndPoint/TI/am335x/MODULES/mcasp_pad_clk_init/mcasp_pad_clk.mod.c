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
	{ 0xb077e70a, __VMLINUX_SYMBOL_STR(clk_unprepare) },
	{ 0x815588a6, __VMLINUX_SYMBOL_STR(clk_enable) },
	{ 0x55e595e5, __VMLINUX_SYMBOL_STR(__pm_runtime_disable) },
	{ 0xb6e6d99d, __VMLINUX_SYMBOL_STR(clk_disable) },
	{ 0x3cced638, __VMLINUX_SYMBOL_STR(__pm_runtime_resume) },
	{ 0x55577450, __VMLINUX_SYMBOL_STR(__platform_driver_register) },
	{ 0x27e1a049, __VMLINUX_SYMBOL_STR(printk) },
	{ 0xdf7308a4, __VMLINUX_SYMBOL_STR(of_property_read_string) },
	{ 0xabed5b29, __VMLINUX_SYMBOL_STR(pm_runtime_enable) },
	{ 0x7c9a7371, __VMLINUX_SYMBOL_STR(clk_prepare) },
	{ 0x69623d59, __VMLINUX_SYMBOL_STR(devm_clk_get) },
	{ 0x8e0e99d3, __VMLINUX_SYMBOL_STR(platform_driver_unregister) },
	{ 0x2597fa90, __VMLINUX_SYMBOL_STR(devm_kmalloc) },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=";

MODULE_ALIAS("of:N*T*Cti,am33xx-mcasp-audio");
MODULE_ALIAS("of:N*T*Cti,am33xx-mcasp-audioC*");
MODULE_ALIAS("of:N*T*Cti,am33xx-mcasp-audio");
MODULE_ALIAS("of:N*T*Cti,am33xx-mcasp-audioC*");

MODULE_INFO(srcversion, "C9C5FF7904C358A6993A7BC");
