#ifndef H_EXCEPTION
#define H_EXCEPTION

#include <string>
#include <iostream>

//--------------------------------------------------------------------

namespace DQ {
//--------------------------------------------------------------------

class Exception
{
public:
	Exception():m_what( "Unknown error." ), m_where( "Unknown place." ) {}
    Exception( const std::string& what, const std::string& where):m_what( what ), m_where( where ) {}

    inline void setExcText( const std::string& excText ) { m_what = excText; }
    inline std::string getExcText( void ) const { return m_what; };
    inline void setExcLoc( const std::string& excLoc ) { m_where = excLoc; };
    inline std::string getExcLoc( void ) const { return m_where; };

	virtual void print( std::ostream& oStr ) const;

    friend std::ostream& operator<<( std::ostream& oStr, const Exception& exc );

protected:
	std::string m_what;
	std::string m_where;
};

//--------------------------------------------------------------------

class MPIException: public Exception
{
public:
    MPIException( const std::string& excText, const std::string& excLoc );
};

class QRegisterOutOfBounds: public Exception
{
public:
	QRegisterOutOfBounds( const std::string& excLoc, size_t min, size_t max, size_t cur );
};

class BitIndexOverflow: public Exception
{
public:
	BitIndexOverflow( size_t max, size_t cur );
};

class OperatorException: public Exception
{
public:
	OperatorException( const std::string& excText, const std::string& excLoc );
};

//--------------------------------------------------------------------
}

#endif
