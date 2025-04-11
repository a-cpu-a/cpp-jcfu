/*
** See Copyright Notice inside Include.hpp
*/
#pragma once

#include <vector>
#include <bit>

#include "State.hpp"
#include "ext/CppMatch.hpp"
#include "WriteBin.hpp"
#include "Instrs.hpp"

namespace cpp_jcfu
{
	template<class T>
	constexpr uint8_t INSTR_OP_CODE = aca::variant_index_v<T, Instr>;

	inline void pushOpCodeId(
		std::vector<uint8_t>& out,
		std::vector<uint16_t>& instrOffsets,
		size_t& curInstrOffset,
		const uint16_t i,
		const InstrId id)
	{
		out.push_back((uint8_t)id);
		_ASSERT(curInstrOffset < UINT16_MAX);
		instrOffsets[i] = uint16_t(curInstrOffset++);
	}
	inline void pushOpCodeByte(
		std::vector<uint8_t>& out,
		std::vector<uint16_t>& instrOffsets,
		size_t& curInstrOffset,
		const uint16_t i,
		const auto& var)
	{
		constexpr uint8_t op = INSTR_OP_CODE<decltype(var)>;
		static_assert(op <= (uint8_t)InstrId::DEPR_JSR32);
		pushOpCodeId(out, instrOffsets, curInstrOffset, i, (InstrId)op);
	}
	inline void pushWideOpCodeId(
		std::vector<uint8_t>& out,
		std::vector<uint16_t>& instrOffsets,
		size_t& curInstrOffset,
		const uint16_t i,
		const InstrId id)
	{
		out.push_back((uint8_t)InstrId::I_WIDE);
		out.push_back((uint8_t)id);
		_ASSERT(curInstrOffset < (UINT16_MAX - 1));
		instrOffsets[i] = uint16_t(curInstrOffset);
		curInstrOffset += 2;
	}


	template<class T>
	concept BaseBranched16 = std::derived_from<T, InstrType::BaseBranch16>;
	template<class T>
	concept BaseBranched32 = std::derived_from<T, InstrType::BaseBranch32>;
	template<class T>
	concept BaseRefed = sizeof(T)==sizeof(void*) 
		&& !std::same_as<T, InstrType::PUSH_RUN_DYN> 
		&& (std::derived_from<T, InstrType::BaseFieldRef>
			|| std::derived_from<T, InstrType::BaseFuncRef>
			|| std::derived_from<T, InstrType::BaseClassRef>);
	template<class T>
	concept BaseVarInstred = sizeof(T) == 1
		&& std::derived_from<T, InstrType::BaseVarInstr>;
	template<class T>
	concept BaseVar16Instred = sizeof(T) == 2
		&& std::derived_from<T, InstrType::BaseVar16Instr>;
	template<class T>
	concept BaseBranched = std::derived_from<T, InstrType::BaseBranch> && !std::same_as<T, InstrType::GOTO>;

	template<class T>
	concept PushConstXed = 
		std::same_as<T, InstrType::I_PUSH_CONST_U16>
		|| std::same_as<T, InstrType::I_PUSH_CONST2_U16>;

	template<class T>
	concept BasicOpCode = sizeof(T)==1
		&& !BaseBranched16<T>
		&& !BaseBranched32<T>
		&& !BaseRefed<T>
		&& !BaseVarInstred<T>
		&& !BaseVar16Instred<T>
		&& !BaseBranched<T>
		&& !PushConstXed<T>;

	struct PatchPoint
	{
		int32_t instrOffset : 31;
		int32_t is32Bit : 1 = false;
		uint16_t instrIdx;
		uint16_t byteOffset;
	};

	inline std::vector<uint8_t> compileInstrs(
		size_t& poolSize, ConstPool& consts,
		const std::vector<Instr>& instrs
	)
	{
		_ASSERT(instrs.size() < UINT16_MAX);

		//TODO: build list of instr offsets
		//TODO: build list of bytes to patch into relative instr offsets
		//TODO: solve relative instr sizes

		std::vector<PatchPoint> instrPatchPoints;
		std::vector<uint16_t> instrOffsets(instrs.size());
		size_t curInstrOffset = 0;

		std::vector<uint8_t> out;
		out.reserve(instrs.size() + (instrs.size() >> 3)); // 1.125X scaling


		for (uint16_t i = 0; i < instrs.size(); i++)
		{
			const Instr& instr = instrs[i];

			//std::visit([out](const auto& v) mutable {out.push_back(INSTR_OP_CODE<decltype(v)>); }, instr);
			//_ASSERT(curInstrOffset < UINT16_MAX);
			//instrOffsets[i] = uint16_t(curInstrOffset++);

			ezmatch(instr)(

			// Easy 1 byte instructions
			varcase(const BasicOpCode auto) {
				pushOpCodeByte(out, instrOffsets, curInstrOffset, i, var);
			},

			// Hard ones

			varcase(const InstrType::I_PUSH_I32_I8) {
				pushOpCodeByte(out, instrOffsets, curInstrOffset, i, var);
				out.push_back(var);
				curInstrOffset++;
			},
			varcase(const InstrType::I_PUSH_I32_I16) {
				pushOpCodeByte(out, instrOffsets, curInstrOffset, i, var);
				u16w(out, var);
				curInstrOffset += 2;
			},

			varcase(const InstrType::I_PUSH_CONST_U8) {
				pushOpCodeByte(out, instrOffsets, curInstrOffset, i, var);
				out.push_back(var.poolIdx);
				curInstrOffset++;
			},
			varcase(const PushConstXed auto) {
				pushOpCodeByte(out, instrOffsets, curInstrOffset, i, var);
				u16w(out, var.poolIdx);
				curInstrOffset += 2;
			},

			varcase(const BaseVarInstred auto) {
				pushOpCodeByte(out, instrOffsets, curInstrOffset, i, var);
				out.push_back(var.varIdx);
				curInstrOffset++;
			},

			varcase(const InstrType::I_ADD_I32_VAR_U8_CI8) {
				pushOpCodeByte(out, instrOffsets, curInstrOffset, i, var);
				out.push_back(var.varIdx);
				out.push_back(var.val);
				curInstrOffset += 2;
			},

			varcase(const BaseBranched16 auto) {
				pushOpCodeByte(out, instrOffsets, curInstrOffset, i, var);
				u16w(out, var.jmpOffsetBytes);
				curInstrOffset += 2;
			},

			varcase(const InstrType::TABLE_SWITCH&) {
				pushOpCodeByte(out, instrOffsets, curInstrOffset, i, var);
				//TODO
			},
			varcase(const InstrType::LOOKUP_SWITCH&) {
				pushOpCodeByte(out, instrOffsets, curInstrOffset, i, var);
				//TODO
			},

			varcase(const BaseRefed auto&) {
				pushOpCodeByte(out, instrOffsets, curInstrOffset, i, var);
				constPoolIdxPushW(out, poolSize, consts, *var.ref);
				curInstrOffset += 2;
			},

			varcase(const InstrType::PUSH_RUN_INTERFACE&) {
				pushOpCodeByte(out, instrOffsets, curInstrOffset, i, var);
				constPoolIdxPushW(out, poolSize, consts, *var.ref);
				out.push_back(var.argCount);
				out.push_back(0);
				curInstrOffset += 4;
			},
			varcase(const InstrType::PUSH_RUN_DYN&) {
				pushOpCodeByte(out, instrOffsets, curInstrOffset, i, var);
				constPoolIdxPushW(out, poolSize, consts, *var.ref);
				out.push_back(0);
				out.push_back(0);
				curInstrOffset += 4;
			},

			varcase(const InstrType::PUSH_ARR) {
				pushOpCodeByte(out, instrOffsets, curInstrOffset, i, var);
				out.push_back((uint8_t)var.type);
				curInstrOffset++;
			},
			varcase(const InstrType::PUSH_OBJARR_U8&) {
				pushOpCodeByte(out, instrOffsets, curInstrOffset, i, var);
				constPoolIdxPushW(out, poolSize, consts, *var.ref);
				out.push_back(var.dims);
				curInstrOffset += 3;
			},

			varcase(const BaseBranched32 auto) {
				pushOpCodeByte(out, instrOffsets, curInstrOffset, i, var);
				u32w(out, var.jmpOffsetBytes);
				curInstrOffset += 4;
			},

			// Wide

			varcase(const BaseVar16Instred auto) {
				//TODO
			},

			varcase(const InstrType::ADD_I32_VAR_U16_CI16) {
				//TODO: check if it fits in non-wide form
				pushWideOpCodeId(out, instrOffsets, curInstrOffset, i,
					InstrId::I_ADD_I32_VAR_U8_CI8);
				//TODO
			},

			// Utilities

			varcase(const InstrType::PUSH_CONST&) {
				//TODO
			},

			varcase(const InstrType::PUSH_I32_I32) {
				//TODO
			},
			varcase(const InstrType::PUSH_F32_F32) {
				//TODO
			},
			varcase(const InstrType::PUSH_I64_I64) {
				//TODO
			},
			varcase(const InstrType::PUSH_F64_F64) {
				//TODO
			},

			varcase(const InstrType::GOTO) {
				if (var.jmpOffset > INT16_MAX || var.jmpOffset < INT16_MIN)
				{// Always 32
					pushOpCodeId(out, instrOffsets, curInstrOffset, i, 
						InstrId::I_GOTO32);
					u32w(out, 0);
					instrPatchPoints.push_back(PatchPoint{
						.instrOffset=var.jmpOffset,
						.is32Bit=true,
						.instrIdx=i,
						.byteOffset=(uint16_t)curInstrOffset
						});
					curInstrOffset += 4;
				}
				else
				{// Hope for 16
					pushOpCodeId(out, instrOffsets, curInstrOffset, i,
						InstrId::I_GOTO16);
					u16w(out, 0);
					instrPatchPoints.push_back(PatchPoint{
						.instrOffset = var.jmpOffset,
						.is32Bit = false,
						.instrIdx = i,
						.byteOffset = (uint16_t)curInstrOffset
						});
					curInstrOffset += 2;
				}
			},

			varcase(const BaseBranched auto) {
				//TODO
			}
			);
		}

		return out;
	}
}
