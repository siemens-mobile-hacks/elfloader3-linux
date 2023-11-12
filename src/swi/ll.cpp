#include "swi.h"

#include <cstdlib>

void linked_list_init(LLQ *q) {
	q->first = nullptr;
	q->last = nullptr;
	q->data_mfree = free;
}

void linked_list_push(LLQ *head, void *item_ptr) {
	LLIST *item = (LLIST *) item_ptr;
	
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
