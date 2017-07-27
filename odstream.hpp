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

#ifdef YAK_DEBUG_NO_HEADER_ONLY
#include <iosfwd>
#ifdef DEBUG
#ifndef ODSTREAM_NO_INCLUDE_IOSTREAM
#include <iostream>
#endif // ifndef ODSTREAM_NO_INCLUDE_IOSTREAM
#endif // ifdef DEBUG
#else // ifdef YAK_DEBUG_NO_HEADER_ONLY
#include <iostream>
#include <sstream>
#include <windows.h>
#endif // ifdef YAK_DEBUG_NO_HEADER_ONLY
#ifdef DEBUG
#define ODS(arg) do { yak::debug::ods() arg ; } while(0)
#else // ifdef DEBUG
#define ODS(arg) do { /* nothing */ } while(0)
#endif // ifdef DEBUG

namespace yak {

	namespace debug_yes {

#ifdef YAK_DEBUG_NO_HEADER_ONLY
		// Use the Construct On First Use Idiom, to avoid static initialization order fiasco
		struct debug_yes_impl
		{
		private:
			class odstringbuf;
			static odstringbuf& odsbuf();
		public:
			static std::ostream& ods();
		}; // struct debug_yes_impl
#else // ifdef YAK_DEBUG_NO_HEADER_ONLY
		// Use the Construct On First Use Idiom, to avoid static initialization order fiasco
		struct debug_yes_impl
		{
		private:
			class odstringbuf : public std::stringbuf
			{
			protected:
				virtual int sync(void) {
					OutputDebugString(str().c_str());
					str("");
					return 0;
				}
			};
			static odstringbuf& odsbuf() {
				static odstringbuf odsbuf_;
				return odsbuf_;
			}
		public:
			static std::ostream& ods() {
				static std::ostream ods_(&odsbuf());
				return ods_;
			}
		}; // struct debug_yes_impl
#endif // ifdef YAK_DEBUG_NO_HEADER_ONLY

		inline std::ostream& ods() {
			return debug_yes_impl::ods();
		}
	} // namespace debug_yes

	namespace debug_no {

		class pseudo_null_stream
		{
		private:
#ifdef YAK_DEBUG_NO_HEADER_ONLY
			class nullstreambuf;
			static nullstreambuf& nullbuf();
			static std::ostream& null_stream();
#else // ifdef YAK_DEBUG_NO_HEADER_ONLY
			class nullstreambuf : public std::streambuf {};
			static nullstreambuf& nullbuf() {
				static nullstreambuf nullbuf_;
				return nullbuf_;
			}
			static std::ostream& null_stream() {
				static std::ostream null_stream_(&nullbuf());
				return null_stream_;
			}
#endif // ifdef YAK_DEBUG_NO_HEADER_ONLY
		public:
			template<typename T>
			const pseudo_null_stream& operator << (T) const { return ods(); }
			const pseudo_null_stream& operator << (std::ostream& (*)(std::ostream&)) const { return ods(); }
			const pseudo_null_stream& operator << (std::ios_base& (*)(std::ios_base&)) const { return ods(); }
			const pseudo_null_stream& operator << (std::basic_ios<char>& (*)(std::basic_ios<char>&)) const { return ods(); }
			operator std::ostream& () { return null_stream(); }
			static pseudo_null_stream& ods() {
				static pseudo_null_stream ods_;
				return ods_;
			}
		};

		inline pseudo_null_stream& ods() {
			return pseudo_null_stream::ods();
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
