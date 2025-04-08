/*
** See Copyright Notice inside Include.hpp
*/
#pragma once

#include <vector>
#include <bit>

#include "State.hpp"
#include "Utf8ToJutf8.hpp"

namespace cpp_jcfu
{
	inline void u64w(std::vector<uint8_t>& out, const uint64_t v)
	{
		out.push_back((v >> 56) & 0xFF);
		out.push_back((v >> 48) & 0xFF);
		out.push_back((v >> 40) & 0xFF);
		out.push_back((v >> 32) & 0xFF);
		out.push_back((v >> 24) & 0xFF);
		out.push_back((v >> 16) & 0xFF);
		out.push_back((v >> 8) & 0xFF);
		out.push_back(v & 0xFF);
	}
	inline void u32w(std::vector<uint8_t>& out, const uint32_t v)
	{
		out.push_back((v >> 24) & 0xFF);
		out.push_back((v >> 16) & 0xFF);
		out.push_back((v >> 8) & 0xFF);
		out.push_back(v & 0xFF);
	}
	inline void u16w(std::vector<uint8_t>& out, const uint16_t v)
	{
		out.push_back((v >> 8) & 0xFF);
		out.push_back(v & 0xFF);
	}
	//https://docs.oracle.com/javase/specs/jvms/se7/html/jvms-4.html#jvms-4.4.7
	inline void jUtf8W(std::vector<uint8_t>& out, std::string v)
	{
		v = utf8ToJutf8(v);
		_ASSERT(v.size() < UINT16_MAX);
		u16w(out, (uint16_t)v.size());
		out.insert(out.end(), v.begin(), v.end());
	}

	inline void constPoolIdxW(std::vector<uint8_t>& out, size_t& poolSize, ConstPool& consts, ConstPoolItm&& itm)
	{
		_ASSERT(poolSize < UINT16_MAX);
		u16w(out, (uint16_t)poolSize);
		consts.emplace_back(itm);
	}
}
