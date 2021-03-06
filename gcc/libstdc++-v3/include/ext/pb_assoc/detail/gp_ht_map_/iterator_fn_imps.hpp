// -*- C++ -*-

// Copyright (C) 2005 Free Software Foundation, Inc.
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

// Copyright (C) 2004 Ami Tavory and Vladimir Dreizin, IBM-HRL.

// Permission to use, copy, modify, sell, and distribute this software
// is hereby granted without fee, provided that the above copyright
// notice appears in all copies, and that both that copyright notice and
// this permission notice appear in supporting documentation. None of
// the above authors, nor IBM Haifa Research Laboratories, make any
// representation about the suitability of this software for any
// purpose. It is provided "as is" without express or implied warranty.

/**
 * @file iterator_fn_imps.hpp
 * Contains implementations of gp_ht_map_'s iterators related functions, e.g.,
 *	begin().
 */

PB_ASSOC_CLASS_T_DEC
typename PB_ASSOC_CLASS_C_DEC::iterator
PB_ASSOC_CLASS_C_DEC::s_end_it;

PB_ASSOC_CLASS_T_DEC
typename PB_ASSOC_CLASS_C_DEC::const_iterator
PB_ASSOC_CLASS_C_DEC::s_const_end_it;

PB_ASSOC_CLASS_T_DEC
inline typename PB_ASSOC_CLASS_C_DEC::iterator
PB_ASSOC_CLASS_C_DEC::
begin()
{
  pointer p_value;
  size_type pos;

  get_start_it_state(p_value, pos);

  return (iterator(p_value, pos, this));
}

PB_ASSOC_CLASS_T_DEC
inline typename PB_ASSOC_CLASS_C_DEC::iterator
PB_ASSOC_CLASS_C_DEC::
end()
{
  return (s_end_it);
}

PB_ASSOC_CLASS_T_DEC
inline typename PB_ASSOC_CLASS_C_DEC::const_iterator
PB_ASSOC_CLASS_C_DEC::
begin() const
{
  const_pointer p_value;
  size_type pos;

  get_start_it_state(p_value, pos);

  return (const_iterator(p_value, pos, this));
}

PB_ASSOC_CLASS_T_DEC
inline typename PB_ASSOC_CLASS_C_DEC::const_iterator
PB_ASSOC_CLASS_C_DEC::
end() const
{
  return (s_const_end_it);
}

#ifdef PB_ASSOC_DATA_TRUE_INDICATOR
PB_ASSOC_CLASS_T_DEC
void
PB_ASSOC_CLASS_C_DEC::
inc_it_state(pointer& r_p_value, size_type& r_pos) const
{
  PB_ASSOC_DBG_ASSERT(r_p_value != NULL);

  for (++r_pos; r_pos < m_num_e; ++r_pos)
    {
      const_entry_pointer p_e =& m_a_entries[r_pos];

      if (p_e->m_stat == VALID_ENTRY_STATUS)
	{
	  r_p_value = static_cast<const_pointer>(&p_e->m_value);

	  return;
	}
    }

  r_p_value = NULL;
}
#endif // #ifdef PB_ASSOC_DATA_TRUE_INDICATOR

PB_ASSOC_CLASS_T_DEC
void
PB_ASSOC_CLASS_C_DEC::
inc_it_state(const_pointer& r_p_value, size_type& r_pos) const
{
  PB_ASSOC_DBG_ASSERT(r_p_value != NULL);

  for (++r_pos; r_pos < m_num_e; ++r_pos)
    {
      const_entry_pointer p_e =& m_a_entries[r_pos];

      if (p_e->m_stat == VALID_ENTRY_STATUS)
	{
	  r_p_value = static_cast<const_pointer>(&p_e->m_value);

	  return;
	}
    }

  r_p_value = NULL;
}

PB_ASSOC_CLASS_T_DEC
void
PB_ASSOC_CLASS_C_DEC::
get_start_it_state(const_pointer& r_p_value, size_type& r_pos) const
{
  for (r_pos = 0; r_pos < m_num_e; ++r_pos)
    {
      const_entry_pointer p_e =& m_a_entries[r_pos];

      if (p_e->m_stat == VALID_ENTRY_STATUS)
	{
	  r_p_value =& p_e->m_value;

	  return;
	}
    }

  r_p_value = NULL;
}

PB_ASSOC_CLASS_T_DEC
void
PB_ASSOC_CLASS_C_DEC::
get_start_it_state(pointer& r_p_value, size_type& r_pos)
{
  for (r_pos = 0; r_pos < m_num_e; ++r_pos)
    {
      entry_pointer p_e =& m_a_entries[r_pos];

      if (p_e->m_stat == VALID_ENTRY_STATUS)
	{
	  r_p_value = const_cast<pointer>(&p_e->m_value);

	  return;
	}
    }

  r_p_value = NULL;
}

