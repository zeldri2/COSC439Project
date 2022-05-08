#include <linux/build-salt.h>
#include <linux/module.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

BUILD_SALT;

MODULE_INFO(vermagic, VERMAGIC_STRING);
MODULE_INFO(name, KBUILD_MODNAME);

__visible struct module __this_module
__section(.gnu.linkonce.this_module) = {
	.name = KBUILD_MODNAME,
	.init = init_module,
#ifdef CONFIG_MODULE_UNLOAD
	.exit = cleanup_module,
#endif
	.arch = MODULE_ARCH_INIT,
};

#ifdef CONFIG_RETPOLINE
MODULE_INFO(retpoline, "Y");
#endif

static const struct modversion_info ____versions[]
__used __section(__versions) = {
	{ 0xa4b86400, "module_layout" },
	{ 0x541fd03b, "__register_chrdev" },
	{ 0xc5850110, "printk" },
	{ 0xc3aaf0a9, "__put_user_1" },
	{ 0x4bb4cfba, "module_put" },
	{ 0xbdfb6dbb, "__fentry__" },
	{ 0x362ef408, "_copy_from_user" },
	{ 0x6a71f8d2, "try_module_get" },
};

MODULE_INFO(depends, "");


MODULE_INFO(srcversion, "457A02D8C1A4E38FE84AABC");
