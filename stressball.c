#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/miscdevice.h>

#define DEVICE_NAME "stressball0"

MODULE_DESCRIPTION("stressball");
MODULE_AUTHOR("Guillaume Dore, dore_g");
MODULE_LICENSE("GPL");

static struct miscdevice stressball_misc;

static long stressball_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
	return 0;
}

static int stressball_open(struct inode *node, struct file *file)
{
	return 0;
}

static int stressball_release(struct inode *node, struct file *file)
{
	return 0;
}


static struct file_operations stressball_fops = {
	.owner = THIS_MODULE,
	.unlocked_ioctl = stressball_ioctl,
	.open = stressball_open,
	.release = stressball_release,
};

static int __init stressball_init(void)
{
	int ret;
	stressball_misc.minor = MISC_DYNAMIC_MINOR;
	stressball_misc.name = DEVICE_NAME;
	stressball_misc.fops = &stressball_fops;	
	ret = misc_register(&stressball_misc);

	return ret;
}

static void __exit stressball_exit(void)
{
  misc_deregister(&stressball_misc);
}

module_init(stressball_init);
module_exit(stressball_exit);
