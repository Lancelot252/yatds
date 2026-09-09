CXX      = g++
CXXFLAGS = -std=c++17 -Wall

BASELINE = ../Baseline/src

SRCS = project.cpp \
       $(BASELINE)/adjacency_list/graph.cpp \
       $(BASELINE)/adjacency_matrix/graph.cpp \
       $(BASELINE)/unweighted_shortest_path/shortest_path.cpp \
       $(BASELINE)/dijkstra/dijkstra.cpp \
       $(BASELINE)/mst_prim/mst_prim.cpp \
       $(BASELINE)/mst_kruskal/mst_kruskal.cpp

TARGET = graph_project

all: $(TARGET)

$(TARGET): $(SRCS)
	$(CXX) $(CXXFLAGS) $(SRCS) -o $(TARGET)

clean:
	rm -f $(TARGET)

.PHONY: all clean
