/*
** See Copyright Notice inside Include.hpp
*/
#pragma once

#include <vector>
#include <bit>

#include "State.hpp"
#include "ext/CppMatch.hpp"
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
	inline void jUtf8W(std::vector<uint8_t>& out,std::string v)
	{
		v = utf8ToJutf8(v);
		_ASSERT(v.size() < UINT16_MAX);
		u16w(out, (uint16_t)v.size());
		out.insert(out.end(), v.begin(), v.end());
	}

	inline void constPoolIdxW(std::vector<uint8_t>& out,size_t& poolSize,ConstPool& consts, ConstPoolItm&& itm)
	{
		_ASSERT(poolSize < UINT16_MAX);
		u16w(out, (uint16_t)poolSize);
		consts.emplace_back(itm);
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

		ConstPool consts = {
			ConstPoolItmType::CLASS("HelloWorld"),
			ConstPoolItmType::CLASS("java/lang/Object")
		};
		const ClassFlags thisClassFlags = ClassFlags_SUPER | ClassFlags_PUBLIC;
		Functions funcs = {
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


		//https://docs.oracle.com/javase/specs/jvms/se7/html/jvms-4.html#jvms-4.4
		//const pool
		{
			std::vector<uint8_t> poolOut;
			size_t poolSize = 1;// +1

			for (size_t i = 0; i < consts.size(); i++)
			{
				poolSize++;

				ezmatch(consts[i])(
				varcase(const ConstPoolItmType::CLASS&) {
					poolOut.push_back((uint8_t)ConstPoolItmId::CLASS);
					constPoolIdxW(poolOut, poolSize, consts,
						ConstPoolItmType::JUTF8(var.name));
				},

				varcase(const ConstPoolItmType::FIELD_REF&) {
					poolOut.push_back((uint8_t)ConstPoolItmId::FIELD_REF);
					constPoolIdxW(poolOut, poolSize, consts,
						ConstPoolItmType::CLASS(var.classIdx));
					constPoolIdxW(poolOut, poolSize, consts,
						ConstPoolItmType::NAME_AND_DESC(var.refDesc));
				},
				varcase(const ConstPoolItmType::FUNC_REF&) {
					poolOut.push_back((uint8_t)ConstPoolItmId::FUNC_REF);
					constPoolIdxW(poolOut, poolSize, consts,
						ConstPoolItmType::CLASS(var.classIdx));
					constPoolIdxW(poolOut, poolSize, consts,
						ConstPoolItmType::NAME_AND_DESC(var.refDesc));
				},
				varcase(const ConstPoolItmType::INTERFACE_FUNC_REF&) {
					poolOut.push_back((uint8_t)ConstPoolItmId::INTERFACE_FUNC_REF);
					constPoolIdxW(poolOut, poolSize, consts,
						ConstPoolItmType::CLASS(var.classIdx));
					constPoolIdxW(poolOut, poolSize, consts,
						ConstPoolItmType::NAME_AND_DESC(var.refDesc));
				},

				varcase(const ConstPoolItmType::STR&) {
					poolOut.push_back((uint8_t)ConstPoolItmId::STR);
					constPoolIdxW(poolOut, poolSize, consts,
						ConstPoolItmType::JUTF8(var.txt));
				},
				varcase(const ConstPoolItmType::I32&) {
					poolOut.push_back((uint8_t)ConstPoolItmId::I32);
					u32w(poolOut, var);
				},
				varcase(const ConstPoolItmType::I64&) {
					poolOut.push_back((uint8_t)ConstPoolItmId::I64);
					poolSize++;
					u64w(poolOut, var);
				},
				varcase(const ConstPoolItmType::F32&) {
					poolOut.push_back((uint8_t)ConstPoolItmId::F32);
					u32w(poolOut, std::bit_cast<uint32_t>(var));
				},
				varcase(const ConstPoolItmType::F64&) {
					poolOut.push_back((uint8_t)ConstPoolItmId::F64);
					poolSize++;
					u64w(poolOut, std::bit_cast<uint64_t>(var));
				},

				varcase(const ConstPoolItmType::NAME_AND_DESC&) {
					poolOut.push_back((uint8_t)ConstPoolItmId::NAME_AND_DESC);
					constPoolIdxW(poolOut, poolSize, consts,
						ConstPoolItmType::JUTF8(var.name));
					constPoolIdxW(poolOut, poolSize, consts,
						ConstPoolItmType::JUTF8(var.desc));
				},
				varcase(const ConstPoolItmType::JUTF8&) {
					poolOut.push_back((uint8_t)ConstPoolItmId::JUTF8);
					jUtf8W(poolOut, var);
				},
				varcase(const ConstPoolItmType::FUNC_HANDLE&) {
					poolOut.push_back((uint8_t)ConstPoolItmId::FUNC_HANDLE);
					u16w(poolOut, (uint16_t)var.kind);
					ConstPoolItm res;
					switch (var.kind)
					{
					case FuncHandleKind::GET_FIELD:
					case FuncHandleKind::PUT_FIELD:
					case FuncHandleKind::GET_STATIC:
					case FuncHandleKind::PUT_STATIC:
						res = ConstPoolItmType::FIELD_REF(var.val);
						break;
					case FuncHandleKind::RUN_DYN:
					case FuncHandleKind::RUN_STATIC:
					case FuncHandleKind::RUN_SPECIAL:
						res = ConstPoolItmType::FUNC_REF(var.val);
						break;
					case FuncHandleKind::RUN_INTERFACE:
						res = ConstPoolItmType::INTERFACE_FUNC_REF(var.val);
						break;
					}
					constPoolIdxW(poolOut, poolSize, consts,
						std::move(res));
				},
				varcase(const ConstPoolItmType::FUNC_TYPE&) {
					poolOut.push_back((uint8_t)ConstPoolItmId::FUNC_TYPE);
					constPoolIdxW(poolOut, poolSize, consts,
						ConstPoolItmType::JUTF8(var.desc));
				},
				varcase(const ConstPoolItmType::RUN_DYN&) {
					poolOut.push_back((uint8_t)ConstPoolItmId::RUN_DYN);
					u16w(poolOut, var.bootstrapIdx);
					constPoolIdxW(poolOut, poolSize, consts,
						ConstPoolItmType::NAME_AND_DESC(var.funcDesc));
				}
				);
			}
			_ASSERT(poolSize < UINT16_MAX);
			u16w(out, (uint16_t)poolSize);

			out.insert(out.end(), poolOut.begin(), poolOut.end());
		}

		u16w(out, thisClassFlags);

		u16w(out, 1);//this
		u16w(out, 2);//super

		u16w(out, 0);//interface count
		u16w(out, 0);//field count
		u16w(out, 1);//method count



		u16w(out, 0);//tag count

		return out;
	}
}
