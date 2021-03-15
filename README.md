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
  * Create new VM instance.The function 'vm_create' would create a new machine instance using the specified parammeters and return the instance.
   ```
      vm_create("./lic",         /* ELF file to run */
                4 * 1024 * 1024, /* Total memory used to run CVM */
                1024             /* the stack size for thread */
      );
   ```
  * Load the symbol into the instance. Using 'elf_resolve' to resolve the ELF and get the symbol ID.
  ```
      elf_resolve(pvm,           /* CVM instance */
                  "__init"       /* Symbol name  */
      );
  ```
  * Setup the main thread of vm.
  ```
      vm_create_main_thread(pvm,   /* CVM instance */
                            symbol /* the function to run by main thread */
      );
  ```
  * Run the CVM.
  ```
      vm_start(pvm,   /* CVM instance */
               0      /* Thread id, using 0 as only one thread supported now */
      );
  ```
#### how to setup parammeters for symbol to run
  We can use vm_push_main_thread_param to do this.
  ```
      vm_push_main_thread_param(pvm, /* CVM instance */
                                0    /* parammeter */
      );
  ```
  Remember, this function would auto push the parammeter, and **CANNOT** specify the parammeter index which would be calculated by the calling sequence.
  
#### how to get the return value of symbol in host
  The following two functions would help u.
  ```
  extern u32_t abi_func_get_return_int(vm_t *vm, u32_t tid);   /* get return value as UINT32 */
  extern void* abi_func_get_return_addr(vm_t *vm, u32_t tid);  /* get return value as pointer */
  ```
