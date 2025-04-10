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
	consteval uint8_t getInstrOpCode() {
		const uint8_t idx = aca::variant_index_v<T, Instr>;
		if (idx > 0xc9)
			throw "Error, not a valid op code!";
		return idx;
	}

	template<class T>
	constexpr uint8_t INSTR_OP_CODE = getInstrOpCode<T>();

	template<class T>
	concept BaseBranched16 = std::derived_from<T, InstrType::BaseBranch16>;
	template<class T>
	concept BaseBranched32 = std::derived_from<T, InstrType::BaseBranch32>;
	template<class T>
	concept BaseFieldRefed = std::derived_from<T, InstrType::BaseFieldRef>;
	template<class T>
	concept BaseVarInstred = sizeof(T) == 1
		&& std::derived_from<T, InstrType::BaseVarInstr>;
	template<class T>
	concept BaseVar16Instred = sizeof(T) == 2
		&& std::derived_from<T, InstrType::BaseVar16Instr>;

	template<class T>
	concept BasicOpCode = sizeof(T)==1
		&& !BaseBranched16<T>
		&& !BaseBranched32<T>
		&& !BaseFieldRefed<T>
		&& !BaseVarInstred<T>
		&& !BaseVar16Instred<T>;

	inline std::vector<uint8_t> compileInstrs(
		size_t& poolSize, ConstPool& consts,
		const std::vector<Instr>& instrs
	)
	{
		//TODO: build list of instr offsets
		//TODO: build list of bytes to patch into relative instr offsets
		//TODO: solve relative instr sizes

		std::vector<uint16_t> instrOffsets(instrs.size());
		size_t curInstrOffset = 0;

		std::vector<uint8_t> out;
		out.reserve(instrs.size() + (instrs.size() >> 3)); // 1.125X scaling


		for (size_t i = 0; i < instrs.size(); i++)
		{
			ezmatch(instrs[i])(

				//TODO: hard ones (has data -> hard)
				//		or, isnt basic op code			

			varcase(const InstrType::I_PUSH_I32_I8) {
				//TODO
			},
			varcase(const InstrType::I_PUSH_I32_I16) {
				//TODO
			},

			varcase(const InstrType::I_PUSH_CONST_U8) {
				//TODO
			},
			varcase(const InstrType::I_PUSH_CONST_U16) {
				//TODO
			},
			varcase(const InstrType::I_PUSH_CONST2_U16) {
				//TODO
			},

			varcase(const BaseVarInstred auto) {
				//TODO
			},

			varcase(const InstrType::I_ADD_I32_VAR_U8_CI8) {
				//TODO
			},

			varcase(const BaseBranched16 auto) {
				//TODO
			},

			varcase(const InstrType::TABLE_SWITCH&) {
				//TODO
			},
			varcase(const InstrType::LOOKUP_SWITCH&) {
				//TODO
			},

			varcase(const BaseFieldRefed auto&) {
				//TODO
			},

			varcase(const InstrType::PUSH_RUN_VIRTUAL&) {
				//TODO
			},
			varcase(const InstrType::PUSH_RUN_SPECIAL&) {
				//TODO
			},
			varcase(const InstrType::PUSH_RUN_STATIC&) {
				//TODO
			},
			varcase(const InstrType::PUSH_RUN_INTERFACE&) {
				//TODO
			},
			varcase(const InstrType::PUSH_RUN_DYN&) {
				//TODO
			},

			varcase(const InstrType::PUSH_OBJ&) {
				//TODO
			},
			varcase(const InstrType::PUSH_ARR) {
				//TODO
			},
			varcase(const InstrType::PUSH_OBJARR_1&) {
				//TODO
			},
			varcase(const InstrType::PUSH_OBJARR_U8&) {
				//TODO
			},

			varcase(const InstrType::CHECK_CAST&) {
				//TODO
			},
			varcase(const InstrType::IS_OF&) {
				//TODO
			},

			// Wide

			varcase(const BaseVar16Instred auto) {
				//TODO
			},

			varcase(const InstrType::ADD_I32_VAR_U16_CI16) {
				//TODO
			},

			varcase(const BaseBranched32 auto) {
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
				//TODO
			},

			varcase(const InstrType::IF_EQL) {
				//TODO
			},
			varcase(const InstrType::IF_NEQ) {
				//TODO
			},
			varcase(const InstrType::IF_LT) {
				//TODO
			},
			varcase(const InstrType::IF_GT) {
				//TODO
			},
			varcase(const InstrType::IF_LTE) {
				//TODO
			},
			varcase(const InstrType::IF_GTE) {
				//TODO
			},

			varcase(const InstrType::IF_I32_EQL) {
				//TODO
			},
			varcase(const InstrType::IF_I32_NEQ) {
				//TODO
			},
			varcase(const InstrType::IF_I32_LT) {
				//TODO
			},
			varcase(const InstrType::IF_I32_GT) {
				//TODO
			},
			varcase(const InstrType::IF_I32_LTE) {
				//TODO
			},
			varcase(const InstrType::IF_I32_GTE) {
				//TODO
			},

			varcase(const InstrType::IF_OBJ_EQL) {
				//TODO
			},
			varcase(const InstrType::IF_OBJ_NEQ) {
				//TODO
			},
			varcase(const InstrType::IF_NIL) {
				//TODO
			},
			varcase(const InstrType::IF_NNIL) {
				//TODO
			},




				// Easy 1 byte instructions
			varcase(const BasicOpCode auto) {
				out.push_back(INSTR_OP_CODE<decltype(var)>);
				_ASSERT(curInstrOffset < UINT16_MAX);
				instrOffsets[i] = uint16_t(curInstrOffset++);
			}
			);
		}

		return out;
	}
}
