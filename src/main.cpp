#include <cassert>
#include <cstdio>
#include <iostream>
#include <memory>
#include <string>

#include <config.hpp>
#include <ast/ast.hpp>
#include <ast/generate.hpp>
#include <ir/generate.hpp>
#include <assembly/generate.hpp>
#include <optimizer/generate.hpp>
#include <optimizer/opt.hpp>
#include <optimizer/asmcode.hpp>

using namespace std;

int main(int argc, const char *argv[])
{
	MC::config::Config config(argc, argv);
	if (!config.configInfo())
		return 0;

	// generate ast here
	auto ast = MC::AST::node::generate(config.getInputCode());

	// generate ir here
	auto ir = MC::IR::generate(ast);
	if (config.getEndMode() == MC::config::EndMode::IR)
	{
		ir->Dump(config.getirOutputFileStream());
		return 0;
	}

	// generate assembly here
	auto assembly = MC::ASM::generate(ir);

	// optimize assembly here
	auto asmfile = MC::OPT::generate(assembly->getString(), config.getOptMode());
	assembly->Dump(config.getTargetOutputFileStream());

	return 0;
}
