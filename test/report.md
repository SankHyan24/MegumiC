![image-20230608120609006](https://cdn.jsdelivr.net/gh/SankHyan24/image1/img/image-20230608120609006.png)

<br><br><br><br><br>

[toc]



# Megumi C

项目主页：[SankHyan24/MegumiC: MegumiC is a subset of C. (github.com)](https://github.com/SankHyan24/MegumiC)

## 0. 总体介绍

本实验是编译原理实验大程的实验报告，实验内容包括“**Megumi C语言设计**”、“Megumi C IR设计”以及“**Megumi C 到 Megumi C IR 到 RISC-V32 汇编代码生成的编译器设计**”等内容。

Megumi C是C的子集，并且包含了一些其他的功能。Megumi C IR是Megumi C编译器使用的代码中间表示，借鉴了[北京大学编译实践课程在线文档 | 北大编译实践在线文档](https://pku-minic.github.io/online-doc/#/preface/)的内容。Megumi C Compiler是我设计的从Megumi C源代码到RISC-V32汇编的编译器。

### 0.0 功能介绍

Megumi C Compiler 将Megumi C代码编译为汇编代码。编译器支持从Megumi C转换为Megumi C IR，并支持从IR转换为RISC-V32代码并对目标代码进行局部优化。

这里是main.cpp中main函数，从中可以一窥本编译器的大致工作流程：

```cpp
int main(int argc, const char *argv[])
{
	MC::config::Config config(argc, argv);
    // 构造一个config类，并且将参数列表输入config中解析。
    // 在这里会访问输入的文件名，并且将文件中的MegumiC代码提取出来。
	if (!config.configInfo())// 如果config解析错误，将错误信息输出并返回值为0
		return 0;

	// generate ast here
    // 在这里，我们将MegumiC代码输入，生成ast树，并返回ast树的根节点
	auto ast = MC::AST::node::generate(config.getInputCode());

	// generate ir here
    // 在这里，我们将ast树输入，生成IR代码列表，并返回IR代码的包装类。
    // IR代码在内存中有两种存在形式：
    // 1. IRCode，IR代码的字符形式
    // 2. IRTree，IR代码的IR树形式
    // 这里生成的是IR字符形式代码，即IRcode
	auto ir = MC::IR::generate(ast);
	if (config.getEndMode() == MC::config::EndMode::IR)
	{
        // 如果输出形式是IR代码，就输出IR代码并终止程序
		ir->Dump(config.getirOutputFileStream());
		return 0;
	}

	// generate assembly here
    // 在这里，我将IRcode列表作为输入，得到字符型汇编代码的包装类
    // 这里并不能直接从IRcode直接转换为汇编代码，而是需要先生成IRtree
    // 再从IRtree进行解析，生成汇编代码
	auto assembly = MC::ASM::generate(ir);

	// optimize assembly here
    // 在这里我对汇编代码（字符形式）进行优化
    // 输入汇编代码的包装类，得到优化后的代码
    // 汇编代码的字符形式也不能直接进行优化，而是先将其解析成汇编代码的内存形式。
    // 这里的内部实际上是实现了一个优化pipeline框架
    // 可以根据优化模式向这个pipeline中加入各种优化模块
    // 并最终输出优化的结果，比如说优化掉了多少代码
    // 这里我暂时只实现了窥孔优化模块
	auto asmfile = MC::OPT::generate(assembly->getString(), config.getOptMode());、
    // 输出形式是汇编代码时，在这里将代码输出到目标文件流并结束程序
	assembly->Dump(config.getTargetOutputFileStream());
	return 0;
}

```

另外我的工作流程大概也是上面的步骤，一步一步从上而下，并且迭代完成的。

### 0.1 依赖与环境安装

#### 0.1.0 编译器依赖环境：

- `CMake`
- `Bison`
- `Flex`

#### 0.1.1 汇编代码编译为机器码及运行时环境：

- `clang` （将汇编转为机器码）
- `qemu`（用于使用其他计算机运行RV32机器码）

- [sysy-runtime-lib](https://github.com/pku-minic/sysy-runtime-lib/) （用于运行时调用的`putch`和`getch`等`sysy`库函数）

### 0.2 工程编译方法

```shell
git clone https://github.com/SankHyan24/MegumiC
cd MegumiC
mkdir build
cd build
cmake ..
make
```

### 0.3 使用方法

```tcl
Usage: ./mc [options]
Options:
  -v                    Print version
  -h                    Print help
  -c                    Compile
  -mc                   Compile from MC.        Followed by the input file path.
  -irc                  Compile from IR.        Followed by the input file path.
  -ir                   Output IR code.         Followed by the output file path.
  -s                    Output RV32 code.       Followed by the output file path.
  -O0                   No optimization
  -O1                   Optimization level 1
  -O2                   Optimization level 2
  -O3                   Optimization level 3
Example:
    # compile mc file task.c and get the ir-code task.ir
	./mc -c -mc task.c -ir task.ir
	# or
	./mc task.c -ir task.ir
	# compile mc file task.c and get the RV32 asm code task.s
	./mc -c -mc task.c -s task.s
	# or
	./mc task.c -s task.s
```

### 0.4 工程代码规范

- 工程使用`CMakeLists`进行管理，使用`git`进行源代码管理

- 代码将函数的声明和实现分离，将声明等信息放置在`hpp`文件中，头文件放在`include`文件夹下。

- 源代码——函数实现和全局变量等，放置在`cpp`文件中，源文件放在`src`文件夹下。

- 代码各模块使用各自的命名空间作为前缀防止命名冲突。

- 函数名和变量名使用类匈牙利命名法的类似规则。
- 全部使用智能指针管理分配的变量，方便内存管理防止内存泄漏。

### 0.5 分工说明

编译器所有代码均由我一个人完成。

对于tester的测试代码借鉴了其他人的的C++代码，并自行修改为Megumi C的语法。

## 1. 预处理与词法分析

### 1.0 预处理

因为GCC编译器都支持很多种options，比如说输入：

```shell
gcc -v
```

就会输出一系列版本和配置信息。输入：

```shell
gcc --help
```

就会输出帮助信息以及一系列options的提示。

我的编译器也支持了类似功能，具体在main函数的这里实现：

```cpp
	MC::config::Config config(argc, argv);
	if (!config.configInfo())
		return 0;
```

这里做的就是将输入的参数交给`Config`类，使其生成相关配置，并控制接下来的程序该运行什么以及怎么运行。如果输入格式错误或者是那些不需要编译的选项，`Config`类的`configInfo`会输出相关错误信息或版本帮助等并返回`0`以终止程序。

如果输入格式正确且需要进行编译任务，`Config`类就会将需要编译的文件和准备输出的文件打开以备后续使用。从此编译器工作正式开始。

### 1.1 词法与词法分析

词法分析器使用`flex`进行token的提取，词法内容在`src/parser/megumic.l`，主要作用是提取关键词、符号和identifier。

词法分析的阶段被包含在了语法分析中，在yyparse调用的时候被执行。

```cpp
	// 在main函数的这里完成，被集成到AST生成中
	auto ast = MC::AST::node::generate(config.getInputCode());
```

具体内容：

```cpp
{WhiteSpace}    { /* 忽略, 不做任何操作 */ }
{LineComment}   { /* 忽略, 不做任何操作 */ }

"int"           { return INT; }/*Key words */
"ptr"           { return PTR; }
"const"         { return CONST; }
"return"        { return RETURN; }
"static"        { return FUNCTION; }
"while"         { return WHILE; }
"if"            { return IF; }
"else"          { return ELSE; }
"break"         { return BREAK; }
"continue"      { return CONTINUE;}
{Char}          { yylval.int_val= yytext[1]; return CHAR_CONST; }/*单个字符：*/


"<="            { return LE_OP; }
">="            { return GE_OP; }
"=="            { return EQ_OP; }
"!="            { return NE_OP; }
"&&"            { return AND_OP; }
"||"            { return OR_OP; }

";"     { return SEMICOLON; }
","     { return COMMA; }

{Identifier}    { yylval.str_val = new string(yytext); return IDENT; }

{Decimal}       { yylval.int_val = strtol(yytext, nullptr, 0); return INT_CONST; }
{Octal}         { yylval.int_val = strtol(yytext, nullptr, 0); return INT_CONST; }
{Hexadecimal}   { yylval.int_val = strtol(yytext, nullptr, 0); return INT_CONST; }

.               { return yytext[0]; }
```

## 2. 语法分析与抽象语法树生成

语法分析和抽象语法树生成时从Megumi C到目标代码的第一步。

这一步在main函数的这行代码中被执行：

```cpp
	// 返回的ast是抽象语法树的根节点
	auto ast = MC::AST::node::generate(config.getInputCode());
```

### 2.0 Megumi C语法

Megumi C的语法和C代码极其类似。

整体语法由变量（全局）声明和函数定义构成：

```cpp
CompUnit
    : CompUnit Decl
    | CompUnit FuncDef 
    | Decl        
    | FuncDef;     
```

函数定义：

```yacc
FuncDef
	: FUNCTION FuncType ident '(' ')' Block
	| FUNCTION FuncType ident '(' FuncParams ')' Block;
```

其中`FUNCTION` token是函数的显示定义声明，目前只实现了`static`函数。

其中的函数参数：

```cpp
FuncParams
	: FuncParams COMMA FuncParam
    | FuncParam;

FuncParam
    : FuncParamOne
    | FuncParamArray;

FuncParamOne
	: BType ident;

FuncParamArray
	: FuncParamArray '[' Exp ']'
    | FuncParamOne;
```

声明语句：

```cpp
Decl
	: ConstDeclStmt
	| VarDeclStmt;

ConstDeclStmt
	: ConstDecl SEMICOLON;

ConstDecl
	: CONST BType ConstDef
    | ConstDecl COMMA ConstDef;

ConstDef 
	: ident '=' Exp;

// VarDef 与上面的语法类似
// 数组定义与函数参数中的数组定义类似
...
```

块定义：

```cpp
Block
	: '{' '}'
	| '{' BlockItems '}';

BlockItems
	: BlockItem 
	| BlockItems BlockItem ;

BlockItem
	: Decl | Stmt;
```

语句定义

```cpp
Stmt
	: Block
	| ReturnStmt
	| WhileStmt
	| BreakStmt
	| IfStmt
	| AssignStmt
	| ContinueStmt
	| Exp SEMICOLON
	| SEMICOLON;

ReturnStmt
	: RETURN Exp SEMICOLON
	| RETURN SEMICOLON;

WhileStmt
	: WHILE '(' Exp ')' Stmt;

BreakStmt
	: BREAK SEMICOLON;

ContinueStmt
	: CONTINUE SEMICOLON;

IfStmt
	: IF '(' Exp ')' Stmt
	| IF '(' Exp ')' Stmt ELSE Stmt;

AssignStmt
	: Assignment SEMICOLON;

Assignment
	: LVal '=' Exp;
```

表达式定义：

```cpp
Exp 
	: LOrExp ;

PrimaryExp
	: '(' Exp ')' 
	| LVal
	| Number;

Number
	: INT_CONST 
	| CHAR_CONST ;

UnaryExp
	: PrimaryExp 
	| FunctionCall
	| UnaryOp UnaryExp ;

UnaryOp
	: '+'
	| '-'
	| '!';

MulOp
	: '*'
	| '/'
	| '%';

AddOp
	: '+'
	| '-';

RelOp
	: '<'
	| '>'
	| LE_OP
	| GE_OP;

EqOp
	: EQ_OP
	| NE_OP;

MulExp
	: UnaryExp 
	| MulExp MulOp UnaryExp;

AddExp
	: MulExp 
	| AddExp AddOp MulExp;

RelExp
	: AddExp 
	| RelExp RelOp AddExp;

EqExp
	: RelExp
	| EqExp EqOp RelExp;

LAndExp
	: EqExp
	| LAndExp AND_OP EqExp;

LOrExp
	: LAndExp 
	| LOrExp OR_OP LAndExp;
```

标识符等内容定义：

```cpp
ident: IDENT;

LVal: ident
	| ArrayItem;

ArrayItem
	: LVal '[' Exp ']'
	| ArrayItem '[' Exp ']';
```

### 2.0 抽象语法树生成

相关文件：`src/ast/ast.cpp`

在语法分析器上，我们的输入是一系列token，输出是构建出的抽象语法树。

对于抽象语法树，这里使用的面向对象方法：

```cpp
namespace MC::AST::node
{
	class BaseAST
	{
        ...;
    };
    class Expression : public BaseAST
	{
        ...;
    };
    ...
}
```

在这里定义了抽象语法树的构建方法：

```cpp
namespace MC::AST::node
{
	class BinaryExpression : public Expression
	{
	public:
		MC::IR::BinOp op;
		std::unique_ptr<Expression> lhs, rhs;
		BinaryExpression(Expression *lhs, MC::IR::BinOp op, Expression *rhs) : op(op), lhs(std::move(lhs)), rhs(std::move(rhs)) {}
	private:
		virtual void _generate_ir(MC::IR::Context &ctx, MC::IR::IRList &ir) override;
		void _dump() const override;
	};
    ...
}
```

上面是一个对于二元表达式的例子，它由两个表达式的指针以及二元操作符构造而成，以及生成ir代码和ast测试输出方法。而这里的所有抽象语法树节点，都使用智能指针对子节点进行管理，以防止内存泄漏。

在语法分析器中，完成了对其表达式的构造实例化过程。

```cpp
MulExp
	: UnaryExp 
	| MulExp MulOp UnaryExp {$$ = new MC::AST::node::BinaryExpression($1,$2,$3);};
```

最终得到一个ast树。

## 3. 语义分析与中间代码生成

相关文件：`src/ir/generate_ir.cpp`，`src/ir/context.cpp`

语义分析和中间代码生成是在同时进行着的。在中间代码生成的同时也同时进行着语义分析和语法检查。

此过程在main函数的这行代码中被执行：

```cpp
	// 在这里遍历抽象语法树，返回值是一个IRcode list的封装类
	auto ir = MC::IR::generate(ast);
```

### 3.0 语义分析

对于一个ast树的语义分析，需要维护一个抽象语法树的上下文的数据结构，来存储一些信息。

我在这里实现了一系列类：

```cpp
amespace MC::IR
{
    class VarInfo
    {
    public:
        std::vector<int> shape;
        VarType type;
        bool is_array;
        std::string name; // with @$%
        VarInfo(std::string name, MC::IR::VarType type = VarType::Val, bool is_array = false, std::vector<int> shape = {});
    };

    class ConstInfo
    {
    public:
        bool is_array;
        std::vector<int> shape;
        std::vector<int> value;
        ConstInfo(std::vector<int> value, bool is_array = false,
                  std::vector<int> shape = {});
        ConstInfo(int value);
    };

    class Context
    {
    public:
        Context();
        int id;
        int get_id() { return ++id; }
        int get_last_id() { return id; }
        using SymbolTable = std::vector<std::unordered_map<std::string, VarInfo>>;
        using ConstTable = std::vector<std::unordered_map<std::string, ConstInfo>>;

        SymbolTable symbol_table = {{}};
        ConstTable const_table = {{}};
        ConstTable const_assign_table = {{}};
        std::stack<std::string> loop_label; // push while_start and while_end

        void insert_symbol(std::string name, VarInfo value);
        void insert_const(std::string name, ConstInfo value);
        void insert_const_assign(std::string name, ConstInfo value);

        VarInfo &find_symbol(std::string name);
        ConstInfo &find_const(std::string name);
        ConstInfo &find_const_assign(std::string name);

        bool if_symbol_exist(std::string name);
        bool if_const_exist(std::string name);
        bool if_symbol_exist_in_this_scope(std::string name);

        void create_scope();
        void end_scope();

        void add_loop_label(std::string label);
        void pop_loop_label();
        std::string get_loop_label();

        bool is_global();

    private:
    };

}
```

上下文的主要维护内容是符号表，其中符号表中存储了变量的具体信息，对应类`VarInfo`。我还实现了`ConstInfo`，用它不仅存储变量信息，更主要是存储常数数据的实际值，使之在编译阶段直接使用立即数而非变量，但我并没有完成这部分，在编译器中也没有真正使用这个功能。

我使用的是一个哈希表的变长数组来存储符号表。如果分析语法进入了一个block，就会进入下一层的哈希表，反之从一个block中出来就会从哈希表中出到外面一层中。

除此之外，上下文还存储着循环语句的标识，用来存储一个循环语句的入口和出口的label的名称。这里主要是为了处理while语句中的break和continue语句。

### 3.1 中间代码生成

关于中间代码生成，实际上都是对ast中的各个类`_generate_ir`方法的多态实现：

```cpp
namespace MC::AST::node
{
    void BaseAST::_generate_ir(MC::IR::Context &ctx, MC::IR::IRList &ir)
    {
        throw std::runtime_error("Not implemented");
    }

    void CompUnitAST::_generate_ir(MC::IR::Context &ctx, MC::IR::IRList &ir)
    {
        for (auto &i : list)
        {
            i->generate_ir(ctx, ir);
        }
    }
    
    // 其他类类似地
    ...
}
```

其主要流程就是：实现ast节点本身的IR翻译以及其子节点的IR翻译，维护上下文，并将得到的IR代码加入到IR列表中。这里实际上会遇到很多种情况，下面是对于一个identifier的翻译过程：

```cpp
	void Identifier::_generate_ir(MC::IR::Context &ctx, MC::IR::IRList &ir)
    { // only as right value
        if (dynamic_cast<ArrayIdentifier *>(this))
        {
            dynamic_cast<ArrayIdentifier *>(this)->generate_ir(ctx, ir);
        }
        else
        {
            if (ctx.if_const_exist(this->name))
            {
                auto &constinfo = ctx.find_const(this->name);
                if (constinfo.is_array)
                    throw std::runtime_error("Can't fetch the value from a array.");
                int value = constinfo.value[0];
                int exp_id = ctx.get_id();
                std::string ir_name_val = "%" + std::to_string(exp_id);
                this->id = exp_id;

                return;
            }

            int exp_id = ctx.get_id();
            std::string ir_name_val = "%" + std::to_string(exp_id);
            this->id = exp_id;

            auto &varinfo = ctx.find_symbol(this->name);
            std::string ir_name_ptr = varinfo.name;
            if (varinfo.is_array)
            {
                auto &shape = varinfo.shape;
                ir.push_back(std::unique_ptr<MC::IR::IRGetElementPtr>());
                std::vector<int> shape_;
                for (int i = 1; i < shape.size(); i++)
                    shape_.push_back(shape[i]);
                if (varinfo.type == MC::IR::VarType::Ptr)
                    ir.back().reset(new MC::IR::IRGetElementPtr(ir_name_val, ir_name_ptr, "0", 1, shape_.size() + 1, shape_));
                else

                    ir.back().reset(new MC::IR::IRGetElementPtr(ir_name_val, ir_name_ptr, "0", 0, shape_.size() + 1, shape_));
            }
            else if (varinfo.type == MC::IR::VarType::Ptr)
            {
                ir.push_back(std::unique_ptr<MC::IR::IRLoad>());
                ir.back().reset(new MC::IR::IRLoad(ir_name_ptr, ir_name_val));
            }
            else
            {
                ir.push_back(std::unique_ptr<MC::IR::IRLoad>());
                ir.back().reset(new MC::IR::IRLoad(ir_name_ptr, ir_name_val));
            }
        }
    }
```

这里是`Identifier`作为右值时候的翻译过程，其作为左值的情况在Assignment语句中单独实现。

可以看到这里面处理了其作为一个`ArrayIdentifier`和单独`Identifier`不同类别的不同处理方式，以及本身不同类别的不同处理方式：比如说如果此Identifier在符号表中为一个数组，则其返回值应为这个数组的首地址，对于`ptr`类型的变量也是如此；否则应该为这个变量的值本身。

### 3.2 IR代码生成

上面方法生成的是IR的code形式，但不是IR代码。

这里我又实现了一系列类，来对IR从类到IRcode的实际字符进行生成：

```cpp
namespace MC::IR
{    
    class IRcode
    {
    public:
        IROp IRType{IROp::Void};
        bool if_new_var{false};
        std::string new_var_name;

        std::string dst;
        virtual ~IRcode() = default;
        void generate();
        std::string dump();

    private:
        virtual void _generate();
    };

    class IRFuncDef : public IRcode
    {
    public:
        std::string funcName;
        VarType retType;
        std::vector<ArgPair> args;
        IRFuncDef(std::string funcName, VarType retType, std::vector<ArgPair> args) : funcName(funcName), retType(retType), args(args) {}

    private:
        virtual void _generate() override;
    };
    
    class IRStore : public IRcode
    {
    public:
        // store %x, %y
        // Mem[y]=x
        std::string Value;
        std::string AddressOfTarget;
        IRStore(std::string Value, std::string AddressOfTarget) : Value(Value), AddressOfTarget(AddressOfTarget) {}

    private:
        virtual void _generate() override;
    };
    
    // 其他类型IR 
    ...
}
```



### 3.3 IR生成结果

IR的生成结果我用一个`IRListWrapper`的类来包装：

```cpp
namespace MC::IR
{
    class IRListWrapper
    {
    public:
        std::unique_ptr<IRList> irList;
        bool print_ir_line_number{true};
        IRListWrapper() : irList(std::make_unique<IRList>()) {}
        void Generate();
        void Dump();
        void Dump(std::string filename);
        void Dump(std::ostream &os);
        std::string toString();
    };
}
```

这里主要是来管理这个IRlist，方便接口调用。这里是调用IR生成的具体实现：

```cpp
namespace MC::IR
{
    std::unique_ptr<IRListWrapper> generate(std::unique_ptr<MC::AST::node::BaseAST> &ast)
    {
        Context ctx; // the context
        IRList ir;
        ast->generate_ir(ctx, ir); // ast树生成IR list
        IRListWrapper irListWrapper;// IR list包装
        irListWrapper.irList = std::make_unique<IRList>(std::move(ir));
        irListWrapper.Generate(); // IR list 生成 IRcode 代码
        return std::make_unique<IRListWrapper>(std::move(irListWrapper));
    }
}
```

## 4. 目标代码生成

目标代码生成是Megumi C到RISC-V32汇编的最后一步。在这个步骤中仍需要维护一个上下文信息，存储ir中的变量符号表、全局变量以及函数调用堆栈空间。

### 4.0 生成目标代码初始形态

此过程在main函数的这行代码中被执行：

```cpp
	auto assembly = MC::ASM::generate(ir);
```

在这里，我实现了一个` AssemblyList`封装类，用来进行对目标代码初始形态（仅字符）的管理。

```cpp
namespace MC::ASM
{
    std::unique_ptr<AssemblyList> generate(std::unique_ptr<MC::IR::IRListWrapper> &irs, std::ostream &out)
    {
        AssemblyList assemblyList(irs);
        assemblyList.Generate();
        return std::make_unique<AssemblyList>(std::move(assemblyList));
    }
}
```

这里执行`AssemblyList`中的`Generate()`方法，会先将IRList中的字符形式的IRcode组建成具有语义和关联含义的IRTree，然后遍历IRTree，得到IRTree的目标代码生成结果。

IRTree也是使用了面向对象的设计思路。其定义：

```cpp
namespace MC::ASM
{
    // IR tree is the irtree constructed from the ircode,
    // the generate method is used to generate the
    class IRTree
    {
    public:
        void generate(Context &ctx, std::ostream &out);
        virtual ~IRTree() = default;

    private:
        virtual void _generateLV0(Context &ctx, std::ostream &out);
    };

    class IRTInst : public IRTree
    {
    public:
        std::string ircode_dst;
        MC::IR::IROp tag;
        std::string opname1, opname2, opname3;
        int imm;
        MC::IR::BinOp opcode;

      	...
        ...
            
        // line:
        int line;

    private:
        virtual void _generateLV0(Context &ctx, std::ostream &out) override;
    };

    class IRTBasicBlock : public IRTree
    {
    public:
        std::string label;
        std::vector<std::unique_ptr<IRTInst>> instLists;
        // for generate
        int needStackByte{0};
        int count_need_stack_byte(Context &ctx);

    private:
        void _count_need_stack_byte(Context &ctx);
        virtual void _generateLV0(Context &ctx, std::ostream &out) override;
    };

    class IRTFunction : public IRTree
    {
    public:
        IRArgType retType;
        std::string functionName;
        std::vector<IRArgPair> argList;
        std::vector<std::unique_ptr<IRTBasicBlock>> bbList;

        int needStackByte{0};

    private:
        virtual void _generateLV0(Context &ctx, std::ostream &out) override;
    };

    class IRTRoot : public IRTree
    {
    public:
        std::vector<std::unique_ptr<IRTInst>> instList;
        std::vector<std::unique_ptr<IRTFunction>> funcList;

    private:
        virtual void _generateLV0(Context &ctx, std::ostream &out) override;
    };

}
```

这里的生成方法`_generateLV0`是代码的最简单生成方式，即不进行寄存器分配，因此生成的代码结果比较长。因为我是一个人结组，精力不够了，就没具体实现寄存器分配版本的generate函数，也就是更高级的LV1、LV2等。具体实现使用哪种生成方法还是取决于`generate`中根据当前优化等级的定义进行选择。

由于generate中的代码实现基本都是很麻烦的堆工作量，没有技术难点，故不在此展开展示。需要注意的是这里我也使用了上下文`Context`不过这里的上下文和IR中的并不属于一个命名空间，所以不是一个类。

```cpp
namespace MC::ASM
{
    class Context
    {
    public:
        static constexpr int reg_count = 12;
        std::ostream &log_out;
        std::vector<std::unordered_map<std::string, Address>> var_table{{}};
        std::unordered_map<std::string, MC::IR::IROp> var_type{{}};          
        std::unordered_map<std::string, std::vector<int>> global_table{{}}; 
        std::unordered_map<std::string, int> function_need_stack{{}};  
        int this_function_stack_size{0};
        Context(std::ostream &out = std::cout);
        void insert_function_need_stack(std::string name, int stack_size);
        int find_function_need_stack(std::string name);

        void insert_symbol(std::string name, Address value);
        void insert_type(std::string name, MC::IR::IROp type);
        Address &find_symbol_last_table(std::string name);
        bool if_in_symbol_table(std::string name);
        MC::IR::IROp find_type(std::string name);

        void insert_global(std::string name, std::vector<int>);
        std::vector<int> &find_global(std::string name);

        void create_scope(int stack_size);
        void end_scope();

        Address allocate_address(int size);
        int rest_address_size();

        RV32RegBitMask &getBackBitMask();

    private:
        std::vector<RV32RegBitMask> regs_to_be_protected; // protect them when call other function
        std::vector<int> stack_allocate_ptr;
        std::vector<int> stack_max_size;
    };
}
```

这里主要负责对函数中每个ir变量的栈地址进行分配，根据之前存储的栈空间进行分配。维护需要保存的寄存器，以便于在call其他函数的时候将其保护起来。这里还维护ir变量的符号表，主要存储ir变量的名称和其地址的映射，以及ir变量为全局变量时的情况。

这一步完成的时候，我们得到的是ircode生成的字符串形式的目标代码，存储在`AssemblyList`的成员`buffer`中。

### 4.1 优化及生成目标代码终极形态

其实上一步的代码就可以作为结果了，但是人类的目标是星辰大海，所以我还加了一步优化模块，这里需要对之前得到的汇编字符串进行进一步语义解析和处理。

这一步在main函数的这行代码中被执行：

```cpp
	auto asmfile = MC::OPT::generate(assembly->getString(), config.getOptMode());
```

这里我又定义了一个`AsmFile`类，用来对输出的汇编代码文件进行管理：

```cpp
namespace MC::OPT
{
    class AsmFile
    {
    public:
        AsmFile(std::string asm_code);
        void deleteComments();
        void addOptimizer(Optimizer *optimizer);
        void Generate();
        void Dump(std::ostream &out = std::cout);

    private:
        int line_count = 0;
        std::vector<AsmCode> codes;
        std::unique_ptr<OptimizerPipeline> optimizer_pipeline{std::make_unique<OptimizerPipeline>()};
    };

    std::unique_ptr<AsmFile> generate(std::string code, MC::config::OptMode optmode);
}
```

它的作用主要是接受上一步的结果`asm_code`，并将字符串形式的汇编解析为`AsmCode`。具体步骤就是把汇编代码字符串切割成一个个汇编指令，并生成AsmCode。

这个`AsmCode`类的作用是解析并管理我生成的RISC-V32汇编代码：

```cpp
namespace MC::OPT
{    
    class AsmCode
    {
    public:
        AsmCode(AsmOp op, std::string arg1, std::string arg2, std::string arg3, std::string comment = "") : op(op), arg1(arg1), arg2(arg2), arg3(arg3), comment(comment){};
        AsmCode(std::string target_inst_str);
        std::string getString();
        void Dump(std::ostream &out = std::cout);

        AsmOp getOp() const { return this->op; }
        std::string getOpStr() const { return this->opstr; }
        std::string getArg1() const { return this->arg1; }
        std::string getArg2() const { return this->arg2; }
        std::string getArg3() const { return this->arg3; }

    private:
        AsmOp op;
        // xxx:
        std::string label;
        // .xxx (yyy)
        std::string asmstr;
        std::string asmstr_arg;
        // opstr arg1, arg2, arg3
        std::string opstr;
        std::string arg1;
        std::string arg2;
        std::string arg3;
        // comment
        std::string comment;
    };
}
```

`AsmFile`类还负责对代码的优化。`AsmFile`管理着一个优化流水线，可以使用这里的`addOptimizer`方法向流水线中添加优化器。比如说这里我实现了一个窥孔优化，就可以这样将其加入到我的`AsmFile`中并对在其中的代码进行优化并输出结果：

```cpp
AsmFile asmfile(code);
// add a peephole optimizer
auto peephole_optimizer = new PeepholeOptimizer();
asmfile.addOptimizer(peephole_optimizer);
// execute the optimizer and generate final target code
asmfile.Generate();
// dump the AsmCodes to std::cout
asmfile->Dump(std::cout);
```

这里我还加入了优化结果的展示，可以更直接地感受到优化的效果：

```cpp
namespace MC::OPT
{    
    void AsmFile::Generate()
    {
        this->optimizer_pipeline->executeOptimizers(this->codes);
        int new_count = this->codes.size();
        float rate = (float)((line_count - new_count) * 100) / (float)line_count;
        std::string rate_str = std::to_string(rate);
        rate_str = rate_str.substr(0, rate_str.find(".") + 3);
        rate = std::stof(rate_str);
        std::cout << "Optimization: " << rate << "\% lines deleted." << std::endl;
    }
}
```

到这里我们的编译器就结束了~

## 5. 总结

- 自行实现 **Megumi C 到 IR**，自行实现**IR 到 RISCV 汇编**
- 所有测试点全部通过
- 完成了一些加分项，比如说窥孔优化
- 所有任务一个人独立完成

coding和debug的过程中室友给了我很多思路上的帮助。

代码框架我主要参考的北大的koopa框架，我还参考了[nzh63/syc: Optimizing compiler for SysY (C subset) (github.com)](https://github.com/nzh63/syc)这个项目，从他的设计和代码框架中学到了很多，比如`main`函数的结构和`config`（不过他的IR和target code和我的完全不一样）。

## 6. 测试与结果展示

### 6.0 课程的官方测试点测试结果：

这里我在Makefile文件中写了几个target，安装依赖成功后，执行`make test*`（`*`=1,2,3）就能自动编译执行并测试。

#### 6.0.0 测试点1

![image-20230608161107558](https://cdn.jsdelivr.net/gh/SankHyan24/image1/img/image-20230608161107558.png)

#### 6.0.1 测试点2

![image-20230608161142687](https://cdn.jsdelivr.net/gh/SankHyan24/image1/img/image-20230608161142687.png)

#### 6.0.2 测试点3

![image-20230608161235778](https://cdn.jsdelivr.net/gh/SankHyan24/image1/img/image-20230608161235778.png)

### 6.1 优化效果展示：

执行

```shell
./mc ../test/task3.c -O1
```

对于`task3.c`的优化，删除了近3%的没用的代码。

![image-20230608162415489](https://cdn.jsdelivr.net/gh/SankHyan24/image1/img/image-20230608162415489.png)

### 6.2 Git工作流

这里因为代码都是我一个人写的也都是我一个人push的，git log 中其实只记录了我每一步都干了啥。

整个项目的开发流程其实就是目录中我介绍的顺序（除了config是我最后加的）。git log如下：

```shell
commit 6b38a4fdb0ab34dbfd024605e2a4ec9ca81b30cc
Author: scitbb <2209339564@qq.com>
Date:   Sat Jun 3 15:53:23 2023 +0000

    refine readme

commit 63901788625c9df16383a432888809f033f03a71
Author: scitbb <2209339564@qq.com>
Date:   Sat Jun 3 15:45:21 2023 +0000

    refine config and finish peephole

commit 6b3b0a329b4ad160a74dd04a995895c3efbae020
Author: scitbb <2209339564@qq.com>
Date:   Sat Jun 3 07:36:25 2023 +0000

    add opt framework

commit 581c32241ca46035dd6d8bcf8f62b993c41e7281
Author: scitbb <2209339564@qq.com>
Date:   Mon May 29 17:06:40 2023 +0000

    refine tester format

commit 070b4ba4e734a560a86cde479e0c7e6959353a06
Author: scitbb <2209339564@qq.com>
Date:   Mon May 29 17:06:26 2023 +0000

    refine output

commit 2c1e27b392f1978a52856494bda873f57427387c
Author: scitbb <2209339564@qq.com>
Date:   Mon May 29 17:06:09 2023 +0000

    add tester code

commit ee0eb7e9876fed485bc2f6eaec89f90d6bff1399
Author: scitbb <2209339564@qq.com>
Date:   Mon May 29 17:05:29 2023 +0000

    clear rubbish

commit 0b85826ac2ed942c214636310c17b6d6b51b851f
Author: scitbb <2209339564@qq.com>
Date:   Mon May 29 13:56:16 2023 +0000

    对ir生成的shape进行修改以及code生成的逻辑，针对多维数组

commit b8f75d8d5b1de335c2525e12046b05d2cdda7cee
Author: scitbb <2209339564@qq.com>
Date:   Mon May 29 10:19:21 2023 +0000

    refine entrance and precode

commit cc7819479140ec922e07e71ccf4ef1888495cd75
Author: scitbb <2209339564@qq.com>
Date:   Mon May 29 09:30:15 2023 +0000

    refine code and l

commit 307fd681e7e41a00b77c0da993951eebc712f5ef
Author: scitbb <2209339564@qq.com>
Date:   Sun May 28 13:33:50 2023 +0000

    solve wrong code of task3 with test 3 not done

commit af5e3c9d0ea53d5e1d0a9da23cbb150ba0ee0a1d
Author: scitbb <2209339564@qq.com>
Date:   Sun May 28 11:34:24 2023 +0000

    task3 modified, but sp wrong

commit 9f90e4580fe84ee9eb197ea893b31c85fbedbdbd
Author: scitbb <2209339564@qq.com>
Date:   Sun May 28 11:11:54 2023 +0000

    modify codes

commit 0a7a0d2dfa9e566e7352ead32ebfc7934df776a9
Author: scitbb <2209339564@qq.com>
Date:   Sun May 28 11:10:16 2023 +0000

    add opt framework

commit 3c30b5741f7400ebe5146aba8396b2748cd69140
Author: scitbb <2209339564@qq.com>
Date:   Sun May 28 10:43:16 2023 +0000

    test1 pass

commit 346539fa6f203329be8d4f184d6de62a4202ec6f
Author: scitbb <2209339564@qq.com>
Date:   Sun May 28 08:25:20 2023 +0000

    数组参数测试完毕

commit 3ba05974ab5c2a32bafb06c95a9d9d0a0039874c
Author: scitbb <2209339564@qq.com>
Date:   Sun May 28 08:02:26 2023 +0000

    数组遍历完成

commit 519926afd04a0ecd1a9eed7bb4f5664ca5d1534e
Author: scitbb <2209339564@qq.com>
Date:   Sun May 28 07:00:20 2023 +0000

    数组参数done

commit ac9b7284e4b026dcc2fb300f9421672e39c560d9
Author: scitbb <2209339564@qq.com>
Date:   Sun May 28 06:00:57 2023 +0000

    rm ir

commit 57e0ce906b6166529e0963d9ad13596b2783c09e
Author: scitbb <2209339564@qq.com>
Date:   Sun May 28 05:49:59 2023 +0000

    read int and print int done!(task4.c)

commit fafa58de3761258b9f0eb89c48b9ef4c1e602662
Author: scitbb <2209339564@qq.com>
Date:   Sat May 27 19:28:02 2023 +0000

    add deffun(int a[][12])

commit 00d4c4293159486a769fbe316fa8b60d1879c44f
Author: scitbb <2209339564@qq.com>
Date:   Sat May 27 16:12:15 2023 +0000

    compiler  nearly done with printint

commit 77c24dcd949582f223569cf7ea854dd059396db8
Author: scitbb <2209339564@qq.com>
Date:   Fri May 26 17:20:49 2023 +0000

    target code doing, 明天从call开始写起

commit dcbfa7c27503f13baa2df04e45d843416ed1b147
Author: scitbb <2209339564@qq.com>
Date:   Thu May 25 19:06:40 2023 +0000

    add void ir between blocks

commit c882816be0dbfcc5525e4fc0ba854838f9e3b2f6
Author: scitbb <2209339564@qq.com>
Date:   Thu May 25 18:59:19 2023 +0000

    add irtree framework

commit 97929867f1aae604c1dca2b87b68c61a51c48c12
Author: scitbb <2209339564@qq.com>
Date:   Wed May 24 19:28:38 2023 +0000

    add asm

commit f2a3bff9b503a79c0de87ba2901e378cdbb736bc
Author: scitbb <2209339564@qq.com>
Date:   Wed May 24 18:18:26 2023 +0000

    array done!

commit ac545666009ed82e65afe43b2c9fa097f32b27ca
Author: scitbb <2209339564@qq.com>
Date:   Wed May 24 16:03:24 2023 +0000

    task 1 to ir pass, wrong with array args

commit 7d25ce4d1672d03fdfeb7044f0e935c05e66f538
Author: scitbb <2209339564@qq.com>
Date:   Tue May 23 14:27:59 2023 +0000

    mainly done

commit 500b6dd9d4113f56facc226a4422c69db779fbdc
Author: scitbb <2209339564@qq.com>
Date:   Tue May 23 09:07:30 2023 +0000

    array done

commit 4fb1cc18899b02d5ff6b138db0d8a9cdb11f32e1
Author: scitbb <2209339564@qq.com>
Date:   Mon May 22 18:36:30 2023 +0000

    global done. now only Arraydef arrayinit arrayfectchvalue and globalArray not implement

commit 8ef541cf17b7086920f0ce4f04f7249b10347239
Author: scitbb <2209339564@qq.com>
Date:   Mon May 22 17:15:25 2023 +0000

    ptr type done

commit 684b815980bdbf0a7c5fc46e46da116d5b104576
Author: scitbb <2209339564@qq.com>
Date:   Mon May 22 16:53:55 2023 +0000

    while if else break continue done

commit aee760a83b62c33a20c3da804c2cdde172342136
Author: scitbb <2209339564@qq.com>
Date:   Mon May 22 14:52:57 2023 +0000

    IR class Nearly done but with bug about assignment

commit 9e9c218ba1d8f4fc055975d3281aac37b6306fcc
Author: scitbb <2209339564@qq.com>
Date:   Sun May 21 19:58:24 2023 +0000

    local var declare done, next with global decalre and const and assignment

commit 999b1484bb73b090d4d53e1d66ac685afcb87a85
Author: scitbb <2209339564@qq.com>
Date:   Sun May 21 19:30:32 2023 +0000

    function call done

commit 4bdb93fe3fd8a02d1d3c1c00e4eedbb625bbbf61
Author: scitbb <2209339564@qq.com>
Date:   Sun May 21 19:07:46 2023 +0000

    expression nearly done

commit 0f838cc27023ba4aee00e0d58a09da02a2aee9de
Author: scitbb <2209339564@qq.com>
Date:   Sun May 21 19:03:07 2023 +0000

    IR with return statement

commit 01da9e8026831e85c73891f7ad487c74d88236ac
Author: scitbb <2209339564@qq.com>
Date:   Sun May 21 15:08:02 2023 +0000

    sc

commit 2358d7e539ab10be0111bfbd1a6aa1920289c9a6
Author: scitbb <2209339564@qq.com>
Date:   Sun May 21 14:57:19 2023 +0000

    ast done, start ir!

commit 3f3bbc0d0d5f863a6dbb6dba4f436f9ab366eb8b
Author: scitbb <2209339564@qq.com>
Date:   Sun May 21 14:56:56 2023 +0000

    ast done, start id

commit 4d528b560d9817c64d455c5b7946f860be86b1f2
Author: scitbb <2209339564@qq.com>
Date:   Sun May 21 14:55:49 2023 +0000

    function call done

commit 5a69f6c00847654655d5b961c52b19c1b93fa007
Author: scitbb <2209339564@qq.com>
Date:   Sun May 21 14:44:11 2023 +0000

    assignment done

commit d9e0824ebc3b8b8885c5207730fbcd7f89f4cf33
Author: scitbb <2209339564@qq.com>
Date:   Sun May 21 14:38:22 2023 +0000

    array define and init part done

commit 0cc681561bc4ea3b6000091147b9ac2b352bc989
Author: scitbb <2209339564@qq.com>
Date:   Sat May 20 18:14:12 2023 +0800

    ast without lval assign and array

commit 4960cfdd7572b203c23de4cf6f500b404430a673
Author: scitbb <2209339564@qq.com>
Date:   Sun May 14 13:12:35 2023 +0800

    大改前存档

commit d586346be0a0ee0e71487b42fa87e8d5b230d521
Author: scitbb <2209339564@qq.com>
Date:   Fri Apr 14 15:34:18 2023 +0000

    refine yac format

commit 75dba10f990b648212b5b5147005040c9db3aeb2
Author: scitbb <2209339564@qq.com>
Date:   Wed Apr 12 12:40:53 2023 +0000

    refine ast dump

commit 072830879c358eefe60258f5434c2a344b766747
Author: scitbb <2209339564@qq.com>
Date:   Wed Apr 12 11:47:53 2023 +0000

    support binary expression

commit 5fc10a0e9dbef6b78f4a26cdeb05659904857777
Author: scitbb <2209339564@qq.com>
Date:   Wed Apr 12 10:17:54 2023 +0000

    adds

commit 6657e51d0b5fc9388b1344c13d4363f79da071ec
Author: scitbb <2209339564@qq.com>
Date:   Wed Apr 12 10:16:01 2023 +0000

    add Exper and reconstruct ast

commit e1dd3e20744465cca8ebc3807e201f11a0ad5058
Author: scitbb <2209339564@qq.com>
Date:   Thu Apr 6 16:25:33 2023 +0000

    add IR part

commit 576e642c57b7c47caa4cbe41c358b8400f81ad10
Author: scitbb <2209339564@qq.com>
Date:   Thu Apr 6 16:25:24 2023 +0000

    reconstruct the ast

commit 16752588eeb378086ec08858fc8de46f1b09e168
Author: scitbb <2209339564@qq.com>
Date:   Thu Apr 6 16:24:56 2023 +0000

    add describe

commit 9e842373e645de80d5c7ec14d367267f731bad4f
Author: scitbb <2209339564@qq.com>
Date:   Thu Apr 6 16:24:47 2023 +0000

    add and update CMAKe config

commit be6905f7e2f0ceae8de6d35dfa2120cbc9f481a0
Author: scitbb <2209339564@qq.com>
Date:   Thu Apr 6 16:24:26 2023 +0000

    add ast

commit bd144928b92548d76d4baa43edfe58568e6392cf
Author: scitbb <2209339564@qq.com>
Date:   Thu Apr 6 16:24:20 2023 +0000

    move ast to module

commit b6f0a91e8a472e26279489707aada215f73f649e
Author: scitbb <2209339564@qq.com>
Date:   Thu Apr 6 13:35:56 2023 +0000

    add config and parser

commit 7a920505536d3b842d886d5ad279377fdc19f2ae
Author: scitbb <2209339564@qq.com>
Date:   Thu Apr 6 13:35:43 2023 +0000

    rename

commit 411802d8123fbecc7de8faa4c4613f45728a60be
Author: scitbb <2209339564@qq.com>
Date:   Thu Apr 6 13:35:16 2023 +0000

    rename

commit 668d3be35319c0106c0ac8f91ce87c30e6f49ec5
Author: scitbb <2209339564@qq.com>
Date:   Thu Apr 6 13:09:43 2023 +0000

    base parser and dump

commit b2ae733edc61036333fb7e749cbcd6a95b7ed253
Author: scitbb <2209339564@qq.com>
Date:   Thu Apr 6 13:08:53 2023 +0000

    add run

commit a18b48b58af4c8de5a6f42cefd992bf4bae528c7
Author: scitbb <2209339564@qq.com>
Date:   Thu Apr 6 08:00:44 2023 +0000

    add test framework

commit 07e329ab190385da392a1ad880bff5851cfa1c0b
Author: scitbb <2209339564@qq.com>
Date:   Thu Apr 6 08:00:30 2023 +0000

    update

commit d9237df3a166f9ed6e3346470e03c830245d9801
Author: Charles Sun <2209339564@qq.com>
Date:   Thu Apr 6 13:06:01 2023 +0800

    Initial commit
```
