/*
** See Copyright Notice inside Include.hpp
*/
#pragma once

#include <vector>
#include <variant>
#include <memory>

#include "Instrs.hpp"

namespace cpp_jcfu
{
	template<typename... Types>
	class InstrVariant
	{
	public:
		inline static constexpr uint8_t maxSize = std::max({ sizeof(Types)... });
	private:


		_JcfuNoUniqAddr alignas(Types...) std::byte storage[maxSize];

		_JcfuNoUniqAddr uint8_t typeIndex = 0;

		template<typename T>
		void destroyAs() {
			reinterpret_cast<T*>(&storage)->~T();
		}

		template<typename T>
		static constexpr std::pair<uint8_t,bool> _type_index() {
			uint8_t index = 0;
			bool found = ((std::is_same<T, Types>::value ? (++index, true) : (++index, false)) || ...);
			return { index - 1 ,found};
		}
		template<typename T>
		static constexpr uint8_t type_index() {
			constexpr std::pair<uint8_t, bool> res = _type_index<T>();
			static_assert(res.second, "Type not found in InstrVariant");
			return res.first;
		}

	public:
		InstrVariant() = default;

		template<class T>
		InstrVariant(T&& item) {
			new (&storage) T(std::forward<T>(item));
			typeIndex = idxOf<T>;
		}

		~InstrVariant() {
			reset();
		}

		InstrVariant(const InstrVariant&) = delete;
		InstrVariant& operator=(const InstrVariant&) = delete;

		InstrVariant(InstrVariant&& other) noexcept {
			moveFrom(std::move(other));
		}

		InstrVariant& operator=(InstrVariant&& other) noexcept {
			if (this != &other)
			{
				reset();
				moveFrom(std::move(other));
			}
			return *this;
		}

		template<class T>
		inline static constexpr uint8_t idxOf = type_index<T>();

		template<typename T, typename... Args>
		void emplace(Args&&... args) {
			reset();
			new (&storage) T(std::forward<Args>(args)...);
			typeIndex = idxOf<T>;
		}

		void reset() {

#define _TRY_DESTROY_TYPE(_T) case idxOf<_T>: destroyAs<_T>();break

			switch (typeIndex)
			{
				_TRY_DESTROY_TYPE(InstrType::TABLE_SWITCH);
				_TRY_DESTROY_TYPE(InstrType::LOOKUP_SWITCH);

				_TRY_DESTROY_TYPE(InstrType::PUSH_GET_STATIC);
				_TRY_DESTROY_TYPE(InstrType::PUSH_GET_FIELD);
				_TRY_DESTROY_TYPE(InstrType::SAVE_STATIC);
				_TRY_DESTROY_TYPE(InstrType::SAVE_FIELD);

				_TRY_DESTROY_TYPE(InstrType::PUSH_RUN_VIRTUAL);
				_TRY_DESTROY_TYPE(InstrType::PUSH_RUN_SPECIAL);
				_TRY_DESTROY_TYPE(InstrType::PUSH_RUN_STATIC);
				_TRY_DESTROY_TYPE(InstrType::PUSH_RUN_INTERFACE);
				_TRY_DESTROY_TYPE(InstrType::PUSH_RUN_DYN);

				_TRY_DESTROY_TYPE(InstrType::PUSH_OBJ);
				_TRY_DESTROY_TYPE(InstrType::PUSH_OBJARR_1);
				_TRY_DESTROY_TYPE(InstrType::PUSH_OBJARR_U8);

				_TRY_DESTROY_TYPE(InstrType::CHECK_CAST);
				_TRY_DESTROY_TYPE(InstrType::IS_OF);

				_TRY_DESTROY_TYPE(InstrType::PUSH_CONST);
			default:
				break;
			}
#undef _TRY_DESTROY_TYPE

			typeIndex = static_cast<uint16_t>(0);
		}

		template<typename T>
		T& get() {
			if (typeIndex == idxOf<T>)
				return reinterpret_cast<T*>(&storage);
			throw std::bad_variant_access();
		}

		template<typename T>
		const T& get() const {
			if (typeIndex == idxOf<T>)
				return reinterpret_cast<const T*>(&storage);
			throw std::bad_variant_access();
		}

		uint8_t index() const {
			return typeIndex;
		}

		template<typename Visitor>
		void visit(Visitor&& visitor) const {
			visitImpl(std::forward<Visitor>(visitor), std::index_sequence_for<Types...>{});
		}

		constexpr bool valueless_by_exception() const noexcept {
			return typeIndex == 0xFF;
		}

		friend struct std::variant_size<InstrVariant>;
		friend struct std::variant_alternative<0, InstrVariant>;

	private:
		template<typename Visitor, size_t... Is>
		void visitImpl(Visitor&& visitor, std::index_sequence<Is...>) const {

			using FuncType = void(*)(const void*, Visitor&);
			static constexpr FuncType visitors[] = {
				[](const void* ptr, Visitor& vis) { vis(*reinterpret_cast<const Types*>(ptr)); }...
			};
			visitors[typeIndex](&storage, visitor);
		}

		void moveFrom(InstrVariant&& other) {

			//No need to reset, as the code that uses this func does that.
			typeIndex = other.typeIndex;

#define _TRY_MOVE_TYPE(_T) case idxOf<_T>: \
	new (&storage) _T(std::move(*reinterpret_cast<_T*>(&other.storage))); \
	break

			switch (typeIndex)
			{
				_TRY_MOVE_TYPE(InstrType::TABLE_SWITCH);
				_TRY_MOVE_TYPE(InstrType::LOOKUP_SWITCH);

				_TRY_MOVE_TYPE(InstrType::PUSH_GET_STATIC);
				_TRY_MOVE_TYPE(InstrType::PUSH_GET_FIELD);
				_TRY_MOVE_TYPE(InstrType::SAVE_STATIC);
				_TRY_MOVE_TYPE(InstrType::SAVE_FIELD);

				_TRY_MOVE_TYPE(InstrType::PUSH_RUN_VIRTUAL);
				_TRY_MOVE_TYPE(InstrType::PUSH_RUN_SPECIAL);
				_TRY_MOVE_TYPE(InstrType::PUSH_RUN_STATIC);
				_TRY_MOVE_TYPE(InstrType::PUSH_RUN_INTERFACE);
				_TRY_MOVE_TYPE(InstrType::PUSH_RUN_DYN);

				_TRY_MOVE_TYPE(InstrType::PUSH_OBJ);
				_TRY_MOVE_TYPE(InstrType::PUSH_OBJARR_1);
				_TRY_MOVE_TYPE(InstrType::PUSH_OBJARR_U8);

				_TRY_MOVE_TYPE(InstrType::CHECK_CAST);
				_TRY_MOVE_TYPE(InstrType::IS_OF);

				_TRY_MOVE_TYPE(InstrType::PUSH_CONST);
			default:
				std::copy_n(other.storage, sizeof(storage), storage);
				break;
			}
#undef _TRY_MOVE_TYPE

			other.reset(); // Leave source in valid state
		}
	};


	using Instr = InstrVariant<
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

	template<class T>
	constexpr InstrId INSTR_OP_CODE = (InstrId)Instr::idxOf<std::remove_cvref_t<T>>;

	static_assert(
		INSTR_OP_CODE<InstrType::I_DEPR_JSR32> == InstrId::I_DEPR_JSR32
	);
	static_assert(
		INSTR_OP_CODE<InstrType::ADD_I32> == InstrId::ADD_I32
	);
	static_assert(
		sizeof(Instr)<=16 && 
		Instr::maxSize <= 15 //Allow for smaller Instructions
	);
}