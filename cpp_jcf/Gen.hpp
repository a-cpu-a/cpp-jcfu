/*
** See Copyright Notice inside Include.hpp
*/
#pragma once

#include <vector>

namespace cpp_jcf
{
	inline void u32w(std::vector<uint8_t>& out,const uint32_t v)
	{
		out.push_back((v >> 24) & 0xFF);
		out.push_back((v >> 16) & 0xFF);
		out.push_back((v >> 8) & 0xFF);
		out.push_back(v & 0xFF);
	}
	inline void u16w(std::vector<uint8_t>& out,const uint16_t v)
	{
		out.push_back((v >> 8) & 0xFF);
		out.push_back(v & 0xFF);
	}

	inline std::vector<uint8_t> gen()
	{
		std::vector<uint8_t> out;
		u32w(out,0xCAFEBABE);
		u16w(out, 0);
		u16w(out, 51);

		//https://docs.oracle.com/javase/specs/jvms/se7/html/jvms-4.html#jvms-4.4
		//const pool
	}
}
