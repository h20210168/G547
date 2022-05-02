#include <linux/delay.h>
#include <linux/i2c.h>
#include <linux/iio/iio.h>
#include <linux/iio/sysfs.h>
#include <linux/module.h>

#define BH1750_PWR_DOWN		0x00
#define BH1750_ONE_TIME_H_RES_MODE	0x20

enum {
	BH1750
};

struct bh1750_chip_info;
struct bh1750_data {
	struct i2c_client *client;
	//An i2c_client identifies a single device (i.e. chip) connected to an i2c bus.
	struct mutex lock;
	const struct bh1750_chip_info *chip_info;
	u16 mtreg;
};

/* Contains all chip information */
struct bh1750_chip_info {
	u16 mtreg_min;
	u16 mtreg_max;
	u16 mtreg_default;
	int mtreg_to_usec;
	int mtreg_to_scale;
	int inc;
	u16 int_time_low_mask;
	u16 int_time_high_mask;
}

static const bh1750_chip_info_tbl[] = {
	[BH1750] = { 31,  254,  69,  1740, 57500000,  1, 0x001F, 0x00E0 }
};

static int bh1750_read(struct bh1750_data *data, int *val)
{
	int ret;
	__be16 result;
	
	ret = i2c_smbus_write_byte(data->client, BH1750_ONE_TIME_H_RES_MODE);
	//  sensor operates on continous mode
	if (ret < 0)
		return ret;
	ret = i2c_master_recv(data->client, (char *)&result, 2);
	//Receives 2bytes of data from sensor
	if (ret < 0)
		return ret;

	*val = be16_to_cpu(result);
	//convert big endian format to u16
	printk(KERN_INFO "Light=%d lx",*val);

	return 0;
}

static int bh1750_read_raw(struct iio_dev *indio_dev,
			   struct iio_chan_spec const *chan,
			   int *val, int *val2, long mask)
{
	int ret;
	struct bh1750_data *data = iio_priv(indio_dev);
	switch (mask) {
	case IIO_CHAN_INFO_RAW:
		switch (chan->type) {
		case IIO_LIGHT:
		    //For particular bit mask.
			mutex_lock(&data->lock);
			ret = bh1750_read(data, val);
			mutex_unlock(&data->lock);
			if (ret < 0)
				return ret;

			return IIO_VAL_INT;
		default:
			return -EINVAL;// Error in value
		}
	default:
		return -EINVAL;
	}
}

static const struct iio_info bh1750_info = {
	.read_raw = bh1750_read_raw //file operation 
};
//An IIO channel is described by the struct iio_chan_spec.
static const struct iio_chan_spec bh1750_channels[] = {
	{
		.type = IIO_LIGHT,//light sensor
		.info_mask_separate = BIT(IIO_CHAN_INFO_RAW) // attributes will be specific to this channel
	}
};

//Device structure for the i2c bus required to communicate with the device

static int bh1750_probe(struct i2c_client *client,
			const struct i2c_device_id *id)
{
	struct bh1750_data *data; /* driver store references to bus and framework structures,
    plus its own data (locks, wait queues, etc.)*/
    
	struct iio_dev *indio_dev; //iio structure for each device
	if (!i2c_check_functionality(client->adapter, I2C_FUNC_I2C |
				I2C_FUNC_SMBUS_WRITE_BYTE))
		return -EOPNOTSUPP;
		
	//allocate an iio_dev from a driver
	indio_dev = devm_iio_device_alloc(&client->dev, sizeof(*data));

	if (!indio_dev)
		return -ENOMEM;// not enough memory
	data = iio_priv(indio_dev);
	i2c_set_clientdata(client, indio_dev);
	data->client = client;
	data->chip_info = &bh1750_chip_info_tbl[id->driver_data];

	mutex_init(&data->lock);
	indio_dev->dev.parent = &client->dev;
	indio_dev->info = &bh1750_info;
	indio_dev->name = id->name;
	indio_dev->channels = bh1750_channels;
	indio_dev->num_channels = ARRAY_SIZE(bh1750_channels);
	indio_dev->modes = INDIO_DIRECT_MODE;/*the device can operate using software
     triggers, enabling the device */
	

	return iio_device_register(indio_dev);
	//Register a new device and is now accessible in user-space)
}

static int bh1750_remove(struct i2c_client *client)
{
	struct iio_dev *indio_dev = i2c_get_clientdata(client);
	//private data structure
	struct bh1750_data *data = iio_priv(indio_dev);

	iio_device_unregister(indio_dev);
    //Unregister device from the system
    iio_device_free(indio_dev);
    //free the allocated memory
    
	mutex_lock(&data->lock);
	i2c_smbus_write_byte(client, BH1750_PWR_DOWN);
	//Device sentto power down mode of operation
	mutex_unlock(&data->lock);

	return 0;
}

static SIMPLE_DEV_PM_OPS(bh1750_pm_ops, bh1750_suspend, NULL);

//Match by name for I2C devices
static const struct i2c_device_id bh1750_id[] = {
	{ "bh1750", BH1750 },
	{ }
};


MODULE_DEVICE_TABLE(i2c, bh1750_id);

//Match by compatible property
static const struct of_device_id bh1750_of_match[] = {
	{ .compatible = "rohm,bh1750", },
	{ }
};
MODULE_DEVICE_TABLE(of, bh1750_of_match); /*  for kernel to know the device is managed by this driver */

static struct i2c_driver bh1750_driver = {
	.driver = {
		.name = "bh1750",
		.of_match_table = bh1750_of_match,
		.pm = &bh1750_pm_ops,
	},
	.probe = bh1750_probe,
	.remove = bh1750_remove,
	.id_table = bh1750_id,

};
 //Registering driver//
module_i2c_driver(bh1750_driver);

MODULE_DESCRIPTION("BH1750 Driver");
MODULE_LICENSE("GPL");
