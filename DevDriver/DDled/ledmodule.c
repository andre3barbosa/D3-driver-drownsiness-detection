#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/slab.h>                 
#include <linux/uaccess.h>             
#include <linux/ioctl.h>   
#include <linux/sched/signal.h> 
#include <linux/proc_fs.h> 
#include <linux/fcntl.h> 
#include <linux/types.h>
#include <linux/gpio.h> 
#include <linux/interrupt.h>
#include "utils.h"

MODULE_LICENSE("GPL");

#define DEVICE_NAME "led0"
#define CLASS_NAME "ledClass"

static dev_t ledDevice_majorminor;
static struct class *ledDevice_class = NULL;
static struct cdev c_dev;  // Character device structure

/* Raspberry GPIO */
static const int LedGpioPin = 21; 
struct GpioRegisters *s_pGpioRegisters;


static int __init led_device_init(void);
static void __exit led_device_exit(void);


/* Driver functions */
int led_device_open(struct inode* p_inode, struct file* p_file);
int led_device_close(struct inode *inode, struct file *file);
ssize_t led_device_read(struct file *filp, char __user *buf, size_t len, loff_t *off);
ssize_t led_device_write(struct file *filp, const char *buf, size_t len, loff_t *off);

/*
** This function will be called when we open the Device file
*/ 
int led_device_open(struct inode* p_inode, struct file *p_file){

    /* Print a msg with the function being called */
    pr_alert("%s: called\n",__FUNCTION__);
    
    /*Store the handle to the GPIO registers */
    p_file->private_data = (struct GpioRegisters *) s_pGpioRegisters;
    return 0;
	
}

/*
** This function will be called when we read the Device file
*/ 
ssize_t led_device_read(struct file *pfile, char __user *p_buff, size_t len, loff_t *poffset){

/* Print a msg with the function being called 
*/
    pr_alert("%s: called (%u)\n",__FUNCTION__,len);
    return 0;
}

/*
** This function will be called when we write the Device file
*/ 
ssize_t led_device_write(struct file *pfile, const char __user *pbuff, size_t len, loff_t *off) {
    
    struct GpioRegisters *pdev; 

    /* Check if we have a valid handle */    

    if(unlikely(pfile->private_data == NULL))
        return -EFAULT;

    /* Get GPIO handle */
    pdev = (struct GpioRegisters *)pfile->private_data;

    if (pbuff[0]=='0') /**< If user space issued 0 */
        SetGPIOOutputValue(pdev, LedGpioPin, 0); /**< Turn off led */
    else 
        SetGPIOOutputValue(pdev, LedGpioPin, 1); /**< Turn on led */
    return len;
}

/*
** This function will be called when we close the Device file
*/
int led_device_close(struct inode *p_inode, struct file * pfile) {

    /* Release the handle to the GPIO registers */
    pfile->private_data = NULL;
    return 0;
}

//File operation structure 
static struct file_operations ledDevice_fops = {
	.owner = THIS_MODULE,
	.write = led_device_write,
	.read = led_device_read,
	.release = led_device_close,
	.open = led_device_open,
};

/*
** Module Init function
*/ 
static int __init led_device_init(void) {

	int ret;
	struct device *dev_ret;

	/* Allocatesa range of char device numbers. The major number will be chosen dynamically, and returned */
  	if(( ret = alloc_chrdev_region(&ledDevice_majorminor, 0, 1, DEVICE_NAME)) <0){
		return ret;
  	}
 
	/* IS_ERR is used to check if class create succeded creating ledDevice_class . If an error occurs unregister 
	 ∗ the char driver and signal the error . */

	if(IS_ERR(ledDevice_class = class_create(THIS_MODULE, CLASS_NAME))){
		unregister_chrdev_region(ledDevice_majorminor, 1);
		return PTR_ERR(ledDevice_class);
	}
	
	/* Creates a device and registers it */
	if(IS_ERR(dev_ret = device_create(ledDevice_class, NULL, ledDevice_majorminor, NULL, DEVICE_NAME))){
		class_destroy(ledDevice_class);
		unregister_chrdev_region(ledDevice_majorminor, 1);
		return PTR_ERR(ledDevice_class);
	}
	
 	/* Initialize device */
    cdev_init(&c_dev, &ledDevice_fops); /**< Map device functions */
    c_dev.owner = THIS_MODULE;

    /* Try to add a char device to the system */
    if ((ret = cdev_add(&c_dev, ledDevice_majorminor, 1)) < 0) {
        
        printk(KERN_NOTICE "Error %d adding device", ret);
        device_destroy(ledDevice_class, ledDevice_majorminor);
        class_destroy(ledDevice_class);
        unregister_chrdev_region(ledDevice_majorminor, 1);
        return ret;
    }

    /* Get GPIO handle remapped to virtual address 
     * ioremap() function is used to map the physical addres of an I/O 
     * device to the kernel virtual address. Kernel creates a page table i.e
     * mapping of virtual address to the physical address requested.
     */
    s_pGpioRegisters = (struct GpioRegisters *)ioremap(GPIO_BASE, sizeof(struct GpioRegisters));

    /* Print the virtual address */
    pr_alert("map to virtual address: 0x%x\n", (unsigned)s_pGpioRegisters);

    /* Set the GPIO as output */
    SetGPIOFunction(s_pGpioRegisters, LedGpioPin, 0b001); //Output

	return 0;
}

module_init(led_device_init);

/*
** Module exit function
*/ 
static void __exit led_device_exit(void) 
{
	SetGPIOFunction(s_pGpioRegisters, LedGpioPin, 0); //Configure the pin as input
	iounmap(s_pGpioRegisters);
	cdev_del(&c_dev);
	device_destroy(ledDevice_class, ledDevice_majorminor);
	class_destroy(ledDevice_class);
	unregister_chrdev_region(ledDevice_majorminor, 1);
}

module_exit(led_device_exit);