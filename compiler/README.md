The compiler has been made in bin.
For more about CPU0-LLVM, please refer to https://jonathan2251.github.io/lbd/llvmstructure.html

The SSC instruction defined as follow:
```
./Cpu0InstrInfo.td:852:def SSC     : Syscall<0x53, "ssc", GPROut, uimm16>;
```
