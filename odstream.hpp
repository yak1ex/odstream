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
#define ODS(arg) do { yak::debug::ods<char>() arg ; yak::debug::ods<char>().flush(); } while(0)
#define ODS_NOFLUSH(arg) do { yak::debug::ods<char>() arg ; } while(0)
#define WODS(arg) do { yak::debug::ods<wchar_t>() arg ; yak::debug::ods<wchar_t>().flush(); } while(0)
#define WODS_NOFLUSH(arg) do { yak::debug::ods<wchar_t>() arg ; } while(0)
#else // ifdef DEBUG
#define ODS(arg) do { /* nothing */ } while(0)
#define ODS_NOFLUSH(arg) do { /* nothing */ } while(0)
#define WODS(arg) do { /* nothing */ } while(0)
#define WODS_NOFLUSH(arg) do { /* nothing */ } while(0)
#endif // ifdef DEBUG

namespace yak {

	namespace debug_yes {

#ifdef YAK_DEBUG_NO_HEADER_ONLY
		// Use the Construct On First Use Idiom, to avoid static initialization order fiasco
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

		template<typename CharT>
		struct debug_yes_impl
		{
		private:
			typedef traits<CharT> traits_;
			class odstringbuf;
			static odstringbuf& odsbuf();
		public:
			static std::basic_ostream<CharT>& ods();
		}; // struct debug_yes_impl
		extern template struct debug_yes_impl<char>;
		extern template struct debug_yes_impl<wchar_t>;
#else
		// Use the Construct On First Use Idiom, to avoid static initialization order fiasco
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

		template<typename CharT>
		struct debug_yes_impl
		{
		private:
			typedef traits<CharT> traits_;
			class odstringbuf : public std::basic_stringbuf<CharT>
			{
			protected:
				virtual int sync(void) {
					traits_::OutputDebugStr(this->str().c_str());
					this->str(traits_::empty());
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

		template<typename CharT = char>
		inline std::basic_ostream<CharT>& ods() {
			return debug_yes_impl<CharT>::ods();
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
			const pseudo_null_stream& flush(void) { return *this; }
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
			const pseudo_null_stream& flush(void) { return *this; }
			operator std::basic_ostream<CharT>& () { return null_stream(); }
			static pseudo_null_stream& ods() {
				static pseudo_null_stream ods_;
				return ods_;
			}
		};
#endif

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
