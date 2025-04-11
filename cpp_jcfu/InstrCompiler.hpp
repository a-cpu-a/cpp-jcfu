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
		static_assert(op <= (uint8_t)InstrId::I_DEPR_JSR32);
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
	inline void pushWideOpCodeByte(
		std::vector<uint8_t>& out,
		std::vector<uint16_t>& instrOffsets,
		size_t& curInstrOffset,
		const uint16_t i,
		const auto& var)
	{
		constexpr uint8_t op = INSTR_OP_CODE<decltype(var)>;
		static_assert(op <= (uint8_t)InstrId::I_DEPR_JSR32);
		pushWideOpCodeId(out, instrOffsets, curInstrOffset, i, (InstrId)op);
	}

	inline void pushConstPoolInstrW(
		std::vector<uint8_t>& out,
		std::vector<uint16_t>& instrOffsets,
		size_t& curInstrOffset,
		const uint16_t i,
		size_t& poolSize, ConstPool& consts,
		ConstPoolItm&& itm)
	{
		const bool isBig = isPoolItemBig(itm);
		const uint16_t idx = constPoolPush(poolSize, consts, std::move(itm));

		if (isBig)
		{
			pushOpCodeId(out, instrOffsets, curInstrOffset, i,
				InstrId::I_PUSH_CONST2_U16);
			u16w(out, idx);
			curInstrOffset += 2;
			return;
		}

		if (idx <= UINT8_MAX)
		{
			pushOpCodeId(out, instrOffsets, curInstrOffset, i,
				InstrId::I_PUSH_CONST_U8);
			out.push_back((uint8_t)idx);
			curInstrOffset++;
			return;
		}

		pushOpCodeId(out, instrOffsets, curInstrOffset, i,
			InstrId::I_PUSH_CONST_U16);
		u16w(out, idx);
		curInstrOffset += 2;
	}

	struct PatchPoint
	{
		uint32_t instrOffset : 31;//Packed!!!
		uint32_t is32Bit : 1 = false;

		uint16_t instrIdx;
		uint16_t byteOffset;
	};

	inline void writePatchPoint32(
		std::vector<uint8_t>& out,
		size_t& curInstrOffset,
		const uint16_t i,
		std::vector<PatchPoint>& instrPatchPoints,
		const int32_t jmpOffset)
	{
		_ASSERT(curInstrOffset <= UINT16_MAX - 4);
		u32w(out, 0);
		instrPatchPoints.emplace_back(
			(uint32_t)jmpOffset,
			true, i,
			(uint16_t)curInstrOffset
		);
		curInstrOffset += 4;
	}

	inline void writePatchPoint16(
		std::vector<uint8_t>& out,
		size_t& curInstrOffset,
		const uint16_t i,
		std::vector<PatchPoint>& instrPatchPoints,
		const int32_t jmpOffset)
	{
		_ASSERT(curInstrOffset <= UINT16_MAX - 2);
		u16w(out, 0);
		instrPatchPoints.emplace_back(
			(uint32_t)jmpOffset,
			false, i,
			(uint16_t)curInstrOffset
		);
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
		&& !BaseVar16Instred<T>
		&& !BaseBranched<T>
		&& !PushConstXed<T>;

	inline std::vector<uint8_t> compileInstrs(
		size_t& poolSize, ConstPool& consts,
		const std::vector<Instr>& instrs
	)
	{
		_ASSERT(instrs.size() < UINT16_MAX);

		//TODO: build list of bytes to patch into relative instr offsets: (switch, 32bit if's)
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

			varcase(const BaseBranched16 auto) {
				pushOpCodeByte(out, instrOffsets, curInstrOffset, i, var);
				u16w(out, var.jmpOffsetBytes);
				curInstrOffset += 2;
			},

			varcase(const InstrType::TABLE_SWITCH&) {
				pushOpCodeByte(out, instrOffsets, curInstrOffset, i, var);

				const uint8_t padBytes = (4 - (out.size() % 4)) % 4;
				out.insert(out.end(), padBytes, 0);
				curInstrOffset += padBytes;

				writePatchPoint32(out, curInstrOffset, i, instrPatchPoints, var->defaultJmpOffset);
				u32w(out, var->min);
				u32w(out, var->min+var->jmpOffsets.size()-1);
				curInstrOffset += 8;

				for (const int32_t jmpOffset : var->jmpOffsets)
				{
					writePatchPoint32(out, curInstrOffset, i, instrPatchPoints, jmpOffset);
				}

			},
			varcase(const InstrType::LOOKUP_SWITCH&) {
				pushOpCodeByte(out, instrOffsets, curInstrOffset, i, var);

				const uint8_t padBytes = (4 - (out.size() % 4))%4;
				out.insert(out.end(), padBytes, 0);
				curInstrOffset += padBytes;

				writePatchPoint32(out, curInstrOffset, i, instrPatchPoints, var->defaultJmpOffset);
				u32w(out, var->cases.size());
				curInstrOffset += 4;

				for (const SwitchCase& kase : var->cases)
				{
					u32w(out, kase.k);
					curInstrOffset += 4;
					writePatchPoint32(out, curInstrOffset, i, instrPatchPoints, kase.jmpOffset);
				}
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
				if (var.varIdx <= UINT8_MAX)
				{
					pushOpCodeByte(out, instrOffsets, curInstrOffset, i, var);
					out.push_back((uint8_t)var.varIdx);
					curInstrOffset++;
					return;
				}
				pushWideOpCodeByte(out, instrOffsets, curInstrOffset, i, var);
				u16w(out, var.varIdx);
				curInstrOffset += 2;
			},

			varcase(const InstrType::ADD_I32_VAR_U16_CI16) {
				if (var.varIdx <= UINT8_MAX
					&& var.val <= INT8_MAX
					&& var.val >= INT8_MIN)
				{
					pushOpCodeId(out, instrOffsets, curInstrOffset, i,
						InstrId::I_ADD_I32_VAR_U8_CI8);
					out.push_back((uint8_t)var.varIdx);
					out.push_back((int8_t)var.val);
					curInstrOffset += 2;
					return;
				}
				pushWideOpCodeId(out, instrOffsets, curInstrOffset, i,
					InstrId::I_ADD_I32_VAR_U8_CI8);
				u16w(out,var.varIdx);
				u16w(out,var.val);
				curInstrOffset += 4;
			},

			// Utilities

			varcase(const InstrType::PUSH_CONST&) {
				pushConstPoolInstrW(out, 
					instrOffsets,curInstrOffset, i, 
					poolSize, consts, 
					ConstPoolItm(*var));
			},

			varcase(const InstrType::PUSH_I32_I32) {
				if (var <= INT8_MAX
					&& var >= INT8_MIN)
				{// Small, so opcode, or i8
					if (var <= 5
						&& var >= -1)
					{// Tiny, so use a opcode
						pushOpCodeId(out, instrOffsets, curInstrOffset, i,
							InstrId((int8_t)InstrId::PUSH_I32_0 + (int8_t)var));
						return;
					}
					pushOpCodeId(out, instrOffsets, curInstrOffset, i,
						InstrId::PUSH_I32_I8);
					out.push_back((int8_t)var);
					curInstrOffset++;
					return;
				}
				// Not i8
				if (var <= INT16_MAX
					&& var >= INT16_MIN)
				{// Short, so i16
					pushOpCodeId(out, instrOffsets, curInstrOffset, i,
						InstrId::PUSH_I32_I16);
					u16w(out, (int16_t)var);
					curInstrOffset += 2;
					return;
				}
				// Not i16 !!

				pushConstPoolInstrW(out,
					instrOffsets, curInstrOffset, i,
					poolSize, consts,
					ConstPoolItmType::I32(var));

			},
			varcase(const InstrType::PUSH_F32_F32) {
				if (var == 0.0f || var == 1.0f || var == 2.0f)
				{
					pushOpCodeId(out, instrOffsets, curInstrOffset, i,
						InstrId((uint8_t)InstrId::PUSH_F32_0 + (uint8_t)var));
					return;
				}

				pushConstPoolInstrW(out,
					instrOffsets, curInstrOffset, i,
					poolSize, consts,
					ConstPoolItmType::F32(var));
			},
			varcase(const InstrType::PUSH_I64_I64) {
				if (var == 0 || var==1)
				{
					pushOpCodeId(out, instrOffsets, curInstrOffset, i,
						InstrId((uint8_t)InstrId::PUSH_I64_0 + (uint8_t)var));
					return;
				}

				pushConstPoolInstrW(out,
					instrOffsets, curInstrOffset, i,
					poolSize, consts,
					ConstPoolItmType::I64(var));
			},
			varcase(const InstrType::PUSH_F64_F64) {
				if (var == 0.0 || var == 1.0)
				{
					pushOpCodeId(out, instrOffsets, curInstrOffset, i,
						InstrId((uint8_t)InstrId::PUSH_F64_0 + (uint8_t)var));
					return;
				}

				pushConstPoolInstrW(out,
					instrOffsets, curInstrOffset, i,
					poolSize, consts,
					ConstPoolItmType::F64(var));
			},

			varcase(const InstrType::GOTO) {
				if (var.jmpOffset > INT16_MAX || var.jmpOffset < INT16_MIN)
				{// Always 32
					pushOpCodeId(out, instrOffsets, curInstrOffset, i, 
						InstrId::I_GOTO32);
					writePatchPoint32(out, curInstrOffset, i, instrPatchPoints, var.jmpOffset);
					return;
				}
				// Hope for 16
				pushOpCodeId(out, instrOffsets, curInstrOffset, i,
					InstrId::I_GOTO16);
				writePatchPoint16(out, curInstrOffset, i, instrPatchPoints, var.jmpOffset);
			},

			varcase(const BaseBranched auto) {
				if (var.jmpOffset > INT16_MAX || var.jmpOffset < INT16_MIN)
				{// Always 32
					_ASSERT(false && "TODO");//TODO
					return;
				}
				// Hope for 16
				pushOpCodeByte(out, instrOffsets, curInstrOffset, i, var);
				writePatchPoint16(out,curInstrOffset,i, instrPatchPoints,var.jmpOffset);

			}
			);
		}

		return out;
	}
}
