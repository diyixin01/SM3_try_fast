# SM3实现并加速

# 运行方式

编译器运行即可


# 文件说明

main.cpp 速度测试


fast_sm3.h 算法实现过程的各种函数声明


fast_sm3.cpp SM3算法函数定义


# 函数解释

#  SM3_cpmpress 
对传入的block进行压缩，主要调用消息拓展和CF函数


![image](https://user-images.githubusercontent.com/75195549/181437371-3348a078-ccc6-4e19-b47b-8248415a851a.png)



# CF
对64byte的大端序数据进行压缩，得到传入block的压缩值，更新到ctx->state中



![image](https://user-images.githubusercontent.com/75195549/181437510-76d10f11-a5b9-4838-b8c5-14e9cfbf88b6.png)




# SM3_process 
处理input的前几个block，并将最后一个不满64byte的block预处理一下


![image](https://user-images.githubusercontent.com/75195549/181437619-8255a7ed-b301-432e-8a60-962d3e011a6a.png)




# SM3_paddingpart 
处理最后一个不满64byte的数据块，主要是填充后进行压缩,并将最终结果正确拷贝到output



![image](https://user-images.githubusercontent.com/75195549/181437704-b94b766d-345f-4bc2-9c13-8a4673599d17.png)




# SM3
计算传入的input的SM3值并储存到output中


![image](https://user-images.githubusercontent.com/75195549/181437781-02542005-16b8-468f-9ce5-6f2dd22c8b67.png)




# timetest
时间测试函数


# main函数



![image](https://user-images.githubusercontent.com/75195549/181438191-0477d3fd-5354-44eb-8461-ec8659dbc07b.png)



# 运行结果


![image](https://user-images.githubusercontent.com/75195549/181439356-40f47813-fe58-4b51-8262-dce452918ab4.png)


# 一秒做180w次计算

