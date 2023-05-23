#include <cassert>
#include <cstdio>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <string>
#include <vector>
#include <map>
#include "back_main.hpp"
using namespace std;

#define cout fout

ofstream fout;

void back_main(const char input[], const char output[]){
    // freopen(output, "w", stdout);

    // 从input中读取IR树
    ifstream fin(input);
    std::istreambuf_iterator<char> beg(fin), end;
    std::string IRTree(beg, end);
    fout = ofstream(output);


    // 解析KoopaIR
    GetKoopaIR(IRTree.c_str());
}

// 从文本IR中解析KoopaIR, 生成raw program
// 通过Visit(raw program), 得到最后的RISCV指令
void GetKoopaIR(const char str[]){
    // 解析字符串 str, 得到 Koopa IR 程序
    koopa_program_t program;
    koopa_error_code_t ret = koopa_parse_from_string(str, &program);
    assert(ret == KOOPA_EC_SUCCESS);  // 确保解析时没有出错
    
    // 创建一个 raw program builder, 用来构建 raw program
    koopa_raw_program_builder_t builder = koopa_new_raw_program_builder();
    
    // 将 Koopa IR 程序转换为 raw program
    koopa_raw_program_t raw = koopa_build_raw_program(builder, program);
    
    // 释放 Koopa IR 程序占用的内存
    koopa_delete_program(program);

    // 处理 raw program
    Visit_Program(raw);
    

    // 处理完成, 释放 raw program builder 占用的内存
    // 注意, raw program 中所有的指针指向的内存均为 raw program builder 的内存
    // 所以不要在 raw program 处理完毕之前释放 builder
    koopa_delete_raw_program_builder(builder);
}

// 访问 raw program
void Visit_Program(const koopa_raw_program_t &program) {
    // printf("-----------Visit_Program---------------\n");

    // 访问所有全局变量
    Visit_Slice(program.values);

    // 执行一些其他的必要操作
    
    // 访问所有函数
    Visit_Slice(program.funcs);
}

// 访问 raw slice
void Visit_Slice(const koopa_raw_slice_t &slice) {
    // printf("-----------Visit_Slice---------------\n");

    for (size_t i = 0; i < slice.len; i++) {
        // 当前slice的内容
        auto ptr = slice.buffer[i];
        
        // 当前slice的类型
        switch (slice.kind) {
            // 当前slice的类型为function
            case KOOPA_RSIK_FUNCTION:{
                Visit_Function(reinterpret_cast<koopa_raw_function_t>(ptr));
                break;
            }
            
            // 当前slice的类型为basic_block
            case KOOPA_RSIK_BASIC_BLOCK:{
                Visit_Basic_Block(reinterpret_cast<koopa_raw_basic_block_t>(ptr));
                break;
            }
            
            // 当前slice的类型为value(即指令)
            case KOOPA_RSIK_VALUE:{
                Visit_Inst(reinterpret_cast<koopa_raw_value_t>(ptr));
                break;
            }
            
            // 其他情况
            default:{
                printf("[Visit_Slice]: slice.kind = %d\n", slice.kind);
                assert(false);
            }
        }
    }
}


/*====================  函数部分 =======================*/ 
// 当前函数已经使用的栈的大小(单位: 字节)
int32_t use_stack = 0;

// 访问函数
void Visit_Function(const koopa_raw_function_t &func) {
    // printf("-----------Visit_Function---------------\n");

    // 如果是函数声明, 则对应bbs.len为0, 应该跳过
    if(func->bbs.len == 0) return;
    
    // 清空当前函数所用栈的大小
    use_stack = 0;

    // 输出当前函数的函数名, 标记当前函数的入口
    // 由于KoopaIR中函数名均为@name, 因此只需要输出name+1即可
    cout << "\n";
    cout << "\t.text\n";                            // 声明之后的数据需要被放入代码段中
    cout << "\t.globl "<< func->name+1 << "\n";     // 将当前函数声明为全局函数, 以便链接器处理
    cout << func->name+1 << ":\n";

    // 计算当前函数指令可能用到的栈空间大小
    int32_t need_stack = 4; // 默认保留ra的值
    for (size_t i = 0; i < func->bbs.len; ++i) {
        // 当前func->bbs的内容
        auto ptr = func->bbs.buffer[i];
        // 计算该bbs需要的栈空间大小
        need_stack += Get_Basic_Block_Need_Stack(reinterpret_cast<koopa_raw_basic_block_t>(ptr));
    }
    // 开辟栈空间, 保留ra的值到栈的最底部
    cout << "\taddi sp, sp, -" << need_stack << "\n";
    cout << "\tsw   ra, " << need_stack-4 << "(sp)\n";

    // 访问当前函数的所有参数
    // koopa_raw_slice_t params, 需要通过Slice进行进一步划分
    // Visit_Slice(func->params);

    // 访问当前函数的所有基本块
    // koopa_raw_slice_t bbs, 需要通过Slice进行进一步划分
    Visit_Slice(func->bbs);

    // 判断当前函数的返回值
    // koopa_raw_type_t Return_Type = func->ty
}

// 获取数组的长度
int32_t Get_Array_Len(const struct koopa_raw_type_kind* base){
    // 是一个指针, 找到指针对应的类型进行计算
    if(base->tag == KOOPA_RTT_POINTER){
        base = base->data.pointer.base;
        // 通过指针获取数组长度, 目前是要去掉最外面的一维
        if(base->tag == KOOPA_RTT_ARRAY){
            base = base->data.array.base;
        }
    }
    int32_t len = 1;
    // 多维数组
    while(base->tag == KOOPA_RTT_ARRAY){
        len *= base->data.array.len;
        base = base->data.array.base;
    }
    return len;
}

// 获取指针指向的内容的基础长度
int32_t Get_Pointer_Len(const struct koopa_raw_type_kind* base){
    // 是一个指针, 找到指针对应的类型进行计算
    if(base->tag == KOOPA_RTT_POINTER){
        base = base->data.pointer.base;
        return Get_Array_Len(base);
    }else{
        printf("[Get_Pointer_Len] base is not a pointer\n");
        assert(0);
    }
}

// 遍历当前函数的所有指令, 计算当前函数可能用到的栈空间大小
int32_t Get_Basic_Block_Need_Stack(const koopa_raw_basic_block_t &bbs){
    int32_t need_stack = 0;
    for (size_t i = 0; i < bbs->insts.len; ++i) {
        // 当前bb->insts的内容
        auto ptr = bbs->insts.buffer[i];
        // 计算该inst需要的栈空间大小
        koopa_raw_value_t value = reinterpret_cast<koopa_raw_value_t>(ptr);
        switch (value->kind.tag) {
            // 当前指令为访问 alloc 指令 (tag = 6)
            case KOOPA_RVT_ALLOC:{
                koopa_raw_type_t alloc_type = value->ty;
                // 判断alloc的类型, need_stack增加不同的值
                switch(alloc_type->tag){
                    // alloc申请了一个指针的地方, 那么就需要指针的类型是什么
                    case KOOPA_RTT_POINTER:{
                        const struct koopa_raw_type_kind* base = alloc_type->data.pointer.base;
                        if(base->tag == KOOPA_RTT_INT32){
                            // int32, 需要的空间为4
                            need_stack += 4;
                        }
                        else if(base->tag == KOOPA_RTT_ARRAY){
                            // 数组, 默认为int32数组, 需要的空间为4*len
                            int32_t len = Get_Array_Len(base);
                            need_stack += len * 4;
                        } else if(base->tag == KOOPA_RTT_POINTER){
                            // 指针, 即指向数组的指针
                            need_stack += 4;
                        } else{
                            printf("Get_Basic_Block_Need_Stack:[No.%zu] base.tag = %d\n", i, base->tag);
                            assert(false);
                        }
                        break;
                    }
                    default:{
                        printf("Visit_Inst_Alloc: type = %d\n", alloc_type->tag);
                        assert(false);
                    }
                }
                break;
            }
            // 访问 load 指令 (tag = 8)
            case KOOPA_RVT_LOAD: { need_stack += 4; break; }   
            // 访问 get_pointer 指令 (tag = 10)
            case KOOPA_RVT_GET_PTR: { need_stack += 4; break; } 
            // 访问 element_pointer 指令 (tag = 11)
            case KOOPA_RVT_GET_ELEM_PTR: { need_stack += 4; break; }  
            // 访问 binary 指令 (tag = 12)
            case KOOPA_RVT_BINARY:{ need_stack += 4; break; }
            // 访问 call 指令 (tag = 15)
            case KOOPA_RVT_CALL:{ 
                const koopa_raw_call_t &call = value->kind.data.call;
                koopa_raw_function_t callee = call.callee;
                const struct koopa_raw_type_kind * ret = callee->ty->data.function.ret;
                koopa_raw_type_tag_t ret_type = ret->tag;
                
                if(ret_type == KOOPA_RTT_INT32){
                    // 返回值为int32, 则需要将其保留到栈中
                    need_stack += 4; 
                } else if(ret_type == KOOPA_RTT_UNIT){
                    // 返回值为void, 不需要将其保留到栈中
                    need_stack += 0;
                } else{
                    // 其他类型
                    printf("[Get_Basic_Block_Need_Stack] call return type = %d\n", ret_type);
                    assert(0);
                }
                break; 
            }       
            // 其他类型
            default:{ break; }
        }
    }
    return need_stack;
}

// 访问基本块
void Visit_Basic_Block(const koopa_raw_basic_block_t &bb) {
    // printf("-----------Visit_Basic_Block---------------\n");
    
    // 输出当前基本块的名成, 标记当前基本块的入口
    // 由于KoopaIR中基本块均为@name, 因此只需要输出name+1即可
    cout << bb->name+1 << ":\n";  // 标记 基本块 的入口点
    
    // 访问当前基本块的所有参数
    // koopa_raw_slice_t params, 需要通过Slice进行进一步划分
    // Visit_Slice(bb->params)

    // 访问当前基本块的所有用到的value
    // koopa_raw_slice_t used_by, 需要通过Slice进行进一步划分
    // Visit_Slice(bb->used_by) 

    // 访问所有指令
    // koopa_raw_slice_t insts, 需要通过Slice进行进一步划分
    Visit_Slice(bb->insts);
}



/*====================  指令部分 =======================*/ 
// 指令 => 在内存中的位置, 即sp+?
std::map<koopa_raw_value_t, int32_t> inst_to_index;

// 访问指令
int32_t Visit_Inst(const koopa_raw_value_t &value) {
    if(inst_to_index.find(value) != inst_to_index.end()) return inst_to_index[value];

    // printf("-----------Visit_Inst, addr = %llx---------------\n", (unsigned long long)value);

    // 根据指令类型判断后续需要如何访问
    const auto &kind = value->kind;
    switch (kind.tag) {
        // 访问 integer 指令 (tag = 0)
        case KOOPA_RVT_INTEGER:{
            return inst_to_index[value] = Visit_Inst_Integer(kind.data.integer);
        }
        // 访问 aggregate 指令 (tag = 3)
        case KOOPA_RVT_AGGREGATE:{
            return inst_to_index[value] = Visit_Inst_Aggregate(kind.data.aggregate);
        }
        // 访问 func_arg_ref 指令 (tag = 4)
        case KOOPA_RVT_FUNC_ARG_REF:{
            return inst_to_index[value] = Visit_Inst_Func_Arg_Ref(kind.data.func_arg_ref);
        }
        // 访问 alloc 指令 (tag = 6)
        case KOOPA_RVT_ALLOC:{
            return inst_to_index[value] = Visit_Inst_Alloc(value->ty);
        }
        // 访问 global_alloc 指令 (tag = 7)
        case KOOPA_RVT_GLOBAL_ALLOC:{
            return inst_to_index[value] = Visit_Inst_Global_Alloc(kind.data.global_alloc, value->name+1);
        }
        // 访问 load 指令 (tag = 8)
        case KOOPA_RVT_LOAD:{
            return inst_to_index[value] = Visit_Inst_Load(kind.data.load);
        }
        // 访问 store 指令 (tag = 9)
        case KOOPA_RVT_STORE:{
            return inst_to_index[value] = Visit_Inst_Store(kind.data.store);
        }
        // 访问 get_pointer 指令 (tag = 10)
        case KOOPA_RVT_GET_PTR:{
            return inst_to_index[value] = Visit_Inst_Get_Ptr(kind.data.get_ptr);
        }
        // 访问 element_pointer 指令 (tag = 11)
        case KOOPA_RVT_GET_ELEM_PTR:{
            return inst_to_index[value] = Visit_Inst_Elem_Ptr(kind.data.get_elem_ptr);
        }
        // 访问 binary 指令 (tag = 12)
        case KOOPA_RVT_BINARY:{
            return inst_to_index[value] = Visit_Inst_Binary(kind.data.binary);
        }
        // 访问 branch 指令 (tag = 13)
        case KOOPA_RVT_BRANCH:{
            return inst_to_index[value] = Visit_Inst_Branch(kind.data.branch);
        }
        // 访问 jump 指令 (tag = 14)
        case KOOPA_RVT_JUMP:{
            return inst_to_index[value] = Visit_Inst_Jump(kind.data.jump);
        }
        // 访问 call 指令 (tag = 15)
        case KOOPA_RVT_CALL:{
            return inst_to_index[value] = Visit_Inst_Call(kind.data.call);
        }
        // 访问 return 指令 (tag = 16)
        case KOOPA_RVT_RETURN:{
            return inst_to_index[value] = Visit_Inst_Return(kind.data.ret);
        }
        
        // 其他类型
        default:{
            printf("Visit_Inst kind.tag = %d\n", kind.tag);
            assert(false);
        }
    }
}

// 访问 integer 指令, 返回整数值 (tag = 0)
int32_t Visit_Inst_Integer(const koopa_raw_integer_t &integer){
    // printf("-----------Visit_Inst_Integer-----------\n");
    
    return integer.value;
}

// 访问 aggregate 指令, 进行数组的初始化操作, 返回栈的起始地址 (tag = 3)
int32_t Visit_Inst_Aggregate(const koopa_raw_aggregate_t &aggregate){
    // printf("-----------Visit_Inst_Aggregate-----------\n");
    
    int32_t now_stack = use_stack;
    // 取出 aggregate 中的元素
    koopa_raw_slice_t elems = aggregate.elems;
    // 遍历elems中的每一个元素
    for(int i = 0; i < elems.len; i++){
        // 当前elems的内容
        auto ptr = elems.buffer[i];
        
        // 当前slice的类型必须为value语句
        if (elems.kind == KOOPA_RSIK_VALUE){
            koopa_raw_value_t value = reinterpret_cast<koopa_raw_value_t>(ptr);
            koopa_raw_value_kind_t kind = value->kind;
            if(kind.tag == KOOPA_RVT_INTEGER){
                // 用 integer 进行初始化
                cout << "\t.word " << Visit_Inst_Integer(kind.data.integer) << "\n";
                use_stack += 4;
            } else if(kind.tag == KOOPA_RVT_AGGREGATE){
                // 用 aggregate进行初始化, 是多维数组
                Visit_Inst_Aggregate(kind.data.aggregate);
            } else{
                printf("[Visit_Inst_Aggregate] kind.tag = %d\n", kind.tag);
                assert(0);
            }
        } else {
            printf("[Visit_Inst_Aggregate] elems.kind = %d\n", elems.kind);
            assert(0);
        }
    }
    return now_stack;
}

// 访问 func_arg_ref 指令, 返回是第x个参数 (tag = 4)
int32_t Visit_Inst_Func_Arg_Ref(const koopa_raw_func_arg_ref_t &func_arg_ref){
    // printf("-----------Visit_Inst_Func_Arg_Ref-----------\n");
    
    if(func_arg_ref.index >= 8){
        printf("[Visit_Inst_Func_Arg_Ref] index >= 8\n");
        assert(0);
    }

    return func_arg_ref.index;
}

// 访问 alloc 指令, 返回结果所在的sp+x (tag = 6)
int32_t Visit_Inst_Alloc(const koopa_raw_type_t &alloc_type){
    // printf("-----------Visit_Inst_Alloc: type = %d-----------\n", alloc_type->tag);
    
    switch(alloc_type -> tag){
        // alloc申请了一个指针的地方, 那么就需要指针的类型是什么
        case KOOPA_RTT_POINTER:{
            const struct koopa_raw_type_kind* base = alloc_type->data.pointer.base;
            // 由于是局部空间, 则只需要将栈指针后移, 不需要额外的操作
            if(base->tag == KOOPA_RTT_INT32){
                // int32, 占用空间 4
                int32_t now_stack = use_stack;
                use_stack += 4;
                return now_stack;
            } else if(base->tag == KOOPA_RTT_ARRAY){
                // 变量为int32数组, 占用空间 4*len
                int32_t len = Get_Array_Len(base);
                int32_t now_stack = use_stack;
                use_stack += len * 4;
                return now_stack;
            } else if(base->tag == KOOPA_RTT_POINTER){
                // 指针, 占用空间 4
                int32_t now_stack = use_stack;
                use_stack += 4;
                return now_stack;
            } else{
                printf("Visit_Inst_Alloc: base.tag = %d\n", base->tag);
                assert(false);
            }
        }

        default:{
            printf("Visit_Inst_Alloc: type = %d\n", alloc_type->tag);
            assert(false);
        }
    }
}

// 访问 global_alloc 指令 (tag = 7)
int32_t Visit_Inst_Global_Alloc(const koopa_raw_global_alloc_t &global_alloc, const char* name){
    // printf("----------- Visit_Inst_Global_Alloc -----------\n");

    cout << "\t.data\n";
    koopa_raw_value_t init = global_alloc.init;

    // 变量的类型
    koopa_raw_type_t ty = init->ty;

    // 变量的名称
    cout << "\t.global " << name << "\n";
    cout << name << ":\n";
    
    // 初始化的值
    koopa_raw_value_kind_t kind = init->kind;
    switch(kind.tag){
        // 通过一个整数初始化
        case KOOPA_RVT_INTEGER:{
            cout << "\t.word " << Visit_Inst_Integer(kind.data.integer) << "\n";
            break;
        }
        // 通过Zero initializer初始化
        case KOOPA_RVT_ZERO_INIT:{
            if(ty->tag == KOOPA_RTT_INT32){
                // 变量为int32类型
                cout << "\t.zero 4" << "\n";
            }else if(ty->tag == KOOPA_RTT_ARRAY){
                // 变量为int32数组
                int32_t len = Get_Array_Len(ty);
                cout << "\t.zero " << 4 * len << "\n";
            } else{
                printf("[Visit_Inst_Global_Alloc] ty->tag = %d\n", ty->tag);
                assert(0);
            }
            break; 
        }
        // 通过 aggregate 初始化
        case KOOPA_RVT_AGGREGATE:{
            Visit_Inst(init);
            break;
        }
        // 其他情况
        default:{
            printf("[Visit_Inst_Global_Alloc] kind.tag = %d\n", kind.tag);
            assert(0);
        }
    }
    cout << "\n";
    return 0;    
}

// 访问 load 指令, 返回结果所在的sp+x (tag = 8)
int32_t Visit_Inst_Load(const koopa_raw_load_t &load){
    // printf("-----------Visit_Inst_Load-----------\n");

    // 先将地址对应的值读取到t0中
    koopa_raw_value_t src = load.src;
    if(src->kind.tag == KOOPA_RVT_GLOBAL_ALLOC){
        // 全局变量的地址
        cout << "\tla   t0, " << src->name+1 << "\n";
        cout << "\tlw   t0, 0(t0)\n";
    } else if(src->kind.tag == KOOPA_RVT_ALLOC){
        // 局部变量的地址
        cout << "\tlw   t0, " << Visit_Inst(src) << "(sp)\n";
    } else if(src->kind.tag == KOOPA_RVT_GET_PTR){
        // 指针指向的地址
        cout << "\tlw   t0, " << Visit_Inst(src) << "(sp)\n";
        cout << "\tlw   t0, 0(t0)\n";
    } else if(src->kind.tag == KOOPA_RVT_GET_ELEM_PTR){
        // 数组
        cout << "\tlw   t1, " << Visit_Inst(src) << "(sp)\n";
        cout << "\tlw   t0, 0(t1)\n";
    } else{
        printf("[Visit_Inst_Load] src->kind.tag = %d\n", src->kind.tag);
        assert(0);
    }
    
    // 再将t0存入内存中
    cout << "\tsw   t0, " << use_stack << "(sp)\n";
    cout << "\n";
    use_stack += 4;
    return use_stack - 4;
}

// 访问 store 指令 (tag = 9)
int32_t Visit_Inst_Store(const koopa_raw_store_t &store){
    // printf("-----------Visit_Inst_Store-----------\n");

    koopa_raw_value_t value = store.value;
    koopa_raw_value_t dest = store.dest;

    // 将value的值存到t0中
    if(value->kind.tag == KOOPA_RVT_INTEGER){
        // value为整数指令
        cout << "\tli   t0, " << Visit_Inst_Integer(value->kind.data.integer) << "\n"; 
    } else if(value->kind.tag == KOOPA_RVT_FUNC_ARG_REF){
        // value为函数参数
        cout << "\tmv   t0, a" << Visit_Inst_Func_Arg_Ref(value->kind.data.func_arg_ref) << "\n"; 
    } else{
        // value在内存中, 将其lw到t0中
        cout << "\tlw   t0, " << Visit_Inst(value) << "(sp)\n";
    }

    // 将 t0 存到 dest 的位置
    if(dest->kind.tag == KOOPA_RVT_GLOBAL_ALLOC){
        // 全局变量的地址
        cout << "\tla   t0, " << dest->name+1 << "\n";
        cout << "\tsw   t0, 0(t0)\n";
    } else if(dest->kind.tag == KOOPA_RVT_ALLOC){
        // 局部变量的地址
        cout << "\tsw   t0, " << Visit_Inst(dest) << "(sp)\n";
    } else if(dest->kind.tag == KOOPA_RVT_GET_PTR){
        // 指针指向的地址
        cout << "\tlw   t0, " << Visit_Inst(dest) << "(sp)\n";
        cout << "\tsw   t0, 0(t0)\n";
    } else if(dest->kind.tag == KOOPA_RVT_GET_ELEM_PTR){
        // 数组
        cout << "\tlw   t1, " << Visit_Inst(dest) << "(sp)\n";
        cout << "\tsw   t0, 0(t1)\n";
    } else{
        printf("[Visit_Inst_Store] dest->kind.tag = %d\n", dest->kind.tag);
        assert(0);
    }
    cout << "\n";
    return 0;
}

// 访问 get_pointer 指令, 返回结果所在的sp+x  (tag = 10)
int32_t Visit_Inst_Get_Ptr(const koopa_raw_get_ptr_t &get_ptr){
    // printf("-----------Visit_Inst_Get_Ptr-----------\n");

	koopa_raw_value_t src = get_ptr.src;        // src是指针类型
	koopa_raw_value_t index = get_ptr.index;    // index是int32

    // 计算指针指向的类型的大小
    int32_t len = Get_Pointer_Len(src->ty);

    // src
    // 判断指针指向的是谁
    if(src->kind.tag == KOOPA_RVT_GLOBAL_ALLOC){
        // 全局数组
        cout << "\tla   t0, " << src->name+1 << "\n";
    } else if(src->kind.tag == KOOPA_RVT_ALLOC){
        // 局部数组
        cout << "\taddi t0, sp, " << Visit_Inst(src) << "\n";
    } else{
        // 局部变量
        cout << "\tlw   t0, " << Visit_Inst(src) << "(sp)\n";
        // printf("[Visit_Inst_Elem_Ptr] src->kind.tag = %d", src->kind.tag);
        // assert(0);
    }
    
    // index * len
    cout << "\tli   t1, " << Visit_Inst(index) << "\n";
    cout << "\tli   t2, " << len * 4 << "\n";
    cout << "\tmul  t1, t1, t2\n";
    // get_ptr的结果为: src + index * len
    cout << "\tadd  t0, t0, t1\n";
    // 再将t0存入内存中
    cout << "\tsw   t0, " << use_stack << "(sp)\n";
    cout << "\n";
    use_stack += 4;
    return use_stack - 4;
}

// 访问 element_pointer 指令 (tag = 11)
int32_t Visit_Inst_Elem_Ptr(const koopa_raw_get_elem_ptr_t &get_elem_ptr){
    // printf("-----------Visit_Inst_Elem_Ptr-----------\n");

    koopa_raw_value_t src = get_elem_ptr.src;
	koopa_raw_value_t index = get_elem_ptr.index;

    // 计算数组的地址
    if(src->kind.tag == KOOPA_RVT_GLOBAL_ALLOC){
        // 全局数组
        cout << "\tla   t0, " << src->name+1 << "\n";
    } else if(src->kind.tag == KOOPA_RVT_ALLOC){
        // 局部数组
        cout << "\taddi t0, sp, " << Visit_Inst(src) << "\n";
    } else{
        printf("[Visit_Inst_Elem_Ptr] src->kind.tag = %d", src->kind.tag);
        assert(0);
    }


    // 计算 get_elemptr 的偏移量
    cout << "\tli   t1, " << Visit_Inst(index) << "\n";
    cout << "\tli   t2, " << Get_Array_Len(src->ty) * 4 << "\n";
    cout << "\tmul  t1, t1, t2\n";
    // 计算 get_elemptr 的结果, 是一个指针
    cout << "\tadd  t0, t0, t1\n";
    // 再将t0存入内存中
    cout << "\tsw   t0, " << use_stack << "(sp)\n";
    cout << "\n";
    use_stack += 4;
    return use_stack - 4;
}

// 访问 binary 指令, 返回结果所在的sp+x (tag = 12)
int32_t Visit_Inst_Binary(const koopa_raw_binary_t &binary){
    // printf("-----------Visit_Inst_Binary, op = %d---------------\n", binary.op);

    // 取出两个操作数
    koopa_raw_value_t lhs = binary.lhs;
    koopa_raw_value_t rhs = binary.rhs;

    // 将lhs的值保存在t0中
    if(lhs->kind.tag == KOOPA_RVT_INTEGER){
        // lhs是整数指令
        cout << "\tli   t0, " << Visit_Inst_Integer(lhs->kind.data.integer) << "\n";
    } else if(lhs->kind.tag == KOOPA_RVT_FUNC_ARG_REF){
        // lhs是函数参数
        cout << "\tmv   t0, a" << Visit_Inst_Func_Arg_Ref(lhs->kind.data.func_arg_ref) << "\n";
    } else{
        // 不是整数指令, 则变量一定在内存中
        cout << "\tlw   t0, " << Visit_Inst(lhs) << "(sp)\n";
    }
    // 将rhs的值保存在t1中
    if(rhs->kind.tag == KOOPA_RVT_INTEGER){
        // rhs是整数指令
        cout << "\tli   t1, " << Visit_Inst_Integer(rhs->kind.data.integer) << "\n";
    } else if(rhs->kind.tag == KOOPA_RVT_FUNC_ARG_REF){
        // rhs是函数参数
        cout << "\tmv   t1, a" << Visit_Inst_Func_Arg_Ref(rhs->kind.data.func_arg_ref) << "\n";
    } else{
        // 不是整数指令, 则变量一定在内存中
        cout << "\tlw   t1, " << Visit_Inst(rhs) << "(sp)\n";
    }


    // 根据op判断是哪一个操作, 计算结果保存在t0中
    switch (binary.op){
        // lhs != rhs
        case KOOPA_RBO_NOT_EQ:{
            // 通过sub后与0判相等, 模拟!=操作
            cout << "\tsub  t0, t0, t1\n";
            cout << "\tsnez t0, t0\n";
            cout << "\tandi t0, t0, 0xff\n";
            break;
        }
        // lhs == rhs
        case KOOPA_RBO_EQ:{
            // 通过sub后与0判相等, 模拟==操作
            cout << "\tsub  t0, t0, t1\n";
            cout << "\tseqz t0, t0\n";
            cout << "\tandi t0, t0, 0xff\n";
            break;
        }
        // lhs > rhs
        case KOOPA_RBO_GT:{
            cout << "\tsgt  t0, t0, t1\n";
            cout << "\tandi t0, t0, 0xff\n";
            break;
        }
        // lhs < rhs
        case KOOPA_RBO_LT:{
            cout << "\tslt  t0, t0, t1\n";
            cout << "\tandi t0, t0, 0xff\n";
            break;
        }
        // lhs >= rhs
        case KOOPA_RBO_GE:{
            cout << "\tslt  t0, t0, t1\n";
            cout << "\txori t0, t0, 1\n";
            cout << "\tandi t0, t0, 0xff\n";
            break;
        }
        // lhs <= rhs
        case KOOPA_RBO_LE:{
            cout << "\tsgt  t0, t0, t1\n";
            cout << "\txori t0, t0, 1\n";
            cout << "\tandi t0, t0, 0xff\n";
            break;
        }
        // lhs + rhs
        case KOOPA_RBO_ADD:{
            cout << "\tadd  t0, t0, t1\n";
            break;
        }
        // lhs - rhs
        case KOOPA_RBO_SUB:{
            cout << "\tsub  t0, t0, t1\n";
            break;
        }
        // lhs * rhs
        case KOOPA_RBO_MUL:{
            cout << "\tmul  t0, t0, t1\n";
            break;
        }
        // lhs / rhs
        case KOOPA_RBO_DIV:{
            cout << "\tdiv  t0, t0, t1\n";
            break;
        }
        // lhs % rhs
        case KOOPA_RBO_MOD:{
            cout << "\trem  t0, t0, t1\n";
            break;
        }
        // lhs & rhs
        case KOOPA_RBO_AND:{
            cout << "\tand  t0, t0, t1\n";
            break;
        }
        // lhs | rhs
        case KOOPA_RBO_OR:{
            cout << "\tor   t0, t0, t1\n";
            break;
        }
        // lhs ^ rhs
        case KOOPA_RBO_XOR:{
            cout << "\txor  t0, t0, t1\n";
            break;
        }
        // lhs << rhs
        case KOOPA_RBO_SHL:{
            cout << "\tsll  t0, t0, t1\n";
            break;
        }
        // lhs >> rhs
        case KOOPA_RBO_SHR:{
            cout << "\tsra  t0, t0, t1\n";
            break;
        }
        // 其他情况
        default:{
            printf("Visit_Inst_Binary binary.op = %d\n", binary.op);
            assert(false);
        }
    }

    // 再将t0存入内存中
    cout << "\tsw   t0, " << use_stack << "(sp)\n";
    cout << "\n";
    use_stack += 4;
    return use_stack - 4;
}

// 访问 branch 指令 (tag = 13)
int32_t Visit_Inst_Branch(const koopa_raw_branch_t &branch){
    // printf("-----------Visit_Inst_Branch-----------\n");
    
    koopa_raw_value_t cond = branch.cond;
    koopa_raw_basic_block_t true_bb = branch.true_bb;
    koopa_raw_basic_block_t false_bb = branch.false_bb;
    // koopa_raw_slice_t true_args = branch.true_args;
	// koopa_raw_slice_t false_args = branch.false_args;

    // 取出条件对应的值, 存到t0中
    if(cond->kind.tag == KOOPA_RVT_INTEGER){
        // cond是整数指令
        cout << "\tli   t0, " << Visit_Inst_Integer(cond->kind.data.integer) << "\n";
    } else if(cond->kind.tag == KOOPA_RVT_FUNC_ARG_REF){
        // cond是函数参数
        cout << "\tmv   t0, a" << Visit_Inst_Func_Arg_Ref(cond->kind.data.func_arg_ref) << "\n"; 
    } else{
        // 不是整数指令, 则变量一定在内存中
        cout << "\tlw   t0, " << Visit_Inst(cond) << "(sp)\n";
    }

    // 输出条件跳转语句
    cout << "\tbnez t0, " << true_bb->name + 1 << "\n";
    cout << "\tj    " << false_bb->name + 1 << "\n";
    cout << "\n";
    return 0;
}

// 访问 jump 指令 (tag = 14)
int32_t Visit_Inst_Jump(const koopa_raw_jump_t &jump){
    // printf("-----------Visit_Inst_Jump-----------\n");

    koopa_raw_basic_block_t target_bb = jump.target;
	// koopa_raw_slice_t args = jump.args;

    cout << "\tj    " << target_bb->name + 1 << "\n";
    cout << "\n";
    return 0;
}

// 访问 call 指令 (tag = 15)
int32_t Visit_Inst_Call(const koopa_raw_call_t &call){
    // printf("-----------Visit_Inst_Call ----------\n");

    // 将a0~a7压栈
    cout << "\taddi sp, sp, -32\n";
    for(int i = 0; i <= 7; i++){
        cout << "\tsw   a" << i << ", " << i*4 << "(sp)\n";
    }

	koopa_raw_function_t callee = call.callee;
	koopa_raw_slice_t args = call.args;
    const struct koopa_raw_type_kind * ret = callee->ty->data.function.ret;
    koopa_raw_type_tag_t ret_type = ret->tag;
    // printf("return value type = %d\n", ret_type);

    for (size_t i = 0; i < args.len; ++i) {
        if(i >= 8) {
            printf("[Visit_Inst_Call] arg count >= 8\n");
            assert(0);
        }
        // 当前args的内容
        auto ptr = args.buffer[i];
        
        // 当前args的类型为value(即指令)
        if (args.kind == KOOPA_RSIK_VALUE) {
            koopa_raw_value_t value = reinterpret_cast<koopa_raw_value_t>(ptr);
            // 将value放入ai寄存器中
            if(value->kind.tag == KOOPA_RVT_INTEGER){
                // value是整数指令
                cout << "\tli   a" << i <<  ", " << Visit_Inst_Integer(value->kind.data.integer) << "\n";
            } else if(value->kind.tag == KOOPA_RVT_FUNC_ARG_REF){
                // value是函数参数
                int index = Visit_Inst_Func_Arg_Ref(value->kind.data.func_arg_ref);
                cout << "\tmv   a" << i <<  ", a" << index << "\n"; 
            } else{
                // 其他情况, value一定在内存中
                cout << "\tlw   a" << i <<  ", " << Visit_Inst(value) << "(sp)\n";
            }
        }
        // 当前args的类型为其他情况
        else{
            printf("[Visit_Inst_Call]: args.kind = %d\n", args.kind);
            assert(false);
        }
    }

    // 调用函数
    cout << "\tcall " << callee->name+1 << "\n";
    
    // 先恢复栈指针
    cout << "\taddi sp, sp, 32\n";
    
    // 返回值为int32时, 需要保留返回值到栈中
    if(ret_type == KOOPA_RTT_INT32){
        cout << "\tsw   a0, " << use_stack << "(sp)\n";
        use_stack += 4; 
    }

    // 恢复函数参数a0~a7
    for(int i = 0; i <= 7; i++){
        cout << "\tlw   a" << i << ", " << i*4-32 << "(sp)\n";
    }
    cout << "\n";
    return ret_type == KOOPA_RTT_INT32 ? 0 : use_stack-4;
}

// 访问 return 指令 (tag = 16)
int32_t Visit_Inst_Return(const koopa_raw_return_t &ret){
    // printf("-----------Visit_Inst_Return---------------\n");

    // return 指令中, value 代表返回值
    koopa_raw_value_t ret_value = ret.value;
    
    // 将返回值放到a0中
    if(ret_value != NULL){
        if(ret_value->kind.tag == KOOPA_RVT_INTEGER){
            // ret_value为整数指令
            cout << "\tli   a0, " << Visit_Inst_Integer(ret_value->kind.data.integer) << "\n";
        } else if(ret_value->kind.tag == KOOPA_RVT_FUNC_ARG_REF){
            // ret_value是函数参数
            cout << "\tmv   a0, a" << Visit_Inst_Func_Arg_Ref(ret_value->kind.data.func_arg_ref) << "\n"; 
        } else{
            // 其他情况
            cout << "\tlw   a0, " << Visit_Inst(ret_value) << "(sp)\n";
        }
    }

    // 取出返回地址
    cout << "\tlw   ra, " << use_stack << "(sp)\n";
    // 恢复栈空间
    use_stack += 4;
    cout << "\taddi sp, sp, " << use_stack << "\n";
    // 返回
    cout << "\tret\n";
    cout << "\n";
    return 0;
}