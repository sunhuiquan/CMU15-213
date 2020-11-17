#include <semaphore.h>

typedef struct{
	int *buf; 	 	/* buffer */
	int n; 		 	/* maximum nuber of slots */
	int front; 	 	/* buf[(front + 1) % n] is first item */
	int rear; 	 	/* buf[rear % n] is last item */
	sem_t mutex; 	/* protects accesses to buf as a lock */
	sem_t slots; 	/* counts availabe slots */
	sem_t items; 	/* counts available items */
}sbuf_t;

void sbuf_init(sbuf_t *sp,int n);
void sbuf_deinit(sbuf_t *sp);
void sbuf_insert(sbuf_t *sp,int item);
int sbuf_remove(sbuf_t *sp);

