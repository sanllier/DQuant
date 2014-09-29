#ifndef H_QMCOMPLEX
#define H_QMCOMPLEX

#include <complex>

#ifndef BASETYPE
    #define BASETYPE double
#endif

//--------------------------------------------------------------------

namespace DQuant {
//--------------------------------------------------------------------

class QMComplex: public std::complex< BASETYPE >
{
public:
    QMComplex( BASETYPE real = 0.0, BASETYPE imag = 0.0 ):std::complex< BASETYPE >( real, imag ) {}
    ~QMComplex() {}

    inline QMComplex conj(void) const { return QMComplex( real(), -imag() ); }
	inline QMComplex operator*(const QMComplex& op) const
    {
	    return QMComplex( real() * op.real() - imag() * op.imag(),
                          real() * op.imag() + imag() * op.real() );
    }
    inline void operator*=(const QMComplex& op)           { (*this) = (*this) * op; }
    inline QMComplex operator+(const QMComplex& op) const { return QMComplex( real() + op.real(), imag() + op.imag() ); }
    inline QMComplex operator-(const QMComplex& op) const { return QMComplex( real() - op.real(), imag() - op.imag() ); }
    inline QMComplex& operator=(const QMComplex& src)     { real( src.real() ); imag( src.imag() ); return *this; }
};

//--------------------------------------------------------------------
}

#endif
