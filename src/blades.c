#include "blades.h"


// projects multivector to each specified grade
blades grade_sparse_grade_project(blades mv,
    size_t *project_grade,
    size_t project_size,
    size_t grade_size){

    // computes a bool array which each index indicates the selected grade
    size_t *g = get_grade_bool(project_grade,project_size,grade_size);
    blades projected_mv;

    size_t projected_size = 0;

    for(size_t i = 0; i < mv.size; i++)
        if(g[mv.grade[i]])
            projected_size++;

    projected_mv = initialize_blades_empty(projected_size--);

    for(size_t i = 0; i < mv.size; i++){
        if(g[mv.grade[i]]){
            projected_mv.data[projected_size] = sparse_copy(mv.data[i]);
            projected_mv.grade[projected_size--] = mv.grade[i];
        }
    }
    free(g);
    return projected_mv;
}

blades graded_general_product(graded_multivectors mvs, project_map pm){
    blades a = mvs.a;
    blades b = mvs.b;
    map m = mvs.m;
    grade_map gm = mvs.gm;
    float precision = mvs.precision;
    return blades_general_product_(a,b,m,gm,pm,precision);
}

blades blades_general_product_(blades a, blades b, map m, grade_map gm, project_map pm, float precision){

    size_t *ga = get_grade_bool(pm.left,pm.left_size,gm.max_grade+1);
    size_t *gb = get_grade_bool(pm.right,pm.right_size,gm.max_grade+1);
    size_t *gy = get_grade_bool(pm.out,pm.out_size,gm.max_grade+1);

    blades dense_y = initialize_blades_empty(gm.max_grade + 1);
    blades sparse_y;
    size_t n_grades = 0;
    size_t *grade_size = initialize_grade_size(gm);

    // iterate over grades
    for(size_t i = 0; i < a.size; i++){
        if(!ga[a.grade[i]]) continue; // skip grade
        for(size_t j = 0; j < b.size; j++){
            if(!gb[b.grade[j]]) continue; // skip grade
            sparse mv_a = a.data[i];
            sparse mv_b = b.data[j];
            // iterate over basis vectors
            for(size_t k = 0; k < a.data[i].size; k++){
                for(size_t l = 0; l < b.data[j].size; l++){

                    int sign = m.sign[mv_a.bitmap[k]][mv_b.bitmap[l]];
                    if(sign == 0) continue;
                    size_t bitmap = m.bitmap[mv_a.bitmap[k]][mv_b.bitmap[l]];
                    size_t grade = gm.grade[bitmap];
                    if(!gy[grade]) continue; // skip grade
                    size_t position = gm.position[bitmap];
                    float value = mv_a.value[k]*mv_b.value[l];

                    if(dense_y.data[grade].bitmap == NULL){
                        dense_y.data[grade] = initialize_sparse(gm.grade_size[grade]);
                        n_grades++;
                    }

                    // write bitmap once to memory
                    if(dense_y.data[grade].bitmap[position] == -1){
                        dense_y.data[grade].bitmap[position] = bitmap;
                        dense_y.data[grade].value[position] = 0;
                        grade_size[grade]++;
                    }
                    // compute the geometric product
                    dense_y.data[grade].value[position] += sign*value;
                }
            }
        }
    }

    graded_remove_small(dense_y,precision,grade_size);
    sparse_y = grade_dense_to_grade_sparse(dense_y,grade_size);
    free(grade_size);
    blades_free_(dense_y);
    free(ga);
    free(gb);
    free(gy);

    return sparse_y;
}


// takes a sparse representation of a multivector and converts it to grade sparse
blades sparse_to_graded(sparse mv, grade_map gm){
    blades sparse_mv;
    blades dense_mv;
    size_t *grade_size = initialize_grade_size(gm);
    size_t n_grades = 0;
    size_t max_grade = 0;
    // determine how many components for each grade
    for(size_t i = 0; i < mv.size; i++){
        size_t grade = gm.grade[mv.bitmap[i]];
        grade_size[grade]++;
        if(grade > max_grade)
            max_grade = grade;
    }
    // counts the number of different grades
    for(size_t i = 0; i <= max_grade; i++)
        if(grade_size[i] > 0)
            n_grades++;

    dense_mv = initialize_blades_empty(max_grade+1);
    sparse_mv = initialize_blades_empty(n_grades);

    // convert sparse to grade dense
    for(size_t i = 0; i < mv.size; i++){
        size_t grade = gm.grade[mv.bitmap[i]];
        if(dense_mv.data[grade].bitmap == NULL)
            dense_mv.data[grade] = initialize_sparse(grade_size[grade]);

        grade_size[grade]--;
        dense_mv.data[grade].bitmap[grade_size[grade]] = mv.bitmap[i];
        dense_mv.data[grade].value[grade_size[grade]] = mv.value[i];
    }

    n_grades = 0;
    for(size_t i = 0; i <= max_grade; i++){
        if(dense_mv.data[i].bitmap != NULL){
            sparse_mv.data[n_grades] = dense_mv.data[i];
            sparse_mv.grade[n_grades] = dense_mv.grade[i];
            n_grades++;
        }
    }
    free(dense_mv.data);
    free(dense_mv.grade);
    free(grade_size);
    return sparse_mv;
}


blades graded_product(graded_multivectors mvs){

    blades a = mvs.a;
    blades b = mvs.b;
    map m = mvs.m;
    grade_map gm = mvs.gm;
    float precision = mvs.precision;
    return blades_product_(a,b,m,gm,precision);
}

blades blades_product_(blades a, blades b, map m, grade_map gm, float precision){

    blades dense_y = initialize_blades_empty(gm.max_grade + 1);
    blades sparse_y;
    size_t n_grades = 0;
    size_t *grade_size = initialize_grade_size(gm);

    // iterate over grades
    for(size_t i = 0; i < a.size; i++){
        for(size_t j = 0; j < b.size; j++){
            sparse mv_a = a.data[i];
            sparse mv_b = b.data[j];
            // iterate over basis vectors
            for(size_t k = 0; k < a.data[i].size; k++){
                for(size_t l = 0; l < b.data[j].size; l++){
                    int sign = m.sign[mv_a.bitmap[k]][mv_b.bitmap[l]];
                    if(sign == 0) continue;
                    size_t bitmap = m.bitmap[mv_a.bitmap[k]][mv_b.bitmap[l]];
                    size_t grade = gm.grade[bitmap];
                    size_t position = gm.position[bitmap];
                    float value = mv_a.value[k]*mv_b.value[l];

                    if(dense_y.data[grade].bitmap == NULL){
                        dense_y.data[grade] = initialize_sparse(gm.grade_size[grade]);
                        n_grades++;
                    }

                    // write bitmap once to memory
                    if(dense_y.data[grade].bitmap[position] == -1){
                        dense_y.data[grade].bitmap[position] = bitmap;
                        dense_y.data[grade].value[position] = 0;
                        grade_size[grade]++;
                    }
                    // compute the geometric product
                    dense_y.data[grade].value[position] += sign*value;
                }
            }
        }
    }

    graded_remove_small(dense_y,precision,grade_size);
    sparse_y =  grade_dense_to_grade_sparse(dense_y,grade_size);
    free(grade_size);
    blades_free_(dense_y);

    return sparse_y;
}

// multiply all the elements of a multivector by a scalar
blades graded_scalar_multiply(float scalar, blades b){
    size_t *grade_size = (size_t*)malloc(b.size*sizeof(size_t));
    for(size_t i = 0; i < b.size; i++)
        grade_size[i] = b.data[i].size;

    blades y = initialize_blades(grade_size,b.size);
    for(size_t i = 0; i < b.size; i++){
        for(size_t j = 0; j < b.data[i].size; j++){
            y.data[i].value[j] = b.data[i].value[j]*scalar;
            y.data[i].bitmap[j] = b.data[i].bitmap[j];
        }
        y.grade[i] = b.grade[i];
    }
    free(grade_size);
    return y;
}

blades graded_add_append(blades a, blades b){
    blades y = initialize_blades_empty(a.size + b.size);

    for(size_t i = 0; i < a.size; i++){
        y.data[i] = sparse_copy(a.data[i]);
        y.grade[i] = a.grade[i];
    }
    for(size_t i = 0; i < b.size; i++){
        y.data[i+a.size] = sparse_copy(b.data[i]);
        y.grade[i+a.size] = b.grade[i];
    }
    return y;
}

blades graded_atomic_add_append(blades **mv, size_t size){
    size_t y_size = 0;
    for(size_t i = 0; i < size; i++)
        y_size += mv[i]->size;

    blades y = initialize_blades_empty(y_size);

    for(size_t i = 0; i < mv[0]->size; i++){
        y.data[i] = sparse_copy(mv[0]->data[i]);
        y.grade[i] = mv[0]->grade[i];
    }

    size_t p = 0;
    for(size_t j = 1; j < size; j++){
        p += mv[j-1]->size;
        for(size_t i = 0; i < mv[j]->size; i++){
            y.data[i+p] = sparse_copy(mv[j]->data[i]);
            y.grade[i+p] = mv[j]->grade[i];
        }
    }
    return y;
}

blades graded_add_add(graded_multivectors mvs){
    blades a = mvs.a;
    blades b = mvs.b;
    grade_map gm = mvs.gm;
    float precision = mvs.precision;
    return blades_add_add_(a,b,gm,precision);
}

blades blades_add_add_(blades a, blades b, grade_map gm, float precision){
    size_t *grade_size = initialize_grade_size(gm);
    blades dense_y = initialize_blades_empty(gm.max_grade + 1);
    blades sparse_y;
    size_t n_grades = 0;

    for(size_t i = 0; i < a.size; i++){
        for(size_t j = 0; j < a.data[i].size; j++){
            size_t position = gm.position[a.data[i].bitmap[j]];
            size_t grade = a.grade[i];
            if(dense_y.data[grade].bitmap == NULL){
                dense_y.data[grade] = initialize_sparse(gm.grade_size[grade]);
                n_grades++;
            }
            // write bitmap once to memory
            if(dense_y.data[grade].bitmap[position] == -1){
                dense_y.data[grade].bitmap[position] = a.data[i].bitmap[j];
                dense_y.data[grade].value[position] = 0;
                grade_size[grade]++;
            }
            dense_y.data[grade].value[position] += a.data[i].value[j];
        }
    }

    for(size_t i = 0; i < b.size; i++){
        for(size_t j = 0; j < b.data[i].size; j++){
            size_t position = gm.position[b.data[i].bitmap[j]];
            size_t grade = b.grade[i];
            if(dense_y.data[grade].bitmap == NULL){
                dense_y.data[grade] = initialize_sparse(gm.grade_size[grade]);
                n_grades++;
            }
            // write bitmap once to memory
            if(dense_y.data[grade].bitmap[position] == -1){
                dense_y.data[grade].bitmap[position] = b.data[i].bitmap[j];
                dense_y.data[grade].value[position] = 0;
                grade_size[grade]++;
            }
            dense_y.data[grade].value[position] += b.data[i].value[j];
        }
    }


    graded_remove_small(dense_y,precision,grade_size);
    sparse_y = grade_dense_to_grade_sparse(dense_y,grade_size);

    free(grade_size);
    blades_free_(dense_y);
    return sparse_y;
}

blades blades_atomic_add_add_(blades **mv, size_t size, grade_map gm, float precision){
    size_t *grade_size = initialize_grade_size(gm);
    blades dense_y = initialize_blades_empty(gm.max_grade + 1);
    blades sparse_y;
    size_t n_grades = 0;

    for(size_t k = 0; k < size; k++){
        for(size_t i = 0; i < mv[k]->size; i++){
            for(size_t j = 0; j < mv[k]->data[i].size; j++){
                size_t position = gm.position[mv[k]->data[i].bitmap[j]];
                size_t grade = mv[k]->grade[i];
                if(dense_y.data[grade].bitmap == NULL){
                    dense_y.data[grade] = initialize_sparse(gm.grade_size[grade]);
                    n_grades++;
                }
                // write bitmap once to memory
                if(dense_y.data[grade].bitmap[position] == -1){
                    dense_y.data[grade].bitmap[position] = mv[k]->data[i].bitmap[j];
                    dense_y.data[grade].value[position] = 0;
                    grade_size[grade]++;
                }
                dense_y.data[grade].value[position] += mv[k]->data[i].value[j];
            }
        }
    }

    graded_remove_small(dense_y,precision,grade_size);
    sparse_y = grade_dense_to_grade_sparse(dense_y,grade_size);

    free(grade_size);
    blades_free_(dense_y);
    return sparse_y;
}


// stores only the non-empyty grades and the non-empty blades
blades grade_dense_to_grade_sparse(blades dense_y, size_t *grade_size){
    size_t n_grades = 0;
    blades sparse_y;

    sparse_y = initialize_blades(grade_size,dense_y.size); // It also only allocates memory for non-empty grades
    for(size_t i = 0; i < dense_y.size; i++){
        if(dense_y.data[i].bitmap != NULL){
            sparse_y.grade[n_grades] = dense_y.grade[i];
            for(size_t j = 0; j < dense_y.data[i].size; j++){
                if(dense_y.data[i].bitmap[j] != -1){
                    grade_size[i]--;
                    int *bitmap = sparse_y.data[n_grades].bitmap;
                    float *value = sparse_y.data[n_grades].value;
                    bitmap[grade_size[i]] = dense_y.data[i].bitmap[j];
                    value[grade_size[i]] = dense_y.data[i].value[j];
                }
            }
            n_grades++;
        }
    }
    return sparse_y;
}


void graded_remove_small(blades y, float precision, size_t* grade_size){
    // Remove if value is too small
    for(size_t i = 0; i < y.size; i++){
        // Check if grade was set
        if(y.data[i].bitmap != NULL){
            for(size_t j = 0; j < y.data[i].size; j++){
                // Check if value was set
                if(y.data[i].bitmap[j] != -1){
                    // compare value with precision
                    if(comp_abs(y.data[i].value[j],precision)){
                        y.data[i].bitmap[j] = -1;
                        grade_size[i]--;

                        // remove grade if all elements have small value
                        if(grade_size[i] <= 0){
                            free(y.data[i].bitmap);
                            free(y.data[i].value);
                            y.data[i].bitmap = NULL;
                            y.data[i].value = NULL;
                            break;
                        }
                    }
                }
            }
        }
    }
}


size_t *initialize_grade_size(grade_map gm){
    size_t *grade_size = (size_t *)malloc((gm.max_grade+1)*sizeof(size_t));
    for(size_t i = 0; i <= gm.max_grade; i++)
        grade_size[i] = 0;
    return grade_size;
}

blades initialize_blades_empty(size_t n_grades){// allocate the necessary memory for each grade
    blades y;
    y.data = (sparse*)malloc(n_grades*sizeof(sparse));
    y.grade = (size_t*)malloc(n_grades*sizeof(size_t));
    y.size = n_grades;
    for(size_t i = 0; i < n_grades; i++){
        y.data[i].bitmap = NULL;
        y.data[i].value = NULL;
        y.grade[i] = i;
    }
    return y;
}


blades initialize_blades(size_t *grade_size, size_t n_grades){// allocate the necessary memory for each grade
    blades y;
    size_t m_grades = 0;
    // determine the total number of nonempty grades
    for(size_t i = 0; i < n_grades; i++)
        if(grade_size[i] > 0)
            m_grades++;

    y.data = (sparse*)malloc(m_grades*sizeof(sparse));
    y.grade = (size_t*)malloc(m_grades*sizeof(size_t));
    y.size = m_grades;
    size_t j = 0;
    for(size_t i = 0; i < n_grades; i++){
        if(grade_size[i] > 0 ){
            y.data[j] = initialize_sparse(grade_size[i]);
            y.grade[j] = -1;
            j++;
        }
    }
    return y;
}


void blades_free_(blades y){
    for(size_t i = 0; i < y.size; i++){
        if(y.data[i].bitmap != NULL)
            free(y.data[i].bitmap);

        if(y.data[i].value != NULL)
            free(y.data[i].value);
    }
    free(y.data);
    free(y.grade);
}

void blades_assign__(void *data, void *temp){
    blades *blades_data = data;
    blades *blades_temp = temp;
    *blades_data = *blades_temp;
}

void blades_init__(void *data, size_t size){
    blades *blades_data = data;
    for(size_t i = 0; i < size; i++){
        blades_data[i].grade = NULL;
        blades_data[i].data = NULL;
        blades_data[i].size = 0;
    }
}

void *blades_product__(void *a, void *b, void *extra){
    blades_extra *gextra = extra;
    blades *ga = a;
    blades *gb = b;
    blades *out = (blades*)malloc(sizeof(blades));
    *out = blades_product_(*ga,*gb,*gextra->m,gextra->gm,gextra->precision);
    return (void*)out;
}

void *blades_operator_product__(void *a, void *b, void *extra, size_t operator){
    blades_extra *gextra = extra;
    blades *ga = a;
    blades *gb = b;
    blades *out = (blades*)malloc(sizeof(blades));
    *out = blades_product_(*ga,*gb,gextra->m[operator],gextra->gm,gextra->precision);
    return (void*)out;
}



void *blades_add_add__(void *a, void *b, void *extra){
    blades_extra *gextra = extra;
    blades *ga = a;
    blades *gb = b;
    blades *out = (blades*)malloc(sizeof(blades));
    *out = blades_add_add_(*ga,*gb,gextra->gm,gextra->precision);
    return (void*)out;
}

void *blades_atomic_add__(void *data, size_t size, void *extra){
    blades_extra *gextra = extra;
    blades **gdata = data;
    blades *out = (blades*)malloc(sizeof(blades));
    *out = blades_atomic_add_add_(gdata,size,gextra->gm,gextra->precision);
    return (void*)out;
}

void blades_free__(void *data, size_t size){
    blades *gdata = data;
    for(size_t i = 0; i < size; i++){
        blades_free_(gdata[i]);
    }
}