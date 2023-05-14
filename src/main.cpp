#include <cassert>
#include <cstdio>
#include <iostream>
#include <memory>
#include <string>

#include <config.hpp>
// #include <koopa.h>
#include <ast/ast.hpp>
#include <ast/generate.hpp>
#include <ir/generate.hpp>

using namespace std;
extern FILE *yyin;

int main(int argc, const char *argv[])
{
	MC::config::parse_arg(argc, argv);

	yyin = fopen(MC::config::inputfile.c_str(), "r");
	assert(yyin);

	// generate ast here
	auto ast = MC::ast::node::generate(yyin);
	ast->Dump();

	// generate ir here
	// auto ir = MC::IR::generate(ast);

	return 0;
}
