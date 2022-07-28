#include "fast_sm3.h"
#include <iostream>
#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <immintrin.h>

//#define PaddingOutput 1

void SM3_init(SM3_CTX* ctx){
    ctx->msgLen = ctx->curlen = 0;
    ctx->state[0] = SM3_A;
    ctx->state[1] = SM3_B;
    ctx->state[2] = SM3_C;
    ctx->state[3] = SM3_D;
    ctx->state[4] = SM3_E;
    ctx->state[5] = SM3_F;
    ctx->state[6] = SM3_G;
    ctx->state[7] = SM3_H;
}

void SM3_W_expend(unsigned int W[68],unsigned int W1[64],const uchar* buf)
{
    int i=0;
    for(i=0; i<16;i++)
        (void)HOST_c2l(buf, W[i]); //利用宏函数，从char类型获得正确顺序的int类型

    unsigned int tmp;
    for(i=16;i<=67;i++)
    {
        tmp=W[i-16]^ W[i-9]^ROTL(W[i-3],15);
        W[i]=P1(tmp)^(ROTL(W[i-13],7))^W[i-6];
    }
    for(i=0;i<=63;i++)
    {
        W1[i]=W[i]^W[i+4];
    }
}


void SM3_compress(SM3_CTX* ctx)
{
    /*
        迭代压缩, 8个寄存器都是int类型，我们的分块进来的都是char* 的字节类型，
        比如{0x12,0x34,0x45,0x56}我们必须把他在内存中写成大端序，系统才能读出我们想要的0x123456的形式去运算
    */
    unsigned int W[68]; 
    unsigned int W1[64];
    SM3_W_expend(W,W1,(const uchar*)ctx->buf);
    CF(W,W1,ctx->state);
}


void CF(unsigned int W[68], unsigned int W1[64], unsigned int V[])
{
    unsigned int SS1;
    unsigned int SS2;
    unsigned int TT1;
    unsigned int TT2;
    unsigned int T=SM3_T1;
    unsigned int FF;
    unsigned int GG;
    int j;
    //reg init,set ABCDEFGH=V0
    register unsigned int A, B, C, D, E, F, G, H;
    A = V[0];  
    B = V[1];
    C = V[2];
    D = V[3];
    E = V[4];
    F = V[5];
    G = V[6];
    H = V[7];
    for(j=0;j<=63;j++)
    {
        //SS1
        if(j==0)
        {
            T=SM3_T1;
        }
        else if(j==16)
        {
            T=ROTL(SM3_T2,16);
        }
        else
        {
            T=ROTL(T,1);
        }
        SS1=ROTL((ROTL(A,12)+E+T),7);
        SS2=SS1^ROTL(A,12);
        if(j<=15)
        {
            FF=FF0(A,B,C);
        }
        else
        {
            FF=FF1(A,B,C);
        }
        TT1=FF+D+SS2+*W1;
        W1++;
        if(j<=15)
        {
            GG=GG0(E,F,G);
        }
        else
        {
            GG=GG1(E,F,G);
        }
        TT2=GG+H+SS1+*W;
        W++;
        D=C;
        C=ROTL(B,9);
        B=A;
        A=TT1;
        H=G;
        G=ROTL(F,19);
        F=E;
        E=P0(TT2);
    }
    //update V
    V[0] ^=A;
    V[1] ^=B;
    V[2] ^=C;
    V[3] ^=D;
    V[4] ^=E;
    V[5] ^=F;
    V[6] ^=G;
    V[7] ^=H;
}


void SM3_process(SM3_CTX* ctx,uchar* input,int msg_bytelen)
{
    while(msg_bytelen>=64)
    {
        memcpy(ctx->buf,input+(int)(ctx->curlen>>9),64);
        msg_bytelen -= 64;
        SM3_compress(ctx);
        ctx->msgLen += 512;
        ctx->curlen = 64;
    }
    //将不足64byte的部分，拷贝到ctx中，注意到，如果是0的话，就不用拷贝了，但是要把curlen清掉
    memset((void*)ctx->buf,0,64);   //先置零
    int tmp = msg_bytelen & 63;     //等价mod64 
    if(tmp)
    {
        memcpy(ctx->buf,input+(ctx->msgLen>>3),tmp);
        ctx->msgLen += tmp <<3;  
    }
    //理论上可以直接用总的长度计算msglen，但是考虑到我们的input可能也不是完整的，因此压缩多少计数多少比较合理
    ctx->curlen = tmp ;        
}


void SM3_paddingpart(SM3_CTX* ctx,uchar* output)
{
    ctx->buf[ctx->curlen] = 0x80;
    ctx->curlen++;
    /*如果填充0x80后当前长度大于56，就不满足尾部8字节填充文本长度规则，
      因此要在下一块中进行填充
    */
    if (ctx->curlen >56)
    {
        SM3_compress(ctx); //直接先把当前block压缩
        memset((void*)ctx->buf,0,64);   //置零
        ctx->curlen = 0;
    }
    ctx->buf[63] =  ctx->msgLen & 0xff;
    ctx->buf[62] = (ctx->msgLen >> 8) & 0xff;
    ctx->buf[61] = (ctx->msgLen >> 16) & 0xff;
    ctx->buf[60] = (ctx->msgLen >> 24) & 0xff;
    //注意此处的curlen和msglen，一直没有动，还是原来的值
#ifdef PaddingOutput
    cout<<"Padding后消息："<<endl;
    print_Hashvalue(ctx->buf,64);
#endif
    SM3_compress(ctx);
    memcpy(output,ctx->state,SM3_OUTLEN);
    for(int i=0;i<8;i++)
        (void)HOST_l2c(ctx->state[i],output);   //利用宏函数，将大端序储存在内存的每个int数据倒序存放，这样才能读出正确顺序的char
}

void SM3(uchar* input, int msg_bytelen, uchar output[SM3_OUTLEN])
{
    SM3_CTX ctx;
    //SM3_init(&ctx);
    ctx.msgLen = ctx.curlen = 0;
    ctx.state[0] = SM3_A;
    ctx.state[1] = SM3_B;
    ctx.state[2] = SM3_C;
    ctx.state[3] = SM3_D;
    ctx.state[4] = SM3_E;
    ctx.state[5] = SM3_F;
    ctx.state[6] = SM3_G;
    ctx.state[7] = SM3_H;
    SM3_process(&ctx,input,msg_bytelen);
    SM3_paddingpart(&ctx, output);      //最后会将ctx中的state拷贝到output中。
    memset(&ctx, 0, sizeof(SM3_CTX));   //清空
}

void print_Hashvalue(uchar buf[],int len)
{
    int i;
    for(i=0;i<len;i++)
    {
        printf("%02x",*(buf + i));
    }
    printf("\n");
}

void SM3(string input_str)
{
    cout<<"Msg: "<<input_str<<endl;
    uchar output[SM3_OUTLEN];
    SM3((uchar*)input_str.c_str(),input_str.length(),output);
    cout << "Hash:";
    print_Hashvalue(output,32);
}

void SM3(string input_str,uchar output[SM3_OUTLEN])
{
    SM3((uchar*)input_str.c_str(),input_str.length(),output);
}
