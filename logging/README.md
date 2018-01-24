# 实现问题
+ 线程安全地构造：`Async_logger`在构造函数中逸出了this，但由于thread在最后构造，且不存在虚函数调用，因此是安全的。
+ 线程安全地析构：
    + `Async_logger`析构时，后台线程正在与其竞争。同样，由于不存在虚函数调用，因此也是安全的。
    + `Async_logger`析构时，需要关闭后台线程。后台线程只应该在条件变量上阻塞，因此，我们需要解除阻塞。最开始时，由于忘记解除阻塞，写入/dev/null需要3s钟。比常规文件还慢，久思不得其解。

# 性能对比
这些的性能对应以muduo为一个baseline。由于我们没有考虑格式化问题，所以理论上应该比muduo快。

先看muduo的数据
```
nop: 0.221695 seconds, 109888890 bytes, 4510701.64 msg/s, 472.71 MiB/s
/dev/null: 0.233367 seconds, 109888890 bytes, 4285096.01 msg/s, 449.07 MiB/s
/tmp/log: 0.270322 seconds, 109888890 bytes, 3699291.96 msg/s, 387.68 MiB/s
test_log_st: 0.259554 seconds, 109888890 bytes, 3852762.82 msg/s, 403.76 MiB/s
test_log_mt: 0.276268 seconds, 109 888 890 bytes, 3 619 673.65 msg/s, 379.34 MiB/s
```

再看我们的数据
```
// 8 * 4MB 
/dev/null: 0.106000 seconds, 43000000 bytes, 9433962.00 msg/s, 386.87 MiB/s
/tmp/log: 0.095000 seconds, 43000000 bytes, 10526316.00 msg/s, 431.66 MiB/s
```

后来，我尝试禁用了FILE的缓冲，然而效果不大。

# Buffer size的选择
由于使用了双缓冲机制，对于缓冲大小的选择十分重要。首先，能否进行一些量化的估计？

我们的目标是每秒写1M条日志，每条日志长度约为100B，因此，就吞吐而言，我们的日志库应该支持100MB的吞吐。显然，磁盘是能够支持的。因此，我们换一个角度，从IOPS入手。对于硬盘而言，IOPS在100左右（保守估计），在顺序写情况下，可以做得更好。为了1s完成100MB吞吐，我们能够写100次，每次1MB。于是，可以得到，缓冲大小约为1MB。

当然，为了准确，我们需要benchmark。

```
4-KB: 0.103000 seconds, 43000000 bytes, 9708738.00 msg/s, 398.14 MiB/s
8-KB: 0.101000 seconds, 43000000 bytes, 9900990.00 msg/s, 406.02 MiB/s
16-KB: 0.102000 seconds, 43000000 bytes, 9803922.00 msg/s, 402.04 MiB/s
32-KB: 0.103000 seconds, 43000000 bytes, 9708738.00 msg/s, 398.14 MiB/s
64-KB: 0.102000 seconds, 43000000 bytes, 9803922.00 msg/s, 402.04 MiB/s
128-KB: 0.103000 seconds, 43000000 bytes, 9708738.00 msg/s, 398.14 MiB/s
256-KB: 0.102000 seconds, 43000000 bytes, 9803922.00 msg/s, 402.04 MiB/s
512-KB: 0.101000 seconds, 43000000 bytes, 9900990.00 msg/s, 406.02 MiB/s
1024-KB: 0.100000 seconds, 43000000 bytes, 10000000.00 msg/s, 410.08 MiB/s
2048-KB: 0.099000 seconds, 43000000 bytes, 10101010.00 msg/s, 414.22 MiB/s
4096-KB: 0.095000 seconds, 43000000 bytes, 10526316.00 msg/s, 431.66 MiB/s
8192-KB: 0.092000 seconds, 43000000 bytes, 10869565.00 msg/s, 445.74 MiB/s
16384-KB: 0.093000 seconds, 43000000 bytes, 10752688.00 msg/s, 440.95 MiB/s
32768-KB: 0.093000 seconds, 43000000 bytes, 10752688.00 msg/s, 440.95 MiB/s
65536-KB: 0.105000 seconds, 43000000 bytes, 9523810.00 msg/s, 390.55 MiB/s
```

结果很显然，在测试机上，最佳大小为8MB。
