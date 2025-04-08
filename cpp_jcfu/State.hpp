/*
** See Copyright Notice inside Include.hpp
*/
#pragma once

#include <vector>
#include <string>
#include <variant>

namespace cpp_jcfu
{
	enum class FuncHandleKind : uint8_t
	{
		GET_FIELD		= 1, //getfield	 C.f : T
		GET_STATIC		= 2, //getstatic C.f : T
		PUT_FIELD		= 3, //putfield	 C.f : T
		PUT_STATIC		= 4, //putstatic C.f : T

		RUN_DYN			= 5, //invokevirtual C.m : (A*)T
		RUN_STATIC		= 6, //invokestatic	 C.m : (A*)T
		RUN_SPECIAL		= 7, //invokespecial C.m : (A*)T

		NEW_RUN_SPECIAL	= 8, //new C; dup; invokespecial C.<init>:(A*)void
		RUN_INTERFACE	= 9, //invokeinterface C.m : (A*)T
	};

	//https://docs.oracle.com/javase/specs/jvms/se7/html/jvms-4.html#jvms-4.4
	//https://docs.oracle.com/javase/specs/jvms/se24/html/jvms-4.html#jvms-4.4
	enum class ConstPoolItmId : uint8_t
	{
		CLASS			= 7,

		FIELD_REF		= 9,
		FUNC_REF		= 10,
		INTERFACE_FUNC_REF = 11,

		STR				= 8,
		I32				= 3,
		F32				= 4,
		I64				= 5,
		F64				= 6,

		NAME_AND_DESC	= 12,
		JUTF8			= 1,
		FUNC_HANDLE		= 15,
		FUNC_TYPE		= 16,
		//TODO: does DYNAMIC exist in j7, even if its not in the spec?
		RUN_DYN			= 18,
	};


	using ConstPoolIdx = uint16_t;

	// Use normal utf8, it will be converted for you
	using Jutf8 = std::string;

	namespace ConstPoolItmType
	{
		using JUTF8 = Jutf8;
		struct CLASS
		{
			Jutf8 name;
		};

		struct STR
		{
			Jutf8 txt;
		};
		using I32 = int32_t;
		using F32 = float;
		using I64 = int64_t;
		using F64 = double;

		struct NAME_AND_DESC
		{
			Jutf8 name;
			Jutf8 desc;
		};
		struct FUNC_TYPE
		{
			Jutf8 desc;
		};
		struct RUN_DYN
		{
			NAME_AND_DESC funcDesc;
			uint16_t bootstrapIdx;
		};

		struct RefBase
		{
			CLASS classIdx;
			NAME_AND_DESC refDesc;
		};

		struct FIELD_REF : RefBase
		{};
		struct FUNC_REF : RefBase
		{};
		struct INTERFACE_FUNC_REF : RefBase
		{};

		/**
		 * Kinds GET/PUT_FIELD/STATIC  => val is FIELD_REF
		 *
		 * Kinds RUN_DYN/STATIC/SPECIAL, NEW_RUN_SPECIAL  => val is FUNC_REF
		 *
		 * Kind  RUN_INTERFACE  => val is INTERFACE_FUNC_REF
		 *
		 * Kinds RUN_DYN/STATIC/SPECIAL/INTERFACE
		 *	=> the name cant be <init> or <clinit>.
		 *
		 * Kind  NEW_RUN_SPECIAL  => name must be <init>.
		 */
		struct FUNC_HANDLE
		{
			FuncHandleKind kind;//encoded as u16
			RefBase val;
		};
	}
	using ConstPoolItm = std::variant<
		ConstPoolItmType::I32,
		ConstPoolItmType::F32,
		ConstPoolItmType::F64,
		ConstPoolItmType::I64,
		ConstPoolItmType::STR,

		ConstPoolItmType::CLASS,

		ConstPoolItmType::FIELD_REF,
		ConstPoolItmType::FUNC_REF,
		ConstPoolItmType::INTERFACE_FUNC_REF,

		ConstPoolItmType::NAME_AND_DESC,
		ConstPoolItmType::JUTF8,
		ConstPoolItmType::FUNC_HANDLE,
		ConstPoolItmType::FUNC_TYPE,
		ConstPoolItmType::RUN_DYN
	>;
	// You can only rely on the ones you added to it.
	// The writer might add some, but it will always be after your ones.
	using ConstPool = std::vector<ConstPoolItm>;

}
