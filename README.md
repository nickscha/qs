# qs
A C89 standard compliant, single header, nostdlib (no C Standard Library) Quicksort Algorithm (QS).

For more information please look at the "qs.h" file or take a look at the "examples" or "tests" folder.

> [!WARNING]
> THIS PROJECT IS A WORK IN PROGRESS! ANYTHING CAN CHANGE AT ANY MOMENT WITHOUT ANY NOTICE! USE THIS PROJECT AT YOUR OWN RISK!

<p align="center">
  <a href="https://github.com/nickscha/qs/releases">
    <img src="https://img.shields.io/github/v/release/nickscha/qs?style=flat-square&color=blue" alt="Latest Release">
  </a>
  <a href="https://github.com/nickscha/qs/releases">
    <img src="https://img.shields.io/github/downloads/nickscha/qs/total?style=flat-square&color=brightgreen" alt="Downloads">
  </a>
  <a href="https://opensource.org/licenses/MIT">
    <img src="https://img.shields.io/badge/License-MIT-yellow.svg?style=flat-square" alt="License">
  </a>
  <img src="https://img.shields.io/badge/Standard-C89-orange?style=flat-square" alt="C Standard">
  <img src="https://img.shields.io/badge/nolib-nostdlib-lightgrey?style=flat-square" alt="nostdlib">
</p>

## **Features**
- **C89 compliant** — portable and legacy-friendly  
- **Single-header API** — just include `qs.h`  
- **nostdlib** — no dependency on the C Standard Library  
- **Minimal binary size** — optimized for small executables  
- **Cross-platform** — Windows, Linux, MacOs 
- **Strict compilation** — built with aggressive warnings & safety checks  

## Quick Start

Download or clone qs.h and include it in your project.

```C
#include "qs.h" /* Quicksort Algorithm */

/* Define your own comparision function */
static int int_cmp(void *a, void *b)
{
  int ai = *(int *)a;
  int bi = *(int *)b;
  return (ai > bi) - (ai < bi);
}

int main(void) {

    int arr[] = {42, 17, 8, 99, 4, 75, 23, 5, 1, 88, 2, 77, 55, 31, 19, 66, 87, 234, 2929};
    unsigned int n = sizeof(arr) / sizeof(arr[0]);
    unsigned int i;

    /* Run the quicksort */
    qs_quicksort(arr, n, sizeof(arr[0]), int_cmp)

    /* Afterwards the "arr[]" is sorted */

    return 0;
}
```

### Insertion-Sort Settings

By default this quicksort algorithm falls back to a insertion sort for <= 24 elements.
If you want to change that threshold you can either use the compile flag `-DQS_INSERTION_SORT_THRESHOLD=32` or define it before including the header.

```C
#define QS_INSERTION_SORT_THRESHOLD 32
#include "csr.h"
```

If you don't need any insertion sort and want to save some extra branches you can also disable insertion sort fallback completely.

```C
#define QS_INSERTION_SORT_DISABLE
#include "csr.h"
```

## Run Example: nostdlib, freestsanding

In this repo you will find the "examples/qs_win32_nostdlib.c" with the corresponding "build.bat" file which
creates an executable only linked to "kernel32" and is not using the C standard library and executes the program afterwards.

## "nostdlib" Motivation & Purpose

nostdlib is a lightweight, minimalistic approach to C development that removes dependencies on the standard library. The motivation behind this project is to provide developers with greater control over their code by eliminating unnecessary overhead, reducing binary size, and enabling deployment in resource-constrained environments.

Many modern development environments rely heavily on the standard library, which, while convenient, introduces unnecessary bloat, security risks, and unpredictable dependencies. nostdlib aims to give developers fine-grained control over memory management, execution flow, and system calls by working directly with the underlying platform.

### Benefits

#### Minimal overhead
By removing the standard library, nostdlib significantly reduces runtime overhead, allowing for faster execution and smaller binary sizes.

#### Increased security
Standard libraries often include unnecessary functions that increase the attack surface of an application. nostdlib mitigates security risks by removing unused and potentially vulnerable components.

#### Reduced binary size
Without linking to the standard library, binaries are smaller, making them ideal for embedded systems, bootloaders, and operating systems where storage is limited.

#### Enhanced performance
Direct control over system calls and memory management leads to performance gains by eliminating abstraction layers imposed by standard libraries.

#### Better portability
By relying only on fundamental system interfaces, nostdlib allows for easier porting across different platforms without worrying about standard library availability.
