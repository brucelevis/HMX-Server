#ifndef __CONFIG_H_
#define __CONFIG_H_


template< typename T, std::size_t Len >
std::size_t LengthOf( const T(&) [Len] )
{
	return Len;
}

#endif