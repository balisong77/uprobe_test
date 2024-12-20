/**
 * 用户空间自定义程序
*/
#include <stdio.h>
#include <unistd.h>

__attribute__((noinline))
int utest_add(int a, int b){
    return a + b;
}

__attribute__((noinline))
int utest_sub(int a, int b){
    return a - b;
}

int main(int argc, char **argv){
    int err, i;

    for (i = 0;; i++) {
        utest_add(i, i + 1);
        utest_sub(i * i, i);
        
        printf("i = %d\n", i);

        sleep(1);
        }
}