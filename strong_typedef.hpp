#ifndef STRONG_TYPEDEF_HPP
#define STRONG_TYPEDEF_HPP

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8
// strong_typedef.hpp:

// (C) Copyright 2002 Robert Ramey - http://www.rrsd.com .
// Use, modification and distribution is subject to the Boost Software
// License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org/libs/serialization for updates, documentation, and revision history.

// macro used to implement a strong typedef.  strong typedef
// guarantees that two types are distinguised even though the
// share the same underlying implementation.  typedef does not create
// a new type.  STRONG_TYPEDEF(T, D) creates a new type named D
// that operates as a type T.

#define STRONG_TYPEDEF(T, D)                                    \
struct D                                                        \
{                                                               \
	T t;                                                      \
	explicit D(const T t_) : t(t_) {};                          \
	D() : t() {};                                                      \
	D(const D & t_) : t(t_.t){}                                 \
	D & operator=(const D & rhs) { t = rhs.t; return *this;}    \
	D & operator=(const T & rhs) { t = rhs; return *this;}      \
	operator const T & () const {return t; }                    \
	operator T & () { return t; }                               \
	bool operator==(const D & rhs) const { return t == rhs.t; } \
	bool operator<(const D & rhs) const { return t < rhs.t; }   \
};

#endif // STRONG_TYPEDEF_HPP
