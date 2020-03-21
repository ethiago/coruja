
// Copyright Ricardo Calheiros de Miranda Cosme 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <boost/core/lightweight_test.hpp>
#include <initializer_list>
#include <range/v3/begin_end.hpp>

template<typename I0, typename S0, typename I1, typename S1>
inline void check_equal(I0 begin0, S0 end0, I1 begin1, S1 end1 )
{  
    for(; begin0 != end0 && begin1 != end1; ++begin0, ++begin1)
        BOOST_TEST(*begin0 == *begin1);
    
    BOOST_TEST( (bool)( begin0 == end0 ) );
    BOOST_TEST( (bool)( begin1 == end1 ) );
}

template<typename ocontainer, typename valye_type>
inline void check_equal(ocontainer&& c, std::initializer_list<valye_type> il)
{
    check_equal(ranges::begin(c), ranges::end(c), ranges::begin(il), ranges::end(il));
}

template<typename Rng0, typename Rng1>
inline void check_equal(Rng0&& rng0, Rng1&& rng1)
{
    check_equal(ranges::begin(rng0), ranges::end(rng0),
                ranges::begin(rng1), ranges::end(rng1));
}

// template<typename ocontainer, typename expected_t>
// void check_equal(const ocontainer& c, const expected_t& expected)
// {
//     BOOST_TEST(c.size() == expected.size());
//     for(auto& e : c) BOOST_TEST(expected.at(e.first) == e.second);
// }
