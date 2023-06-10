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

MC::config::Config *MC::config::config = nullptr;

using namespace std;
int main(int argc, const char *argv[])
{
	MC::config::config = new MC::config::Config(argc, argv);
	if (!MC::config::config->configInfo())
		return 0;

	// generate ast here
	auto ast = MC::AST::node::generate(MC::config::config->getInputCode());

	// generate ir here
	auto ir = MC::IR::generate(ast);
	if (MC::config::config->getFirstMode() == MC::config::FirstMode::Online)
	{
		ir->Dump();
		MC::config::config->printOnlineSeprarator();
	}
	else if (MC::config::config->getEndMode() == MC::config::EndMode::IR)
	{
		ir->Dump(MC::config::config->getirOutputFileStream());
		return 0;
	}

	// generate assembly here
	auto assembly = MC::ASM::generate(ir);

	// optimize assembly here
	auto asmfile = MC::OPT::generate(assembly->getString(), MC::config::config->getOptMode());
	if (MC::config::config->getFirstMode() == MC::config::FirstMode::Online)
		asmfile->Dump(std::cout);
	else
		assembly->Dump(MC::config::config->getTargetOutputFileStream());

	// std::cout << "hello world" << std::endl;
	return 0;
}
