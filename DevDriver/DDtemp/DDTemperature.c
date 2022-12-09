
/*############################################################################
#   @brief -> character device driver for temperature sensor xxxx
#   
#
#
#
############################################################################*/
//#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
//#include <linux/slab.h>
#include <linux/fs.h>
#include <linux/errno.h>
#include <linux/types.h>
//#include <linux/proc_fs.h>
#include <linux/fcntl.h>
//#include <asm/switch_to.h>
//#include <linux/uaccess.h>


static struct cdev c_dev;  // Character device structure

int memory_major = 60;     //major number of the device

static char temp_open(struct inode* inode, struct file *file);
static char temp_close();
static char temp_read();
static int temp_init();
static void temp_exit();

static struct file_operations temp_fops = 
 {
    //.owner = THIS_MODULE,
    //.write = _write,
    .read = temp_read,
    .release = temp_close,
    .open = temp_open,
    //.unlocked_ioctl = temp_ioctl,
 };



static char temp_open(struct inode* inode, struct file *file)
{

}

static char temp_close()
{

}

static char temp_read()
{

}

static int __init temp_init()
{

}

static void __exit temp_exit()
{

}