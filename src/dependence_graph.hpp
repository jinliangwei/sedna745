#pragma once

#include <vector>
#include <cstdint>
#include <string>
#include <map>

using IterId = int64_t;
using IterVec = std::vector<size_t>;
using Edge = std::pair<IterId, IterId>;
using EdgeSet = std::set<Edge>;
using Vertex = IterId;
using Neighbors = std::vector<Vertex>;
using AdjacencyList = std::map<Vertex, Neighbors>;
using VertexSet = std::set<Vertex>;
using Color = int;
using ColorSet = std::map<Color, VertexSet>;

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

  IterationSpace(const IterationSpace& other):
      dims_(other.dims_),
      strides_(other.strides_) { }

  size_t
  GetNumberIterations() const {
    size_t num_iterations = 1;
    for (int i = 0; i < dims_.size(); ++i) {
      num_iterations *= dims_[i];
    }
    return num_iterations;
  }

  IterId GetIterationId(const IterVec &iter_vector) const {
    IterId id = 0;
    for (auto i = 0; i < dims_.size(); ++i) {
      id += iter_vector[i] * strides_[i];
    }
    return id;
  }

  IterVec GetIterationVector(IterId iter_id) const {
    IterVec iter_vec(dims_.size());
    for (auto i = 0; i < dims_.size(); ++i) {
      iter_vec[i] = iter_id / strides_[i];
      iter_id = iter_id % strides_[i];
    }
    return iter_vec;
  }

  std::string
  IterIdToString(IterId iter_id) const {
    IterVec iter_vec = GetIterationVector(iter_id);
    std::string s("(");
    size_t i = 0;
    for (auto v : iter_vec) {
      s += std::to_string(v);
      if (++i < iter_vec.size())
        s += ",";
    }
    s += ")";
    return s;
  }
};

class DependenceGraph {
 private:
  IterationSpace iteration_space_;
  EdgeSet edge_set_;
  AdjacencyList adjacency_list_;
  std::vector<int> colors_;
 public:
  DependenceGraph(const IterationSpace &iteration_space):
      iteration_space_(iteration_space) { }
  DependenceGraph(const DependenceGraph &dg):
      iteration_space_(dg.iteration_space_),
      edge_set_(dg.edge_set_) { }
  void
  AddEdge(Vertex iter1, Vertex iter2) {
    edge_set_.emplace(iter1, iter2);
    adjacency_list_[iter1].push_back(iter2);
    adjacency_list_[iter2].push_back(iter1);
  }

  void
  Print() {
    for (auto edge : edge_set_) {
      std::cout << iteration_space_.IterIdToString(edge.first)
                << " -- " << iteration_space_.IterIdToString(edge.second)
                << std::endl;
    }
  }

  int MinColorNotUsedByNeighbors(Vertex v) {
    Neighbors& neighbors = adjacency_list_[v];
    std::set<Color> neighbor_colors;
    for (const auto& n : neighbors)
      neighbor_colors.insert(colors_[n]);

    int min_color = 1;
    while (neighbor_colors.find(min_color) != neighbor_colors.end())
      ++min_color;

    assert(min_color < colors_.size());
    return min_color;
  }

  void GreedyColor() {
    int n = iteration_space_.GetNumberIterations();
    colors_.resize(n, 0);
    for (int i = 0; i < n; ++i)
      colors_[i] = MinColorNotUsedByNeighbors(i);

    ColorSet color_set;
    for (int i = 0; i < n; ++i)
      color_set[colors_[i]].insert(i);

    for (const auto& cs : color_set) {
      std::cout << "The following iterations can be executed in parallel\n";
      for (const auto i : cs.second)
        std::cout << iteration_space_.IterIdToString(i) << "\n";
    }
  }
};
