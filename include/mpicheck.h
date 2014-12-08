#ifndef H_MPICHECK
#define H_MPICHECK

//------------------------------------------------------------

#define CHECK( __ERR_CODE__ ) checkMPIRes( __ERR_CODE__, __FUNCTION__ )

void checkMPIRes( int errCode, const char* location );

//------------------------------------------------------------
#endif
