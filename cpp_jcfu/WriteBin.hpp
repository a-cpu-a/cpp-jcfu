/*
** See Copyright Notice inside Include.hpp
*/
#pragma once

#include <vector>
#include <bit>

#include "ext/ExtendVariant.hpp"
#include "State.hpp"
#include "Utf8ToJutf8.hpp"
#include "StateUtils.hpp"

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
	inline void u32w(std::vector<uint8_t>& out, const uint32_t v)
	{
		out.push_back((v >> 24) & 0xFF);
		out.push_back((v >> 16) & 0xFF);
		out.push_back((v >> 8) & 0xFF);
		out.push_back(v & 0xFF);
	}
	inline void u16w(std::vector<uint8_t>& out, const uint16_t v)
	{
		out.push_back((v >> 8) & 0xFF);
		out.push_back(v & 0xFF);
	}
	inline void u32Patch(std::vector<uint8_t>& out,const size_t offset, const uint32_t v)
	{
		out[offset] = (v >> 24) & 0xFF;
		out[offset+1] = (v >> 16) & 0xFF;
		out[offset+2] = (v >> 8) & 0xFF;
		out[offset+3] = v & 0xFF;
	}
	inline void u16Patch(std::vector<uint8_t>& out, const size_t offset, const uint16_t v)
	{
		out[offset] = (v >> 8) & 0xFF;
		out[offset + 1] = v & 0xFF;
	}
	//https://docs.oracle.com/javase/specs/jvms/se7/html/jvms-4.html#jvms-4.4.7
	inline void jUtf8W(std::vector<uint8_t>& out, std::string v)
	{
		v = utf8ToJutf8(v);
		_ASSERT(v.size() < UINT16_MAX);
		u16w(out, (uint16_t)v.size());
		out.insert(out.end(), v.begin(), v.end());
	}

	inline void constPoolIdxW(std::vector<uint8_t>& out, size_t& poolSize, ConstPool& consts, ConstPoolItm&& itm)
	{
		_ASSERT(poolSize < UINT16_MAX);
		u16w(out, (uint16_t)poolSize);
		consts.emplace_back(itm);
	}
	inline void constPoolIdxPushW(std::vector<uint8_t>& out, size_t& poolSize, ConstPool& consts, ConstPoolItm&& itm) {
		u16w(out, constPoolPush(poolSize, consts, std::move(itm)));
	}
	inline void pushJutf8IdxW(std::vector<uint8_t>& out, size_t& poolSize, ConstPool& consts, const std::string& str)
	{
		constPoolIdxPushW(out, poolSize, consts,
			ConstPoolItmType::JUTF8(str));
	}
	inline void pushClassIdxW(std::vector<uint8_t>& out, size_t& poolSize, ConstPool& consts, const std::string& className)
	{
		constPoolIdxPushW(out, poolSize, consts,
			ConstPoolItmType::CLASS(className));
	}

	inline void codeSlotKindW(std::vector<uint8_t>& out, size_t& poolSize, ConstPool& consts, const CodeSlotKind& itm)
	{
		ezmatch(itm)(
		varcase(const auto){
			out.push_back(aca::variant_index_v<decltype(var),CodeSlotKind>);

		},
		varcase(const CodeSlotKindType::OBJ){
			out.push_back(7);
			u16w(out, var.constPoolIdx);
		},
		varcase(const CodeSlotKindType::RAW_OBJ){
			out.push_back(8);
			u16w(out, var);
		}
		);
	}
	inline void codeStackFrameW(std::vector<uint8_t>& out, size_t& poolSize, ConstPool& consts, const CodeStackFrame& itm)
	{
		ezmatch(itm)(
		varcase(const CodeStackFrameType::SAME_NO_STACK){
			if (var <= 63)
				out.push_back(var);
			else
			{
				out.push_back(251);
				u16w(out, var);
			}
		},
		varcase(const CodeStackFrameType::SAME_1_STACK){
			if (var.delta <= 63)
				out.push_back(var.delta+64);
			else
			{
				out.push_back(247);
				u16w(out, var.delta);
			}
			codeSlotKindW(out, poolSize, consts, var.stackKind);
		},
		varcase(const CodeStackFrameType::AnyCodeChopStackFrame auto){
			out.push_back(var.binId);
			u16w(out, var.delta);
		},
		varcase(const CodeStackFrameType::AnyCodeAddStackFrame auto) {
			out.push_back(251+ var.localKinds.size());
			u16w(out, var.delta);
			for (const CodeSlotKind k : var.localKinds)
				codeSlotKindW(out, poolSize, consts, k);
		},
		varcase(const CodeStackFrameType::FULL&){
			out.push_back(255);
			u16w(out, var->delta);

			u16w(out, var->localKinds.size());
			for (const CodeSlotKind k : var->localKinds)
				codeSlotKindW(out, poolSize, consts, k);
			u16w(out, var->stackKinds.size());
			for (const CodeSlotKind k : var->stackKinds)
				codeSlotKindW(out, poolSize, consts, k);
		}
		);
	}
	inline void codeTagW(std::vector<uint8_t>& out, size_t& poolSize, ConstPool& consts, const CodeTag& itm)
	{
		ezmatch(itm)(
		varcase(const CodeTagType::LINE_NUMS&){
			//TODO
		},
		varcase(const CodeTagType::LOCALS&){
			//TODO
		},
		varcase(const CodeTagType::LOCAL_TYPES&){
			//TODO
		},
		// https://docs.oracle.com/javase/specs/jvms/se24/html/jvms-4.html#jvms-4.7.4
		varcase(const CodeTagType::STACK_FRAMES&){
			pushJutf8IdxW(out, poolSize, consts, "StackMapTable");
			std::vector<uint8_t> tagOut;

			_ASSERT(var.size() < UINT16_MAX);
			u16w(tagOut, (uint16_t)var.size());
			for (const CodeStackFrame& frame : var)
				codeStackFrameW(tagOut, poolSize, consts, frame);

			_ASSERT(tagOut.size() < UINT32_MAX);
			u32w(out, (uint32_t)tagOut.size());
			out.insert(out.end(), tagOut.begin(), tagOut.end());
		}
		);
	}
	inline void funcTagW(std::vector<uint8_t>& out, size_t& poolSize, ConstPool& consts, const FuncTag& itm)
	{
		ezmatch(itm)(
		varcase(const FuncTagType::CODE&){
			pushJutf8IdxW(out, poolSize, consts, "Code");
			std::vector<uint8_t> tagOut;

			u16w(tagOut, var.maxStack);
			u16w(tagOut, var.maxLocals);

			_ASSERT(!var.bytecode.empty() && var.bytecode.size() < UINT16_MAX);
			u32w(tagOut, (uint32_t)var.bytecode.size());

			tagOut.insert(tagOut.end(),
				var.bytecode.begin(), 
				var.bytecode.end());

			_ASSERT(var.errorHandlers.size() < UINT16_MAX);
			u16w(tagOut, (uint16_t)var.errorHandlers.size());
			for (const CodeTagErrorHandler& eh : var.errorHandlers)
			{
				u16w(tagOut, eh.startByte);
				u16w(tagOut, eh.afterEndByte);
				u16w(tagOut, eh.handlerByte);
				if (eh.catchType.has_value())
				{
					pushClassIdxW(tagOut, poolSize, consts,
						eh.catchType->name);
				}
				else
					u16w(tagOut, 0);
			}
			_ASSERT(var.tags.size() < UINT16_MAX);
			u16w(tagOut, (uint16_t)var.tags.size());
			for (const CodeTag& ct : var.tags)
				codeTagW(out, poolSize, consts, ct);

			_ASSERT(tagOut.size() < UINT32_MAX);
			u32w(out, (uint32_t)tagOut.size());
			out.insert(out.end(), tagOut.begin(), tagOut.end());

		},
			//TODO
		varcase(const FuncTagType::EXCEPTIONS&){
		},
		varcase(const FuncTagType::SHOWN_PARAM_ANNOTATIONS&){
		},
		varcase(const FuncTagType::PARAM_ANNOTATIONS&){
		},
		varcase(const FuncTagType::ANNOTATION_DEFAULT&){
		},
		varcase(const FuncTagType::PARAMS&){
		},

			//TODO
		varcase(const FuncTagType::SYNTHETIC&){
		},
		varcase(const FuncTagType::DEPRECATED&){
		},
		varcase(const FuncTagType::SIGNATURE&){
		},
		varcase(const FuncTagType::SHOWN_ANNOTATIONS&){
		},
		varcase(const FuncTagType::ANNOTATIONS&){
		},
		varcase(const FuncTagType::SHOWN_TYPE_ANNOTATIONS&){
		},
		varcase(const FuncTagType::TYPE_ANNOTATIONS&){
		}
		);
	}
	inline void fieldTagW(std::vector<uint8_t>& out, size_t& poolSize, ConstPool& consts, const FieldTag& itm)
	{
		ezmatch(itm)(
			//TODO
		varcase(const FieldTagType::CONST_VAL&){
		},

			//TODO
		varcase(const FieldTagType::SYNTHETIC&){
		},
		varcase(const FieldTagType::DEPRECATED&){
		},
		varcase(const FieldTagType::SIGNATURE&){
		},
		varcase(const FieldTagType::SHOWN_ANNOTATIONS&){
		},
		varcase(const FieldTagType::ANNOTATIONS&){
		},
		varcase(const FieldTagType::SHOWN_TYPE_ANNOTATIONS&){
		},
		varcase(const FieldTagType::TYPE_ANNOTATIONS&){
		}
		);
	}
}
