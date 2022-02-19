#include<linux/init.h>
#include<linux/module.h> /* Specifically, a module */
#include<linux/moduleparam.h>
#include<linux/version.h>
#include<linux/uaccess.h> /* for get_user and put_user */
#include<linux/kernel.h> /* We're doing kernel work */
#include<linux/types.h>
#include<linux/kdev_t.h>
#include<linux/fs.h>
#include<linux/device.h>
#include<linux/cdev.h>
#include <linux/ioctl.h>
#include <linux/types.h>
#include <linux/random.h>
#include<linux/errno.h>
#include<linux/slab.h> 

#define MAGIC_NUM 900
#define SET_CHANNEL _IOW(MAGIC_NUM,0,int32_t*)
#define SET_ALLIGNMENT _IOW(MAGIC_NUM,1,int32_t*)
#define SET_MODE _IOW(MAGIC_NUM,2,int32_t*)

static dev_t adc; /*Global variable for the adc device number*/
static struct cdev c_dev;
static struct class *cls;

uint16_t data;
uint32_t mode, allignment, channel;

/*Read system call for ADC*/

static int device_open(struct inode *i, struct file *f)
{
	printk(KERN_INFO "ADC is open \n");
	return 0;	
}
static int device_close(struct inode *i, struct file *f)
{
	printk(KERN_INFO "ADC closed\n");
	return 0; 
	
}

static ssize_t device_read(struct file *f, char __user *buf, size_t len, loff_t *off) /*ADC read function*/
{
	printk(KERN_INFO "Data from ADC()\n");
	get_random_bytes(&data, 2); /*creating 16bits random number as virtual ADC*/
	if(allignment == 0)
	{
	printk(KERN_INFO "Right alligned\n");
	data=((data>>4)& 0x0FFF);/*Right shifting data to obtain significant 12 bits*/
	printk(KERN_INFO "ADC value : %x\n",data);
	copy_to_user(buf, &data, 2);
	}
	
	else if (allignment== 1)
	{
	printk(KERN_INFO "Left alligned\n");
	data=(data & 0xFFF0);
	printk(KERN_INFO "ADC value : %x\n",data);
	copy_to_user(buf, &data, 2);
	}
	return 0;	
}

/*IOCTL calls for  different ADC settings */

static long device_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
         switch(cmd) {
                case SET_CHANNEL:
                        copy_from_user(&channel ,(uint32_t*) arg, sizeof(channel)); /*reading user input for selection of ADC channel*/
                        printk(KERN_INFO "ADC Channel no = %d\n", channel);
                        break;

                case SET_ALLIGNMENT:
                        copy_from_user(&allignment ,(uint32_t*) arg, sizeof(allignment)); /*reading user input for selection of ADC allignment*/
                        printk(KERN_INFO "Allignment = %d\n", allignment);
                        break;
                        
                case SET_MODE:
                        copy_from_user(&mode,(uint32_t*) arg, sizeof(mode)); /*reading user input for selection of ADC conversion mode */
                        printk(KERN_INFO "ADC mode = %d\n", mode);
                        break;
        }
        return 0;
}
/* file operation structure*/
static struct file_operations fops= 
{
	.owner = THIS_MODULE,
	.open = device_open,
	.release = device_close,
	.read = device_read,
	.unlocked_ioctl = device_ioctl,
		
};
	
/*Initialisation function*/
 static int __init adc_init(void) /* constructor*/
{
	printk(KERN_INFO " ADC registered");
	if (alloc_chrdev_region(&adc, 30, 1, "adc") < 0) /*dynamic allocation of device number by kernel*/
	{
	return -1;
	}
	
	if ((cls = class_create(THIS_MODULE, "chardrv")) == NULL) /*creating device node*/
	{
		unregister_chrdev_region(adc,1);
		return -1;
	}	
	
	if (device_create(cls, NULL, adc, NULL, "adc-dev") == NULL) 
	
	{
		class_destroy(cls);
		unregister_chrdev_region(adc,1); /*freeing device number*/
		return -1;
	}
	

	cdev_init(&c_dev, &fops); /*cdev structure initialisation*/
	if (cdev_add(&c_dev,adc,1) == -1) /*driver is live*/
	{
	device_destroy(cls,adc);
	class_destroy(cls);
	unregister_chrdev_region(adc,1); /*freeing device number*/
	return -1;
	}	
	return 0;
}

/* Exit function*/
static void __exit adc_exit(void) /* destructor*/ 

{
	cdev_del(&c_dev); /*destroying cdev structure*/
	device_destroy(cls,adc);  /*destroying device node*/
	class_destroy(cls);
	unregister_chrdev_region(adc,1);/*freeing device number*/
	printk(KERN_INFO "Bye: ADC driver unregistered \n\n");	
}

module_init(adc_init);
module_exit(adc_exit);

MODULE_DESCRIPTION("Assignment1");
MODULE_AUTHOR("Pravashika Rai <pravarai3@gmail.com>");
MODULE_LICENSE("GPL");
MODULE_INFO(ChipSupport, "PCA9687,CH340G");


