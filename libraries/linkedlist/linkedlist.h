// Used for struct definitions
#define LinkedList(type) type *STB_CONCAT(type, _head); type *STB_CONCAT(type, _tail); int STB_CONCAT(type, _len);

#define InitLinkedList(parent, type) STB_CONCAT(parent.type, _head) = NULL; STB_CONCAT(parent.type, _tail) = NULL; STB_CONCAT(parent.type, _len) = 0;

#define AppendToLinkedListPrefix(prefix, type, ...) \
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

#define AppendToLinkedList(parent, type, ...) AppendToLinkedListPrefix(parent.type, type, __VA_ARGS__)

#define SetLinkedList(ll1, ll2, type) \
    ll1.type##_head = ll2.type##_head; \
    ll1.type##_tail = ll2.type##_tail; \
    ll1.type##_len = ll2.type##_len;

#define GetLinkedListHead(ll, type) ll.type##_head
#define GetLinkedListTail(ll, type) ll.type##_tail
#define GetLinkedListLen(ll, type) ll.type##_len
#define GetLinkedListNextElem(ll, type) (type*)ll->next

#define PopTopLinkedList(parent, type) GetLinkedListHead(parent, type) = (type*)GetLinkedListNextElem(GetLinkedListHead(parent, type), type); GetLinkedListLen(parent, type)--;


// Go through each element in a linked list and free them
#define FreeLinkedList(ll, type) \
    type *last_elem == ll.type##_head; \
    for (int i=0; i<ll.type##_len; i++){ \
        type *old = last_elem; \
        last_elem = last_elem->next; \
        free(old); \
    }; \
    free(last_elem);

