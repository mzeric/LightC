是否生成 IR_BRANCH 通用代码？

优点：
	包含分支信息多，利于以后某种优化

缺点：
	不利于代码生成，BRAHC里无法包含是否是while信息，生成优化的while代码困难


改进1：生成 IR_IF_BRANCH 添加注释信息
	缺点：和IF_STMT没区别
改进2：生成 JMP + Goto 代码
	缺点：失去了高层一点的信息，不利于后续的优化，如何进行循环展开，但此操作在IR_BRANCH上同样无法进行
	优点：生成JMP代码效率高，如果涉及while优化的代码在lower之前进行的话，JMP可行
	
折中方案：
	生成JMP的同时，想办法保留while_stmt的结构
	参考gcc 循环优化的实现

if-then-else 代码生成

	cond
	if_jmpeq label_true
	[goto label_false]
	[label_false:]
	xxx

	goto if_end
label_true:
	xxx

if_end:
	


while 代码生成 优化：

goto label_cond

while_body:

	xxx

label_cond:
	cond
	if_jmpeq while_body

while 代码生成 直接：

label_cond:
 	cond
 	if_jmpeq label_body
 	goto label_end
label_body:
 	xxx
 	goto label_cond
label_end:

