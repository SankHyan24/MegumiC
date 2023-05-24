#include <cassert>
#include <cstdio>
#include <iostream>
#include <memory>
#include <string>
#include <koopa.h>

#include <config.hpp>
#include <ast/ast.hpp>
#include <ast/generate.hpp>
#include <ir/generate.hpp>
#include <assembly/generate.hpp>

using namespace std;

int main(int argc, const char *argv[])
{
	MC::config::Config config(argc, argv);

	FILE *inputFile = fopen(config.inputFile.c_str(), "r");

	// generate ast here
	auto ast = MC::AST::node::generate(inputFile);
	ast->Dump();

	// generate ir here
	auto ir = MC::IR::generate(ast);
	ir->Dump();

	// generate assembly here
	auto assembly = MC::ASM::generate(ir);

	std::cout << "done" << MC::ASM::add(1, 2) << std::endl;

	return 0;
}
