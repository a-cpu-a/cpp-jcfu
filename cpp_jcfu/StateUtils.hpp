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

	inline uint16_t constPoolPush(size_t& poolSize, ConstPool& consts, ConstPoolItm&& itm)
	{
		const bool is2x = isPoolItemBig(itm);
		_ASSERT(poolSize < (UINT16_MAX - (is2x ? 1 : 0)));
		consts.emplace_back(itm);
		const uint16_t res = (uint16_t)poolSize;

		poolSize++;
		if (is2x)
			poolSize++;

		return res;
	}
}
