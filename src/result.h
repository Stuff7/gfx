#pragma once

#include <stdio.h>
#include <stdlib.h>

typedef enum {
  Error_Ok = 0,
  Error_App,
  Error_System,
} Error;

typedef struct Result {
  Error err;
  char *reason;
  struct Result *parent;
} Result;

#define ERR3F(_kind, _reason, ...)                                                                                     \
  __extension__({                                                                                                      \
    Result *_r = malloc(sizeof(Result));                                                                               \
    asprintf(                                                                                                          \
        &_r->reason,                                                                                                   \
        "\x1b[1m\x1b[38;5;210m[Error]\x1b[0m " _reason                                                                 \
        "\n\tin function \x1b[1m\x1b[38;5;227m%s\x1b[0m\n\tat \x1b[38;5;195m%s:\x1b[1m\x1b[38;5;157m%d\x1b[0m\n",      \
        __VA_ARGS__                                                                                                    \
    );                                                                                                                 \
    _r->err = Error_##_kind;                                                                                           \
    _r->parent = NULL;                                                                                                 \
    _r;                                                                                                                \
  })
#define ERR3(_kind, _reason, ...) ERR3F(_kind, _reason, __VA_ARGS__, __PRETTY_FUNCTION__, __FILE__, __LINE__)
#define ERR2(_kind, _reason) ERR3F(_kind, _reason, __PRETTY_FUNCTION__, __FILE__, __LINE__)
#define ERR1(_reason) ERR2(App, _reason)
#define ERRN(_1, _2, _3, _NAME, ...) _NAME
#define ERR(...) ERRN(__VA_ARGS__, ERR3, ERR2, ERR1)(__VA_ARGS__)

#define ERRF(...) ERR3(App, __VA_ARGS__)
#define ERR_SYS(...) ERR3(System, __VA_ARGS__)

extern Result RESULT_OK;
#define OK &RESULT_OK

#define TRY2(_result, _reason)                                                                                         \
  {                                                                                                                    \
    Result *_res = (_result);                                                                                          \
    if (_res->err) {                                                                                                   \
      Result *_fail = ERR(_reason);                                                                                    \
      _fail->parent = _res;                                                                                            \
      return _fail;                                                                                                    \
    }                                                                                                                  \
  }
#define TRY1(_result) TRY2(_result, "Try failed")
#define TRYN(_1, _2, _NAME, ...) _NAME
#define TRY(...) TRYN(__VA_ARGS__, TRY2, TRY1)(__VA_ARGS__)

typedef struct {
  float x, y;
} Point;

bool ResultUnwrap(Result *res);

#define UNWRAP(_result)                                                                                                \
  __extension__({                                                                                                      \
    Result *_res = (_result);                                                                                          \
    bool _ret;                                                                                                         \
    if (_res->err) {                                                                                                   \
      Result *_unwrap = ERR("Tried to unwrap an error result");                                                        \
      _unwrap->parent = _res;                                                                                          \
      _ret = ResultUnwrap(_unwrap);                                                                                    \
    }                                                                                                                  \
    else { _ret = ResultUnwrap(_res); }                                                                                \
    _ret;                                                                                                              \
  })

#define ASSERT(assertion, ...)                                                                                         \
  if (!(assertion)) { return ERRF("Assertion Failed: " __VA_ARGS__); }
