/*
** See Copyright Notice inside Include.hpp
*/
#pragma once

#include <vector>
#include <bit>

#include "State.hpp"
#include "ext/CppMatch.hpp"
#include "WriteBin.hpp"
#include "StateUtils.hpp"

namespace cpp_jcfu
{
	inline void constPoolW(std::vector<uint8_t>& out,ConstPool&& consts)
	{
		//https://docs.oracle.com/javase/specs/jvms/se7/html/jvms-4.html#jvms-4.4
		//const pool
			std::vector<uint8_t> poolOut;
			size_t poolSize = calcConstPoolSize(consts)+1;

			for (size_t i = 0; i < consts.size(); i++)
			{
				ezmatch(consts[i])(
					varcase(const ConstPoolItmType::CLASS&) {
					poolOut.push_back((uint8_t)ConstPoolItmId::CLASS);
					constPoolIdxPushW(poolOut, poolSize, consts,
						ConstPoolItmType::JUTF8(var.name));
				},

				varcase(ConstPoolItmType::FIELD_REF&) {
					poolOut.push_back((uint8_t)ConstPoolItmId::FIELD_REF);

					const ConstPoolItmType::NAME_AND_DESC refDescSteal = std::move(var.refDesc);

					constPoolIdxPushW(poolOut, poolSize, consts,
						ConstPoolItmType::CLASS(var.classIdx));
					constPoolIdxPushW(poolOut, poolSize, consts,
						ConstPoolItmType::NAME_AND_DESC(refDescSteal));
				},
				varcase(ConstPoolItmType::FUNC_REF&) {
					poolOut.push_back((uint8_t)ConstPoolItmId::FUNC_REF);

					const ConstPoolItmType::NAME_AND_DESC refDescSteal = std::move(var.refDesc);

					constPoolIdxPushW(poolOut, poolSize, consts,
						ConstPoolItmType::CLASS(var.classIdx));
					constPoolIdxPushW(poolOut, poolSize, consts,
						ConstPoolItmType::NAME_AND_DESC(refDescSteal));
				},
				varcase(ConstPoolItmType::INTERFACE_FUNC_REF&) {
					poolOut.push_back((uint8_t)ConstPoolItmId::INTERFACE_FUNC_REF);

					const ConstPoolItmType::NAME_AND_DESC refDescSteal = std::move(var.refDesc);

					constPoolIdxPushW(poolOut, poolSize, consts,
						ConstPoolItmType::CLASS(var.classIdx));
					constPoolIdxPushW(poolOut, poolSize, consts,
						ConstPoolItmType::NAME_AND_DESC(refDescSteal));
				},

				varcase(const ConstPoolItmType::STR&) {
					poolOut.push_back((uint8_t)ConstPoolItmId::STR);
					constPoolIdxPushW(poolOut, poolSize, consts,
						ConstPoolItmType::JUTF8(var.txt));
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

				varcase(ConstPoolItmType::NAME_AND_DESC&) {
					poolOut.push_back((uint8_t)ConstPoolItmId::NAME_AND_DESC);

					const std::string descSteal = std::move(var.desc);

					constPoolIdxPushW(poolOut, poolSize, consts,
						ConstPoolItmType::JUTF8(var.name));
					constPoolIdxPushW(poolOut, poolSize, consts,
						ConstPoolItmType::JUTF8(descSteal));
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
					case FuncHandleKind::NEW_RUN_SPECIAL:
						res = ConstPoolItmType::FUNC_REF(var.val);
						break;
					case FuncHandleKind::RUN_INTERFACE:
						res = ConstPoolItmType::INTERFACE_FUNC_REF(var.val);
						break;
					}
					constPoolIdxPushW(poolOut, poolSize, consts,
						std::move(res));
				},
				varcase(const ConstPoolItmType::FUNC_TYPE&) {
					poolOut.push_back((uint8_t)ConstPoolItmId::FUNC_TYPE);
					constPoolIdxPushW(poolOut, poolSize, consts,
						ConstPoolItmType::JUTF8(var.desc));
				},
				varcase(const ConstPoolItmType::RUN_DYN&) {
					poolOut.push_back((uint8_t)ConstPoolItmId::RUN_DYN);
					u16w(poolOut, var.bootstrapIdx);
					constPoolIdxPushW(poolOut, poolSize, consts,
						ConstPoolItmType::NAME_AND_DESC(var.funcDesc));
				}
					);
			}
			_ASSERT(poolSize < UINT16_MAX);
			u16w(out, (uint16_t)poolSize);

			out.insert(out.end(), poolOut.begin(), poolOut.end());
	}
}
