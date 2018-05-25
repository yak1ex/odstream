/***********************************************************************/
/*                                                                     */
/* odprintf.h: Header file for printf-variant using OutputDebugString  */
/*                                                                     */
/*     Copyright (C) 2018 Yak! / Yasutaka ATARASHI                     */
/*                                                                     */
/*     This software is distributed under the terms of a zlib/libpng   */
/*     License.                                                        */
/*                                                                     */
/***********************************************************************/

#ifndef ODPRINTF_H
#define ODPRINTF_H

#include <windows.h>

#ifdef __GNUC__
	#define PRIAS "%s"
	#define PRIWS "%ls"
	#define PRIAC "%c"
	#define PRIWC "%lc"
#endif /* def __GNUC __ */
#ifdef _MSC_VER
#ifdef UNICODE
	#define PRIAS "%S"
	#define PRIWS "%s"
	#define PRIAC "%C"
	#define PRIWC "%c"
#else /* def UNICODE */
	#define PRIAS "%s"
	#define PRIWS "%S"
	#define PRIAC "%c"
	#define PRIWC "%S"
#endif /* def UNICODE */
#endif /* def _MSC_VER */

#ifdef UNICODE
	#define PRITS PRIWS
#else /* def UNICODE */
	#define PRITS PRIAS
#endif /* def UNICODE */

#ifndef DEBUG

	#define OutputDebugPrintf   (void)
	#define OutputDebugPrintfW  (void)
	#define OutputDebugVPrintfW (void)
	#define OutputDebugPrintf   (void)
	#define OutputDebugPrintfA  (void)
	#define OutputDebugVPrintfA (void)

#else /* def DEBUG */

	#ifdef __cplusplus
		#include <cstdarg>
		#include <cwchar> // for WEOF required by Strsafe.h
	#else
		#include <stdarg.h>
		#include <wchar.h> // for WEOF required by Strsafe.h
	#endif
	#ifdef __GNUC__
		#define WEOF ((wint_t)-1)
		#define _vsnprintf vsnprintf
		#define _vsnwprintf vswprintf
	#else // MSVC assumed
	#endif
	#include <Strsafe.h>
	#if defined(__GNUC__) && !defined(__cplusplus) // gcc has some confusion for inline implementation
		#ifdef STRSAFEAPI
			#undef STRSAFEAPI
		#endif
		#define STRSAFEAPI extern __inline__ HRESULT WINAPI
		#if defined(StringVPrintfWorkerA)
			#undef StringVPrintfWorkerA
		#endif
		#if defined(StringVPrintfWorkerW)
			#undef StringVPrintfWorkerW
		#endif
		STRSAFEAPI StringCchVPrintfA(STRSAFE_LPSTR pszDest,size_t cchDest,STRSAFE_LPCSTR pszFormat,va_list argList);
		STRSAFEAPI StringCchVPrintfW(STRSAFE_LPWSTR pszDest,size_t cchDest,STRSAFE_LPCWSTR pszFormat,va_list argList);
		STRSAFEAPI StringVPrintfWorkerA(STRSAFE_LPSTR pszDest,size_t cchDest,STRSAFE_LPCSTR pszFormat,va_list argList);
		STRSAFEAPI StringVPrintfWorkerW(STRSAFE_LPWSTR pszDest,size_t cchDest,STRSAFE_LPCWSTR pszFormat,va_list argList);
	#endif

	#ifdef __cplusplus
		#define INLINE inline
		#define STD(arg) std::arg
	#else /* def __cplusplus */
		#define STD(arg) arg
		#ifdef __GNUC__
			#define INLINE __inline__
		#endif /* def __GNUC__ */
		#ifdef _MSC_VER
			#define INLINE __inline
		#endif /* def _MSC_VER */
	#endif /* def __cplusplus */
	#ifndef INLINE
		#error "Unknown target"
	#endif /* ndef INLINE */

	#ifndef NUM_OUTPUT_DEBUG_PRINTF_BUFFER
		#define NUM_OUTPUT_DEBUG_PRINTF_BUFFER 2048
	#endif

	#define MAKE_IMPL(TCHAR, LPCTSTR, AW) \
	static INLINE void OutputDebugPrintf##AW(LPCTSTR format, ...)\
	{\
		TCHAR buf[NUM_OUTPUT_DEBUG_PRINTF_BUFFER];\
		STD(va_list) ap;\
		va_start(ap, format);\
		StringCchVPrintf##AW(buf, sizeof(buf), format, ap);\
		OutputDebugString##AW(buf);\
		va_end(ap);\
	}\
	static INLINE void OutputDebugVPrintf##AW(LPCTSTR format, STD(va_list) ap)\
	{\
		TCHAR buf[NUM_OUTPUT_DEBUG_PRINTF_BUFFER];\
		StringCchVPrintf##AW(buf, sizeof(buf), format, ap);\
		OutputDebugString##AW(buf);\
	}
	MAKE_IMPL(char, LPCSTR, A)
	MAKE_IMPL(WCHAR, LPCWSTR, W)
	#ifdef UNICODE
		#define OutputDebugPrintf OutputDebugPrintfW
		#define OutputDebugVPrintf OutputDebugVPrintfW
		#define PRITS PRIWS
	#else /* def UNICODE */
		#define OutputDebugPrintf OutputDebugPrintfA
		#define OutputDebugVPrintf OutputDebugVPrintfA
		#define PRITS PRIAS
	#endif /* def UNICODE */


	#undef MAKE_IMPL
	#undef INLINE
	#undef STD

#endif /* def DEBUG */

#endif /* ndef ODPRINTF_H */
