# Linux_Shell
# Linux下模拟实现简单的Shell



**完整代码：https://github.com/JiaZhengJingXianSheng/Linux_Shell** 



## 一、 fork

fork系统调用用于创建一个新进程，称为**子进程**，它与进程（称为系统调用fork的进程）同时运行，此进程称为**父进程**。创建新的子进程后，两个进程将执行fork（）系统调用之后的下一条指令。子进程使用相同的[程序计数器](https://baike.baidu.com/item/程序计数器/3219536)，相同的CPU寄存器，在父进程中使用的相同打开文件。

### 特点

  **1）在父进程中，fork返回新创建子进程的进程ID；**
  **2）在子进程中，fork返回0；**
  **3）如果出现错误，fork返回一个负值；**

如果创建新进程成功，则出现两个进程，一个是子进程，一个是父进程。在子进程中，fork函数返回0，在父进程中，fork返回新创建子进程的进程ID。因此我们可以通过判断fork返回值来确定进程是子进程还是父进程。

## 二、 execvp

**execvp()** 会从PATH所指的目录中查找符合参数file 的文件名，找到后便执行该文件，然后将第二个参数 **argv** 传给该欲执行的文件。

如果执行成功则函数不会返回，执行失败则直接返回-1，失败原因存于errno中。我们可以通过 **perror()** 函数来打印错误。

## 三、 wait/waitpid

**wait:**  父进程一旦调用了wait就立即阻塞自己，由wait自动分析是否当前进程的某个子进程已经退出，如果让它找到了这样一个已经变成僵尸的子进程，wait就会收集这个子进程的信息，并把它彻底销毁后返回；如果没有找到这样一个子进程，wait就会一直阻塞在这里，直到有一个出现为止。

**waitpid:**  暂时停止目前进程的执行, 直到有信号来到或子进程结束. 如果在调用时子进程已经结束, 则会立即返回子进程结束状态值. 子进程的结束状态值会由参数status 返回, 而子进程的进程识别码也会一快返回.



## 四、 实现

### 1. 相关函数

```c
pid_t pid = fork();
```

调用fork函数可创建一个子进程，这边说的子进程就是我们平时所讲的线程。只不过在Linux下线程是用拷贝进程的形式实现，所以这边称为子进程。

```c
int execvp(const char *__file, char *const *__argv)
```

execvp函数可以调用PATH下的命令，其中参数1为操作码（命令），参数2为操作码+操作数（命令+参数）。

```c
perror("fork error");
```

eperror函数用来打印错误

```c
wait(NULL);
```

在父进程中调用wait函数，会阻塞父进程，并检查子进程是否退出，未退出则杀死子进程。

```c
chdir();
```

由于父进程和子进程并不共享环境变量，子进程修改了当前工作目录的环境变量对父进程也没有什么影响，所以cd命令是无法简单用execvp函数实现的，这边会用到chdir函数。

### 2. 具体实现

首先从键盘读取一行命令。并将命令根据空格划分开，第一个参数即为命令，后续为参数。

```c
char cmd[64];
cin.getline(cmd, 64);
```

```c
char *command[64];
const char *s = " ";
char *token;

/* 获取第一个子字符串 */
token = strtok(cmd, s);

int i = 0;
 /* 继续获取其他的子字符串 */
while (token != NULL)
{
      command[i] = token;
      token = strtok(NULL, s);
      i++;
}
command[i] = NULL;
```

至此我么就将命令按格式全部划分。

接下来我们去创建一个子线程，在子线程内，判断命令如果为cd则调用chdir()， 否则调用execvp() , 并用perror(), 打印错误。在父进程调用wait() 函数来回收子进程。

```c
        if (strcmp(command[0], "exit") == 0)
            exit(0);

        pid_t pid = fork(); // 创建子进程（线程）

        if (pid == 0) // 子进程
        {

            if (strcmp(command[0], "cd") == 0) // cd命令用chdir函数
                chdir(command[1]);

            else
            {
                execvp(command[0], command);
                perror("fork error"); // execvp未执行成功，fork失败
                exit(1);
            }
        }
        wait(NULL); // 子进程死亡回收
```

### 3. 结果

```
lyz@ubuntu:~/Desktop/LinuxHomework2$ g++ MyShell.cpp -o SHELL
lyz@ubuntu:~/Desktop/LinuxHomework2$ ./SHELL 
Shell>> pwd
/home/lyz/Desktop/LinuxHomework2
Shell>> ls 
MyShell.cpp  SHELL
Shell>> cd ..
Shell>> pwd
/home/lyz/Desktop
Shell>> exit
lyz@ubuntu:~/Desktop/LinuxHomework2$ 

```

我们的shell运行正常
