#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/interrupt.h>
#include <linux/hrtimer.h>
#include <linux/time.h>

// 假设使用的中断号，这里需要根据实际情况修改
#define INTERRUPT_NUMBER 1

// 高精度定时器
static struct hrtimer hr_timer;
// 记录中断开始时间
static ktime_t start_time;
// 记录中断处理总时间
static u64 total_time = 0;
// 中断处理次数
static int interrupt_count = 0;

// 定义一个设备标识符
static void *interrupt_dev_id;

// 中断处理函数
static irqreturn_t interrupt_handler(int irq, void *dev_id) {
    ktime_t end_time;
    s64 delta_ns;

    // 记录中断开始时间
    start_time = ktime_get();

    // 模拟一些中断处理工作
    // 这里可以添加实际的中断处理代码
    for (int i = 0; i < 10000; i++) {
        // 简单的循环模拟处理
    }

    // 记录中断结束时间
    end_time = ktime_get();

    // 计算中断处理时间
    delta_ns = ktime_to_ns(ktime_sub(end_time, start_time));
    total_time += delta_ns;
    interrupt_count++;

    printk(KERN_INFO "Interrupt %d handled in %lld ns\n", irq, (long long)delta_ns);

    return IRQ_HANDLED;
}

// 高精度定时器回调函数
static enum hrtimer_restart hrtimer_callback(struct hrtimer *timer) {
    // 可以在这里添加定时器触发时的操作
    // 这里只是简单地重新启动定时器
    hrtimer_forward_now(timer, ms_to_ktime(100));
    return HRTIMER_RESTART;
}

// 模块初始化函数
static int __init interrupt_response_time_init(void) {
    int ret;

    // 初始化设备标识符
    interrupt_dev_id = &interrupt_dev_id;

    // 注册中断处理函数
    ret = request_irq(INTERRUPT_NUMBER, interrupt_handler, IRQF_SHARED, "interrupt_response_time", interrupt_dev_id);
    if (ret) {
        printk(KERN_ERR "Failed to request IRQ %d\n", INTERRUPT_NUMBER);
        return ret;
    }

    // 初始化高精度定时器
    hrtimer_init(&hr_timer, CLOCK_MONOTONIC, HRTIMER_MODE_REL);
    hr_timer.function = hrtimer_callback;
    hrtimer_start(&hr_timer, ms_to_ktime(100), HRTIMER_MODE_REL);

    printk(KERN_INFO "Interrupt response time module initialized\n");
    return 0;
}

// 模块退出函数
static void __exit interrupt_response_time_exit(void) {
    // 注销中断处理函数
    free_irq(INTERRUPT_NUMBER, interrupt_dev_id);

    // 停止高精度定时器
    hrtimer_cancel(&hr_timer);

    if (interrupt_count > 0) {
        printk(KERN_INFO "Average interrupt handling time: %lld ns\n", (long long)(total_time / interrupt_count));
    }

    printk(KERN_INFO "Interrupt response time module exited\n");
}

module_init(interrupt_response_time_init);
module_exit(interrupt_response_time_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Your Name");
MODULE_DESCRIPTION("Test interrupt response time");