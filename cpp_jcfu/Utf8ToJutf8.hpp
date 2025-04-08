/*
** See Copyright Notice inside Include.hpp
*/
#pragma once

#include <string>

namespace cpp_jcfu
{

	//https://en.wikipedia.org/wiki/CESU-8
	//https://en.wikipedia.org/wiki/UTF-8
	//https://en.wikipedia.org/wiki/UTF-16
	//https://stackoverflow.com/questions/34151138/convert-cesu-8-to-utf-8-with-high-performance
	// Unsafe, wont work on chars that are >4 bytes long
	inline std::string utf8ToJutf8(const std::string& in)
	{
		//Codes to replace are: (0xF_), but only 0xF0 is supported by surogate pairs

		std::string ret;
		ret.reserve(in.size() + 4);

		uint32_t decodedNum = 0;
		uint8_t parseIdx = 0;

		for (size_t i = 0; i < in.size(); i++)
		{
			const char ch = in[i];

			if (parseIdx != 0)
			{
				//currently recoding a 4 byte character
				parseIdx++;
				const uint8_t chBits = (ch & 0b111111);
				switch (parseIdx)
				{
				case 2:
					decodedNum |= chBits << 12;//byte 2
					break;
				case 3:
					decodedNum |= chBits << 6;//byte 3
					break;
				case 4:
					decodedNum |= chBits;//byte 4
					decodedNum -= 0x10000;//how is it off...
					parseIdx = 0;
					ret.push_back(0b10100000 | uint8_t(decodedNum >> 16));//4bits
					ret.push_back(0b10000000 | uint8_t((decodedNum >> 10) & 0b111111));//6bits

					ret.push_back((uint8_t)0xED);
					ret.push_back(0b10110000 | uint8_t((decodedNum >> 6) & 0b1111));//4bits
					ret.push_back(0b10000000 | uint8_t(decodedNum & 0b111111));//6bits
					break;
				}
				continue;
			}
			else if ((ch & 0xF0) == 0xF0)
			{
				//Found the start of a 4 byte unicode character
				decodedNum = (ch & 0b11) << 18; //NOTE: 1 extra bit is ignored, to reduce errors in high surrogate encoding 
				ret.push_back((uint8_t)0xED);
				parseIdx = 1;
				continue;
			}

			if (ch == 0)
			{
				ret.push_back(0b11000000);
				ret.push_back(0b10000000);
				continue;
			}

			//just a normal <4 byte unicode character
			ret.push_back(ch);
		}

		return ret;
	}
}