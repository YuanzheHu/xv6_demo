#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

/*
 * pingpong 程序功能：
 * 使用两个 pipe 在父子进程之间传递 1 个字节。
 *
 * 流程：
 * 父进程  --(pipe1)-->  子进程
 * 子进程  --(pipe2)-->  父进程
 *
 * 输出：
 *   <child_pid>: received ping
 *   <parent_pid>: received pong
 */

int
main(int argc, char *argv[])
{
    int p1[2];  // pipe1：父 -> 子
    int p2[2];  // pipe2：子 -> 父

    char buf[1];  // 只传 1 个字节

    // 创建两个管道
    // p1[0] = 读端, p1[1] = 写端
    // p2[0] = 读端, p2[1] = 写端
    pipe(p1);
    pipe(p2);

    int pid = fork();  // 创建子进程

    if(pid == 0){
        // =========================
        // 子进程执行的代码
        // =========================

        // 关闭不需要的端口：
        // 子进程只从 p1 读，所以关闭 p1 的写端
        close(p1[1]);

        // 子进程只往 p2 写，所以关闭 p2 的读端
        close(p2[0]);

        // 从父进程发送的 pipe1 中读取 1 字节
        // 如果父进程还没写，这里会阻塞等待
        read(p1[0], buf, 1);

        // 打印收到 ping
        printf("%d: received ping\n", getpid());

        // 把字节通过 pipe2 写回父进程
        write(p2[1], buf, 1);

        // 子进程结束
        exit(0);
    }
    else{
        // =========================
        // 父进程执行的代码
        // =========================

        // 关闭不需要的端口：
        // 父进程只往 p1 写，所以关闭 p1 的读端
        close(p1[0]);

        // 父进程只从 p2 读，所以关闭 p2 的写端
        close(p2[1]);

        // 向子进程发送 1 个字节
        buf[0] = 'a';
        write(p1[1], buf, 1);

        // 等待子进程通过 pipe2 写回数据
        // 如果子进程还没写，这里会阻塞
        read(p2[0], buf, 1);

        // 打印收到 pong
        printf("%d: received pong\n", getpid());

        // 父进程结束
        exit(0);
    }
}
