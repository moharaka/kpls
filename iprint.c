#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/path.h>
#include <linux/namei.h>
#include <linux/dcache.h>
#include <linux/fs.h>


static char *path_param;

module_param(path_param, charp, 0);

int init_module(void)
{
	int err;
	struct path pth;
	struct dentry *dtr;
	struct dentry *parent;
	struct dentry *child;

	err = 0;

	if (path_param)
		pr_info("The parameter is %s\n", path_param);
	else {
		pr_info("The parameter is empty");
		goto out;
	}

	err = kern_path(path_param, 0, &pth);
	if (err)
		goto out;

	dtr = pth.dentry;
	BUG_ON(dtr == NULL);

	/* Print dtr dtr->inode info */
	spin_lock(&dtr->d_lock);

	pr_info("The inode address is %pK\n", dtr->d_inode);
	if (dtr->d_inode) {
		pr_info("The inode number is %lu\n", dtr->d_inode->i_ino);
		pr_info("The inode size is %lld\n", i_size_read(dtr->d_inode));
	} else
		pr_info("Negative inode!\n");

	pr_info("Dentry name %s\n", dtr->d_name.name);
	spin_unlock(&dtr->d_lock);
	/* Done printing dtr info */


	/* Print parent info */
	parent = dget_parent(dtr);
	if (IS_ROOT(parent))
		goto put_all;
	spin_lock(&parent->d_lock);
	pr_info("Parent name %s\n", parent->d_name.name);
	list_for_each_entry(child, &parent->d_subdirs, d_u.d_child) {
		if (child) {
			spin_lock_nested(&child->d_lock, DENTRY_D_LOCK_NESTED);
			if (child->d_inode)
				pr_info("\tChild name %s\n",
							child->d_name.name);
			else
				pr_info("\tChild name (negative) %s\n",
							child->d_name.name);
			spin_unlock(&child->d_lock);
		}
	}
	spin_unlock(&parent->d_lock);
	/* Done printing parent info*/

put_all:
	dput(parent);
	path_put(&pth);
out:
	return err;
}

void cleanup_module(void)
{
	pr_info("Module iprint unloaded\n");
}

MODULE_LICENSE("GPL");
