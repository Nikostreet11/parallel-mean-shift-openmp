#ifndef PARALLEL_MEAN_SHIFT_OPENMP_ALIAS_H
#define PARALLEL_MEAN_SHIFT_OPENMP_ALIAS_H

#include <memory>

template <typename T>
using Ref = std::shared_ptr<T>;

template <typename T>
using Unique = std::unique_ptr<T>;

#endif //PARALLEL_MEAN_SHIFT_OPENMP_ALIAS_H
