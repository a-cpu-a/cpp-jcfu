/*
** See Copyright Notice inside Include.hpp
*/
#pragma once

#include <vector>
#include <variant>
#include <memory>

#include "State.hpp"
#include "ext/ExtendVariant.hpp"

namespace cpp_jcfu
{
	enum class InstrId : uint8_t
	{
		NOP,
		PUSH_OBJ_NULL,

		PUSH_I32_M1,
		PUSH_I32_0,
		PUSH_I32_1,
		PUSH_I32_2,
		PUSH_I32_3,
		PUSH_I32_4,
		PUSH_I32_5,

		PUSH_I64_0,
		PUSH_I64_1,

		PUSH_F32_0,
		PUSH_F32_1,
		PUSH_F32_2,

		PUSH_F64_0,
		PUSH_F64_1,

		I_PUSH_I32_I8,
		I_PUSH_I32_I16,

		I_PUSH_CONST_U8,
		I_PUSH_CONST_U16,
		I_PUSH_CONST2_U16,

		PUSH_I32_VAR_U16,
		PUSH_I64_VAR_U16,
		PUSH_F32_VAR_U16,
		PUSH_F64_VAR_U16,
		PUSH_OBJ_VAR_U16,

		I_PUSH_I32_VAR_0,
		I_PUSH_I32_VAR_1,
		I_PUSH_I32_VAR_2,
		I_PUSH_I32_VAR_3,
		I_PUSH_I64_VAR_0,
		I_PUSH_I64_VAR_1,
		I_PUSH_I64_VAR_2,
		I_PUSH_I64_VAR_3,

		I_PUSH_F32_VAR_0,
		I_PUSH_F32_VAR_1,
		I_PUSH_F32_VAR_2,
		I_PUSH_F32_VAR_3,
		I_PUSH_F64_VAR_0,
		I_PUSH_F64_VAR_1,
		I_PUSH_F64_VAR_2,
		I_PUSH_F64_VAR_3,

		I_PUSH_OBJ_VAR_0,
		I_PUSH_OBJ_VAR_1,
		I_PUSH_OBJ_VAR_2,
		I_PUSH_OBJ_VAR_3,

		PUSH_I32_ARR,
		PUSH_I64_ARR,
		PUSH_F32_ARR,
		PUSH_F64_ARR,
		PUSH_OBJ_ARR,

		PUSH_BI8_ARR,
		PUSH_CHR_ARR,
		PUSH_I16_ARR,

		SAVE_I32_VAR_U16,
		SAVE_I64_VAR_U16,
		SAVE_F32_VAR_U16,
		SAVE_F64_VAR_U16,
		SAVE_OBJ_VAR_U16,

		I_SAVE_I32_VAR_0,
		I_SAVE_I32_VAR_1,
		I_SAVE_I32_VAR_2,
		I_SAVE_I32_VAR_3,
		I_SAVE_I64_VAR_0,
		I_SAVE_I64_VAR_1,
		I_SAVE_I64_VAR_2,
		I_SAVE_I64_VAR_3,

		I_SAVE_F32_VAR_0,
		I_SAVE_F32_VAR_1,
		I_SAVE_F32_VAR_2,
		I_SAVE_F32_VAR_3,
		I_SAVE_F64_VAR_0,
		I_SAVE_F64_VAR_1,
		I_SAVE_F64_VAR_2,
		I_SAVE_F64_VAR_3,

		I_SAVE_OBJ_VAR_0,
		I_SAVE_OBJ_VAR_1,
		I_SAVE_OBJ_VAR_2,
		I_SAVE_OBJ_VAR_3,

		SAVE_I32_ARR,
		SAVE_I64_ARR,
		SAVE_F32_ARR,
		SAVE_F64_ARR,
		SAVE_OBJ_ARR,

		SAVE_BI8_ARR,
		SAVE_CHR_ARR,
		SAVE_I16_ARR,

		POP_1,
		POP_2,

		DUP_1,
		DUP_1_X,
		DUP_1_X2,
		DUP_2,
		DUP_2_X,
		DUP_2_X2,

		SWAP,

		ADD_I32,
		ADD_I64,
		ADD_F32,
		ADD_F64,
		SUB_I32,
		SUB_I64,
		SUB_F32,
		SUB_F64,

		MUL_I32,
		MUL_I64,
		MUL_F32,
		MUL_F64,
		DIV_I32,
		DIV_I64,
		DIV_F32,
		DIV_F64,
		REM_I32,
		REM_I64,
		REM_F32,
		REM_F64,

		NEG_I32,
		NEG_I64,
		NEG_F32,
		NEG_F64,

		SHL_I32,
		SHL_I64,
		SRC_I32,
		SRC_I64,
		SHR_I32,
		SHR_I64,

		AND_I32,
		AND_I64,
		OR_I32,
		OR_I64,
		XOR_I32,
		XOR_I64,

		I_ADD_I32_VAR_U8_CI8,

		CAST_I32_I64,
		CAST_I32_F32,
		CAST_I32_F64,
		CAST_I64_I32,
		CAST_I64_F32,
		CAST_I64_F64,

		CAST_F32_I32,
		CAST_F32_I64,
		CAST_F32_F64,
		CAST_F64_I32,
		CAST_F64_I64,
		CAST_F64_F32,

		CAST_I32_I8,
		CAST_I32_CHR,
		CAST_I32_I16,

		CMP_I64,
		CMP_F32_M,
		CMP_F32_P,
		CMP_F64_M,
		CMP_F64_P,

		IF_EQL,
		IF_NEQ,
		IF_LT,
		IF_GTE,
		IF_GT,
		IF_LTE,
		IF_I32_EQL,
		IF_I32_NEQ,
		IF_I32_LT,
		IF_I32_GTE,
		IF_I32_GT,
		IF_I32_LTE,

		IF_OBJ_EQL,
		IF_OBJ_NEQ,

		I_GOTO16,
		I_DEPR_JSR16,
		I_DEPR_GOTO_VAR_U16,

		TABLE_SWITCH,
		LOOKUP_SWITCH,

		RET_I32,
		RET_I64,
		RET_F32,
		RET_F64,
		RET_OBJ,
		RET,

		PUSH_GET_STATIC,
		SAVE_STATIC,
		PUSH_GET_FIELD,
		SAVE_FIELD,

		PUSH_RUN_VIRTUAL,
		PUSH_RUN_SPECIAL,
		PUSH_RUN_STATIC,
		PUSH_RUN_INTERFACE,
		PUSH_RUN_DYN,

		PUSH_OBJ,
		PUSH_ARR,
		PUSH_OBJARR_1,

		PUSH_ARRLEN,

		THROW,

		CHECK_CAST,
		IS_OF,

		SYNC_ON,
		SYNC_OFF,

		I_WIDE,

		PUSH_OBJARR_U8,

		IF_NIL,
		IF_NNIL,

		I_GOTO32,
		I_DEPR_JSR32,
	};

	struct SwitchCase
	{
		int32_t k;
		int32_t jmpOffset;
	};
	enum class ArrayType : uint8_t
	{
		BOOL = 4,
		CHR = 5,
		F32 = 6,
		D64 = 7,
		I8 = 8,
		I16 = 9,
		I32 = 10,
		I64 = 11,
	};

	namespace InstrType
	{
		struct BaseVar16Instr
		{
			uint16_t varIdx;
		};
		struct BaseBranch16
		{
			int16_t jmpOffsetBytes;//In bytes
		};
		struct BaseBranch32
		{
			int32_t jmpOffsetBytes;//In bytes
		};
		struct BaseBranch
		{
			int32_t jmpOffset;//In instructions, not bytes
		};
		struct BaseFieldRef
		{
			std::unique_ptr<ConstPoolItmType::FIELD_REF> ref;
		};
		struct BaseFuncRef
		{
			std::unique_ptr<ConstPoolItmType::FUNC_REF> ref;
		};
		struct BaseClassRef
		{
			std::unique_ptr<ConstPoolItmType::CLASS> ref;
		};

		using NOP = std::monostate;

		struct PUSH_OBJ_NULL {};

		struct PUSH_I32_M1 {}; // -1
		struct PUSH_I32_0 {};
		struct PUSH_I32_1 {};
		struct PUSH_I32_2 {};
		struct PUSH_I32_3 {};
		struct PUSH_I32_4 {};
		struct PUSH_I32_5 {};

		using I_PUSH_I32_I8 = int8_t;
		using I_PUSH_I32_I16 = int16_t;

		struct PUSH_F32_0 {};
		struct PUSH_F32_1 {};
		struct PUSH_F32_2 {};

		struct PUSH_I64_0 {};
		struct PUSH_I64_1 {};

		struct PUSH_F64_0 {};
		struct PUSH_F64_1 {};

		struct I_PUSH_CONST_U8 { uint8_t poolIdx; };
		struct I_PUSH_CONST_U16 { uint16_t poolIdx; };
		struct I_PUSH_CONST2_U16 { uint16_t poolIdx; };

		struct PUSH_I32_VAR_U16 : BaseVar16Instr {};
		struct PUSH_F32_VAR_U16 : BaseVar16Instr {};
		struct PUSH_I64_VAR_U16 : BaseVar16Instr {};
		struct PUSH_F64_VAR_U16 : BaseVar16Instr {};
		struct PUSH_OBJ_VAR_U16 : BaseVar16Instr {};

		struct I_PUSH_I32_VAR_0 {};
		struct I_PUSH_I32_VAR_1 {};
		struct I_PUSH_I32_VAR_2 {};
		struct I_PUSH_I32_VAR_3 {};
		struct I_PUSH_F32_VAR_0 {};
		struct I_PUSH_F32_VAR_1 {};
		struct I_PUSH_F32_VAR_2 {};
		struct I_PUSH_F32_VAR_3 {};

		struct I_PUSH_I64_VAR_0 {};
		struct I_PUSH_I64_VAR_1 {};
		struct I_PUSH_I64_VAR_2 {};
		struct I_PUSH_I64_VAR_3 {};
		struct I_PUSH_F64_VAR_0 {};
		struct I_PUSH_F64_VAR_1 {};
		struct I_PUSH_F64_VAR_2 {};
		struct I_PUSH_F64_VAR_3 {};

		struct I_PUSH_OBJ_VAR_0 {};
		struct I_PUSH_OBJ_VAR_1 {};
		struct I_PUSH_OBJ_VAR_2 {};
		struct I_PUSH_OBJ_VAR_3 {};

		struct PUSH_I32_ARR {};
		struct PUSH_F32_ARR {};
		struct PUSH_I64_ARR {};
		struct PUSH_F64_ARR {};
		struct PUSH_OBJ_ARR {};

		struct PUSH_BI8_ARR {};// bool or i8
		struct PUSH_CHR_ARR {};
		struct PUSH_I16_ARR {};

		struct SAVE_I32_VAR_U16 : BaseVar16Instr {};
		struct SAVE_F32_VAR_U16 : BaseVar16Instr {};
		struct SAVE_I64_VAR_U16 : BaseVar16Instr {};
		struct SAVE_F64_VAR_U16 : BaseVar16Instr {};
		struct SAVE_OBJ_VAR_U16 : BaseVar16Instr {};

		struct I_SAVE_I32_VAR_0 {};
		struct I_SAVE_I32_VAR_1 {};
		struct I_SAVE_I32_VAR_2 {};
		struct I_SAVE_I32_VAR_3 {};
		struct I_SAVE_I64_VAR_0 {};
		struct I_SAVE_I64_VAR_1 {};
		struct I_SAVE_I64_VAR_2 {};
		struct I_SAVE_I64_VAR_3 {};

		struct I_SAVE_F32_VAR_0 {};
		struct I_SAVE_F32_VAR_1 {};
		struct I_SAVE_F32_VAR_2 {};
		struct I_SAVE_F32_VAR_3 {};
		struct I_SAVE_F64_VAR_0 {};
		struct I_SAVE_F64_VAR_1 {};
		struct I_SAVE_F64_VAR_2 {};
		struct I_SAVE_F64_VAR_3 {};

		struct I_SAVE_OBJ_VAR_0 {};
		struct I_SAVE_OBJ_VAR_1 {};
		struct I_SAVE_OBJ_VAR_2 {};
		struct I_SAVE_OBJ_VAR_3 {};

		struct SAVE_I32_ARR {};
		struct SAVE_F32_ARR {};
		struct SAVE_I64_ARR {};
		struct SAVE_F64_ARR {};
		struct SAVE_OBJ_ARR {};

		struct SAVE_BI8_ARR {};// bool or i8
		struct SAVE_CHR_ARR {};
		struct SAVE_I16_ARR {};

		struct POP_1 {};
		struct POP_2 {};

		struct DUP_1 {};
		struct DUP_1_X {};
		struct DUP_1_X2 {};
		struct DUP_2 {};
		struct DUP_2_X {};
		struct DUP_2_X2 {};

		struct SWAP {};

		struct ADD_I32 {};
		struct ADD_F32 {};
		struct ADD_I64 {};
		struct ADD_F64 {};
		struct SUB_I32 {};
		struct SUB_F32 {};
		struct SUB_I64 {};
		struct SUB_F64 {};

		struct MUL_I32 {};
		struct MUL_F32 {};
		struct MUL_I64 {};
		struct MUL_F64 {};
		struct DIV_I32 {};
		struct DIV_F32 {};
		struct DIV_I64 {};
		struct DIV_F64 {};
		struct REM_I32 {};
		struct REM_F32 {};
		struct REM_I64 {};
		struct REM_F64 {};

		struct NEG_I32 {};
		struct NEG_F32 {};
		struct NEG_I64 {};
		struct NEG_F64 {};

		struct SHL_I32 {};
		struct SHL_I64 {};
		struct SRC_I32 {};
		struct SRC_I64 {};
		struct SHR_I32 {};
		struct SHR_I64 {};

		struct AND_I32 {};
		struct AND_I64 {};
		struct OR_I32 {};
		struct OR_I64 {};
		struct XOR_I32 {};
		struct XOR_I64 {};

		struct ADD_I32_VAR_U16_CI16 :BaseVar16Instr { int16_t val; };

		struct CAST_I32_I8 {};
		struct CAST_I32_CHR {};
		struct CAST_I32_I16 {};

		struct CAST_I32_F32 {};
		struct CAST_I32_I64 {};
		struct CAST_I32_F64 {};
		struct CAST_F32_I32 {};
		struct CAST_F32_I64 {};
		struct CAST_F32_F64 {};

		struct CAST_I64_I32 {};
		struct CAST_I64_F32 {};
		struct CAST_I64_F64 {};
		struct CAST_F64_I32 {};
		struct CAST_F64_F32 {};
		struct CAST_F64_I64 {};

		struct CMP_F32_M {};
		struct CMP_F32_P {};
		struct CMP_I64 {};
		struct CMP_F64_M {};
		struct CMP_F64_P {};

		struct IF_EQL :BaseBranch {};
		struct IF_NEQ :BaseBranch {};
		struct IF_LT :BaseBranch {};
		struct IF_GT :BaseBranch {};
		struct IF_LTE :BaseBranch {};
		struct IF_GTE :BaseBranch {};

		struct IF_I32_EQL :BaseBranch {};
		struct IF_I32_NEQ :BaseBranch {};
		struct IF_I32_LT :BaseBranch {};
		struct IF_I32_GT :BaseBranch {};
		struct IF_I32_LTE :BaseBranch {};
		struct IF_I32_GTE :BaseBranch {};

		struct IF_OBJ_EQL :BaseBranch {};
		struct IF_OBJ_NEQ :BaseBranch {};
		struct IF_NIL :BaseBranch {};
		struct IF_NNIL :BaseBranch {};

		struct I_GOTO16 :BaseBranch16 {};
		struct I_DEPR_JSR16 :BaseBranch16 {};
		struct I_DEPR_GOTO_VAR_U16 :BaseVar16Instr {};

		struct TableSwitchData
		{
			std::vector<int32_t> jmpOffsets;
			int32_t defaultJmpOffset;
			int32_t min;
			//max - calc from min + jmpOffsets.size()
		};
		using TABLE_SWITCH = std::unique_ptr<TableSwitchData>;

		struct LookupSwitchData
		{
			std::vector<SwitchCase> cases;
			int32_t defaultJmpOffset;
		};
		using LOOKUP_SWITCH = std::unique_ptr<LookupSwitchData>;

		struct RET_I32 {};
		struct RET_F32 {};
		struct RET_I64 {};
		struct RET_F64 {};
		struct RET_OBJ {};
		struct RET {};

		struct PUSH_GET_STATIC :BaseFieldRef {};
		struct PUSH_GET_FIELD :BaseFieldRef {};
		struct SAVE_STATIC :BaseFieldRef {};
		struct SAVE_FIELD :BaseFieldRef {};

		struct PUSH_RUN_VIRTUAL :BaseFuncRef {};
		struct PUSH_RUN_SPECIAL :BaseFuncRef {};
		struct PUSH_RUN_STATIC :BaseFuncRef {};
		struct PUSH_RUN_INTERFACE :BaseFuncRef { uint8_t argCount; };
		struct PUSH_RUN_DYN :BaseFuncRef {};

		struct PUSH_OBJ :BaseClassRef {};
		struct PUSH_ARR { ArrayType type; };
		struct PUSH_OBJARR_1 :BaseClassRef {};
		struct PUSH_OBJARR_U8 :BaseClassRef { uint8_t dims; };
		struct PUSH_ARRLEN {};

		struct THROW {};

		struct CHECK_CAST :BaseClassRef {};
		struct IS_OF :BaseClassRef {};

		struct SYNC_ON {};
		struct SYNC_OFF {};

		struct I_WIDE {};

		struct I_GOTO32 :BaseBranch32 {};
		struct I_DEPR_JSR32 :BaseBranch32 {};

		// Utility

		using PUSH_CONST = std::unique_ptr<ConstPoolItm>;

		using PUSH_I32_I32 = int32_t;	//Will be converted
		using PUSH_F32_F32 = float;		//Will be converted
		using PUSH_I64_I64 = int64_t;	//Will be converted
		using PUSH_F64_F64 = double;	//Will be converted

		struct GOTO :BaseBranch {};
	}
	using Instr = std::variant<
		InstrType::NOP,
		InstrType::PUSH_OBJ_NULL,

		InstrType::PUSH_I32_M1,
		InstrType::PUSH_I32_0,
		InstrType::PUSH_I32_1,
		InstrType::PUSH_I32_2,
		InstrType::PUSH_I32_3,
		InstrType::PUSH_I32_4,
		InstrType::PUSH_I32_5,

		InstrType::PUSH_I64_0,
		InstrType::PUSH_I64_1,

		InstrType::PUSH_F32_0,
		InstrType::PUSH_F32_1,
		InstrType::PUSH_F32_2,

		InstrType::PUSH_F64_0,
		InstrType::PUSH_F64_1,

		InstrType::I_PUSH_I32_I8,
		InstrType::I_PUSH_I32_I16,

		InstrType::I_PUSH_CONST_U8,//ldc
		InstrType::I_PUSH_CONST_U16,//ldc_w(for constants > ff)
		InstrType::I_PUSH_CONST2_U16,//ldc2_w (for long / double)

		InstrType::PUSH_I32_VAR_U16,//iload
		InstrType::PUSH_I64_VAR_U16,//lload
		InstrType::PUSH_F32_VAR_U16,//fload
		InstrType::PUSH_F64_VAR_U16,//dload
		InstrType::PUSH_OBJ_VAR_U16,//aload

		InstrType::I_PUSH_I32_VAR_0,
		InstrType::I_PUSH_I32_VAR_1,
		InstrType::I_PUSH_I32_VAR_2,
		InstrType::I_PUSH_I32_VAR_3,
		InstrType::I_PUSH_I64_VAR_0,
		InstrType::I_PUSH_I64_VAR_1,
		InstrType::I_PUSH_I64_VAR_2,
		InstrType::I_PUSH_I64_VAR_3,

		InstrType::I_PUSH_F32_VAR_0,
		InstrType::I_PUSH_F32_VAR_1,
		InstrType::I_PUSH_F32_VAR_2,
		InstrType::I_PUSH_F32_VAR_3,
		InstrType::I_PUSH_F64_VAR_0,
		InstrType::I_PUSH_F64_VAR_1,
		InstrType::I_PUSH_F64_VAR_2,
		InstrType::I_PUSH_F64_VAR_3,

		InstrType::I_PUSH_OBJ_VAR_0,
		InstrType::I_PUSH_OBJ_VAR_1,
		InstrType::I_PUSH_OBJ_VAR_2,
		InstrType::I_PUSH_OBJ_VAR_3,

		InstrType::PUSH_I32_ARR,
		InstrType::PUSH_I64_ARR,
		InstrType::PUSH_F32_ARR,
		InstrType::PUSH_F64_ARR,
		InstrType::PUSH_OBJ_ARR,

		InstrType::PUSH_BI8_ARR,
		InstrType::PUSH_CHR_ARR,
		InstrType::PUSH_I16_ARR,

		InstrType::SAVE_I32_VAR_U16,
		InstrType::SAVE_I64_VAR_U16,
		InstrType::SAVE_F32_VAR_U16,
		InstrType::SAVE_F64_VAR_U16,
		InstrType::SAVE_OBJ_VAR_U16,

		InstrType::I_SAVE_I32_VAR_0,
		InstrType::I_SAVE_I32_VAR_1,
		InstrType::I_SAVE_I32_VAR_2,
		InstrType::I_SAVE_I32_VAR_3,
		InstrType::I_SAVE_I64_VAR_0,
		InstrType::I_SAVE_I64_VAR_1,
		InstrType::I_SAVE_I64_VAR_2,
		InstrType::I_SAVE_I64_VAR_3,

		InstrType::I_SAVE_F32_VAR_0,
		InstrType::I_SAVE_F32_VAR_1,
		InstrType::I_SAVE_F32_VAR_2,
		InstrType::I_SAVE_F32_VAR_3,
		InstrType::I_SAVE_F64_VAR_0,
		InstrType::I_SAVE_F64_VAR_1,
		InstrType::I_SAVE_F64_VAR_2,
		InstrType::I_SAVE_F64_VAR_3,

		InstrType::I_SAVE_OBJ_VAR_0,
		InstrType::I_SAVE_OBJ_VAR_1,
		InstrType::I_SAVE_OBJ_VAR_2,
		InstrType::I_SAVE_OBJ_VAR_3,

		InstrType::SAVE_I32_ARR,
		InstrType::SAVE_I64_ARR,
		InstrType::SAVE_F32_ARR,
		InstrType::SAVE_F64_ARR,
		InstrType::SAVE_OBJ_ARR,

		InstrType::SAVE_BI8_ARR,
		InstrType::SAVE_CHR_ARR,
		InstrType::SAVE_I16_ARR,

		InstrType::POP_1,
		InstrType::POP_2,

		InstrType::DUP_1,
		InstrType::DUP_1_X,
		InstrType::DUP_1_X2,
		InstrType::DUP_2,
		InstrType::DUP_2_X,
		InstrType::DUP_2_X2,

		InstrType::SWAP,

		InstrType::ADD_I32,
		InstrType::ADD_I64,
		InstrType::ADD_F32,
		InstrType::ADD_F64,
		InstrType::SUB_I32,
		InstrType::SUB_I64,
		InstrType::SUB_F32,
		InstrType::SUB_F64,

		InstrType::MUL_I32,
		InstrType::MUL_I64,
		InstrType::MUL_F32,
		InstrType::MUL_F64,
		InstrType::DIV_I32,
		InstrType::DIV_I64,
		InstrType::DIV_F32,
		InstrType::DIV_F64,
		InstrType::REM_I32,
		InstrType::REM_I64,
		InstrType::REM_F32,
		InstrType::REM_F64,

		InstrType::NEG_I32,
		InstrType::NEG_I64,
		InstrType::NEG_F32,
		InstrType::NEG_F64,

		InstrType::SHL_I32,
		InstrType::SHL_I64,
		InstrType::SRC_I32,
		InstrType::SRC_I64,
		InstrType::SHR_I32,
		InstrType::SHR_I64,

		InstrType::AND_I32,
		InstrType::AND_I64,
		InstrType::OR_I32,
		InstrType::OR_I64,
		InstrType::XOR_I32,
		InstrType::XOR_I64,

		InstrType::ADD_I32_VAR_U16_CI16,

		InstrType::CAST_I32_I64,
		InstrType::CAST_I32_F32,
		InstrType::CAST_I32_F64,
		InstrType::CAST_I64_I32,
		InstrType::CAST_I64_F32,
		InstrType::CAST_I64_F64,

		InstrType::CAST_F32_I32,
		InstrType::CAST_F32_I64,
		InstrType::CAST_F32_F64,
		InstrType::CAST_F64_I32,
		InstrType::CAST_F64_I64,
		InstrType::CAST_F64_F32,

		InstrType::CAST_I32_I8,
		InstrType::CAST_I32_CHR,
		InstrType::CAST_I32_I16,

		InstrType::CMP_I64,
		InstrType::CMP_F32_M,
		InstrType::CMP_F32_P,
		InstrType::CMP_F64_M,
		InstrType::CMP_F64_P,

		//Note: if's may build a jump pad, to achive >16 bit jumps
		InstrType::IF_EQL,
		InstrType::IF_NEQ,
		InstrType::IF_LT,
		InstrType::IF_GTE,
		InstrType::IF_GT,
		InstrType::IF_LTE,
		InstrType::IF_I32_EQL,
		InstrType::IF_I32_NEQ,
		InstrType::IF_I32_LT,
		InstrType::IF_I32_GTE,
		InstrType::IF_I32_GT,
		InstrType::IF_I32_LTE,

		InstrType::IF_OBJ_EQL,
		InstrType::IF_OBJ_NEQ,

		InstrType::I_GOTO16,
		InstrType::I_DEPR_JSR16,
		InstrType::I_DEPR_GOTO_VAR_U16,

		InstrType::TABLE_SWITCH,
		InstrType::LOOKUP_SWITCH,

		InstrType::RET_I32,
		InstrType::RET_I64,
		InstrType::RET_F32,
		InstrType::RET_F64,
		InstrType::RET_OBJ,
		InstrType::RET,

		InstrType::PUSH_GET_STATIC,
		InstrType::SAVE_STATIC,
		InstrType::PUSH_GET_FIELD,
		InstrType::SAVE_FIELD,

		InstrType::PUSH_RUN_VIRTUAL,
		InstrType::PUSH_RUN_SPECIAL,
		InstrType::PUSH_RUN_STATIC,
		InstrType::PUSH_RUN_INTERFACE,
		InstrType::PUSH_RUN_DYN, //Has 2 zero bytes

		InstrType::PUSH_OBJ,
		InstrType::PUSH_ARR,
		InstrType::PUSH_OBJARR_1,

		InstrType::PUSH_ARRLEN,

		InstrType::THROW,

		InstrType::CHECK_CAST,
		InstrType::IS_OF,

		InstrType::SYNC_ON,
		InstrType::SYNC_OFF,

		InstrType::I_WIDE,

		InstrType::PUSH_OBJARR_U8,

		InstrType::IF_NIL,
		InstrType::IF_NNIL,

		InstrType::I_GOTO32,
		InstrType::I_DEPR_JSR32,

		// Utility

		InstrType::PUSH_CONST,//ldc, ldc_w(for constants > ff), ldc2_w (for long / double)

		InstrType::PUSH_I32_I32, //Will be converted
		InstrType::PUSH_I64_I64, //Will be converted
		InstrType::PUSH_F32_F32, //Will be converted
		InstrType::PUSH_F64_F64, //Will be converted

		InstrType::GOTO //Either goto, or goto_w
	>;

	//TODO: switch to something better than variant, as it is not optimal (no packing, uses short tag)

	template<class T>
	constexpr InstrId INSTR_OP_CODE = (InstrId)aca::variant_index_v<std::remove_cvref_t<T>, Instr>;

	static_assert( 
		INSTR_OP_CODE<InstrType::I_DEPR_JSR32> == InstrId::I_DEPR_JSR32
	);
	static_assert( 
		INSTR_OP_CODE<InstrType::ADD_I32> == InstrId::ADD_I32
	);

	constexpr InstrId invertIfInstr(const InstrId id) {
#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wswitch"
#endif
		switch (id)
		{
		case InstrId::IF_EQL:        return InstrId::IF_NEQ;
		case InstrId::IF_NEQ:        return InstrId::IF_EQL;
		case InstrId::IF_LT:         return InstrId::IF_GTE;
		case InstrId::IF_GTE:        return InstrId::IF_LT;
		case InstrId::IF_GT:         return InstrId::IF_LTE;
		case InstrId::IF_LTE:        return InstrId::IF_GT;

		case InstrId::IF_I32_EQL:    return InstrId::IF_I32_NEQ;
		case InstrId::IF_I32_NEQ:    return InstrId::IF_I32_EQL;
		case InstrId::IF_I32_LT:     return InstrId::IF_I32_GTE;
		case InstrId::IF_I32_GTE:    return InstrId::IF_I32_LT;
		case InstrId::IF_I32_GT:     return InstrId::IF_I32_LTE;
		case InstrId::IF_I32_LTE:    return InstrId::IF_I32_GT;

		case InstrId::IF_OBJ_EQL:    return InstrId::IF_OBJ_NEQ;
		case InstrId::IF_OBJ_NEQ:    return InstrId::IF_OBJ_EQL;

		case InstrId::IF_NIL:        return InstrId::IF_NNIL;
		case InstrId::IF_NNIL:       return InstrId::IF_NIL;
		}
#ifdef __clang__
#pragma clang diagnostic pop
#endif
		_ASSERT(false && "Invalid instruction, expected IF_*");
		std::abort();//Error lol
	}

}
