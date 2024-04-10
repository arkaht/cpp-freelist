#pragma once
#include <raylib.h>

#include <string>

namespace utils
{
	inline const char* bytes_to_str( int bytes )
	{
		const int KILO = 1024;
		const int MEGA = 1024 * 1024;
		const int GIGA = 1024 * 1024 * 1024;

		std::string unit = "XiB";
		float value = (float)bytes;
		if ( bytes >= GIGA )
		{
			value /= GIGA;
			unit[0] = 'G';
		}
		else if ( bytes >= MEGA )
		{
			value /= MEGA;
			unit[0] = 'M';
		}
		else if ( bytes >= KILO )
		{
			value /= KILO;
			unit[0] = 'K';
		}
		else
		{
			unit = "B";
		}

		const char* out_str = TextFormat( "%.2f %s", value, unit.c_str() );
		return out_str;
	}
}