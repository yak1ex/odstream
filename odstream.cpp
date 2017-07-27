/***********************************************************************/
/*                                                                     */
/* odstream.cpp: Source file for stream class using OutputDebugString  */
/*                                                                     */
/*     Copyright (C) 2011,2017 Yak! / Yasutaka ATARASHI                */
/*                                                                     */
/*     This software is distributed under the terms of a zlib/libpng   */
/*     License.                                                        */
/*                                                                     */
/*     $Id: 6513ed3a9f23aca93b18470eacbced77a34566dd $                 */
/*                                                                     */
/***********************************************************************/

#include <sstream>
#include <windows.h>

#define YAK_DEBUG_NO_HEADER_ONLY
#include "odstream.hpp"

namespace yak {

    namespace debug_yes {

		class debug_yes_impl::odstringbuf : public std::stringbuf
		{
		protected:
			virtual int sync(void) {
				OutputDebugString(str().c_str());
				str("");
				return 0;
			}
		};
		debug_yes_impl::odstringbuf& debug_yes_impl::odsbuf() {
			static odstringbuf odsbuf_;
			return odsbuf_;
		}
		std::ostream& debug_yes_impl::ods() {
			static std::ostream ods_(&debug_yes_impl::odsbuf());
			return ods_;
		}

	} // namespace debug_yes

	namespace debug_no {

		class pseudo_null_stream::nullstreambuf : public std::streambuf {};
		pseudo_null_stream::nullstreambuf& pseudo_null_stream::nullbuf() {
			static pseudo_null_stream::nullstreambuf nullbuf_;
			return nullbuf_;
		}
		std::ostream& pseudo_null_stream::null_stream() {
			static std::ostream null_stream_(&nullbuf());
			return null_stream_;
		}

	} // namespace debug_no

} // namespace yak
