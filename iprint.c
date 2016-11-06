#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/path.h>
#include <linux/namei.h>
#include <linux/dcache.h>
#include <linux/fs.h>


static char* path_param = NULL;

module_param(path_param, charp, 0);


int init_module(void)
{
	struct path pth;
	struct dentry *dtr;
	struct dentry *parent;
	struct dentry *child;

	if(path_param)
		printk(KERN_INFO "The parameter is %s\n", path_param);
	else {
		printk(KERN_INFO "The parameter is empty");
		goto out;
	}
	
	if(kern_path(path_param, 0, &pth) != 0)
		goto donexist;

	dtr = pth.dentry;
	if(dtr == NULL)
		goto donexist;
	
	/* Print dtr dtr->inode info */
	spin_lock(&dtr->d_lock);
	printk(KERN_INFO "The inode address is %pK\n", dtr->d_inode);
	if(dtr->d_inode) {
		printk(KERN_INFO "The inode number is %lu\n", dtr->d_inode->i_ino);
		printk(KERN_INFO "The inode size is %lld\n", dtr->d_inode->i_size);
	}else
		printk(KERN_INFO "Negative inode!\n");
	printk(KERN_INFO "Dentry name %s\n", dtr->d_name.name);
	parent = dtr->d_parent;
	spin_unlock(&dtr->d_lock);
	/* Done printing dtr info*/

	/* Print parent info */
	spin_lock(&parent->d_lock);
	if(IS_ROOT(parent))
		goto unlockparent;
	printk(KERN_INFO "Parent name %s\n", parent->d_name.name);
	list_for_each_entry(child, &parent->d_subdirs, d_u.d_child) {
		if (child) {
			spin_lock_nested(&child->d_lock, DENTRY_D_LOCK_NESTED);
			if(child->d_inode)/* skip negative dentries ? */
				printk(KERN_INFO "\tChild dentry name %s\n", child->d_name.name);
			else
				printk(KERN_INFO "\tChild dentry name (negative) %s\n", child->d_name.name);
			spin_unlock(&child->d_lock);
		}
	}
unlockparent:
	spin_unlock(&parent->d_lock);
	/* Done printing parent info*/
	goto out;
donexist:	
	printk(KERN_INFO "The file %s does not exist\n", path_param);
out:
	return 0;
}

void cleanup_module(void)
{
	printk(KERN_INFO "Module %s unloaded\n", __FILE__);
}

MODULE_LICENSE("GPL");

