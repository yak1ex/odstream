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

#include <iostream>
#include <sstream>
#include <windows.h>

#include "odstream.hpp"

template struct yak::debug_yes::traits<char>;
template struct yak::debug_yes::traits<wchar_t>;

template struct yak::debug_yes::debug_yes_impl<char>;
template struct yak::debug_yes::debug_yes_impl<wchar_t>;

template struct yak::debug_no::pseudo_null_stream<char>;
template struct yak::debug_no::pseudo_null_stream<wchar_t>;
