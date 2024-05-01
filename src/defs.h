#pragma once

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

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

#define UNUSED(type) __attribute__((unused)) type

typedef enum {
  Error_Ok = 0,
  Error_GlfwInit,
  Error_GlfwWindow,
  Error_GladLoading,
  Error_ShaderCompilation,
  Error_System,
  Error_ShaderProgramLink,
  Error_ShaderProgramValidation,
  Error_BitstreamEoF,
  Error_AssertionFail,
  Error_TryFail,
} Error;

typedef void (*ResultFreeFn)(void *);

typedef struct Result {
  Error err;
  const void *ctx;
  ResultFreeFn free;
  char *fileName;
  int line;
  struct Result *parent;
  struct Result *self;
  struct Result *cause;
} Result;

extern const char *RESULT_STR[11];

#define ERR3(error, context, freeFn)                                                                                   \
  (Result) { .err = Error_##error, .ctx = context, .free = freeFn, .fileName = __FILE__, .line = __LINE__ }
#define ERR2(error, context) ERR3(error, context, NULL)
#define ERR1(error) ERR2(error, NULL)
#define ERRN(_1, _2, _3, NAME, ...) NAME

#define ERR(...) ERRN(__VA_ARGS__, ERR3, ERR2, ERR1)(__VA_ARGS__)
#define OK                                                                                                             \
  (Result) { .err = Error_Ok, .fileName = __FILE__, .line = __LINE__ }

#define ASSERT(assertion, ...)                                                                                         \
  if (!(assertion)) { return ERR(AssertionFail, __VA_ARGS__); }

#define TRY_ERR(result, errInit)                                                                                       \
  {                                                                                                                    \
    Result og_res = (result);                                                                                          \
    if (og_res.err) {                                                                                                  \
      Result *cause = malloc(sizeof(Result));                                                                          \
      Result res = errInit;                                                                                            \
      *cause = og_res;                                                                                                 \
      res.cause = cause;                                                                                               \
      res.cause->self = cause;                                                                                         \
      if (res.cause->cause != NULL) { res.cause->cause->parent = res.cause; }                                          \
      return res;                                                                                                      \
    }                                                                                                                  \
  }

#define TRY3(result, context, freeFn) TRY_ERR(result, ERR(TryFail, context, freeFn))
#define TRY2(result, context) TRY3(result, context, NULL)
#define TRY1(result) TRY3(result, NULL, NULL)
#define TRY(...) ERRN(__VA_ARGS__, TRY3, TRY2, TRY1)(__VA_ARGS__);

bool ResultUnwrap(Result res);
