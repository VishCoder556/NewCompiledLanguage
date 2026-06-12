#include <stdio.h>
#include <assert.h>
#include <stdlib.h>

// Used for struct definitions
#define LinkedList(type) type *STB_CONCAT(type, _head); type *STB_CONCAT(type, _tail); int STB_CONCAT(type, _len);

#define InitLinkedList(parent, type) do {STB_CONCAT(parent.type, _head) = NULL; STB_CONCAT(parent.type, _tail) = NULL; STB_CONCAT(parent.type, _len) = 0;}while(0)

#define AppendToLinkedListPrefix(prefix, type, ...) do {\
if (prefix##_head == NULL){ \
    prefix##_head = malloc(sizeof(type)); \
    prefix##_tail = prefix##_head; \
    prefix##_head->next = NULL; \
    *prefix##_tail = __VA_ARGS__; \
}else if (prefix##_tail != NULL){ \
    prefix##_tail->next = malloc(sizeof(type)); \
    prefix##_tail = (type*)prefix##_tail->next; \
    *prefix##_tail = __VA_ARGS__; \
}else { \
    assert(0 && "Unreachable code: the tail should only be a null pointer if the head is also a null pointer -- error in linked list"); \
} \
    prefix##_len++; \
}while(0)

#define AppendToLinkedList(parent, type, ...) do {AppendToLinkedListPrefix(parent.type, type, __VA_ARGS__);}while (0)

#define SetLinkedList(ll1, ll2, type) \
do { \
    STB_CONCAT(ll1.type, _head) = STB_CONCAT(ll2.type, _head); \
    STB_CONCAT(ll1.type, _tail) = STB_CONCAT(ll2.type, _tail); \
    STB_CONCAT(ll1.type, _len) = STB_CONCAT(ll2.type, _len); \
}while(0)

#define GetLinkedListHead(ll, type) STB_CONCAT(ll.type, _head)
#define GetLinkedListTail(ll, type) ll.type##_tail
#define GetLinkedListLen(ll, type) STB_CONCAT(ll.type, _len)
#define GetLinkedListNextElem(ll, type) (type*)ll->next

#define PopTopLinkedList(parent, type) \
do { \
    type *old_head = GetLinkedListHead(parent, type); \
    if (old_head != NULL) { \
        GetLinkedListHead(parent, type) = (type*)GetLinkedListNextElem(old_head, type); \
        free(old_head); \
        GetLinkedListLen(parent, type)--; \
    } \
} while(0)

// Go through each element in a linked list and free them
#define FreeLinkedList(ll, type) \
do { \
    type *last_elem = ll.type##_head; \
    for (int i=0; i<ll.type##_len; i++){ \
        type *old = last_elem; \
        last_elem = last_elem->next; \
        free(old); \
    }; \
    free(last_elem); \
}while(0)
