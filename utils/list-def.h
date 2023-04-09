#pragma once

namespace utils {

struct InnerListHead {
  InnerListHead *next;
  InnerListHead *prev;
};

} // namespace utils

#ifndef __INNER_LIST_DEF__
#define __INNER_LIST_DEF__

#define inner_list_init(ptr) ((ptr)->next = (ptr), (ptr)->prev = (ptr))

#define INNER_OFFSETOF(TYPE, MEMBER) ((size_t) & ((TYPE *)0)->MEMBER)

#define INNER_CONTAINEROF(ptr, type, member)                                   \
  ((type *)(((char *)((type *)ptr)) - INNER_OFFSETOF(type, member)))

#define inner_list_entry(ptr, type, member) INNER_CONTAINEROF(ptr, type, member)

/*--------------------------------------------------------------------*/
/* inner_list operation                                                    */
/*--------------------------------------------------------------------*/
#define inner_list_add(node, head)                                             \
  ((node)->prev = (head), (node)->next = (head)->next,                         \
   (head)->next->prev = (node), (head)->next = (node))

#define inner_list_add_tail(node, head)                                        \
  ((node)->prev = (head)->prev, (node)->next = (head),                         \
   (head)->prev->next = (node), (head)->prev = (node))

#define inner_list_del_BETWEEN(p, n) ((n)->prev = (p), (p)->next = (n))

#define inner_list_del(entry)                                                  \
  ((entry)->next->prev = (entry)->prev, (entry)->prev->next = (entry)->next,   \
   (entry)->next = 0, (entry)->prev = 0)

#define inner_list_del_init(entry)                                             \
  do {                                                                         \
    inner_list_del(entry);                                                     \
    inner_list_init(entry);                                                    \
  } while (0)

#define inner_list_is_empty(entry) ((entry) == (entry)->next)

#define INNER_LIST_FOREACH(iterator, head, TYPE, MEMBER)                       \
  for ((iterator) = inner_list_entry((head)->next, TYPE, MEMBER);              \
       &((iterator)->MEMBER) != (head);                                        \
       (iterator) = inner_list_entry((iterator)->MEMBER.next, TYPE, MEMBER))

#define inner_list_foreach(iterator, head, TYPE, MEMBER)                       \
  INNER_LIST_FOREACH(iterator, head, TYPE, MEMBER)

#define inner_list_foreach_entry(pos, head)                                    \
  for ((pos) = (head)->next; (pos) != (head); (pos) = (pos)->next)

#define inner_list_splice(inner_list, head)                                    \
  do {                                                                         \
    if (!inner_list_is_empty(inner_list)) {                                    \
      utils::InnerListHead *first = (inner_list)->next,                        \
                           *last = (inner_list)->prev;                         \
      utils::InnerListHead *at = (head)->next;                                 \
      (first)->prev = (head), (head)->next = (first);                          \
      (last)->next = (at), (at)->prev = (last);                                \
    }                                                                          \
  } while (0)

#define inner_list_splice_init(inner_list, head)                               \
  do {                                                                         \
    inner_list_splice(inner_list, head);                                       \
    inner_list_init(inner_list);                                               \
  } while (0)

#endif