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
	{ 0x9a68e9f, __VMLINUX_SYMBOL_STR(cdev_del) },
	{ 0xfa78cb8b, __VMLINUX_SYMBOL_STR(cdev_init) },
	{ 0x12ff120b, __VMLINUX_SYMBOL_STR(mem_map) },
	{ 0x97255bdf, __VMLINUX_SYMBOL_STR(strlen) },
	{ 0x5754d04c, __VMLINUX_SYMBOL_STR(__mutex_do_init) },
	{ 0x160c48a1, __VMLINUX_SYMBOL_STR(rpmsg_send) },
	{ 0xe90c5b44, __VMLINUX_SYMBOL_STR(__rt_mutex_init) },
	{ 0x27e0eb0a, __VMLINUX_SYMBOL_STR(device_destroy) },
	{ 0xe21e730c, __VMLINUX_SYMBOL_STR(_mutex_unlock) },
	{ 0x987c11c7, __VMLINUX_SYMBOL_STR(__pv_phys_pfn_offset) },
	{ 0x28cc25db, __VMLINUX_SYMBOL_STR(arm_copy_from_user) },
	{ 0x7485e15e, __VMLINUX_SYMBOL_STR(unregister_chrdev_region) },
	{ 0x91715312, __VMLINUX_SYMBOL_STR(sprintf) },
	{ 0x18f0c02e, __VMLINUX_SYMBOL_STR(sysfs_remove_group) },
	{ 0xf4fa543b, __VMLINUX_SYMBOL_STR(arm_copy_to_user) },
	{ 0x18d37396, __VMLINUX_SYMBOL_STR(__init_waitqueue_head) },
	{ 0xfa2a45e, __VMLINUX_SYMBOL_STR(__memzero) },
	{ 0x27e1a049, __VMLINUX_SYMBOL_STR(printk) },
	{ 0x72b09c60, __VMLINUX_SYMBOL_STR(sysfs_create_group) },
	{ 0x705aad01, __VMLINUX_SYMBOL_STR(unregister_rpmsg_driver) },
	{ 0x9cdf3816, __VMLINUX_SYMBOL_STR(device_create) },
	{ 0x8e865d3c, __VMLINUX_SYMBOL_STR(arm_delay_ops) },
	{ 0xbe03ccda, __VMLINUX_SYMBOL_STR(__register_rpmsg_driver) },
	{ 0x622598b1, __VMLINUX_SYMBOL_STR(init_wait_entry) },
	{ 0x95e33ef9, __VMLINUX_SYMBOL_STR(cdev_add) },
	{ 0x93fca811, __VMLINUX_SYMBOL_STR(__get_free_pages) },
	{ 0xd62c833f, __VMLINUX_SYMBOL_STR(schedule_timeout) },
	{ 0x94eea794, __VMLINUX_SYMBOL_STR(getnstimeofday64) },
	{ 0x4302d0eb, __VMLINUX_SYMBOL_STR(free_pages) },
	{ 0xf2b23d4f, __VMLINUX_SYMBOL_STR(__wake_up) },
	{ 0x7c73ebf9, __VMLINUX_SYMBOL_STR(prepare_to_wait_event) },
	{ 0x9d669763, __VMLINUX_SYMBOL_STR(memcpy) },
	{ 0x39ed57e, __VMLINUX_SYMBOL_STR(class_destroy) },
	{ 0x5933a608, __VMLINUX_SYMBOL_STR(finish_wait) },
	{ 0x4298b775, __VMLINUX_SYMBOL_STR(v7_flush_kern_cache_all) },
	{ 0x2597fa90, __VMLINUX_SYMBOL_STR(devm_kmalloc) },
	{ 0x72b6f5a4, __VMLINUX_SYMBOL_STR(__class_create) },
	{ 0x2409d6c3, __VMLINUX_SYMBOL_STR(flush_dcache_page) },
	{ 0x29537c9e, __VMLINUX_SYMBOL_STR(alloc_chrdev_region) },
	{ 0xac229cf2, __VMLINUX_SYMBOL_STR(_mutex_lock_interruptible) },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=";


MODULE_INFO(srcversion, "A67DE5F4FDF91AC4BB137BA");
