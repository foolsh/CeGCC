// Copyright (C) 2003, 2004, 2005 Free Software Foundation, Inc.
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

// 20.1.5 allocator requirements / 20.4.1.1 allocator members

#include <list>
#include <cstdlib>
#include <testsuite_hooks.h>

class Bob
{
public:
  static void* operator new(size_t sz)
  { return std::malloc(sz); }
};

// libstdc++/10378
void test01()
{
  using namespace std;
  bool test __attribute__((unused)) = true;

  list<Bob> uniset;
  uniset.push_back(Bob());
}

int main()
{
  test01();
  return 0;
}
