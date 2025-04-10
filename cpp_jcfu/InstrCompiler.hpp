/*
** See Copyright Notice inside Include.hpp
*/
#pragma once

#include <vector>
#include <span>
#include <bit>

#include "State.hpp"
#include "ext/CppMatch.hpp"
#include "WriteBin.hpp"
#include "Instrs.hpp"

namespace cpp_jcfu
{
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
		constexpr InstrId op = INSTR_OP_CODE<decltype(var)>;
		static_assert((uint8_t)op <= (uint8_t)InstrId::I_DEPR_JSR32);
		pushOpCodeId(out, instrOffsets, curInstrOffset, i, op);
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
		constexpr InstrId op = INSTR_OP_CODE<decltype(var)>;
		static_assert((uint8_t)op <= (uint8_t)InstrId::I_DEPR_JSR32);
		pushWideOpCodeId(out, instrOffsets, curInstrOffset, i, op);
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

	struct ErrorHandler
	{
		std::optional<ConstPoolItmType::CLASS> catchType; // None -> catch all
		uint16_t startInstr;
		uint16_t endInstr;
		uint16_t handlerInstr;
	};
	struct CodeCompileData
	{
		std::span<const Instr> instrs;
		std::span<const ErrorHandler> errorHandlers;
		std::vector<CodeTag> extraTags;
		uint16_t maxStack;
		uint16_t maxLocals;
	};
	inline FuncTagType::CODE compileCode(
		size_t& poolSize, ConstPool& consts,
		const CodeCompileData& data
	)
	{
		const std::span<const Instr> instrs = data.instrs;

		_ASSERT(instrs.size() < UINT16_MAX);

		std::vector<PatchPoint> instrPatchPoints;
		std::vector<uint16_t> instrOffsets(instrs.size());
		size_t curInstrOffset = 0;

		std::vector<uint8_t> out;
		out.reserve(instrs.size() + (instrs.size() >> 3)); // 1.125X scaling

		for (uint16_t i = 0; i < instrs.size(); i++)
		{
			const Instr& instr = instrs[i];

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
				u32w(out, uint32_t(uint32_t(var->min) + var->jmpOffsets.size() - 1));
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

				_ASSERT(var->cases.size() < UINT32_MAX);
				u32w(out, uint32_t(var->cases.size()));
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

					pushOpCodeId(out, instrOffsets, curInstrOffset, i, invertIfInstr(INSTR_OP_CODE<decltype(var)>));
					u16w(out, 1+4);//skip goto32

					out.push_back((uint8_t)InstrId::I_GOTO32);
					curInstrOffset += 3;
					writePatchPoint32(out, curInstrOffset, i, instrPatchPoints, var.jmpOffset);
					return;
				}
				pushOpCodeByte(out, instrOffsets, curInstrOffset, i, var);
				// Hope for 16
				writePatchPoint16(out,curInstrOffset,i, instrPatchPoints,var.jmpOffset);
			}
			);
		}
		_ASSERT(curInstrOffset <= UINT16_MAX);
		instrOffsets.push_back((uint16_t)curInstrOffset);//Prevent oob

		size_t ppOffset = 0;
		for (const PatchPoint& pp : instrPatchPoints)
		{
			const uint16_t relPoint = instrOffsets[pp.instrIdx + 1];

			const int32_t instrOffset = int32_t(pp.instrOffset << 1)>>1;//carry top bit
			const int32_t movement = instrOffsets[pp.instrIdx+instrOffset] - int32_t(relPoint);

			if (pp.is32Bit)
			{//Ez
				u32Patch(out, pp.byteOffset + ppOffset, movement);
				continue;
			}

			if (movement >= INT16_MIN && movement <= INT16_MAX)
			{//Ez 16 bit move
				u16Patch(out, pp.byteOffset + ppOffset, (int16_t)movement);
				continue;
			}
			// Uh oh!!! need to upsize!

			InstrId& instr = reinterpret_cast<InstrId&>(out[pp.byteOffset + ppOffset - 1]);

			if (instr == InstrId::I_GOTO16)
			{
				instr = InstrId::I_GOTO32;

				out.insert(out.begin() + (pp.byteOffset + ppOffset), 2, 0);
				u32Patch(out, pp.byteOffset + ppOffset, movement+2);//+2, for added size, since calculation

				for (size_t i = pp.instrIdx+1; i < instrOffsets.size(); i++)
					instrOffsets[i] += 2;
				ppOffset += 2;
				continue;
			}
			// Its an if

			instr = invertIfInstr(instr);
			u16Patch(out, pp.byteOffset + ppOffset, 1+4);//skip injected goto32

			out.insert(out.begin() + (pp.byteOffset + ppOffset+2), 5, 
				(uint8_t)InstrId::I_GOTO32);//use goto32, to auto fill in the opcode

			u32Patch(out, 
				pp.byteOffset + ppOffset+3, //+3, cuz we writing to the goto32's offset
				movement + 5);//+5, for added size, since calculation

			for (size_t i = pp.instrIdx + 1; i < instrOffsets.size(); i++)
				instrOffsets[i] += 5;
			ppOffset += 5;
		}

		FuncTagType::CODE ret;
		ret.bytecode = std::move(out);
		ret.maxLocals = data.maxLocals;
		ret.maxStack = data.maxStack;

		ret.tags = data.extraTags;

		ret.errorHandlers.resize(data.errorHandlers.size());
		for (size_t i = 0; i < data.errorHandlers.size(); i++)
		{
			const ErrorHandler& mh = data.errorHandlers[i];
			CodeTagErrorHandler& eh = ret.errorHandlers[i];

			eh.catchType = mh.catchType;
			eh.startByte = instrOffsets[mh.startInstr];
			eh.afterEndByte = instrOffsets[mh.endInstr+1];
			eh.handlerByte = instrOffsets[mh.handlerInstr];
		}

		return ret;
	}
}
