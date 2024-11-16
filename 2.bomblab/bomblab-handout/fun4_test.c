#include <stdio.h>

int func4(int arg1, int arg2, int arg3){
    int ebx;
    ebx = arg3 - arg2 + (arg3>=arg2 ? 0 : -1);
    ebx = (unsigned) ebx;
    ebx >>= 1;
    ebx = (int) ebx;
    ebx += arg2;
    if(ebx > arg1){
        arg3 = ebx -1;
        ebx += func4(arg1, arg2, arg3);
        return ebx;
    }else if(ebx < arg1){
        arg2 = ebx + 1;
        ebx += func4(arg1, arg2, arg3);
        return ebx;
    }else{
        return ebx;
    }
}

int main(){
    // 求解方程func4(x, 0, 14) = 19, 0 <= x <= 14
    for(int x=0; x<=14; x++)
        if(func4(x, 0, 14) == 19)
            printf("Num1 = %d found.\n", x);
    return 0;
}