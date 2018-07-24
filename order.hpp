//
//  order.hpp
//
//  Created by Jure Mihelič on 10/23/13.
//

#ifndef ORDER_HPP
#define ORDER_HPP


class Order {
public:
    int n;
    int* order;         // ordering of pattern nodes
    int* orderInv;      // inverse of order
    // neigh
    int** succInNeigh;
    int** succOutNeigh;
    // not neigh
    int** succNotInNeigh;
    int** succNotOutNeigh;
    // parents
    int* parent;
    int* parentKind;  // 0 (root), -1 (in), 1 (out)
    
protected:
    void calcOrder(Digraph& g) {
        int n = g.size();
        order = new int[n];
        orderInv = new int[n];
        parent = new int[n];
        parentKind = new int[n];
        memset(parentKind, 0, sizeof(int)*n);
        //
        int* clustdeg = new int[n];
        for (int i = 0; i < n; i++) clustdeg[i] = clustering1(g, i) + g.degree(i);
        //
        bool* avail = new bool[n];
		memset(avail, true, sizeof(bool) * n);
        //
        int idx = 0;
        while (idx < n) {
            int bestn = NULL_NODE;
            int bestv = -1;
            for (int i = 0; i < n; i++) if (avail[i]) {
                int deg = 0;
                for (int *p = g.neigh(i); *p >= 0; p++) if (!avail[*p]) deg++;
                if (bestn < 0 || (deg > bestv || (deg == bestv && clustdeg[i] > clustdeg[bestn]))) { bestn = i; bestv = deg; }
            }
            avail[bestn] = false;
            order[idx] = bestn;
            orderInv[bestn] = idx;
            for (int *p = g.inneigh(bestn); *p >= 0; p++) if (avail[*p]) { parentKind[*p] = -1; parent[*p] = bestn; }
            for (int *p = g.outneigh(bestn); *p >= 0; p++) if (avail[*p]) { parentKind[*p] = 1; parent[*p] = bestn; }
            idx++;
        }
        delete[] clustdeg;
        delete[] avail;
    }
    
    void calcSucc(Digraph& g) {
        succInNeigh = new int*[n];
        succOutNeigh = new int*[n];
        succNotInNeigh = new int*[n];
        succNotOutNeigh = new int*[n];
        for (int i = 0; i < n; i++) {
            int ordi = orderInv[i];
            // in
            succInNeigh[i] = new int[g.indegree(i)+1];
            int k = 0;
            for (int *j = g.inneigh(i); *j >= 0; j++)
                if (orderInv[*j] > ordi) succInNeigh[i][k++] = *j;
            succInNeigh[i][k] = NULL_NODE;
            // out
            succOutNeigh[i] = new int[g.outdegree(i)+1];
            k = 0;
            for (int *j = g.outneigh(i); *j >= 0; j++)
                if (orderInv[*j] > ordi) succOutNeigh[i][k++] = *j;
            succOutNeigh[i][k] = NULL_NODE;
            // not in
            succNotInNeigh[i] = new int[n+1];
            k = 0;
            for (int *j = g.notinneigh(i); *j >= 0; j++)
                if (orderInv[*j] > ordi) succNotInNeigh[i][k++] = *j;
            succNotInNeigh[i][k] = NULL_NODE;
            // not out
            succNotOutNeigh[i] = new int[n+1];
            k = 0;
            for (int *j = g.notoutneigh(i); *j >= 0; j++)
                if (orderInv[*j] > ordi) succNotOutNeigh[i][k++] = *j;
            succNotOutNeigh[i][k] = NULL_NODE;
        }
    }
    
public:
    Order(Digraph& g): n(g.size()) {
        calcOrder(g);
        calcSucc(g);
    }
    
    ~Order() {
        delete[] order;
        delete[] orderInv;
        delete[] parent;
        for (int i = 0; i < n; i++) {
            delete[] succInNeigh[i];
            delete[] succOutNeigh[i];
        }
        delete[] succInNeigh;
        delete[] succOutNeigh;
    }
};

#endif
