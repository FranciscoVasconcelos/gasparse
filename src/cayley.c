#include "cayley.h"

// returns true if abs(v) == p
int comp_abs_eq(int v,int p){
    int r = (v < 0) ? -v : v;
    return r == p;
}


void free_map(map m){
    for(size_t i = 0; i < m.size; i++){
        free(m.sign[i]);
        free(m.bitmap[i]);
    }
    free(m.sign);
    free(m.bitmap);
}

// Determines the number of one bits in an integer
// Which is equivalent to determining the grade of a bitset
size_t grade(size_t v){
    size_t c = 0; // c accumulates the total bits set in v
    for (c = 0; v; c++)
        v &= v - 1; // clear the least significant bit set
    return c;
}

void free_grade_map(grade_map m){
    free(m.grade);
    free(m.position);
    free(m.grade_size);
}

// Determines the position of the bitmap on the corresponding grade
grade_map bitmap_grade_map(size_t size){
    grade_map m;
    size_t max_grade = grade(size-1);
    size_t *g_pos = (size_t*)malloc((max_grade + 1)*sizeof(size_t));
    m.grade = (size_t*)malloc(size*sizeof(size_t));
    m.position = (size_t*)malloc(size*sizeof(size_t));
    for(size_t i = 0; i <= max_grade; i++)
        g_pos[i] = 0;

    for(size_t i = 0; i < size; i++){
        m.grade[i] = grade(i);
        m.position[i] = g_pos[m.grade[i]]++; // assign value then increment
    }
    m.size = size;
    m.grade_size = g_pos;
    m.max_grade = max_grade;
    return m;
}

map init_map(size_t n){
    map m;
    char **sign = (char**)malloc(n*sizeof(char*));
    size_t **bitmap = (size_t**)malloc(n*sizeof(size_t*));

    for(size_t i = 0; i < n; i++){
        sign[i] = (char*)malloc(n*sizeof(char));
        bitmap[i] = (size_t*)malloc(n*sizeof(size_t));
    }
    m.bitmap = bitmap;
    m.sign = sign;
    m.size = n;
    return m;
}


// converts the cayley table for the geometric product to the inner product
map inner_cayley_table(map m, grade_map gm){
    map inner_m = init_map(m.size);
    for(size_t i = 0; i < m.size; i++){
        for(size_t j = 0; j < m.size; j++){
            if(comp_abs_eq(gm.grade[i] - gm.grade[j],gm.grade[m.bitmap[i][j]])){
                inner_m.bitmap[i][j] = m.bitmap[i][j];
                inner_m.sign[i][j] = m.sign[i][j];
            }else{
                inner_m.sign[i][j] = 0;
                inner_m.bitmap[i][j] = -1;
            }
        }
    }
    return inner_m;
}


// converts the cayley table for the geometric product to the outer product
map outer_cayley_table(map m, grade_map gm){
    map outer_m = init_map(m.size);
    for(size_t i = 0; i < m.size; i++){
        for(size_t j = 0; j < m.size; j++){
            if(gm.grade[i] + gm.grade[j] == gm.grade[m.bitmap[i][j]]){
                outer_m.bitmap[i][j] = m.bitmap[i][j];
                outer_m.sign[i][j] = m.sign[i][j];
            }else{
                outer_m.sign[i][j] = 0;
                outer_m.bitmap[i][j] = -1;
            }
        }
    }
    return outer_m;
}

map cayley_table(size_t p, size_t q, size_t r){
    map m;
    size_t n = 1 << (p+q+r);
    // allocate memory for the whole table
    m = init_map(n);

    m.sign[0][0] = 1;// initialize algebra of scalars
    for(size_t i = 0; i < p; i++)//loop through the identity-square basis vectors
        sub_algebra(i,m.sign,1);
    for(size_t i = p; i < p+q; i++)//loop through the negative-square basis vectors
        sub_algebra(i,m.sign,-1);
    for(size_t i = p+q; i < p+q+r; i++)//loop through the zero-square basis vectors
        sub_algebra(i,m.sign,0);

    // determine each basis blade and its grade
    for(size_t i = 0; i < n; i++){
        for(size_t j = 0; j < n; j++){
            size_t bitmap_ij = i ^ j;
            m.bitmap[i][j] = bitmap_ij;
        }
    }

    return m;
}

void sub_algebra(size_t k, char **s, int metric){
    size_t m = 1 << k; // same as 2^k
    size_t n = m << 1; // same as 2^(k+1)
    int sign;
    // This could be improved by checking if the element in the array is zero
    for(size_t i = m; i < n; i++){// loop through the new elements
        for(size_t j = 0; j < m; j++){// loop through old elements
            // j is indepedent of the new basis vector
            sign = ((grade(j) & 1) == 0) ? 1 : -1;// return minus one if grade is odd
            s[i][j] = sign*s[i-m][j];
            s[j][i] = s[j][i-m];
        }
        if(metric != 0 ){
            for(size_t j = m; j < n; j++){// loop through new elements
                // These elements have the new basis vector in common
                sign = metric;
                // remove the new basis vector then determine sign
                sign *= ((grade(j-m) & 1) == 0) ? 1 : -1;
                sign *= s[i-m][j-m];// remove the new vector part
                s[i][j] = sign;
            }
        }else{//if null metric -> set all elements to zero
            for(size_t j = m; j < n; j++)
                s[i][j] = 0;
        }
    }
}

size_t* get_grade_bool(size_t *grades, size_t size, size_t max_grade){
    size_t *g = (size_t*)malloc(max_grade*sizeof(size_t));
    if(size == 0){ // if size is 0 project to all grades
        for(size_t i = 0; i < max_grade; i++)
            g[i] = 1;
    }else{
        for(size_t i = 0; i < max_grade; i++)
            g[i] = 0;
        for(size_t i = 0; i < size; i++)
            g[grades[i]] = 1;
    }
    return g;
}


map invert_map(map m){
    map m_inv = init_map(m.size);
    for(size_t i = 0; i < m.size; i++){
        for(size_t j = 0; j < m.size; j++){
            m_inv.bitmap[i][m.bitmap[i][j]] = j;
            m_inv.sign[i][m.bitmap[i][j]] = m.sign[i][j];
        }
    }
    return m_inv;

}
