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

#define ESC(_code) "\x1b[" _code
#define ESCG(_code) ESC(_code "m")
#define RESET ESCG("0")

#define FGID(_id, _msg) ESCG("38;5;" #_id) _msg RESET
#define BOLD(_msg) ESCG("1") _msg RESET

#define ERR3(_kind, _reason, ...)                                                                                      \
  __extension__({                                                                                                      \
    Result *_r = malloc(sizeof(Result));                                                                               \
    asprintf(                                                                                                          \
        &_r->reason,                                                                                                   \
        FGID(210, BOLD("%-7s ")) _reason FGID(183, BOLD("\n\tfn "))                                                    \
            FGID(227, BOLD("%s")) " at " FGID(195, "%s") ":" FGID(157, BOLD("%d")) "\n",                               \
        "Error",                                                                                                       \
        ##__VA_ARGS__,                                                                                                 \
        __PRETTY_FUNCTION__,                                                                                           \
        __FILE__,                                                                                                      \
        __LINE__                                                                                                       \
    );                                                                                                                 \
    _r->err = Error_##_kind;                                                                                           \
    _r->parent = NULL;                                                                                                 \
    _r;                                                                                                                \
  })
#define ERR(...) ERR3(App, ##__VA_ARGS__)
#define ERR_SYS(...) ERR3(System, ##__VA_ARGS__)

extern Result RESULT_OK;
#define OK &RESULT_OK

#define TRY(_result, ...)                                                                                              \
  {                                                                                                                    \
    Result *_res = (_result);                                                                                          \
    if (_res->err) {                                                                                                   \
      Result *_fail = ERR3(App, "Try failed. " __VA_ARGS__);                                                           \
      _fail->parent = _res;                                                                                            \
      return _fail;                                                                                                    \
    }                                                                                                                  \
  }

bool Result_unwrap(Result *res);

#define PANIC(...) Result_unwrap(ERR(__VA_ARGS__))

#define UNWRAP(_result)                                                                                                \
  __extension__({                                                                                                      \
    Result *_res = (_result);                                                                                          \
    bool _ret;                                                                                                         \
    if (_res->err) {                                                                                                   \
      Result *_unwrap = ERR("Tried to unwrap an error result");                                                        \
      _unwrap->parent = _res;                                                                                          \
      _ret = Result_unwrap(_unwrap);                                                                                   \
    }                                                                                                                  \
    else { _ret = Result_unwrap(_res); }                                                                               \
    _ret;                                                                                                              \
  })

#define ASSERT(_assertion, _format, ...)                                                                               \
  if (!(_assertion)) { return ERR("Assertion Failed. " _format, ##__VA_ARGS__); }
