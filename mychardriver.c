
/*  This is a sample char driver - Attempt 1 */
/*  10th July 2019 */
/*  Author: Pannagashri Rao */


#include <linux/module.h>
#include <linux/types.h>
#include <linux/errno.h>
#include <linux/miscdevice.h>
#include <linux/ioport.h>
#include <linux/fcntl.h>
#include <linux/mc146818rtc.h>
#include <linux/init.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/slab.h>
#include <linux/spinlock.h>
#include <linux/io.h>
#include <linux/uaccess.h>
#include <linux/mutex.h>
#include <linux/pagemap.h>
#include <linux/fs.h>
#include <linux/random.h>
#include <linux/ioctl.h>

#define MYCHARFLUSH _IO('R', 0x08)

static int mychardriver_open_mode;
static int mychardriver_count;
static char mychardriver_buffer[1024];

static int mychardriver_open(struct inode* inode, struct file* file)
{
    mychardriver_open_mode = file->f_mode;
    printk("%s: The file is opened in %x\n", __FUNCTION__, mychardriver_open_mode);
    return 0;
}

static int mychardriver_release(struct inode* inode, struct file* file)
{
    mychardriver_open_mode = 0;
    printk("%s: The file is closed...!\n", __FUNCTION__);
    return 0;
}

static ssize_t mychardriver_read(struct file* file, char __user *buf, size_t count, loff_t *pos)
{
    int local_count = 0;
    if (mychardriver_buffer[0] == '\0') {
        printk("%s: The file is empty\n", __FUNCTION__);
        return 0;
    }

    printk("%s: Reading from file...\n", __FUNCTION__);

    while(count && (mychardriver_buffer[local_count] != '\0')) {
        printk("Putting to user: %d\n", local_count);
        put_user(mychardriver_buffer[local_count], buf++);
        local_count++; count--;
    }

    printk("Reading done..! %d bytes\n", local_count);
    return local_count;
}


static ssize_t mychardriver_write(struct file* file, const char __user *buf, size_t count, loff_t *pos)
{
    ssize_t local_count = 0;
    printk("%s: Writing to your file...\n", __FUNCTION__);
    while (count && (mychardriver_count < 1024)) {
        get_user(mychardriver_buffer[mychardriver_count], buf++);
        mychardriver_count++; local_count++; count--;
    }

    if (local_count == 0) {
        printk("Buffer overflow..!\n");
        return 0;
    }

    printk("Writing done..! %ld bytes starting from %d\n", local_count, mychardriver_count);
    printk("This is what we wrote: %s\n", mychardriver_buffer);
    return local_count;
}

static loff_t mychardriver_llseek(struct file *file, loff_t offset, int origin)
{
    loff_t pos;
    printk("%s: Seeking position %d...\n", __FUNCTION__, origin);
    switch(origin) {
        case SEEK_SET:
            pos = offset;
            break;

        case SEEK_CUR:
            pos = file->f_pos + offset;
            break;

        case SEEK_END:
            pos = 1024 - offset;
            break;

        default:
            printk("%s: Invalid lseek option\n", __FUNCTION__);
            return -1;
    }
    if (pos < 0) pos = 0;
    if (pos > 1024) pos = 1024;
    return pos;
}

static long mychardriver_ioctl(struct file *file, unsigned int cmd, unsigned long args)
{
    switch(cmd) {
        case MYCHARFLUSH:
            printk("Flushing the buffer/file...\n");
            memset(mychardriver_buffer, 0, 1024);
            printk("Flush done..!\n");
            break;

        default:
            printk("Unsupported file operation\n");
            return -EINVAL;
    }
    return 0;
}

static const struct file_operations mychardriver_fops = {
    .owner      = THIS_MODULE,
    .llseek     = mychardriver_llseek,
    .read       = mychardriver_read,
    .write      = mychardriver_write,
    .unlocked_ioctl = mychardriver_ioctl,
    .open       = mychardriver_open,
    .release    = mychardriver_release,

};

static int __init mychardriver_module_init(void)
{
    int ret = -1;
    /* Register fops structure with char device */
    ret = register_chrdev(0, "my_char_driver", &mychardriver_fops);
    if (ret < 0) {
        printk("%s: Errored while registering char device: %d\n", __FUNCTION__, ret);
        return ret;
    }
    printk("%s: My char device is registered with major number %d\n", __FUNCTION__, ret);
    return 0;
}

static void __exit mychardriver_module_exit(void)
{
    /* Deregister fops struct with char device */
    unregister_chrdev(0, "my_char_driver");
    printk("Unregistered char driver..!\n");
}

module_init(mychardriver_module_init);
module_exit(mychardriver_module_exit);
MODULE_LICENSE("GPL v2");
MODULE_VERSION("1.0.0");
MODULE_AUTHOR("Pannagashri Rao");
