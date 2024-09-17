我完成了所有内容. \
还有声卡  $>omega<$\
咚  咚  咚 咚咚咚咚咚 咚 咚 咚咚咚

#line(length: 100%)
#align(center, text(17pt)[
  *必答题*
])

= YEMU
\
- *状态机* 

表示为(r0, r1, My, Mx, Mz)
$(?,?,33,16,0)->(33,?,33,16,0)->(33,33,33,16,0)->(16,33,33,16,0)->(49,33,33,16,0)->(49,33,33,16,49)$

- *如何执行*

$"取指" ->"解析操作码 "->"解析操作数" ->"执行与写入" ->"更新PC"$

= 指令执行过程
\
\# 已关闭所有trace\
内存读出指令与snpc修改 -> 解析指令 -> 解析imm与reg -> 执行与访存与dnpc修改 -> 更新PC

= 打字小游戏如何运行
\
- 状态机视角\
  按下一个字母按键后状态机的转移将会"分叉", 然后继续状态转移
- 抽象层视角
  - 程序: 计时, 读取键盘输入, 比较输入与目标字符, 更新画面进行渲染
  - 运行时环境: printf: 在控制台输出提示, rand生成随机数, ioread: 读取键盘输入, etc., iowrite: 输出到屏幕
  - AM: 访问 AM_INPUT_KEYBRD, AM_GPU_FBDRAW等寄存器对应的MMIO地址
  - ISA: 指令访问内存
  - NEMU: 按键通过SDL读取key并放入key_queue, 收到读取请求时从key_queue中取出放入对应MMIO地址; 根据vmem进行绘制

= 编译与链接
\
- static&inline
  - 去掉static: 没有任何影响 ???
  - 去掉inline: error: ‘inst_fetch’ defined but not used, 关掉Wall没有任何影响 ???
  - 全去掉:  multiple definition of 'inst_fetch'
  - 我的解释:
    - 有 inline 的函数会被编译器在调用处展开, 不存在符号表中, 所以不会有重复定义
    - static 函数只在当前文件中可见, 但是链接的时候连了过去, 所以也不会有重复定义, 且能正常调用
  - 证明: 查符号表

- volatile
  - n = (size_after - size_before) / sizeof(int) = (59120 - 57536)/4 = 396

= 了解Makefile


#line(length: 100%)

#align(center, text(17pt)[
  *新发现*
])

- 原来strdup是Linux特供, gnuc真神奇
- %prec以修改终结符的优先级
- yyparse不能改返回值类型, 还是传出参吧
- %parse-param不止给yyparse传参, 还给yyerror传参

#line(length: 100%)

#align(center, text(17pt)[
  *问题*
])

- 为什么watchpoint提供的框架搓了一个数组加链表的东西, 为什么不直接用数组?
  - 数组: 访问O(1), 删除O(1), 插入看删除的情况, 而且规模也就64甚至32

#line(length: 100%)

#align(center, text(17pt)[
  *思考题*
])

- 可以没有寄存器吧, 划分一块内存来用, 可能会慢(?)
- 从main, 对....对吧? 对吗? 哦对的对的对的.
- 字符串连接和替换, 但是yzh的宏魔法还是太神奇了
- kiss, 功能分割, 看起来更清晰
- 不是命令行? 不是命令行? 不是命令行?
- 回绕, -1即最大的uint; 不是ub啊, 回绕是明确定义的
- 模拟器是承载, 调试器是附加

#line(length: 100%)