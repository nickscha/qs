/* qs.h - v0.2 - public domain data structures - nickscha 2025

A C89 standard compliant, single header, nostdlib (no C Standard Library) Quicksort Algorithm (QS).

LICENSE

  Placed in the public domain and also MIT licensed.
  See end of file for detailed license information.

*/
#ifndef QS_H
#define QS_H

/* #############################################################################
 * # COMPILER SETTINGS
 * #############################################################################
 */
/* Check if using C99 or later (inline is supported) */
#if __STDC_VERSION__ >= 199901L
#define QS_INLINE inline
#define QS_API static
#elif defined(__GNUC__) || defined(__clang__)
#define QS_INLINE __inline__
#define QS_API static
#elif defined(_MSC_VER)
#define QS_INLINE __inline
#define QS_API static
#else
#define QS_INLINE
#define QS_API static
#endif

#ifndef QS_INSERTION_SORT_DISABLE
#ifndef QS_INSERTION_SORT_THRESHOLD
#define QS_INSERTION_SORT_THRESHOLD 24
#endif
#endif

#ifndef QS_TMPBUF
#define QS_TMPBUF 512 /* pivot buffer size for safety */
#endif

typedef int (*qs_cmp_func)(void *, void *);

QS_API QS_INLINE void qs_swap(char *a, char *b, unsigned size)
{
  if (a == b)
  {
    return;
  }

  if (size == 4)
  {
    int tmp = *(int *)a;
    *(int *)a = *(int *)b;
    *(int *)b = tmp;
    return;
  }

  if (size == 8)
  {
    long tmp1 = *(long *)a;
    long tmp2 = *((long *)a + 1);
    *(long *)a = *(long *)b;
    *((long *)a + 1) = *((long *)b + 1);
    *(long *)b = tmp1;
    *((long *)b + 1) = tmp2;
    return;
  }

  /* Fallback for arbitrary sizes */
  while (size--)
  {
    char tmp = *a;
    *a++ = *b;
    *b++ = tmp;
  }
}

/* Median-of-three pivot selection */
QS_API QS_INLINE int qs_median_of_three(char *base, unsigned size, int low, int high, qs_cmp_func cmp)
{
  int mid = low + ((high - low) >> 1);
  char *pa = base + low * (int)size;
  char *pb = base + mid * (int)size;
  char *pc = base + high * (int)size;

  if (cmp(pa, pb) > 0)
  {
    qs_swap(pa, pb, size);
  }

  if (cmp(pa, pc) > 0)
  {
    qs_swap(pa, pc, size);
  }

  if (cmp(pb, pc) > 0)
  {
    qs_swap(pb, pc, size);
  }

  return (int)((pb - base) / (int)size);
}

/* Hoare partitioning using median-of-three */
QS_API QS_INLINE int qs_partition(char *base, unsigned size, int low, int high, qs_cmp_func cmp)
{
  int pidx = qs_median_of_three(base, size, low, high, cmp);
  char *lo;
  char *hi;
  int step;
  unsigned int k;

  unsigned char pivot_buf[QS_TMPBUF];
  char *pivot_src = base + pidx * (int)size;

  for (k = 0; k < size; ++k)
  {
    pivot_buf[k] = (unsigned char)pivot_src[k];
  }

  if ((unsigned)size <= (unsigned)QS_TMPBUF)
  {
    unsigned k;
    unsigned char *src = (unsigned char *)(base + pidx * (int)size);

    for (k = 0; k < size; ++k)
    {
      pivot_buf[k] = src[k];
    }
  }
  else
  {
    /*
     * For very large records we will move the pivot to 'high' in array to
     * stabilize (avoid allocating huge local buffer). This keeps correctness.
     */
    qs_swap(base + pidx * (int)size, base + high * (int)size, size);
    /* copy pivot from high (it is inside array but at a stable slot) */
    {
      unsigned k;
      unsigned char *src = (unsigned char *)(base + high * (int)size);

      for (k = 0; k < size; ++k)
      {
        pivot_buf[k] = src[k];
      }
    }
  }

  /* pointer walkers to avoid repeated multiplications */
  lo = base + low * (int)size;
  hi = base + high * (int)size;
  step = (int)size;

  for (;;)
  {
    while (cmp(lo, pivot_buf) < 0)
    {
      lo += step;
    }

    while (cmp(hi, pivot_buf) > 0)
    {
      hi -= step;
    }

    if (lo >= hi)
    {
      return (int)((hi - base) / (int)size);
    }

    qs_swap(lo, hi, size);
    lo += step;
    hi -= step;
  }
}

#ifndef QS_INSERTION_SORT_DISABLE
/* Insertion sort */
QS_API QS_INLINE void qs_insertion_sort(char *base, unsigned size, int low, int high, qs_cmp_func cmp)
{
  int i, j;
  for (i = low + 1; i <= high; ++i)
  {
    j = i;

    while (j > low && cmp(base + j * (int)size, base + (j - 1) * (int)size) < 0)
    {
      qs_swap(base + j * (int)size, base + (j - 1) * (int)size, size);
      --j;
    }
  }
}
#endif

/* Hybrid Quicksort with manual stack */
QS_API QS_INLINE void qs_quicksort(void *base, unsigned nmemb, unsigned size, qs_cmp_func cmp)
{
  int stack[64];
  int top = 0;
  int low = 0;
  int high = (int)nmemb - 1;

  if (nmemb < 2 || size == 0)
  {
    return;
  }

  stack[top++] = low;
  stack[top++] = high;

  while (top > 0)
  {
    int p;

    high = stack[--top];
    low = stack[--top];

#ifndef QS_INSERTION_SORT_DISABLE
    /* Use insertion sort for small partitions */
    if (high - low <= QS_INSERTION_SORT_THRESHOLD)
    {
      qs_insertion_sort((char *)base, size, low, high, cmp);
      continue;
    }
#endif

    /* Partition */
    p = qs_partition((char *)base, size, low, high, cmp);

    /* Push larger partition first to keep stack shallow */
    if (p - low < high - p)
    {
      if (p + 1 < high)
      {
        stack[top++] = p + 1;
        stack[top++] = high;
      }
      if (low < p)
      {
        stack[top++] = low;
        stack[top++] = p;
      }
    }
    else
    {
      if (low < p)
      {
        stack[top++] = low;
        stack[top++] = p;
      }
      if (p + 1 < high)
      {
        stack[top++] = p + 1;
        stack[top++] = high;
      }
    }
  }
}

#endif /* QS_H */

/*
   ------------------------------------------------------------------------------
   This software is available under 2 licenses -- choose whichever you prefer.
   ------------------------------------------------------------------------------
   ALTERNATIVE A - MIT License
   Copyright (c) 2025 nickscha
   Permission is hereby granted, free of charge, to any person obtaining a copy of
   this software and associated documentation files (the "Software"), to deal in
   the Software without restriction, including without limitation the rights to
   use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
   of the Software, and to permit persons to whom the Software is furnished to do
   so, subject to the following conditions:
   The above copyright notice and this permission notice shall be included in all
   copies or substantial portions of the Software.
   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
   OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
   SOFTWARE.
   ------------------------------------------------------------------------------
   ALTERNATIVE B - Public Domain (www.unlicense.org)
   This is free and unencumbered software released into the public domain.
   Anyone is free to copy, modify, publish, use, compile, sell, or distribute this
   software, either in source code form or as a compiled binary, for any purpose,
   commercial or non-commercial, and by any means.
   In jurisdictions that recognize copyright laws, the author or authors of this
   software dedicate any and all copyright interest in the software to the public
   domain. We make this dedication for the benefit of the public at large and to
   the detriment of our heirs and successors. We intend this dedication to be an
   overt act of relinquishment in perpetuity of all present and future rights to
   this software under copyright law.
   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
   AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
   ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
   WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
   ------------------------------------------------------------------------------
*/
