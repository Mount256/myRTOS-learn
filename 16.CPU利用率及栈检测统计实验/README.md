## 注意事项

`Doc`文件夹的内容是一些相关知识点，可供参考。

项目裁剪：在`os_cfg.h`和`stm32f10x_conf.h`中那一堆`#define`里可以自行修改是否保留对应代码。

配置用户代码：在`app.c`和`app_cfg.h`里操作。

配置硬件：在`bsp.h`和`bsp.c`里操作。

## 实验内容

在uCOS中创建了四个任务`AppTaskPost()`、 `AppTaskPend1()`和 `AppTaskPend2()`，第四个任务是'AppTaskStatus()'。

任务 `AppTaskPost()` 用于发送消息，任务 `AppTaskPend1()` 和`AppTaskPend2()`均用于接收上述消息，三个任务独立运行，并通过串口调试助手把结果打印出来。
 
'AppTaskStatus()'任务用于获取CPU利用率与任务相关信息并通过串口打印出来。

