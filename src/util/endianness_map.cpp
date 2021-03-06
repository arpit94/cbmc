/*******************************************************************\

Module:

Author: Daniel Kroening, kroening@kroening.com

\*******************************************************************/

#include "endianness_map.h"

#include <ostream>

#include "invariant.h"
#include "std_types.h"
#include "pointer_offset_size.h"
#include "arith_tools.h"
#include "namespace.h"

void endianness_mapt::output(std::ostream &out) const
{
  for(std::vector<size_t>::const_iterator it=map.begin();
      it!=map.end();
      ++it)
  {
    if(it!=map.begin())
      out << ", ";
    out << *it;
  }
}

void endianness_mapt::build(const typet &src, bool little_endian)
{
  if(little_endian)
    build_little_endian(src);
  else
    build_big_endian(src);
}

void endianness_mapt::build_little_endian(const typet &src)
{
  mp_integer s=pointer_offset_bits(src, ns); // error is -1
  if(s<=0)
    return;

  std::size_t new_size=map.size()+integer2size_t(s);
  map.reserve(new_size);

  for(std::size_t i=map.size(); i<new_size; ++i)
    map.push_back(i);
}

void endianness_mapt::build_big_endian(const typet &src)
{
  if(src.id()==ID_symbol)
    build_big_endian(ns.follow(src));
  else if(src.id()==ID_c_enum_tag)
    build_big_endian(ns.follow_tag(to_c_enum_tag_type(src)));
  else if(src.id()==ID_unsignedbv ||
          src.id()==ID_signedbv ||
          src.id()==ID_fixedbv ||
          src.id()==ID_floatbv ||
          src.id()==ID_c_enum ||
          src.id()==ID_c_bit_field)
  {
    // these do get re-ordered!
    mp_integer bits=pointer_offset_bits(src, ns); // error is -1
    CHECK_RETURN(bits>=0);

    size_t bits_int=integer2size_t(bits), base=map.size();

    for(size_t bit=0; bit<bits_int; bit++)
    {
      map.push_back(base+bits_int-1-bit);
    }
  }
  else if(src.id()==ID_struct)
  {
    const struct_typet &struct_type=to_struct_type(src);

    // todo: worry about padding being in wrong order
    for(struct_typet::componentst::const_iterator
        it=struct_type.components().begin();
        it!=struct_type.components().end();
        it++)
    {
      build_big_endian(it->type());
    }
  }
  else if(src.id()==ID_array)
  {
    const array_typet &array_type=to_array_type(src);

    // array size constant?
    mp_integer s;
    if(!to_integer(array_type.size(), s))
    {
      while(s>0)
      {
        build_big_endian(array_type.subtype());
        --s;
      }
    }
  }
  else if(src.id()==ID_vector)
  {
    const vector_typet &vector_type=to_vector_type(src);

    mp_integer s;
    if(to_integer(vector_type.size(), s))
      CHECK_RETURN(false);

    while(s>0)
    {
      build_big_endian(vector_type.subtype());
      --s;
    }
  }
  else
  {
    // everything else (unions in particular)
    // is treated like a byte-array
    mp_integer s=pointer_offset_bits(src, ns); // error is -1
    if(s<=0)
      return;

    std::size_t new_size=map.size()+integer2size_t(s);
    map.reserve(new_size);

    for(std::size_t i=map.size(); i<new_size; ++i)
      map.push_back(i);
  }
}
