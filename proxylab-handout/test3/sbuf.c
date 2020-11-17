#include "sbuf.h"
#include <stdlib.h>

void sbuf_init(sbuf_t *sp,int n)
{
	sp->buf = calloc(n,sizeof(int));
	sp->n = n;
	sp->front = sp->rear = 0;
	sem_init(&sp->mutex,0,1);
	sem_init(&sp->slots,0,n);
	sem_init(&sp->items,0,0);
}

void sbuf_deinit(sbuf_t *sp)
{
	free(sp->buf);
}

void sbuf_insert(sbuf_t *sp,int item)
{
	sem_wait(&sp->slots); /* only when slots != 0 can insert */
	sem_wait(&sp->mutex);
	sp->buf[(++sp->rear) % (sp->n)] = item;
	sem_post(&sp->mutex);
	sem_post(&sp->items);
}

int sbuf_remove(sbuf_t *sp)
{
	int items;
	sem_wait(&sp->items); /* only when items != 0 can remove */
	sem_wait(&sp->mutex);
	items = sp->buf[(++sp->front) % (sp->n)];
	sem_post(&sp->mutex);
	sem_post(&sp->slots);
	return items;
}

