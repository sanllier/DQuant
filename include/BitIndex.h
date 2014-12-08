#ifndef H_BITINDEX
#define H_BITINDEX

#define BYTESIZE 8

//--------------------------------------------------------------------

namespace DQ {
//--------------------------------------------------------------------

template<typename T>
class BitIndex
{
private:
	inline void ctrlBitAtPos( int pos, bool val )
    {
	    #ifndef PERFORMANCE
	    if ( pos >= m_typeTSize )
		    throw BitIndexOverflow( n_typeTSize - 1, pos );
	    #endif

	    if ( val )
		    m_idx |= T(1) << pos;
	    else
		    m_idx &= ~( T(1) << pos );
    }

public:
    BitIndex( const T idx = 0 ):m_idx( idx ), m_typeTSize( sizeof(T) * BYTESIZE ) {}
    ~BitIndex() {}

    inline void setIdx( const T idx ) { m_idx = idx; }
    inline T getIdx( void ) const { return m_idx; }

    inline T setBitAtPos( int pos ) { ctrlBitAtPos( pos, true ); return m_idx;	}
    inline T resetBitAtPos( int pos ) {	ctrlBitAtPos( pos, false ); return m_idx; };
	inline bool testBitAtPos( int pos )
    {
	    #ifndef PERFORMANCE
	    if ( pos >= m_typeTSize )
		    throw BitIndexOverflow( m_typeTSize - 1, pos );
	    #endif
	    return ( ( m_idx & ( T(1) << pos ) ) != 0 );
    }
	inline T flipBitAtPos( int pos )
    {
	    #ifndef PERFORMANCE
	    if (pos >= m_typeTSize) {
		    throw BitIndexOverflow( m_typeTSize - 1, pos );
	    }
	    #endif
	    return m_idx ^= ( T(1) << pos );
    }
	inline T select( const int* pos, int len )
    {
        #ifndef PERFORMANCE
	    for ( int i = 0; i < len; ++i )
		    if ( pos[i] >= m_typeTSize )
			    throw BitIndexOverflow( m_typeTSize - 1, pos[i] );
	    #endif

	    T res = 0;
	    for ( int i = 0; i < len; ++i ) {
		    res <<= 1;
		    if ( testBitAtPos( pos[i] ) )
			    res ^= T(1);
	    }
	    return res;
    }

    inline T operator<<( unsigned shift ) const { return m_idx << shift; }
    inline T operator>>( unsigned shift ) const { return m_idx >> shift; }
    inline T operator=( const T idx ) { m_idx = idx; return idx; }

private:
	T m_idx;
	int m_typeTSize;
};

//--------------------------------------------------------------------
}

#endif
