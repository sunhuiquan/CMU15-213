/*
	A virsion of implicit free list has header and footer,
	do combine immeriately,find the first fit one to use.
	
	Run in -m32, and double word(4) aligned.
*/

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <string.h>

#include "mm.h"
#include "memlib.h"

team_t team = {"sunhuiquan","2020","/11","/","3"};

/* word(4) double and word(8) */
#define WSIZE 4
#define DSIZE 8
#define CHUNKSIZE (1<<12)

/* single word (4) or double word (8) alignment */
#define ALIGNMENT 8

/* rounds up to the nearest multiple of ALIGNMENT */
#define ALIGN(size) (((size) + (ALIGNMENT-1)) & ~0x7)
#define ALI_SIZE(size) (ALIGN((size) + (DSIZE)))

/* som helpful macros */
#define MAX(x,y) ((x) > (y) ? (x) : (y))

#define PACK(size,alloc) ((size) | (alloc))

#define GET(p) (*(unsigned int *)(p))
#define PUT(p,val) (*(unsigned int *)(p) = (val))

#define GET_SIZE(p) (GET(p) & ~0x7)
#define ALLOC(p) (GET(p) & 0x1)

/* void * first change to  char * because the 
   size of char is 1, then we can use 1 as 1 
   not the sizeof(Type) */
#define HDRP(bp) ((char *)(bp) - WSIZE)
#define FTRP(bp) ((char *)(bp) + GET_SIZE(HDRP(bp)) - DSIZE)

#define NEXT_BLKP(bp) ((char *)(bp) + GET_SIZE(HDRP(bp)))
#define PREV_BLKP(bp) ((char *)(bp) - GET_SIZE((char *)(bp) - DSIZE))

/* use it to find fit from the first block */
static char *heap_listp = 0;

/* my helpful functions */
static void *extend_heap(size_t words);
static void place(void *bp,size_t asize);
static void *find_fit(size_t asize);
static void *coalesce(void *bp);

/* my heap check functions */
//static void printblock(void *bp);
//static void checkheap(int verbose);
//static void checkblock(void *bp);

/* 
 * mm_init - initialize the malloc package.
 */
int mm_init(void)
{
	if((heap_listp = mem_sbrk(4 * WSIZE)) == (void *)-1)
		return -1;
	PUT(heap_listp,0);
	PUT(heap_listp + WSIZE,PACK(DSIZE,1));
	PUT(heap_listp + 2 * WSIZE,PACK(DSIZE,1));
	PUT(heap_listp + 3 * WSIZE,PACK(0,1));

	heap_listp += 2 * WSIZE;

	/* we want to have a CHUNKSIZE size heap
	   but our extend_heap use words(the number
	   of WSIZE), so we use CHUNKSIZE / WSIZE
	   to get a CHUNKSIZE size heap when init
	*/
	if(extend_heap(CHUNKSIZE / WSIZE) == NULL)
		return -1;

    return 0;
}

/* 
 * mm_malloc - Allocate a block by incrementing the brk pointer.
 *     Always allocate a block whose size is a multiple of the alignment.
 */
void *mm_malloc(size_t size)
{
	char * bp;
	size_t asize,enough_size;

	if(size == 0)
		return NULL;

	if(size <= DSIZE)
		asize = 2 * DSIZE;
	else
		asize = ALI_SIZE(size);

	if((bp = find_fit(asize)) != NULL)
	{
		place(bp,asize);
		return bp;
	}

	enough_size = MAX(asize,CHUNKSIZE);
	if((bp = extend_heap(enough_size / WSIZE)) == NULL)
		return NULL;
	place(bp,asize);
	return bp;
}

/*
 * mm_free - Freeing a block does nothing.
 */
void mm_free(void *ptr)
{
	size_t size = GET_SIZE(HDRP(ptr));
	PUT(HDRP(ptr),PACK(size,0));
	PUT(FTRP(ptr),PACK(size,0));

	coalesce(ptr);
}

/*
 * mm_realloc - Implemented simply in terms of mm_malloc and mm_free
 */
void *mm_realloc(void *ptr, size_t size)
{
	/* just free */
	if(size == 0){
		mm_free(ptr);
		return NULL;
	}	

	void *new_address;
	size_t asize = ALI_SIZE(size);
	/* just malloc */
	if(ptr == NULL)
	{
		new_address = mm_malloc(asize);
	}/* we don't think the less or equal one */
	else if(size >= GET_SIZE(HDRP(ptr)))
	{
		new_address = mm_malloc(asize);
		if(new_address)
			memcpy(new_address,ptr,GET_SIZE(HDRP(ptr)) - DSIZE);
		mm_free(ptr);
	}
	else
	{
		new_address = mm_malloc(asize);
		if(new_address)
			memcpy(new_address,ptr,asize - DSIZE);
		mm_free(ptr);
	}

	return new_address;
}

/*
 * extend_heap - Use it to expend heap when init or not enough to malloc
 */
static void *extend_heap(size_t words)
{
	char *bp;
	size_t size;

	size = (words % 2) ? (words + 1) * WSIZE : words * WSIZE;
	
	if((long)(bp = mem_sbrk(size)) == -1)
		return NULL;

	/* the old last block(size 0 and allocated) is replaced by HDRP(bp) */
	PUT(HDRP(bp),PACK(size,0));
	PUT(FTRP(bp),PACK(size,0));
	PUT(HDRP(NEXT_BLKP(bp)),PACK(0,1));
	
	return coalesce(bp);
}

void place(void *bp,size_t asize)
{
	size_t size,rest;
	size  = GET_SIZE(HDRP(bp));
	rest = size - asize;
	
	/* the asize is aligned and the size is also aligned, then the rest is same too */
	/* rest > DSIZE means the new block has payload(not involve haed and foot)*/
	if(rest > DSIZE)
	{
		PUT(HDRP(bp),PACK(asize,1));
		PUT(FTRP(bp),PACK(asize,1));
		PUT(HDRP(NEXT_BLKP(bp)),PACK(rest,0));
		PUT(FTRP(NEXT_BLKP(bp)),PACK(rest,0));
	}
	else
	{
		PUT(HDRP(bp),PACK(size,1));
		PUT(FTRP(bp),PACK(size,1));
	}
}

void *find_fit(size_t asize)
{
	/* preface block's bp */
	void *fp = heap_listp;

	while(GET_SIZE(HDRP(fp)) != 0)
	{
		if(GET_SIZE(HDRP(fp)) >= asize && ALLOC(HDRP(fp)) == 0)
			break;
	
		fp = NEXT_BLKP(fp);
	}

	if(GET_SIZE(HDRP(fp)) == 0)
		fp = NULL;

	return fp;
}

void *coalesce(void *bp)
{
	int pre_alloc,next_alloc;
	size_t size;

	size = GET_SIZE(HDRP(bp));
	pre_alloc = ALLOC(HDRP(PREV_BLKP(bp)));
	next_alloc = ALLOC(FTRP(NEXT_BLKP(bp)));

	if(!pre_alloc && next_alloc)
	{
		bp = PREV_BLKP(bp);
		size = size + GET_SIZE(HDRP(bp));
		PUT(HDRP(bp),PACK(size,0));
		PUT(FTRP(bp),PACK(size,0));
	}
	else if(pre_alloc && !next_alloc)
	{
		size = size + GET_SIZE(HDRP(NEXT_BLKP(bp)));
		PUT(HDRP(bp),PACK(size,0));
		PUT(FTRP(bp),PACK(size,0));		
	}
	else if(!pre_alloc && !next_alloc)
	{
		size = size + GET_SIZE(HDRP(NEXT_BLKP(bp)));
		size = size + GET_SIZE(HDRP(PREV_BLKP(bp)));
		bp = PREV_BLKP(bp);
		PUT(HDRP(bp),PACK(size,0));
		PUT(FTRP(bp),PACK(size,0));
	}
	return bp;
}

