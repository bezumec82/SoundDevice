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
	{ 0xa3faa638, __VMLINUX_SYMBOL_STR(cdev_del) },
	{ 0xa84f36df, __VMLINUX_SYMBOL_STR(cdev_init) },
	{ 0x2dde9e03, __VMLINUX_SYMBOL_STR(mem_map) },
	{ 0x1ccb6712, __VMLINUX_SYMBOL_STR(__mutex_do_init) },
	{ 0x7210040e, __VMLINUX_SYMBOL_STR(rpmsg_send) },
	{ 0x4604efc8, __VMLINUX_SYMBOL_STR(__rt_mutex_init) },
	{ 0x30a9673, __VMLINUX_SYMBOL_STR(device_destroy) },
	{ 0x55757b57, __VMLINUX_SYMBOL_STR(_mutex_unlock) },
	{ 0x987c11c7, __VMLINUX_SYMBOL_STR(__pv_phys_pfn_offset) },
	{ 0x28cc25db, __VMLINUX_SYMBOL_STR(arm_copy_from_user) },
	{ 0x7485e15e, __VMLINUX_SYMBOL_STR(unregister_chrdev_region) },
	{ 0x91715312, __VMLINUX_SYMBOL_STR(sprintf) },
	{ 0xf4fa543b, __VMLINUX_SYMBOL_STR(arm_copy_to_user) },
	{ 0xf10103c0, __VMLINUX_SYMBOL_STR(__init_waitqueue_head) },
	{ 0xfa2a45e, __VMLINUX_SYMBOL_STR(__memzero) },
	{ 0x27e1a049, __VMLINUX_SYMBOL_STR(printk) },
	{ 0xb6c59a74, __VMLINUX_SYMBOL_STR(unregister_rpmsg_driver) },
	{ 0x771416fc, __VMLINUX_SYMBOL_STR(device_create) },
	{ 0x8e865d3c, __VMLINUX_SYMBOL_STR(arm_delay_ops) },
	{ 0x5150f4e0, __VMLINUX_SYMBOL_STR(__register_rpmsg_driver) },
	{ 0x622598b1, __VMLINUX_SYMBOL_STR(init_wait_entry) },
	{ 0x5262624d, __VMLINUX_SYMBOL_STR(cdev_add) },
	{ 0x93fca811, __VMLINUX_SYMBOL_STR(__get_free_pages) },
	{ 0xd62c833f, __VMLINUX_SYMBOL_STR(schedule_timeout) },
	{ 0x4302d0eb, __VMLINUX_SYMBOL_STR(free_pages) },
	{ 0x3e912bcc, __VMLINUX_SYMBOL_STR(__wake_up) },
	{ 0xc9c69841, __VMLINUX_SYMBOL_STR(prepare_to_wait_event) },
	{ 0x9d669763, __VMLINUX_SYMBOL_STR(memcpy) },
	{ 0xbe409e19, __VMLINUX_SYMBOL_STR(class_destroy) },
	{ 0x96285bdc, __VMLINUX_SYMBOL_STR(finish_wait) },
	{ 0x4298b775, __VMLINUX_SYMBOL_STR(v7_flush_kern_cache_all) },
	{ 0x609035c0, __VMLINUX_SYMBOL_STR(devm_kmalloc) },
	{ 0x43e29cfb, __VMLINUX_SYMBOL_STR(__class_create) },
	{ 0x33112965, __VMLINUX_SYMBOL_STR(flush_dcache_page) },
	{ 0x29537c9e, __VMLINUX_SYMBOL_STR(alloc_chrdev_region) },
	{ 0x3fde940, __VMLINUX_SYMBOL_STR(_mutex_lock_interruptible) },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=";


MODULE_INFO(srcversion, "31FA8EA15AC5D0C38EF9581");
