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
	//https://docs.oracle.com/javase/specs/jvms/se7/html/jvms-4.html#jvms-4.4.7
	inline void jUtf8W(std::vector<uint8_t>& out,const std::string& v)
	{
		u16w(out, v.size());
	}

	inline size_t calcConstPoolSize(const ConstPool& pool)
	{
		size_t ret = 0;
		for (const ConstPoolItm& itm : pool)
		{
			if (
				std::holds_alternative<ConstPoolItmType::I64>(itm)
				|| std::holds_alternative<ConstPoolItmType::F64>(itm)
				)
				ret += 2;
			else
				ret++;
		}
		return ret;
	}

	inline std::vector<uint8_t> gen()
	{
		std::vector<uint8_t> out;

		//https://docs.oracle.com/javase/specs/jvms/se7/html/jvms-4.html#jvms-4.1

		u32w(out,0xCAFEBABE);
		u16w(out, 0);
		u16w(out, 51);

		ConstPool consts(10);

		{
			std::vector<uint8_t> poolOut;
			size_t poolSize = 1;// +1

			for (const ConstPoolItm& itm : consts)
			{
				ezmatch(itm)(
				varcase(const ConstPoolItmType::CLASS&) {
					poolOut.push_back((uint8_t)ConstPoolItmId::CLASS);

				},

				varcase(const ConstPoolItmType::FIELD_REF&) {
					poolOut.push_back((uint8_t)ConstPoolItmId::FIELD_REF);
				},
				varcase(const ConstPoolItmType::FUNC_REF&) {
					poolOut.push_back((uint8_t)ConstPoolItmId::FUNC_REF);
				},
				varcase(const ConstPoolItmType::INTERFACE_FUNC_REF&) {
					poolOut.push_back((uint8_t)ConstPoolItmId::INTERFACE_FUNC_REF);
				},

				varcase(const ConstPoolItmType::STR&) {
					poolOut.push_back((uint8_t)ConstPoolItmId::STR);
					jUtf8W(poolOut, var);
				},
				varcase(const ConstPoolItmType::I32&) {
					poolOut.push_back((uint8_t)ConstPoolItmId::I32);
					u32w(poolOut, var);
				},
				varcase(const ConstPoolItmType::I64&) {
					poolOut.push_back((uint8_t)ConstPoolItmId::I64);
					u64w(poolOut, var);
				},
				varcase(const ConstPoolItmType::F32&) {
					poolOut.push_back((uint8_t)ConstPoolItmId::F32);
					u32w(poolOut, std::bit_cast<uint32_t>(var));
				},
				varcase(const ConstPoolItmType::F64&) {
					poolOut.push_back((uint8_t)ConstPoolItmId::F64);
					u64w(poolOut, std::bit_cast<uint64_t>(var));
				},

				varcase(const ConstPoolItmType::NAME_AND_DESC&) {
					poolOut.push_back((uint8_t)ConstPoolItmId::NAME_AND_DESC);
				},
				varcase(const ConstPoolItmType::JUTF8&) {
					poolOut.push_back((uint8_t)ConstPoolItmId::JUTF8);
				},
				varcase(const ConstPoolItmType::FUNC_HANDLE&) {
					poolOut.push_back((uint8_t)ConstPoolItmId::FUNC_HANDLE);
				},
				varcase(const ConstPoolItmType::FUNC_TYPE&) {
					poolOut.push_back((uint8_t)ConstPoolItmId::FUNC_TYPE);
				},
				varcase(const ConstPoolItmType::RUN_DYN&) {
					poolOut.push_back((uint8_t)ConstPoolItmId::RUN_DYN);
				}
				);
			}
			_ASSERT(poolSize < UINT16_MAX);
			u16w(out, poolSize);

			out.insert(out.end(), poolOut.begin(), poolOut.end());
		}


		//https://docs.oracle.com/javase/specs/jvms/se7/html/jvms-4.html#jvms-4.4
		//const pool
	}
}
