/*
	A virsion of implicit free list has header and footer,
	do combine immeriately,find the first fit one to use.
	
	Run in -m32, and double word(4) aligned.
	
	-m32 means that the size of pointer is 4 not 8,which means
	the pointer to previous or next free block is take place 
	just one WSIZE,don't forget it.
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
#define LISTMAX 16

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

#define SET_PTR(p,ptr) (*(unsigned int *)(p) = (unsigned int)(ptr))

#define PRED_PTR(bp) ((char *)bp)
#define SUCC_PTR(bp) ((char *)bp + WSIZE)

#define PRED(ptr) (*(char **)(ptr))
#define SUCC(ptr) (*(char **)(SUCC_PTR(ptr)))

/* my helpful functions */
static void *extend_heap(size_t words);
static void place(void *bp,size_t asize);
static void *coalesce(void *bp);
static void insert_node(void *bp,size_t size);
static void delete_node(void *bp);

/* my global varible */
void *segregated_free_lists[LISTMAX];

/* 
 * mm_init - initialize the malloc package.
 */
int mm_init(void)
{
	char *heap_listp;
	if((heap_listp = mem_sbrk(4 * WSIZE)) == (void *)-1)
		return -1;
	PUT(heap_listp,0);
	PUT(heap_listp + WSIZE,PACK(DSIZE,1));
	PUT(heap_listp + 2 * WSIZE,PACK(DSIZE,1));
	PUT(heap_listp + 3 * WSIZE,PACK(0,1));

	for(int i = 0;i < LISTMAX;++i)
	{
		segregated_free_lists[i] = NULL;
	}

	/* we want to have a CHUNKSIZE size heap but our extend_heap
	 * use words(the number of WSIZE), so we use CHUNKSIZE / WSIZE
	 * to get a CHUNKSIZE size heap when init. */
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
	/* after seeing the mdriver.c file(just find the mm_init()), we can
	 * know that we don't need to call the mm_init(), it does it for us */	
	if(size == 0)
		return NULL;

	size_t asize;
	if(size <= DSIZE)
		asize = 2 * DSIZE; // the min block size (have payload)
	else
		asize = ALI_SIZE(size);

	size_t search_size = asize;
	int index = 0;
	void *bp = NULL;

	while(index < LISTMAX)
	{
		if(search_size <= 1 && segregated_free_lists[index] != NULL)
		{
			bp = segregated_free_lists[index];
			while(bp != NULL && asize > GET_SIZE(HDRP(bp)))
				bp = SUCC(bp);
			
			if(bp != NULL)
				break;
		}		

		search_size >>= 1;
		++index;
	}

	if(bp == NULL)
	{
		if((bp = extend_heap(MAX(asize,CHUNKSIZE) / WSIZE)) == NULL)
			return NULL;
	}

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
	insert_node(ptr,size);
	coalesce(ptr);
}

/*
 * mm_realloc - Implemented simply in terms of mm_malloc and mm_free
 */
void *combine(void *ptr)
{
	int pre_alloc = ALLOC(HDRP(PREV_BLKP(ptr)));
	int next_alloc = ALLOC(HDRP(NEXT_BLKP(ptr)));
	size_t size = GET_SIZE(HDRP(ptr));

	if(!pre_alloc && next_alloc)
	{
		delete_node(PREV_BLKP(ptr));
		ptr = PREV_BLKP(ptr);
		size += GET_SIZE(HDRP(ptr));
		PUT(HDRP(ptr),PACK(size,1));
		PUT(FTRP(ptr),PACK(size,1));
	}
	else if(pre_alloc && !next_alloc)
	{
		delete_node(NEXT_BLKP(ptr));
		size += GET_SIZE(HDRP(NEXT_BLKP(ptr)));
		PUT(HDRP(ptr),PACK(size,1));
		PUT(FTRP(ptr),PACK(size,1));
	}
	else if(!pre_alloc && !next_alloc)
	{
		delete_node(PREV_BLKP(ptr));
		delete_node(NEXT_BLKP(ptr));
		size += GET_SIZE(HDRP(NEXT_BLKP(ptr)))
			+ GET_SIZE(HDRP(PREV_BLKP(ptr)));
		ptr = PREV_BLKP(ptr);
		PUT(HDRP(ptr),PACK(size,1));
		PUT(FTRP(ptr),PACK(size,1));
	}

	return ptr;
}

void *mm_realloc(void *ptr,size_t size)
{
	if(ptr == NULL)
		return mm_malloc(size);

	if(size == 0)
	{
		mm_free(ptr);
		return NULL;
	}

	size_t old_size = GET_SIZE(HDRP(ptr));
	size_t asize = ((size <= DSIZE) ? (2 * DSIZE) : (ALI_SIZE(size)));
	void *p = combine(ptr);
	size_t psize = GET_SIZE(HDRP(p));
	if(p != ptr) /* like conbine with the predecous one */
		memcpy(p,ptr,old_size - DSIZE);

	if(psize == asize)
		return p;
	if(psize > asize)
	{
		if(psize - asize >= 2 * DSIZE)
		{
			PUT(HDRP(p),PACK(asize,1));
			PUT(FTRP(p),PACK(asize,1));
			PUT(HDRP(NEXT_BLKP(p)),PACK(psize - asize,0));
			PUT(FTRP(NEXT_BLKP(p)),PACK(psize - asize,0));
			insert_node(NEXT_BLKP(p),psize - asize);	
		}

		return p;
	}
	else
	{
		if((ptr = mm_malloc(asize)) == NULL)
			return NULL;
		memcpy(ptr,p,psize - DSIZE);
		mm_free(p);
		return ptr;
	}
}


/*
 * Following version is very eassy, but I'm sorry to say it's utilization
 * is too low, and because of this, our program will run out of the memory,
 * we have to make a good change.
 */
//void *mm_realloc(void *ptr, size_t size)
//{                                       
//	if(size == 0){                       
//		mm_free(ptr);                
//		return NULL;              
//	}                              
//                                              
//	size_t asize;                              
//	if(size <= DSIZE)                    
//		asize = 2 * DSIZE;                    
//	else                                      
//		asize = ALI_SIZE(size);         
//                                            
//	void *bp; 
//	if(ptr == NULL)
//	{
//		bp = mm_malloc(asize);
//	}
//	else if(asize <= GET_SIZE(HDRP(ptr)))
//	{
//		bp = mm_malloc(asize);
//		if(bp)
//			memcpy(bp,ptr,asize - DSIZE);
//		free(ptr);
//	}
//	else if(asize > GET_SIZE(HDRP(ptr)))           
//	{                                         
//		bp = mm_malloc(asize);               
//		if(bp)                                
//			memcpy(bp,ptr,GET_SIZE(ptr) - DSIZE);
//		free(ptr);                             
//	}	                                       
//                                              
//	return bp;                                 
//}                                       

/*
 * extend_heap - Use it to expend heap when init or not enough to malloc
 */
static void *extend_heap(size_t words)
{
	void *bp;
	size_t size;

	size = (words % 2) ? (words + 1) * WSIZE : words * WSIZE;
	
	if((long)(bp = mem_sbrk(size)) == -1)
		return NULL;

	/* the old last block(size 0 and allocated) is replaced by HDRP(bp) */
	PUT(HDRP(bp),PACK(size,0));
	PUT(FTRP(bp),PACK(size,0));
	PUT(HDRP(NEXT_BLKP(bp)),PACK(0,1));
	
	insert_node(bp,size);

	return coalesce(bp);
}

void place(void *bp,size_t asize)
{
	size_t size = GET_SIZE(HDRP(bp));
	
	delete_node(bp);
	/* asize and size are all 8*k, so after sub also be 8 * k' */	
	if((size - asize) >= (2 * DSIZE))
	{
		PUT(HDRP(bp),PACK(asize,1));
		PUT(FTRP(bp),PACK(asize,1));
		PUT(HDRP(NEXT_BLKP(bp)),PACK(size - asize,0));
		PUT(FTRP(NEXT_BLKP(bp)),PACK(size - asize,0));
		insert_node(NEXT_BLKP(bp),size - asize);
	}
	else
	{
		PUT(HDRP(bp),PACK(size,1));
		PUT(FTRP(bp),PACK(size,1));	
	}
}

void *coalesce(void *bp)
{
	int pre_alloc = ALLOC(HDRP(PREV_BLKP(bp)));
	int next_alloc = ALLOC(HDRP(NEXT_BLKP(bp)));
	size_t size = GET_SIZE(HDRP(bp));

	if(pre_alloc && next_alloc)
		return bp;
	if(!pre_alloc && next_alloc)
	{
		delete_node(bp);
		size += GET_SIZE(HDRP(PREV_BLKP(bp)));
		bp = PREV_BLKP(bp);
		delete_node(bp);
		PUT(HDRP(bp),PACK(size,0));
		PUT(FTRP(bp),PACK(size,0));
	}
	else if(pre_alloc && !next_alloc)
	{
		delete_node(NEXT_BLKP(bp));
		delete_node(bp);
		size += GET_SIZE(HDRP(NEXT_BLKP(bp)));
		PUT(HDRP(bp),PACK(size,0));
		PUT(FTRP(bp),PACK(size,0));
	}
	else
	{
		size += GET_SIZE(HDRP(PREV_BLKP(bp)))
			+ GET_SIZE(HDRP(NEXT_BLKP(bp)));
		delete_node(bp);
		delete_node(NEXT_BLKP(bp));
		bp = PREV_BLKP(bp);
		delete_node(bp);
		PUT(HDRP(bp),PACK(size,0));
		PUT(FTRP(bp),PACK(size,0));
	}

	insert_node(bp,size);
	
	return bp;
}

static void insert_node(void *bp,size_t size)
{
	/* the size here is already be a aliged number, so use it easily */
	int list_num = 0;
	size_t tmp_size = size;
	
	/* 1 2 3-4 5-8 9-16 ... (the last one can storge to +infinte) */
	while(list_num < LISTMAX - 1 && tmp_size > 1)
	{
		tmp_size >>= 1;
		++list_num;
	}

	/* make each line of free list in order, for first-fit's good utilization */
	void *insert_ptr = NULL;
	void *search_ptr = segregated_free_lists[list_num];
	while((search_ptr != NULL) && (size > GET_SIZE(HDRP(search_ptr))))
	{
		insert_ptr = search_ptr;
		search_ptr = SUCC(search_ptr);
	}

	/* insert before search_ptr */
	if(search_ptr != NULL)
	{
		/* insert after insert_ptr before search_ptr */
		if(insert_ptr != NULL)
		{
			SET_PTR(SUCC_PTR(insert_ptr),bp);
			SET_PTR(PRED_PTR(search_ptr),bp);
			SET_PTR(PRED_PTR(bp),insert_ptr);
			SET_PTR(SUCC_PTR(bp),search_ptr);
		}
		/* insert as the first one and before serch_ptr */
		else
		{
			segregated_free_lists[list_num] = bp;
			SET_PTR(SUCC_PTR(bp),search_ptr);
			SET_PTR(PRED_PTR(search_ptr),bp);
			SET_PTR(PRED_PTR(bp),NULL);
		}
	}
	/* insert as the last */
	else
	{
		/* insert after insert_ptr(not NULL) */
		if(insert_ptr != NULL)
		{
			SET_PTR(SUCC_PTR(insert_ptr),bp);
			SET_PTR(PRED_PTR(bp),insert_ptr);
			SET_PTR(SUCC_PTR(bp),NULL);
		}
		/* the first one insert in the list */
		else
		{
			segregated_free_lists[list_num] = bp;
			SET_PTR(SUCC_PTR(bp),NULL);
			SET_PTR(PRED_PTR(bp),NULL);
		}
	}
}

static void delete_node(void *bp)
{
	size_t tmp = GET_SIZE(HDRP(bp));
	int list_num = 0;
	while(tmp > 1 && list_num < LISTMAX - 1)
	{
		tmp >>= 1;
		++list_num;
	}

	/* xx -> bp  */
	if(PRED(bp) != NULL)
	{
		/* xx -> bp -> yy */
		if(SUCC(bp) != NULL)
		{
			SET_PTR(SUCC_PTR(PRED(bp)),SUCC(bp));
			SET_PTR(PRED_PTR(SUCC(bp)),PRED(bp));
		}
		/* xx -> bp */
		else
		{
			SET_PTR(SUCC_PTR(PRED(bp)),NULL);
		}
	}
	/* bp is the first one */
	else
	{
		/* bp -> xx */
		if(SUCC(bp) != NULL)
		{
			SET_PTR(PRED_PTR(SUCC(bp)),NULL);
			segregated_free_lists[list_num] = SUCC(bp);
		}
		/* only a bp */
		else
		{
			segregated_free_lists[list_num] = NULL;
		}
	}
}

