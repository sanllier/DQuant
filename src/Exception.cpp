#include "Exception.h"

//--------------------------------------------------------------------

namespace DQuant{
//--------------------------------------------------------------------

void Exception::print( std::ostream& oStr ) const
{
	oStr << "ERROR WAS OCCURED: " << m_what << " " << m_where << "\r\n";
}

std::ostream& operator<<( std::ostream& oStr, const Exception& exc )
{
	exc.print( oStr );
	return oStr;
}

//------------------------------------------------------------

DislibInactive::DislibInactive( const std::string& excLoc )
{
	m_what = "Trying to use inactive dislib.";
	m_where = excLoc;
}

//------------------------------------------------------------

QRegisterOverflow::QRegisterOverflow( const std::string& excLoc, size_t max, size_t cur )
{
	m_what = "Trying to access " + std::to_string(( unsigned long long )(cur)) +  \
		     " element, but " + std::to_string((unsigned long long)(max - 1)) + \
		     " is maximum avail.";
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

//--------------------------------------------------------------------
}
