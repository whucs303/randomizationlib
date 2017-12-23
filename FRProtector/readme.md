## FRPROTECTOR
本开源项目为securecomm2017的会议论文。论文核心观点为：将函数级随机化与控制流完整性保护相结合，提出一种新型抵抗代码复用攻击的方法。当前版本在ubuntu 16.04 32bit下测试通过。理论上可以支持任何llvm支持的操作系统，建议使用linux。
## 安装方式
参考llvm3.9.1官方说明文档
需要llvm的依赖环境
## 使用方式
假设需要保护的代码为a.cpp, 
```
llvm_root/bin/clang -o0 -fno-stack-protector -emit-llvm a.cpp -c -o a.bc
llvm_root/bin/opt -load llvm_root/lib/LLVMFPR.so -FRProtector <a.bc> a_frp.bc
llvm_root/bin/clang a_frp.bc -o a.out
./a.out
```


