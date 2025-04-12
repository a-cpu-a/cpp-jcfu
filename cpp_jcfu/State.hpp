/*
** See Copyright Notice inside Include.hpp
*/
#pragma once

#include <vector>
#include <string>
#include <variant>
#include <optional>
#include <memory>

#include "ext/MorLib.hpp"

namespace cpp_jcfu
{
#ifdef _MSC_VER
#define _JcfuNoUniqAddr [[msvc::no_unique_address]]
#else
#define _JcfuNoUniqAddr [[no_unique_address]]
#endif // _MSC_VER


	template<class T>
	struct CppBox
	{
		std::unique_ptr<T> v;
		constexpr bool operator==(const CppBox& o) const {
			return *v == *o;
		}
		constexpr T& operator*()const  {
			return *v;
		}
		constexpr T* operator->() const {
			return v.get();
		}
	};
	template<class T>
	constexpr CppBox<T> newCppBox(T&& in) {
		return CppBox(std::make_unique<T>(in));
	}


	using AccessFlags = uint16_t;
	enum AccessFlags_ : uint16_t
	{
		// I - inner class
		// C - class
		// L - module
		// R - requires
		// O - opens & exports
		// F - field
		// M - func/method

		AccessFlags_NONE = 0,

		AccessFlags_PUBLIC						= 0x0001,//	IC, ___, FM
		AccessFlags_PRIVATE						= 0x0002,//	I_, ___, FM
		AccessFlags_PROTECTED					= 0x0004,//	I_, ___, FM
		AccessFlags_STATIC						= 0x0008,//	I_, ___, FM
		AccessFlags_FINAL						= 0x0010,//	IC, ___, FMP
		AccessFlags_SUPER_OPEN_TRANSITIVE_SYNC	= 0x0020,//	_C, LR_, _M
		AccessFlags_STATICPHASE_VOLATILE_BRIDGE = 0x0040,//	__, _R_, FM
		AccessFlags_TRANSIENT_VARARGS			= 0x0080,//	__, ___, FM
		AccessFlags_NATIVE						= 0x0100,//	__, ___, _M
		AccessFlags_INTERFACE					= 0x0200,//	IC
		AccessFlags_ABSTRACT					= 0x0400,//	IC, ___, _M
		AccessFlags_STRICTFP					= 0x0800,//	__, ___, _M
		AccessFlags_SYNTHETIC					= 0x1000,//	IC, LRO, FMP
		AccessFlags_ANNOTATION					= 0x2000,//	IC
		AccessFlags_ENUM						= 0x4000,//	IC, ___, F

		AccessFlags_MOD_MANDATED				= 0x8000,//	_C, LRO, __P
	};

	using ClassFlags = uint16_t;
	enum ClassFlags_ : uint16_t
	{
		ClassFlags_NONE = 0,

		ClassFlags_PUBLIC = AccessFlags_PUBLIC,
		ClassFlags_FINAL = AccessFlags_FINAL,

		ClassFlags_SUPER = AccessFlags_SUPER_OPEN_TRANSITIVE_SYNC,
		ClassFlags_INTERFACE = AccessFlags_INTERFACE,
		ClassFlags_ABSTRACT = AccessFlags_ABSTRACT,
		ClassFlags_SYNTHETIC = AccessFlags_SYNTHETIC,
		ClassFlags_ANNOTATION = AccessFlags_ANNOTATION,
		ClassFlags_ENUM = AccessFlags_ENUM,

		ClassFlags_MOD = AccessFlags_MOD_MANDATED,
	};

	using InnerClassFlags = uint16_t;
	enum InnerClassFlags_ : uint16_t
	{
		InnerClassFlags_NONE = 0,

		InnerClassFlags_PUBLIC = AccessFlags_PUBLIC,
		InnerClassFlags_PRIVATE = AccessFlags_PRIVATE,
		InnerClassFlags_PROTECTED = AccessFlags_PROTECTED,
		InnerClassFlags_STATIC = AccessFlags_STATIC,
		InnerClassFlags_FINAL = AccessFlags_FINAL,

		InnerClassFlags_INTERFACE = AccessFlags_INTERFACE,
		InnerClassFlags_ABSTRACT = AccessFlags_ABSTRACT,
		InnerClassFlags_SYNTHETIC = AccessFlags_SYNTHETIC,
		InnerClassFlags_ANNOTATION = AccessFlags_ANNOTATION,
		InnerClassFlags_ENUM = AccessFlags_ENUM,
	};

	using FieldFlags = uint16_t;
	enum FieldFlags_ : uint16_t
	{
		FieldFlags_NONE = 0,

		FieldFlags_PUBLIC = AccessFlags_PUBLIC,
		FieldFlags_PRIVATE = AccessFlags_PRIVATE,
		FieldFlags_PROTECTED = AccessFlags_PROTECTED,
		FieldFlags_STATIC = AccessFlags_STATIC,
		FieldFlags_FINAL = AccessFlags_FINAL,

		FieldFlags_VOLATILE = AccessFlags_STATICPHASE_VOLATILE_BRIDGE,
		FieldFlags_TRANSIENT = AccessFlags_TRANSIENT_VARARGS,

		FieldFlags_SYNTHETIC = AccessFlags_SYNTHETIC,
		FieldFlags_ENUM = AccessFlags_ENUM,
	};

	using FuncFlags = uint16_t;
	enum FuncFlags_ : uint16_t
	{
		FuncFlags_NONE = 0,

		FuncFlags_PUBLIC = AccessFlags_PUBLIC,
		FuncFlags_PRIVATE = AccessFlags_PRIVATE,
		FuncFlags_PROTECTED = AccessFlags_PROTECTED,
		FuncFlags_STATIC = AccessFlags_STATIC,
		FuncFlags_FINAL = AccessFlags_FINAL,

		FuncFlags_SYNC = AccessFlags_SUPER_OPEN_TRANSITIVE_SYNC,
		FuncFlags_BRIDGE = AccessFlags_STATICPHASE_VOLATILE_BRIDGE,
		FuncFlags_VARARGS = AccessFlags_TRANSIENT_VARARGS,
		FuncFlags_NATIVE = AccessFlags_NATIVE,
		FuncFlags_ABSTRACT = AccessFlags_ABSTRACT,
		FuncFlags_STRICTFP = AccessFlags_STRICTFP,

		FuncFlags_SYNTHETIC = AccessFlags_SYNTHETIC,
	};

	using ParamFlags = uint16_t;
	enum ParamFlags_ : uint16_t
	{
		ParamFlags_NONE = 0,

		ParamFlags_FINAL = AccessFlags_FINAL,

		ParamFlags_SYNTHETIC = AccessFlags_SYNTHETIC,
		ParamFlags_MANDATED = AccessFlags_MOD_MANDATED,
	};

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

	namespace ConstPoolItmType
	{
		using JUTF8 = std::string;
		struct CLASS
		{
			std::string name;
			Mor_cmp_op(CLASS);
		};

		struct STR
		{
			std::string txt;
		};
		using I32 = int32_t;
		using F32 = float;
		using I64 = int64_t;
		using F64 = double;

		struct NAME_AND_DESC
		{
			std::string name;
			std::string desc;
		};
		struct FUNC_TYPE
		{
			std::string desc;
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

	namespace CommonTagType
	{
		struct DEPRECATED {};
		struct SYNTHETIC {};
		struct SIGNATURE {};				//TODO
		struct SHOWN_ANNOTATIONS {};		//TODO
		struct ANNOTATIONS {};				//TODO
		struct SHOWN_TYPE_ANNOTATIONS {};	//TODO
		struct TYPE_ANNOTATIONS {};			//TODO
	}

	struct LineNumEntry
	{
		uint16_t startPc;
		uint16_t line;
	};
	struct LocalEntry
	{
		std::string name;
		std::string desc;
		uint16_t startPc;
		uint16_t len;
		uint16_t idx;
	};
	struct LocalTypeEntry
	{
		std::string name;
		std::string sig;
		uint16_t startPc;
		uint16_t len;
		uint16_t idx;
	};

	namespace SlotKindType
	{
		struct PAD { Mor_cmp_op(PAD); };
		struct I32 { Mor_cmp_op(I32); };
		struct F32 { Mor_cmp_op(F32); };
		struct I64 { Mor_cmp_op(I64); };
		struct F64 { Mor_cmp_op(F64); };
		struct NIL { Mor_cmp_op(NIL); };
		using  OBJ = CppBox<ConstPoolItmType::CLASS>;
		using  RAW_OBJ = uint16_t;//The instruction index of a new, that made this variable
	}
	using SlotKind = std::variant<
		SlotKindType::PAD,
		SlotKindType::I32,
		SlotKindType::F32,
		SlotKindType::I64,
		SlotKindType::F64,
		SlotKindType::NIL,
		SlotKindType::OBJ,
		SlotKindType::RAW_OBJ
	>;
	inline SlotKind newObjSlotKind(const std::string& klass)
	{
		return newCppBox(
			cpp_jcfu::ConstPoolItmType::CLASS{ klass }
		);
	}


	namespace CodeSlotKindType
	{
		using  PAD = SlotKindType::PAD;
		using  I32 = SlotKindType::I32;
		using  F32 = SlotKindType::F32;
		using  I64 = SlotKindType::I64;
		using  F64 = SlotKindType::F64;
		using  NIL = SlotKindType::NIL;
		struct OBJ { uint16_t constPoolIdx; };
		using  RAW_OBJ = uint16_t;//The instruction index of a new, that made this variable
	}
	using CodeSlotKind = std::variant<
		CodeSlotKindType::PAD,
		CodeSlotKindType::I32,
		CodeSlotKindType::F32,
		CodeSlotKindType::I64,
		CodeSlotKindType::F64,
		CodeSlotKindType::NIL,
		CodeSlotKindType::OBJ,
		CodeSlotKindType::RAW_OBJ
	>;

	namespace CodeStackFrameType
	{
		struct BaseDelta { uint16_t delta; };

		using SAME_NO_STACK = uint16_t;
		struct SAME_1_STACK : BaseDelta
		{
			CodeSlotKind stackKind;
		};

		struct CHOP1_NO_STACK : BaseDelta {};
		struct CHOP2_NO_STACK : BaseDelta {};
		struct CHOP3_NO_STACK : BaseDelta {};

		struct ADD1_NO_STACK : BaseDelta { CodeSlotKind localKinds[1]; };
		struct ADD2_NO_STACK : BaseDelta { CodeSlotKind localKinds[2]; };
		struct ADD3_NO_STACK : BaseDelta { CodeSlotKind localKinds[3]; };

		struct BaseFull : BaseDelta
		{
			std::vector<CodeSlotKind> localKinds;
			std::vector<CodeSlotKind> stackKinds;
		};
		using FULL = std::unique_ptr<CodeStackFrameType::BaseFull>;
	}

	using CodeStackFrame = std::variant<
		CodeStackFrameType::SAME_NO_STACK,
		CodeStackFrameType::SAME_1_STACK,

		CodeStackFrameType::CHOP1_NO_STACK,
		CodeStackFrameType::CHOP2_NO_STACK,
		CodeStackFrameType::CHOP3_NO_STACK,

		CodeStackFrameType::ADD1_NO_STACK,
		CodeStackFrameType::ADD2_NO_STACK,
		CodeStackFrameType::ADD3_NO_STACK,

		CodeStackFrameType::FULL
	>;

	namespace CodeTagType
	{
		using LINE_NUMS = std::vector<LineNumEntry>;
		using LOCALS = std::vector<LocalEntry>;
		using LOCAL_TYPES = std::vector<LocalTypeEntry>;
		using STACK_FRAMES = std::vector<CodeStackFrame>;
	}
	using CodeTag = std::variant<
		CodeTagType::LINE_NUMS,
		CodeTagType::LOCALS,
		CodeTagType::LOCAL_TYPES,
		CodeTagType::STACK_FRAMES
	>;
	using BasicCodeTag = std::variant<
		CodeTagType::LINE_NUMS,
		CodeTagType::LOCALS,
		CodeTagType::LOCAL_TYPES
	>;

	struct CodeTagErrorHandler
	{
		std::optional<ConstPoolItmType::CLASS> catchType; // None -> catch all
		uint16_t startByte;
		uint16_t afterEndByte;
		uint16_t handlerByte;
	};
	struct FuncParam
	{
		std::string name;
		ParamFlags flags;
	};
	namespace FuncTagType
	{
		struct CODE {
			std::vector<uint8_t> bytecode;
			std::vector<CodeTagErrorHandler> errorHandlers;
			std::vector<CodeTag> tags;
			uint16_t maxStack;
			uint16_t maxLocals;
		};
		struct EXCEPTIONS {};				//TODO
		struct SHOWN_PARAM_ANNOTATIONS {};	//TODO
		struct PARAM_ANNOTATIONS {};		//TODO
		struct ANNOTATION_DEFAULT {};		//TODO
		using PARAMS = std::vector<FuncParam>;

		using SYNTHETIC = CommonTagType::SYNTHETIC;
		using DEPRECATED = CommonTagType::DEPRECATED;
		using SIGNATURE = CommonTagType::SIGNATURE;
		using SHOWN_ANNOTATIONS = CommonTagType::SHOWN_ANNOTATIONS;
		using ANNOTATIONS = CommonTagType::ANNOTATIONS;
		using SHOWN_TYPE_ANNOTATIONS = CommonTagType::SHOWN_TYPE_ANNOTATIONS;
		using TYPE_ANNOTATIONS = CommonTagType::TYPE_ANNOTATIONS;
	}
	using FuncTag = std::variant<
		FuncTagType::CODE,
		FuncTagType::EXCEPTIONS,
		FuncTagType::SHOWN_PARAM_ANNOTATIONS,
		FuncTagType::PARAM_ANNOTATIONS,
		FuncTagType::ANNOTATION_DEFAULT,
		FuncTagType::PARAMS,

		FuncTagType::SYNTHETIC,
		FuncTagType::DEPRECATED,
		FuncTagType::SIGNATURE,
		FuncTagType::SHOWN_ANNOTATIONS,
		FuncTagType::ANNOTATIONS,
		FuncTagType::SHOWN_TYPE_ANNOTATIONS,
		FuncTagType::TYPE_ANNOTATIONS
	>;


	namespace FieldTagType
	{
		using CONST_VAL = std::vector<LineNumEntry>;

		using SYNTHETIC = CommonTagType::SYNTHETIC;
		using DEPRECATED = CommonTagType::DEPRECATED;
		using SIGNATURE = CommonTagType::SIGNATURE;
		using SHOWN_ANNOTATIONS = CommonTagType::SHOWN_ANNOTATIONS;
		using ANNOTATIONS = CommonTagType::ANNOTATIONS;
		using SHOWN_TYPE_ANNOTATIONS = CommonTagType::SHOWN_TYPE_ANNOTATIONS;
		using TYPE_ANNOTATIONS = CommonTagType::TYPE_ANNOTATIONS;
	}
	using FieldTag = std::variant<
		FieldTagType::CONST_VAL,

		FieldTagType::SYNTHETIC,
		FieldTagType::DEPRECATED,
		FieldTagType::SIGNATURE,
		FieldTagType::SHOWN_ANNOTATIONS,
		FieldTagType::ANNOTATIONS,
		FieldTagType::SHOWN_TYPE_ANNOTATIONS,
		FieldTagType::TYPE_ANNOTATIONS
	>;

	struct FuncInfo
	{
		std::vector<FuncTag> tags;
		std::string name;
		std::string desc;
		FuncFlags flags;
	};
	using Functions = std::vector<FuncInfo>;

	struct FieldInfo
	{
		std::vector<FieldTag> tags;
		std::string name;
		std::string desc;
		FieldFlags flags;
	};
	using Fields = std::vector<FieldInfo>;
}
