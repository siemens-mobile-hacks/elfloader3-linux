#pragma once

#include <swilib/base.h>

void LL_Init(LLQ *q);
void LL_Push(LLQ *head, void *item);
void LL_Remove(LLQ *head, void *item_ptr);
