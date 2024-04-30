/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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
 */

#ifndef JS_CONCURRENT_MODULE_UTILS_LOCKS_GRAPH_H
#define JS_CONCURRENT_MODULE_UTILS_LOCKS_GRAPH_H

#include <vector>
#include <map>
#include <string>
#include <limits>
#include <stack>
#include <functional>

namespace Commonlibrary::Concurrent::LocksModule {

template <class VertexId = size_t, class EdgeData = void>
class Graph {
public:
    using EdgeDataCPtr = const EdgeData *;

private:
    struct Vertex {
        enum class Color { WHITE = 0, GREY = 1, BLACK = 2 };
        Color color = Color::WHITE;
        VertexId data = VertexId {};
    };
    using Vertices = std::vector<Vertex>;
    using VIdx = size_t;
    using VColor = typename Vertex::Color;
    using AdjacencyMatrix = std::vector<std::vector<EdgeDataCPtr>>;

    static constexpr VIdx INVALID_V_IDX = std::numeric_limits<VIdx>::max();

public:
    // from, to, what
    using EdgeDef = std::tuple<VertexId, VertexId, EdgeDataCPtr>;
    using AdjacencyList = std::vector<EdgeDef>;
    // for pretty printing paths
    using VertexPrinter = std::function<std::string(VertexId)>;
    using EdgePrinter = std::function<std::string(EdgeDataCPtr)>;

    struct Path {
        // vertex0 -> edge0 -> vertex1 -> edge1 -> ...
        std::vector<VertexId> vertices;
        std::vector<EdgeDataCPtr> edges;
        bool IsEmpty()
        {
            return vertices.empty();
        }
    };

    static constexpr auto defaultVertexPrinter = [](VertexId vid) { return std::to_string(vid); };
    static constexpr auto DEFAULT_EDGE_PRINTER = []() { return " <- "; };

    explicit Graph(AdjacencyList &&adj)
    {
        std::map<VertexId, size_t> vDataPtrToVidx;
        for (auto &edef : adj) {
            // from
            auto fromVertexPtr = std::get<0>(edef);
            if (vDataPtrToVidx.find(fromVertexPtr) == vDataPtrToVidx.end()) {
                v_.push_back({VColor::WHITE, fromVertexPtr});
                vDataPtrToVidx[fromVertexPtr] = v_.size() - 1;
            }
            // to
            auto toVertexPtr = std::get<1>(edef);
            if (vDataPtrToVidx.find(toVertexPtr) == vDataPtrToVidx.end()) {
                v_.push_back({VColor::WHITE, toVertexPtr});
                vDataPtrToVidx[toVertexPtr] = v_.size() - 1;
            }
        }
        // For now let's use an adjacency matrix as the storage.
        // Will use an adjacency list later on.
        e_.resize(v_.size());
        for (auto &row : e_) {
            row.resize(v_.size());
            for (auto &eid : row) {
                eid = nullptr;
            }
        }
        for (auto &edef : adj) {
            auto fromVertexPtr = std::get<0>(edef);
            auto toVertexPtr = std::get<1>(edef);
            auto edgeDataPtr = std::get<2>(edef);
            e_[vDataPtrToVidx[fromVertexPtr]][vDataPtrToVidx[toVertexPtr]] = edgeDataPtr;
        }
    }

    bool IsValid()
    {
        // NOTE: maybe its worth to add more detailed verification
        size_t n = e_.size();
        if (e_.empty()) {
            return false;
        }
        for (auto &row : e_) {
            if (row.size() != n) {
                return false;
            }
        }
        return true;
    }

    static std::string CycleAsString(Path cycle, std::string prompt = "L: ", std::string terminator = "|",
                                     VertexPrinter vertexPrinter = defaultVertexPrinter,
                                     EdgePrinter edgePrinter = DEFAULT_EDGE_PRINTER)
    {
        if (cycle.vertices.empty() || (cycle.edges.size() != (cycle.vertices.size() - 1))) {
            return "";
        }
        std::stringstream result;
        result << prompt;
        auto edgeIt = cycle.edges.begin();
        for (auto vDataPtr : cycle.vertices) {
            result << vertexPrinter(vDataPtr);
            if (edgeIt != cycle.edges.end()) {
                result << edgePrinter(*edgeIt);
                ++edgeIt;
            }
        }
        result << terminator;
        return result.str();
    }

    Path FindFirstCycle()
    {
        // verify
        if (!IsValid()) {
            return Path {};
        }
        // run dfs
        for (VIdx seedIdx = 0; seedIdx < NumVertices(); ++seedIdx) {
            if (VertexColor(seedIdx) != Graph::VColor::WHITE) {
                continue;
            }
            Path cycle = RunDfsFromVertex(seedIdx);
            if (!cycle.vertices.empty()) {
                return cycle;
            }
        }
        return Path {};
    }

private:
    VColor VertexColor(VIdx vidx)
    {
        return v_[vidx].color;
    }

    bool HasEdge(VIdx fromIdx, VIdx toIdx)
    {
        return e_[fromIdx][toIdx] != nullptr;
    }

    size_t NumVertices()
    {
        return v_.size();
    }

    void Mark(VIdx vidx, VColor color)
    {
        v_[vidx].color = color;
    }

    enum class DfsAction { RESTART = 0, FINISH = 1, PROCEED = 2 };
    struct DfsState {
        VIdx currentIdx;
        VIdx childIdx;
    };
    using DfsStack = std::stack<DfsState>;

    Path DfsBuildCycleInfo(DfsStack &dfsStack, DfsState state)
    {
        Path result;
        VIdx originIdx = state.childIdx;
        result.vertices.push_back(v_[state.childIdx].data);
        result.vertices.push_back(v_[state.currentIdx].data);
        result.edges.push_back(e_[state.currentIdx][state.childIdx]);
        VIdx prevIdx = state.currentIdx;
        while (!dfsStack.empty()) {
            auto s = dfsStack.top();
            dfsStack.pop();
            result.vertices.push_back(v_[s.currentIdx].data);
            result.edges.push_back(e_[s.currentIdx][prevIdx]);
            prevIdx = s.currentIdx;
            if (s.currentIdx == originIdx) {
                break;
            }
        }
        return result;
    }

    DfsAction DfsVisitChildren(DfsStack &dfsStack, DfsState &state)
    {
        for (; state.childIdx < NumVertices(); ++state.childIdx) {
            if (HasEdge(state.currentIdx, state.childIdx)) {
                // the edge v -> child exists
                if (VertexColor(state.childIdx) == Graph::VColor::BLACK) {
                    // the child processing is already completed
                    continue;
                }
                if (VertexColor(state.childIdx) == Graph::VColor::GREY) {
                    // the child is visited: means a cycle has been found
                    return DfsAction::FINISH;
                }
                // the child is "white", i.e. not visited yet: run DFS from it
                VIdx nextChild = state.childIdx + 1 < NumVertices() ? state.childIdx + 1 : Graph::INVALID_V_IDX;
                dfsStack.push(DfsState {state.currentIdx, nextChild});
                state.currentIdx = state.childIdx;
                state.childIdx = 0;
                return DfsAction::RESTART;
            }
        }
        state.childIdx = Graph::INVALID_V_IDX;
        return DfsAction::PROCEED;
    }

    bool DfsPopState(DfsStack &dfsStack, DfsState &state)
    {
        while (!dfsStack.empty()) {
            state = dfsStack.top();
            dfsStack.pop();
            if (state.childIdx != Graph::INVALID_V_IDX) {
                return true;
            }
            Mark(state.currentIdx, Graph::VColor::BLACK);
        }
        return false;
    }

    Path RunDfsFromVertex(VIdx seedVertexIdx)
    {
        DfsStack dfsStack;
        DfsState state {seedVertexIdx, 0};
        bool areUnprocessedVerticesLeft = true;

        while (areUnprocessedVerticesLeft) {
            Mark(state.currentIdx, Graph::VColor::GREY);
            DfsAction a = DfsVisitChildren(dfsStack, state);
            switch (a) {
                case DfsAction::FINISH:
                    return DfsBuildCycleInfo(dfsStack, state);
                case DfsAction::RESTART:
                    continue;
                case DfsAction::PROCEED:
                default:
                    break;
            }
            Mark(state.currentIdx, Graph::VColor::BLACK);
            areUnprocessedVerticesLeft = DfsPopState(dfsStack, state);
        }
        // no cycles found
        return Path {};
    }

    Vertices v_;
    AdjacencyMatrix e_;
};

}  // namespace Commonlibrary::Concurrent::LocksModule

#endif
