#pragma once

#include <stdint.h>

typedef unsigned int uint;
typedef uintptr_t uintptr;

typedef int64_t i64;
typedef int32_t i32;
typedef int16_t i16;

typedef uint64_t u64;
typedef uint32_t u32;
typedef uint16_t u16;
typedef uint8_t u8;

typedef float f32;
typedef double f64;

#define UNUSED(_type) __attribute__((unused)) _type
