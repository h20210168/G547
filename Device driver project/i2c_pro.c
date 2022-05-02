#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/i2c.h>

//Address probing 

static const unsigned short probe_i2c_addrs[] = I2C_ADDRS(0x23, 0x5c);
//   7-bit I2C slave addresses: 0x23 (ADDR pin low), 0x5C (ADDR pin high)


// board info
static struct i2c_board_info bh1750_i2c_info[] = {

	{ I2C_BOARD_INFO("bh1750", 0x23), }
};

static int __init i2c_adapter_init(void)
{
	
	int nr;
	printk(KERN_ALERT "%s probing bus\n",__FUNCTION__);
	for(nr =0; nr < 6; nr++) {
		struct i2c_adapter * adap = i2c_get_adapter(nr);
		//gives structure of i2c_adapter 
		if(adap) {
			i2c_new_scanned_device(adap, bh1750_i2c_info, probe_i2c_addrs, NULL);
		}
	}
	
	return 0;
}

static void __exit i2c_adapter_exit(void)
{
	printk(KERN_ALERT "%s\n", __FUNCTION__);
}

module_init(i2c_adapter_init);
module_exit(i2c_adapter_exit);

MODULE_AUTHOR("Owner");l
MODULE_DESCRIPTION("I2C ");
MODULE_LICENSE("GPL");


