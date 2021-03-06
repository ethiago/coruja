
// Copyright Ricardo Calheiros de Miranda Cosme 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include "coruja/support/signal/signal_id_t.hpp"
#include "coruja/support/type_traits.hpp"

#include <range/v3/range_traits.hpp>

#include <utility>

namespace coruja { namespace view {

template<typename ObservableErasableRange>
class container : ranges::view_base
{
    using Rng = ObservableErasableRange;
    Rng* _rng{nullptr};
    
public:
    
    using value_type = typename Rng::value_type;
    using observed_t = typename Rng::observed_t;
    using for_each_connection_t = typename Rng::for_each_connection_t;
    using before_erase_connection_t = typename Rng::before_erase_connection_t;

    container() = default;
    
    container(Rng& o) : _rng(&o) {}

    ranges::range_iterator_t<Rng> begin() const noexcept
    { return _rng->begin(); }
    
    ranges::range_iterator_t<Rng> begin() noexcept
    { return _rng->begin(); }
    
    ranges::range_iterator_t<Rng> end() const noexcept
    { return _rng->end(); }
    
    ranges::range_iterator_t<Rng> end() noexcept
    { return _rng->end(); }
    
    const observed_t& observed() const noexcept
    { return _rng->observed(); }
    
    template<typename F>
    for_each_connection_t for_each(F&& f)
    { return _rng->for_each(std::forward<F>(f)); }

    template<typename F>
    before_erase_connection_t before_erase(F&& f)
    { return _rng->before_erase(std::forward<F>(f)); }
    
    //Experimental
    signal_id_t for_each_id() const noexcept
    { return _rng->for_each_id(); }
    
    //Experimental
    signal_id_t before_erase_id() const noexcept
    { return _rng->before_erase_id(); }
};

template<typename ObservableErasableRange>
inline
enable_if_t<
    (!std::is_base_of<
     ranges::view_base, ObservableErasableRange>::value)
    &&
    is_observable_erasable_range<ObservableErasableRange>::value,
    container<ObservableErasableRange>
>
view(ObservableErasableRange& rng)
{ return {rng}; }
    
template<typename ObservableErasableRange>
inline
enable_if_t<
    std::is_base_of<
        ranges::view_base, remove_reference_t<ObservableErasableRange>>::value,
    remove_reference_t<ObservableErasableRange>
>
view(ObservableErasableRange&& rng)
{ return std::forward<ObservableErasableRange>(rng); }

template<typename ObservableErasableRange>
using view_t =
    meta::_t<std::decay<decltype(view(std::declval<ObservableErasableRange>()))>>;


}}
