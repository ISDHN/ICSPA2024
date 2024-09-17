我完成了所有内容.
还有声卡. >v<

#line(length: 100%)
#align(center, text(17pt)[
  *必答题*
])

= YEMU

== *状态机* 
\
表示为(r0, r1, My, Mx, Mz)
$(?,?,33,16,0)->(33,?,33,16,0)->(33,33,33,16,0)->(16,33,33,16,0)->(49,33,33,16,0)->(49,33,33,16,49)$

== 如何执行
\
$"取指" ->"解析操作码 "->"解析操作数" ->"执行与写入" ->"更新PC"$

= 指令执行过程
\
\# 已关闭所有trace\
内存读出指令与snpc修改 -> 解析指令 -> 解析imm与reg -> 执行与访存与dnpc修改 -> 更新PC

= 手册范围
- riscv
  + riscv32有哪几种指令格式? Volumn I, P23-25
  + LUI指令的行为是什么? Volumn I, P27
  + mstatus寄存器的结构是怎么样的? Volumn II, P25-26

= 代码统计
 All:  295231  #align(right, " #包括flexbison的编译结果")
 PA1前:  290354  \
 Method:
  - \#in pwsh:
  ```
    $lines = 0;
    foreach($f in find . -name "*.[c|h]"){
        $lines += [System.IO.File]::ReadAllLines($f).Length;
    }
    $lines
  ```
 无空行: 258247

= 开关作用

``` -Wall ``` 生成所有警告信息\
``` -Werror ``` 警告当作错误处理\
为了更加安全且容易发现错误

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