/*
** See Copyright Notice inside Include.hpp
*/
#pragma once

#include <vector>
#include <bit>

#include "State.hpp"
#include "ext/CppMatch.hpp"

namespace cpp_jcfu
{
	inline bool isPoolItemBig(const ConstPoolItm& itm)
	{
		return std::holds_alternative<ConstPoolItmType::I64>(itm)
			|| std::holds_alternative<ConstPoolItmType::F64>(itm);
	}

	inline size_t calcConstPoolSize(const ConstPool& pool)
	{
		size_t ret = 0;
		for (const ConstPoolItm& itm : pool)
		{
			if (isPoolItemBig(itm))
				ret += 2;
			else
				ret++;
		}
		return ret;
	}
}
