
MODULE_AUTHOR("Yigit Orcun Gokbulut"); /** TODO: fill in your name **/
MODULE_LICENSE("Dual BSD/GPL");

int aesd_init_module(void)
{
    return 0;
}

void aesd_cleanup_module(void)
{

}

module_init(aesd_init_module);
module_exit(aesd_cleanup_module);
