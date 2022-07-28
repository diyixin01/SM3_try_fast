#pragma once
#include <string.h>
#include <stdio.h>
#include <iostream>
using namespace std;
typedef unsigned int uint;
typedef unsigned char uchar;

# define SM3_WORD uint  //SM3中的单位，是4byte
# define SM3_OUTLEN    32       //输出32字节长的结果，32*8=256bit
# define SM3_BLOCK  64          //一次对64个字节的数据进行压缩

#define SM3_A 0x7380166f
#define SM3_B 0x4914b2b9
#define SM3_C 0x172442d7
#define SM3_D 0xda8a0600
#define SM3_E 0xa96f30bc
#define SM3_F 0x163138aa
#define SM3_G 0xe38dee4d
#define SM3_H 0xb0fb0e4e

#define SM3_T1 0x79CC4519
#define SM3_T2 0x7A879D8A

typedef struct{
    SM3_WORD state[8];        //256bit长，8个SM3_WORD寄存器，就是A-H
    uint msgLen;     //消息总长度
    uint curlen;      //当前block消息长度
    uchar buf[64];    //每次只存一个block，64byte
}SM3_CTX;

void SM3_init(SM3_CTX* ctx) ;
void SM3_compress(SM3_CTX* ctx);
void SM3_process(SM3_CTX* ctx,uchar* input,int msg_bytelen);
void SM3_paddingpart(SM3_CTX* ctx,uchar* output);
void SM3_W_expend(uint W[68],uint W1[64],const uchar* buf);
void CF(uint W[68], uint W1[64], uint V[]);
void SM3(uchar* input, int msg_bytelen, uchar output[32]);
void SM3(string input_str);
void SM3(string input_str,uchar output[SM3_OUTLEN]);
void print_Hashvalue(uchar buf[],int len);

/************************************************************************
 * 宏函数名：HOST_c2l
 * 描述: char to int（long）
 * 例:  系统读取整数是按照大端序，比如我们有char* a 指向4byte数组{0x12,0x34,0x56,0x78}，注意a不能是数组名，因为数组名不能++
 *      HOST_c2l(a,b)就可以得到uint b= 0x12345678,如果直接令 b = *(int*)a,系统会按照大端读出得到b=0x78563412 
 * 
************************************************************************/
#  define HOST_c2l(c,l) (l =(((unsigned long)(*(c++)))<<24),          \
                         l|=(((unsigned long)(*(c++)))<<16),          \
                         l|=(((unsigned long)(*(c++)))<< 8),          \
                         l|=(((unsigned long)(*(c++)))    )           )
                         
/************************************************************************
 * 宏函数名：HOST_l2c
 * 描述: int(long) to char
 * 例:  系统读取整数是按照大端序，比如我们有uint b= 0x12345678,系统中储存的就是78 56 34 12
 *      如果直接令a指向b的内存，那么a指向的就是{0x78,0x56,0x34,0x12} 
 *      HOST_l2c(b,a)就可以令a指向{0x12,0x34,0x56,0x78}
 * 
************************************************************************/                
#  define HOST_l2c(l,c) (*(c++)=(uchar)(((l)>>24)&0xff),      \
                         *(c++)=(uchar)(((l)>>16)&0xff),      \
                         *(c++)=(uchar)(((l)>> 8)&0xff),      \
                         *(c++)=(uchar)(((l)    )&0xff),      \
                         l)

/*  利用define实现部分函数  */
# define ROTATE(a,n)     (((a)<<(n))|((a&0xffffffff)>>(32-n)))

#define FF0(x,y,z) ( x^ y ^ z)
#define FF1(x,y,z) ((x& y) | ( x & z) | ( y & z))

#define GG0(x,y,z) ( x ^ y ^ z)
#define GG1(x,y,z) ((x & y) | ( (~x) & z) )

#define SHL(x,n) ((x & 0xFFFFFFFF) << n)
#define ROTL(x,n) (SHL(x,n) | (x >> (32 - n))) //实现循环左移n位

#define P0(x) (x ^  ROTL(x,9) ^ ROTL(x,17))
#define P1(x) (x ^  ROTL(x,15) ^ ROTL(x,23))
