/*
    Code create by LiuYuZhao
    Time: 2021/12/14
*/

#include <iostream>
#include "string.h"
#include <unistd.h>
#include <sys/wait.h>
using namespace std;

int main()
{
    while (1)
    {
        cout << "Shell>> ";

        char cmd[64];
        cin.getline(cmd, 64);

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
    }

    return 0;
}