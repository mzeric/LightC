# 一个简易的C语言编译器

ightC is a ANSI C compiler. implement with modern retargetable design

为了实践编译原理，解决实际实现中会遇到的问题；希望结构会清晰。

## Architecture
版本2.0 设计：
* 手写C的前端
  * 部分完成 例子见 [json parser](https://mzeric.github.io/compiler/2016/09/30/json-parser.html)
* AST 转换成中间代码后，立即进入SSA模式
  * 计算 出 CFG 的 dominance frontier 
  * 计算出 变量的liveness
  * 使用Cytron 算法构建SSA [cytron算法实现](https://github.com/mzeric/LightC/wiki/cytron)
* SSA的优化
* 基于 SSA的寄存器分配
* SSA转换成普通模式
* olive代码生成器



版本1.0 设计:
* 构造出AST后再生成中间代码，而不是前端一边parse一边生成
* 核心结构是 `typedef anode_node* anode;` 前端和中间代码均使用anode代表
* 构造AST时使用 c-tree.def 定义的节点类型，与GCC相同
* 构造AST时完成声明和符号表的处理，AST中的decl节点含有中间代码的类型信息
* 用anode实现基础的list等结构
* 在AST上建立 basic block，并对block进行线性化
* 对AST的代码进行化简，创建临时变量
* 将化简后的代码传输给后端的指令选择生成器
* 进行寄存器分配
* 输出汇编代码

## Grammar of C
declaration：一条声明语句，可以是变量声明、函数声明、for循环声明或其他复杂 声明

primary：包括变量、整数、字符串等 

declarator：声明中得变量，被声明对象

## Type subsystem
  
  当前运算只支持 32位整数，类型用结构 anode_type表示，记录在anode 节点的->type里
```c++
anode build_pointer_type(anode type)
{
        anode_type *t = new anode_type(POINTER_TYPE);
        ANODE_TYPE(t) = type;
        return t;
}

anode_int_cst *int_zero = new anode_int_cst(0);
ANODE_TYPE(int_zero) = new anode_type(INTEGER_TYPE);
```
## AST internal
three kind of AST: Type Decl Stmt

although could just use llvm’s Type system,but attemp approach another way

### SymbolTable
符号表由链表链接，确切的说是反向链接。
```c++
typedef struct basic_block_s{
        unsigned                index; /* used for goto expr */
        struct basic_block_s    *prev, *next; /* the chain */
        anode                   list;           /* use list to link stmt inside bb instead modify The AST */
        anode                   outer_loop; /* outer loop for break */
        anode                   entry, exit; /* first & last node of the block */
        anode                   lower_ir;
        struct edge_s           *pred, *succ; /* edges in / out of the block */
        anode                   decl_context; /* store the current_declspaces */
        anode                   decl_current;
        char                    *comment;
        ssa_table_t             *ssa_table;
        int                     status;

        anode                   phi;   /* all phi instructs linked by ->chain */
        phi_edge_t              *phi_edge;
        struct live_ness_t      *live;
        stmt_live_t             *stmt_live;

        std::list<anode_ins*>        *ins;

}basic_block_t,bb_t, *bb;
```
在创建basic_block的时候，每个basic_block就代表一个作用域，或者是在某一个作用域里，而不会在block内部出现嵌套的作用域。

basic_block->decl_context, ->decl_current 就像堆栈的两个 esp,ebp指针，有 push 往链表的底端加入符号和pop往上移动一个符号，2个操作
* 在 {} 的进入和退出时执行 push 和 pop，由词法解析器完成,symboltable 放在 com-
pound_statement 里
* function_definition结束时pop
* function proto 结束时push
* declaration in for loop时处理和函数处理相同
* declaration in block_item直接加入当前符号表
* declaration 同上 (此处是函数外的全局声明)
## SSA
c和lua两种实现，lua的SSA构建脚本：
```lua
function place_phi( cfg )
        -- body
        local all_node = cfg_nodes(cfg)
        local DF = DF_build(cfg)
        local PHI={}
        for node in pairs(all_node) do end
        for _,var in pairs(vars_need_phi(cfg)) do
                local w = defs_of_var(var)
                local orig = deepcopy(w)
                while(table.length(w)) do
                        local n = table.remove(w, 1) -- get the head element
                        for _,y in pairs(DF[n]) do
                                if not set_in(PHI[var], y) then
                                        -- insert phi at top of block[y]
                                        print ("inset phi in block "..y.." for var: "..var)
                                        table.insert(PHI[var],y) -- append
                                        if not set_in(orig, y) then
                                                table.insert(w, y)
                                        end
                                end
                        end
                end
        end
        return PHI

end
```
SSA消除脚本：
```lua
function topological_phi(graph, b)

    local tmp_count = 1
    local break_cycle = nil
    local break_next = nil
    while(table.length(b) > 0) do
            local n = find_next(b)
            if not n then
                    -- break the cycle with temp var
                    for ik, iv in pairs(b) do
                            break_cycle = ik
                            break
                    end

                    -- break
                    break_next = table.first(b[break_cycle])
                    out_mov(break_cycle, "T"..tmp_count)
                    reduce(b, break_next)

                    --n = find_next(b)
                    topological_phi(graph, b)
                out_mov("T"..tmp_count, break_next)
            end
            if (not graph[n]) then
                    -- n is last of the cluter
            else
                    out_mov(graph[n], n)
            end
            if n then
                    reduce(b, n)
            end
            
    end
    if(break_next) then
--          out_mov("T"..tmp_count, break_next)
    end

end
```
## Error 定位
前端Error会有错误定位，并用 `^` 下划标记

中间的语义分析可以给出简单的错误信息：未定义的变量等
