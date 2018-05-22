/***********************************************************************/
/*                                                                     */
/* odstream.hpp: Header file for stream class using OutputDebugString  */
/*                                                                     */
/*     Copyright (C) 2011,2017 Yak! / Yasutaka ATARASHI                */
/*                                                                     */
/*     This software is distributed under the terms of a zlib/libpng   */
/*     License.                                                        */
/*                                                                     */
/*     $Id: 3637c03a3f2adb2e627120e488208f73bb995977 $                 */
/*                                                                     */
/***********************************************************************/

#ifndef ODSTREAM_HPP
#define ODSTREAM_HPP

#include <windows.h>
#include <utility>

#ifdef YAK_DEBUG_NO_HEADER_ONLY
#include <iosfwd>
#ifdef DEBUG
#ifndef ODSTREAM_NO_INCLUDE_IOSTREAM
#include <iostream>
#endif // ifndef ODSTREAM_NO_INCLUDE_IOSTREAM
#endif // ifdef DEBUG
#else // ifdef YAK_DEBUG_NO_HEADER_ONLY
#include <windows.h>
#include <iostream>
#include <sstream>
#endif // ifdef YAK_DEBUG_NO_HEADER_ONLY
#ifdef DEBUG
#define CALLDEBUG (void)
#else /* def DEBUG */
#define CALLDEBUG (void)sizeof
#endif /* def DEBUG */
#define ODS(arg) (CALLDEBUG((yak::debug::ods<char>() arg).flush()))
#define ODS_NOFLUSH(arg) (CALLDEBUG(yak::debug::ods<char>() arg))
#define WODS(arg) (CALLDEBUG((yak::debug::ods<wchar_t>() arg).flush()))
#define WODS_NOFLUSH(arg) (CALLDEBUG(yak::debug::ods<wchar_t>() arg))

namespace yak {

	namespace debug_yes {

		namespace detail {
			struct critical_section : CRITICAL_SECTION
			{
				critical_section() {
					InitializeCriticalSection(this);
				}
				~critical_section() {
					DeleteCriticalSection(this);
				}
			};
			template<typename T>
			struct holder
			{
			private:
				static LPCRITICAL_SECTION getPtr() {
					static critical_section cs;
					return &cs;
				}
				T& t;
				bool hold;
			public:
				holder(T& t) : t(t), hold(true) {
					EnterCriticalSection(getPtr());
				}
				explicit operator T&() { return t; }
				holder(const holder&) = delete;
				holder& operator=(const holder&) = delete;
				holder(holder&& h) : t(h.t), hold(false) {
					std::swap(hold, h.hold);
				}
				holder&& operator=(holder&& h) = delete;
				~holder() {
					if(hold) LeaveCriticalSection(getPtr());
				}
				template<typename U> friend T& operator<<(const holder<T>& h, const U& u) { return h.t << u; }
				T& flush() { return t.flush(); }
			};
		} // namespace detail

#ifdef YAK_DEBUG_NO_HEADER_ONLY
		// Use the Construct On First Use Idiom, to avoid static initialization order fiasco
		namespace detail {
			template<typename CharT>
			struct traits
			{
				static void OutputDebugStr(const char* lp);
				static const char* empty();
			};
			template<>
			struct traits<wchar_t>
			{
				static void OutputDebugStr(const wchar_t* lp);
				static const wchar_t* empty();
			};
			extern template struct traits<char>;
			extern template struct traits<wchar_t>;
		} // namespace detail

		template<typename CharT>
		struct debug_yes_impl
		{
		private:
			typedef detail::traits<CharT> traits;
			class odstringbuf;
			static odstringbuf& odsbuf();
		public:
			static std::basic_ostream<CharT>& ods();
		}; // struct debug_yes_impl
		extern template struct debug_yes_impl<char>;
		extern template struct debug_yes_impl<wchar_t>;
#else
		// Use the Construct On First Use Idiom, to avoid static initialization order fiasco
		namespace detail {
			template<typename CharT>
			struct traits
			{
				static void OutputDebugStr(const char* lp) { ::OutputDebugStringA(lp); }
				static const char* empty() {
					static const char empty_[] = "";
					return empty_;
				}
			};
			template<>
			struct traits<wchar_t>
			{
				static void OutputDebugStr(const wchar_t* lp) { ::OutputDebugStringW(lp); }
				static const wchar_t* empty() {
					static const wchar_t empty_[] = L"";
					return empty_;
				}
			};
		} // namespace detail

		template<typename CharT>
		struct debug_yes_impl
		{
		private:
			typedef detail::traits<CharT> traits;
			class odstringbuf : public std::basic_stringbuf<CharT>
			{
			protected:
				virtual int sync(void) {
					traits::OutputDebugStr(this->str().c_str());
					this->str(traits::empty());
					return 0;
				}
			};
			static odstringbuf& odsbuf() {
				static odstringbuf odsbuf_;
				return odsbuf_;
			}
		public:
			static std::basic_ostream<CharT>& ods() {
				static std::basic_ostream<CharT> ods_(&odsbuf());
				return ods_;
			}
		}; // struct debug_yes_impl
#endif

		// without lock
		template<typename CharT = char>
		inline std::basic_ostream<CharT>& ods_() {
			return debug_yes_impl<CharT>::ods();
		}

		// with lock
		template<typename CharT = char>
		inline detail::holder<std::basic_ostream<CharT>> ods() {
			return detail::holder<std::basic_ostream<CharT>>(ods_<CharT>());
		}

	} // namespace debug_yes

	namespace debug_no {

#ifdef YAK_DEBUG_NO_HEADER_ONLY
		template<typename CharT>
		class pseudo_null_stream
		{
		private:
			class nullstreambuf;
			static nullstreambuf& nullbuf();
			static std::basic_ostream<CharT>& null_stream();
		public:
			template<typename T>
			const pseudo_null_stream& operator << (T) const { return *this; }
			const pseudo_null_stream& operator << (std::basic_ostream<CharT>& (*)(std::basic_ostream<CharT>&)) const { return *this; }
			const pseudo_null_stream& operator << (std::ios_base& (*)(std::ios_base&)) const { return *this; }
			const pseudo_null_stream& operator << (std::basic_ios<CharT>& (*)(std::basic_ios<CharT>&)) const { return *this; }
			const pseudo_null_stream& flush(void) const { return *this; }
			operator std::basic_ostream<CharT>& () { return null_stream(); }
			static pseudo_null_stream& ods();
		};
		extern template struct pseudo_null_stream<char>;
		extern template struct pseudo_null_stream<wchar_t>;
#else // ifdef YAK_DEBUG_NO_HEADER_ONLY
		template<typename CharT>
		class pseudo_null_stream
		{
		private:
			class nullstreambuf : public std::basic_streambuf<CharT> {};
			static nullstreambuf& nullbuf() {
				static nullstreambuf nullbuf_;
				return nullbuf_;
			}
			static std::basic_ostream<CharT>& null_stream() {
				static std::basic_ostream<CharT> null_stream_(&nullbuf());
				return null_stream_;
			}
		public:
			template<typename T>
			const pseudo_null_stream& operator << (T) const { return *this; }
			const pseudo_null_stream& operator << (std::basic_ostream<CharT>& (*)(std::basic_ostream<CharT>&)) const { return *this; }
			const pseudo_null_stream& operator << (std::ios_base& (*)(std::ios_base&)) const { return *this; }
			const pseudo_null_stream& operator << (std::basic_ios<CharT>& (*)(std::basic_ios<CharT>&)) const { return *this; }
			const pseudo_null_stream& flush(void) const { return *this; }
			operator std::basic_ostream<CharT>& () { return null_stream(); }
			static pseudo_null_stream& ods() {
				static pseudo_null_stream ods_;
				return ods_;
			}
		};
#endif

		template<typename CharT = char>
		inline pseudo_null_stream<CharT>& ods_() {
			return pseudo_null_stream<CharT>::ods();
		}

		template<typename CharT = char>
		inline pseudo_null_stream<CharT>& ods() {
			return pseudo_null_stream<CharT>::ods();
		}

	} // namespace debug_no

#ifndef YAK_DEBUG_NO_DECLARE_NAMESPACE
#ifdef DEBUG
	namespace debug = debug_yes;
#else
	namespace debug = debug_no;
#endif
#endif

} // namespace yak

#endif
