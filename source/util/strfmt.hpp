#ifndef H_SDTP_UTIL_STR_FMT_H
#define H_SDTP_UTIL_STR_FMT_H

#include <memory>
#include <iostream>
#include <string>
#include <cstdio>

namespace sdtp::util::str{
	//format function for C++ std::strings (for use before c++20)
	template<typename ... Args>
	inline std::string format( const std::string& format, Args ... args )
	{
		size_t size = snprintf( nullptr, 0, format.c_str(), args ... ) + 1; // Extra space for '\0'
		std::unique_ptr<char[]> buf( new char[ size ] ); 
		snprintf( buf.get(), size, format.c_str(), args ... );
		return std::string( buf.get(), buf.get() + size - 1 ); // We don't want the '\0' inside
	}

	inline std::string format( const std::string& format)
	{
		return std::string(format);
	}
};

#endif //H_SDTP_UTIL_STR_FMT_H
