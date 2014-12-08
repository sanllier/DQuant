#ifndef H_DEFS
#define H_DEFS

//-------------------------------------------------------------------

enum
{
	MASTERID = 0
};

//-------------------------------------------------------------------

#ifndef BASETYPE
    #define BASETYPE double
#endif

#ifndef MPI_BASETYPE
    #define MPI_BASETYPE MPI_DOUBLE
#endif

#ifndef MPI_COMPLEXTYPE
    #define MPI_COMPLEXTYPE MPI_DOUBLE_COMPLEX
#endif

//-------------------------------------------------------------------
#endif
