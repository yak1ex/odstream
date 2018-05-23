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
#include <memory>

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
#define CALLDEBUG(arg) (void)(arg)
#else /* def DEBUG */
#define CALLDEBUG(arg) (void)0
#endif /* def DEBUG */
#define ODS_IMPL(type, ods, arg)         (CALLDEBUG((*yak::debug::ods<type>() arg).flush()))
#define ODS_NOFLUSH_IMPL(type, ods, arg) (CALLDEBUG(*yak::debug::ods<type>() arg))
#define ODS(arg)                         ODS_IMPL(char, ods, arg)
#define ODS_(arg)                        ODS_IMPL(char, ods_, arg)
#define ODS_NOFLUSH(arg)                 ODS_NOFLUSH_IMPL(char, ods, arg)
#define ODS_NOFLUSH_(arg)                ODS_NOFLUSH_IMPL(char, ods_, arg)
#define WODS(arg)                        ODS_IMPL(wchar_t, ods, arg)
#define WODS_(arg)                       ODS_IMPL(wchar_t, ods_, arg)
#define WODS_NOFLUSH(arg)                ODS_NOFLUSH_IMPL(wchar_t, ods, arg)
#define WODS_NOFLUSH_(arg)               ODS_NOFLUSH_IMPL(wchar_t, ods_, arg)

namespace yak {

	namespace debug_yes {

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

		namespace detail {
			struct critical_section : CRITICAL_SECTION
			{
				critical_section() { InitializeCriticalSection(this); }
				~critical_section() { DeleteCriticalSection(this); }
				template<typename T>
				static LPCRITICAL_SECTION getPtr()
				{
					static critical_section cs;
					return &cs;
				}
			};
			struct cs_deleter
			{
			private:
				LPCRITICAL_SECTION pcs;
			public:
				cs_deleter(LPCRITICAL_SECTION pcs) : pcs(pcs) {}
				template<typename T>
				void operator()(T*) { LeaveCriticalSection(pcs); }
			};
		} // namespace detail

		// without lock
		template<typename CharT = char>
		inline std::basic_ostream<CharT>* ods_() {
			return &debug_yes_impl<CharT>::ods();
		}

		template<typename CharT = char>
		inline std::basic_ostream<CharT>* ods_flush_() {
			return &debug_yes_impl<CharT>::ods().flush();
		}

		// with lock
		template<typename CharT = char>
		inline std::unique_ptr<std::basic_ostream<CharT>, detail::cs_deleter> ods() {
			LPCRITICAL_SECTION pcs = detail::critical_section::getPtr<CharT>();
			EnterCriticalSection(pcs);
			return std::unique_ptr<std::basic_ostream<CharT>, detail::cs_deleter>(ods_<CharT>(), detail::cs_deleter(pcs));
		}

		template<typename CharT = char>
		inline std::unique_ptr<std::basic_ostream<CharT>, detail::cs_deleter> ods_flush() {
			LPCRITICAL_SECTION pcs = detail::critical_section::getPtr<CharT>();
			EnterCriticalSection(pcs);
			auto p = std::unique_ptr<std::basic_ostream<CharT>, detail::cs_deleter>(ods_<CharT>(), detail::cs_deleter(pcs));
			p->flush();
			return std::move(p);
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

		namespace detail {
			template<typename CharT>
			struct holder
			{
			private:
				pseudo_null_stream<CharT>& r;
			public:
				holder(pseudo_null_stream<CharT>& r) : r(r) {}
				std::basic_ostream<CharT>* operator->() const { return &static_cast<std::basic_ostream<CharT>&>(r); }
				pseudo_null_stream<CharT>& operator*() { return r; }
			};
		} // namespace detail

		template<typename CharT = char>
		inline detail::holder<CharT> ods_() {
			return detail::holder<CharT>(pseudo_null_stream<CharT>::ods());
		}

		template<typename CharT = char>
		inline detail::holder<CharT> ods_flush_() {
			return detail::holder<CharT>(pseudo_null_stream<CharT>::ods());
		}

		template<typename CharT = char>
		inline detail::holder<CharT> ods() {
			return detail::holder<CharT>(pseudo_null_stream<CharT>::ods());
		}

		template<typename CharT = char>
		inline detail::holder<CharT> ods_flush() {
			return detail::holder<CharT>(pseudo_null_stream<CharT>::ods());
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
