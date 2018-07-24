//
//  ullimp.hpp
//
//  Created by Jure Mihelič on 10/23/13.
//


#ifndef ULLIMP_HPP
#define ULLIMP_HPP

class UllImp {
protected:
	Digraph &g;         // pattern graph
	Digraph &h;         // target graph
    int n, m;           // n=size of pattern, m=size of target
public:
	int isoCount;       // no. of isomorphisms found
protected:
    int* iso;           // isomorphism: iso[i] -> j
    int* isoinv;        // isomorphism inverse
    int* colidx;
    int* allcols;
    Map* map;           // possible mappings (matrix with snapshots)
    Order* order;       // ordering of pattern nodes
    
    void init() {
        iso = new int[n];
        for (int i = 0; i < n; i++) iso[i] = NULL_NODE;
        isoinv = new int[m];
        for (int i = 0; i < m; i++) isoinv[i] = NULL_NODE;
        colidx = new int[n];
        for (int i = 0; i < n; i++) colidx[i] = -1;
        allcols = new int[m+1];
        for (int i = 0; i < m; i++) allcols[i] = i;
        allcols[m] = -1;
        order = new Order(g);
        map = new Map(n, m);
        for (int i = 0; i < n; i++) {
            int a = g.indegree(i);
            int b = g.outdegree(i);
            for (int j = 0; j < m; j++)
                (*map)(i, j) = h.indegree(j) >= a && h.outdegree(j) >= b;
        }
        refineMapFull(0);
    }
    
    void done() {
        delete[] iso;
        delete[] isoinv;
        delete map;
        delete order;
    }
    
    bool canMapFull(int row, int col) {
        for (int *i = g.outneigh(row); *i >= 0; i++) {
            bool all0 = true;
            for (int *j = h.outneigh(col); *j >= 0; j++)
                if ((*map)(*i, *j)) { all0 = false; break; }
            if (all0) return false;
        }
        for (int *i = g.inneigh(row); *i >= 0; i++) {
            bool all0 = true;
            for (int *j = h.inneigh(col); *j >= 0; j++)
                if ((*map)(*i, *j)) { all0 = false; break; }
            if (all0) return false;
        }
        return true;
    }
    
    void refineMapFull(int depth) {
        bool changed = false;
        do {
            changed = false;
            for (int i = 0; i < n; i++) for (int j = 0; j < m; j++)
                if ((*map)(i, j) && !canMapFull(i, j)) { (*map)(i, j) = 0; changed = true; }
        } while (changed);
    }
    
    bool canMap(int row, int col) {
        for (int *i = order->succInNeigh[row]; *i >= 0; i++) {
            bool all0 = true;
            for (int *j = h.inneigh(col); *j >= 0; j++)
                if (isoinv[*j] < 0 && (*map)(*i, *j)) { all0 = false; break; }
            if (all0) return false;
        }
        for (int *i = order->succOutNeigh[row]; *i >= 0; i++) {
            bool all0 = true;
            for (int *j = h.outneigh(col); *j >= 0; j++)
                if (isoinv[*j] < 0 && (*map)(*i, *j)) { all0 = false; break; }
            if (all0) return false;
        }
        return true;
    }
    
    void refineMapPartial(int level, int row, int col) {
        for (int *i = order->succInNeigh[row]; *i >= 0; i++)
            for (int *j = h.inneigh(col); *j >= 0; j++)
                if (isoinv[*j] < 0 && (*map)(*i, *j) && !canMap(*i, *j)) map->annul(*i, *j);
        for (int *i = order->succOutNeigh[row]; *i >= 0; i++)
            for (int *j = h.outneigh(col); *j >= 0; j++)
                if (isoinv[*j] < 0 && (*map)(*i, *j) && !canMap(*i, *j)) map->annul(*i, *j);
    }
    
    void filter(int depth, int row, int col) {
        // vrstica & stolpec
        //for (int j = 0; j < m; j++) map->annul(row, j);  // ne rabiš, ker se preverja iso[i]
        //for (int i = 0; i < n; i++) map->annul(i, col);
        
        for (int *i = order->succInNeigh[row]; *i >= 0; i++)
            for (int *j = h.notinneigh(col); *j >=0; j++) if (isoinv[*j] < 0) map->annul(*i, *j);
        for (int *i = order->succOutNeigh[row]; *i >= 0; i++)
            for (int *j = h.notoutneigh(col); *j >=0; j++) if (isoinv[*j] < 0) map->annul(*i, *j);
        
        for (int *j = h.inneigh(col); *j >=0; j++) if (isoinv[*j] < 0)
            for (int *i = order->succNotInNeigh[row]; *i >= 0; i++) map->annul(*i, *j);
        for (int *j = h.outneigh(col); *j >=0; j++) if (isoinv[*j] < 0)
            for (int *i = order->succNotOutNeigh[row]; *i >= 0; i++) map->annul(*i, *j);
    }
    
    inline int nextCol(int row) {
        if (iso[row] >= 0) isoinv[iso[row]] = NULL_NODE;			// reset previous visit
        // find next available column
        int col = iso[row] + 1;
        while (col < m && isoinv[col] < 0 && !(*map)(row, col)) col += 1;
        //        while (col < m && (!(*map)(row, col) || !checkNode(row, col) || !checkEdges(row, col))) col += 1;
        return col;
    }
    
    inline int nextCol1(int row) {
        if (iso[row] >= 0) isoinv[iso[row]] = NULL_NODE;			// reset previous visit
        // find next available column
        int pk = order->parentKind[row];
        int* nn = pk == 0 ? allcols : (pk == -1 ? h.inneigh(iso[order->parent[row]]) : h.outneigh(iso[order->parent[row]]));
        register int col;
        while ((col = nn[++colidx[row]]) != -1 && (isoinv[col] >= 0 || !(*map)(row, col)));
        return col;
    }
    
    void run(bool enumerate) {
    	int depth = 0;
        while (true) {
        	while (depth < n - 1) {
                int row = order->order[depth];
                int col = nextCol1(row);
                if (col != -1) {
                    iso[row] = col;									// map node -> col
                    isoinv[col] = row;								// mark column as used
                    map->snapshot();
                    filter(depth, row, col);
                    if (depth <= n/2) refineMapPartial(depth, row, col);
                    if (canMap(row, col)) depth++;
                    else map->undo();
	            } else {
                    if (row != -1) { iso[row] = -1;  colidx[row] = -1; }			// backtrack iso
	                if (--depth < 0) return;	// all paths tried?
                    map->undo();
	            }
        	}
        	// all rows mapped (depth == g.size)
            int row = order->order[depth];
            for (int j = 0; j < m; j++) if (isoinv[j] < 0 && (*map)(row, j)) isoCount++;
            map->undo();
        	depth -= 1;
    	    if (!enumerate) return;
        }
	}
    
    bool checkNode(int row, int col) {
        return isoinv[col] < 0 && h.outdegree(col) >= g.outdegree(row) && h.indegree(col) >= g.indegree(row);
    }
    
    bool checkEdges(int row, int col) {
        for (int *i = g.outneigh(row); *i >= 0; i++) if (iso[*i] >= 0)
            if (!h.edge(col, iso[*i])) return false;
        for (int *i = g.inneigh(row); *i >= 0; i++) if (iso[*i] >= 0)
            if (!h.edge(iso[*i], col)) return false;
        
        for (int *j = h.outneigh(col); *j >= 0; j++) if (isoinv[*j] >= 0)
            if (!g.edge(row, isoinv[*j])) return false;
        for (int *j = h.inneigh(col); *j >= 0; j++) if (isoinv[*j] >= 0)
            if (!g.edge(isoinv[*j], row)) return false;
        return true;
    }
    
public:
	UllImp(Digraph &g, Digraph &h): g(g), h(h), n(g.size()), m(h.size()), isoCount(0) { }
    
    void find(bool enumerate) {
		init();
        run(enumerate);
        done();
    }
};

#endif
