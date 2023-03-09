#ifndef EINSUM_H_
#define EINSUM_H_

#include "dense.h"
#include "sparse.h"
#include "grade_sparse.h"

typedef struct dense_tensor_multivector{
    dense **data;
    size_t **shapes;
    size_t *shape_size;
    size_t size; // size of the shape_size
    map m;
    dense_grade_map dgm;
}dense_tensor_multivector;

typedef struct sparse_tensor_multivectors{
    sparse **data;
    size_t **shapes;
    size_t *shape_size;
    size_t size; // size of the shape_size
    map m;
    float precision;
    dense_grade_map dgm;
}sparse_tensor_multivectors;

typedef struct graded_tensor_multivectors{
    blades **data;
    size_t **shapes;
    size_t *shape_size;
    size_t size; // size of the shape size (number of multivector tensors)
    map m;
    grade_map gm;
    float precision;
}graded_tensor_multivectors;

typedef struct labels{
    size_t size;
    char *op_labels;
}labels;

typedef struct symbols{
    size_t *size;
    char **subscripts;
    size_t size_;
}symbols;

typedef struct graded_tensor{
    blades *data;
    size_t *shapes;
    size_t shape_size;
}graded_tensor;

typedef struct tensor_strides{
    size_t **strides; // strides for each symbol for each tensor
    size_t *n_strides;
    size_t n_tensors;
    size_t n_symbols;
}tensor_strides;

labels parse_subscripts(char*,size_t,size_t);
symbols parse_args(char*,size_t);
void free_symbols(symbols);
symbols parse_all(char*,size_t,size_t*,size_t);


int iterator(void**,size_t**,size_t,size_t*,size_t*,size_t);
void einsum_sum_prods(tensor_strides,graded_tensor_multivectors);
void einsum_no_sum_prods(tensor_strides,graded_tensor_multivectors);
void sum_of_products(graded_tensor_multivectors,size_t *,size_t,size_t);
graded_tensor vector_matrix_mult(graded_tensor_multivectors);

#endif // EINSUM_H_
