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

	inline std::vector<uint8_t> gen(
		const ClassFlags thisClassFlags,
		const std::string& thisClass,
		const std::string& superClass,
		ConstPool&& consts, 
		const Functions& funcs, 
		const Fields& fields)
	{
		_ASSERT(funcs.size() < UINT16_MAX);
		_ASSERT(fields.size() < UINT16_MAX);

		std::vector<uint8_t> out;

		//https://docs.oracle.com/javase/specs/jvms/se7/html/jvms-4.html#jvms-4.1

		u32w(out, 0xCAFEBABE);
		u16w(out, 0);
		u16w(out, 51);

		std::vector<uint8_t> fieldOut;

		size_t poolSize = calcConstPoolSize(consts) + 1;

		//https://docs.oracle.com/javase/specs/jvms/se7/html/jvms-4.html#jvms-4.5
		// Fields
		{

			for (const FieldInfo& info : fields)
			{
				u16w(fieldOut, info.flags);
				constPoolIdxPushW(fieldOut, poolSize, consts,
					ConstPoolItmType::JUTF8(info.name));
				constPoolIdxPushW(fieldOut, poolSize, consts,
					ConstPoolItmType::JUTF8(info.desc));

				_ASSERT(info.tags.size() < UINT16_MAX);
				u16w(fieldOut, (uint16_t)info.tags.size());
				for (const FieldTag& tag : info.tags)
					fieldTagW(fieldOut, poolSize, consts, tag);
			}
		}

		std::vector<uint8_t> funcOut;

		//https://docs.oracle.com/javase/specs/jvms/se7/html/jvms-4.html#jvms-4.6
		// Funcs
		{
			for (const FuncInfo& info : funcs)
			{
				u16w(funcOut, info.flags);
				constPoolIdxPushW(funcOut, poolSize, consts, 
					ConstPoolItmType::JUTF8(info.name));
				constPoolIdxPushW(funcOut, poolSize, consts, 
					ConstPoolItmType::JUTF8(info.desc));

				_ASSERT(info.tags.size() < UINT16_MAX);
				u16w(funcOut, (uint16_t)info.tags.size());
				for (const FuncTag& tag : info.tags)
					funcTagW(funcOut, poolSize, consts, tag);
			}
		}

		//Do next to last, to optimize small op-code stuff
		const uint16_t thisClassIdx = constPoolPush(poolSize, consts, ConstPoolItmType::CLASS(thisClass));
		const uint16_t superClassIdx = constPoolPush(poolSize, consts, ConstPoolItmType::CLASS(superClass));

		constPoolW(out, std::move(consts));

		u16w(out, thisClassFlags);

		u16w(out, thisClassIdx);//this
		u16w(out, superClassIdx);//super

		u16w(out, 0);//interface count
		u16w(out, (uint16_t)fields.size());//field count
		out.insert(out.end(), fieldOut.begin(), fieldOut.end());

		u16w(out, (uint16_t)funcs.size());//method count
		out.insert(out.end(), funcOut.begin(), funcOut.end());

		u16w(out, 0);//tag count

		return out;
	}
}
