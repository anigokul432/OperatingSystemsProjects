#include <linux/module.h>
#include <linux/printk.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/sched.h>

static int seq_show(struct seq_file *s, void *v)
{
	// count the number of processes
	struct task_struct *t;
	int cnt = 0;
	for_each_process(t) {
		cnt = cnt + 1;
	}

	// show the number of processes
	seq_printf(s, "%d\n", cnt);
	return 0;
}

static struct proc_dir_entry* my_new_entry;

static int __init proc_count_init(void)
{
	pr_info("proc_count: init\n");

	// create a file called count and pass the show function
	my_new_entry = proc_create_single("count", 0644, NULL, seq_show);

	return 0;
}

static void __exit proc_count_exit(void)
{
	pr_info("proc_count: exit\n");
	proc_remove(my_new_entry);
}

module_init(proc_count_init);
module_exit(proc_count_exit);

MODULE_AUTHOR("Anieerudh Gokulakrishnan");
MODULE_DESCRIPTION("This kernel module creates a file /proc/count which displays the current number of running processes and deletes the file when the module is unloaded.");
MODULE_LICENSE("GPL");
