/*
** See Copyright Notice inside Include.hpp
*/
#pragma once

#include <vector>
#include <bit>

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
	inline void constPoolIdxPushW(std::vector<uint8_t>& out, size_t& poolSize, ConstPool& consts, ConstPoolItm&& itm)
	{
		const bool is2x = isPoolItemBig(itm);
		constPoolIdxW(out, poolSize, consts, std::move(itm));

		poolSize++;
		if (is2x)
			poolSize++;
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
	inline void codeTagW(std::vector<uint8_t>& out, size_t& poolSize, ConstPool& consts, const CodeTag& itm)
	{
		//TODO
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

			out.insert(tagOut.end(), 
				var.bytecode.begin(), 
				var.bytecode.end());

			_ASSERT(var.errorHandlers.size() < UINT16_MAX);
			u16w(tagOut, (uint16_t)var.errorHandlers.size());
			for (const ErrorHandler& eh : var.errorHandlers)
			{
				u16w(tagOut, eh.startPc);
				u16w(tagOut, eh.afterEndPc);
				u16w(tagOut, eh.handlerPc);
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
}
