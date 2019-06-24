#ifndef __ETL_PROFILE_H__
#define __ETL_PROFILE_H__

  #define ETL_THROW_EXCEPTIONS
  #define ETL_VERBOSE_ERRORS
  #define ETL_CHECK_PUSH_POP

  #define ETL_TARGET_OS_NONE
  #define ETL_COMPILER_GCC
  #define ETL_CPP11_SUPPORTED 1
  #define ETL_CPP14_SUPPORTED 1
  #define ETL_CPP17_SUPPORTED 0
  //#define ETL_NO_NULLPTR_SUPPORT    1
  #define ETL_NO_LARGE_CHAR_SUPPORT 1
  #define ETL_CPP11_TYPE_TRAITS_IS_TRIVIAL_SUPPORTED 0

  // "etl/profiles/gcc.h":
  #define ETL_TARGET_DEVICE_GENERIC
  #define ETL_TARGET_OS_NONE
  #define ETL_COMPILER_GCC
  #ifdef __cplusplus
    #define ETL_CPP11_SUPPORTED                      (__cplusplus >= 201103L)
    #define ETL_CPP14_SUPPORTED                      (__cplusplus >= 201402L)
    #define ETL_CPP17_SUPPORTED                      (__cplusplus >= 201703L)
  #else
    #define ETL_CPP11_SUPPORTED                      0
    #define ETL_CPP14_SUPPORTED                      0
    #define ETL_CPP17_SUPPORTED                      0
  #endif
  #define ETL_NO_NULLPTR_SUPPORT                     !ETL_CPP11_SUPPORTED
  #define ETL_NO_LARGE_CHAR_SUPPORT                  !ETL_CPP11_SUPPORTED
  #define ETL_CPP11_TYPE_TRAITS_IS_TRIVIAL_SUPPORTED ETL_CPP14_SUPPORTED

#endif // __ETL_PROFILE_H__
