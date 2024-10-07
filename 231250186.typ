我完成了所有内容. \
还有音频播放, echo, etc.     \
信号机制锐意制作中\

#line(length: 100%)
#align(center, text(17pt)[
  *前言*
])

豪德, 我现在知道timer的坑在哪里了. \

#line(length: 100%)
#align(center, text(17pt)[
  *必答题*
])

= Context
\
- *在哪里 & 从哪来* 

即为当前栈指针, ` __am_asm_trap`开始的时候在当前栈上开了一个context的空间然后保存 gpr| mepc | mstatus | mcause 到栈上, 按照context的结构构造出一个context 结构体, 调用 `__am_irq_handle` 的时候把a0设置为sp 以传递context\*\

- *在哪赋值*

```yasm
  MAP(REGS, PUSH)

  csrr t0, mcause
  csrr t1, mstatus
  csrr t2, mepc

  STORE t0, OFFSET_CAUSE(sp)
  STORE t1, OFFSET_STATUS(sp)
  STORE t2, OFFSET_EPC(sp)
```

- *联系*

 - riscv.h 定义了一个context结构体的规范, trap.S和c同时遵守以实现数据沟通 \
 - 上述讲义: 告诉我要干啥 \
 - 新指令: 用来访问csr, 使csr的值能够被保存到context中 \
\
= Yield
\
 - ```c yield() ```: 把异常号 (-1) 放进a7, 然后发动 ```asm ecall``` \
 - ```asm ecall```: 发动 ```c isa_raise_intr ```, 传递异常号 \
 - ```c isa_raise_intr()```: 保存mepc, 设置mcause, 跳转到mtvec ( ```asm __am_asm_trap ```) \
 - ```asm __am_asm_trap ```: 保存context, 调用 ```c __am_irq_handle() ``` \
 - ```c __am_irq_handle() ```: 如果存在user_handle, 则根据mcause进行事件分发, 然后把事件和context传给``` user_handle ``` \
 - ``` user_handle ```: 发现是EVENT_YIELD, 打个y, 并返回原先context \
 - ```c __am_irq_handle() ```: 返回user_handle返回的context \
 - ```asm __am_asm_trap ```: 从context中恢复gpr, mepc, mstatus, mcause, 发动 ```asm mret``` \ 
 - ```asm mret```: 跳转到mepc的下一条指令, 返回原先执行流 \
  - \# 注: 我认为mepc应当是pc after exception, 而非 pc when exception. pa3中单纯的ecall回来后下一条指令一定是pc+4, 但是pa4中加入硬件中断后则不一定. eg. 在jmp | mret | .etc 后产生硬件中断, 保存 pc when exception 则丢弃了本来的下一条指令的信息. 但是保存 pc after exception 则可以保证下一条指令的信息不会丢失. \
\
= *Hello*
\
- 程序加载相关
 - 一开始在文件(ramdisk)里, 通过loader读到内存中. elf文件的头部有elf文件的组织结构, 指示了程序的不同segment应当加载到内存的哪个地址上, 第一条指令是Elf_Ehdr中的e_entry所指示的地址, 在c代码中通过函数调用以执行该第一条指令. \
\
- 字符输出相关
 - $"printf(格式字符串, 要格式化的值)" -> "write(stdout, 格式化完的字符串)" ->\ "_write(stdout, 格式化完的字符串)" -> "_syscall(系统调用号:SYS_write, 其他参数):" \
 "把调用号和参数放进寄存器, 执行ecall" -> "中断处理流程..." -> "do_event(事件,上下文):" \ "发现是事件是syscall" ->"do_syscall(上下文): 从上下文中捞出调用号和参数" -> \ "sys_write(stdout, 字符串)" -> "fs_write(stdout, 字符串)" -> "serial_write(字符串):" \ "逐字符输出" -> "putch(字符): 向串口mmio区域写入字符" -> "nemu执行写入发现是mmio" \ "区域, 写入对应位置后调用callback, 输出字符到终端" $

#line(length: 100%)

#align(center, text(17pt)[
  *槽点*
])

#align(center,
  box(width: 90%, stroke: gray, radius: 4pt,
    pad(
      text(12pt)[
      最近席卷计算机领域的, 就要数2018年1月爆出的Meltdown和Spectre这两个大名鼎鼎的硬件漏洞了.
      ],x: 16pt,y:4pt), fill: color.linear-rgb(200, 200, 200, 100)))

哥们你这有点古老啊, 不整整最近Linux的打印机RCE吗? \



#line(length: 100%)

#align(center, text(17pt)[
  *思考题*
])

CTE还可能切换进程, 返回别人的context, 所以要保存全部reg \
CISC简化了软件设计, RISC简化了硬件设计, 给了软件自由度. 合不合理? 看当时环境! \
堆和栈是操作系统分配的, 放到文件内会固化可使用的内存空间且使文件巨大 \
头上的magic number不一样 \
有的数据不需要包含在文件内, 但是载入内存的时候要分配空间(eg. .bss), 清零是为了避免读到别人用的脏数据 \
会产生资源竞争和状态不一致(eg. malloc维护的内存分配要OS统一管理) \
a0是函数的参数传递, 用a7是为了避免和既有约定冲突 (? \
/dev/events不可定位, 所以用open \
float to fixedpt: 把对应bits捞出来, 按IEEE754标准转换成fixedpt \
LD_PRELOAD: 优先加载动态链接库 \
秘技: 笑死, uint aroundwarp了 \
唉唉, 代码混淆. 最简单方案: 关键字算hash然后替换