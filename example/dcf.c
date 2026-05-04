#ifdef CInfo
#define GatherCopyInfo 1
#endif

#ifdef FInfo
#define GatherFlopInfo 1
#endif

#define _INTRINSICS_ 1

#include "sisal.h"

extern int sisal_file_io;

int ProvideModuleDataBaseOnAllCompiles;

static void InitGlobalData();

#undef SliceBody1MAIN
static void         SliceBody1MAIN();	/* [Call=F,Rec=F,Par=T,Xmk=F,Mk= ] */
#undef _MAIN
static void         _MAIN();	/* [Call=F,Rec=F,Par=F,Xmk=F,Mk=e] */
#undef IReadDoubleVector
static POINTER      IReadDoubleVector();
#undef IWriteDoubleVector
static void         IWriteDoubleVector();
#undef SFreeDoubleVector
static void         SFreeDoubleVector();
#undef PFreeDoubleVector
static void         PFreeDoubleVector();
#undef ReadArr11
static POINTER      ReadArr11();
#undef WriteArr11
static void         WriteArr11();
#undef IReadArr11
static POINTER      IReadArr11();
#undef IWriteArr11
static void         IWriteArr11();
#undef SFreeArr11
static void         SFreeArr11();
#undef PFreeArr11
static void         PFreeArr11();

struct Args15 {   
struct ActRec *FirstAR; int Count;   
POINTER In1;    POINTER In2;    double  Out1;   
  };

struct Args18 {   
struct ActRec *FirstAR; int Count;   
double  In1;    double  In2;    double  Out1;   
  };

struct Args21 {   
struct ActRec *FirstAR; int Count;   
int     In1;    int     In2;    POINTER Out1;   
  };

struct Args32 {   
struct ActRec *FirstAR; int Count;   
int     In1;    int     In2;    POINTER In3;    POINTER In4;    
  int     In5;    int     In6;    POINTER Out1;   
  };

static void ArrDblCopy( dest, source, num )
POINTER  dest;
POINTER  source;
register int num;
{
  register int  i;
  register double  *src = (double*) source;
  register double  *dst = (double*) dest;
/* _VECTOR_ */
/* _ASSOC_ */
/* _SAFE(dst) */
  for ( i = 0; i < num; i++ )
    dst[i] = src[i];
}

static void ArrArrCopy( dest, source, num )
POINTER  dest;
POINTER  source;
register int num;
{
  register ARRAY **src = (ARRAY**) source;
  register ARRAY **dst = (ARRAY**) dest;
  while ( num-- > 0 ) {
    *dst = *src++;
    MY_LOCK( &(*dst)->Mutex );
    (*dst)->RefCount++;
    FLUSHLINE(&((*dst)->RefCount));
    CACHESYNC;
    MY_UNLOCK( &(*dst)->Mutex );
    FLUSHLINE( &(*dst)->Mutex );
    dst++;
    }
}
/* RUNTIME DECISION FOR STYLE */

static void SliceBody1MAIN( args, lo, hi, step )
FUNCTION args;
int lo, hi, step;
{
  register double tmp14;
  register int tmp18;
  register double tmp17;
  register double tmp19;
  register POINTER tmp15;
  register int tmp16;
  BUFFER buffer1;
  register BUFFERP tmp13 = &buffer1;
  register int tmp11;
  register POINTER tmp10;
  register int tmp12;
  register int tmp9;
  register POINTER tmp8;
  register int tmp7;

  tmp7 = ((struct Args32*)args)->In2;
  tmp8 = ((struct Args32*)args)->In3;
  tmp9 = ((struct Args32*)args)->In6;
  tmp11 = lo;
  tmp12 = hi;
  VecSliceGathATInit( tmp11, double, tmp10, ((struct Args32*)args)->In4 );
  for ( ; tmp11 <= tmp12; tmp11 += step ) {    /* Style=R */
    MAlloc( tmp13, tmp9, double )
    Dbl( tmp14, tmp11 );
    tmp16 = (1);
    GathATInit( double, tmp15, tmp13 );
IncFlopCountA((((double)1)*((double)(((tmp7-tmp16+1) < 0)? 0 : tmp7-tmp16+1))));
    for ( ; tmp16 <= tmp7; tmp16++ ) {  /* Normal Loop */
      Dbl( tmp17, tmp16 );
      OptPlus( tmp17, tmp17, tmp14 );
      GathATUpd( double, tmp15, tmp17 );
      }
    BldAT( double, tmp15, (1), tmp9, tmp13 );
    SetRefCount( tmp15, ARRAY, 1 );
    ASize( tmp16, tmp15 );
    tmp18 = (1);
    tmp17 = (0.0e0);
IncFlopCountA((((double)2)*((double)(((tmp16-tmp18+1) < 0)? 0 : tmp16-tmp18+1))));
    for ( ; tmp18 <= tmp16; tmp18++ ) {  /* Normal Loop */
BoundsCheck1( tmp8, "v2", "i", tmp18, "dcf.sis,dot_product,line=10" );
      AElm( double, tmp14, tmp8, tmp18 );
BoundsCheck1( tmp15, "v1", "i", tmp18, "dcf.sis,dot_product,line=9" );
      AElm( double, tmp19, tmp15, tmp18 );
      OptTimes( tmp14, tmp14, tmp19 );
/* YankedRed */
      OptPlus( tmp17, tmp17, tmp14 );
      }
    VecGathATUpd( tmp11, double, tmp10, tmp17 );
    PFreeDoubleVector( tmp15 );
    }
}

static shared struct Args32 FrAmE1;

static void _MAIN( args )
FUNCTION args;
{
  register double tmp13;
  register int tmp11;
  register POINTER tmp10;
  register int tmp12;
  register int tmp8;
  register int tmp5;
  BUFFER buffer1;
  register BUFFERP tmp9 = &buffer1;
  BUFFER buffer2;
  register BUFFERP tmp7 = &buffer2;
  register int tmp6;
  register FUNCTION tframe14 = (FUNCTION) &FrAmE1;

#undef  MY_LOCK
#undef  MY_UNLOCK
#define MY_LOCK(x)
#define MY_UNLOCK(x)

  tmp5 = ((struct Args21*)args)->In1;
  ((struct Args32*)tframe14)->In2 = ((struct Args21*)args)->In2;
  Max( tmp6, tmp5, (0) );
  MAlloc( tmp7, tmp6, double )
  Max( tmp8, ((struct Args21*)args)->In2, (0) );
  ((struct Args32*)tframe14)->In6 = tmp8;
  MAlloc( tmp9, tmp8, double )
  tmp11 = (1);
  tmp12 = ((struct Args21*)args)->In2;
  GathATInit( double, tmp10, tmp9 );
IncFlopCountA((((double)1)*((double)(((tmp12-tmp11+1) < 0)? 0 : tmp12-tmp11+1))));
  for ( ; tmp11 <= tmp12; tmp11++ ) {  /* Normal Loop */
    Dbl( tmp13, tmp11 );
    OptTimes( tmp13, tmp13, (0.005e0) );
    GathATUpd( double, tmp10, tmp13 );
    }
  BldAT( double, tmp10, (1), tmp8, tmp9 );
  SetRefCount( tmp10, ARRAY, 1 );
  ((struct Args32*)tframe14)->In3 = tmp10;
  BSlices( SPAWN_COMPLEX, struct Args32, SliceBody1MAIN, tframe14           , (1), tmp5, 1,LoopSlices, 0, 0 );
  VecGathATInit( (1), double, ((struct Args32*)tframe14)->In4, ((BUFFERP)tmp7) );
  BldAT( double, ((struct Args32*)tframe14)->Out1, (1), tmp6, tmp7 );
  SetRefCount( ((struct Args32*)tframe14)->Out1, ARRAY, 1 );
  OptSpawnFast( struct Args32, SPAWN_COMPLEX, SliceBody1MAIN, tframe14, 0, 0, (1), tmp5 );
  ((struct Args21*)args)->Out1 = ((struct Args32*)tframe14)->Out1;

#undef  MY_LOCK
#undef  MY_UNLOCK
#define MY_LOCK(x)    MY_LOCK_BACKUP(x)
#define MY_UNLOCK(x)  MY_UNLOCK_BACKUP(x)
}

static int *GInit = NULL;

static void InitGlobalData()
{
  SLockParent;

  if ( GInit == NULL )
    GInit = (int *) Alloc( sizeof(int) );
  else if ( *GInit ) {
    SUnlockParent;
    return;
    }

  *GInit = TRUE;


  SUnlockParent;
}

void SisalMain( args )
POINTER args;
{
#ifdef CInfo
  SaveCopyInfo;
#endif
#ifdef FInfo
  SaveFlopInfo;
#endif
  /* Set file I/O flag for output... */
  sisal_file_io = 0;
  _MAIN( args );
}

static void PFreeDoubleVector( ptr )
POINTER ptr;
{
  register int RefCount;
  {
    register PHYSP   phys;
    register ARRAYP arr = (ARRAYP) ptr;
    MY_LOCK( &arr->Mutex );
    RefCount = --(arr->RefCount);
    FLUSHLINE(&(arr->RefCount));
    CACHESYNC;
    MY_UNLOCK( &arr->Mutex );
    FLUSHLINE( &arr->Mutex );
    CACHESYNC;
    if ( RefCount == 0 ) {
      phys = arr->Phys;
      MY_LOCK( &phys->Mutex );
      RefCount = --(phys->RefCount);
      FLUSHLINE(&(phys->RefCount));
      CACHESYNC;
      MY_UNLOCK( &phys->Mutex );
      FLUSHLINE( &phys->Mutex );
      CACHESYNC;
      if ( RefCount == 0 ) {
        DeAlloc( (POINTER) phys );
        }
      DeAlloc( (POINTER) arr );
      }
  }
}

#undef GatherCopyInfo

FUNCTION ReadFibreInputs()
{
  int previous_io_state = sisal_file_io;
  register struct Args21 *args = (struct Args21*) Alloc( sizeof( struct Args21 ) );
  /* Set file I/O flag for input... */
  sisal_file_io = 0;
  ReadInt( args->In1 );
  ReadInt( args->In2 );
  sisal_file_io = previous_io_state;
  return( (FUNCTION) args );
}

#ifdef CInfo
#define GatherCopyInfo 1
#endif

void WriteFibreOutputs( args )
FUNCTION args;
{
  int previous_io_state = sisal_file_io;
  register struct Args21 *p = (struct Args21*) args;
  /* Set file I/O flag for input... */
  sisal_file_io = 0;
  WriteDoubleVector( (p->Out1) );
  sisal_file_io = previous_io_state;
}
