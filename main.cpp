#include "fast_sm3.h"
#include<time.h>
#include <string>

//#define VS 1

void timetest()
{
    clock_t start,end;//定义clock_t变量
    uchar input[64] = {0xaa,0xaa,0xaa,0xaa,0xaa,0xaa,0xaa,0xaa,0xaa,0xaa,0xaa,0xaa,0xaa,0xaa,0xaa,
                        0xaa,0xaa,0xaa,0xaa,0xaa,0xaa,0xaa,0xaa,0xaa,0xaa,0xaa,0xaa,0xaa,0xaa,0xaa,
                        0xaa,0xaa,0xaa,0xaa,0xaa,0xaa,0xaa,0xaa,0xaa,0xaa,0xaa,0xaa,0xaa,0xaa,0xaa,
                        0xaa,0xaa,0xaa,0xaa,0xaa,0xaa,0xaa,0xaa,0xaa,0xaa,0xaa,0xaa,0xaa,0xaa,0xaa,
                        0xaa,0xaa,0xaa,0xaa};
    uchar output[32];
    int64_t t = 10000000; 
    start = clock();  //开始时间
    for(int i=0;i<t;i++)
        SM3(input,64,output);
    end = clock();   //结束时间
#ifdef VS
    cout << "For 64 byte data, doing " << t << " times SM3 costs time = " << double(end - start) / CLOCKS_PER_SEC << "s" << endl;  //输出时间（单位：ｓ）
    cout << "Average do " << t / (double(end - start) / CLOCKS_PER_SEC) << " times SM3 in 1s" << endl;
#endif // VS
#ifndef VS     
    cout << "对64字节的数据做" << t << "次SM3，花费时间：time = " << double(end - start) / CLOCKS_PER_SEC << "s" << endl;  //输出时间（单位：ｓ）
    cout << "平均每秒做" << t / (double(end - start) / CLOCKS_PER_SEC) << "次SM3" << endl;
#endif // !VS
}

int main(int argc, char* argv[])
{
    timetest();
    string input_str = "Hello World";
    SM3(input_str);
    cout<<"input a string:";
    getline(cin,input_str); //使用getline获取输入（字符串格式），防止cin遇到空格被打断；
    SM3(input_str);
    printf("\n");
    return 0;
}
