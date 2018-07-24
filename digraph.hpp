//
//  digraph.hpp
//
//  Created by Jure Mihelič on 10/23/13.
//

#ifndef DIGRAPH_HPP
#define DIGRAPH_HPP


#define NULL_NODE   -1


class Digraph {
protected:
	int nodeCnt;                // number of nodes
    int edgeCnt;                // number of edges
	bool* adjacency;            // adjacency matrix
	int *indegs;                // in degrees
	int *outdegs;               // out degrees
	int **inneighs;             // in neighbors list, terminated with NULL_NODE
	int **outneighs;            // out neighbors, ...
	int **neighs;               // all neighbors, ...
	int **notinneighs;          // not in neighbors, ...
	int **notoutneighs;         // not out neighbors, ...
    
	void prepare(int size) {
		nodeCnt = size;
        edgeCnt = 0;
		adjacency = new bool[size * size];
		memset(adjacency, 0, sizeof(bool) * size * size);
		indegs = new int[size];
		memset(indegs, 0, sizeof(int) * size);
		outdegs = new int[size];
		memset(outdegs, 0, sizeof(int) * size);
	}
    
    void finish() {
    	inneighs = new int* [nodeCnt];
    	outneighs = new int* [nodeCnt];
        neighs = new int* [nodeCnt];
    	notinneighs = new int* [nodeCnt];
    	notoutneighs = new int* [nodeCnt];
    	for (int i = 0; i < nodeCnt; i++) {
            int l = indegs[i];  inneighs[i] = new int[l + 1];  inneighs[i][l] = NULL_NODE;
            l = nodeCnt - l - 1;  notinneighs[i] = new int[l + 1];  notinneighs[i][l] = NULL_NODE;
            l = outdegs[i];  outneighs[i] = new int[l + 1];  outneighs[i][l] = NULL_NODE;
            l = nodeCnt - l - 1;  notoutneighs[i] = new int[l + 1];  notoutneighs[i][l] = NULL_NODE;
            l = indegs[i] + outdegs[i];  neighs[i] = new int[l + 1];  neighs[i][l] = NULL_NODE;
            int k1 = 0, k2 = 0, k3 = 0, k4 = 0, k5 = 0;
            for (int j = 0; j < nodeCnt; j++) if (j != i) {
                if (edge(j, i)) { inneighs[i][k1++] = j; neighs[i][k5++] = j; }
                else notinneighs[i][k3++] = j;
                if (edge(i, j)) { outneighs[i][k2++] = j; neighs[i][k5++] = j; }
                else notoutneighs[i][k4++] = j;
            }
    	}
    }
    
    inline unsigned int read1(std::ifstream& in) {
    	unsigned char a, b;
    	a = (char)(in.get());
    	b = (char)(in.get());
    	return a | (b << 8);
    }
    
public:
	Digraph(const char* filename): nodeCnt(0), edgeCnt(0), adjacency(), indegs(), outdegs(), inneighs(), outneighs() {
	    readGraphBinary(filename);
	}
    
	~Digraph() {
		delete[] adjacency;
		delete[] indegs;
		delete[] outdegs;
		for (int i = 0; i < nodeCnt; i++) {
			delete inneighs[i];
			delete outneighs[i];
            delete neighs[i];
			delete notinneighs[i];
			delete notoutneighs[i];
		}
		delete[] inneighs;
		delete[] outneighs;
        delete[] neighs;
		delete[] notinneighs;
		delete[] notoutneighs;
	}
    
    void setEdge(int i, int j) {
        if (edge(i, j)) return;
        edgeCnt++;
        adjacency[i * nodeCnt + j] = true;
        indegs[j]++;
        outdegs[i]++;
    }
    
    void readGraphBinary(const char* filename) {
        std::ifstream in (filename, std::ios::in|std::ios::binary);
        int n = read1(in);
        prepare(n);
        for (int i = 0; i < n; i++) {
        	int cnt = read1(in);
            for (int j = 0; j < cnt; j++) {
                int k = read1(in);
                setEdge(i, k);
            }
        }
        finish();
    }
    
    inline int size() {
    	return nodeCnt;
    }
    
    inline bool node(int i) {
    	return i < size();
    }
    
    inline int degree(int i) {
    	return indegs[i] + outdegs[i];
    }
    
    inline int indegree(int i) {
    	return indegs[i];
    }
    
    inline int outdegree(int i) {
    	return outdegs[i];
    }
    
    inline int* inneigh(int i) {
    	return inneighs[i];
    }
    
    inline int* outneigh(int i) {
    	return outneighs[i];
    }
    
    inline int* neigh(int i) {
    	return neighs[i];
    }
    
    inline int* notinneigh(int i) {
    	return notinneighs[i];
    }
    
    inline int* notoutneigh(int i) {
    	return notoutneighs[i];
    }
    
    inline bool edge(int i, int j) {
    	return adjacency[i * nodeCnt + j];
    }
};

#endif
