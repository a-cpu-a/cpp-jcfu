// cpp-jcf.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <fstream>

#include "cpp_jcfu/Gen.hpp"
#include "cpp_jcfu/InstrCompiler.hpp"


template<class T,class... Ts>
std::vector<T> newVec(Ts&&... instrs)
{
	std::vector<T> ret;
	ret.reserve(sizeof...(instrs));

	(ret.emplace_back(std::forward<decltype(instrs)>(instrs)), ...);
	return ret;
}

template<typename Kt, typename Vt>
std::map<Kt, Vt> newMap() {
	return {};
}

template<typename Kt, typename Vt, typename Key, typename Value, typename... Rest>
std::map<Kt, Vt> newMap(Key&& key, Value&& value, Rest&&... rest) {
	std::map<Kt, Vt> result;
	result.emplace(std::forward<Key>(key), std::forward<Value>(value));
	auto restMap = newMap<Kt, Vt>(std::forward<Rest>(rest)...);
	result.insert(std::make_move_iterator(restMap.begin()), std::make_move_iterator(restMap.end()));
	return result;
}


int main()
{
	std::cout << "Hello World!\n";

	cpp_jcfu::ConstPool consts{};
	size_t poolSize = 1;


	const std::vector<cpp_jcfu::Instr> instrs = newVec<cpp_jcfu::Instr>(
		cpp_jcfu::newPushRunStatic("StaticHolder", { "fname","()LGlobeObject;" }),
		cpp_jcfu::newPushRunStatic("StaticHolder", { "isTrue","()Z" }),
		cpp_jcfu::InstrType::IF_EQL{3},
		cpp_jcfu::InstrType::DUP_1{},
		cpp_jcfu::newPushRunVirtual("GlobeObject", { "fname","()V" }),
		cpp_jcfu::InstrType::RET_OBJ{}
	);

	const std::vector<uint8_t> out = cpp_jcfu::gen(
		cpp_jcfu::ClassFlags_SUPER | cpp_jcfu::ClassFlags_PUBLIC,
		"HelloWorld",
		"java/lang/Object",
		std::move(consts),
		newVec<cpp_jcfu::FuncInfo>(
			cpp_jcfu::FuncInfo{
				.tags = newVec<cpp_jcfu::FuncTag>(
					cpp_jcfu::compileCode(poolSize, consts, {
					.instrs = instrs,
					.startFrameLocals = newVec<cpp_jcfu::SlotKind>(
						cpp_jcfu::newObjSlotKind("[Ljava/lang/String;")
					),
					.instructionFrames = newMap<uint16_t,cpp_jcfu::StackFrame>(
						5,
						cpp_jcfu::StackFrame{.stack = newVec<cpp_jcfu::SlotKind>(
							cpp_jcfu::newObjSlotKind("LGlobeObject;")
						)}
					),
					.lineNums = {{1,9},{2,10},{4,11}},
					.localVars = {cpp_jcfu::LocalEntry{
						.name = "helloWorld",
						.desc="[Ljava/lang/String;",
						.startInstr=0,
						.instrCount=6,
						.idx=0
					}},
					.maxStack = 2,
					.maxLocals = 1
				})
				),
				.name = "main",
				.desc = "([Ljava/lang/String;)LGlobeObject;",
				.flags = cpp_jcfu::FuncFlags_PUBLIC | cpp_jcfu::FuncFlags_STATIC
			}
		),
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
