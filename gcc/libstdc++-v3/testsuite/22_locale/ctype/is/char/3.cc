// Copyright (C) 2000, 2001, 2002, 2003 Free Software Foundation, Inc.
//
// This file is part of the GNU ISO C++ Library.  This library is free
// software; you can redistribute it and/or modify it under the
// terms of the GNU General Public License as published by the
// Free Software Foundation; either version 2, or (at your option)
// any later version.

// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License along
// with this library; see the file COPYING.  If not, write to the Free
// Software Foundation, 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301,
// USA.

// As a special exception, you may use this file as part of a free software
// library without restriction.  Specifically, if other files instantiate
// templates or use macros or inline functions from this file, or you compile
// this file and link it with other files to produce an executable, this
// file does not by itself cause the resulting executable to be covered by
// the GNU General Public License.  This exception does not however
// invalidate any other reasons why the executable file might be covered by
// the GNU General Public License.

// 22.2.1.3.2 ctype<char> members

#include <locale>
#include <testsuite_hooks.h>

typedef char char_type;

// Per Liboriussen <liborius@stofanet.dk>
void test03()
{
  bool test __attribute__((unused)) = true;
  std::ctype_base::mask maskdata[256];
  for (int i = 0; i < 256; ++i)
    maskdata[i] = std::ctype_base::alpha;
  std::ctype<char>* f = new std::ctype<char>(maskdata);
  std::locale loc_c = std::locale::classic();
  std::locale loc(loc_c, f);
  for (int i = 0; i < 256; ++i) 
    {
      char_type ch = i;
      VERIFY( std::isalpha(ch, loc) );
    }
}

int main() 
{
  test03();
  return 0;
}
