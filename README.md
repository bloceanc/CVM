# CVM
CVM is a C language Virtual Machine like JVM.


## What is CVM
CVM is a 32-bit Virtual Machine like JVM witch used for C language.
It would support Linux and Windows and based on CPU0 architecture which is a special CPU for teching, for more infomation please refferer https://jonathan2251.github.io/lbd/llvmstructure.html.

## CVM Features
The CVM could run on Linux now, but only little file APIs are implemented(such as open,read,write,close ...).
Fortunately, it has built an syscall framework through an special virtual instruction -- 'SSC' which makes extending new system APIs very easily.

#### already supported
  * Elf file supported
  * Single thread supported
  * Little file syscalls supported
  * Complete virtual instructions supported
  * Simple memory manange
  * LLVM compiler, please refferer https://jonathan2251.github.io/lbd/llvmstructure.html
  * Interactive calling between Host and Virtual App
  * MD5 hash testing passed

#### todo(would supported)
  * Sockets for network
  * Full syscall APIs
  * Multi-threading
  * New memory schedule to make interactive calling between Host and Virtual App easily.
  * ...

## How to Use
CVM could be easily integrated with any apps/systems.
The file 'sim.c' in simulator path is an example to show how to use CVM.

#### how to run an VM instance



