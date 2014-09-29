#ifdef __cplusplus
 extern "C" {
#endif
    extern void shmem_barrier_all( void );
    extern void shmem_put( void *dstaddr, void *srcaddr, int length, int node );
//    extern void shmem_get( void *dstaddr, void *srcaddr, int length, int node );
    extern void shmem_int_p( int *dstaddr, int src, int node );
    extern void shmem_long_p( long *dstaddr, long src, int node );
    extern void shmem_short_p( short *dstaddr, short src, int node );
    extern void shmem_double_p( double *dstaddr, double src, int node );
    extern void shmem_float_p( float *dstaddr, float src, int node );
//    extern void shmem_double_add( double *dstaddr, double src, int node );
//    extern void shmem_int_add( int *dstaddr, int src, int node );
    extern void shmem_register_handler(void(*f)(int,void*,int),int n);
    extern void shmem_send(void *srcaddr, int type,int length, int node );
    extern void* shmem_xsend(int type,int length, int node );
    extern void shmem_sendnb(void *srcaddr1,int type, int length1,  int node, void *srcaddr2, int length2);
    extern int shmem_my_pe( void );
    extern int shmem_n_pes( void );
//    extern void* shmem_base(void)
    extern void dislib_flush(void);
    extern void dislib_unflush(void);
    extern void dislib_wait(void);
    extern void flush_send(int);
    extern int shmem_init(int *,char***);
    extern void shmem_finalize(void);

// Collectives:
    extern void shmem_double_allsum(double*);
//    extern void shmem_int_allor(int*);
    extern void shmem_int_allsum(int*);
    extern void shmem_long_allsum(long*);
    extern void shmem_long_allmax(long*);
    extern void shmem_long_allmin(long*);
    extern void shmem_int_toall(int *addr,int rootpe);
    extern void shmem_double_toall(double *addr,int rootpe);
//    extern void shmem_int_a2a(int*);
//    extern void shmem_allgatherv(int*);
//    extern void shmem_bcast(int rootpe, void *addr,int len);
    extern int shmem_coarray_all(void *section, int sectionsize, void *capointer[]);
    extern double shmem_time();
#ifdef __cplusplus
 }
#endif

#define my_pe shmem_my_pe
#define num_pes shmem_n_pes

#define shmem_long_put( dst, src, l, n ) shmem_put( (dst), (src), (l)*sizeof(long), (n) )
#define shmem_int_put( dst, src, l, n ) shmem_put( (dst), (src), (l)*sizeof(int), (n) )
#define shmem_short_put( dst, src, l, n ) shmem_put( (dst), (src), (l)*sizeof(short), (n) )
#define shmem_double_put( dst, src, l, n ) shmem_put( (dst), (src), (l)*sizeof(double), (n) )
#define shmem_float_put( dst, src, l, n ) shmem_put( (dst), (src), (l)*sizeof(float), (n) )

//#define shmem_int_get( dst, src, l, n ) shmem_get( (dst), (src), (l)*sizeof(int), (n) )
//#define shmem_double_get( dst, src, l, n ) shmem_get( (dst), (src), (l)*sizeof(double), (n) )