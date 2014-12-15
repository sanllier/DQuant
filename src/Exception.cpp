#include "Exception.h"

//------------------------------------------------------------

namespace DQ{
//------------------------------------------------------------

void Exception::print( std::ostream& oStr ) const
{
	oStr << "ERROR WAS OCCURED: " << m_what << " " << m_where << ".\r\n";
}

std::ostream& operator<<( std::ostream& oStr, const Exception& exc )
{
	exc.print( oStr );
	return oStr;
}

//------------------------------------------------------------

MPIException::MPIException( const std::string& excText, const std::string& excLoc )
{
    m_what = excText;
    m_where = excLoc;
}

//------------------------------------------------------------

QRegisterOutOfBounds::QRegisterOutOfBounds( const std::string& excLoc, size_t min, size_t max, size_t cur )
{
	m_what = "Trying to access " + std::to_string(( unsigned long long )(cur)) +  \
		     " element, but [" + std::to_string(( unsigned long long )(min)) + \
             ", " + std::to_string(( unsigned long long )(max)) + \
		     "] is available.";
	m_where = excLoc;
}

//------------------------------------------------------------

QDensityMatOutOfBounds::QDensityMatOutOfBounds( const std::string& excLoc, size_t min, size_t max, size_t cur )
{
	m_what = "Trying to access " + std::to_string(( unsigned long long )(cur)) +  \
		     " element, but [" + std::to_string(( unsigned long long )(min)) + \
             ", " + std::to_string(( unsigned long long )(max)) + \
		     "] is available.";
	m_where = excLoc;
}

//------------------------------------------------------------

BitIndexOverflow::BitIndexOverflow(size_t max, size_t cur)
{
	m_what = "(BitIndex class) Bit position overflow. Accessing to " + \
	 		  std::to_string((unsigned long long)(cur)) + " of " +     \
	 		  std::to_string((unsigned long long)(max)) + ".";
}

//------------------------------------------------------------

OperatorException::OperatorException( const std::string& excText, const std::string& excLoc )
{
	m_what = excText;
	m_where = excLoc;
}

//------------------------------------------------------------
}
