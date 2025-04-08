/*
** See Copyright Notice inside Include.hpp
*/
#pragma once

#include <vector>
#include <bit>

#include "State.hpp"
#include "ext/CppMatch.hpp"
#include "WriteBin.hpp"
#include "WriteConstPool.hpp"

namespace cpp_jcfu
{

	inline std::vector<uint8_t> gen()
	{
		std::vector<uint8_t> out;

		//https://docs.oracle.com/javase/specs/jvms/se7/html/jvms-4.html#jvms-4.1

		u32w(out,0xCAFEBABE);
		u16w(out, 0);
		u16w(out, 51);

		ConstPool consts = {
			ConstPoolItmType::CLASS("HelloWorld"),
			ConstPoolItmType::CLASS("java/lang/Object")
		};
		const ClassFlags thisClassFlags = ClassFlags_SUPER | ClassFlags_PUBLIC;
		const Functions funcs = {
			FuncInfo{
				.tags = {FuncTagType::CODE{
					.bytecode = {0xb1}, //return
					.maxStack = 1,
					.maxLocals = 1
				}},
				.name = "main",
				.desc = "([Ljava/lang/String;)V",
				.flags = FuncFlags_PUBLIC | FuncFlags_STATIC
			}
		};
		_ASSERT(funcs.size() < UINT16_MAX);

		std::vector<uint8_t> funcOut;

		//https://docs.oracle.com/javase/specs/jvms/se7/html/jvms-4.html#jvms-4.6
		//funcs
		{
			size_t poolSize = calcConstPoolSize(consts)+1;

			for (const FuncInfo& func : funcs)
			{
				u16w(funcOut, func.flags);
				constPoolIdxPushW(funcOut, poolSize, consts, 
					ConstPoolItmType::JUTF8(func.name));
				constPoolIdxPushW(funcOut, poolSize, consts, 
					ConstPoolItmType::JUTF8(func.desc));

				_ASSERT(func.tags.size() < UINT16_MAX);
				u16w(funcOut, (uint16_t)func.tags.size());
				for (const FuncTag& tag : func.tags)
					funcTagW(funcOut, poolSize, consts, tag);
			}
		}

		constPoolW(out, std::move(consts));

		u16w(out, thisClassFlags);

		u16w(out, 1);//this
		u16w(out, 2);//super

		u16w(out, 0);//interface count
		u16w(out, 0);//field count

		u16w(out, (uint16_t)funcs.size());//method count
		out.insert(out.end(), funcOut.begin(), funcOut.end());



		u16w(out, 0);//tag count

		return out;
	}
}
