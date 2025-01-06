#include "src/swi/ll.h"
#include <cstdlib>

void LL_Init(LLQ *q) {
	q->first = nullptr;
	q->last = nullptr;
	q->data_mfree = free;
}

void LL_Push(LLQ *head, void *itemPtr) {
	LLIST *item = (LLIST *) itemPtr;
	
	item->next = nullptr;
	item->prev = head->last;
	
	if (head->last) {
		LLIST *last = (LLIST *) head->last;
		last->next = item;
	} else {
		head->first = item;
	}
	
	head->last = item;
}

void LL_Remove(LLQ *head, void *itemPtr) {
	LLIST *item = (LLIST *) itemPtr;
	
	LLIST *prev = (LLIST *) item->prev;
	LLIST *next = (LLIST *) item->next;
	
	if (prev)
		prev->next = next;
	
	if (next)
		next->prev = prev;
	
	if (head->first == item)
		head->first = next;
	
	if (head->last == item)
		head->last = prev;
}
