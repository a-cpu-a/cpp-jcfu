// cpp-jcf.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <fstream>

#include "cpp_jcfu/Gen.hpp"
#include "cpp_jcfu/InstrCompiler.hpp"

int main()
{
	std::cout << "Hello World!\n";

	cpp_jcfu::ConstPool consts{};
	size_t poolSize = 1;

	const std::vector<uint8_t> out = cpp_jcfu::gen(
		cpp_jcfu::ClassFlags_SUPER | cpp_jcfu::ClassFlags_PUBLIC,
		"HelloWorld",
		"java/lang/Object",
		std::move(consts),
		{
			cpp_jcfu::FuncInfo{
				.tags = {cpp_jcfu::compileCode(poolSize, consts, {
					.instrs = {{
							cpp_jcfu::InstrType::GOTO{0},
							cpp_jcfu::InstrType::RET{}
					}},
					.maxStack = 0,
					.maxLocals = 1
				})
				},
				.name = "main",
				.desc = "([Ljava/lang/String;)V",
				.flags = cpp_jcfu::FuncFlags_PUBLIC | cpp_jcfu::FuncFlags_STATIC
			}
		},
		{
			cpp_jcfu::FieldInfo{
				.name = "var",
				.desc = "[Ljava/lang/String;",
				.flags = cpp_jcfu::FieldFlags_PUBLIC 
					| cpp_jcfu::FieldFlags_FINAL
			}
		}
	);

	std::ofstream outF("out/HelloWorld.class", std::ios::binary);

	outF.write((const char*)out.data(), out.size());
	outF.close();

	return 0;
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
