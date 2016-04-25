#pragma once

#include <vector>
#include <cstdint>
#include <string>

using IterId = int64_t;
using IterVec = std::vector<size_t>;

class IterationSpace {
 private:
  const IterVec dims_;
  IterVec strides_;
 public:
  IterationSpace(const IterVec &dims):
      dims_(dims),
      strides_(dims.size(), 1) {
    for (int i = 0; i < dims_.size(); ++i) {
      for (int j = i + 1; j < dims_.size(); ++j) {
        strides_[i] *= dims_[j];
      }
    }
  }
  ~ IterationSpace() { }

  IterId GetIterationId(const IterVec &iter_vector) {
    IterId id = 0;
    for (auto i = 0; i < dims_.size(); ++i) {
      id += iter_vector[i] * strides_[i];
    }
    return id;
  }

  IterVec GetIterationVector(IterId iter_id) {
    IterVec iter_vec(dims_.size());
    for (auto i = 0; i < dims_.size(); ++i) {
      iter_vec[i] = iter_id / strides_[i];
      iter_id = iter_id % strides_[i];
    }
    return iter_vec;
  }
};


class DependenceGraph {

 public:
  std::string ToString() { return ""; }

};
