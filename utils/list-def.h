#pragma once

#ifndef __INNER_LIST_DEF__
#define __INNER_LIST_DEF__

extern "C" struct inner_list_head {
  inner_list_head *next;
  inner_list_head *prev;
};

#define inner_list_init(ptr) ((ptr)->next = (ptr), (ptr)->prev = (ptr))

#define inner_offsetof(type, member) ((size_t)(&((type *)0)->member))

#define inner_containerof(ptr, type, member)                                   \
  ((type *)(((char *)((type *)ptr)) - inner_offsetof(type, member)))

#define inner_list_entry(ptr, type, member) inner_containerof(ptr, type, member)

#define inner_list_add(node, head)                                             \
  ((node)->prev = (head), (node)->next = (head)->next,                         \
   (head)->next->prev = (node), (head)->next = (node))

#define inner_list_add_tail(node, head)                                        \
  ((node)->prev = (head)->prev, (node)->next = (head),                         \
   (head)->prev->next = (node), (head)->prev = (node))

#define inner_list_del(entry)                                                  \
  ((entry)->next->prev = (entry)->prev, (entry)->prev->next = (entry)->next,   \
   (entry)->next = 0, (entry)->prev = 0)

#define inner_list_del_init(entry)                                             \
  do {                                                                         \
    inner_list_del(entry);                                                     \
    inner_list_init(entry);                                                    \
  } while (0)

#define inner_list_is_empty(entry) ((entry) == (entry)->next)

#define inner_list_foreach(iterator, head, type, member)                       \
  for ((iterator) = inner_list_entry((head)->next, type, member);              \
       &((iterator)->member) != (head);                                        \
       (iterator) = inner_list_entry((iterator)->member.next, type, member))

#define inner_list_foreach_entry(pos, head)                                    \
  for ((pos) = (head)->next; (pos) != (head); (pos) = (pos)->next)

#define inner_list_splice_front(inner_list, head)                              \
  do {                                                                         \
    if (!inner_list_is_empty(inner_list)) {                                    \
      inner_list_head *first = (inner_list)->next, *last = (inner_list)->prev; \
      inner_list_head *at = (head)->next;                                      \
      (first)->prev = (head), (head)->next = (first);                          \
      (last)->next = (at), (at)->prev = (last);                                \
    }                                                                          \
  } while (0)

#define inner_list_splice_tail(inner_list, head)                               \
  do {                                                                         \
    if (!inner_list_is_empty(inner_list)) {                                    \
      inner_list_head *first = (inner_list)->next, *last = (inner_list)->prev; \
      inner_list_head *at = (head)->prev;                                      \
      (first)->prev = (at), (at)->next = (first);                              \
      (last)->next = (head), (head)->prev = (last);                            \
    }                                                                          \
  } while (0)

#define inner_list_splice_init(inner_list, head)                               \
  do {                                                                         \
    inner_list_splice_front(inner_list, head);                                 \
    inner_list_init(inner_list);                                               \
  } while (0)

#endif

namespace utils {

typedef inner_list_head ListHead;

} // namespace utils