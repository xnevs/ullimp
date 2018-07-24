//
//  map.hpp
//
//  Created by Jure Mihelič on 10/23/13.
//

#ifndef MAP_HPP
#define MAP_HPP


class Map {
protected:
	int rows;                   // number of rows
	int cols;                   // number of columns
	int size;                   // size = rows * cols
	bool* data;                 // current matrix
    
    // history - stack of all changes
	int* history;               // stack of indices of changes
	int index;                  // top of stack
	int lastindex;              // last index
    // shots - snapshosts in history
	int* shots;                 // stack of snapshots
	int shotidx;                // top of stack
    
public:
	Map(int rows, int cols) {
		this->rows = rows;
		this->cols = cols;
		size = rows * cols;
		data = new bool[size];
		//memset(data, 0, sizeof(bool) * size);
        //
		history = new int[size];
		//memset(history, 0, sizeof(int) * size);
		index = -1;
        lastindex = -1;
		shots = new int[size];
        //memset(shots, 0, sizeof(int) * size);
		shotidx = -1;
	}
    
    ~Map() {
		delete[] data;
		delete[] history;
		delete[] shots;
	}
    
    inline bool& operator()(int row, int col) {
	    return data[row * cols + col];
	}
    
	inline const bool operator()(int row, int col) const {
	    return data[row * cols + col];
	}
    
	inline void snapshot() {
		shots[++shotidx] = index;
        reset();
	}
    
	inline void undo() {
		int stop = shots[shotidx--];
		while (index > stop)
			data[history[index--]] = true;
	}
    
	inline void annul(int row, int col) {
		register int idx = row * cols + col;
		if (data[idx]) {
			data[idx] = false;
			history[++index] = idx;
		}
	}
    
	inline void reset() {
		lastindex = index;
	}
    
	inline int changes() {
		return abs(index - lastindex);
	}
};


inline int clustering1(Digraph& g, int node) {
    int c = 0;
    int* nn = g.neigh(node);
    for (int *p = nn; *p >= 0; p++)
        for (int *q = g.neigh(*p); *q >= 0; q++)
            if (g.edge(*q, node) || g.edge(node, *q)) c++;
    return c;
}

#endif
