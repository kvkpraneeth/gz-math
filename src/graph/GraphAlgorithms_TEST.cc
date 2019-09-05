/*
 * Copyright (C) 2017 Open Source Robotics Foundation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
*/

#include <gtest/gtest.h>
#include <string>

#include "ignition/math/graph/Graph.hh"
#include "ignition/math/graph/GraphAlgorithms.hh"

using namespace ignition;
using namespace math;
using namespace graph;

// Define a test fixture class template.
template <class T>
class GraphTestFixture : public testing::Test
{
};

// The list of graphs we want to test.
using GraphTypes = ::testing::Types<DirectedGraph<int, double>,
                                    UndirectedGraph<int, double>>;
TYPED_TEST_CASE(GraphTestFixture, GraphTypes);

/////////////////////////////////////////////////
TYPED_TEST(GraphTestFixture, BreadthFirstSort)
{
  TypeParam graph(
  {
    // Vertices.
    {{"A", 0, 0}, {"B", 1, 1}, {"C", 2, 2}, {"D", 3, 3}, {"E", 4, 4},
     {"F", 5, 5}, {"G", 6, 6}},
    // Edges.
    {{{0, 1}, 2.0}, {{0, 2}, 3.0}, {{0, 4}, 4.0},
     {{1, 3}, 2.0}, {{1, 5}, 3.0}, {{2, 6}, 4.0},
     {{5, 4}, 2.0}}
  });

  auto res = BreadthFirstSort(graph, 0);
  std::vector<VertexId> expected = {0, 1, 2, 4, 3, 5, 6};
  EXPECT_EQ(expected, res);
}

/////////////////////////////////////////////////
TEST(GraphTest, DepthFirstSortDirected)
{
  DirectedGraph<int, double> graph(
  {
    // Vertices.
    {{"A", 0, 0}, {"B", 1, 1}, {"C", 2, 2}, {"D", 3, 3}, {"E", 4, 4},
     {"F", 5, 5}, {"G", 6, 6}},
    // Edges.
    {{{0, 1}, 2.0}, {{0, 2}, 3.0}, {{0, 4}, 4.0}, {{1, 3}, 2.0},
     {{1, 5}, 3.0}, {{2, 6}, 4.0}, {{5, 4}, 2.0}}
  });

  auto res = DepthFirstSort(graph, 0);
  std::vector<VertexId> expected = {0, 4, 2, 6, 1, 5, 3};
  EXPECT_EQ(expected, res);
}

/////////////////////////////////////////////////
TEST(UndirectedGraphTest, DepthFirstSortUndirected)
{
  UndirectedGraph<int, double> graph(
  {
    // Vertices.
    {{"A", 0, 0}, {"B", 1, 1}, {"C", 2, 2}, {"D", 3, 3}, {"E", 4, 4},
     {"F", 5, 5}, {"G", 6, 6}},
    // Edges.
    {{{0, 1}, 2.0}, {{0, 2}, 3.0}, {{0, 4}, 4.0}, {{1, 3}, 2.0},
     {{1, 5}, 3.0}, {{2, 6}, 4.0}, {{5, 4}, 2.0}}
  });

  auto res = DepthFirstSort(graph, 0);
  std::vector<VertexId> expected = {0, 4, 5, 1, 3, 2, 6};
  EXPECT_EQ(expected, res);
}

/////////////////////////////////////////////////
TEST(GraphTestFixture, DijkstraUndirected)
{
  ///              (6)                 |
  ///           0-------1              |
  ///           |      /|\             |
  ///           |     / | \(5)         |
  ///           | (2)/  |  \           |
  ///           |   /   |   2          |
  ///        (1)|  / (2)|  /           |
  ///           | /     | /(5)         |
  ///           |/      |/             |
  ///           3-------4              |
  ///              (1)                 |
  UndirectedGraph<int, double> graph(
  {
    // Vertices.
    {{"0", 0, 0}, {"1", 1, 1}, {"2", 2, 2}, {"3", 3, 3}, {"4", 4, 4}},
    // Edges.
    {{{0, 1}, 2.0, 6.0}, {{0, 3}, 3.0, 1.0},
     {{1, 2}, 4.0, 5.0}, {{1, 3}, 4.0, 2.0}, {{1, 4}, 4.0, 2.0},
     {{2, 4}, 2.0, 5.0},
     {{3, 4}, 2.0, 1.0}}
  });

  // Inexistent source vertex.
  auto res = Dijkstra(graph, 99);
  EXPECT_TRUE(res.empty());

  // Inexistent destination vertex.
  res = Dijkstra(graph, 0, 99);
  EXPECT_TRUE(res.empty());

  // Calculate all shortest paths from 0.
  res = Dijkstra(graph, 0);

  ASSERT_NE(res.end(), res.find(0));
  EXPECT_DOUBLE_EQ(0, res.at(0).first);
  EXPECT_EQ(0u, res.at(0).second);
  ASSERT_NE(res.end(), res.find(1));
  EXPECT_DOUBLE_EQ(3, res.at(1).first);
  EXPECT_EQ(3u, res.at(1).second);
  ASSERT_NE(res.end(), res.find(2));
  EXPECT_DOUBLE_EQ(7, res.at(2).first);
  EXPECT_EQ(4u, res.at(2).second);
  ASSERT_NE(res.end(), res.find(3));
  EXPECT_DOUBLE_EQ(1, res.at(3).first);
  EXPECT_EQ(0u, res.at(3).second);
  ASSERT_NE(res.end(), res.find(4));
  EXPECT_DOUBLE_EQ(2, res.at(4).first);
  EXPECT_EQ(3u, res.at(4).second);

  // Calculate the shortest path between 0 and 1.
  res = Dijkstra(graph, 0, 1);

  ASSERT_NE(res.end(), res.find(1));
  EXPECT_DOUBLE_EQ(3, res.at(1).first);
  EXPECT_EQ(3u, res.at(1).second);
}

/////////////////////////////////////////////////
TEST(GraphTestFixture, DijkstraDirected)
{
  ///              (6)                  |
  ///           0------>1               |
  ///           |      /|\              |
  ///           |     / | \(5)          |
  ///           | (2)/  |  ┘            |
  ///           |   /   |   2           |
  ///        (1)|  / (2)|  /            |
  ///           | /     | /(5)          |
  ///           VL      VL              |
  ///           3------>4               |
  ///              (1)                  |
  DirectedGraph<int, double> graph(
  {
    // Vertices.
    {{"0", 0, 0}, {"1", 1, 1}, {"2", 2, 2}, {"3", 3, 3}, {"4", 4, 4}},
    // Edges.
    {{{0, 1}, 2.0, 6.0}, {{0, 3}, 3.0, 1.0},
     {{1, 2}, 4.0, 5.0}, {{1, 3}, 4.0, 2.0}, {{1, 4}, 4.0, 2.0},
     {{2, 4}, 2.0, 5.0},
     {{3, 4}, 2.0, 1.0}}
  });

  // Inexistent source vertex.
  auto res = Dijkstra(graph, 99);
  EXPECT_TRUE(res.empty());

  // Inexistent destination vertex.
  res = Dijkstra(graph, 0, 99);
  EXPECT_TRUE(res.empty());

  // Calculate all shortest paths from 0.
  res = Dijkstra(graph, 0);

  ASSERT_NE(res.end(), res.find(0));
  EXPECT_DOUBLE_EQ(0, res.at(0).first);
  EXPECT_EQ(0u, res.at(0).second);
  ASSERT_NE(res.end(), res.find(1));
  EXPECT_DOUBLE_EQ(6, res.at(1).first);
  EXPECT_EQ(0u, res.at(1).second);
  ASSERT_NE(res.end(), res.find(2));
  EXPECT_DOUBLE_EQ(11, res.at(2).first);
  EXPECT_EQ(1u, res.at(2).second);
  ASSERT_NE(res.end(), res.find(3));
  EXPECT_DOUBLE_EQ(1, res.at(3).first);
  EXPECT_EQ(0u, res.at(3).second);
  ASSERT_NE(res.end(), res.find(4));
  EXPECT_DOUBLE_EQ(2, res.at(4).first);
  EXPECT_EQ(3u, res.at(4).second);

  // Calculate the shortest path between 0 and 1.
  res = Dijkstra(graph, 0, 1);

  ASSERT_NE(res.end(), res.find(1));
  EXPECT_DOUBLE_EQ(6, res.at(1).first);
  EXPECT_EQ(0u, res.at(1).second);
}

/////////////////////////////////////////////////
TEST(GraphTestFixture, ConnectedComponents)
{
  // Connected components of an empty graph.
  UndirectedGraph<int, double> emptyGraph;
  auto components = ConnectedComponents(emptyGraph);
  EXPECT_TRUE(components.empty());

  UndirectedGraph<int, double> graph(
  {
    // Vertices.
    {{"A", 0, 0}, {"B", 1, 1}, {"C", 2, 2}, {"D", 3, 3}, {"E", 4, 4}},
    // Edges.
    {{{0, 2}, 2.0, 6.0},
     {{1, 4}, 4.0, 5.0}}
  });

  // Connected components of a graph with three components.
  components = ConnectedComponents(graph);
  ASSERT_EQ(3u, components.size());

  // Component #0.
  auto component = components.at(0);
  auto vertices = component.Vertices();
  EXPECT_EQ(2u, vertices.size());
  EXPECT_NE(vertices.end(), vertices.find(0));
  EXPECT_NE(vertices.end(), vertices.find(2));
  for (auto const &vertexPair : vertices)
  {
    auto &vertex = vertexPair.second.get();
    switch (vertex.Id())
    {
      case 0:
      {
        EXPECT_EQ("A", vertex.Name());
        EXPECT_EQ(0, vertex.Data());
        break;
      }
      case 2:
      {
        EXPECT_EQ("C", vertex.Name());
        EXPECT_EQ(2, vertex.Data());
        break;
      }
      default:
        FAIL();
    };
  }

  // Component #1.
  component = components.at(1);
  vertices = component.Vertices();
  EXPECT_EQ(2u, vertices.size());
  EXPECT_NE(vertices.end(), vertices.find(1));
  EXPECT_NE(vertices.end(), vertices.find(4));
  for (auto const &vertexPair : vertices)
  {
    auto &vertex = vertexPair.second.get();
    switch (vertex.Id())
    {
      case 1:
      {
        EXPECT_EQ("B", vertex.Name());
        EXPECT_EQ(1, vertex.Data());
        break;
      }
      case 4:
      {
        EXPECT_EQ("E", vertex.Name());
        EXPECT_EQ(4, vertex.Data());
        break;
      }
      default:
        FAIL();
    };
  }

  // Component #2.
  component = components.at(2);
  vertices = component.Vertices();
  EXPECT_EQ(1u, vertices.size());
  EXPECT_NE(vertices.end(), vertices.find(3));
  for (auto const &vertexPair : vertices)
  {
    auto &vertex = vertexPair.second.get();
    switch (vertex.Id())
    {
      case 3:
      {
        EXPECT_EQ("D", vertex.Name());
        EXPECT_EQ(3, vertex.Data());
        break;
      }
      default:
        FAIL();
    };
  }
}

/////////////////////////////////////////////////
TEST(GraphTestFixture, FindSourceVertex)
{
  ///              (6)                  |
  ///           0------>1               |
  ///           |      /|\              |
  ///           |     / | \(5)          |
  ///           | (2)/  |  ┘            |
  ///           |   /   |   2           |
  ///        (1)|  / (2)|  /            |
  ///           | /     | /(5)          |
  ///           VL      VL              |
  ///           3------>4               |
  ///              (1)                  |
  DirectedGraph<int, double> graph(
  {
    // Vertices.
    {{"0", 0, 0}, {"1", 1, 1}, {"2", 2, 2}, {"3", 3, 3}, {"4", 4, 4}},
    // Edges.
    {{{0, 1}, 2.0, 6.0}, {{0, 3}, 3.0, 1.0},
     {{1, 2}, 4.0, 5.0}, {{1, 3}, 4.0, 2.0}, {{1, 4}, 4.0, 2.0},
     {{2, 4}, 2.0, 5.0},
     {{3, 4}, 2.0, 1.0}}
  });

  // invalid input vertex and no edges
  EXPECT_FALSE(FindSourceVertex(graph, 99).first.Valid());
  EXPECT_TRUE(FindSourceVertex(graph, 99).second.empty());

  // starting from 0, 1, 2 will lead to vertex 0
  auto pair0 = FindSourceVertex(graph, 0);
  EXPECT_EQ(0u, pair0.first.Id());
  EXPECT_EQ(0u, pair0.second.size());

  auto pair1 = FindSourceVertex(graph, 1);
  EXPECT_EQ(0u, pair1.first.Id());
  EXPECT_EQ(1u, pair1.second.size());
  EXPECT_TRUE(pair1.second[0].Valid());
  EXPECT_DOUBLE_EQ(6.0, pair1.second[0].Weight());

  auto pair2 = FindSourceVertex(graph, 2);
  EXPECT_EQ(0u, pair2.first.Id());
  EXPECT_EQ(2u, pair2.second.size());
  EXPECT_TRUE(pair2.second[0].Valid());
  EXPECT_DOUBLE_EQ(5.0, pair2.second[0].Weight());
  EXPECT_TRUE(pair2.second[1].Valid());
  EXPECT_DOUBLE_EQ(6.0, pair2.second[1].Weight());

  // vertices 3 and 4 both have multiple incoming edges, so are invalid
  EXPECT_FALSE(FindSourceVertex(graph, 3).first.Valid());
  EXPECT_FALSE(FindSourceVertex(graph, 4).first.Valid());
  EXPECT_TRUE(FindSourceVertex(graph, 3).second.empty());
  EXPECT_TRUE(FindSourceVertex(graph, 4).second.empty());

  // test a graph with a cycle
  DirectedGraph<int, double> cycle(
  {
    // Vertices.
    {{"0", 0, 0}, {"1", 1, 1}, {"2", 2, 2}, {"3", 3, 3}, {"4", 4, 4}},
    // Edges.
    {{{0, 1}, 2.0, 6.0}, {{1, 2}, 3.0, 1.0},
     {{2, 3}, 4.0, 5.0}, {{3, 4}, 4.0, 2.0},
     {{4, 0}, 4.0, 2.0}}
  });
  EXPECT_FALSE(FindSourceVertex(cycle, 0).first.Valid());
  EXPECT_FALSE(FindSourceVertex(cycle, 1).first.Valid());
  EXPECT_FALSE(FindSourceVertex(cycle, 2).first.Valid());
  EXPECT_FALSE(FindSourceVertex(cycle, 3).first.Valid());
  EXPECT_FALSE(FindSourceVertex(cycle, 4).first.Valid());
  EXPECT_TRUE(FindSourceVertex(cycle, 0).second.empty());
  EXPECT_TRUE(FindSourceVertex(cycle, 1).second.empty());
  EXPECT_TRUE(FindSourceVertex(cycle, 2).second.empty());
  EXPECT_TRUE(FindSourceVertex(cycle, 3).second.empty());
  EXPECT_TRUE(FindSourceVertex(cycle, 4).second.empty());
}

/////////////////////////////////////////////////
TEST(GraphTestFixture, FindSinkVertex)
{
  ///              (6)                  |
  ///           0------>1               |
  ///           |      /|\              |
  ///           |     / | \(5)          |
  ///           | (2)/  |  ┘            |
  ///           |   /   |   2           |
  ///        (1)|  / (2)|  /            |
  ///           | /     | /(5)          |
  ///           VL      VL              |
  ///           3------>4               |
  ///              (1)                  |
  DirectedGraph<int, double> graph(
  {
    // Vertices.
    {{"0", 0, 0}, {"1", 1, 1}, {"2", 2, 2}, {"3", 3, 3}, {"4", 4, 4}},
    // Edges.
    {{{0, 1}, 2.0, 6.0}, {{0, 3}, 3.0, 1.0},
     {{1, 2}, 4.0, 5.0}, {{1, 3}, 4.0, 2.0}, {{1, 4}, 4.0, 2.0},
     {{2, 4}, 2.0, 5.0},
     {{3, 4}, 2.0, 1.0}}
  });

  // invalid input vertex and no edges
  EXPECT_FALSE(FindSinkVertex(graph, 99).first.Valid());
  EXPECT_TRUE(FindSinkVertex(graph, 99).second.empty());

  // vertices 0 and 1 both have multiple incoming edges, so are invalid
  EXPECT_FALSE(FindSinkVertex(graph, 0).first.Valid());
  EXPECT_FALSE(FindSinkVertex(graph, 1).first.Valid());
  EXPECT_TRUE(FindSinkVertex(graph, 0).second.empty());
  EXPECT_TRUE(FindSinkVertex(graph, 1).second.empty());

  // starting from 2, 3, 4 will lead to vertex 4
  auto pair2 = FindSinkVertex(graph, 2);
  EXPECT_EQ(4u, pair2.first.Id());
  EXPECT_EQ(1u, pair2.second.size());
  EXPECT_TRUE(pair2.second[0].Valid());
  EXPECT_DOUBLE_EQ(5.0, pair2.second[0].Weight());

  auto pair3 = FindSinkVertex(graph, 3);
  EXPECT_EQ(4u, pair3.first.Id());
  EXPECT_EQ(1u, pair3.second.size());
  EXPECT_TRUE(pair3.second[0].Valid());
  EXPECT_DOUBLE_EQ(1.0, pair3.second[0].Weight());

  auto pair4 = FindSinkVertex(graph, 4);
  EXPECT_EQ(4u, pair4.first.Id());
  EXPECT_EQ(0u, pair4.second.size());

  // test a graph with a cycle
  DirectedGraph<int, double> cycle(
  {
    // Vertices.
    {{"0", 0, 0}, {"1", 1, 1}, {"2", 2, 2}, {"3", 3, 3}, {"4", 4, 4}},
    // Edges.
    {{{0, 1}, 2.0, 6.0}, {{1, 2}, 3.0, 1.0},
     {{2, 3}, 4.0, 5.0}, {{3, 4}, 4.0, 2.0},
     {{4, 0}, 4.0, 2.0}}
  });
  EXPECT_FALSE(FindSinkVertex(cycle, 0).first.Valid());
  EXPECT_FALSE(FindSinkVertex(cycle, 1).first.Valid());
  EXPECT_FALSE(FindSinkVertex(cycle, 2).first.Valid());
  EXPECT_FALSE(FindSinkVertex(cycle, 3).first.Valid());
  EXPECT_FALSE(FindSinkVertex(cycle, 4).first.Valid());
  EXPECT_TRUE(FindSinkVertex(cycle, 0).second.empty());
  EXPECT_TRUE(FindSinkVertex(cycle, 1).second.empty());
  EXPECT_TRUE(FindSinkVertex(cycle, 2).second.empty());
  EXPECT_TRUE(FindSinkVertex(cycle, 3).second.empty());
  EXPECT_TRUE(FindSinkVertex(cycle, 4).second.empty());
}

////////////////////////////////////////////////
TEST(GraphTestFixture, ToUndirectedGraph)
{
  ///              (6)                  |
  ///           0------>1               |
  ///           |      /|\              |
  ///           |     / | \(5)          |
  ///           | (2)/  |  ┘            |
  ///           |   /   |   2           |
  ///        (1)|  / (2)|  /            |
  ///           | /     | /(5)          |
  ///           VL      VL              |
  ///           3------>4               |
  ///              (1)                  |
  DirectedGraph<int, double> directed(
  {
    // Vertices.
    {{"0", 0, 0}, {"1", 1, 1}, {"2", 2, 2}, {"3", 3, 3}, {"4", 4, 4}},
    // Edges.
    {{{0, 1}, 2.0, 6.0}, {{0, 3}, 3.0, 1.0},
     {{1, 2}, 4.0, 5.0}, {{1, 3}, 4.0, 2.0}, {{1, 4}, 4.0, 2.0},
     {{2, 4}, 2.0, 5.0},
     {{3, 4}, 2.0, 1.0}}
  });

  // Convert to undirected graph.
  auto undirected = ToUndirectedGraph(directed);
  EXPECT_EQ(5u, undirected.Vertices().size());
  EXPECT_EQ(7u, undirected.Edges().size());
  EXPECT_EQ(directed.Vertices().size(), undirected.Vertices().size());
  EXPECT_EQ(directed.Edges().size(), undirected.Edges().size());

  // Compare vertices.
  for (auto const &dvPair : directed.Vertices())
  {
    const VertexId dvId = dvPair.first;
    auto const &uv = undirected.VertexFromId(dvId);
    EXPECT_TRUE(uv.Valid());
    EXPECT_TRUE(dvPair.second.get().Valid());
    EXPECT_EQ(uv.Id(), dvId);
    EXPECT_EQ(uv.Name(), dvPair.second.get().Name());
    EXPECT_EQ(uv.Data(), dvPair.second.get().Data());
  }

  // Compare edges.
  for (auto const &dePair : directed.Edges())
  {
    const EdgeId deId = dePair.first;
    auto const &ue = undirected.EdgeFromId(deId);
    EXPECT_TRUE(ue.Valid());
    EXPECT_TRUE(dePair.second.get().Valid());
    EXPECT_EQ(ue.Id(), deId);
    EXPECT_DOUBLE_EQ(ue.Data(), dePair.second.get().Data());
    EXPECT_DOUBLE_EQ(ue.Weight(), dePair.second.get().Weight());
  }

  // std::cerr << directed << std::endl;
  // std::cerr << undirected << std::endl;
}
