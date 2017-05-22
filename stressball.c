#include <asm/uaccess.h>
#include <linux/errno.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/usb.h>

#include "stressball.h"

#define DEVICE_NAME "stressball%d"
#define VENDOR_ID 0x1d34
#define PRODUCT_ID 0x0020

MODULE_DESCRIPTION("stressball");
MODULE_AUTHOR("Guillaume Dore, dore_g");
MODULE_LICENSE("GPL");

struct usb_stressball {
  struct usb_device *udev;
  int i1;
  int i2;
  int i3;
};

static struct usb_driver stressball_driver;

static struct usb_device_id id_table [] = {
  { USB_DEVICE(VENDOR_ID, PRODUCT_ID) },
  { },
};
MODULE_DEVICE_TABLE (usb, id_table);

static int get_stressball_infos(struct usb_stressball *str_dev)
{
  char buff[8];
  int ret = 0, l;

  printk("stressball: entered %s\n", __FUNCTION__);

  memset (buff, 0, 8);
  buff[7] = 0x02;

  ret = usb_control_msg(str_dev->udev, usb_sndctrlpipe(str_dev->udev, 0), 0x09, USB_TYPE_CLASS | USB_RECIP_INTERFACE, 0x200, 0x00, buff, 8, 2 * HZ);

  if (ret < 0) {
    printk (KERN_WARNING "%s: IN, ret = %d\n", __FUNCTION__, ret);
    return ret;
  }

  memset (buff, 0, 8);
  ret = usb_interrupt_msg (str_dev->udev, usb_rcvintpipe (str_dev->udev, 1), buff, 8, &l, 2 * HZ);
  if (ret < 0) {
    printk (KERN_WARNING "%s: usb_interrupt_msg(), ret = %d l= %d\n", __FUNCTION__, ret, l);
    return ret;
  }
  else
  {
    str_dev->i1 = buff[0];
    str_dev->i2 = buff[1];
    str_dev->i3 = buff[2];
  }

  return 0;
  return 0;
}

static ssize_t show_status(struct device *dev, struct device_attribute *attr, char *buff)
{
  struct usb_interface *intf = to_usb_interface(dev); 
  struct usb_stressball *str_dev = usb_get_intfdata(intf);

  printk("stressball: entered %s\n", __FUNCTION__);

  get_stressball_infos(str_dev);

  sprintf(buff, "Stressball: %d, %d, %d\n", str_dev->i1, str_dev->i2, str_dev->i3);

  return 0;
}

static DEVICE_ATTR(status, 0444, show_status, NULL);

static int stressball_open(struct inode *inode, struct file *file)
{
  struct usb_stressball *dev;
  struct usb_interface *interface;
  int minor = MINOR(inode->i_rdev);

  printk("stressball: entered %s\n", __FUNCTION__);
  interface = usb_find_interface (&stressball_driver, minor);
  if (!interface)
    return -ENODEV;

  dev = usb_get_intfdata (interface);
  if (dev == NULL) {
    printk (KERN_WARNING "%s: can't find device for minor %d\n", __FUNCTION__, minor);
    return -ENODEV;
  }

  file->private_data = (void *)dev;

  return 0;
}

static int stressball_release(struct inode *inode, struct file *file)
{
  printk("stressball: entered %s\n", __FUNCTION__);
  return 0;
}

static long stressball_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{

  struct usb_stressball *str_dev;
  str_dev = file->private_data;

  printk("stressball: entered %s\n", __FUNCTION__);
  if (!str_dev)
    return -ENODEV;

  switch (cmd)
  {
    case STRESSBALL_GET_STATUS:
      if (get_stressball_infos(str_dev) == 0)
      {
        printk("IOCTL TOUSSA TOUSSA\n");
      }
      break;
    default:
      printk("IOCTL TOUSSA TOUSSA\n");
      break;
  }
  return 0;
}

static struct file_operations stressball_fops = {
  .open = stressball_open,
  .release = stressball_release,
  .unlocked_ioctl = stressball_ioctl
};

static struct usb_class_driver stressball_class_driver = {
  .name = "usb/stressball%d",
  .fops = &stressball_fops,
  .minor_base = 0
};

static int stressball_probe(struct usb_interface *interface, const struct usb_device_id *id)
{
  struct usb_device *udev = interface_to_usbdev (interface);
  struct usb_stressball *str_dev;
  int ret;

  printk("stressball: entered %s\n", __FUNCTION__);
  ret = usb_register_dev(interface, &stressball_class_driver);
  str_dev = kmalloc (sizeof(struct usb_stressball), GFP_KERNEL);
  memset (str_dev, 0x00, sizeof (struct usb_stressball));
  str_dev->udev = usb_get_dev(udev);
  str_dev->i1 = str_dev->i2 = str_dev->i3 = 0;
  usb_set_intfdata(interface, str_dev);
  ret = device_create_file(&interface->dev, &dev_attr_status);
  return ret;
}

static void stressball_disconnect(struct usb_interface *interface)
{
  struct usb_stressball *str_dev;

  printk("stressball: entered %s\n", __FUNCTION__);
  str_dev = usb_get_intfdata (interface);
  usb_deregister_dev (interface, &stressball_class_driver);
  usb_set_intfdata (interface, NULL);

  device_remove_file(&interface->dev, &dev_attr_status);
  usb_put_dev(str_dev->udev);

  kfree(str_dev);

  return;
}

static struct usb_driver str_driver = {
  .name = "stressball",
  .probe = stressball_probe,
  .disconnect =  stressball_disconnect,
  .id_table = id_table,
};


static int __init stressball_init(void)
{

  int ret = usb_register(&str_driver);
  printk("stressball: entered %s\n", __FUNCTION__);
  return ret;
}

static void __exit stressball_exit(void)
{
  printk("stressball: entered %s\n", __FUNCTION__);
  usb_deregister(&str_driver);
}

module_init(stressball_init);
module_exit(stressball_exit);
