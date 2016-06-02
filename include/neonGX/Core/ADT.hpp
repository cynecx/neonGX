/*
 * neonGX - Optional.hpp
 *
 * Copyright (c) 2016, cynecx
 *
 * This file is part of neonGX and is distributed under a modified
 * BSD 3-Clause License. See LICENSE.txt for details.
 *
 */

#ifndef NEONGX_OPTIONAL_H
#define NEONGX_OPTIONAL_H

#include <experimental/optional>
#include <experimental/string_view>

namespace neonGX {
  using std::experimental::optional;
  using std::experimental::nullopt_t;
  using std::experimental::nullopt;

  using std::experimental::string_view;

} // end namespace neonGX

#endif // !NEONGX_OPTIONAL_H
