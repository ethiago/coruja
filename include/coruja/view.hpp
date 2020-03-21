#pragma once

#include <type_traits>

#include "coruja/object/view/object.hpp"
#include "coruja/container/view/container.hpp"

namespace coruja { namespace view {

template<typename Observable>
using view_t =
    meta::_t<std::decay<decltype(view(std::declval<Observable>()))>>;

}}