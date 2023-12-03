#pragma once

#include <utils/head_define.h>

/// target and source address are both aligned to `align`
/// `n` must be the multiple of `align`
template <size_t align>
ALWAYS_INLINE void *aligned_memcpy(void *RESTRICT _tar,
                                   const void *RESTRICT _src, size_t n) {
  char *tar = (char *)(_tar);
  const char *src = (char *)(_src);

  __builtin_assume(U64(n) % align == 0);
  for (; n != 0; n -= align) {
    __builtin_assume(U64(tar) % align == 0);
    __builtin_assume(U64(src) % align == 0);

    __builtin_memcpy(tar, src, align);
    tar += align;
    src += align;
  }
  return _tar;
}