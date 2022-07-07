#include <rtthread.h>
#include <dfs_posix.h> /* 当需要使用文件操作时，需要包含这个头文件 */
static void mkdir_sample(void)
{
    int ret;
    /* 创建目录*/
    ret = mkdir("/webnet", 0x777);
    if (ret < 0)
    {
        /* 创建目录失败*/
        rt_kprintf("dir error!\n");
    }
    else
    {
        /* 创建目录成功*/
        rt_kprintf("mkdir ok!\n");
    }
}
/* 导出到 msh 命令列表中 */
MSH_CMD_EXPORT(mkdir_sample, mkdir sample);
