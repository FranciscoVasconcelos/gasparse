#include <Python.h>
#include "types.h"
#include "common.h"
#include "multivector_gen.h"

//#include "multivector_types.h"
/* #include "multivector.h" */


#define blades0zero {{0},{0},{0},{0}}


typedef struct _gen0_DenseMultivector{
    ga_float value[8];
}gen0_DenseMultivector;

typedef struct _gen0_BladesMultivector{
    ga_float value0[1];
    ga_float value1[3];
    ga_float value2[3];
    ga_float value3[1];
}gen0_BladesMultivector;


typedef struct _gen0_GradesBitmap{
    int bitmap0[1];
    int bitmap1[3];
    int bitmap2[3];
    int bitmap3[1];
}gen0_GradesBitmap;

static gen0_GradesBitmap gen0_gradesbitmap = {
    {0},
    {1,2,4},
    {3,5,6},
    {7},
};

static int gen0_grades_position[8] = {0,0,1,0,2,1,2,0};

static void gen0_dense_grade0project(gen0_DenseMultivector *dense0, gen0_DenseMultivector *dense){

    dense->value[0] = dense0->value[0];
}

static void gen0_dense_grade1project(gen0_DenseMultivector *dense0, gen0_DenseMultivector *dense){

    dense->value[1] = dense0->value[1];
    dense->value[2] = dense0->value[2];
    dense->value[4] = dense0->value[4];
}

static void gen0_dense_grade2project(gen0_DenseMultivector *dense0, gen0_DenseMultivector *dense){

    dense->value[3] = dense0->value[3];
    dense->value[5] = dense0->value[5];
    dense->value[6] = dense0->value[6];
}

static void gen0_dense_grade3project(gen0_DenseMultivector *dense0, gen0_DenseMultivector *dense){

    dense->value[7] = dense0->value[7];
}


static void gen0_blades_grade0project(gen0_BladesMultivector *blades0, gen0_BladesMultivector *blades){
    memcpy(blades->value0,blades0->value0,sizeof(blades0->value0));
}

static void gen0_blades_grade1project(gen0_BladesMultivector *blades0, gen0_BladesMultivector *blades){
    memcpy(blades->value1,blades0->value1,sizeof(blades0->value1));
}

static void gen0_blades_grade2project(gen0_BladesMultivector *blades0, gen0_BladesMultivector *blades){
    memcpy(blades->value2,blades0->value2,sizeof(blades0->value2));
}

static void gen0_blades_grade3project(gen0_BladesMultivector *blades0, gen0_BladesMultivector *blades){
    memcpy(blades->value3,blades0->value3,sizeof(blades0->value3));
}


#define GEN0_BLADES_GRADE0PROJECT(blades0,blades)\
    (memcpy(blades->value0,blades0->value0,sizeof(blades0->value0)))


#define GEN0_BLADES_GRADE1PROJECT(blades0,blades)\
    (memcpy(blades->value1,blades0->value1,sizeof(blades0->value1)))


#define GEN0_BLADES_GRADE2PROJECT(blades0,blades)\
    (memcpy(blades->value2,blades0->value2,sizeof(blades0->value2)))


#define GEN0_BLADES_GRADE3PROJECT(blades0,blades)\
    (memcpy(blades->value3,blades0->value3,sizeof(blades0->value3)))




typedef void (*gen0densegradeprojectfunc)(gen0_DenseMultivector*,gen0_DenseMultivector*);

typedef struct gen0_DenseGradeProject_funcs{
    gen0densegradeprojectfunc gradeproject[4];
}gen0_DenseGradeProject_func;

static gen0_DenseGradeProject_func gen0denseproject = {
    .gradeproject[0] = gen0_dense_grade0project,
    .gradeproject[1] = gen0_dense_grade1project,
    .gradeproject[2] = gen0_dense_grade2project,
    .gradeproject[3] = gen0_dense_grade3project,
};

typedef void (*gen0bladesgradeprojectfunc)(gen0_BladesMultivector*,gen0_BladesMultivector*);
typedef struct gen0_BladesGradeProject_funcs{
    gen0bladesgradeprojectfunc gradeproject[4];
}gen0_BladesGradeProject_func;

static gen0_BladesGradeProject_func gen0bladesproject = {
    .gradeproject[0] = gen0_blades_grade0project,
    .gradeproject[1] = gen0_blades_grade1project,
    .gradeproject[2] = gen0_blades_grade2project,
    .gradeproject[3] = gen0_blades_grade3project,
};

static int gen0_dense_gradeproject(gen0_DenseMultivector *dense, gen0_DenseMultivector *dense0, int *grades, Py_ssize_t size){
    for(Py_ssize_t i = 0; i < size; i++){
        gen0densegradeprojectfunc gradeproject =
                            gen0denseproject.gradeproject[grades[i]];
        if(gradeproject)
            gradeproject(dense0,dense);
        else return -1;
    }
    return 0;
}
static int gen0_blades_gradeproject(gen0_BladesMultivector *blades, gen0_BladesMultivector *blades0, int *grades, Py_ssize_t size){
    for(Py_ssize_t i = 0; i < size; i++){
        gen0bladesgradeprojectfunc gradeproject =
                            gen0bladesproject.gradeproject[grades[i]];
        if(gradeproject)
            gradeproject(blades0,blades);
        else return -1;
    }
    return 0;
}

static gen0_DenseMultivector dense0_init_(int *bitmap, ga_float *value, Py_ssize_t size, PyAlgebraObject *ga){
    gen0_DenseMultivector dense = {{0}};
    for(Py_ssize_t i = 0; i < size; i++){
        if(bitmap[i] >= 8){
            return dense; // raise error
        }
        dense.value[bitmap[i]] += value[i]; // repeated blades are added to the same value
    }
    return dense;
}

static gen0_BladesMultivector blades0_init_(int *bitmap, ga_float *value, Py_ssize_t size, PyAlgebraObject *ga){
    gen0_BladesMultivector blades = {{0},{0},{0},{0},};

    for(Py_ssize_t i = 0; i < size; i++){
        switch(GRADE(bitmap[i])){
            case 0:
                blades.value0[gen0_grades_position[bitmap[i]]] += value[i];
                break;
            case 1:
                blades.value1[gen0_grades_position[bitmap[i]]] += value[i];
                break;
            case 2:
                blades.value2[gen0_grades_position[bitmap[i]]] += value[i];
                break;
            case 3:
                blades.value3[gen0_grades_position[bitmap[i]]] += value[i];
                break;
            default:
                return blades; // raise error
        }
    }
    return blades;
}

static int cast_to_blades0(PyMultivectorIter *from, void *to, PyAlgebraObject *GA){
    gen0_BladesMultivector *pblades = (gen0_BladesMultivector*)to;
    gen0_BladesMultivector blades =  {{0},{0},{0},{0},};
    if(!from || !pblades){
        return 0;
    }
    while(from->next(from)){
        switch(from->grade){
            case 0:
                blades.value0[gen0_grades_position[from->bitmap]] += from->value;
                break;
            case 1:
                blades.value1[gen0_grades_position[from->bitmap]] += from->value;
                break;
            case 2:
                blades.value2[gen0_grades_position[from->bitmap]] += from->value;
                break;
            case 3:
                blades.value3[gen0_grades_position[from->bitmap]] += from->value;
                break;
            default:
                return 0;
        }
    }
    *pblades = blades;
    return 1;
}

static int cast_to_dense0(PyMultivectorIter *from, void *to, PyAlgebraObject *GA){
    gen0_DenseMultivector *pdense = (gen0_DenseMultivector*)to;
    gen0_DenseMultivector dense =  {{0}};    
    if(!from || !pdense)
        return 0;
    
    while(from->next(from)){
        if(from->bitmap < 8)
            dense.value[from->bitmap] += from->value;
        else{
            return 0;
        }
    }
    *pdense = dense;
    return 1;
}

 
static int dense0_init(void *data, PyAlgebraObject *ga, int *bitmap, ga_float *value, Py_ssize_t size){
    gen0_DenseMultivector *dense = data;
    *dense = dense0_init_(bitmap,value,size,ga);
    return 1;
}

 
static int blades0_init(void *data, PyAlgebraObject *ga, int *bitmap, ga_float *value, Py_ssize_t size){
    gen0_BladesMultivector *blades = data;
    *blades = blades0_init_(bitmap,value,size,ga);
    return 1;
}




static PyMultivectorIter dense0_iterinit(void *data, PyMultivectorSubType *type){
    PyMultivectorIter iter;
    iter.data = data;
    iter.bitmap = -1;
    iter.value = 0;
    iter.type = type->ntype;
    iter.index = (Py_ssize_t*)PyMem_RawMalloc(sizeof(Py_ssize_t));
    iter.index[0] = 0;
    iter.size = 1;
    iter.niters = 8;
    iter.next = type->data_funcs->iter_next;
    iter.type_name = type->type_name;
    return iter;
}

static PyMultivectorIter blades0_iterinit(void *data, PyMultivectorSubType *type){
    PyMultivectorIter iter;
    iter.data= data;
    iter.bitmap = -1;
    iter.value = 0;
    iter.type = type->ntype;
    iter.index = (Py_ssize_t*)PyMem_RawMalloc(2*sizeof(Py_ssize_t));
    iter.index[0] = 0;
    iter.index[1] = 0;
    iter.size = 2;
    iter.niters = 8;
    iter.next = type->data_funcs->iter_next;
    iter.type_name = type->type_name;
    return iter;
}


static int blades0_iternext(PyMultivectorIter *iter){
    gen0_BladesMultivector *blades = (gen0_BladesMultivector*)iter->data;


    switch(*iter->index){
        case 0:
            iter->value = blades->value0[iter->index[1]];
            iter->bitmap = gen0_gradesbitmap.bitmap0[iter->index[1]];
            iter->grade = 0;
            if(++iter->index[1] >= 1){
                iter->index[1] = 0;
                (*iter->index)++;
            }
            return 1;
        case 1:
            iter->value = blades->value1[iter->index[1]];
            iter->bitmap = gen0_gradesbitmap.bitmap1[iter->index[1]];
            iter->grade = 1;
            if(++iter->index[1] >= 3){
                iter->index[1] = 0;
                (*iter->index)++;
            }
            return 1;
        case 2:
            iter->value = blades->value2[iter->index[1]];
            iter->bitmap = gen0_gradesbitmap.bitmap2[iter->index[1]];
            iter->grade = 2;
            if(++iter->index[1] >= 3){
                iter->index[1] = 0;
                (*iter->index)++;
            }
            return 1;
        case 3:
            iter->value = blades->value3[iter->index[1]];
            iter->bitmap = gen0_gradesbitmap.bitmap3[iter->index[1]];
            iter->grade = 3;
            if(++iter->index[1] >= 1){
                iter->index[1] = 0;
                (*iter->index)++;
            }
            return 1;
        default: // reset indices
            iter->index[1] = 0;
            iter->index[0] = 0;
            return 0; // stop loop
    }

    return 0;
}

static int dense0_iternext(PyMultivectorIter *iter){
    gen0_DenseMultivector *dense = (gen0_DenseMultivector*)iter->data;
    if(*iter->index >= 8){
        *iter->index = 0;
        return 0;
    }
    iter->bitmap = *iter->index;
    iter->value = dense->value[(*iter->index)++];
    iter->grade = GRADE(iter->bitmap);
    return 1;
}

#define blades1zero {{0},{0},{0},{0},{0},{0}}


typedef struct _gen1_DenseMultivector{
    ga_float value[32];
}gen1_DenseMultivector;

typedef struct _gen1_BladesMultivector{
    ga_float value0[1];
    ga_float value1[5];
    ga_float value2[10];
    ga_float value3[10];
    ga_float value4[5];
    ga_float value5[1];
}gen1_BladesMultivector;


typedef struct _gen1_GradesBitmap{
    int bitmap0[1];
    int bitmap1[5];
    int bitmap2[10];
    int bitmap3[10];
    int bitmap4[5];
    int bitmap5[1];
}gen1_GradesBitmap;

static gen1_GradesBitmap gen1_gradesbitmap = {
    {0},
    {1,2,4,8,16},
    {3,5,6,9,10,12,17,18,20,24},
    {7,11,13,14,19,21,22,25,26,28},
    {15,23,27,29,30},
    {31},
};

static int gen1_grades_position[32] = {0,0,1,0,2,1,2,0,3,3,4,1,5,2,3,0,4,6,7,4,8,5,6,1,9,7,8,2,9,3,4,0};

static void gen1_dense_grade0project(gen1_DenseMultivector *dense0, gen1_DenseMultivector *dense){

    dense->value[0] = dense0->value[0];
}

static void gen1_dense_grade1project(gen1_DenseMultivector *dense0, gen1_DenseMultivector *dense){

    dense->value[1] = dense0->value[1];
    dense->value[2] = dense0->value[2];
    dense->value[4] = dense0->value[4];
    dense->value[8] = dense0->value[8];
    dense->value[16] = dense0->value[16];
}

static void gen1_dense_grade2project(gen1_DenseMultivector *dense0, gen1_DenseMultivector *dense){

    dense->value[3] = dense0->value[3];
    dense->value[5] = dense0->value[5];
    dense->value[6] = dense0->value[6];
    dense->value[9] = dense0->value[9];
    dense->value[10] = dense0->value[10];
    dense->value[12] = dense0->value[12];
    dense->value[17] = dense0->value[17];
    dense->value[18] = dense0->value[18];
    dense->value[20] = dense0->value[20];
    dense->value[24] = dense0->value[24];
}

static void gen1_dense_grade3project(gen1_DenseMultivector *dense0, gen1_DenseMultivector *dense){

    dense->value[7] = dense0->value[7];
    dense->value[11] = dense0->value[11];
    dense->value[13] = dense0->value[13];
    dense->value[14] = dense0->value[14];
    dense->value[19] = dense0->value[19];
    dense->value[21] = dense0->value[21];
    dense->value[22] = dense0->value[22];
    dense->value[25] = dense0->value[25];
    dense->value[26] = dense0->value[26];
    dense->value[28] = dense0->value[28];
}

static void gen1_dense_grade4project(gen1_DenseMultivector *dense0, gen1_DenseMultivector *dense){

    dense->value[15] = dense0->value[15];
    dense->value[23] = dense0->value[23];
    dense->value[27] = dense0->value[27];
    dense->value[29] = dense0->value[29];
    dense->value[30] = dense0->value[30];
}

static void gen1_dense_grade5project(gen1_DenseMultivector *dense0, gen1_DenseMultivector *dense){

    dense->value[31] = dense0->value[31];
}


static void gen1_blades_grade0project(gen1_BladesMultivector *blades0, gen1_BladesMultivector *blades){
    memcpy(blades->value0,blades0->value0,sizeof(blades0->value0));
}

static void gen1_blades_grade1project(gen1_BladesMultivector *blades0, gen1_BladesMultivector *blades){
    memcpy(blades->value1,blades0->value1,sizeof(blades0->value1));
}

static void gen1_blades_grade2project(gen1_BladesMultivector *blades0, gen1_BladesMultivector *blades){
    memcpy(blades->value2,blades0->value2,sizeof(blades0->value2));
}

static void gen1_blades_grade3project(gen1_BladesMultivector *blades0, gen1_BladesMultivector *blades){
    memcpy(blades->value3,blades0->value3,sizeof(blades0->value3));
}

static void gen1_blades_grade4project(gen1_BladesMultivector *blades0, gen1_BladesMultivector *blades){
    memcpy(blades->value4,blades0->value4,sizeof(blades0->value4));
}

static void gen1_blades_grade5project(gen1_BladesMultivector *blades0, gen1_BladesMultivector *blades){
    memcpy(blades->value5,blades0->value5,sizeof(blades0->value5));
}


#define GEN1_BLADES_GRADE0PROJECT(blades0,blades)\
    (memcpy(blades->value0,blades0->value0,sizeof(blades0->value0)))


#define GEN1_BLADES_GRADE1PROJECT(blades0,blades)\
    (memcpy(blades->value1,blades0->value1,sizeof(blades0->value1)))


#define GEN1_BLADES_GRADE2PROJECT(blades0,blades)\
    (memcpy(blades->value2,blades0->value2,sizeof(blades0->value2)))


#define GEN1_BLADES_GRADE3PROJECT(blades0,blades)\
    (memcpy(blades->value3,blades0->value3,sizeof(blades0->value3)))


#define GEN1_BLADES_GRADE4PROJECT(blades0,blades)\
    (memcpy(blades->value4,blades0->value4,sizeof(blades0->value4)))


#define GEN1_BLADES_GRADE5PROJECT(blades0,blades)\
    (memcpy(blades->value5,blades0->value5,sizeof(blades0->value5)))




typedef void (*gen1densegradeprojectfunc)(gen1_DenseMultivector*,gen1_DenseMultivector*);

typedef struct gen1_DenseGradeProject_funcs{
    gen1densegradeprojectfunc gradeproject[6];
}gen1_DenseGradeProject_func;

static gen1_DenseGradeProject_func gen1denseproject = {
    .gradeproject[0] = gen1_dense_grade0project,
    .gradeproject[1] = gen1_dense_grade1project,
    .gradeproject[2] = gen1_dense_grade2project,
    .gradeproject[3] = gen1_dense_grade3project,
    .gradeproject[4] = gen1_dense_grade4project,
    .gradeproject[5] = gen1_dense_grade5project,
};

typedef void (*gen1bladesgradeprojectfunc)(gen1_BladesMultivector*,gen1_BladesMultivector*);
typedef struct gen1_BladesGradeProject_funcs{
    gen1bladesgradeprojectfunc gradeproject[6];
}gen1_BladesGradeProject_func;

static gen1_BladesGradeProject_func gen1bladesproject = {
    .gradeproject[0] = gen1_blades_grade0project,
    .gradeproject[1] = gen1_blades_grade1project,
    .gradeproject[2] = gen1_blades_grade2project,
    .gradeproject[3] = gen1_blades_grade3project,
    .gradeproject[4] = gen1_blades_grade4project,
    .gradeproject[5] = gen1_blades_grade5project,
};

static int gen1_dense_gradeproject(gen1_DenseMultivector *dense, gen1_DenseMultivector *dense0, int *grades, Py_ssize_t size){
    for(Py_ssize_t i = 0; i < size; i++){
        gen1densegradeprojectfunc gradeproject =
                            gen1denseproject.gradeproject[grades[i]];
        if(gradeproject)
            gradeproject(dense0,dense);
        else return -1;
    }
    return 0;
}
static int gen1_blades_gradeproject(gen1_BladesMultivector *blades, gen1_BladesMultivector *blades0, int *grades, Py_ssize_t size){
    for(Py_ssize_t i = 0; i < size; i++){
        gen1bladesgradeprojectfunc gradeproject =
                            gen1bladesproject.gradeproject[grades[i]];
        if(gradeproject)
            gradeproject(blades0,blades);
        else return -1;
    }
    return 0;
}

static gen1_DenseMultivector dense1_init_(int *bitmap, ga_float *value, Py_ssize_t size, PyAlgebraObject *ga){
    gen1_DenseMultivector dense = {{0}};
    for(Py_ssize_t i = 0; i < size; i++){
        if(bitmap[i] >= 32){
            return dense; // raise error
        }
        dense.value[bitmap[i]] += value[i]; // repeated blades are added to the same value
    }
    return dense;
}

static gen1_BladesMultivector blades1_init_(int *bitmap, ga_float *value, Py_ssize_t size, PyAlgebraObject *ga){
    gen1_BladesMultivector blades = {{0},{0},{0},{0},{0},{0},};

    for(Py_ssize_t i = 0; i < size; i++){
        switch(GRADE(bitmap[i])){
            case 0:
                blades.value0[gen1_grades_position[bitmap[i]]] += value[i];
                break;
            case 1:
                blades.value1[gen1_grades_position[bitmap[i]]] += value[i];
                break;
            case 2:
                blades.value2[gen1_grades_position[bitmap[i]]] += value[i];
                break;
            case 3:
                blades.value3[gen1_grades_position[bitmap[i]]] += value[i];
                break;
            case 4:
                blades.value4[gen1_grades_position[bitmap[i]]] += value[i];
                break;
            case 5:
                blades.value5[gen1_grades_position[bitmap[i]]] += value[i];
                break;
            default:
                return blades; // raise error
        }
    }
    return blades;
}

static int cast_to_blades1(PyMultivectorIter *from, void *to, PyAlgebraObject *GA){
    gen1_BladesMultivector *pblades = (gen1_BladesMultivector*)to;
    gen1_BladesMultivector blades =  {{0},{0},{0},{0},{0},{0},};
    if(!from || !pblades){
        return 0;
    }
    while(from->next(from)){
        switch(from->grade){
            case 0:
                blades.value0[gen1_grades_position[from->bitmap]] += from->value;
                break;
            case 1:
                blades.value1[gen1_grades_position[from->bitmap]] += from->value;
                break;
            case 2:
                blades.value2[gen1_grades_position[from->bitmap]] += from->value;
                break;
            case 3:
                blades.value3[gen1_grades_position[from->bitmap]] += from->value;
                break;
            case 4:
                blades.value4[gen1_grades_position[from->bitmap]] += from->value;
                break;
            case 5:
                blades.value5[gen1_grades_position[from->bitmap]] += from->value;
                break;
            default:
                return 0;
        }
    }
    *pblades = blades;
    return 1;
}

static int cast_to_dense1(PyMultivectorIter *from, void *to, PyAlgebraObject *GA){
    gen1_DenseMultivector *pdense = (gen1_DenseMultivector*)to;
    gen1_DenseMultivector dense =  {{0}};    
    if(!from || !pdense)
        return 0;
    
    while(from->next(from)){
        if(from->bitmap < 32)
            dense.value[from->bitmap] += from->value;
        else{
            return 0;
        }
    }
    *pdense = dense;
    return 1;
}

 
static int dense1_init(void *data, PyAlgebraObject *ga, int *bitmap, ga_float *value, Py_ssize_t size){
    gen1_DenseMultivector *dense = data;
    *dense = dense1_init_(bitmap,value,size,ga);
    return 1;
}

 
static int blades1_init(void *data, PyAlgebraObject *ga, int *bitmap, ga_float *value, Py_ssize_t size){
    gen1_BladesMultivector *blades = data;
    *blades = blades1_init_(bitmap,value,size,ga);
    return 1;
}




static PyMultivectorIter dense1_iterinit(void *data, PyMultivectorSubType *type){
    PyMultivectorIter iter;
    iter.data = data;
    iter.bitmap = -1;
    iter.value = 0;
    iter.type = type->ntype;
    iter.index = (Py_ssize_t*)PyMem_RawMalloc(sizeof(Py_ssize_t));
    iter.index[0] = 0;
    iter.size = 1;
    iter.niters = 32;
    iter.next = type->data_funcs->iter_next;
    iter.type_name = type->type_name;
    return iter;
}

static PyMultivectorIter blades1_iterinit(void *data, PyMultivectorSubType *type){
    PyMultivectorIter iter;
    iter.data= data;
    iter.bitmap = -1;
    iter.value = 0;
    iter.type = type->ntype;
    iter.index = (Py_ssize_t*)PyMem_RawMalloc(2*sizeof(Py_ssize_t));
    iter.index[0] = 0;
    iter.index[1] = 0;
    iter.size = 2;
    iter.niters = 32;
    iter.next = type->data_funcs->iter_next;
    iter.type_name = type->type_name;
    return iter;
}


static int blades1_iternext(PyMultivectorIter *iter){
    gen1_BladesMultivector *blades = (gen1_BladesMultivector*)iter->data;


    switch(*iter->index){
        case 0:
            iter->value = blades->value0[iter->index[1]];
            iter->bitmap = gen1_gradesbitmap.bitmap0[iter->index[1]];
            iter->grade = 0;
            if(++iter->index[1] >= 1){
                iter->index[1] = 0;
                (*iter->index)++;
            }
            return 1;
        case 1:
            iter->value = blades->value1[iter->index[1]];
            iter->bitmap = gen1_gradesbitmap.bitmap1[iter->index[1]];
            iter->grade = 1;
            if(++iter->index[1] >= 5){
                iter->index[1] = 0;
                (*iter->index)++;
            }
            return 1;
        case 2:
            iter->value = blades->value2[iter->index[1]];
            iter->bitmap = gen1_gradesbitmap.bitmap2[iter->index[1]];
            iter->grade = 2;
            if(++iter->index[1] >= 10){
                iter->index[1] = 0;
                (*iter->index)++;
            }
            return 1;
        case 3:
            iter->value = blades->value3[iter->index[1]];
            iter->bitmap = gen1_gradesbitmap.bitmap3[iter->index[1]];
            iter->grade = 3;
            if(++iter->index[1] >= 10){
                iter->index[1] = 0;
                (*iter->index)++;
            }
            return 1;
        case 4:
            iter->value = blades->value4[iter->index[1]];
            iter->bitmap = gen1_gradesbitmap.bitmap4[iter->index[1]];
            iter->grade = 4;
            if(++iter->index[1] >= 5){
                iter->index[1] = 0;
                (*iter->index)++;
            }
            return 1;
        case 5:
            iter->value = blades->value5[iter->index[1]];
            iter->bitmap = gen1_gradesbitmap.bitmap5[iter->index[1]];
            iter->grade = 5;
            if(++iter->index[1] >= 1){
                iter->index[1] = 0;
                (*iter->index)++;
            }
            return 1;
        default: // reset indices
            iter->index[1] = 0;
            iter->index[0] = 0;
            return 0; // stop loop
    }

    return 0;
}

static int dense1_iternext(PyMultivectorIter *iter){
    gen1_DenseMultivector *dense = (gen1_DenseMultivector*)iter->data;
    if(*iter->index >= 32){
        *iter->index = 0;
        return 0;
    }
    iter->bitmap = *iter->index;
    iter->value = dense->value[(*iter->index)++];
    iter->grade = GRADE(iter->bitmap);
    return 1;
}




static gen0_DenseMultivector gen0_dense_geometricproduct(gen0_DenseMultivector dense0, gen0_DenseMultivector dense1){
    gen0_DenseMultivector dense = {{0}};
    dense.value[0] =
    +dense0.value[0]*dense1.value[0]
    +dense0.value[1]*dense1.value[1]
    +dense0.value[2]*dense1.value[2]
    -dense0.value[3]*dense1.value[3]
    +dense0.value[4]*dense1.value[4]
    -dense0.value[5]*dense1.value[5]
    -dense0.value[6]*dense1.value[6]
    -dense0.value[7]*dense1.value[7]
;
    dense.value[1] =
    +dense0.value[0]*dense1.value[1]
    +dense0.value[1]*dense1.value[0]
    -dense0.value[2]*dense1.value[3]
    +dense0.value[3]*dense1.value[2]
    -dense0.value[4]*dense1.value[5]
    +dense0.value[5]*dense1.value[4]
    -dense0.value[6]*dense1.value[7]
    -dense0.value[7]*dense1.value[6]
;
    dense.value[2] =
    +dense0.value[0]*dense1.value[2]
    +dense0.value[1]*dense1.value[3]
    +dense0.value[2]*dense1.value[0]
    -dense0.value[3]*dense1.value[1]
    -dense0.value[4]*dense1.value[6]
    +dense0.value[5]*dense1.value[7]
    +dense0.value[6]*dense1.value[4]
    +dense0.value[7]*dense1.value[5]
;
    dense.value[3] =
    +dense0.value[0]*dense1.value[3]
    +dense0.value[1]*dense1.value[2]
    -dense0.value[2]*dense1.value[1]
    +dense0.value[3]*dense1.value[0]
    +dense0.value[4]*dense1.value[7]
    -dense0.value[5]*dense1.value[6]
    +dense0.value[6]*dense1.value[5]
    +dense0.value[7]*dense1.value[4]
;
    dense.value[4] =
    +dense0.value[0]*dense1.value[4]
    +dense0.value[1]*dense1.value[5]
    +dense0.value[2]*dense1.value[6]
    -dense0.value[3]*dense1.value[7]
    +dense0.value[4]*dense1.value[0]
    -dense0.value[5]*dense1.value[1]
    -dense0.value[6]*dense1.value[2]
    -dense0.value[7]*dense1.value[3]
;
    dense.value[5] =
    +dense0.value[0]*dense1.value[5]
    +dense0.value[1]*dense1.value[4]
    -dense0.value[2]*dense1.value[7]
    +dense0.value[3]*dense1.value[6]
    -dense0.value[4]*dense1.value[1]
    +dense0.value[5]*dense1.value[0]
    -dense0.value[6]*dense1.value[3]
    -dense0.value[7]*dense1.value[2]
;
    dense.value[6] =
    +dense0.value[0]*dense1.value[6]
    +dense0.value[1]*dense1.value[7]
    +dense0.value[2]*dense1.value[4]
    -dense0.value[3]*dense1.value[5]
    -dense0.value[4]*dense1.value[2]
    +dense0.value[5]*dense1.value[3]
    +dense0.value[6]*dense1.value[0]
    +dense0.value[7]*dense1.value[1]
;
    dense.value[7] =
    +dense0.value[0]*dense1.value[7]
    +dense0.value[1]*dense1.value[6]
    -dense0.value[2]*dense1.value[5]
    +dense0.value[3]*dense1.value[4]
    +dense0.value[4]*dense1.value[3]
    -dense0.value[5]*dense1.value[2]
    +dense0.value[6]*dense1.value[1]
    +dense0.value[7]*dense1.value[0]
;
    return dense;
}

// grade projection of the product of two multivectors <ab>_r
#define GEN0_DENSE_GRADE0GEOMETRICPRODUCT(dense,dense0,dense1){\
    dense.value[0] =\
    +dense0.value[0]*dense1.value[0]\
    +dense0.value[1]*dense1.value[1]\
    +dense0.value[2]*dense1.value[2]\
    -dense0.value[3]*dense1.value[3]\
    +dense0.value[4]*dense1.value[4]\
    -dense0.value[5]*dense1.value[5]\
    -dense0.value[6]*dense1.value[6]\
    -dense0.value[7]*dense1.value[7]\
;\
}
#define GEN0_DENSE_GRADE1GEOMETRICPRODUCT(dense,dense0,dense1){\
    dense.value[1] =\
    +dense0.value[0]*dense1.value[1]\
    +dense0.value[1]*dense1.value[0]\
    -dense0.value[2]*dense1.value[3]\
    +dense0.value[3]*dense1.value[2]\
    -dense0.value[4]*dense1.value[5]\
    +dense0.value[5]*dense1.value[4]\
    -dense0.value[6]*dense1.value[7]\
    -dense0.value[7]*dense1.value[6]\
;\
    dense.value[2] =\
    +dense0.value[0]*dense1.value[2]\
    +dense0.value[1]*dense1.value[3]\
    +dense0.value[2]*dense1.value[0]\
    -dense0.value[3]*dense1.value[1]\
    -dense0.value[4]*dense1.value[6]\
    +dense0.value[5]*dense1.value[7]\
    +dense0.value[6]*dense1.value[4]\
    +dense0.value[7]*dense1.value[5]\
;\
    dense.value[4] =\
    +dense0.value[0]*dense1.value[4]\
    +dense0.value[1]*dense1.value[5]\
    +dense0.value[2]*dense1.value[6]\
    -dense0.value[3]*dense1.value[7]\
    +dense0.value[4]*dense1.value[0]\
    -dense0.value[5]*dense1.value[1]\
    -dense0.value[6]*dense1.value[2]\
    -dense0.value[7]*dense1.value[3]\
;\
}
#define GEN0_DENSE_GRADE2GEOMETRICPRODUCT(dense,dense0,dense1){\
    dense.value[3] =\
    +dense0.value[0]*dense1.value[3]\
    +dense0.value[1]*dense1.value[2]\
    -dense0.value[2]*dense1.value[1]\
    +dense0.value[3]*dense1.value[0]\
    +dense0.value[4]*dense1.value[7]\
    -dense0.value[5]*dense1.value[6]\
    +dense0.value[6]*dense1.value[5]\
    +dense0.value[7]*dense1.value[4]\
;\
    dense.value[5] =\
    +dense0.value[0]*dense1.value[5]\
    +dense0.value[1]*dense1.value[4]\
    -dense0.value[2]*dense1.value[7]\
    +dense0.value[3]*dense1.value[6]\
    -dense0.value[4]*dense1.value[1]\
    +dense0.value[5]*dense1.value[0]\
    -dense0.value[6]*dense1.value[3]\
    -dense0.value[7]*dense1.value[2]\
;\
    dense.value[6] =\
    +dense0.value[0]*dense1.value[6]\
    +dense0.value[1]*dense1.value[7]\
    +dense0.value[2]*dense1.value[4]\
    -dense0.value[3]*dense1.value[5]\
    -dense0.value[4]*dense1.value[2]\
    +dense0.value[5]*dense1.value[3]\
    +dense0.value[6]*dense1.value[0]\
    +dense0.value[7]*dense1.value[1]\
;\
}
#define GEN0_DENSE_GRADE3GEOMETRICPRODUCT(dense,dense0,dense1){\
    dense.value[7] =\
    +dense0.value[0]*dense1.value[7]\
    +dense0.value[1]*dense1.value[6]\
    -dense0.value[2]*dense1.value[5]\
    +dense0.value[3]*dense1.value[4]\
    +dense0.value[4]*dense1.value[3]\
    -dense0.value[5]*dense1.value[2]\
    +dense0.value[6]*dense1.value[1]\
    +dense0.value[7]*dense1.value[0]\
;\
}

static gen0_DenseMultivector gen0_dense_gradegeometricproduct(gen0_DenseMultivector dense0, gen0_DenseMultivector dense1, int *grades, Py_ssize_t size){
    gen0_DenseMultivector dense = {{0}};
    for(Py_ssize_t i = 0; i < size; i++){
        switch(grades[i]){
            case 0:
                GEN0_DENSE_GRADE0GEOMETRICPRODUCT(dense,dense0,dense1);
                break;
            case 1:
                GEN0_DENSE_GRADE1GEOMETRICPRODUCT(dense,dense0,dense1);
                break;
            case 2:
                GEN0_DENSE_GRADE2GEOMETRICPRODUCT(dense,dense0,dense1);
                break;
            case 3:
                GEN0_DENSE_GRADE3GEOMETRICPRODUCT(dense,dense0,dense1);
                break;
            default:// do nothing for different values
                break;
        }
    }
    return dense;
}


static gen0_DenseMultivector gen0_dense_innerproduct(gen0_DenseMultivector dense0, gen0_DenseMultivector dense1){
    gen0_DenseMultivector dense = {{0}};
    dense.value[0] =
    +dense0.value[1]*dense1.value[1]
    +dense0.value[2]*dense1.value[2]
    -dense0.value[3]*dense1.value[3]
    +dense0.value[4]*dense1.value[4]
    -dense0.value[5]*dense1.value[5]
    -dense0.value[6]*dense1.value[6]
    -dense0.value[7]*dense1.value[7]
;
    dense.value[1] =
    -dense0.value[2]*dense1.value[3]
    +dense0.value[3]*dense1.value[2]
    -dense0.value[4]*dense1.value[5]
    +dense0.value[5]*dense1.value[4]
    -dense0.value[6]*dense1.value[7]
    -dense0.value[7]*dense1.value[6]
;
    dense.value[2] =
    +dense0.value[1]*dense1.value[3]
    -dense0.value[3]*dense1.value[1]
    -dense0.value[4]*dense1.value[6]
    +dense0.value[5]*dense1.value[7]
    +dense0.value[6]*dense1.value[4]
    +dense0.value[7]*dense1.value[5]
;
    dense.value[3] =
    +dense0.value[4]*dense1.value[7]
    +dense0.value[7]*dense1.value[4]
;
    dense.value[4] =
    +dense0.value[1]*dense1.value[5]
    +dense0.value[2]*dense1.value[6]
    -dense0.value[3]*dense1.value[7]
    -dense0.value[5]*dense1.value[1]
    -dense0.value[6]*dense1.value[2]
    -dense0.value[7]*dense1.value[3]
;
    dense.value[5] =
    -dense0.value[2]*dense1.value[7]
    -dense0.value[7]*dense1.value[2]
;
    dense.value[6] =
    +dense0.value[1]*dense1.value[7]
    +dense0.value[7]*dense1.value[1]
;
    return dense;
}

// grade projection of the product of two multivectors <ab>_r
#define GEN0_DENSE_GRADE0INNERPRODUCT(dense,dense0,dense1){\
    dense.value[0] =\
    +dense0.value[1]*dense1.value[1]\
    +dense0.value[2]*dense1.value[2]\
    -dense0.value[3]*dense1.value[3]\
    +dense0.value[4]*dense1.value[4]\
    -dense0.value[5]*dense1.value[5]\
    -dense0.value[6]*dense1.value[6]\
    -dense0.value[7]*dense1.value[7]\
;\
}
#define GEN0_DENSE_GRADE1INNERPRODUCT(dense,dense0,dense1){\
    dense.value[1] =\
    -dense0.value[2]*dense1.value[3]\
    +dense0.value[3]*dense1.value[2]\
    -dense0.value[4]*dense1.value[5]\
    +dense0.value[5]*dense1.value[4]\
    -dense0.value[6]*dense1.value[7]\
    -dense0.value[7]*dense1.value[6]\
;\
    dense.value[2] =\
    +dense0.value[1]*dense1.value[3]\
    -dense0.value[3]*dense1.value[1]\
    -dense0.value[4]*dense1.value[6]\
    +dense0.value[5]*dense1.value[7]\
    +dense0.value[6]*dense1.value[4]\
    +dense0.value[7]*dense1.value[5]\
;\
    dense.value[4] =\
    +dense0.value[1]*dense1.value[5]\
    +dense0.value[2]*dense1.value[6]\
    -dense0.value[3]*dense1.value[7]\
    -dense0.value[5]*dense1.value[1]\
    -dense0.value[6]*dense1.value[2]\
    -dense0.value[7]*dense1.value[3]\
;\
}
#define GEN0_DENSE_GRADE2INNERPRODUCT(dense,dense0,dense1){\
    dense.value[3] =\
    +dense0.value[4]*dense1.value[7]\
    +dense0.value[7]*dense1.value[4]\
;\
    dense.value[5] =\
    -dense0.value[2]*dense1.value[7]\
    -dense0.value[7]*dense1.value[2]\
;\
    dense.value[6] =\
    +dense0.value[1]*dense1.value[7]\
    +dense0.value[7]*dense1.value[1]\
;\
}
#define GEN0_DENSE_GRADE3INNERPRODUCT(dense,dense0,dense1){\
}

static gen0_DenseMultivector gen0_dense_gradeinnerproduct(gen0_DenseMultivector dense0, gen0_DenseMultivector dense1, int *grades, Py_ssize_t size){
    gen0_DenseMultivector dense = {{0}};
    for(Py_ssize_t i = 0; i < size; i++){
        switch(grades[i]){
            case 0:
                GEN0_DENSE_GRADE0INNERPRODUCT(dense,dense0,dense1);
                break;
            case 1:
                GEN0_DENSE_GRADE1INNERPRODUCT(dense,dense0,dense1);
                break;
            case 2:
                GEN0_DENSE_GRADE2INNERPRODUCT(dense,dense0,dense1);
                break;
            case 3:
                GEN0_DENSE_GRADE3INNERPRODUCT(dense,dense0,dense1);
                break;
            default:// do nothing for different values
                break;
        }
    }
    return dense;
}


static gen0_DenseMultivector gen0_dense_outerproduct(gen0_DenseMultivector dense0, gen0_DenseMultivector dense1){
    gen0_DenseMultivector dense = {{0}};
    dense.value[0] =
    +dense0.value[0]*dense1.value[0]
;
    dense.value[1] =
    +dense0.value[0]*dense1.value[1]
    +dense0.value[1]*dense1.value[0]
;
    dense.value[2] =
    +dense0.value[0]*dense1.value[2]
    +dense0.value[2]*dense1.value[0]
;
    dense.value[3] =
    +dense0.value[0]*dense1.value[3]
    +dense0.value[1]*dense1.value[2]
    -dense0.value[2]*dense1.value[1]
    +dense0.value[3]*dense1.value[0]
;
    dense.value[4] =
    +dense0.value[0]*dense1.value[4]
    +dense0.value[4]*dense1.value[0]
;
    dense.value[5] =
    +dense0.value[0]*dense1.value[5]
    +dense0.value[1]*dense1.value[4]
    -dense0.value[4]*dense1.value[1]
    +dense0.value[5]*dense1.value[0]
;
    dense.value[6] =
    +dense0.value[0]*dense1.value[6]
    +dense0.value[2]*dense1.value[4]
    -dense0.value[4]*dense1.value[2]
    +dense0.value[6]*dense1.value[0]
;
    dense.value[7] =
    +dense0.value[0]*dense1.value[7]
    +dense0.value[1]*dense1.value[6]
    -dense0.value[2]*dense1.value[5]
    +dense0.value[3]*dense1.value[4]
    +dense0.value[4]*dense1.value[3]
    -dense0.value[5]*dense1.value[2]
    +dense0.value[6]*dense1.value[1]
    +dense0.value[7]*dense1.value[0]
;
    return dense;
}

// grade projection of the product of two multivectors <ab>_r
#define GEN0_DENSE_GRADE0OUTERPRODUCT(dense,dense0,dense1){\
    dense.value[0] =\
    +dense0.value[0]*dense1.value[0]\
;\
}
#define GEN0_DENSE_GRADE1OUTERPRODUCT(dense,dense0,dense1){\
    dense.value[1] =\
    +dense0.value[0]*dense1.value[1]\
    +dense0.value[1]*dense1.value[0]\
;\
    dense.value[2] =\
    +dense0.value[0]*dense1.value[2]\
    +dense0.value[2]*dense1.value[0]\
;\
    dense.value[4] =\
    +dense0.value[0]*dense1.value[4]\
    +dense0.value[4]*dense1.value[0]\
;\
}
#define GEN0_DENSE_GRADE2OUTERPRODUCT(dense,dense0,dense1){\
    dense.value[3] =\
    +dense0.value[0]*dense1.value[3]\
    +dense0.value[1]*dense1.value[2]\
    -dense0.value[2]*dense1.value[1]\
    +dense0.value[3]*dense1.value[0]\
;\
    dense.value[5] =\
    +dense0.value[0]*dense1.value[5]\
    +dense0.value[1]*dense1.value[4]\
    -dense0.value[4]*dense1.value[1]\
    +dense0.value[5]*dense1.value[0]\
;\
    dense.value[6] =\
    +dense0.value[0]*dense1.value[6]\
    +dense0.value[2]*dense1.value[4]\
    -dense0.value[4]*dense1.value[2]\
    +dense0.value[6]*dense1.value[0]\
;\
}
#define GEN0_DENSE_GRADE3OUTERPRODUCT(dense,dense0,dense1){\
    dense.value[7] =\
    +dense0.value[0]*dense1.value[7]\
    +dense0.value[1]*dense1.value[6]\
    -dense0.value[2]*dense1.value[5]\
    +dense0.value[3]*dense1.value[4]\
    +dense0.value[4]*dense1.value[3]\
    -dense0.value[5]*dense1.value[2]\
    +dense0.value[6]*dense1.value[1]\
    +dense0.value[7]*dense1.value[0]\
;\
}

static gen0_DenseMultivector gen0_dense_gradeouterproduct(gen0_DenseMultivector dense0, gen0_DenseMultivector dense1, int *grades, Py_ssize_t size){
    gen0_DenseMultivector dense = {{0}};
    for(Py_ssize_t i = 0; i < size; i++){
        switch(grades[i]){
            case 0:
                GEN0_DENSE_GRADE0OUTERPRODUCT(dense,dense0,dense1);
                break;
            case 1:
                GEN0_DENSE_GRADE1OUTERPRODUCT(dense,dense0,dense1);
                break;
            case 2:
                GEN0_DENSE_GRADE2OUTERPRODUCT(dense,dense0,dense1);
                break;
            case 3:
                GEN0_DENSE_GRADE3OUTERPRODUCT(dense,dense0,dense1);
                break;
            default:// do nothing for different values
                break;
        }
    }
    return dense;
}


static gen0_DenseMultivector gen0_dense_regressiveproduct(gen0_DenseMultivector dense0, gen0_DenseMultivector dense1){
    gen0_DenseMultivector dense = {{0}};
    dense.value[0] =
    +dense0.value[0]*dense1.value[7]
    +dense0.value[1]*dense1.value[6]
    -dense0.value[2]*dense1.value[5]
    +dense0.value[3]*dense1.value[4]
    +dense0.value[4]*dense1.value[3]
    -dense0.value[5]*dense1.value[2]
    +dense0.value[6]*dense1.value[1]
    +dense0.value[7]*dense1.value[0]
;
    dense.value[1] =
    +dense0.value[1]*dense1.value[7]
    -dense0.value[3]*dense1.value[5]
    +dense0.value[5]*dense1.value[3]
    +dense0.value[7]*dense1.value[1]
;
    dense.value[2] =
    -dense0.value[2]*dense1.value[7]
    +dense0.value[3]*dense1.value[6]
    -dense0.value[6]*dense1.value[3]
    -dense0.value[7]*dense1.value[2]
;
    dense.value[3] =
    -dense0.value[3]*dense1.value[7]
    -dense0.value[7]*dense1.value[3]
;
    dense.value[4] =
    +dense0.value[4]*dense1.value[7]
    -dense0.value[5]*dense1.value[6]
    +dense0.value[6]*dense1.value[5]
    +dense0.value[7]*dense1.value[4]
;
    dense.value[5] =
    +dense0.value[5]*dense1.value[7]
    +dense0.value[7]*dense1.value[5]
;
    dense.value[6] =
    -dense0.value[6]*dense1.value[7]
    -dense0.value[7]*dense1.value[6]
;
    dense.value[7] =
    -dense0.value[7]*dense1.value[7]
;
    return dense;
}

// grade projection of the product of two multivectors <ab>_r
#define GEN0_DENSE_GRADE0REGRESSIVEPRODUCT(dense,dense0,dense1){\
    dense.value[0] =\
    +dense0.value[0]*dense1.value[7]\
    +dense0.value[1]*dense1.value[6]\
    -dense0.value[2]*dense1.value[5]\
    +dense0.value[3]*dense1.value[4]\
    +dense0.value[4]*dense1.value[3]\
    -dense0.value[5]*dense1.value[2]\
    +dense0.value[6]*dense1.value[1]\
    +dense0.value[7]*dense1.value[0]\
;\
}
#define GEN0_DENSE_GRADE1REGRESSIVEPRODUCT(dense,dense0,dense1){\
    dense.value[1] =\
    +dense0.value[1]*dense1.value[7]\
    -dense0.value[3]*dense1.value[5]\
    +dense0.value[5]*dense1.value[3]\
    +dense0.value[7]*dense1.value[1]\
;\
    dense.value[2] =\
    -dense0.value[2]*dense1.value[7]\
    +dense0.value[3]*dense1.value[6]\
    -dense0.value[6]*dense1.value[3]\
    -dense0.value[7]*dense1.value[2]\
;\
    dense.value[4] =\
    +dense0.value[4]*dense1.value[7]\
    -dense0.value[5]*dense1.value[6]\
    +dense0.value[6]*dense1.value[5]\
    +dense0.value[7]*dense1.value[4]\
;\
}
#define GEN0_DENSE_GRADE2REGRESSIVEPRODUCT(dense,dense0,dense1){\
    dense.value[3] =\
    -dense0.value[3]*dense1.value[7]\
    -dense0.value[7]*dense1.value[3]\
;\
    dense.value[5] =\
    +dense0.value[5]*dense1.value[7]\
    +dense0.value[7]*dense1.value[5]\
;\
    dense.value[6] =\
    -dense0.value[6]*dense1.value[7]\
    -dense0.value[7]*dense1.value[6]\
;\
}
#define GEN0_DENSE_GRADE3REGRESSIVEPRODUCT(dense,dense0,dense1){\
    dense.value[7] =\
    -dense0.value[7]*dense1.value[7]\
;\
}

static gen0_DenseMultivector gen0_dense_graderegressiveproduct(gen0_DenseMultivector dense0, gen0_DenseMultivector dense1, int *grades, Py_ssize_t size){
    gen0_DenseMultivector dense = {{0}};
    for(Py_ssize_t i = 0; i < size; i++){
        switch(grades[i]){
            case 0:
                GEN0_DENSE_GRADE0REGRESSIVEPRODUCT(dense,dense0,dense1);
                break;
            case 1:
                GEN0_DENSE_GRADE1REGRESSIVEPRODUCT(dense,dense0,dense1);
                break;
            case 2:
                GEN0_DENSE_GRADE2REGRESSIVEPRODUCT(dense,dense0,dense1);
                break;
            case 3:
                GEN0_DENSE_GRADE3REGRESSIVEPRODUCT(dense,dense0,dense1);
                break;
            default:// do nothing for different values
                break;
        }
    }
    return dense;
}


static int atomic_dense0_add(void *out, void *data0, PyAlgebraObject *GA, Py_ssize_t size){
    gen0_DenseMultivector *dense_array = (gen0_DenseMultivector *)data0;
    gen0_DenseMultivector dense = {{0}};
    gen0_DenseMultivector *dense_out = (gen0_DenseMultivector *)out;

    for(Py_ssize_t i = 0; i < size; i++){
        dense.value[0] += dense_array[i].value[0];
        dense.value[1] += dense_array[i].value[1];
        dense.value[2] += dense_array[i].value[2];
        dense.value[3] += dense_array[i].value[3];
        dense.value[4] += dense_array[i].value[4];
        dense.value[5] += dense_array[i].value[5];
        dense.value[6] += dense_array[i].value[6];
        dense.value[7] += dense_array[i].value[7];
    }

    *dense_out = dense;
    return 1;
}


static int binary_dense0_add(void *out, void *data0, void *data1, PyAlgebraObject *GA, int sign){
    gen0_DenseMultivector *dense0 = (gen0_DenseMultivector *)data0;
    gen0_DenseMultivector *dense1 = (gen0_DenseMultivector *)data1;
    gen0_DenseMultivector *dense_out = (gen0_DenseMultivector *)out;

    gen0_DenseMultivector dense = {{0}};
    if(sign == -1){
        dense.value[0] = dense0->value[0] - dense1->value[0];
        dense.value[1] = dense0->value[1] - dense1->value[1];
        dense.value[2] = dense0->value[2] - dense1->value[2];
        dense.value[3] = dense0->value[3] - dense1->value[3];
        dense.value[4] = dense0->value[4] - dense1->value[4];
        dense.value[5] = dense0->value[5] - dense1->value[5];
        dense.value[6] = dense0->value[6] - dense1->value[6];
        dense.value[7] = dense0->value[7] - dense1->value[7];
    }else if(sign == 1){
        dense.value[0] = dense0->value[0] + dense1->value[0];
        dense.value[1] = dense0->value[1] + dense1->value[1];
        dense.value[2] = dense0->value[2] + dense1->value[2];
        dense.value[3] = dense0->value[3] + dense1->value[3];
        dense.value[4] = dense0->value[4] + dense1->value[4];
        dense.value[5] = dense0->value[5] + dense1->value[5];
        dense.value[6] = dense0->value[6] + dense1->value[6];
        dense.value[7] = dense0->value[7] + dense1->value[7];
    } else{
        dense.value[0] = dense0->value[0] + sign*dense1->value[0];
        dense.value[1] = dense0->value[1] + sign*dense1->value[1];
        dense.value[2] = dense0->value[2] + sign*dense1->value[2];
        dense.value[3] = dense0->value[3] + sign*dense1->value[3];
        dense.value[4] = dense0->value[4] + sign*dense1->value[4];
        dense.value[5] = dense0->value[5] + sign*dense1->value[5];
        dense.value[6] = dense0->value[6] + sign*dense1->value[6];
        dense.value[7] = dense0->value[7] + sign*dense1->value[7];
    }

    *dense_out = dense;
    return 1;
}


static int binary_dense0_scalaradd(void *out, void *data0, PyAlgebraObject *GA, ga_float value, int sign){
    gen0_DenseMultivector *dense0 = (gen0_DenseMultivector*)data0;
    gen0_DenseMultivector *dense_out = (gen0_DenseMultivector*)out;

    gen0_DenseMultivector dense = {{0}};
    if(sign == -1){
        dense.value[0] = -dense0->value[0];
        dense.value[1] = -dense0->value[1];
        dense.value[2] = -dense0->value[2];
        dense.value[3] = -dense0->value[3];
        dense.value[4] = -dense0->value[4];
        dense.value[5] = -dense0->value[5];
        dense.value[6] = -dense0->value[6];
        dense.value[7] = -dense0->value[7];
    }else if(sign == 1){
        dense.value[0] = dense0->value[0];
        dense.value[1] = dense0->value[1];
        dense.value[2] = dense0->value[2];
        dense.value[3] = dense0->value[3];
        dense.value[4] = dense0->value[4];
        dense.value[5] = dense0->value[5];
        dense.value[6] = dense0->value[6];
        dense.value[7] = dense0->value[7];
    } else{
        dense.value[0] = sign*dense0->value[0];
        dense.value[1] = sign*dense0->value[1];
        dense.value[2] = sign*dense0->value[2];
        dense.value[3] = sign*dense0->value[3];
        dense.value[4] = sign*dense0->value[4];
        dense.value[5] = sign*dense0->value[5];
        dense.value[6] = sign*dense0->value[6];
        dense.value[7] = sign*dense0->value[7];
    }
    dense.value[0] += value;
    
    *dense_out = dense;
    return 1;
}

static int binary_dense0_scalarproduct(void *out, void *data0, PyAlgebraObject *GA, ga_float value){
    gen0_DenseMultivector* dense0 = (gen0_DenseMultivector*)data0;
    gen0_DenseMultivector* dense_out = (gen0_DenseMultivector*)out;
    gen0_DenseMultivector dense = {{0}};

    dense.value[0] = value*dense0->value[0];
    dense.value[1] = value*dense0->value[1];
    dense.value[2] = value*dense0->value[2];
    dense.value[3] = value*dense0->value[3];
    dense.value[4] = value*dense0->value[4];
    dense.value[5] = value*dense0->value[5];
    dense.value[6] = value*dense0->value[6];
    dense.value[7] = value*dense0->value[7];

    *dense_out = dense;
    return 1;
}

static int unary_dense0_reverse(void *out, void *data0, PyAlgebraObject *GA){
    gen0_DenseMultivector* dense0 = (gen0_DenseMultivector*)data0;
    gen0_DenseMultivector* dense_out = (gen0_DenseMultivector*)out;
    gen0_DenseMultivector dense = {{0}};

    dense.value[0] = dense0->value[0];
    dense.value[1] = dense0->value[1];
    dense.value[2] = dense0->value[2];
    dense.value[3] = -dense0->value[3];
    dense.value[4] = dense0->value[4];
    dense.value[5] = -dense0->value[5];
    dense.value[6] = -dense0->value[6];
    dense.value[7] = -dense0->value[7];

    *dense_out = dense;
    return 1;
}

static int unary_dense0_dual(void *out, void *data0, PyAlgebraObject *GA){
    gen0_DenseMultivector* dense0 = (gen0_DenseMultivector*)data0;
    gen0_DenseMultivector* dense_out = (gen0_DenseMultivector*)out;
    gen0_DenseMultivector dense = {{0}};

    dense.value[7] = -dense0->value[0];
    dense.value[6] = -dense0->value[1];
    dense.value[5] = dense0->value[2];
    dense.value[4] = dense0->value[3];
    dense.value[3] = -dense0->value[4];
    dense.value[2] = -dense0->value[5];
    dense.value[1] = dense0->value[6];
    dense.value[0] = dense0->value[7];

    *dense_out = dense;
    return 1;
}

static int unary_dense0_undual(void *out, void *data0, PyAlgebraObject *GA){
    gen0_DenseMultivector* dense0 = (gen0_DenseMultivector*)data0;
    gen0_DenseMultivector* dense_out = (gen0_DenseMultivector*)out;
    gen0_DenseMultivector dense = {{0}};

    dense.value[7] = dense0->value[0];
    dense.value[6] = dense0->value[1];
    dense.value[5] = -dense0->value[2];
    dense.value[4] = -dense0->value[3];
    dense.value[3] = dense0->value[4];
    dense.value[2] = dense0->value[5];
    dense.value[1] = -dense0->value[6];
    dense.value[0] = -dense0->value[7];

    *dense_out = dense;
    return 1;
}


static gen0_BladesMultivector gen0_blades_geometricproduct(gen0_BladesMultivector blades0, gen0_BladesMultivector blades1){
    gen0_BladesMultivector blades = {{0},{0},{0},{0},};

    blades.value0[0] =
    +blades0.value0[0]*blades1.value0[0]
    +blades0.value1[0]*blades1.value1[0]
    +blades0.value1[1]*blades1.value1[1]
    -blades0.value2[0]*blades1.value2[0]
    +blades0.value1[2]*blades1.value1[2]
    -blades0.value2[1]*blades1.value2[1]
    -blades0.value2[2]*blades1.value2[2]
    -blades0.value3[0]*blades1.value3[0]
;
    blades.value1[0] =
    +blades0.value0[0]*blades1.value1[0]
    +blades0.value1[0]*blades1.value0[0]
    -blades0.value1[1]*blades1.value2[0]
    +blades0.value2[0]*blades1.value1[1]
    -blades0.value1[2]*blades1.value2[1]
    +blades0.value2[1]*blades1.value1[2]
    -blades0.value2[2]*blades1.value3[0]
    -blades0.value3[0]*blades1.value2[2]
;
    blades.value1[1] =
    +blades0.value0[0]*blades1.value1[1]
    +blades0.value1[0]*blades1.value2[0]
    +blades0.value1[1]*blades1.value0[0]
    -blades0.value2[0]*blades1.value1[0]
    -blades0.value1[2]*blades1.value2[2]
    +blades0.value2[1]*blades1.value3[0]
    +blades0.value2[2]*blades1.value1[2]
    +blades0.value3[0]*blades1.value2[1]
;
    blades.value2[0] =
    +blades0.value0[0]*blades1.value2[0]
    +blades0.value1[0]*blades1.value1[1]
    -blades0.value1[1]*blades1.value1[0]
    +blades0.value2[0]*blades1.value0[0]
    +blades0.value1[2]*blades1.value3[0]
    -blades0.value2[1]*blades1.value2[2]
    +blades0.value2[2]*blades1.value2[1]
    +blades0.value3[0]*blades1.value1[2]
;
    blades.value1[2] =
    +blades0.value0[0]*blades1.value1[2]
    +blades0.value1[0]*blades1.value2[1]
    +blades0.value1[1]*blades1.value2[2]
    -blades0.value2[0]*blades1.value3[0]
    +blades0.value1[2]*blades1.value0[0]
    -blades0.value2[1]*blades1.value1[0]
    -blades0.value2[2]*blades1.value1[1]
    -blades0.value3[0]*blades1.value2[0]
;
    blades.value2[1] =
    +blades0.value0[0]*blades1.value2[1]
    +blades0.value1[0]*blades1.value1[2]
    -blades0.value1[1]*blades1.value3[0]
    +blades0.value2[0]*blades1.value2[2]
    -blades0.value1[2]*blades1.value1[0]
    +blades0.value2[1]*blades1.value0[0]
    -blades0.value2[2]*blades1.value2[0]
    -blades0.value3[0]*blades1.value1[1]
;
    blades.value2[2] =
    +blades0.value0[0]*blades1.value2[2]
    +blades0.value1[0]*blades1.value3[0]
    +blades0.value1[1]*blades1.value1[2]
    -blades0.value2[0]*blades1.value2[1]
    -blades0.value1[2]*blades1.value1[1]
    +blades0.value2[1]*blades1.value2[0]
    +blades0.value2[2]*blades1.value0[0]
    +blades0.value3[0]*blades1.value1[0]
;
    blades.value3[0] =
    +blades0.value0[0]*blades1.value3[0]
    +blades0.value1[0]*blades1.value2[2]
    -blades0.value1[1]*blades1.value2[1]
    +blades0.value2[0]*blades1.value1[2]
    +blades0.value1[2]*blades1.value2[0]
    -blades0.value2[1]*blades1.value1[1]
    +blades0.value2[2]*blades1.value1[0]
    +blades0.value3[0]*blades1.value0[0]
;
    return blades;
}

#define GEN0_BLADES_GRADE0GEOMETRICPRODUCT(blades,blades0,blades1){\
    blades.value0[0] =\
    +blades0.value0[0]*blades1.value0[0]\
    +blades0.value1[0]*blades1.value1[0]\
    +blades0.value1[1]*blades1.value1[1]\
    -blades0.value2[0]*blades1.value2[0]\
    +blades0.value1[2]*blades1.value1[2]\
    -blades0.value2[1]*blades1.value2[1]\
    -blades0.value2[2]*blades1.value2[2]\
    -blades0.value3[0]*blades1.value3[0]\
;\
}
#define GEN0_BLADES_GRADE1GEOMETRICPRODUCT(blades,blades0,blades1){\
    blades.value1[0] =\
    +blades0.value0[0]*blades1.value1[0]\
    +blades0.value1[0]*blades1.value0[0]\
    -blades0.value1[1]*blades1.value2[0]\
    +blades0.value2[0]*blades1.value1[1]\
    -blades0.value1[2]*blades1.value2[1]\
    +blades0.value2[1]*blades1.value1[2]\
    -blades0.value2[2]*blades1.value3[0]\
    -blades0.value3[0]*blades1.value2[2]\
;\
    blades.value1[1] =\
    +blades0.value0[0]*blades1.value1[1]\
    +blades0.value1[0]*blades1.value2[0]\
    +blades0.value1[1]*blades1.value0[0]\
    -blades0.value2[0]*blades1.value1[0]\
    -blades0.value1[2]*blades1.value2[2]\
    +blades0.value2[1]*blades1.value3[0]\
    +blades0.value2[2]*blades1.value1[2]\
    +blades0.value3[0]*blades1.value2[1]\
;\
    blades.value1[2] =\
    +blades0.value0[0]*blades1.value1[2]\
    +blades0.value1[0]*blades1.value2[1]\
    +blades0.value1[1]*blades1.value2[2]\
    -blades0.value2[0]*blades1.value3[0]\
    +blades0.value1[2]*blades1.value0[0]\
    -blades0.value2[1]*blades1.value1[0]\
    -blades0.value2[2]*blades1.value1[1]\
    -blades0.value3[0]*blades1.value2[0]\
;\
}
#define GEN0_BLADES_GRADE2GEOMETRICPRODUCT(blades,blades0,blades1){\
    blades.value2[0] =\
    +blades0.value0[0]*blades1.value2[0]\
    +blades0.value1[0]*blades1.value1[1]\
    -blades0.value1[1]*blades1.value1[0]\
    +blades0.value2[0]*blades1.value0[0]\
    +blades0.value1[2]*blades1.value3[0]\
    -blades0.value2[1]*blades1.value2[2]\
    +blades0.value2[2]*blades1.value2[1]\
    +blades0.value3[0]*blades1.value1[2]\
;\
    blades.value2[1] =\
    +blades0.value0[0]*blades1.value2[1]\
    +blades0.value1[0]*blades1.value1[2]\
    -blades0.value1[1]*blades1.value3[0]\
    +blades0.value2[0]*blades1.value2[2]\
    -blades0.value1[2]*blades1.value1[0]\
    +blades0.value2[1]*blades1.value0[0]\
    -blades0.value2[2]*blades1.value2[0]\
    -blades0.value3[0]*blades1.value1[1]\
;\
    blades.value2[2] =\
    +blades0.value0[0]*blades1.value2[2]\
    +blades0.value1[0]*blades1.value3[0]\
    +blades0.value1[1]*blades1.value1[2]\
    -blades0.value2[0]*blades1.value2[1]\
    -blades0.value1[2]*blades1.value1[1]\
    +blades0.value2[1]*blades1.value2[0]\
    +blades0.value2[2]*blades1.value0[0]\
    +blades0.value3[0]*blades1.value1[0]\
;\
}
#define GEN0_BLADES_GRADE3GEOMETRICPRODUCT(blades,blades0,blades1){\
    blades.value3[0] =\
    +blades0.value0[0]*blades1.value3[0]\
    +blades0.value1[0]*blades1.value2[2]\
    -blades0.value1[1]*blades1.value2[1]\
    +blades0.value2[0]*blades1.value1[2]\
    +blades0.value1[2]*blades1.value2[0]\
    -blades0.value2[1]*blades1.value1[1]\
    +blades0.value2[2]*blades1.value1[0]\
    +blades0.value3[0]*blades1.value0[0]\
;\
}

static gen0_BladesMultivector gen0_blades_gradegeometricproduct(gen0_BladesMultivector blades0, gen0_BladesMultivector blades1, int *grades, Py_ssize_t size){
    gen0_BladesMultivector blades = blades0zero;
    for(Py_ssize_t i = 0; i < size; i++){
        switch(grades[i]){
            case 0:
                GEN0_BLADES_GRADE0GEOMETRICPRODUCT(blades,blades0,blades1);
                break;
            case 1:
                GEN0_BLADES_GRADE1GEOMETRICPRODUCT(blades,blades0,blades1);
                break;
            case 2:
                GEN0_BLADES_GRADE2GEOMETRICPRODUCT(blades,blades0,blades1);
                break;
            case 3:
                GEN0_BLADES_GRADE3GEOMETRICPRODUCT(blades,blades0,blades1);
                break;
            default:// do nothing for different values
                break;
        }
    }
    return blades;
}


static gen0_BladesMultivector gen0_blades_innerproduct(gen0_BladesMultivector blades0, gen0_BladesMultivector blades1){
    gen0_BladesMultivector blades = {{0},{0},{0},{0},};

    blades.value0[0] =
    +blades0.value1[0]*blades1.value1[0]
    +blades0.value1[1]*blades1.value1[1]
    -blades0.value2[0]*blades1.value2[0]
    +blades0.value1[2]*blades1.value1[2]
    -blades0.value2[1]*blades1.value2[1]
    -blades0.value2[2]*blades1.value2[2]
    -blades0.value3[0]*blades1.value3[0]
;
    blades.value1[0] =
    -blades0.value1[1]*blades1.value2[0]
    +blades0.value2[0]*blades1.value1[1]
    -blades0.value1[2]*blades1.value2[1]
    +blades0.value2[1]*blades1.value1[2]
    -blades0.value2[2]*blades1.value3[0]
    -blades0.value3[0]*blades1.value2[2]
;
    blades.value1[1] =
    +blades0.value1[0]*blades1.value2[0]
    -blades0.value2[0]*blades1.value1[0]
    -blades0.value1[2]*blades1.value2[2]
    +blades0.value2[1]*blades1.value3[0]
    +blades0.value2[2]*blades1.value1[2]
    +blades0.value3[0]*blades1.value2[1]
;
    blades.value2[0] =
    +blades0.value1[2]*blades1.value3[0]
    +blades0.value3[0]*blades1.value1[2]
;
    blades.value1[2] =
    +blades0.value1[0]*blades1.value2[1]
    +blades0.value1[1]*blades1.value2[2]
    -blades0.value2[0]*blades1.value3[0]
    -blades0.value2[1]*blades1.value1[0]
    -blades0.value2[2]*blades1.value1[1]
    -blades0.value3[0]*blades1.value2[0]
;
    blades.value2[1] =
    -blades0.value1[1]*blades1.value3[0]
    -blades0.value3[0]*blades1.value1[1]
;
    blades.value2[2] =
    +blades0.value1[0]*blades1.value3[0]
    +blades0.value3[0]*blades1.value1[0]
;
    return blades;
}

#define GEN0_BLADES_GRADE0INNERPRODUCT(blades,blades0,blades1){\
    blades.value0[0] =\
    +blades0.value1[0]*blades1.value1[0]\
    +blades0.value1[1]*blades1.value1[1]\
    -blades0.value2[0]*blades1.value2[0]\
    +blades0.value1[2]*blades1.value1[2]\
    -blades0.value2[1]*blades1.value2[1]\
    -blades0.value2[2]*blades1.value2[2]\
    -blades0.value3[0]*blades1.value3[0]\
;\
}
#define GEN0_BLADES_GRADE1INNERPRODUCT(blades,blades0,blades1){\
    blades.value1[0] =\
    -blades0.value1[1]*blades1.value2[0]\
    +blades0.value2[0]*blades1.value1[1]\
    -blades0.value1[2]*blades1.value2[1]\
    +blades0.value2[1]*blades1.value1[2]\
    -blades0.value2[2]*blades1.value3[0]\
    -blades0.value3[0]*blades1.value2[2]\
;\
    blades.value1[1] =\
    +blades0.value1[0]*blades1.value2[0]\
    -blades0.value2[0]*blades1.value1[0]\
    -blades0.value1[2]*blades1.value2[2]\
    +blades0.value2[1]*blades1.value3[0]\
    +blades0.value2[2]*blades1.value1[2]\
    +blades0.value3[0]*blades1.value2[1]\
;\
    blades.value1[2] =\
    +blades0.value1[0]*blades1.value2[1]\
    +blades0.value1[1]*blades1.value2[2]\
    -blades0.value2[0]*blades1.value3[0]\
    -blades0.value2[1]*blades1.value1[0]\
    -blades0.value2[2]*blades1.value1[1]\
    -blades0.value3[0]*blades1.value2[0]\
;\
}
#define GEN0_BLADES_GRADE2INNERPRODUCT(blades,blades0,blades1){\
    blades.value2[0] =\
    +blades0.value1[2]*blades1.value3[0]\
    +blades0.value3[0]*blades1.value1[2]\
;\
    blades.value2[1] =\
    -blades0.value1[1]*blades1.value3[0]\
    -blades0.value3[0]*blades1.value1[1]\
;\
    blades.value2[2] =\
    +blades0.value1[0]*blades1.value3[0]\
    +blades0.value3[0]*blades1.value1[0]\
;\
}
#define GEN0_BLADES_GRADE3INNERPRODUCT(blades,blades0,blades1){\
}

static gen0_BladesMultivector gen0_blades_gradeinnerproduct(gen0_BladesMultivector blades0, gen0_BladesMultivector blades1, int *grades, Py_ssize_t size){
    gen0_BladesMultivector blades = blades0zero;
    for(Py_ssize_t i = 0; i < size; i++){
        switch(grades[i]){
            case 0:
                GEN0_BLADES_GRADE0INNERPRODUCT(blades,blades0,blades1);
                break;
            case 1:
                GEN0_BLADES_GRADE1INNERPRODUCT(blades,blades0,blades1);
                break;
            case 2:
                GEN0_BLADES_GRADE2INNERPRODUCT(blades,blades0,blades1);
                break;
            case 3:
                GEN0_BLADES_GRADE3INNERPRODUCT(blades,blades0,blades1);
                break;
            default:// do nothing for different values
                break;
        }
    }
    return blades;
}


static gen0_BladesMultivector gen0_blades_outerproduct(gen0_BladesMultivector blades0, gen0_BladesMultivector blades1){
    gen0_BladesMultivector blades = {{0},{0},{0},{0},};

    blades.value0[0] =
    +blades0.value0[0]*blades1.value0[0]
;
    blades.value1[0] =
    +blades0.value0[0]*blades1.value1[0]
    +blades0.value1[0]*blades1.value0[0]
;
    blades.value1[1] =
    +blades0.value0[0]*blades1.value1[1]
    +blades0.value1[1]*blades1.value0[0]
;
    blades.value2[0] =
    +blades0.value0[0]*blades1.value2[0]
    +blades0.value1[0]*blades1.value1[1]
    -blades0.value1[1]*blades1.value1[0]
    +blades0.value2[0]*blades1.value0[0]
;
    blades.value1[2] =
    +blades0.value0[0]*blades1.value1[2]
    +blades0.value1[2]*blades1.value0[0]
;
    blades.value2[1] =
    +blades0.value0[0]*blades1.value2[1]
    +blades0.value1[0]*blades1.value1[2]
    -blades0.value1[2]*blades1.value1[0]
    +blades0.value2[1]*blades1.value0[0]
;
    blades.value2[2] =
    +blades0.value0[0]*blades1.value2[2]
    +blades0.value1[1]*blades1.value1[2]
    -blades0.value1[2]*blades1.value1[1]
    +blades0.value2[2]*blades1.value0[0]
;
    blades.value3[0] =
    +blades0.value0[0]*blades1.value3[0]
    +blades0.value1[0]*blades1.value2[2]
    -blades0.value1[1]*blades1.value2[1]
    +blades0.value2[0]*blades1.value1[2]
    +blades0.value1[2]*blades1.value2[0]
    -blades0.value2[1]*blades1.value1[1]
    +blades0.value2[2]*blades1.value1[0]
    +blades0.value3[0]*blades1.value0[0]
;
    return blades;
}

#define GEN0_BLADES_GRADE0OUTERPRODUCT(blades,blades0,blades1){\
    blades.value0[0] =\
    +blades0.value0[0]*blades1.value0[0]\
;\
}
#define GEN0_BLADES_GRADE1OUTERPRODUCT(blades,blades0,blades1){\
    blades.value1[0] =\
    +blades0.value0[0]*blades1.value1[0]\
    +blades0.value1[0]*blades1.value0[0]\
;\
    blades.value1[1] =\
    +blades0.value0[0]*blades1.value1[1]\
    +blades0.value1[1]*blades1.value0[0]\
;\
    blades.value1[2] =\
    +blades0.value0[0]*blades1.value1[2]\
    +blades0.value1[2]*blades1.value0[0]\
;\
}
#define GEN0_BLADES_GRADE2OUTERPRODUCT(blades,blades0,blades1){\
    blades.value2[0] =\
    +blades0.value0[0]*blades1.value2[0]\
    +blades0.value1[0]*blades1.value1[1]\
    -blades0.value1[1]*blades1.value1[0]\
    +blades0.value2[0]*blades1.value0[0]\
;\
    blades.value2[1] =\
    +blades0.value0[0]*blades1.value2[1]\
    +blades0.value1[0]*blades1.value1[2]\
    -blades0.value1[2]*blades1.value1[0]\
    +blades0.value2[1]*blades1.value0[0]\
;\
    blades.value2[2] =\
    +blades0.value0[0]*blades1.value2[2]\
    +blades0.value1[1]*blades1.value1[2]\
    -blades0.value1[2]*blades1.value1[1]\
    +blades0.value2[2]*blades1.value0[0]\
;\
}
#define GEN0_BLADES_GRADE3OUTERPRODUCT(blades,blades0,blades1){\
    blades.value3[0] =\
    +blades0.value0[0]*blades1.value3[0]\
    +blades0.value1[0]*blades1.value2[2]\
    -blades0.value1[1]*blades1.value2[1]\
    +blades0.value2[0]*blades1.value1[2]\
    +blades0.value1[2]*blades1.value2[0]\
    -blades0.value2[1]*blades1.value1[1]\
    +blades0.value2[2]*blades1.value1[0]\
    +blades0.value3[0]*blades1.value0[0]\
;\
}

static gen0_BladesMultivector gen0_blades_gradeouterproduct(gen0_BladesMultivector blades0, gen0_BladesMultivector blades1, int *grades, Py_ssize_t size){
    gen0_BladesMultivector blades = blades0zero;
    for(Py_ssize_t i = 0; i < size; i++){
        switch(grades[i]){
            case 0:
                GEN0_BLADES_GRADE0OUTERPRODUCT(blades,blades0,blades1);
                break;
            case 1:
                GEN0_BLADES_GRADE1OUTERPRODUCT(blades,blades0,blades1);
                break;
            case 2:
                GEN0_BLADES_GRADE2OUTERPRODUCT(blades,blades0,blades1);
                break;
            case 3:
                GEN0_BLADES_GRADE3OUTERPRODUCT(blades,blades0,blades1);
                break;
            default:// do nothing for different values
                break;
        }
    }
    return blades;
}


static gen0_BladesMultivector gen0_blades_regressiveproduct(gen0_BladesMultivector blades0, gen0_BladesMultivector blades1){
    gen0_BladesMultivector blades = {{0},{0},{0},{0},};

    blades.value0[0] =
    +blades0.value0[0]*blades1.value3[0]
    +blades0.value1[0]*blades1.value2[2]
    -blades0.value1[1]*blades1.value2[1]
    +blades0.value2[0]*blades1.value1[2]
    +blades0.value1[2]*blades1.value2[0]
    -blades0.value2[1]*blades1.value1[1]
    +blades0.value2[2]*blades1.value1[0]
    +blades0.value3[0]*blades1.value0[0]
;
    blades.value1[0] =
    +blades0.value1[0]*blades1.value3[0]
    -blades0.value2[0]*blades1.value2[1]
    +blades0.value2[1]*blades1.value2[0]
    +blades0.value3[0]*blades1.value1[0]
;
    blades.value1[1] =
    -blades0.value1[1]*blades1.value3[0]
    +blades0.value2[0]*blades1.value2[2]
    -blades0.value2[2]*blades1.value2[0]
    -blades0.value3[0]*blades1.value1[1]
;
    blades.value2[0] =
    -blades0.value2[0]*blades1.value3[0]
    -blades0.value3[0]*blades1.value2[0]
;
    blades.value1[2] =
    +blades0.value1[2]*blades1.value3[0]
    -blades0.value2[1]*blades1.value2[2]
    +blades0.value2[2]*blades1.value2[1]
    +blades0.value3[0]*blades1.value1[2]
;
    blades.value2[1] =
    +blades0.value2[1]*blades1.value3[0]
    +blades0.value3[0]*blades1.value2[1]
;
    blades.value2[2] =
    -blades0.value2[2]*blades1.value3[0]
    -blades0.value3[0]*blades1.value2[2]
;
    blades.value3[0] =
    -blades0.value3[0]*blades1.value3[0]
;
    return blades;
}

#define GEN0_BLADES_GRADE0REGRESSIVEPRODUCT(blades,blades0,blades1){\
    blades.value0[0] =\
    +blades0.value0[0]*blades1.value3[0]\
    +blades0.value1[0]*blades1.value2[2]\
    -blades0.value1[1]*blades1.value2[1]\
    +blades0.value2[0]*blades1.value1[2]\
    +blades0.value1[2]*blades1.value2[0]\
    -blades0.value2[1]*blades1.value1[1]\
    +blades0.value2[2]*blades1.value1[0]\
    +blades0.value3[0]*blades1.value0[0]\
;\
}
#define GEN0_BLADES_GRADE1REGRESSIVEPRODUCT(blades,blades0,blades1){\
    blades.value1[0] =\
    +blades0.value1[0]*blades1.value3[0]\
    -blades0.value2[0]*blades1.value2[1]\
    +blades0.value2[1]*blades1.value2[0]\
    +blades0.value3[0]*blades1.value1[0]\
;\
    blades.value1[1] =\
    -blades0.value1[1]*blades1.value3[0]\
    +blades0.value2[0]*blades1.value2[2]\
    -blades0.value2[2]*blades1.value2[0]\
    -blades0.value3[0]*blades1.value1[1]\
;\
    blades.value1[2] =\
    +blades0.value1[2]*blades1.value3[0]\
    -blades0.value2[1]*blades1.value2[2]\
    +blades0.value2[2]*blades1.value2[1]\
    +blades0.value3[0]*blades1.value1[2]\
;\
}
#define GEN0_BLADES_GRADE2REGRESSIVEPRODUCT(blades,blades0,blades1){\
    blades.value2[0] =\
    -blades0.value2[0]*blades1.value3[0]\
    -blades0.value3[0]*blades1.value2[0]\
;\
    blades.value2[1] =\
    +blades0.value2[1]*blades1.value3[0]\
    +blades0.value3[0]*blades1.value2[1]\
;\
    blades.value2[2] =\
    -blades0.value2[2]*blades1.value3[0]\
    -blades0.value3[0]*blades1.value2[2]\
;\
}
#define GEN0_BLADES_GRADE3REGRESSIVEPRODUCT(blades,blades0,blades1){\
    blades.value3[0] =\
    -blades0.value3[0]*blades1.value3[0]\
;\
}

static gen0_BladesMultivector gen0_blades_graderegressiveproduct(gen0_BladesMultivector blades0, gen0_BladesMultivector blades1, int *grades, Py_ssize_t size){
    gen0_BladesMultivector blades = blades0zero;
    for(Py_ssize_t i = 0; i < size; i++){
        switch(grades[i]){
            case 0:
                GEN0_BLADES_GRADE0REGRESSIVEPRODUCT(blades,blades0,blades1);
                break;
            case 1:
                GEN0_BLADES_GRADE1REGRESSIVEPRODUCT(blades,blades0,blades1);
                break;
            case 2:
                GEN0_BLADES_GRADE2REGRESSIVEPRODUCT(blades,blades0,blades1);
                break;
            case 3:
                GEN0_BLADES_GRADE3REGRESSIVEPRODUCT(blades,blades0,blades1);
                break;
            default:// do nothing for different values
                break;
        }
    }
    return blades;
}


static int atomic_blades0_add(void *out, void *data0, PyAlgebraObject *GA, Py_ssize_t size){
    gen0_BladesMultivector *blades_array = (gen0_BladesMultivector *)data0;
    gen0_BladesMultivector *blades_out = (gen0_BladesMultivector *)out;
    gen0_BladesMultivector blades = {{0},{0},{0},{0},};

    for(Py_ssize_t i = 0; i < size; i++){
       blades.value0[0] += blades_array[i].value0[0];
       blades.value1[0] += blades_array[i].value1[0];
       blades.value1[1] += blades_array[i].value1[1];
       blades.value1[2] += blades_array[i].value1[2];
       blades.value2[0] += blades_array[i].value2[0];
       blades.value2[1] += blades_array[i].value2[1];
       blades.value2[2] += blades_array[i].value2[2];
       blades.value3[0] += blades_array[i].value3[0];
    }

    *blades_out = blades;
    return 1;
}

static int binary_blades0_add(void *out, void *data0, void *data1, PyAlgebraObject *GA, int sign){
    gen0_BladesMultivector *blades0 = (gen0_BladesMultivector *)data0;
    gen0_BladesMultivector *blades1 = (gen0_BladesMultivector *)data1;
    gen0_BladesMultivector *blades_out = (gen0_BladesMultivector *)out;

    gen0_BladesMultivector blades = {{0},{0},{0},{0},};

    if(sign == -1){
        blades.value0[0] = blades0->value0[0] - blades1->value0[0];
        blades.value1[0] = blades0->value1[0] - blades1->value1[0];
        blades.value1[1] = blades0->value1[1] - blades1->value1[1];
        blades.value1[2] = blades0->value1[2] - blades1->value1[2];
        blades.value2[0] = blades0->value2[0] - blades1->value2[0];
        blades.value2[1] = blades0->value2[1] - blades1->value2[1];
        blades.value2[2] = blades0->value2[2] - blades1->value2[2];
        blades.value3[0] = blades0->value3[0] - blades1->value3[0];
    }else if(sign == 1){
        blades.value0[0] = blades0->value0[0] + blades1->value0[0];
        blades.value1[0] = blades0->value1[0] + blades1->value1[0];
        blades.value1[1] = blades0->value1[1] + blades1->value1[1];
        blades.value1[2] = blades0->value1[2] + blades1->value1[2];
        blades.value2[0] = blades0->value2[0] + blades1->value2[0];
        blades.value2[1] = blades0->value2[1] + blades1->value2[1];
        blades.value2[2] = blades0->value2[2] + blades1->value2[2];
        blades.value3[0] = blades0->value3[0] + blades1->value3[0];
    }else{
        blades.value0[0] = blades0->value0[0] + sign*blades1->value0[0];
        blades.value1[0] = blades0->value1[0] + sign*blades1->value1[0];
        blades.value1[1] = blades0->value1[1] + sign*blades1->value1[1];
        blades.value1[2] = blades0->value1[2] + sign*blades1->value1[2];
        blades.value2[0] = blades0->value2[0] + sign*blades1->value2[0];
        blades.value2[1] = blades0->value2[1] + sign*blades1->value2[1];
        blades.value2[2] = blades0->value2[2] + sign*blades1->value2[2];
        blades.value3[0] = blades0->value3[0] + sign*blades1->value3[0];
    }
    *blades_out = blades;
    return 1;
}


static int binary_blades0_scalaradd(void *out, void *data0, PyAlgebraObject *GA, ga_float value, int sign){
    gen0_BladesMultivector *blades0 = (gen0_BladesMultivector *)data0;
    gen0_BladesMultivector *blades_out = (gen0_BladesMultivector *)out;

    gen0_BladesMultivector blades = {{0},{0},{0},{0},};

    if(sign == -1){
        blades.value0[0] = -blades0->value0[0];
        blades.value1[0] = -blades0->value1[0];
        blades.value1[1] = -blades0->value1[1];
        blades.value1[2] = -blades0->value1[2];
        blades.value2[0] = -blades0->value2[0];
        blades.value2[1] = -blades0->value2[1];
        blades.value2[2] = -blades0->value2[2];
        blades.value3[0] = -blades0->value3[0];
    }else if(sign == 1){
        blades.value0[0] = blades0->value0[0];
        blades.value1[0] = blades0->value1[0];
        blades.value1[1] = blades0->value1[1];
        blades.value1[2] = blades0->value1[2];
        blades.value2[0] = blades0->value2[0];
        blades.value2[1] = blades0->value2[1];
        blades.value2[2] = blades0->value2[2];
        blades.value3[0] = blades0->value3[0];
    }else{
        blades.value0[0] = sign*blades0->value0[0];
        blades.value1[0] = sign*blades0->value1[0];
        blades.value1[1] = sign*blades0->value1[1];
        blades.value1[2] = sign*blades0->value1[2];
        blades.value2[0] = sign*blades0->value2[0];
        blades.value2[1] = sign*blades0->value2[1];
        blades.value2[2] = sign*blades0->value2[2];
        blades.value3[0] = sign*blades0->value3[0];
    }
    blades.value0[0] += value;
    *blades_out = blades;
    return 1;
}


static int binary_blades0_scalarproduct(void *out, void *data0, PyAlgebraObject *GA, ga_float value){
    gen0_BladesMultivector *blades0 = (gen0_BladesMultivector *)data0;
    gen0_BladesMultivector *blades_out = (gen0_BladesMultivector *)out;

    gen0_BladesMultivector blades = {{0},{0},{0},{0},};

    blades.value0[0] = value*blades0->value0[0];
    blades.value1[0] = value*blades0->value1[0];
    blades.value1[1] = value*blades0->value1[1];
    blades.value1[2] = value*blades0->value1[2];
    blades.value2[0] = value*blades0->value2[0];
    blades.value2[1] = value*blades0->value2[1];
    blades.value2[2] = value*blades0->value2[2];
    blades.value3[0] = value*blades0->value3[0];
    *blades_out = blades;
    return 1;
}

static int unary_blades0_reverse(void *out, void *data0, PyAlgebraObject *GA){
    gen0_BladesMultivector *blades0 = (gen0_BladesMultivector *)data0;
    gen0_BladesMultivector *blades_out = (gen0_BladesMultivector *)out;

    gen0_BladesMultivector blades = {{0},{0},{0},{0},};

    blades.value0[0] = blades0->value0[0];
    blades.value1[0] = blades0->value1[0];
    blades.value1[1] = blades0->value1[1];
    blades.value1[2] = blades0->value1[2];
    blades.value2[0] = -blades0->value2[0];
    blades.value2[1] = -blades0->value2[1];
    blades.value2[2] = -blades0->value2[2];
    blades.value3[0] = -blades0->value3[0];
    
    *blades_out = blades;
    return 1;
}


static int unary_blades0_dual(void *out, void *data0, PyAlgebraObject *GA){
    gen0_BladesMultivector *blades0 = (gen0_BladesMultivector *)data0;
    gen0_BladesMultivector *blades_out = (gen0_BladesMultivector *)out;

    gen0_BladesMultivector blades = {{0},{0},{0},{0},};

    blades.value3[0] = -blades0->value0[0];
    blades.value2[2] = -blades0->value1[0];
    blades.value2[1] =  blades0->value1[1];
    blades.value1[2] =  blades0->value2[0];
    blades.value2[0] = -blades0->value1[2];
    blades.value1[1] = -blades0->value2[1];
    blades.value1[0] =  blades0->value2[2];
    blades.value0[0] =  blades0->value3[0];
    *blades_out = blades;
    return 1;
}

static int unary_blades0_undual(void *out, void *data0, PyAlgebraObject *GA){
    gen0_BladesMultivector *blades0 = (gen0_BladesMultivector*)data0;
    gen0_BladesMultivector *blades_out = (gen0_BladesMultivector*)out;

    gen0_BladesMultivector blades = {{0},{0},{0},{0},};

    blades.value3[0] =  blades0->value0[0];
    blades.value2[2] =  blades0->value1[0];
    blades.value2[1] = -blades0->value1[1];
    blades.value1[2] = -blades0->value2[0];
    blades.value2[0] =  blades0->value1[2];
    blades.value1[1] =  blades0->value2[1];
    blades.value1[0] = -blades0->value2[2];
    blades.value0[0] = -blades0->value3[0];
    *blades_out = blades;
    return 1;
}




static int binary_dense0_product(void *out, void *data0, void *data1, PyAlgebraObject *GA, ProductType ptype){
    gen0_DenseMultivector *pdense0 = (gen0_DenseMultivector*)data0;
    gen0_DenseMultivector *pdense1 = (gen0_DenseMultivector*)data1;
    gen0_DenseMultivector *pdense  = (gen0_DenseMultivector*)out;

    if(!pdense0 || !pdense1 || !pdense){
        return 0; // raise error
    }

    switch(ptype){
        case ProductType_geometric:
            *pdense = gen0_dense_geometricproduct(*pdense0,*pdense1);
            break;
        case ProductType_inner:
            *pdense = gen0_dense_innerproduct(*pdense0,*pdense1);
            break;
        case ProductType_outer:
            *pdense = gen0_dense_outerproduct(*pdense0,*pdense1);
            break;
        case ProductType_regressive:
            *pdense = gen0_dense_regressiveproduct(*pdense0,*pdense1);
            break;
        default:
            return 0;
    }

    return 1;
}
static int binary_blades0_product(void *out, void *data0, void *data1, PyAlgebraObject *GA, ProductType ptype){
    gen0_BladesMultivector *pblades0 = (gen0_BladesMultivector*)data0;
    gen0_BladesMultivector *pblades1 = (gen0_BladesMultivector*)data1;
    gen0_BladesMultivector *pblades  = (gen0_BladesMultivector*)out;

    if(!pblades0 || !pblades1 || !pblades){
        return 0; // raise error
    }

    switch(ptype){
        case ProductType_geometric:
            *pblades = gen0_blades_geometricproduct(*pblades0,*pblades1);
            break;
        case ProductType_inner:
            *pblades = gen0_blades_innerproduct(*pblades0,*pblades1);
            break;
        case ProductType_outer:
            *pblades = gen0_blades_outerproduct(*pblades0,*pblades1);
            break;
        case ProductType_regressive:
            *pblades = gen0_blades_regressiveproduct(*pblades0,*pblades1);
            break;
        default:
            return 0;
    }

    return 1;
}


static int binary_dense0_gradeproduct(void *out, void *data0, void *data1, PyAlgebraObject *GA, ProductType ptype, GradeProjectMap gpmap){
    gen0_DenseMultivector *pdense0 = (gen0_DenseMultivector*)data0;
    gen0_DenseMultivector *pdense1 = (gen0_DenseMultivector*)data1;
    gen0_DenseMultivector *pdense  = (gen0_DenseMultivector*)out;

    gen0_DenseMultivector projdense0 = {{0}};
    gen0_DenseMultivector projdense1 = {{0}};

    
    if(!pdense0 || !pdense1 || !pdense){
        return 0; // raise error
    }
    // grade projection of the input
    if(gen0_dense_gradeproject(&projdense0,pdense0,gpmap.grades0,gpmap.size0) == -1) return 0;
    if(gen0_dense_gradeproject(&projdense1,pdense1,gpmap.grades0,gpmap.size0) == -1) return 0;


    switch(ptype){
        case ProductType_geometric:
            *pdense = gen0_dense_gradegeometricproduct(projdense0,projdense1,gpmap.grades,gpmap.size);
            break;
        case ProductType_inner:
            *pdense = gen0_dense_gradeinnerproduct(projdense0,projdense1,gpmap.grades,gpmap.size);
            break;
        case ProductType_outer:
            *pdense = gen0_dense_gradeouterproduct(projdense0,projdense1,gpmap.grades,gpmap.size);
            break;
        case ProductType_regressive:
            *pdense = gen0_dense_graderegressiveproduct(projdense0,projdense1,gpmap.grades,gpmap.size);
            break;
        default:
            return 0;
    }

    return 1;
}
static int binary_blades0_gradeproduct(void *out, void *data0, void *data1, PyAlgebraObject *GA, ProductType ptype, GradeProjectMap gpmap){
    gen0_BladesMultivector *pblades0 = (gen0_BladesMultivector*)data0;
    gen0_BladesMultivector *pblades1 = (gen0_BladesMultivector*)data1;
    gen0_BladesMultivector *pblades  = (gen0_BladesMultivector*)out;

    gen0_BladesMultivector projblades0 =  blades0zero;
    gen0_BladesMultivector projblades1 =  blades0zero;

    
    if(!pblades0 || !pblades1 || !pblades){
        return 0; // raise error
    }
    // grade projection of the input
    if(gen0_blades_gradeproject(&projblades0,pblades0,gpmap.grades0,gpmap.size0) == -1) return 0;
    if(gen0_blades_gradeproject(&projblades1,pblades1,gpmap.grades0,gpmap.size0) == -1) return 0;


    switch(ptype){
        case ProductType_geometric:
            *pblades = gen0_blades_gradegeometricproduct(projblades0,projblades1,gpmap.grades,gpmap.size);
            break;
        case ProductType_inner:
            *pblades = gen0_blades_gradeinnerproduct(projblades0,projblades1,gpmap.grades,gpmap.size);
            break;
        case ProductType_outer:
            *pblades = gen0_blades_gradeouterproduct(projblades0,projblades1,gpmap.grades,gpmap.size);
            break;
        case ProductType_regressive:
            *pblades = gen0_blades_graderegressiveproduct(projblades0,projblades1,gpmap.grades,gpmap.size);
            break;
        default:
            return 0;
    }

    return 1;
}



static int ternary_dense0_product(void *out, void *data0, void *data1, void *data2, PyAlgebraObject *GA, ProductType ptype){
    gen0_DenseMultivector *pdense0 = (gen0_DenseMultivector*)data0;
    gen0_DenseMultivector *pdense1 = (gen0_DenseMultivector*)data1;
    gen0_DenseMultivector *pdense2 = (gen0_DenseMultivector*)data2;
    gen0_DenseMultivector *pdense  = (gen0_DenseMultivector*)out;
    
    if(!pdense0 || !pdense1 || !pdense2 || !pdense){
        return 0; // raise error
    }

    switch(ptype){
        case ProductType_geometric:
            *pdense = gen0_dense_geometricproduct(*pdense0,*pdense1);
            *pdense = gen0_dense_geometricproduct(*pdense,*pdense2);
            break;
        case ProductType_inner:
            *pdense = gen0_dense_innerproduct(*pdense0,*pdense1);
            *pdense = gen0_dense_innerproduct(*pdense,*pdense2);
            break;
        case ProductType_outer:
            *pdense = gen0_dense_outerproduct(*pdense0,*pdense1);
            *pdense = gen0_dense_outerproduct(*pdense,*pdense2);
            break;
        case ProductType_regressive:
            *pdense = gen0_dense_regressiveproduct(*pdense0,*pdense1);
            *pdense = gen0_dense_regressiveproduct(*pdense,*pdense2);
            break;
        default:
            return 0;
    }

    return 1;
}
static int ternary_blades0_product(void *out, void *data0, void *data1, void *data2, PyAlgebraObject *GA, ProductType ptype){
    gen0_BladesMultivector *pblades0 = (gen0_BladesMultivector*)data0;
    gen0_BladesMultivector *pblades1 = (gen0_BladesMultivector*)data1;
    gen0_BladesMultivector *pblades2 = (gen0_BladesMultivector*)data2;
    gen0_BladesMultivector *pblades  = (gen0_BladesMultivector*)out;
    
    if(!pblades0 || !pblades1 || !pblades2 || !pblades){
        return 0; // raise error
    }

    switch(ptype){
        case ProductType_geometric:
            *pblades = gen0_blades_geometricproduct(*pblades0,*pblades1);
            *pblades = gen0_blades_geometricproduct(*pblades,*pblades2);
            break;
        case ProductType_inner:
            *pblades = gen0_blades_innerproduct(*pblades0,*pblades1);
            *pblades = gen0_blades_innerproduct(*pblades,*pblades2);
            break;
        case ProductType_outer:
            *pblades = gen0_blades_outerproduct(*pblades0,*pblades1);
            *pblades = gen0_blades_outerproduct(*pblades,*pblades2);
            break;
        case ProductType_regressive:
            *pblades = gen0_blades_regressiveproduct(*pblades0,*pblades1);
            *pblades = gen0_blades_regressiveproduct(*pblades,*pblades2);
            break;
        default:
            return 0;
    }

    return 1;
}

static int unary_dense0_gradeproject(void *out, void *self, PyAlgebraObject *GA, int *grades, Py_ssize_t size){
    
    //gen0_DenseMultivector dense = {{0}};
    gen0_DenseMultivector *pdense = (gen0_DenseMultivector *)out;
    gen0_DenseMultivector *pdense0 = (gen0_DenseMultivector*)self;

    if(gen0_dense_gradeproject(pdense,pdense0,grades,size) == -1) return 0;
    
    return 1;
}
static int unary_blades0_gradeproject(void *out, void *self, PyAlgebraObject *GA, int *grades, Py_ssize_t size){
    
    //gen0_BladesMultivector blades =  blades0zero;
    gen0_BladesMultivector *pblades = (gen0_BladesMultivector *)out;
    gen0_BladesMultivector *pblades0 = (gen0_BladesMultivector*)self;

    if(gen0_blades_gradeproject(pblades,pblades0,grades,size) == -1) return 0;
    
    return 1;
}

static int atomic_dense0_product(void *out, void *data0, PyAlgebraObject *GA, Py_ssize_t size, ProductType ptype){
    if(size < 2) return 0;
    
    gen0_DenseMultivector *pdense0 = (gen0_DenseMultivector*)data0;
    gen0_DenseMultivector *pdense = (gen0_DenseMultivector*)out;
    gen0_DenseMultivector dense;
    if(!out  || !pdense){
        return 0; // raise memory error
    }
    switch(ptype){
        case ProductType_geometric:
            dense = gen0_dense_geometricproduct(*pdense0,pdense0[1]);
            for(Py_ssize_t i = 2; i < size; i++){
                dense = gen0_dense_geometricproduct(dense,pdense0[i]);
            }
            break;
        case ProductType_inner:
            dense = gen0_dense_innerproduct(*pdense0,pdense0[1]);
            for(Py_ssize_t i = 2; i < size; i++){
                dense = gen0_dense_innerproduct(dense,pdense0[i]);
            }
            break;
        case ProductType_outer:
            dense = gen0_dense_outerproduct(*pdense0,pdense0[1]);
            for(Py_ssize_t i = 2; i < size; i++){
                dense = gen0_dense_outerproduct(dense,pdense0[i]);
            }
            break;
        case ProductType_regressive:
            dense = gen0_dense_regressiveproduct(*pdense0,pdense0[1]);
            for(Py_ssize_t i = 2; i < size; i++){
                dense = gen0_dense_regressiveproduct(dense,pdense0[i]);
            }
            break;
        default:
            return 0;
    }
    *pdense = dense;

    return 1;
}
static int atomic_blades0_product(void *out, void *data0, PyAlgebraObject *GA, Py_ssize_t size, ProductType ptype){
    if(size < 2) return 0;
    
    gen0_BladesMultivector *pblades0 = (gen0_BladesMultivector*)data0;
    gen0_BladesMultivector *pblades = (gen0_BladesMultivector*)out;
    gen0_BladesMultivector blades;
    if(!out  || !pblades){
        return 0; // raise memory error
    }
    switch(ptype){
        case ProductType_geometric:
            blades = gen0_blades_geometricproduct(*pblades0,pblades0[1]);
            for(Py_ssize_t i = 2; i < size; i++){
                blades = gen0_blades_geometricproduct(blades,pblades0[i]);
            }
            break;
        case ProductType_inner:
            blades = gen0_blades_innerproduct(*pblades0,pblades0[1]);
            for(Py_ssize_t i = 2; i < size; i++){
                blades = gen0_blades_innerproduct(blades,pblades0[i]);
            }
            break;
        case ProductType_outer:
            blades = gen0_blades_outerproduct(*pblades0,pblades0[1]);
            for(Py_ssize_t i = 2; i < size; i++){
                blades = gen0_blades_outerproduct(blades,pblades0[i]);
            }
            break;
        case ProductType_regressive:
            blades = gen0_blades_regressiveproduct(*pblades0,pblades0[1]);
            for(Py_ssize_t i = 2; i < size; i++){
                blades = gen0_blades_regressiveproduct(blades,pblades0[i]);
            }
            break;
        default:
            return 0;
    }
    *pblades = blades;

    return 1;
}


static gen1_DenseMultivector gen1_dense_geometricproduct(gen1_DenseMultivector dense0, gen1_DenseMultivector dense1){
    gen1_DenseMultivector dense = {{0}};
    dense.value[0] =
    +dense0.value[0]*dense1.value[0]
    +dense0.value[1]*dense1.value[1]
    +dense0.value[2]*dense1.value[2]
    -dense0.value[3]*dense1.value[3]
    +dense0.value[4]*dense1.value[4]
    -dense0.value[5]*dense1.value[5]
    -dense0.value[6]*dense1.value[6]
    -dense0.value[7]*dense1.value[7]
    +dense0.value[8]*dense1.value[8]
    -dense0.value[9]*dense1.value[9]
    -dense0.value[10]*dense1.value[10]
    -dense0.value[11]*dense1.value[11]
    -dense0.value[12]*dense1.value[12]
    -dense0.value[13]*dense1.value[13]
    -dense0.value[14]*dense1.value[14]
    +dense0.value[15]*dense1.value[15]
    -dense0.value[16]*dense1.value[16]
    +dense0.value[17]*dense1.value[17]
    +dense0.value[18]*dense1.value[18]
    +dense0.value[19]*dense1.value[19]
    +dense0.value[20]*dense1.value[20]
    +dense0.value[21]*dense1.value[21]
    +dense0.value[22]*dense1.value[22]
    -dense0.value[23]*dense1.value[23]
    +dense0.value[24]*dense1.value[24]
    +dense0.value[25]*dense1.value[25]
    +dense0.value[26]*dense1.value[26]
    -dense0.value[27]*dense1.value[27]
    +dense0.value[28]*dense1.value[28]
    -dense0.value[29]*dense1.value[29]
    -dense0.value[30]*dense1.value[30]
    -dense0.value[31]*dense1.value[31]
;
    dense.value[1] =
    +dense0.value[0]*dense1.value[1]
    +dense0.value[1]*dense1.value[0]
    -dense0.value[2]*dense1.value[3]
    +dense0.value[3]*dense1.value[2]
    -dense0.value[4]*dense1.value[5]
    +dense0.value[5]*dense1.value[4]
    -dense0.value[6]*dense1.value[7]
    -dense0.value[7]*dense1.value[6]
    -dense0.value[8]*dense1.value[9]
    +dense0.value[9]*dense1.value[8]
    -dense0.value[10]*dense1.value[11]
    -dense0.value[11]*dense1.value[10]
    -dense0.value[12]*dense1.value[13]
    -dense0.value[13]*dense1.value[12]
    +dense0.value[14]*dense1.value[15]
    -dense0.value[15]*dense1.value[14]
    +dense0.value[16]*dense1.value[17]
    -dense0.value[17]*dense1.value[16]
    +dense0.value[18]*dense1.value[19]
    +dense0.value[19]*dense1.value[18]
    +dense0.value[20]*dense1.value[21]
    +dense0.value[21]*dense1.value[20]
    -dense0.value[22]*dense1.value[23]
    +dense0.value[23]*dense1.value[22]
    +dense0.value[24]*dense1.value[25]
    +dense0.value[25]*dense1.value[24]
    -dense0.value[26]*dense1.value[27]
    +dense0.value[27]*dense1.value[26]
    -dense0.value[28]*dense1.value[29]
    +dense0.value[29]*dense1.value[28]
    -dense0.value[30]*dense1.value[31]
    -dense0.value[31]*dense1.value[30]
;
    dense.value[2] =
    +dense0.value[0]*dense1.value[2]
    +dense0.value[1]*dense1.value[3]
    +dense0.value[2]*dense1.value[0]
    -dense0.value[3]*dense1.value[1]
    -dense0.value[4]*dense1.value[6]
    +dense0.value[5]*dense1.value[7]
    +dense0.value[6]*dense1.value[4]
    +dense0.value[7]*dense1.value[5]
    -dense0.value[8]*dense1.value[10]
    +dense0.value[9]*dense1.value[11]
    +dense0.value[10]*dense1.value[8]
    +dense0.value[11]*dense1.value[9]
    -dense0.value[12]*dense1.value[14]
    -dense0.value[13]*dense1.value[15]
    -dense0.value[14]*dense1.value[12]
    +dense0.value[15]*dense1.value[13]
    +dense0.value[16]*dense1.value[18]
    -dense0.value[17]*dense1.value[19]
    -dense0.value[18]*dense1.value[16]
    -dense0.value[19]*dense1.value[17]
    +dense0.value[20]*dense1.value[22]
    +dense0.value[21]*dense1.value[23]
    +dense0.value[22]*dense1.value[20]
    -dense0.value[23]*dense1.value[21]
    +dense0.value[24]*dense1.value[26]
    +dense0.value[25]*dense1.value[27]
    +dense0.value[26]*dense1.value[24]
    -dense0.value[27]*dense1.value[25]
    -dense0.value[28]*dense1.value[30]
    +dense0.value[29]*dense1.value[31]
    +dense0.value[30]*dense1.value[28]
    +dense0.value[31]*dense1.value[29]
;
    dense.value[3] =
    +dense0.value[0]*dense1.value[3]
    +dense0.value[1]*dense1.value[2]
    -dense0.value[2]*dense1.value[1]
    +dense0.value[3]*dense1.value[0]
    +dense0.value[4]*dense1.value[7]
    -dense0.value[5]*dense1.value[6]
    +dense0.value[6]*dense1.value[5]
    +dense0.value[7]*dense1.value[4]
    +dense0.value[8]*dense1.value[11]
    -dense0.value[9]*dense1.value[10]
    +dense0.value[10]*dense1.value[9]
    +dense0.value[11]*dense1.value[8]
    -dense0.value[12]*dense1.value[15]
    -dense0.value[13]*dense1.value[14]
    +dense0.value[14]*dense1.value[13]
    -dense0.value[15]*dense1.value[12]
    -dense0.value[16]*dense1.value[19]
    +dense0.value[17]*dense1.value[18]
    -dense0.value[18]*dense1.value[17]
    -dense0.value[19]*dense1.value[16]
    +dense0.value[20]*dense1.value[23]
    +dense0.value[21]*dense1.value[22]
    -dense0.value[22]*dense1.value[21]
    +dense0.value[23]*dense1.value[20]
    +dense0.value[24]*dense1.value[27]
    +dense0.value[25]*dense1.value[26]
    -dense0.value[26]*dense1.value[25]
    +dense0.value[27]*dense1.value[24]
    +dense0.value[28]*dense1.value[31]
    -dense0.value[29]*dense1.value[30]
    +dense0.value[30]*dense1.value[29]
    +dense0.value[31]*dense1.value[28]
;
    dense.value[4] =
    +dense0.value[0]*dense1.value[4]
    +dense0.value[1]*dense1.value[5]
    +dense0.value[2]*dense1.value[6]
    -dense0.value[3]*dense1.value[7]
    +dense0.value[4]*dense1.value[0]
    -dense0.value[5]*dense1.value[1]
    -dense0.value[6]*dense1.value[2]
    -dense0.value[7]*dense1.value[3]
    -dense0.value[8]*dense1.value[12]
    +dense0.value[9]*dense1.value[13]
    +dense0.value[10]*dense1.value[14]
    +dense0.value[11]*dense1.value[15]
    +dense0.value[12]*dense1.value[8]
    +dense0.value[13]*dense1.value[9]
    +dense0.value[14]*dense1.value[10]
    -dense0.value[15]*dense1.value[11]
    +dense0.value[16]*dense1.value[20]
    -dense0.value[17]*dense1.value[21]
    -dense0.value[18]*dense1.value[22]
    -dense0.value[19]*dense1.value[23]
    -dense0.value[20]*dense1.value[16]
    -dense0.value[21]*dense1.value[17]
    -dense0.value[22]*dense1.value[18]
    +dense0.value[23]*dense1.value[19]
    +dense0.value[24]*dense1.value[28]
    +dense0.value[25]*dense1.value[29]
    +dense0.value[26]*dense1.value[30]
    -dense0.value[27]*dense1.value[31]
    +dense0.value[28]*dense1.value[24]
    -dense0.value[29]*dense1.value[25]
    -dense0.value[30]*dense1.value[26]
    -dense0.value[31]*dense1.value[27]
;
    dense.value[5] =
    +dense0.value[0]*dense1.value[5]
    +dense0.value[1]*dense1.value[4]
    -dense0.value[2]*dense1.value[7]
    +dense0.value[3]*dense1.value[6]
    -dense0.value[4]*dense1.value[1]
    +dense0.value[5]*dense1.value[0]
    -dense0.value[6]*dense1.value[3]
    -dense0.value[7]*dense1.value[2]
    +dense0.value[8]*dense1.value[13]
    -dense0.value[9]*dense1.value[12]
    +dense0.value[10]*dense1.value[15]
    +dense0.value[11]*dense1.value[14]
    +dense0.value[12]*dense1.value[9]
    +dense0.value[13]*dense1.value[8]
    -dense0.value[14]*dense1.value[11]
    +dense0.value[15]*dense1.value[10]
    -dense0.value[16]*dense1.value[21]
    +dense0.value[17]*dense1.value[20]
    -dense0.value[18]*dense1.value[23]
    -dense0.value[19]*dense1.value[22]
    -dense0.value[20]*dense1.value[17]
    -dense0.value[21]*dense1.value[16]
    +dense0.value[22]*dense1.value[19]
    -dense0.value[23]*dense1.value[18]
    +dense0.value[24]*dense1.value[29]
    +dense0.value[25]*dense1.value[28]
    -dense0.value[26]*dense1.value[31]
    +dense0.value[27]*dense1.value[30]
    -dense0.value[28]*dense1.value[25]
    +dense0.value[29]*dense1.value[24]
    -dense0.value[30]*dense1.value[27]
    -dense0.value[31]*dense1.value[26]
;
    dense.value[6] =
    +dense0.value[0]*dense1.value[6]
    +dense0.value[1]*dense1.value[7]
    +dense0.value[2]*dense1.value[4]
    -dense0.value[3]*dense1.value[5]
    -dense0.value[4]*dense1.value[2]
    +dense0.value[5]*dense1.value[3]
    +dense0.value[6]*dense1.value[0]
    +dense0.value[7]*dense1.value[1]
    +dense0.value[8]*dense1.value[14]
    -dense0.value[9]*dense1.value[15]
    -dense0.value[10]*dense1.value[12]
    -dense0.value[11]*dense1.value[13]
    +dense0.value[12]*dense1.value[10]
    +dense0.value[13]*dense1.value[11]
    +dense0.value[14]*dense1.value[8]
    -dense0.value[15]*dense1.value[9]
    -dense0.value[16]*dense1.value[22]
    +dense0.value[17]*dense1.value[23]
    +dense0.value[18]*dense1.value[20]
    +dense0.value[19]*dense1.value[21]
    -dense0.value[20]*dense1.value[18]
    -dense0.value[21]*dense1.value[19]
    -dense0.value[22]*dense1.value[16]
    +dense0.value[23]*dense1.value[17]
    +dense0.value[24]*dense1.value[30]
    +dense0.value[25]*dense1.value[31]
    +dense0.value[26]*dense1.value[28]
    -dense0.value[27]*dense1.value[29]
    -dense0.value[28]*dense1.value[26]
    +dense0.value[29]*dense1.value[27]
    +dense0.value[30]*dense1.value[24]
    +dense0.value[31]*dense1.value[25]
;
    dense.value[7] =
    +dense0.value[0]*dense1.value[7]
    +dense0.value[1]*dense1.value[6]
    -dense0.value[2]*dense1.value[5]
    +dense0.value[3]*dense1.value[4]
    +dense0.value[4]*dense1.value[3]
    -dense0.value[5]*dense1.value[2]
    +dense0.value[6]*dense1.value[1]
    +dense0.value[7]*dense1.value[0]
    -dense0.value[8]*dense1.value[15]
    +dense0.value[9]*dense1.value[14]
    -dense0.value[10]*dense1.value[13]
    -dense0.value[11]*dense1.value[12]
    +dense0.value[12]*dense1.value[11]
    +dense0.value[13]*dense1.value[10]
    -dense0.value[14]*dense1.value[9]
    +dense0.value[15]*dense1.value[8]
    +dense0.value[16]*dense1.value[23]
    -dense0.value[17]*dense1.value[22]
    +dense0.value[18]*dense1.value[21]
    +dense0.value[19]*dense1.value[20]
    -dense0.value[20]*dense1.value[19]
    -dense0.value[21]*dense1.value[18]
    +dense0.value[22]*dense1.value[17]
    -dense0.value[23]*dense1.value[16]
    +dense0.value[24]*dense1.value[31]
    +dense0.value[25]*dense1.value[30]
    -dense0.value[26]*dense1.value[29]
    +dense0.value[27]*dense1.value[28]
    +dense0.value[28]*dense1.value[27]
    -dense0.value[29]*dense1.value[26]
    +dense0.value[30]*dense1.value[25]
    +dense0.value[31]*dense1.value[24]
;
    dense.value[8] =
    +dense0.value[0]*dense1.value[8]
    +dense0.value[1]*dense1.value[9]
    +dense0.value[2]*dense1.value[10]
    -dense0.value[3]*dense1.value[11]
    +dense0.value[4]*dense1.value[12]
    -dense0.value[5]*dense1.value[13]
    -dense0.value[6]*dense1.value[14]
    -dense0.value[7]*dense1.value[15]
    +dense0.value[8]*dense1.value[0]
    -dense0.value[9]*dense1.value[1]
    -dense0.value[10]*dense1.value[2]
    -dense0.value[11]*dense1.value[3]
    -dense0.value[12]*dense1.value[4]
    -dense0.value[13]*dense1.value[5]
    -dense0.value[14]*dense1.value[6]
    +dense0.value[15]*dense1.value[7]
    +dense0.value[16]*dense1.value[24]
    -dense0.value[17]*dense1.value[25]
    -dense0.value[18]*dense1.value[26]
    -dense0.value[19]*dense1.value[27]
    -dense0.value[20]*dense1.value[28]
    -dense0.value[21]*dense1.value[29]
    -dense0.value[22]*dense1.value[30]
    +dense0.value[23]*dense1.value[31]
    -dense0.value[24]*dense1.value[16]
    -dense0.value[25]*dense1.value[17]
    -dense0.value[26]*dense1.value[18]
    +dense0.value[27]*dense1.value[19]
    -dense0.value[28]*dense1.value[20]
    +dense0.value[29]*dense1.value[21]
    +dense0.value[30]*dense1.value[22]
    +dense0.value[31]*dense1.value[23]
;
    dense.value[9] =
    +dense0.value[0]*dense1.value[9]
    +dense0.value[1]*dense1.value[8]
    -dense0.value[2]*dense1.value[11]
    +dense0.value[3]*dense1.value[10]
    -dense0.value[4]*dense1.value[13]
    +dense0.value[5]*dense1.value[12]
    -dense0.value[6]*dense1.value[15]
    -dense0.value[7]*dense1.value[14]
    -dense0.value[8]*dense1.value[1]
    +dense0.value[9]*dense1.value[0]
    -dense0.value[10]*dense1.value[3]
    -dense0.value[11]*dense1.value[2]
    -dense0.value[12]*dense1.value[5]
    -dense0.value[13]*dense1.value[4]
    +dense0.value[14]*dense1.value[7]
    -dense0.value[15]*dense1.value[6]
    -dense0.value[16]*dense1.value[25]
    +dense0.value[17]*dense1.value[24]
    -dense0.value[18]*dense1.value[27]
    -dense0.value[19]*dense1.value[26]
    -dense0.value[20]*dense1.value[29]
    -dense0.value[21]*dense1.value[28]
    +dense0.value[22]*dense1.value[31]
    -dense0.value[23]*dense1.value[30]
    -dense0.value[24]*dense1.value[17]
    -dense0.value[25]*dense1.value[16]
    +dense0.value[26]*dense1.value[19]
    -dense0.value[27]*dense1.value[18]
    +dense0.value[28]*dense1.value[21]
    -dense0.value[29]*dense1.value[20]
    +dense0.value[30]*dense1.value[23]
    +dense0.value[31]*dense1.value[22]
;
    dense.value[10] =
    +dense0.value[0]*dense1.value[10]
    +dense0.value[1]*dense1.value[11]
    +dense0.value[2]*dense1.value[8]
    -dense0.value[3]*dense1.value[9]
    -dense0.value[4]*dense1.value[14]
    +dense0.value[5]*dense1.value[15]
    +dense0.value[6]*dense1.value[12]
    +dense0.value[7]*dense1.value[13]
    -dense0.value[8]*dense1.value[2]
    +dense0.value[9]*dense1.value[3]
    +dense0.value[10]*dense1.value[0]
    +dense0.value[11]*dense1.value[1]
    -dense0.value[12]*dense1.value[6]
    -dense0.value[13]*dense1.value[7]
    -dense0.value[14]*dense1.value[4]
    +dense0.value[15]*dense1.value[5]
    -dense0.value[16]*dense1.value[26]
    +dense0.value[17]*dense1.value[27]
    +dense0.value[18]*dense1.value[24]
    +dense0.value[19]*dense1.value[25]
    -dense0.value[20]*dense1.value[30]
    -dense0.value[21]*dense1.value[31]
    -dense0.value[22]*dense1.value[28]
    +dense0.value[23]*dense1.value[29]
    -dense0.value[24]*dense1.value[18]
    -dense0.value[25]*dense1.value[19]
    -dense0.value[26]*dense1.value[16]
    +dense0.value[27]*dense1.value[17]
    +dense0.value[28]*dense1.value[22]
    -dense0.value[29]*dense1.value[23]
    -dense0.value[30]*dense1.value[20]
    -dense0.value[31]*dense1.value[21]
;
    dense.value[11] =
    +dense0.value[0]*dense1.value[11]
    +dense0.value[1]*dense1.value[10]
    -dense0.value[2]*dense1.value[9]
    +dense0.value[3]*dense1.value[8]
    +dense0.value[4]*dense1.value[15]
    -dense0.value[5]*dense1.value[14]
    +dense0.value[6]*dense1.value[13]
    +dense0.value[7]*dense1.value[12]
    +dense0.value[8]*dense1.value[3]
    -dense0.value[9]*dense1.value[2]
    +dense0.value[10]*dense1.value[1]
    +dense0.value[11]*dense1.value[0]
    -dense0.value[12]*dense1.value[7]
    -dense0.value[13]*dense1.value[6]
    +dense0.value[14]*dense1.value[5]
    -dense0.value[15]*dense1.value[4]
    +dense0.value[16]*dense1.value[27]
    -dense0.value[17]*dense1.value[26]
    +dense0.value[18]*dense1.value[25]
    +dense0.value[19]*dense1.value[24]
    -dense0.value[20]*dense1.value[31]
    -dense0.value[21]*dense1.value[30]
    +dense0.value[22]*dense1.value[29]
    -dense0.value[23]*dense1.value[28]
    -dense0.value[24]*dense1.value[19]
    -dense0.value[25]*dense1.value[18]
    +dense0.value[26]*dense1.value[17]
    -dense0.value[27]*dense1.value[16]
    -dense0.value[28]*dense1.value[23]
    +dense0.value[29]*dense1.value[22]
    -dense0.value[30]*dense1.value[21]
    -dense0.value[31]*dense1.value[20]
;
    dense.value[12] =
    +dense0.value[0]*dense1.value[12]
    +dense0.value[1]*dense1.value[13]
    +dense0.value[2]*dense1.value[14]
    -dense0.value[3]*dense1.value[15]
    +dense0.value[4]*dense1.value[8]
    -dense0.value[5]*dense1.value[9]
    -dense0.value[6]*dense1.value[10]
    -dense0.value[7]*dense1.value[11]
    -dense0.value[8]*dense1.value[4]
    +dense0.value[9]*dense1.value[5]
    +dense0.value[10]*dense1.value[6]
    +dense0.value[11]*dense1.value[7]
    +dense0.value[12]*dense1.value[0]
    +dense0.value[13]*dense1.value[1]
    +dense0.value[14]*dense1.value[2]
    -dense0.value[15]*dense1.value[3]
    -dense0.value[16]*dense1.value[28]
    +dense0.value[17]*dense1.value[29]
    +dense0.value[18]*dense1.value[30]
    +dense0.value[19]*dense1.value[31]
    +dense0.value[20]*dense1.value[24]
    +dense0.value[21]*dense1.value[25]
    +dense0.value[22]*dense1.value[26]
    -dense0.value[23]*dense1.value[27]
    -dense0.value[24]*dense1.value[20]
    -dense0.value[25]*dense1.value[21]
    -dense0.value[26]*dense1.value[22]
    +dense0.value[27]*dense1.value[23]
    -dense0.value[28]*dense1.value[16]
    +dense0.value[29]*dense1.value[17]
    +dense0.value[30]*dense1.value[18]
    +dense0.value[31]*dense1.value[19]
;
    dense.value[13] =
    +dense0.value[0]*dense1.value[13]
    +dense0.value[1]*dense1.value[12]
    -dense0.value[2]*dense1.value[15]
    +dense0.value[3]*dense1.value[14]
    -dense0.value[4]*dense1.value[9]
    +dense0.value[5]*dense1.value[8]
    -dense0.value[6]*dense1.value[11]
    -dense0.value[7]*dense1.value[10]
    +dense0.value[8]*dense1.value[5]
    -dense0.value[9]*dense1.value[4]
    +dense0.value[10]*dense1.value[7]
    +dense0.value[11]*dense1.value[6]
    +dense0.value[12]*dense1.value[1]
    +dense0.value[13]*dense1.value[0]
    -dense0.value[14]*dense1.value[3]
    +dense0.value[15]*dense1.value[2]
    +dense0.value[16]*dense1.value[29]
    -dense0.value[17]*dense1.value[28]
    +dense0.value[18]*dense1.value[31]
    +dense0.value[19]*dense1.value[30]
    +dense0.value[20]*dense1.value[25]
    +dense0.value[21]*dense1.value[24]
    -dense0.value[22]*dense1.value[27]
    +dense0.value[23]*dense1.value[26]
    -dense0.value[24]*dense1.value[21]
    -dense0.value[25]*dense1.value[20]
    +dense0.value[26]*dense1.value[23]
    -dense0.value[27]*dense1.value[22]
    +dense0.value[28]*dense1.value[17]
    -dense0.value[29]*dense1.value[16]
    +dense0.value[30]*dense1.value[19]
    +dense0.value[31]*dense1.value[18]
;
    dense.value[14] =
    +dense0.value[0]*dense1.value[14]
    +dense0.value[1]*dense1.value[15]
    +dense0.value[2]*dense1.value[12]
    -dense0.value[3]*dense1.value[13]
    -dense0.value[4]*dense1.value[10]
    +dense0.value[5]*dense1.value[11]
    +dense0.value[6]*dense1.value[8]
    +dense0.value[7]*dense1.value[9]
    +dense0.value[8]*dense1.value[6]
    -dense0.value[9]*dense1.value[7]
    -dense0.value[10]*dense1.value[4]
    -dense0.value[11]*dense1.value[5]
    +dense0.value[12]*dense1.value[2]
    +dense0.value[13]*dense1.value[3]
    +dense0.value[14]*dense1.value[0]
    -dense0.value[15]*dense1.value[1]
    +dense0.value[16]*dense1.value[30]
    -dense0.value[17]*dense1.value[31]
    -dense0.value[18]*dense1.value[28]
    -dense0.value[19]*dense1.value[29]
    +dense0.value[20]*dense1.value[26]
    +dense0.value[21]*dense1.value[27]
    +dense0.value[22]*dense1.value[24]
    -dense0.value[23]*dense1.value[25]
    -dense0.value[24]*dense1.value[22]
    -dense0.value[25]*dense1.value[23]
    -dense0.value[26]*dense1.value[20]
    +dense0.value[27]*dense1.value[21]
    +dense0.value[28]*dense1.value[18]
    -dense0.value[29]*dense1.value[19]
    -dense0.value[30]*dense1.value[16]
    -dense0.value[31]*dense1.value[17]
;
    dense.value[15] =
    +dense0.value[0]*dense1.value[15]
    +dense0.value[1]*dense1.value[14]
    -dense0.value[2]*dense1.value[13]
    +dense0.value[3]*dense1.value[12]
    +dense0.value[4]*dense1.value[11]
    -dense0.value[5]*dense1.value[10]
    +dense0.value[6]*dense1.value[9]
    +dense0.value[7]*dense1.value[8]
    -dense0.value[8]*dense1.value[7]
    +dense0.value[9]*dense1.value[6]
    -dense0.value[10]*dense1.value[5]
    -dense0.value[11]*dense1.value[4]
    +dense0.value[12]*dense1.value[3]
    +dense0.value[13]*dense1.value[2]
    -dense0.value[14]*dense1.value[1]
    +dense0.value[15]*dense1.value[0]
    -dense0.value[16]*dense1.value[31]
    +dense0.value[17]*dense1.value[30]
    -dense0.value[18]*dense1.value[29]
    -dense0.value[19]*dense1.value[28]
    +dense0.value[20]*dense1.value[27]
    +dense0.value[21]*dense1.value[26]
    -dense0.value[22]*dense1.value[25]
    +dense0.value[23]*dense1.value[24]
    -dense0.value[24]*dense1.value[23]
    -dense0.value[25]*dense1.value[22]
    +dense0.value[26]*dense1.value[21]
    -dense0.value[27]*dense1.value[20]
    -dense0.value[28]*dense1.value[19]
    +dense0.value[29]*dense1.value[18]
    -dense0.value[30]*dense1.value[17]
    -dense0.value[31]*dense1.value[16]
;
    dense.value[16] =
    +dense0.value[0]*dense1.value[16]
    +dense0.value[1]*dense1.value[17]
    +dense0.value[2]*dense1.value[18]
    -dense0.value[3]*dense1.value[19]
    +dense0.value[4]*dense1.value[20]
    -dense0.value[5]*dense1.value[21]
    -dense0.value[6]*dense1.value[22]
    -dense0.value[7]*dense1.value[23]
    +dense0.value[8]*dense1.value[24]
    -dense0.value[9]*dense1.value[25]
    -dense0.value[10]*dense1.value[26]
    -dense0.value[11]*dense1.value[27]
    -dense0.value[12]*dense1.value[28]
    -dense0.value[13]*dense1.value[29]
    -dense0.value[14]*dense1.value[30]
    +dense0.value[15]*dense1.value[31]
    +dense0.value[16]*dense1.value[0]
    -dense0.value[17]*dense1.value[1]
    -dense0.value[18]*dense1.value[2]
    -dense0.value[19]*dense1.value[3]
    -dense0.value[20]*dense1.value[4]
    -dense0.value[21]*dense1.value[5]
    -dense0.value[22]*dense1.value[6]
    +dense0.value[23]*dense1.value[7]
    -dense0.value[24]*dense1.value[8]
    -dense0.value[25]*dense1.value[9]
    -dense0.value[26]*dense1.value[10]
    +dense0.value[27]*dense1.value[11]
    -dense0.value[28]*dense1.value[12]
    +dense0.value[29]*dense1.value[13]
    +dense0.value[30]*dense1.value[14]
    +dense0.value[31]*dense1.value[15]
;
    dense.value[17] =
    +dense0.value[0]*dense1.value[17]
    +dense0.value[1]*dense1.value[16]
    -dense0.value[2]*dense1.value[19]
    +dense0.value[3]*dense1.value[18]
    -dense0.value[4]*dense1.value[21]
    +dense0.value[5]*dense1.value[20]
    -dense0.value[6]*dense1.value[23]
    -dense0.value[7]*dense1.value[22]
    -dense0.value[8]*dense1.value[25]
    +dense0.value[9]*dense1.value[24]
    -dense0.value[10]*dense1.value[27]
    -dense0.value[11]*dense1.value[26]
    -dense0.value[12]*dense1.value[29]
    -dense0.value[13]*dense1.value[28]
    +dense0.value[14]*dense1.value[31]
    -dense0.value[15]*dense1.value[30]
    -dense0.value[16]*dense1.value[1]
    +dense0.value[17]*dense1.value[0]
    -dense0.value[18]*dense1.value[3]
    -dense0.value[19]*dense1.value[2]
    -dense0.value[20]*dense1.value[5]
    -dense0.value[21]*dense1.value[4]
    +dense0.value[22]*dense1.value[7]
    -dense0.value[23]*dense1.value[6]
    -dense0.value[24]*dense1.value[9]
    -dense0.value[25]*dense1.value[8]
    +dense0.value[26]*dense1.value[11]
    -dense0.value[27]*dense1.value[10]
    +dense0.value[28]*dense1.value[13]
    -dense0.value[29]*dense1.value[12]
    +dense0.value[30]*dense1.value[15]
    +dense0.value[31]*dense1.value[14]
;
    dense.value[18] =
    +dense0.value[0]*dense1.value[18]
    +dense0.value[1]*dense1.value[19]
    +dense0.value[2]*dense1.value[16]
    -dense0.value[3]*dense1.value[17]
    -dense0.value[4]*dense1.value[22]
    +dense0.value[5]*dense1.value[23]
    +dense0.value[6]*dense1.value[20]
    +dense0.value[7]*dense1.value[21]
    -dense0.value[8]*dense1.value[26]
    +dense0.value[9]*dense1.value[27]
    +dense0.value[10]*dense1.value[24]
    +dense0.value[11]*dense1.value[25]
    -dense0.value[12]*dense1.value[30]
    -dense0.value[13]*dense1.value[31]
    -dense0.value[14]*dense1.value[28]
    +dense0.value[15]*dense1.value[29]
    -dense0.value[16]*dense1.value[2]
    +dense0.value[17]*dense1.value[3]
    +dense0.value[18]*dense1.value[0]
    +dense0.value[19]*dense1.value[1]
    -dense0.value[20]*dense1.value[6]
    -dense0.value[21]*dense1.value[7]
    -dense0.value[22]*dense1.value[4]
    +dense0.value[23]*dense1.value[5]
    -dense0.value[24]*dense1.value[10]
    -dense0.value[25]*dense1.value[11]
    -dense0.value[26]*dense1.value[8]
    +dense0.value[27]*dense1.value[9]
    +dense0.value[28]*dense1.value[14]
    -dense0.value[29]*dense1.value[15]
    -dense0.value[30]*dense1.value[12]
    -dense0.value[31]*dense1.value[13]
;
    dense.value[19] =
    +dense0.value[0]*dense1.value[19]
    +dense0.value[1]*dense1.value[18]
    -dense0.value[2]*dense1.value[17]
    +dense0.value[3]*dense1.value[16]
    +dense0.value[4]*dense1.value[23]
    -dense0.value[5]*dense1.value[22]
    +dense0.value[6]*dense1.value[21]
    +dense0.value[7]*dense1.value[20]
    +dense0.value[8]*dense1.value[27]
    -dense0.value[9]*dense1.value[26]
    +dense0.value[10]*dense1.value[25]
    +dense0.value[11]*dense1.value[24]
    -dense0.value[12]*dense1.value[31]
    -dense0.value[13]*dense1.value[30]
    +dense0.value[14]*dense1.value[29]
    -dense0.value[15]*dense1.value[28]
    +dense0.value[16]*dense1.value[3]
    -dense0.value[17]*dense1.value[2]
    +dense0.value[18]*dense1.value[1]
    +dense0.value[19]*dense1.value[0]
    -dense0.value[20]*dense1.value[7]
    -dense0.value[21]*dense1.value[6]
    +dense0.value[22]*dense1.value[5]
    -dense0.value[23]*dense1.value[4]
    -dense0.value[24]*dense1.value[11]
    -dense0.value[25]*dense1.value[10]
    +dense0.value[26]*dense1.value[9]
    -dense0.value[27]*dense1.value[8]
    -dense0.value[28]*dense1.value[15]
    +dense0.value[29]*dense1.value[14]
    -dense0.value[30]*dense1.value[13]
    -dense0.value[31]*dense1.value[12]
;
    dense.value[20] =
    +dense0.value[0]*dense1.value[20]
    +dense0.value[1]*dense1.value[21]
    +dense0.value[2]*dense1.value[22]
    -dense0.value[3]*dense1.value[23]
    +dense0.value[4]*dense1.value[16]
    -dense0.value[5]*dense1.value[17]
    -dense0.value[6]*dense1.value[18]
    -dense0.value[7]*dense1.value[19]
    -dense0.value[8]*dense1.value[28]
    +dense0.value[9]*dense1.value[29]
    +dense0.value[10]*dense1.value[30]
    +dense0.value[11]*dense1.value[31]
    +dense0.value[12]*dense1.value[24]
    +dense0.value[13]*dense1.value[25]
    +dense0.value[14]*dense1.value[26]
    -dense0.value[15]*dense1.value[27]
    -dense0.value[16]*dense1.value[4]
    +dense0.value[17]*dense1.value[5]
    +dense0.value[18]*dense1.value[6]
    +dense0.value[19]*dense1.value[7]
    +dense0.value[20]*dense1.value[0]
    +dense0.value[21]*dense1.value[1]
    +dense0.value[22]*dense1.value[2]
    -dense0.value[23]*dense1.value[3]
    -dense0.value[24]*dense1.value[12]
    -dense0.value[25]*dense1.value[13]
    -dense0.value[26]*dense1.value[14]
    +dense0.value[27]*dense1.value[15]
    -dense0.value[28]*dense1.value[8]
    +dense0.value[29]*dense1.value[9]
    +dense0.value[30]*dense1.value[10]
    +dense0.value[31]*dense1.value[11]
;
    dense.value[21] =
    +dense0.value[0]*dense1.value[21]
    +dense0.value[1]*dense1.value[20]
    -dense0.value[2]*dense1.value[23]
    +dense0.value[3]*dense1.value[22]
    -dense0.value[4]*dense1.value[17]
    +dense0.value[5]*dense1.value[16]
    -dense0.value[6]*dense1.value[19]
    -dense0.value[7]*dense1.value[18]
    +dense0.value[8]*dense1.value[29]
    -dense0.value[9]*dense1.value[28]
    +dense0.value[10]*dense1.value[31]
    +dense0.value[11]*dense1.value[30]
    +dense0.value[12]*dense1.value[25]
    +dense0.value[13]*dense1.value[24]
    -dense0.value[14]*dense1.value[27]
    +dense0.value[15]*dense1.value[26]
    +dense0.value[16]*dense1.value[5]
    -dense0.value[17]*dense1.value[4]
    +dense0.value[18]*dense1.value[7]
    +dense0.value[19]*dense1.value[6]
    +dense0.value[20]*dense1.value[1]
    +dense0.value[21]*dense1.value[0]
    -dense0.value[22]*dense1.value[3]
    +dense0.value[23]*dense1.value[2]
    -dense0.value[24]*dense1.value[13]
    -dense0.value[25]*dense1.value[12]
    +dense0.value[26]*dense1.value[15]
    -dense0.value[27]*dense1.value[14]
    +dense0.value[28]*dense1.value[9]
    -dense0.value[29]*dense1.value[8]
    +dense0.value[30]*dense1.value[11]
    +dense0.value[31]*dense1.value[10]
;
    dense.value[22] =
    +dense0.value[0]*dense1.value[22]
    +dense0.value[1]*dense1.value[23]
    +dense0.value[2]*dense1.value[20]
    -dense0.value[3]*dense1.value[21]
    -dense0.value[4]*dense1.value[18]
    +dense0.value[5]*dense1.value[19]
    +dense0.value[6]*dense1.value[16]
    +dense0.value[7]*dense1.value[17]
    +dense0.value[8]*dense1.value[30]
    -dense0.value[9]*dense1.value[31]
    -dense0.value[10]*dense1.value[28]
    -dense0.value[11]*dense1.value[29]
    +dense0.value[12]*dense1.value[26]
    +dense0.value[13]*dense1.value[27]
    +dense0.value[14]*dense1.value[24]
    -dense0.value[15]*dense1.value[25]
    +dense0.value[16]*dense1.value[6]
    -dense0.value[17]*dense1.value[7]
    -dense0.value[18]*dense1.value[4]
    -dense0.value[19]*dense1.value[5]
    +dense0.value[20]*dense1.value[2]
    +dense0.value[21]*dense1.value[3]
    +dense0.value[22]*dense1.value[0]
    -dense0.value[23]*dense1.value[1]
    -dense0.value[24]*dense1.value[14]
    -dense0.value[25]*dense1.value[15]
    -dense0.value[26]*dense1.value[12]
    +dense0.value[27]*dense1.value[13]
    +dense0.value[28]*dense1.value[10]
    -dense0.value[29]*dense1.value[11]
    -dense0.value[30]*dense1.value[8]
    -dense0.value[31]*dense1.value[9]
;
    dense.value[23] =
    +dense0.value[0]*dense1.value[23]
    +dense0.value[1]*dense1.value[22]
    -dense0.value[2]*dense1.value[21]
    +dense0.value[3]*dense1.value[20]
    +dense0.value[4]*dense1.value[19]
    -dense0.value[5]*dense1.value[18]
    +dense0.value[6]*dense1.value[17]
    +dense0.value[7]*dense1.value[16]
    -dense0.value[8]*dense1.value[31]
    +dense0.value[9]*dense1.value[30]
    -dense0.value[10]*dense1.value[29]
    -dense0.value[11]*dense1.value[28]
    +dense0.value[12]*dense1.value[27]
    +dense0.value[13]*dense1.value[26]
    -dense0.value[14]*dense1.value[25]
    +dense0.value[15]*dense1.value[24]
    -dense0.value[16]*dense1.value[7]
    +dense0.value[17]*dense1.value[6]
    -dense0.value[18]*dense1.value[5]
    -dense0.value[19]*dense1.value[4]
    +dense0.value[20]*dense1.value[3]
    +dense0.value[21]*dense1.value[2]
    -dense0.value[22]*dense1.value[1]
    +dense0.value[23]*dense1.value[0]
    -dense0.value[24]*dense1.value[15]
    -dense0.value[25]*dense1.value[14]
    +dense0.value[26]*dense1.value[13]
    -dense0.value[27]*dense1.value[12]
    -dense0.value[28]*dense1.value[11]
    +dense0.value[29]*dense1.value[10]
    -dense0.value[30]*dense1.value[9]
    -dense0.value[31]*dense1.value[8]
;
    dense.value[24] =
    +dense0.value[0]*dense1.value[24]
    +dense0.value[1]*dense1.value[25]
    +dense0.value[2]*dense1.value[26]
    -dense0.value[3]*dense1.value[27]
    +dense0.value[4]*dense1.value[28]
    -dense0.value[5]*dense1.value[29]
    -dense0.value[6]*dense1.value[30]
    -dense0.value[7]*dense1.value[31]
    +dense0.value[8]*dense1.value[16]
    -dense0.value[9]*dense1.value[17]
    -dense0.value[10]*dense1.value[18]
    -dense0.value[11]*dense1.value[19]
    -dense0.value[12]*dense1.value[20]
    -dense0.value[13]*dense1.value[21]
    -dense0.value[14]*dense1.value[22]
    +dense0.value[15]*dense1.value[23]
    -dense0.value[16]*dense1.value[8]
    +dense0.value[17]*dense1.value[9]
    +dense0.value[18]*dense1.value[10]
    +dense0.value[19]*dense1.value[11]
    +dense0.value[20]*dense1.value[12]
    +dense0.value[21]*dense1.value[13]
    +dense0.value[22]*dense1.value[14]
    -dense0.value[23]*dense1.value[15]
    +dense0.value[24]*dense1.value[0]
    +dense0.value[25]*dense1.value[1]
    +dense0.value[26]*dense1.value[2]
    -dense0.value[27]*dense1.value[3]
    +dense0.value[28]*dense1.value[4]
    -dense0.value[29]*dense1.value[5]
    -dense0.value[30]*dense1.value[6]
    -dense0.value[31]*dense1.value[7]
;
    dense.value[25] =
    +dense0.value[0]*dense1.value[25]
    +dense0.value[1]*dense1.value[24]
    -dense0.value[2]*dense1.value[27]
    +dense0.value[3]*dense1.value[26]
    -dense0.value[4]*dense1.value[29]
    +dense0.value[5]*dense1.value[28]
    -dense0.value[6]*dense1.value[31]
    -dense0.value[7]*dense1.value[30]
    -dense0.value[8]*dense1.value[17]
    +dense0.value[9]*dense1.value[16]
    -dense0.value[10]*dense1.value[19]
    -dense0.value[11]*dense1.value[18]
    -dense0.value[12]*dense1.value[21]
    -dense0.value[13]*dense1.value[20]
    +dense0.value[14]*dense1.value[23]
    -dense0.value[15]*dense1.value[22]
    +dense0.value[16]*dense1.value[9]
    -dense0.value[17]*dense1.value[8]
    +dense0.value[18]*dense1.value[11]
    +dense0.value[19]*dense1.value[10]
    +dense0.value[20]*dense1.value[13]
    +dense0.value[21]*dense1.value[12]
    -dense0.value[22]*dense1.value[15]
    +dense0.value[23]*dense1.value[14]
    +dense0.value[24]*dense1.value[1]
    +dense0.value[25]*dense1.value[0]
    -dense0.value[26]*dense1.value[3]
    +dense0.value[27]*dense1.value[2]
    -dense0.value[28]*dense1.value[5]
    +dense0.value[29]*dense1.value[4]
    -dense0.value[30]*dense1.value[7]
    -dense0.value[31]*dense1.value[6]
;
    dense.value[26] =
    +dense0.value[0]*dense1.value[26]
    +dense0.value[1]*dense1.value[27]
    +dense0.value[2]*dense1.value[24]
    -dense0.value[3]*dense1.value[25]
    -dense0.value[4]*dense1.value[30]
    +dense0.value[5]*dense1.value[31]
    +dense0.value[6]*dense1.value[28]
    +dense0.value[7]*dense1.value[29]
    -dense0.value[8]*dense1.value[18]
    +dense0.value[9]*dense1.value[19]
    +dense0.value[10]*dense1.value[16]
    +dense0.value[11]*dense1.value[17]
    -dense0.value[12]*dense1.value[22]
    -dense0.value[13]*dense1.value[23]
    -dense0.value[14]*dense1.value[20]
    +dense0.value[15]*dense1.value[21]
    +dense0.value[16]*dense1.value[10]
    -dense0.value[17]*dense1.value[11]
    -dense0.value[18]*dense1.value[8]
    -dense0.value[19]*dense1.value[9]
    +dense0.value[20]*dense1.value[14]
    +dense0.value[21]*dense1.value[15]
    +dense0.value[22]*dense1.value[12]
    -dense0.value[23]*dense1.value[13]
    +dense0.value[24]*dense1.value[2]
    +dense0.value[25]*dense1.value[3]
    +dense0.value[26]*dense1.value[0]
    -dense0.value[27]*dense1.value[1]
    -dense0.value[28]*dense1.value[6]
    +dense0.value[29]*dense1.value[7]
    +dense0.value[30]*dense1.value[4]
    +dense0.value[31]*dense1.value[5]
;
    dense.value[27] =
    +dense0.value[0]*dense1.value[27]
    +dense0.value[1]*dense1.value[26]
    -dense0.value[2]*dense1.value[25]
    +dense0.value[3]*dense1.value[24]
    +dense0.value[4]*dense1.value[31]
    -dense0.value[5]*dense1.value[30]
    +dense0.value[6]*dense1.value[29]
    +dense0.value[7]*dense1.value[28]
    +dense0.value[8]*dense1.value[19]
    -dense0.value[9]*dense1.value[18]
    +dense0.value[10]*dense1.value[17]
    +dense0.value[11]*dense1.value[16]
    -dense0.value[12]*dense1.value[23]
    -dense0.value[13]*dense1.value[22]
    +dense0.value[14]*dense1.value[21]
    -dense0.value[15]*dense1.value[20]
    -dense0.value[16]*dense1.value[11]
    +dense0.value[17]*dense1.value[10]
    -dense0.value[18]*dense1.value[9]
    -dense0.value[19]*dense1.value[8]
    +dense0.value[20]*dense1.value[15]
    +dense0.value[21]*dense1.value[14]
    -dense0.value[22]*dense1.value[13]
    +dense0.value[23]*dense1.value[12]
    +dense0.value[24]*dense1.value[3]
    +dense0.value[25]*dense1.value[2]
    -dense0.value[26]*dense1.value[1]
    +dense0.value[27]*dense1.value[0]
    +dense0.value[28]*dense1.value[7]
    -dense0.value[29]*dense1.value[6]
    +dense0.value[30]*dense1.value[5]
    +dense0.value[31]*dense1.value[4]
;
    dense.value[28] =
    +dense0.value[0]*dense1.value[28]
    +dense0.value[1]*dense1.value[29]
    +dense0.value[2]*dense1.value[30]
    -dense0.value[3]*dense1.value[31]
    +dense0.value[4]*dense1.value[24]
    -dense0.value[5]*dense1.value[25]
    -dense0.value[6]*dense1.value[26]
    -dense0.value[7]*dense1.value[27]
    -dense0.value[8]*dense1.value[20]
    +dense0.value[9]*dense1.value[21]
    +dense0.value[10]*dense1.value[22]
    +dense0.value[11]*dense1.value[23]
    +dense0.value[12]*dense1.value[16]
    +dense0.value[13]*dense1.value[17]
    +dense0.value[14]*dense1.value[18]
    -dense0.value[15]*dense1.value[19]
    +dense0.value[16]*dense1.value[12]
    -dense0.value[17]*dense1.value[13]
    -dense0.value[18]*dense1.value[14]
    -dense0.value[19]*dense1.value[15]
    -dense0.value[20]*dense1.value[8]
    -dense0.value[21]*dense1.value[9]
    -dense0.value[22]*dense1.value[10]
    +dense0.value[23]*dense1.value[11]
    +dense0.value[24]*dense1.value[4]
    +dense0.value[25]*dense1.value[5]
    +dense0.value[26]*dense1.value[6]
    -dense0.value[27]*dense1.value[7]
    +dense0.value[28]*dense1.value[0]
    -dense0.value[29]*dense1.value[1]
    -dense0.value[30]*dense1.value[2]
    -dense0.value[31]*dense1.value[3]
;
    dense.value[29] =
    +dense0.value[0]*dense1.value[29]
    +dense0.value[1]*dense1.value[28]
    -dense0.value[2]*dense1.value[31]
    +dense0.value[3]*dense1.value[30]
    -dense0.value[4]*dense1.value[25]
    +dense0.value[5]*dense1.value[24]
    -dense0.value[6]*dense1.value[27]
    -dense0.value[7]*dense1.value[26]
    +dense0.value[8]*dense1.value[21]
    -dense0.value[9]*dense1.value[20]
    +dense0.value[10]*dense1.value[23]
    +dense0.value[11]*dense1.value[22]
    +dense0.value[12]*dense1.value[17]
    +dense0.value[13]*dense1.value[16]
    -dense0.value[14]*dense1.value[19]
    +dense0.value[15]*dense1.value[18]
    -dense0.value[16]*dense1.value[13]
    +dense0.value[17]*dense1.value[12]
    -dense0.value[18]*dense1.value[15]
    -dense0.value[19]*dense1.value[14]
    -dense0.value[20]*dense1.value[9]
    -dense0.value[21]*dense1.value[8]
    +dense0.value[22]*dense1.value[11]
    -dense0.value[23]*dense1.value[10]
    +dense0.value[24]*dense1.value[5]
    +dense0.value[25]*dense1.value[4]
    -dense0.value[26]*dense1.value[7]
    +dense0.value[27]*dense1.value[6]
    -dense0.value[28]*dense1.value[1]
    +dense0.value[29]*dense1.value[0]
    -dense0.value[30]*dense1.value[3]
    -dense0.value[31]*dense1.value[2]
;
    dense.value[30] =
    +dense0.value[0]*dense1.value[30]
    +dense0.value[1]*dense1.value[31]
    +dense0.value[2]*dense1.value[28]
    -dense0.value[3]*dense1.value[29]
    -dense0.value[4]*dense1.value[26]
    +dense0.value[5]*dense1.value[27]
    +dense0.value[6]*dense1.value[24]
    +dense0.value[7]*dense1.value[25]
    +dense0.value[8]*dense1.value[22]
    -dense0.value[9]*dense1.value[23]
    -dense0.value[10]*dense1.value[20]
    -dense0.value[11]*dense1.value[21]
    +dense0.value[12]*dense1.value[18]
    +dense0.value[13]*dense1.value[19]
    +dense0.value[14]*dense1.value[16]
    -dense0.value[15]*dense1.value[17]
    -dense0.value[16]*dense1.value[14]
    +dense0.value[17]*dense1.value[15]
    +dense0.value[18]*dense1.value[12]
    +dense0.value[19]*dense1.value[13]
    -dense0.value[20]*dense1.value[10]
    -dense0.value[21]*dense1.value[11]
    -dense0.value[22]*dense1.value[8]
    +dense0.value[23]*dense1.value[9]
    +dense0.value[24]*dense1.value[6]
    +dense0.value[25]*dense1.value[7]
    +dense0.value[26]*dense1.value[4]
    -dense0.value[27]*dense1.value[5]
    -dense0.value[28]*dense1.value[2]
    +dense0.value[29]*dense1.value[3]
    +dense0.value[30]*dense1.value[0]
    +dense0.value[31]*dense1.value[1]
;
    dense.value[31] =
    +dense0.value[0]*dense1.value[31]
    +dense0.value[1]*dense1.value[30]
    -dense0.value[2]*dense1.value[29]
    +dense0.value[3]*dense1.value[28]
    +dense0.value[4]*dense1.value[27]
    -dense0.value[5]*dense1.value[26]
    +dense0.value[6]*dense1.value[25]
    +dense0.value[7]*dense1.value[24]
    -dense0.value[8]*dense1.value[23]
    +dense0.value[9]*dense1.value[22]
    -dense0.value[10]*dense1.value[21]
    -dense0.value[11]*dense1.value[20]
    +dense0.value[12]*dense1.value[19]
    +dense0.value[13]*dense1.value[18]
    -dense0.value[14]*dense1.value[17]
    +dense0.value[15]*dense1.value[16]
    +dense0.value[16]*dense1.value[15]
    -dense0.value[17]*dense1.value[14]
    +dense0.value[18]*dense1.value[13]
    +dense0.value[19]*dense1.value[12]
    -dense0.value[20]*dense1.value[11]
    -dense0.value[21]*dense1.value[10]
    +dense0.value[22]*dense1.value[9]
    -dense0.value[23]*dense1.value[8]
    +dense0.value[24]*dense1.value[7]
    +dense0.value[25]*dense1.value[6]
    -dense0.value[26]*dense1.value[5]
    +dense0.value[27]*dense1.value[4]
    +dense0.value[28]*dense1.value[3]
    -dense0.value[29]*dense1.value[2]
    +dense0.value[30]*dense1.value[1]
    +dense0.value[31]*dense1.value[0]
;
    return dense;
}

// grade projection of the product of two multivectors <ab>_r
#define GEN1_DENSE_GRADE0GEOMETRICPRODUCT(dense,dense0,dense1){\
    dense.value[0] =\
    +dense0.value[0]*dense1.value[0]\
    +dense0.value[1]*dense1.value[1]\
    +dense0.value[2]*dense1.value[2]\
    -dense0.value[3]*dense1.value[3]\
    +dense0.value[4]*dense1.value[4]\
    -dense0.value[5]*dense1.value[5]\
    -dense0.value[6]*dense1.value[6]\
    -dense0.value[7]*dense1.value[7]\
    +dense0.value[8]*dense1.value[8]\
    -dense0.value[9]*dense1.value[9]\
    -dense0.value[10]*dense1.value[10]\
    -dense0.value[11]*dense1.value[11]\
    -dense0.value[12]*dense1.value[12]\
    -dense0.value[13]*dense1.value[13]\
    -dense0.value[14]*dense1.value[14]\
    +dense0.value[15]*dense1.value[15]\
    -dense0.value[16]*dense1.value[16]\
    +dense0.value[17]*dense1.value[17]\
    +dense0.value[18]*dense1.value[18]\
    +dense0.value[19]*dense1.value[19]\
    +dense0.value[20]*dense1.value[20]\
    +dense0.value[21]*dense1.value[21]\
    +dense0.value[22]*dense1.value[22]\
    -dense0.value[23]*dense1.value[23]\
    +dense0.value[24]*dense1.value[24]\
    +dense0.value[25]*dense1.value[25]\
    +dense0.value[26]*dense1.value[26]\
    -dense0.value[27]*dense1.value[27]\
    +dense0.value[28]*dense1.value[28]\
    -dense0.value[29]*dense1.value[29]\
    -dense0.value[30]*dense1.value[30]\
    -dense0.value[31]*dense1.value[31]\
;\
}
#define GEN1_DENSE_GRADE1GEOMETRICPRODUCT(dense,dense0,dense1){\
    dense.value[1] =\
    +dense0.value[0]*dense1.value[1]\
    +dense0.value[1]*dense1.value[0]\
    -dense0.value[2]*dense1.value[3]\
    +dense0.value[3]*dense1.value[2]\
    -dense0.value[4]*dense1.value[5]\
    +dense0.value[5]*dense1.value[4]\
    -dense0.value[6]*dense1.value[7]\
    -dense0.value[7]*dense1.value[6]\
    -dense0.value[8]*dense1.value[9]\
    +dense0.value[9]*dense1.value[8]\
    -dense0.value[10]*dense1.value[11]\
    -dense0.value[11]*dense1.value[10]\
    -dense0.value[12]*dense1.value[13]\
    -dense0.value[13]*dense1.value[12]\
    +dense0.value[14]*dense1.value[15]\
    -dense0.value[15]*dense1.value[14]\
    +dense0.value[16]*dense1.value[17]\
    -dense0.value[17]*dense1.value[16]\
    +dense0.value[18]*dense1.value[19]\
    +dense0.value[19]*dense1.value[18]\
    +dense0.value[20]*dense1.value[21]\
    +dense0.value[21]*dense1.value[20]\
    -dense0.value[22]*dense1.value[23]\
    +dense0.value[23]*dense1.value[22]\
    +dense0.value[24]*dense1.value[25]\
    +dense0.value[25]*dense1.value[24]\
    -dense0.value[26]*dense1.value[27]\
    +dense0.value[27]*dense1.value[26]\
    -dense0.value[28]*dense1.value[29]\
    +dense0.value[29]*dense1.value[28]\
    -dense0.value[30]*dense1.value[31]\
    -dense0.value[31]*dense1.value[30]\
;\
    dense.value[2] =\
    +dense0.value[0]*dense1.value[2]\
    +dense0.value[1]*dense1.value[3]\
    +dense0.value[2]*dense1.value[0]\
    -dense0.value[3]*dense1.value[1]\
    -dense0.value[4]*dense1.value[6]\
    +dense0.value[5]*dense1.value[7]\
    +dense0.value[6]*dense1.value[4]\
    +dense0.value[7]*dense1.value[5]\
    -dense0.value[8]*dense1.value[10]\
    +dense0.value[9]*dense1.value[11]\
    +dense0.value[10]*dense1.value[8]\
    +dense0.value[11]*dense1.value[9]\
    -dense0.value[12]*dense1.value[14]\
    -dense0.value[13]*dense1.value[15]\
    -dense0.value[14]*dense1.value[12]\
    +dense0.value[15]*dense1.value[13]\
    +dense0.value[16]*dense1.value[18]\
    -dense0.value[17]*dense1.value[19]\
    -dense0.value[18]*dense1.value[16]\
    -dense0.value[19]*dense1.value[17]\
    +dense0.value[20]*dense1.value[22]\
    +dense0.value[21]*dense1.value[23]\
    +dense0.value[22]*dense1.value[20]\
    -dense0.value[23]*dense1.value[21]\
    +dense0.value[24]*dense1.value[26]\
    +dense0.value[25]*dense1.value[27]\
    +dense0.value[26]*dense1.value[24]\
    -dense0.value[27]*dense1.value[25]\
    -dense0.value[28]*dense1.value[30]\
    +dense0.value[29]*dense1.value[31]\
    +dense0.value[30]*dense1.value[28]\
    +dense0.value[31]*dense1.value[29]\
;\
    dense.value[4] =\
    +dense0.value[0]*dense1.value[4]\
    +dense0.value[1]*dense1.value[5]\
    +dense0.value[2]*dense1.value[6]\
    -dense0.value[3]*dense1.value[7]\
    +dense0.value[4]*dense1.value[0]\
    -dense0.value[5]*dense1.value[1]\
    -dense0.value[6]*dense1.value[2]\
    -dense0.value[7]*dense1.value[3]\
    -dense0.value[8]*dense1.value[12]\
    +dense0.value[9]*dense1.value[13]\
    +dense0.value[10]*dense1.value[14]\
    +dense0.value[11]*dense1.value[15]\
    +dense0.value[12]*dense1.value[8]\
    +dense0.value[13]*dense1.value[9]\
    +dense0.value[14]*dense1.value[10]\
    -dense0.value[15]*dense1.value[11]\
    +dense0.value[16]*dense1.value[20]\
    -dense0.value[17]*dense1.value[21]\
    -dense0.value[18]*dense1.value[22]\
    -dense0.value[19]*dense1.value[23]\
    -dense0.value[20]*dense1.value[16]\
    -dense0.value[21]*dense1.value[17]\
    -dense0.value[22]*dense1.value[18]\
    +dense0.value[23]*dense1.value[19]\
    +dense0.value[24]*dense1.value[28]\
    +dense0.value[25]*dense1.value[29]\
    +dense0.value[26]*dense1.value[30]\
    -dense0.value[27]*dense1.value[31]\
    +dense0.value[28]*dense1.value[24]\
    -dense0.value[29]*dense1.value[25]\
    -dense0.value[30]*dense1.value[26]\
    -dense0.value[31]*dense1.value[27]\
;\
    dense.value[8] =\
    +dense0.value[0]*dense1.value[8]\
    +dense0.value[1]*dense1.value[9]\
    +dense0.value[2]*dense1.value[10]\
    -dense0.value[3]*dense1.value[11]\
    +dense0.value[4]*dense1.value[12]\
    -dense0.value[5]*dense1.value[13]\
    -dense0.value[6]*dense1.value[14]\
    -dense0.value[7]*dense1.value[15]\
    +dense0.value[8]*dense1.value[0]\
    -dense0.value[9]*dense1.value[1]\
    -dense0.value[10]*dense1.value[2]\
    -dense0.value[11]*dense1.value[3]\
    -dense0.value[12]*dense1.value[4]\
    -dense0.value[13]*dense1.value[5]\
    -dense0.value[14]*dense1.value[6]\
    +dense0.value[15]*dense1.value[7]\
    +dense0.value[16]*dense1.value[24]\
    -dense0.value[17]*dense1.value[25]\
    -dense0.value[18]*dense1.value[26]\
    -dense0.value[19]*dense1.value[27]\
    -dense0.value[20]*dense1.value[28]\
    -dense0.value[21]*dense1.value[29]\
    -dense0.value[22]*dense1.value[30]\
    +dense0.value[23]*dense1.value[31]\
    -dense0.value[24]*dense1.value[16]\
    -dense0.value[25]*dense1.value[17]\
    -dense0.value[26]*dense1.value[18]\
    +dense0.value[27]*dense1.value[19]\
    -dense0.value[28]*dense1.value[20]\
    +dense0.value[29]*dense1.value[21]\
    +dense0.value[30]*dense1.value[22]\
    +dense0.value[31]*dense1.value[23]\
;\
    dense.value[16] =\
    +dense0.value[0]*dense1.value[16]\
    +dense0.value[1]*dense1.value[17]\
    +dense0.value[2]*dense1.value[18]\
    -dense0.value[3]*dense1.value[19]\
    +dense0.value[4]*dense1.value[20]\
    -dense0.value[5]*dense1.value[21]\
    -dense0.value[6]*dense1.value[22]\
    -dense0.value[7]*dense1.value[23]\
    +dense0.value[8]*dense1.value[24]\
    -dense0.value[9]*dense1.value[25]\
    -dense0.value[10]*dense1.value[26]\
    -dense0.value[11]*dense1.value[27]\
    -dense0.value[12]*dense1.value[28]\
    -dense0.value[13]*dense1.value[29]\
    -dense0.value[14]*dense1.value[30]\
    +dense0.value[15]*dense1.value[31]\
    +dense0.value[16]*dense1.value[0]\
    -dense0.value[17]*dense1.value[1]\
    -dense0.value[18]*dense1.value[2]\
    -dense0.value[19]*dense1.value[3]\
    -dense0.value[20]*dense1.value[4]\
    -dense0.value[21]*dense1.value[5]\
    -dense0.value[22]*dense1.value[6]\
    +dense0.value[23]*dense1.value[7]\
    -dense0.value[24]*dense1.value[8]\
    -dense0.value[25]*dense1.value[9]\
    -dense0.value[26]*dense1.value[10]\
    +dense0.value[27]*dense1.value[11]\
    -dense0.value[28]*dense1.value[12]\
    +dense0.value[29]*dense1.value[13]\
    +dense0.value[30]*dense1.value[14]\
    +dense0.value[31]*dense1.value[15]\
;\
}
#define GEN1_DENSE_GRADE2GEOMETRICPRODUCT(dense,dense0,dense1){\
    dense.value[3] =\
    +dense0.value[0]*dense1.value[3]\
    +dense0.value[1]*dense1.value[2]\
    -dense0.value[2]*dense1.value[1]\
    +dense0.value[3]*dense1.value[0]\
    +dense0.value[4]*dense1.value[7]\
    -dense0.value[5]*dense1.value[6]\
    +dense0.value[6]*dense1.value[5]\
    +dense0.value[7]*dense1.value[4]\
    +dense0.value[8]*dense1.value[11]\
    -dense0.value[9]*dense1.value[10]\
    +dense0.value[10]*dense1.value[9]\
    +dense0.value[11]*dense1.value[8]\
    -dense0.value[12]*dense1.value[15]\
    -dense0.value[13]*dense1.value[14]\
    +dense0.value[14]*dense1.value[13]\
    -dense0.value[15]*dense1.value[12]\
    -dense0.value[16]*dense1.value[19]\
    +dense0.value[17]*dense1.value[18]\
    -dense0.value[18]*dense1.value[17]\
    -dense0.value[19]*dense1.value[16]\
    +dense0.value[20]*dense1.value[23]\
    +dense0.value[21]*dense1.value[22]\
    -dense0.value[22]*dense1.value[21]\
    +dense0.value[23]*dense1.value[20]\
    +dense0.value[24]*dense1.value[27]\
    +dense0.value[25]*dense1.value[26]\
    -dense0.value[26]*dense1.value[25]\
    +dense0.value[27]*dense1.value[24]\
    +dense0.value[28]*dense1.value[31]\
    -dense0.value[29]*dense1.value[30]\
    +dense0.value[30]*dense1.value[29]\
    +dense0.value[31]*dense1.value[28]\
;\
    dense.value[5] =\
    +dense0.value[0]*dense1.value[5]\
    +dense0.value[1]*dense1.value[4]\
    -dense0.value[2]*dense1.value[7]\
    +dense0.value[3]*dense1.value[6]\
    -dense0.value[4]*dense1.value[1]\
    +dense0.value[5]*dense1.value[0]\
    -dense0.value[6]*dense1.value[3]\
    -dense0.value[7]*dense1.value[2]\
    +dense0.value[8]*dense1.value[13]\
    -dense0.value[9]*dense1.value[12]\
    +dense0.value[10]*dense1.value[15]\
    +dense0.value[11]*dense1.value[14]\
    +dense0.value[12]*dense1.value[9]\
    +dense0.value[13]*dense1.value[8]\
    -dense0.value[14]*dense1.value[11]\
    +dense0.value[15]*dense1.value[10]\
    -dense0.value[16]*dense1.value[21]\
    +dense0.value[17]*dense1.value[20]\
    -dense0.value[18]*dense1.value[23]\
    -dense0.value[19]*dense1.value[22]\
    -dense0.value[20]*dense1.value[17]\
    -dense0.value[21]*dense1.value[16]\
    +dense0.value[22]*dense1.value[19]\
    -dense0.value[23]*dense1.value[18]\
    +dense0.value[24]*dense1.value[29]\
    +dense0.value[25]*dense1.value[28]\
    -dense0.value[26]*dense1.value[31]\
    +dense0.value[27]*dense1.value[30]\
    -dense0.value[28]*dense1.value[25]\
    +dense0.value[29]*dense1.value[24]\
    -dense0.value[30]*dense1.value[27]\
    -dense0.value[31]*dense1.value[26]\
;\
    dense.value[6] =\
    +dense0.value[0]*dense1.value[6]\
    +dense0.value[1]*dense1.value[7]\
    +dense0.value[2]*dense1.value[4]\
    -dense0.value[3]*dense1.value[5]\
    -dense0.value[4]*dense1.value[2]\
    +dense0.value[5]*dense1.value[3]\
    +dense0.value[6]*dense1.value[0]\
    +dense0.value[7]*dense1.value[1]\
    +dense0.value[8]*dense1.value[14]\
    -dense0.value[9]*dense1.value[15]\
    -dense0.value[10]*dense1.value[12]\
    -dense0.value[11]*dense1.value[13]\
    +dense0.value[12]*dense1.value[10]\
    +dense0.value[13]*dense1.value[11]\
    +dense0.value[14]*dense1.value[8]\
    -dense0.value[15]*dense1.value[9]\
    -dense0.value[16]*dense1.value[22]\
    +dense0.value[17]*dense1.value[23]\
    +dense0.value[18]*dense1.value[20]\
    +dense0.value[19]*dense1.value[21]\
    -dense0.value[20]*dense1.value[18]\
    -dense0.value[21]*dense1.value[19]\
    -dense0.value[22]*dense1.value[16]\
    +dense0.value[23]*dense1.value[17]\
    +dense0.value[24]*dense1.value[30]\
    +dense0.value[25]*dense1.value[31]\
    +dense0.value[26]*dense1.value[28]\
    -dense0.value[27]*dense1.value[29]\
    -dense0.value[28]*dense1.value[26]\
    +dense0.value[29]*dense1.value[27]\
    +dense0.value[30]*dense1.value[24]\
    +dense0.value[31]*dense1.value[25]\
;\
    dense.value[9] =\
    +dense0.value[0]*dense1.value[9]\
    +dense0.value[1]*dense1.value[8]\
    -dense0.value[2]*dense1.value[11]\
    +dense0.value[3]*dense1.value[10]\
    -dense0.value[4]*dense1.value[13]\
    +dense0.value[5]*dense1.value[12]\
    -dense0.value[6]*dense1.value[15]\
    -dense0.value[7]*dense1.value[14]\
    -dense0.value[8]*dense1.value[1]\
    +dense0.value[9]*dense1.value[0]\
    -dense0.value[10]*dense1.value[3]\
    -dense0.value[11]*dense1.value[2]\
    -dense0.value[12]*dense1.value[5]\
    -dense0.value[13]*dense1.value[4]\
    +dense0.value[14]*dense1.value[7]\
    -dense0.value[15]*dense1.value[6]\
    -dense0.value[16]*dense1.value[25]\
    +dense0.value[17]*dense1.value[24]\
    -dense0.value[18]*dense1.value[27]\
    -dense0.value[19]*dense1.value[26]\
    -dense0.value[20]*dense1.value[29]\
    -dense0.value[21]*dense1.value[28]\
    +dense0.value[22]*dense1.value[31]\
    -dense0.value[23]*dense1.value[30]\
    -dense0.value[24]*dense1.value[17]\
    -dense0.value[25]*dense1.value[16]\
    +dense0.value[26]*dense1.value[19]\
    -dense0.value[27]*dense1.value[18]\
    +dense0.value[28]*dense1.value[21]\
    -dense0.value[29]*dense1.value[20]\
    +dense0.value[30]*dense1.value[23]\
    +dense0.value[31]*dense1.value[22]\
;\
    dense.value[10] =\
    +dense0.value[0]*dense1.value[10]\
    +dense0.value[1]*dense1.value[11]\
    +dense0.value[2]*dense1.value[8]\
    -dense0.value[3]*dense1.value[9]\
    -dense0.value[4]*dense1.value[14]\
    +dense0.value[5]*dense1.value[15]\
    +dense0.value[6]*dense1.value[12]\
    +dense0.value[7]*dense1.value[13]\
    -dense0.value[8]*dense1.value[2]\
    +dense0.value[9]*dense1.value[3]\
    +dense0.value[10]*dense1.value[0]\
    +dense0.value[11]*dense1.value[1]\
    -dense0.value[12]*dense1.value[6]\
    -dense0.value[13]*dense1.value[7]\
    -dense0.value[14]*dense1.value[4]\
    +dense0.value[15]*dense1.value[5]\
    -dense0.value[16]*dense1.value[26]\
    +dense0.value[17]*dense1.value[27]\
    +dense0.value[18]*dense1.value[24]\
    +dense0.value[19]*dense1.value[25]\
    -dense0.value[20]*dense1.value[30]\
    -dense0.value[21]*dense1.value[31]\
    -dense0.value[22]*dense1.value[28]\
    +dense0.value[23]*dense1.value[29]\
    -dense0.value[24]*dense1.value[18]\
    -dense0.value[25]*dense1.value[19]\
    -dense0.value[26]*dense1.value[16]\
    +dense0.value[27]*dense1.value[17]\
    +dense0.value[28]*dense1.value[22]\
    -dense0.value[29]*dense1.value[23]\
    -dense0.value[30]*dense1.value[20]\
    -dense0.value[31]*dense1.value[21]\
;\
    dense.value[12] =\
    +dense0.value[0]*dense1.value[12]\
    +dense0.value[1]*dense1.value[13]\
    +dense0.value[2]*dense1.value[14]\
    -dense0.value[3]*dense1.value[15]\
    +dense0.value[4]*dense1.value[8]\
    -dense0.value[5]*dense1.value[9]\
    -dense0.value[6]*dense1.value[10]\
    -dense0.value[7]*dense1.value[11]\
    -dense0.value[8]*dense1.value[4]\
    +dense0.value[9]*dense1.value[5]\
    +dense0.value[10]*dense1.value[6]\
    +dense0.value[11]*dense1.value[7]\
    +dense0.value[12]*dense1.value[0]\
    +dense0.value[13]*dense1.value[1]\
    +dense0.value[14]*dense1.value[2]\
    -dense0.value[15]*dense1.value[3]\
    -dense0.value[16]*dense1.value[28]\
    +dense0.value[17]*dense1.value[29]\
    +dense0.value[18]*dense1.value[30]\
    +dense0.value[19]*dense1.value[31]\
    +dense0.value[20]*dense1.value[24]\
    +dense0.value[21]*dense1.value[25]\
    +dense0.value[22]*dense1.value[26]\
    -dense0.value[23]*dense1.value[27]\
    -dense0.value[24]*dense1.value[20]\
    -dense0.value[25]*dense1.value[21]\
    -dense0.value[26]*dense1.value[22]\
    +dense0.value[27]*dense1.value[23]\
    -dense0.value[28]*dense1.value[16]\
    +dense0.value[29]*dense1.value[17]\
    +dense0.value[30]*dense1.value[18]\
    +dense0.value[31]*dense1.value[19]\
;\
    dense.value[17] =\
    +dense0.value[0]*dense1.value[17]\
    +dense0.value[1]*dense1.value[16]\
    -dense0.value[2]*dense1.value[19]\
    +dense0.value[3]*dense1.value[18]\
    -dense0.value[4]*dense1.value[21]\
    +dense0.value[5]*dense1.value[20]\
    -dense0.value[6]*dense1.value[23]\
    -dense0.value[7]*dense1.value[22]\
    -dense0.value[8]*dense1.value[25]\
    +dense0.value[9]*dense1.value[24]\
    -dense0.value[10]*dense1.value[27]\
    -dense0.value[11]*dense1.value[26]\
    -dense0.value[12]*dense1.value[29]\
    -dense0.value[13]*dense1.value[28]\
    +dense0.value[14]*dense1.value[31]\
    -dense0.value[15]*dense1.value[30]\
    -dense0.value[16]*dense1.value[1]\
    +dense0.value[17]*dense1.value[0]\
    -dense0.value[18]*dense1.value[3]\
    -dense0.value[19]*dense1.value[2]\
    -dense0.value[20]*dense1.value[5]\
    -dense0.value[21]*dense1.value[4]\
    +dense0.value[22]*dense1.value[7]\
    -dense0.value[23]*dense1.value[6]\
    -dense0.value[24]*dense1.value[9]\
    -dense0.value[25]*dense1.value[8]\
    +dense0.value[26]*dense1.value[11]\
    -dense0.value[27]*dense1.value[10]\
    +dense0.value[28]*dense1.value[13]\
    -dense0.value[29]*dense1.value[12]\
    +dense0.value[30]*dense1.value[15]\
    +dense0.value[31]*dense1.value[14]\
;\
    dense.value[18] =\
    +dense0.value[0]*dense1.value[18]\
    +dense0.value[1]*dense1.value[19]\
    +dense0.value[2]*dense1.value[16]\
    -dense0.value[3]*dense1.value[17]\
    -dense0.value[4]*dense1.value[22]\
    +dense0.value[5]*dense1.value[23]\
    +dense0.value[6]*dense1.value[20]\
    +dense0.value[7]*dense1.value[21]\
    -dense0.value[8]*dense1.value[26]\
    +dense0.value[9]*dense1.value[27]\
    +dense0.value[10]*dense1.value[24]\
    +dense0.value[11]*dense1.value[25]\
    -dense0.value[12]*dense1.value[30]\
    -dense0.value[13]*dense1.value[31]\
    -dense0.value[14]*dense1.value[28]\
    +dense0.value[15]*dense1.value[29]\
    -dense0.value[16]*dense1.value[2]\
    +dense0.value[17]*dense1.value[3]\
    +dense0.value[18]*dense1.value[0]\
    +dense0.value[19]*dense1.value[1]\
    -dense0.value[20]*dense1.value[6]\
    -dense0.value[21]*dense1.value[7]\
    -dense0.value[22]*dense1.value[4]\
    +dense0.value[23]*dense1.value[5]\
    -dense0.value[24]*dense1.value[10]\
    -dense0.value[25]*dense1.value[11]\
    -dense0.value[26]*dense1.value[8]\
    +dense0.value[27]*dense1.value[9]\
    +dense0.value[28]*dense1.value[14]\
    -dense0.value[29]*dense1.value[15]\
    -dense0.value[30]*dense1.value[12]\
    -dense0.value[31]*dense1.value[13]\
;\
    dense.value[20] =\
    +dense0.value[0]*dense1.value[20]\
    +dense0.value[1]*dense1.value[21]\
    +dense0.value[2]*dense1.value[22]\
    -dense0.value[3]*dense1.value[23]\
    +dense0.value[4]*dense1.value[16]\
    -dense0.value[5]*dense1.value[17]\
    -dense0.value[6]*dense1.value[18]\
    -dense0.value[7]*dense1.value[19]\
    -dense0.value[8]*dense1.value[28]\
    +dense0.value[9]*dense1.value[29]\
    +dense0.value[10]*dense1.value[30]\
    +dense0.value[11]*dense1.value[31]\
    +dense0.value[12]*dense1.value[24]\
    +dense0.value[13]*dense1.value[25]\
    +dense0.value[14]*dense1.value[26]\
    -dense0.value[15]*dense1.value[27]\
    -dense0.value[16]*dense1.value[4]\
    +dense0.value[17]*dense1.value[5]\
    +dense0.value[18]*dense1.value[6]\
    +dense0.value[19]*dense1.value[7]\
    +dense0.value[20]*dense1.value[0]\
    +dense0.value[21]*dense1.value[1]\
    +dense0.value[22]*dense1.value[2]\
    -dense0.value[23]*dense1.value[3]\
    -dense0.value[24]*dense1.value[12]\
    -dense0.value[25]*dense1.value[13]\
    -dense0.value[26]*dense1.value[14]\
    +dense0.value[27]*dense1.value[15]\
    -dense0.value[28]*dense1.value[8]\
    +dense0.value[29]*dense1.value[9]\
    +dense0.value[30]*dense1.value[10]\
    +dense0.value[31]*dense1.value[11]\
;\
    dense.value[24] =\
    +dense0.value[0]*dense1.value[24]\
    +dense0.value[1]*dense1.value[25]\
    +dense0.value[2]*dense1.value[26]\
    -dense0.value[3]*dense1.value[27]\
    +dense0.value[4]*dense1.value[28]\
    -dense0.value[5]*dense1.value[29]\
    -dense0.value[6]*dense1.value[30]\
    -dense0.value[7]*dense1.value[31]\
    +dense0.value[8]*dense1.value[16]\
    -dense0.value[9]*dense1.value[17]\
    -dense0.value[10]*dense1.value[18]\
    -dense0.value[11]*dense1.value[19]\
    -dense0.value[12]*dense1.value[20]\
    -dense0.value[13]*dense1.value[21]\
    -dense0.value[14]*dense1.value[22]\
    +dense0.value[15]*dense1.value[23]\
    -dense0.value[16]*dense1.value[8]\
    +dense0.value[17]*dense1.value[9]\
    +dense0.value[18]*dense1.value[10]\
    +dense0.value[19]*dense1.value[11]\
    +dense0.value[20]*dense1.value[12]\
    +dense0.value[21]*dense1.value[13]\
    +dense0.value[22]*dense1.value[14]\
    -dense0.value[23]*dense1.value[15]\
    +dense0.value[24]*dense1.value[0]\
    +dense0.value[25]*dense1.value[1]\
    +dense0.value[26]*dense1.value[2]\
    -dense0.value[27]*dense1.value[3]\
    +dense0.value[28]*dense1.value[4]\
    -dense0.value[29]*dense1.value[5]\
    -dense0.value[30]*dense1.value[6]\
    -dense0.value[31]*dense1.value[7]\
;\
}
#define GEN1_DENSE_GRADE3GEOMETRICPRODUCT(dense,dense0,dense1){\
    dense.value[7] =\
    +dense0.value[0]*dense1.value[7]\
    +dense0.value[1]*dense1.value[6]\
    -dense0.value[2]*dense1.value[5]\
    +dense0.value[3]*dense1.value[4]\
    +dense0.value[4]*dense1.value[3]\
    -dense0.value[5]*dense1.value[2]\
    +dense0.value[6]*dense1.value[1]\
    +dense0.value[7]*dense1.value[0]\
    -dense0.value[8]*dense1.value[15]\
    +dense0.value[9]*dense1.value[14]\
    -dense0.value[10]*dense1.value[13]\
    -dense0.value[11]*dense1.value[12]\
    +dense0.value[12]*dense1.value[11]\
    +dense0.value[13]*dense1.value[10]\
    -dense0.value[14]*dense1.value[9]\
    +dense0.value[15]*dense1.value[8]\
    +dense0.value[16]*dense1.value[23]\
    -dense0.value[17]*dense1.value[22]\
    +dense0.value[18]*dense1.value[21]\
    +dense0.value[19]*dense1.value[20]\
    -dense0.value[20]*dense1.value[19]\
    -dense0.value[21]*dense1.value[18]\
    +dense0.value[22]*dense1.value[17]\
    -dense0.value[23]*dense1.value[16]\
    +dense0.value[24]*dense1.value[31]\
    +dense0.value[25]*dense1.value[30]\
    -dense0.value[26]*dense1.value[29]\
    +dense0.value[27]*dense1.value[28]\
    +dense0.value[28]*dense1.value[27]\
    -dense0.value[29]*dense1.value[26]\
    +dense0.value[30]*dense1.value[25]\
    +dense0.value[31]*dense1.value[24]\
;\
    dense.value[11] =\
    +dense0.value[0]*dense1.value[11]\
    +dense0.value[1]*dense1.value[10]\
    -dense0.value[2]*dense1.value[9]\
    +dense0.value[3]*dense1.value[8]\
    +dense0.value[4]*dense1.value[15]\
    -dense0.value[5]*dense1.value[14]\
    +dense0.value[6]*dense1.value[13]\
    +dense0.value[7]*dense1.value[12]\
    +dense0.value[8]*dense1.value[3]\
    -dense0.value[9]*dense1.value[2]\
    +dense0.value[10]*dense1.value[1]\
    +dense0.value[11]*dense1.value[0]\
    -dense0.value[12]*dense1.value[7]\
    -dense0.value[13]*dense1.value[6]\
    +dense0.value[14]*dense1.value[5]\
    -dense0.value[15]*dense1.value[4]\
    +dense0.value[16]*dense1.value[27]\
    -dense0.value[17]*dense1.value[26]\
    +dense0.value[18]*dense1.value[25]\
    +dense0.value[19]*dense1.value[24]\
    -dense0.value[20]*dense1.value[31]\
    -dense0.value[21]*dense1.value[30]\
    +dense0.value[22]*dense1.value[29]\
    -dense0.value[23]*dense1.value[28]\
    -dense0.value[24]*dense1.value[19]\
    -dense0.value[25]*dense1.value[18]\
    +dense0.value[26]*dense1.value[17]\
    -dense0.value[27]*dense1.value[16]\
    -dense0.value[28]*dense1.value[23]\
    +dense0.value[29]*dense1.value[22]\
    -dense0.value[30]*dense1.value[21]\
    -dense0.value[31]*dense1.value[20]\
;\
    dense.value[13] =\
    +dense0.value[0]*dense1.value[13]\
    +dense0.value[1]*dense1.value[12]\
    -dense0.value[2]*dense1.value[15]\
    +dense0.value[3]*dense1.value[14]\
    -dense0.value[4]*dense1.value[9]\
    +dense0.value[5]*dense1.value[8]\
    -dense0.value[6]*dense1.value[11]\
    -dense0.value[7]*dense1.value[10]\
    +dense0.value[8]*dense1.value[5]\
    -dense0.value[9]*dense1.value[4]\
    +dense0.value[10]*dense1.value[7]\
    +dense0.value[11]*dense1.value[6]\
    +dense0.value[12]*dense1.value[1]\
    +dense0.value[13]*dense1.value[0]\
    -dense0.value[14]*dense1.value[3]\
    +dense0.value[15]*dense1.value[2]\
    +dense0.value[16]*dense1.value[29]\
    -dense0.value[17]*dense1.value[28]\
    +dense0.value[18]*dense1.value[31]\
    +dense0.value[19]*dense1.value[30]\
    +dense0.value[20]*dense1.value[25]\
    +dense0.value[21]*dense1.value[24]\
    -dense0.value[22]*dense1.value[27]\
    +dense0.value[23]*dense1.value[26]\
    -dense0.value[24]*dense1.value[21]\
    -dense0.value[25]*dense1.value[20]\
    +dense0.value[26]*dense1.value[23]\
    -dense0.value[27]*dense1.value[22]\
    +dense0.value[28]*dense1.value[17]\
    -dense0.value[29]*dense1.value[16]\
    +dense0.value[30]*dense1.value[19]\
    +dense0.value[31]*dense1.value[18]\
;\
    dense.value[14] =\
    +dense0.value[0]*dense1.value[14]\
    +dense0.value[1]*dense1.value[15]\
    +dense0.value[2]*dense1.value[12]\
    -dense0.value[3]*dense1.value[13]\
    -dense0.value[4]*dense1.value[10]\
    +dense0.value[5]*dense1.value[11]\
    +dense0.value[6]*dense1.value[8]\
    +dense0.value[7]*dense1.value[9]\
    +dense0.value[8]*dense1.value[6]\
    -dense0.value[9]*dense1.value[7]\
    -dense0.value[10]*dense1.value[4]\
    -dense0.value[11]*dense1.value[5]\
    +dense0.value[12]*dense1.value[2]\
    +dense0.value[13]*dense1.value[3]\
    +dense0.value[14]*dense1.value[0]\
    -dense0.value[15]*dense1.value[1]\
    +dense0.value[16]*dense1.value[30]\
    -dense0.value[17]*dense1.value[31]\
    -dense0.value[18]*dense1.value[28]\
    -dense0.value[19]*dense1.value[29]\
    +dense0.value[20]*dense1.value[26]\
    +dense0.value[21]*dense1.value[27]\
    +dense0.value[22]*dense1.value[24]\
    -dense0.value[23]*dense1.value[25]\
    -dense0.value[24]*dense1.value[22]\
    -dense0.value[25]*dense1.value[23]\
    -dense0.value[26]*dense1.value[20]\
    +dense0.value[27]*dense1.value[21]\
    +dense0.value[28]*dense1.value[18]\
    -dense0.value[29]*dense1.value[19]\
    -dense0.value[30]*dense1.value[16]\
    -dense0.value[31]*dense1.value[17]\
;\
    dense.value[19] =\
    +dense0.value[0]*dense1.value[19]\
    +dense0.value[1]*dense1.value[18]\
    -dense0.value[2]*dense1.value[17]\
    +dense0.value[3]*dense1.value[16]\
    +dense0.value[4]*dense1.value[23]\
    -dense0.value[5]*dense1.value[22]\
    +dense0.value[6]*dense1.value[21]\
    +dense0.value[7]*dense1.value[20]\
    +dense0.value[8]*dense1.value[27]\
    -dense0.value[9]*dense1.value[26]\
    +dense0.value[10]*dense1.value[25]\
    +dense0.value[11]*dense1.value[24]\
    -dense0.value[12]*dense1.value[31]\
    -dense0.value[13]*dense1.value[30]\
    +dense0.value[14]*dense1.value[29]\
    -dense0.value[15]*dense1.value[28]\
    +dense0.value[16]*dense1.value[3]\
    -dense0.value[17]*dense1.value[2]\
    +dense0.value[18]*dense1.value[1]\
    +dense0.value[19]*dense1.value[0]\
    -dense0.value[20]*dense1.value[7]\
    -dense0.value[21]*dense1.value[6]\
    +dense0.value[22]*dense1.value[5]\
    -dense0.value[23]*dense1.value[4]\
    -dense0.value[24]*dense1.value[11]\
    -dense0.value[25]*dense1.value[10]\
    +dense0.value[26]*dense1.value[9]\
    -dense0.value[27]*dense1.value[8]\
    -dense0.value[28]*dense1.value[15]\
    +dense0.value[29]*dense1.value[14]\
    -dense0.value[30]*dense1.value[13]\
    -dense0.value[31]*dense1.value[12]\
;\
    dense.value[21] =\
    +dense0.value[0]*dense1.value[21]\
    +dense0.value[1]*dense1.value[20]\
    -dense0.value[2]*dense1.value[23]\
    +dense0.value[3]*dense1.value[22]\
    -dense0.value[4]*dense1.value[17]\
    +dense0.value[5]*dense1.value[16]\
    -dense0.value[6]*dense1.value[19]\
    -dense0.value[7]*dense1.value[18]\
    +dense0.value[8]*dense1.value[29]\
    -dense0.value[9]*dense1.value[28]\
    +dense0.value[10]*dense1.value[31]\
    +dense0.value[11]*dense1.value[30]\
    +dense0.value[12]*dense1.value[25]\
    +dense0.value[13]*dense1.value[24]\
    -dense0.value[14]*dense1.value[27]\
    +dense0.value[15]*dense1.value[26]\
    +dense0.value[16]*dense1.value[5]\
    -dense0.value[17]*dense1.value[4]\
    +dense0.value[18]*dense1.value[7]\
    +dense0.value[19]*dense1.value[6]\
    +dense0.value[20]*dense1.value[1]\
    +dense0.value[21]*dense1.value[0]\
    -dense0.value[22]*dense1.value[3]\
    +dense0.value[23]*dense1.value[2]\
    -dense0.value[24]*dense1.value[13]\
    -dense0.value[25]*dense1.value[12]\
    +dense0.value[26]*dense1.value[15]\
    -dense0.value[27]*dense1.value[14]\
    +dense0.value[28]*dense1.value[9]\
    -dense0.value[29]*dense1.value[8]\
    +dense0.value[30]*dense1.value[11]\
    +dense0.value[31]*dense1.value[10]\
;\
    dense.value[22] =\
    +dense0.value[0]*dense1.value[22]\
    +dense0.value[1]*dense1.value[23]\
    +dense0.value[2]*dense1.value[20]\
    -dense0.value[3]*dense1.value[21]\
    -dense0.value[4]*dense1.value[18]\
    +dense0.value[5]*dense1.value[19]\
    +dense0.value[6]*dense1.value[16]\
    +dense0.value[7]*dense1.value[17]\
    +dense0.value[8]*dense1.value[30]\
    -dense0.value[9]*dense1.value[31]\
    -dense0.value[10]*dense1.value[28]\
    -dense0.value[11]*dense1.value[29]\
    +dense0.value[12]*dense1.value[26]\
    +dense0.value[13]*dense1.value[27]\
    +dense0.value[14]*dense1.value[24]\
    -dense0.value[15]*dense1.value[25]\
    +dense0.value[16]*dense1.value[6]\
    -dense0.value[17]*dense1.value[7]\
    -dense0.value[18]*dense1.value[4]\
    -dense0.value[19]*dense1.value[5]\
    +dense0.value[20]*dense1.value[2]\
    +dense0.value[21]*dense1.value[3]\
    +dense0.value[22]*dense1.value[0]\
    -dense0.value[23]*dense1.value[1]\
    -dense0.value[24]*dense1.value[14]\
    -dense0.value[25]*dense1.value[15]\
    -dense0.value[26]*dense1.value[12]\
    +dense0.value[27]*dense1.value[13]\
    +dense0.value[28]*dense1.value[10]\
    -dense0.value[29]*dense1.value[11]\
    -dense0.value[30]*dense1.value[8]\
    -dense0.value[31]*dense1.value[9]\
;\
    dense.value[25] =\
    +dense0.value[0]*dense1.value[25]\
    +dense0.value[1]*dense1.value[24]\
    -dense0.value[2]*dense1.value[27]\
    +dense0.value[3]*dense1.value[26]\
    -dense0.value[4]*dense1.value[29]\
    +dense0.value[5]*dense1.value[28]\
    -dense0.value[6]*dense1.value[31]\
    -dense0.value[7]*dense1.value[30]\
    -dense0.value[8]*dense1.value[17]\
    +dense0.value[9]*dense1.value[16]\
    -dense0.value[10]*dense1.value[19]\
    -dense0.value[11]*dense1.value[18]\
    -dense0.value[12]*dense1.value[21]\
    -dense0.value[13]*dense1.value[20]\
    +dense0.value[14]*dense1.value[23]\
    -dense0.value[15]*dense1.value[22]\
    +dense0.value[16]*dense1.value[9]\
    -dense0.value[17]*dense1.value[8]\
    +dense0.value[18]*dense1.value[11]\
    +dense0.value[19]*dense1.value[10]\
    +dense0.value[20]*dense1.value[13]\
    +dense0.value[21]*dense1.value[12]\
    -dense0.value[22]*dense1.value[15]\
    +dense0.value[23]*dense1.value[14]\
    +dense0.value[24]*dense1.value[1]\
    +dense0.value[25]*dense1.value[0]\
    -dense0.value[26]*dense1.value[3]\
    +dense0.value[27]*dense1.value[2]\
    -dense0.value[28]*dense1.value[5]\
    +dense0.value[29]*dense1.value[4]\
    -dense0.value[30]*dense1.value[7]\
    -dense0.value[31]*dense1.value[6]\
;\
    dense.value[26] =\
    +dense0.value[0]*dense1.value[26]\
    +dense0.value[1]*dense1.value[27]\
    +dense0.value[2]*dense1.value[24]\
    -dense0.value[3]*dense1.value[25]\
    -dense0.value[4]*dense1.value[30]\
    +dense0.value[5]*dense1.value[31]\
    +dense0.value[6]*dense1.value[28]\
    +dense0.value[7]*dense1.value[29]\
    -dense0.value[8]*dense1.value[18]\
    +dense0.value[9]*dense1.value[19]\
    +dense0.value[10]*dense1.value[16]\
    +dense0.value[11]*dense1.value[17]\
    -dense0.value[12]*dense1.value[22]\
    -dense0.value[13]*dense1.value[23]\
    -dense0.value[14]*dense1.value[20]\
    +dense0.value[15]*dense1.value[21]\
    +dense0.value[16]*dense1.value[10]\
    -dense0.value[17]*dense1.value[11]\
    -dense0.value[18]*dense1.value[8]\
    -dense0.value[19]*dense1.value[9]\
    +dense0.value[20]*dense1.value[14]\
    +dense0.value[21]*dense1.value[15]\
    +dense0.value[22]*dense1.value[12]\
    -dense0.value[23]*dense1.value[13]\
    +dense0.value[24]*dense1.value[2]\
    +dense0.value[25]*dense1.value[3]\
    +dense0.value[26]*dense1.value[0]\
    -dense0.value[27]*dense1.value[1]\
    -dense0.value[28]*dense1.value[6]\
    +dense0.value[29]*dense1.value[7]\
    +dense0.value[30]*dense1.value[4]\
    +dense0.value[31]*dense1.value[5]\
;\
    dense.value[28] =\
    +dense0.value[0]*dense1.value[28]\
    +dense0.value[1]*dense1.value[29]\
    +dense0.value[2]*dense1.value[30]\
    -dense0.value[3]*dense1.value[31]\
    +dense0.value[4]*dense1.value[24]\
    -dense0.value[5]*dense1.value[25]\
    -dense0.value[6]*dense1.value[26]\
    -dense0.value[7]*dense1.value[27]\
    -dense0.value[8]*dense1.value[20]\
    +dense0.value[9]*dense1.value[21]\
    +dense0.value[10]*dense1.value[22]\
    +dense0.value[11]*dense1.value[23]\
    +dense0.value[12]*dense1.value[16]\
    +dense0.value[13]*dense1.value[17]\
    +dense0.value[14]*dense1.value[18]\
    -dense0.value[15]*dense1.value[19]\
    +dense0.value[16]*dense1.value[12]\
    -dense0.value[17]*dense1.value[13]\
    -dense0.value[18]*dense1.value[14]\
    -dense0.value[19]*dense1.value[15]\
    -dense0.value[20]*dense1.value[8]\
    -dense0.value[21]*dense1.value[9]\
    -dense0.value[22]*dense1.value[10]\
    +dense0.value[23]*dense1.value[11]\
    +dense0.value[24]*dense1.value[4]\
    +dense0.value[25]*dense1.value[5]\
    +dense0.value[26]*dense1.value[6]\
    -dense0.value[27]*dense1.value[7]\
    +dense0.value[28]*dense1.value[0]\
    -dense0.value[29]*dense1.value[1]\
    -dense0.value[30]*dense1.value[2]\
    -dense0.value[31]*dense1.value[3]\
;\
}
#define GEN1_DENSE_GRADE4GEOMETRICPRODUCT(dense,dense0,dense1){\
    dense.value[15] =\
    +dense0.value[0]*dense1.value[15]\
    +dense0.value[1]*dense1.value[14]\
    -dense0.value[2]*dense1.value[13]\
    +dense0.value[3]*dense1.value[12]\
    +dense0.value[4]*dense1.value[11]\
    -dense0.value[5]*dense1.value[10]\
    +dense0.value[6]*dense1.value[9]\
    +dense0.value[7]*dense1.value[8]\
    -dense0.value[8]*dense1.value[7]\
    +dense0.value[9]*dense1.value[6]\
    -dense0.value[10]*dense1.value[5]\
    -dense0.value[11]*dense1.value[4]\
    +dense0.value[12]*dense1.value[3]\
    +dense0.value[13]*dense1.value[2]\
    -dense0.value[14]*dense1.value[1]\
    +dense0.value[15]*dense1.value[0]\
    -dense0.value[16]*dense1.value[31]\
    +dense0.value[17]*dense1.value[30]\
    -dense0.value[18]*dense1.value[29]\
    -dense0.value[19]*dense1.value[28]\
    +dense0.value[20]*dense1.value[27]\
    +dense0.value[21]*dense1.value[26]\
    -dense0.value[22]*dense1.value[25]\
    +dense0.value[23]*dense1.value[24]\
    -dense0.value[24]*dense1.value[23]\
    -dense0.value[25]*dense1.value[22]\
    +dense0.value[26]*dense1.value[21]\
    -dense0.value[27]*dense1.value[20]\
    -dense0.value[28]*dense1.value[19]\
    +dense0.value[29]*dense1.value[18]\
    -dense0.value[30]*dense1.value[17]\
    -dense0.value[31]*dense1.value[16]\
;\
    dense.value[23] =\
    +dense0.value[0]*dense1.value[23]\
    +dense0.value[1]*dense1.value[22]\
    -dense0.value[2]*dense1.value[21]\
    +dense0.value[3]*dense1.value[20]\
    +dense0.value[4]*dense1.value[19]\
    -dense0.value[5]*dense1.value[18]\
    +dense0.value[6]*dense1.value[17]\
    +dense0.value[7]*dense1.value[16]\
    -dense0.value[8]*dense1.value[31]\
    +dense0.value[9]*dense1.value[30]\
    -dense0.value[10]*dense1.value[29]\
    -dense0.value[11]*dense1.value[28]\
    +dense0.value[12]*dense1.value[27]\
    +dense0.value[13]*dense1.value[26]\
    -dense0.value[14]*dense1.value[25]\
    +dense0.value[15]*dense1.value[24]\
    -dense0.value[16]*dense1.value[7]\
    +dense0.value[17]*dense1.value[6]\
    -dense0.value[18]*dense1.value[5]\
    -dense0.value[19]*dense1.value[4]\
    +dense0.value[20]*dense1.value[3]\
    +dense0.value[21]*dense1.value[2]\
    -dense0.value[22]*dense1.value[1]\
    +dense0.value[23]*dense1.value[0]\
    -dense0.value[24]*dense1.value[15]\
    -dense0.value[25]*dense1.value[14]\
    +dense0.value[26]*dense1.value[13]\
    -dense0.value[27]*dense1.value[12]\
    -dense0.value[28]*dense1.value[11]\
    +dense0.value[29]*dense1.value[10]\
    -dense0.value[30]*dense1.value[9]\
    -dense0.value[31]*dense1.value[8]\
;\
    dense.value[27] =\
    +dense0.value[0]*dense1.value[27]\
    +dense0.value[1]*dense1.value[26]\
    -dense0.value[2]*dense1.value[25]\
    +dense0.value[3]*dense1.value[24]\
    +dense0.value[4]*dense1.value[31]\
    -dense0.value[5]*dense1.value[30]\
    +dense0.value[6]*dense1.value[29]\
    +dense0.value[7]*dense1.value[28]\
    +dense0.value[8]*dense1.value[19]\
    -dense0.value[9]*dense1.value[18]\
    +dense0.value[10]*dense1.value[17]\
    +dense0.value[11]*dense1.value[16]\
    -dense0.value[12]*dense1.value[23]\
    -dense0.value[13]*dense1.value[22]\
    +dense0.value[14]*dense1.value[21]\
    -dense0.value[15]*dense1.value[20]\
    -dense0.value[16]*dense1.value[11]\
    +dense0.value[17]*dense1.value[10]\
    -dense0.value[18]*dense1.value[9]\
    -dense0.value[19]*dense1.value[8]\
    +dense0.value[20]*dense1.value[15]\
    +dense0.value[21]*dense1.value[14]\
    -dense0.value[22]*dense1.value[13]\
    +dense0.value[23]*dense1.value[12]\
    +dense0.value[24]*dense1.value[3]\
    +dense0.value[25]*dense1.value[2]\
    -dense0.value[26]*dense1.value[1]\
    +dense0.value[27]*dense1.value[0]\
    +dense0.value[28]*dense1.value[7]\
    -dense0.value[29]*dense1.value[6]\
    +dense0.value[30]*dense1.value[5]\
    +dense0.value[31]*dense1.value[4]\
;\
    dense.value[29] =\
    +dense0.value[0]*dense1.value[29]\
    +dense0.value[1]*dense1.value[28]\
    -dense0.value[2]*dense1.value[31]\
    +dense0.value[3]*dense1.value[30]\
    -dense0.value[4]*dense1.value[25]\
    +dense0.value[5]*dense1.value[24]\
    -dense0.value[6]*dense1.value[27]\
    -dense0.value[7]*dense1.value[26]\
    +dense0.value[8]*dense1.value[21]\
    -dense0.value[9]*dense1.value[20]\
    +dense0.value[10]*dense1.value[23]\
    +dense0.value[11]*dense1.value[22]\
    +dense0.value[12]*dense1.value[17]\
    +dense0.value[13]*dense1.value[16]\
    -dense0.value[14]*dense1.value[19]\
    +dense0.value[15]*dense1.value[18]\
    -dense0.value[16]*dense1.value[13]\
    +dense0.value[17]*dense1.value[12]\
    -dense0.value[18]*dense1.value[15]\
    -dense0.value[19]*dense1.value[14]\
    -dense0.value[20]*dense1.value[9]\
    -dense0.value[21]*dense1.value[8]\
    +dense0.value[22]*dense1.value[11]\
    -dense0.value[23]*dense1.value[10]\
    +dense0.value[24]*dense1.value[5]\
    +dense0.value[25]*dense1.value[4]\
    -dense0.value[26]*dense1.value[7]\
    +dense0.value[27]*dense1.value[6]\
    -dense0.value[28]*dense1.value[1]\
    +dense0.value[29]*dense1.value[0]\
    -dense0.value[30]*dense1.value[3]\
    -dense0.value[31]*dense1.value[2]\
;\
    dense.value[30] =\
    +dense0.value[0]*dense1.value[30]\
    +dense0.value[1]*dense1.value[31]\
    +dense0.value[2]*dense1.value[28]\
    -dense0.value[3]*dense1.value[29]\
    -dense0.value[4]*dense1.value[26]\
    +dense0.value[5]*dense1.value[27]\
    +dense0.value[6]*dense1.value[24]\
    +dense0.value[7]*dense1.value[25]\
    +dense0.value[8]*dense1.value[22]\
    -dense0.value[9]*dense1.value[23]\
    -dense0.value[10]*dense1.value[20]\
    -dense0.value[11]*dense1.value[21]\
    +dense0.value[12]*dense1.value[18]\
    +dense0.value[13]*dense1.value[19]\
    +dense0.value[14]*dense1.value[16]\
    -dense0.value[15]*dense1.value[17]\
    -dense0.value[16]*dense1.value[14]\
    +dense0.value[17]*dense1.value[15]\
    +dense0.value[18]*dense1.value[12]\
    +dense0.value[19]*dense1.value[13]\
    -dense0.value[20]*dense1.value[10]\
    -dense0.value[21]*dense1.value[11]\
    -dense0.value[22]*dense1.value[8]\
    +dense0.value[23]*dense1.value[9]\
    +dense0.value[24]*dense1.value[6]\
    +dense0.value[25]*dense1.value[7]\
    +dense0.value[26]*dense1.value[4]\
    -dense0.value[27]*dense1.value[5]\
    -dense0.value[28]*dense1.value[2]\
    +dense0.value[29]*dense1.value[3]\
    +dense0.value[30]*dense1.value[0]\
    +dense0.value[31]*dense1.value[1]\
;\
}
#define GEN1_DENSE_GRADE5GEOMETRICPRODUCT(dense,dense0,dense1){\
    dense.value[31] =\
    +dense0.value[0]*dense1.value[31]\
    +dense0.value[1]*dense1.value[30]\
    -dense0.value[2]*dense1.value[29]\
    +dense0.value[3]*dense1.value[28]\
    +dense0.value[4]*dense1.value[27]\
    -dense0.value[5]*dense1.value[26]\
    +dense0.value[6]*dense1.value[25]\
    +dense0.value[7]*dense1.value[24]\
    -dense0.value[8]*dense1.value[23]\
    +dense0.value[9]*dense1.value[22]\
    -dense0.value[10]*dense1.value[21]\
    -dense0.value[11]*dense1.value[20]\
    +dense0.value[12]*dense1.value[19]\
    +dense0.value[13]*dense1.value[18]\
    -dense0.value[14]*dense1.value[17]\
    +dense0.value[15]*dense1.value[16]\
    +dense0.value[16]*dense1.value[15]\
    -dense0.value[17]*dense1.value[14]\
    +dense0.value[18]*dense1.value[13]\
    +dense0.value[19]*dense1.value[12]\
    -dense0.value[20]*dense1.value[11]\
    -dense0.value[21]*dense1.value[10]\
    +dense0.value[22]*dense1.value[9]\
    -dense0.value[23]*dense1.value[8]\
    +dense0.value[24]*dense1.value[7]\
    +dense0.value[25]*dense1.value[6]\
    -dense0.value[26]*dense1.value[5]\
    +dense0.value[27]*dense1.value[4]\
    +dense0.value[28]*dense1.value[3]\
    -dense0.value[29]*dense1.value[2]\
    +dense0.value[30]*dense1.value[1]\
    +dense0.value[31]*dense1.value[0]\
;\
}

static gen1_DenseMultivector gen1_dense_gradegeometricproduct(gen1_DenseMultivector dense0, gen1_DenseMultivector dense1, int *grades, Py_ssize_t size){
    gen1_DenseMultivector dense = {{0}};
    for(Py_ssize_t i = 0; i < size; i++){
        switch(grades[i]){
            case 0:
                GEN1_DENSE_GRADE0GEOMETRICPRODUCT(dense,dense0,dense1);
                break;
            case 1:
                GEN1_DENSE_GRADE1GEOMETRICPRODUCT(dense,dense0,dense1);
                break;
            case 2:
                GEN1_DENSE_GRADE2GEOMETRICPRODUCT(dense,dense0,dense1);
                break;
            case 3:
                GEN1_DENSE_GRADE3GEOMETRICPRODUCT(dense,dense0,dense1);
                break;
            case 4:
                GEN1_DENSE_GRADE4GEOMETRICPRODUCT(dense,dense0,dense1);
                break;
            case 5:
                GEN1_DENSE_GRADE5GEOMETRICPRODUCT(dense,dense0,dense1);
                break;
            default:// do nothing for different values
                break;
        }
    }
    return dense;
}


static gen1_DenseMultivector gen1_dense_innerproduct(gen1_DenseMultivector dense0, gen1_DenseMultivector dense1){
    gen1_DenseMultivector dense = {{0}};
    dense.value[0] =
    +dense0.value[1]*dense1.value[1]
    +dense0.value[2]*dense1.value[2]
    -dense0.value[3]*dense1.value[3]
    +dense0.value[4]*dense1.value[4]
    -dense0.value[5]*dense1.value[5]
    -dense0.value[6]*dense1.value[6]
    -dense0.value[7]*dense1.value[7]
    +dense0.value[8]*dense1.value[8]
    -dense0.value[9]*dense1.value[9]
    -dense0.value[10]*dense1.value[10]
    -dense0.value[11]*dense1.value[11]
    -dense0.value[12]*dense1.value[12]
    -dense0.value[13]*dense1.value[13]
    -dense0.value[14]*dense1.value[14]
    +dense0.value[15]*dense1.value[15]
    -dense0.value[16]*dense1.value[16]
    +dense0.value[17]*dense1.value[17]
    +dense0.value[18]*dense1.value[18]
    +dense0.value[19]*dense1.value[19]
    +dense0.value[20]*dense1.value[20]
    +dense0.value[21]*dense1.value[21]
    +dense0.value[22]*dense1.value[22]
    -dense0.value[23]*dense1.value[23]
    +dense0.value[24]*dense1.value[24]
    +dense0.value[25]*dense1.value[25]
    +dense0.value[26]*dense1.value[26]
    -dense0.value[27]*dense1.value[27]
    +dense0.value[28]*dense1.value[28]
    -dense0.value[29]*dense1.value[29]
    -dense0.value[30]*dense1.value[30]
    -dense0.value[31]*dense1.value[31]
;
    dense.value[1] =
    -dense0.value[2]*dense1.value[3]
    +dense0.value[3]*dense1.value[2]
    -dense0.value[4]*dense1.value[5]
    +dense0.value[5]*dense1.value[4]
    -dense0.value[6]*dense1.value[7]
    -dense0.value[7]*dense1.value[6]
    -dense0.value[8]*dense1.value[9]
    +dense0.value[9]*dense1.value[8]
    -dense0.value[10]*dense1.value[11]
    -dense0.value[11]*dense1.value[10]
    -dense0.value[12]*dense1.value[13]
    -dense0.value[13]*dense1.value[12]
    +dense0.value[14]*dense1.value[15]
    -dense0.value[15]*dense1.value[14]
    +dense0.value[16]*dense1.value[17]
    -dense0.value[17]*dense1.value[16]
    +dense0.value[18]*dense1.value[19]
    +dense0.value[19]*dense1.value[18]
    +dense0.value[20]*dense1.value[21]
    +dense0.value[21]*dense1.value[20]
    -dense0.value[22]*dense1.value[23]
    +dense0.value[23]*dense1.value[22]
    +dense0.value[24]*dense1.value[25]
    +dense0.value[25]*dense1.value[24]
    -dense0.value[26]*dense1.value[27]
    +dense0.value[27]*dense1.value[26]
    -dense0.value[28]*dense1.value[29]
    +dense0.value[29]*dense1.value[28]
    -dense0.value[30]*dense1.value[31]
    -dense0.value[31]*dense1.value[30]
;
    dense.value[2] =
    +dense0.value[1]*dense1.value[3]
    -dense0.value[3]*dense1.value[1]
    -dense0.value[4]*dense1.value[6]
    +dense0.value[5]*dense1.value[7]
    +dense0.value[6]*dense1.value[4]
    +dense0.value[7]*dense1.value[5]
    -dense0.value[8]*dense1.value[10]
    +dense0.value[9]*dense1.value[11]
    +dense0.value[10]*dense1.value[8]
    +dense0.value[11]*dense1.value[9]
    -dense0.value[12]*dense1.value[14]
    -dense0.value[13]*dense1.value[15]
    -dense0.value[14]*dense1.value[12]
    +dense0.value[15]*dense1.value[13]
    +dense0.value[16]*dense1.value[18]
    -dense0.value[17]*dense1.value[19]
    -dense0.value[18]*dense1.value[16]
    -dense0.value[19]*dense1.value[17]
    +dense0.value[20]*dense1.value[22]
    +dense0.value[21]*dense1.value[23]
    +dense0.value[22]*dense1.value[20]
    -dense0.value[23]*dense1.value[21]
    +dense0.value[24]*dense1.value[26]
    +dense0.value[25]*dense1.value[27]
    +dense0.value[26]*dense1.value[24]
    -dense0.value[27]*dense1.value[25]
    -dense0.value[28]*dense1.value[30]
    +dense0.value[29]*dense1.value[31]
    +dense0.value[30]*dense1.value[28]
    +dense0.value[31]*dense1.value[29]
;
    dense.value[3] =
    +dense0.value[4]*dense1.value[7]
    +dense0.value[7]*dense1.value[4]
    +dense0.value[8]*dense1.value[11]
    +dense0.value[11]*dense1.value[8]
    -dense0.value[12]*dense1.value[15]
    -dense0.value[15]*dense1.value[12]
    -dense0.value[16]*dense1.value[19]
    -dense0.value[19]*dense1.value[16]
    +dense0.value[20]*dense1.value[23]
    +dense0.value[23]*dense1.value[20]
    +dense0.value[24]*dense1.value[27]
    +dense0.value[27]*dense1.value[24]
    +dense0.value[28]*dense1.value[31]
    +dense0.value[31]*dense1.value[28]
;
    dense.value[4] =
    +dense0.value[1]*dense1.value[5]
    +dense0.value[2]*dense1.value[6]
    -dense0.value[3]*dense1.value[7]
    -dense0.value[5]*dense1.value[1]
    -dense0.value[6]*dense1.value[2]
    -dense0.value[7]*dense1.value[3]
    -dense0.value[8]*dense1.value[12]
    +dense0.value[9]*dense1.value[13]
    +dense0.value[10]*dense1.value[14]
    +dense0.value[11]*dense1.value[15]
    +dense0.value[12]*dense1.value[8]
    +dense0.value[13]*dense1.value[9]
    +dense0.value[14]*dense1.value[10]
    -dense0.value[15]*dense1.value[11]
    +dense0.value[16]*dense1.value[20]
    -dense0.value[17]*dense1.value[21]
    -dense0.value[18]*dense1.value[22]
    -dense0.value[19]*dense1.value[23]
    -dense0.value[20]*dense1.value[16]
    -dense0.value[21]*dense1.value[17]
    -dense0.value[22]*dense1.value[18]
    +dense0.value[23]*dense1.value[19]
    +dense0.value[24]*dense1.value[28]
    +dense0.value[25]*dense1.value[29]
    +dense0.value[26]*dense1.value[30]
    -dense0.value[27]*dense1.value[31]
    +dense0.value[28]*dense1.value[24]
    -dense0.value[29]*dense1.value[25]
    -dense0.value[30]*dense1.value[26]
    -dense0.value[31]*dense1.value[27]
;
    dense.value[5] =
    -dense0.value[2]*dense1.value[7]
    -dense0.value[7]*dense1.value[2]
    +dense0.value[8]*dense1.value[13]
    +dense0.value[10]*dense1.value[15]
    +dense0.value[13]*dense1.value[8]
    +dense0.value[15]*dense1.value[10]
    -dense0.value[16]*dense1.value[21]
    -dense0.value[18]*dense1.value[23]
    -dense0.value[21]*dense1.value[16]
    -dense0.value[23]*dense1.value[18]
    +dense0.value[24]*dense1.value[29]
    -dense0.value[26]*dense1.value[31]
    +dense0.value[29]*dense1.value[24]
    -dense0.value[31]*dense1.value[26]
;
    dense.value[6] =
    +dense0.value[1]*dense1.value[7]
    +dense0.value[7]*dense1.value[1]
    +dense0.value[8]*dense1.value[14]
    -dense0.value[9]*dense1.value[15]
    +dense0.value[14]*dense1.value[8]
    -dense0.value[15]*dense1.value[9]
    -dense0.value[16]*dense1.value[22]
    +dense0.value[17]*dense1.value[23]
    -dense0.value[22]*dense1.value[16]
    +dense0.value[23]*dense1.value[17]
    +dense0.value[24]*dense1.value[30]
    +dense0.value[25]*dense1.value[31]
    +dense0.value[30]*dense1.value[24]
    +dense0.value[31]*dense1.value[25]
;
    dense.value[7] =
    -dense0.value[8]*dense1.value[15]
    +dense0.value[15]*dense1.value[8]
    +dense0.value[16]*dense1.value[23]
    -dense0.value[23]*dense1.value[16]
    +dense0.value[24]*dense1.value[31]
    +dense0.value[31]*dense1.value[24]
;
    dense.value[8] =
    +dense0.value[1]*dense1.value[9]
    +dense0.value[2]*dense1.value[10]
    -dense0.value[3]*dense1.value[11]
    +dense0.value[4]*dense1.value[12]
    -dense0.value[5]*dense1.value[13]
    -dense0.value[6]*dense1.value[14]
    -dense0.value[7]*dense1.value[15]
    -dense0.value[9]*dense1.value[1]
    -dense0.value[10]*dense1.value[2]
    -dense0.value[11]*dense1.value[3]
    -dense0.value[12]*dense1.value[4]
    -dense0.value[13]*dense1.value[5]
    -dense0.value[14]*dense1.value[6]
    +dense0.value[15]*dense1.value[7]
    +dense0.value[16]*dense1.value[24]
    -dense0.value[17]*dense1.value[25]
    -dense0.value[18]*dense1.value[26]
    -dense0.value[19]*dense1.value[27]
    -dense0.value[20]*dense1.value[28]
    -dense0.value[21]*dense1.value[29]
    -dense0.value[22]*dense1.value[30]
    +dense0.value[23]*dense1.value[31]
    -dense0.value[24]*dense1.value[16]
    -dense0.value[25]*dense1.value[17]
    -dense0.value[26]*dense1.value[18]
    +dense0.value[27]*dense1.value[19]
    -dense0.value[28]*dense1.value[20]
    +dense0.value[29]*dense1.value[21]
    +dense0.value[30]*dense1.value[22]
    +dense0.value[31]*dense1.value[23]
;
    dense.value[9] =
    -dense0.value[2]*dense1.value[11]
    -dense0.value[4]*dense1.value[13]
    -dense0.value[6]*dense1.value[15]
    -dense0.value[11]*dense1.value[2]
    -dense0.value[13]*dense1.value[4]
    -dense0.value[15]*dense1.value[6]
    -dense0.value[16]*dense1.value[25]
    -dense0.value[18]*dense1.value[27]
    -dense0.value[20]*dense1.value[29]
    +dense0.value[22]*dense1.value[31]
    -dense0.value[25]*dense1.value[16]
    -dense0.value[27]*dense1.value[18]
    -dense0.value[29]*dense1.value[20]
    +dense0.value[31]*dense1.value[22]
;
    dense.value[10] =
    +dense0.value[1]*dense1.value[11]
    -dense0.value[4]*dense1.value[14]
    +dense0.value[5]*dense1.value[15]
    +dense0.value[11]*dense1.value[1]
    -dense0.value[14]*dense1.value[4]
    +dense0.value[15]*dense1.value[5]
    -dense0.value[16]*dense1.value[26]
    +dense0.value[17]*dense1.value[27]
    -dense0.value[20]*dense1.value[30]
    -dense0.value[21]*dense1.value[31]
    -dense0.value[26]*dense1.value[16]
    +dense0.value[27]*dense1.value[17]
    -dense0.value[30]*dense1.value[20]
    -dense0.value[31]*dense1.value[21]
;
    dense.value[11] =
    +dense0.value[4]*dense1.value[15]
    -dense0.value[15]*dense1.value[4]
    +dense0.value[16]*dense1.value[27]
    -dense0.value[20]*dense1.value[31]
    -dense0.value[27]*dense1.value[16]
    -dense0.value[31]*dense1.value[20]
;
    dense.value[12] =
    +dense0.value[1]*dense1.value[13]
    +dense0.value[2]*dense1.value[14]
    -dense0.value[3]*dense1.value[15]
    +dense0.value[13]*dense1.value[1]
    +dense0.value[14]*dense1.value[2]
    -dense0.value[15]*dense1.value[3]
    -dense0.value[16]*dense1.value[28]
    +dense0.value[17]*dense1.value[29]
    +dense0.value[18]*dense1.value[30]
    +dense0.value[19]*dense1.value[31]
    -dense0.value[28]*dense1.value[16]
    +dense0.value[29]*dense1.value[17]
    +dense0.value[30]*dense1.value[18]
    +dense0.value[31]*dense1.value[19]
;
    dense.value[13] =
    -dense0.value[2]*dense1.value[15]
    +dense0.value[15]*dense1.value[2]
    +dense0.value[16]*dense1.value[29]
    +dense0.value[18]*dense1.value[31]
    -dense0.value[29]*dense1.value[16]
    +dense0.value[31]*dense1.value[18]
;
    dense.value[14] =
    +dense0.value[1]*dense1.value[15]
    -dense0.value[15]*dense1.value[1]
    +dense0.value[16]*dense1.value[30]
    -dense0.value[17]*dense1.value[31]
    -dense0.value[30]*dense1.value[16]
    -dense0.value[31]*dense1.value[17]
;
    dense.value[15] =
    -dense0.value[16]*dense1.value[31]
    -dense0.value[31]*dense1.value[16]
;
    dense.value[16] =
    +dense0.value[1]*dense1.value[17]
    +dense0.value[2]*dense1.value[18]
    -dense0.value[3]*dense1.value[19]
    +dense0.value[4]*dense1.value[20]
    -dense0.value[5]*dense1.value[21]
    -dense0.value[6]*dense1.value[22]
    -dense0.value[7]*dense1.value[23]
    +dense0.value[8]*dense1.value[24]
    -dense0.value[9]*dense1.value[25]
    -dense0.value[10]*dense1.value[26]
    -dense0.value[11]*dense1.value[27]
    -dense0.value[12]*dense1.value[28]
    -dense0.value[13]*dense1.value[29]
    -dense0.value[14]*dense1.value[30]
    +dense0.value[15]*dense1.value[31]
    -dense0.value[17]*dense1.value[1]
    -dense0.value[18]*dense1.value[2]
    -dense0.value[19]*dense1.value[3]
    -dense0.value[20]*dense1.value[4]
    -dense0.value[21]*dense1.value[5]
    -dense0.value[22]*dense1.value[6]
    +dense0.value[23]*dense1.value[7]
    -dense0.value[24]*dense1.value[8]
    -dense0.value[25]*dense1.value[9]
    -dense0.value[26]*dense1.value[10]
    +dense0.value[27]*dense1.value[11]
    -dense0.value[28]*dense1.value[12]
    +dense0.value[29]*dense1.value[13]
    +dense0.value[30]*dense1.value[14]
    +dense0.value[31]*dense1.value[15]
;
    dense.value[17] =
    -dense0.value[2]*dense1.value[19]
    -dense0.value[4]*dense1.value[21]
    -dense0.value[6]*dense1.value[23]
    -dense0.value[8]*dense1.value[25]
    -dense0.value[10]*dense1.value[27]
    -dense0.value[12]*dense1.value[29]
    +dense0.value[14]*dense1.value[31]
    -dense0.value[19]*dense1.value[2]
    -dense0.value[21]*dense1.value[4]
    -dense0.value[23]*dense1.value[6]
    -dense0.value[25]*dense1.value[8]
    -dense0.value[27]*dense1.value[10]
    -dense0.value[29]*dense1.value[12]
    +dense0.value[31]*dense1.value[14]
;
    dense.value[18] =
    +dense0.value[1]*dense1.value[19]
    -dense0.value[4]*dense1.value[22]
    +dense0.value[5]*dense1.value[23]
    -dense0.value[8]*dense1.value[26]
    +dense0.value[9]*dense1.value[27]
    -dense0.value[12]*dense1.value[30]
    -dense0.value[13]*dense1.value[31]
    +dense0.value[19]*dense1.value[1]
    -dense0.value[22]*dense1.value[4]
    +dense0.value[23]*dense1.value[5]
    -dense0.value[26]*dense1.value[8]
    +dense0.value[27]*dense1.value[9]
    -dense0.value[30]*dense1.value[12]
    -dense0.value[31]*dense1.value[13]
;
    dense.value[19] =
    +dense0.value[4]*dense1.value[23]
    +dense0.value[8]*dense1.value[27]
    -dense0.value[12]*dense1.value[31]
    -dense0.value[23]*dense1.value[4]
    -dense0.value[27]*dense1.value[8]
    -dense0.value[31]*dense1.value[12]
;
    dense.value[20] =
    +dense0.value[1]*dense1.value[21]
    +dense0.value[2]*dense1.value[22]
    -dense0.value[3]*dense1.value[23]
    -dense0.value[8]*dense1.value[28]
    +dense0.value[9]*dense1.value[29]
    +dense0.value[10]*dense1.value[30]
    +dense0.value[11]*dense1.value[31]
    +dense0.value[21]*dense1.value[1]
    +dense0.value[22]*dense1.value[2]
    -dense0.value[23]*dense1.value[3]
    -dense0.value[28]*dense1.value[8]
    +dense0.value[29]*dense1.value[9]
    +dense0.value[30]*dense1.value[10]
    +dense0.value[31]*dense1.value[11]
;
    dense.value[21] =
    -dense0.value[2]*dense1.value[23]
    +dense0.value[8]*dense1.value[29]
    +dense0.value[10]*dense1.value[31]
    +dense0.value[23]*dense1.value[2]
    -dense0.value[29]*dense1.value[8]
    +dense0.value[31]*dense1.value[10]
;
    dense.value[22] =
    +dense0.value[1]*dense1.value[23]
    +dense0.value[8]*dense1.value[30]
    -dense0.value[9]*dense1.value[31]
    -dense0.value[23]*dense1.value[1]
    -dense0.value[30]*dense1.value[8]
    -dense0.value[31]*dense1.value[9]
;
    dense.value[23] =
    -dense0.value[8]*dense1.value[31]
    -dense0.value[31]*dense1.value[8]
;
    dense.value[24] =
    +dense0.value[1]*dense1.value[25]
    +dense0.value[2]*dense1.value[26]
    -dense0.value[3]*dense1.value[27]
    +dense0.value[4]*dense1.value[28]
    -dense0.value[5]*dense1.value[29]
    -dense0.value[6]*dense1.value[30]
    -dense0.value[7]*dense1.value[31]
    +dense0.value[25]*dense1.value[1]
    +dense0.value[26]*dense1.value[2]
    -dense0.value[27]*dense1.value[3]
    +dense0.value[28]*dense1.value[4]
    -dense0.value[29]*dense1.value[5]
    -dense0.value[30]*dense1.value[6]
    -dense0.value[31]*dense1.value[7]
;
    dense.value[25] =
    -dense0.value[2]*dense1.value[27]
    -dense0.value[4]*dense1.value[29]
    -dense0.value[6]*dense1.value[31]
    +dense0.value[27]*dense1.value[2]
    +dense0.value[29]*dense1.value[4]
    -dense0.value[31]*dense1.value[6]
;
    dense.value[26] =
    +dense0.value[1]*dense1.value[27]
    -dense0.value[4]*dense1.value[30]
    +dense0.value[5]*dense1.value[31]
    -dense0.value[27]*dense1.value[1]
    +dense0.value[30]*dense1.value[4]
    +dense0.value[31]*dense1.value[5]
;
    dense.value[27] =
    +dense0.value[4]*dense1.value[31]
    +dense0.value[31]*dense1.value[4]
;
    dense.value[28] =
    +dense0.value[1]*dense1.value[29]
    +dense0.value[2]*dense1.value[30]
    -dense0.value[3]*dense1.value[31]
    -dense0.value[29]*dense1.value[1]
    -dense0.value[30]*dense1.value[2]
    -dense0.value[31]*dense1.value[3]
;
    dense.value[29] =
    -dense0.value[2]*dense1.value[31]
    -dense0.value[31]*dense1.value[2]
;
    dense.value[30] =
    +dense0.value[1]*dense1.value[31]
    +dense0.value[31]*dense1.value[1]
;
    return dense;
}

// grade projection of the product of two multivectors <ab>_r
#define GEN1_DENSE_GRADE0INNERPRODUCT(dense,dense0,dense1){\
    dense.value[0] =\
    +dense0.value[1]*dense1.value[1]\
    +dense0.value[2]*dense1.value[2]\
    -dense0.value[3]*dense1.value[3]\
    +dense0.value[4]*dense1.value[4]\
    -dense0.value[5]*dense1.value[5]\
    -dense0.value[6]*dense1.value[6]\
    -dense0.value[7]*dense1.value[7]\
    +dense0.value[8]*dense1.value[8]\
    -dense0.value[9]*dense1.value[9]\
    -dense0.value[10]*dense1.value[10]\
    -dense0.value[11]*dense1.value[11]\
    -dense0.value[12]*dense1.value[12]\
    -dense0.value[13]*dense1.value[13]\
    -dense0.value[14]*dense1.value[14]\
    +dense0.value[15]*dense1.value[15]\
    -dense0.value[16]*dense1.value[16]\
    +dense0.value[17]*dense1.value[17]\
    +dense0.value[18]*dense1.value[18]\
    +dense0.value[19]*dense1.value[19]\
    +dense0.value[20]*dense1.value[20]\
    +dense0.value[21]*dense1.value[21]\
    +dense0.value[22]*dense1.value[22]\
    -dense0.value[23]*dense1.value[23]\
    +dense0.value[24]*dense1.value[24]\
    +dense0.value[25]*dense1.value[25]\
    +dense0.value[26]*dense1.value[26]\
    -dense0.value[27]*dense1.value[27]\
    +dense0.value[28]*dense1.value[28]\
    -dense0.value[29]*dense1.value[29]\
    -dense0.value[30]*dense1.value[30]\
    -dense0.value[31]*dense1.value[31]\
;\
}
#define GEN1_DENSE_GRADE1INNERPRODUCT(dense,dense0,dense1){\
    dense.value[1] =\
    -dense0.value[2]*dense1.value[3]\
    +dense0.value[3]*dense1.value[2]\
    -dense0.value[4]*dense1.value[5]\
    +dense0.value[5]*dense1.value[4]\
    -dense0.value[6]*dense1.value[7]\
    -dense0.value[7]*dense1.value[6]\
    -dense0.value[8]*dense1.value[9]\
    +dense0.value[9]*dense1.value[8]\
    -dense0.value[10]*dense1.value[11]\
    -dense0.value[11]*dense1.value[10]\
    -dense0.value[12]*dense1.value[13]\
    -dense0.value[13]*dense1.value[12]\
    +dense0.value[14]*dense1.value[15]\
    -dense0.value[15]*dense1.value[14]\
    +dense0.value[16]*dense1.value[17]\
    -dense0.value[17]*dense1.value[16]\
    +dense0.value[18]*dense1.value[19]\
    +dense0.value[19]*dense1.value[18]\
    +dense0.value[20]*dense1.value[21]\
    +dense0.value[21]*dense1.value[20]\
    -dense0.value[22]*dense1.value[23]\
    +dense0.value[23]*dense1.value[22]\
    +dense0.value[24]*dense1.value[25]\
    +dense0.value[25]*dense1.value[24]\
    -dense0.value[26]*dense1.value[27]\
    +dense0.value[27]*dense1.value[26]\
    -dense0.value[28]*dense1.value[29]\
    +dense0.value[29]*dense1.value[28]\
    -dense0.value[30]*dense1.value[31]\
    -dense0.value[31]*dense1.value[30]\
;\
    dense.value[2] =\
    +dense0.value[1]*dense1.value[3]\
    -dense0.value[3]*dense1.value[1]\
    -dense0.value[4]*dense1.value[6]\
    +dense0.value[5]*dense1.value[7]\
    +dense0.value[6]*dense1.value[4]\
    +dense0.value[7]*dense1.value[5]\
    -dense0.value[8]*dense1.value[10]\
    +dense0.value[9]*dense1.value[11]\
    +dense0.value[10]*dense1.value[8]\
    +dense0.value[11]*dense1.value[9]\
    -dense0.value[12]*dense1.value[14]\
    -dense0.value[13]*dense1.value[15]\
    -dense0.value[14]*dense1.value[12]\
    +dense0.value[15]*dense1.value[13]\
    +dense0.value[16]*dense1.value[18]\
    -dense0.value[17]*dense1.value[19]\
    -dense0.value[18]*dense1.value[16]\
    -dense0.value[19]*dense1.value[17]\
    +dense0.value[20]*dense1.value[22]\
    +dense0.value[21]*dense1.value[23]\
    +dense0.value[22]*dense1.value[20]\
    -dense0.value[23]*dense1.value[21]\
    +dense0.value[24]*dense1.value[26]\
    +dense0.value[25]*dense1.value[27]\
    +dense0.value[26]*dense1.value[24]\
    -dense0.value[27]*dense1.value[25]\
    -dense0.value[28]*dense1.value[30]\
    +dense0.value[29]*dense1.value[31]\
    +dense0.value[30]*dense1.value[28]\
    +dense0.value[31]*dense1.value[29]\
;\
    dense.value[4] =\
    +dense0.value[1]*dense1.value[5]\
    +dense0.value[2]*dense1.value[6]\
    -dense0.value[3]*dense1.value[7]\
    -dense0.value[5]*dense1.value[1]\
    -dense0.value[6]*dense1.value[2]\
    -dense0.value[7]*dense1.value[3]\
    -dense0.value[8]*dense1.value[12]\
    +dense0.value[9]*dense1.value[13]\
    +dense0.value[10]*dense1.value[14]\
    +dense0.value[11]*dense1.value[15]\
    +dense0.value[12]*dense1.value[8]\
    +dense0.value[13]*dense1.value[9]\
    +dense0.value[14]*dense1.value[10]\
    -dense0.value[15]*dense1.value[11]\
    +dense0.value[16]*dense1.value[20]\
    -dense0.value[17]*dense1.value[21]\
    -dense0.value[18]*dense1.value[22]\
    -dense0.value[19]*dense1.value[23]\
    -dense0.value[20]*dense1.value[16]\
    -dense0.value[21]*dense1.value[17]\
    -dense0.value[22]*dense1.value[18]\
    +dense0.value[23]*dense1.value[19]\
    +dense0.value[24]*dense1.value[28]\
    +dense0.value[25]*dense1.value[29]\
    +dense0.value[26]*dense1.value[30]\
    -dense0.value[27]*dense1.value[31]\
    +dense0.value[28]*dense1.value[24]\
    -dense0.value[29]*dense1.value[25]\
    -dense0.value[30]*dense1.value[26]\
    -dense0.value[31]*dense1.value[27]\
;\
    dense.value[8] =\
    +dense0.value[1]*dense1.value[9]\
    +dense0.value[2]*dense1.value[10]\
    -dense0.value[3]*dense1.value[11]\
    +dense0.value[4]*dense1.value[12]\
    -dense0.value[5]*dense1.value[13]\
    -dense0.value[6]*dense1.value[14]\
    -dense0.value[7]*dense1.value[15]\
    -dense0.value[9]*dense1.value[1]\
    -dense0.value[10]*dense1.value[2]\
    -dense0.value[11]*dense1.value[3]\
    -dense0.value[12]*dense1.value[4]\
    -dense0.value[13]*dense1.value[5]\
    -dense0.value[14]*dense1.value[6]\
    +dense0.value[15]*dense1.value[7]\
    +dense0.value[16]*dense1.value[24]\
    -dense0.value[17]*dense1.value[25]\
    -dense0.value[18]*dense1.value[26]\
    -dense0.value[19]*dense1.value[27]\
    -dense0.value[20]*dense1.value[28]\
    -dense0.value[21]*dense1.value[29]\
    -dense0.value[22]*dense1.value[30]\
    +dense0.value[23]*dense1.value[31]\
    -dense0.value[24]*dense1.value[16]\
    -dense0.value[25]*dense1.value[17]\
    -dense0.value[26]*dense1.value[18]\
    +dense0.value[27]*dense1.value[19]\
    -dense0.value[28]*dense1.value[20]\
    +dense0.value[29]*dense1.value[21]\
    +dense0.value[30]*dense1.value[22]\
    +dense0.value[31]*dense1.value[23]\
;\
    dense.value[16] =\
    +dense0.value[1]*dense1.value[17]\
    +dense0.value[2]*dense1.value[18]\
    -dense0.value[3]*dense1.value[19]\
    +dense0.value[4]*dense1.value[20]\
    -dense0.value[5]*dense1.value[21]\
    -dense0.value[6]*dense1.value[22]\
    -dense0.value[7]*dense1.value[23]\
    +dense0.value[8]*dense1.value[24]\
    -dense0.value[9]*dense1.value[25]\
    -dense0.value[10]*dense1.value[26]\
    -dense0.value[11]*dense1.value[27]\
    -dense0.value[12]*dense1.value[28]\
    -dense0.value[13]*dense1.value[29]\
    -dense0.value[14]*dense1.value[30]\
    +dense0.value[15]*dense1.value[31]\
    -dense0.value[17]*dense1.value[1]\
    -dense0.value[18]*dense1.value[2]\
    -dense0.value[19]*dense1.value[3]\
    -dense0.value[20]*dense1.value[4]\
    -dense0.value[21]*dense1.value[5]\
    -dense0.value[22]*dense1.value[6]\
    +dense0.value[23]*dense1.value[7]\
    -dense0.value[24]*dense1.value[8]\
    -dense0.value[25]*dense1.value[9]\
    -dense0.value[26]*dense1.value[10]\
    +dense0.value[27]*dense1.value[11]\
    -dense0.value[28]*dense1.value[12]\
    +dense0.value[29]*dense1.value[13]\
    +dense0.value[30]*dense1.value[14]\
    +dense0.value[31]*dense1.value[15]\
;\
}
#define GEN1_DENSE_GRADE2INNERPRODUCT(dense,dense0,dense1){\
    dense.value[3] =\
    +dense0.value[4]*dense1.value[7]\
    +dense0.value[7]*dense1.value[4]\
    +dense0.value[8]*dense1.value[11]\
    +dense0.value[11]*dense1.value[8]\
    -dense0.value[12]*dense1.value[15]\
    -dense0.value[15]*dense1.value[12]\
    -dense0.value[16]*dense1.value[19]\
    -dense0.value[19]*dense1.value[16]\
    +dense0.value[20]*dense1.value[23]\
    +dense0.value[23]*dense1.value[20]\
    +dense0.value[24]*dense1.value[27]\
    +dense0.value[27]*dense1.value[24]\
    +dense0.value[28]*dense1.value[31]\
    +dense0.value[31]*dense1.value[28]\
;\
    dense.value[5] =\
    -dense0.value[2]*dense1.value[7]\
    -dense0.value[7]*dense1.value[2]\
    +dense0.value[8]*dense1.value[13]\
    +dense0.value[10]*dense1.value[15]\
    +dense0.value[13]*dense1.value[8]\
    +dense0.value[15]*dense1.value[10]\
    -dense0.value[16]*dense1.value[21]\
    -dense0.value[18]*dense1.value[23]\
    -dense0.value[21]*dense1.value[16]\
    -dense0.value[23]*dense1.value[18]\
    +dense0.value[24]*dense1.value[29]\
    -dense0.value[26]*dense1.value[31]\
    +dense0.value[29]*dense1.value[24]\
    -dense0.value[31]*dense1.value[26]\
;\
    dense.value[6] =\
    +dense0.value[1]*dense1.value[7]\
    +dense0.value[7]*dense1.value[1]\
    +dense0.value[8]*dense1.value[14]\
    -dense0.value[9]*dense1.value[15]\
    +dense0.value[14]*dense1.value[8]\
    -dense0.value[15]*dense1.value[9]\
    -dense0.value[16]*dense1.value[22]\
    +dense0.value[17]*dense1.value[23]\
    -dense0.value[22]*dense1.value[16]\
    +dense0.value[23]*dense1.value[17]\
    +dense0.value[24]*dense1.value[30]\
    +dense0.value[25]*dense1.value[31]\
    +dense0.value[30]*dense1.value[24]\
    +dense0.value[31]*dense1.value[25]\
;\
    dense.value[9] =\
    -dense0.value[2]*dense1.value[11]\
    -dense0.value[4]*dense1.value[13]\
    -dense0.value[6]*dense1.value[15]\
    -dense0.value[11]*dense1.value[2]\
    -dense0.value[13]*dense1.value[4]\
    -dense0.value[15]*dense1.value[6]\
    -dense0.value[16]*dense1.value[25]\
    -dense0.value[18]*dense1.value[27]\
    -dense0.value[20]*dense1.value[29]\
    +dense0.value[22]*dense1.value[31]\
    -dense0.value[25]*dense1.value[16]\
    -dense0.value[27]*dense1.value[18]\
    -dense0.value[29]*dense1.value[20]\
    +dense0.value[31]*dense1.value[22]\
;\
    dense.value[10] =\
    +dense0.value[1]*dense1.value[11]\
    -dense0.value[4]*dense1.value[14]\
    +dense0.value[5]*dense1.value[15]\
    +dense0.value[11]*dense1.value[1]\
    -dense0.value[14]*dense1.value[4]\
    +dense0.value[15]*dense1.value[5]\
    -dense0.value[16]*dense1.value[26]\
    +dense0.value[17]*dense1.value[27]\
    -dense0.value[20]*dense1.value[30]\
    -dense0.value[21]*dense1.value[31]\
    -dense0.value[26]*dense1.value[16]\
    +dense0.value[27]*dense1.value[17]\
    -dense0.value[30]*dense1.value[20]\
    -dense0.value[31]*dense1.value[21]\
;\
    dense.value[12] =\
    +dense0.value[1]*dense1.value[13]\
    +dense0.value[2]*dense1.value[14]\
    -dense0.value[3]*dense1.value[15]\
    +dense0.value[13]*dense1.value[1]\
    +dense0.value[14]*dense1.value[2]\
    -dense0.value[15]*dense1.value[3]\
    -dense0.value[16]*dense1.value[28]\
    +dense0.value[17]*dense1.value[29]\
    +dense0.value[18]*dense1.value[30]\
    +dense0.value[19]*dense1.value[31]\
    -dense0.value[28]*dense1.value[16]\
    +dense0.value[29]*dense1.value[17]\
    +dense0.value[30]*dense1.value[18]\
    +dense0.value[31]*dense1.value[19]\
;\
    dense.value[17] =\
    -dense0.value[2]*dense1.value[19]\
    -dense0.value[4]*dense1.value[21]\
    -dense0.value[6]*dense1.value[23]\
    -dense0.value[8]*dense1.value[25]\
    -dense0.value[10]*dense1.value[27]\
    -dense0.value[12]*dense1.value[29]\
    +dense0.value[14]*dense1.value[31]\
    -dense0.value[19]*dense1.value[2]\
    -dense0.value[21]*dense1.value[4]\
    -dense0.value[23]*dense1.value[6]\
    -dense0.value[25]*dense1.value[8]\
    -dense0.value[27]*dense1.value[10]\
    -dense0.value[29]*dense1.value[12]\
    +dense0.value[31]*dense1.value[14]\
;\
    dense.value[18] =\
    +dense0.value[1]*dense1.value[19]\
    -dense0.value[4]*dense1.value[22]\
    +dense0.value[5]*dense1.value[23]\
    -dense0.value[8]*dense1.value[26]\
    +dense0.value[9]*dense1.value[27]\
    -dense0.value[12]*dense1.value[30]\
    -dense0.value[13]*dense1.value[31]\
    +dense0.value[19]*dense1.value[1]\
    -dense0.value[22]*dense1.value[4]\
    +dense0.value[23]*dense1.value[5]\
    -dense0.value[26]*dense1.value[8]\
    +dense0.value[27]*dense1.value[9]\
    -dense0.value[30]*dense1.value[12]\
    -dense0.value[31]*dense1.value[13]\
;\
    dense.value[20] =\
    +dense0.value[1]*dense1.value[21]\
    +dense0.value[2]*dense1.value[22]\
    -dense0.value[3]*dense1.value[23]\
    -dense0.value[8]*dense1.value[28]\
    +dense0.value[9]*dense1.value[29]\
    +dense0.value[10]*dense1.value[30]\
    +dense0.value[11]*dense1.value[31]\
    +dense0.value[21]*dense1.value[1]\
    +dense0.value[22]*dense1.value[2]\
    -dense0.value[23]*dense1.value[3]\
    -dense0.value[28]*dense1.value[8]\
    +dense0.value[29]*dense1.value[9]\
    +dense0.value[30]*dense1.value[10]\
    +dense0.value[31]*dense1.value[11]\
;\
    dense.value[24] =\
    +dense0.value[1]*dense1.value[25]\
    +dense0.value[2]*dense1.value[26]\
    -dense0.value[3]*dense1.value[27]\
    +dense0.value[4]*dense1.value[28]\
    -dense0.value[5]*dense1.value[29]\
    -dense0.value[6]*dense1.value[30]\
    -dense0.value[7]*dense1.value[31]\
    +dense0.value[25]*dense1.value[1]\
    +dense0.value[26]*dense1.value[2]\
    -dense0.value[27]*dense1.value[3]\
    +dense0.value[28]*dense1.value[4]\
    -dense0.value[29]*dense1.value[5]\
    -dense0.value[30]*dense1.value[6]\
    -dense0.value[31]*dense1.value[7]\
;\
}
#define GEN1_DENSE_GRADE3INNERPRODUCT(dense,dense0,dense1){\
    dense.value[7] =\
    -dense0.value[8]*dense1.value[15]\
    +dense0.value[15]*dense1.value[8]\
    +dense0.value[16]*dense1.value[23]\
    -dense0.value[23]*dense1.value[16]\
    +dense0.value[24]*dense1.value[31]\
    +dense0.value[31]*dense1.value[24]\
;\
    dense.value[11] =\
    +dense0.value[4]*dense1.value[15]\
    -dense0.value[15]*dense1.value[4]\
    +dense0.value[16]*dense1.value[27]\
    -dense0.value[20]*dense1.value[31]\
    -dense0.value[27]*dense1.value[16]\
    -dense0.value[31]*dense1.value[20]\
;\
    dense.value[13] =\
    -dense0.value[2]*dense1.value[15]\
    +dense0.value[15]*dense1.value[2]\
    +dense0.value[16]*dense1.value[29]\
    +dense0.value[18]*dense1.value[31]\
    -dense0.value[29]*dense1.value[16]\
    +dense0.value[31]*dense1.value[18]\
;\
    dense.value[14] =\
    +dense0.value[1]*dense1.value[15]\
    -dense0.value[15]*dense1.value[1]\
    +dense0.value[16]*dense1.value[30]\
    -dense0.value[17]*dense1.value[31]\
    -dense0.value[30]*dense1.value[16]\
    -dense0.value[31]*dense1.value[17]\
;\
    dense.value[19] =\
    +dense0.value[4]*dense1.value[23]\
    +dense0.value[8]*dense1.value[27]\
    -dense0.value[12]*dense1.value[31]\
    -dense0.value[23]*dense1.value[4]\
    -dense0.value[27]*dense1.value[8]\
    -dense0.value[31]*dense1.value[12]\
;\
    dense.value[21] =\
    -dense0.value[2]*dense1.value[23]\
    +dense0.value[8]*dense1.value[29]\
    +dense0.value[10]*dense1.value[31]\
    +dense0.value[23]*dense1.value[2]\
    -dense0.value[29]*dense1.value[8]\
    +dense0.value[31]*dense1.value[10]\
;\
    dense.value[22] =\
    +dense0.value[1]*dense1.value[23]\
    +dense0.value[8]*dense1.value[30]\
    -dense0.value[9]*dense1.value[31]\
    -dense0.value[23]*dense1.value[1]\
    -dense0.value[30]*dense1.value[8]\
    -dense0.value[31]*dense1.value[9]\
;\
    dense.value[25] =\
    -dense0.value[2]*dense1.value[27]\
    -dense0.value[4]*dense1.value[29]\
    -dense0.value[6]*dense1.value[31]\
    +dense0.value[27]*dense1.value[2]\
    +dense0.value[29]*dense1.value[4]\
    -dense0.value[31]*dense1.value[6]\
;\
    dense.value[26] =\
    +dense0.value[1]*dense1.value[27]\
    -dense0.value[4]*dense1.value[30]\
    +dense0.value[5]*dense1.value[31]\
    -dense0.value[27]*dense1.value[1]\
    +dense0.value[30]*dense1.value[4]\
    +dense0.value[31]*dense1.value[5]\
;\
    dense.value[28] =\
    +dense0.value[1]*dense1.value[29]\
    +dense0.value[2]*dense1.value[30]\
    -dense0.value[3]*dense1.value[31]\
    -dense0.value[29]*dense1.value[1]\
    -dense0.value[30]*dense1.value[2]\
    -dense0.value[31]*dense1.value[3]\
;\
}
#define GEN1_DENSE_GRADE4INNERPRODUCT(dense,dense0,dense1){\
    dense.value[15] =\
    -dense0.value[16]*dense1.value[31]\
    -dense0.value[31]*dense1.value[16]\
;\
    dense.value[23] =\
    -dense0.value[8]*dense1.value[31]\
    -dense0.value[31]*dense1.value[8]\
;\
    dense.value[27] =\
    +dense0.value[4]*dense1.value[31]\
    +dense0.value[31]*dense1.value[4]\
;\
    dense.value[29] =\
    -dense0.value[2]*dense1.value[31]\
    -dense0.value[31]*dense1.value[2]\
;\
    dense.value[30] =\
    +dense0.value[1]*dense1.value[31]\
    +dense0.value[31]*dense1.value[1]\
;\
}
#define GEN1_DENSE_GRADE5INNERPRODUCT(dense,dense0,dense1){\
}

static gen1_DenseMultivector gen1_dense_gradeinnerproduct(gen1_DenseMultivector dense0, gen1_DenseMultivector dense1, int *grades, Py_ssize_t size){
    gen1_DenseMultivector dense = {{0}};
    for(Py_ssize_t i = 0; i < size; i++){
        switch(grades[i]){
            case 0:
                GEN1_DENSE_GRADE0INNERPRODUCT(dense,dense0,dense1);
                break;
            case 1:
                GEN1_DENSE_GRADE1INNERPRODUCT(dense,dense0,dense1);
                break;
            case 2:
                GEN1_DENSE_GRADE2INNERPRODUCT(dense,dense0,dense1);
                break;
            case 3:
                GEN1_DENSE_GRADE3INNERPRODUCT(dense,dense0,dense1);
                break;
            case 4:
                GEN1_DENSE_GRADE4INNERPRODUCT(dense,dense0,dense1);
                break;
            case 5:
                GEN1_DENSE_GRADE5INNERPRODUCT(dense,dense0,dense1);
                break;
            default:// do nothing for different values
                break;
        }
    }
    return dense;
}


static gen1_DenseMultivector gen1_dense_outerproduct(gen1_DenseMultivector dense0, gen1_DenseMultivector dense1){
    gen1_DenseMultivector dense = {{0}};
    dense.value[0] =
    +dense0.value[0]*dense1.value[0]
;
    dense.value[1] =
    +dense0.value[0]*dense1.value[1]
    +dense0.value[1]*dense1.value[0]
;
    dense.value[2] =
    +dense0.value[0]*dense1.value[2]
    +dense0.value[2]*dense1.value[0]
;
    dense.value[3] =
    +dense0.value[0]*dense1.value[3]
    +dense0.value[1]*dense1.value[2]
    -dense0.value[2]*dense1.value[1]
    +dense0.value[3]*dense1.value[0]
;
    dense.value[4] =
    +dense0.value[0]*dense1.value[4]
    +dense0.value[4]*dense1.value[0]
;
    dense.value[5] =
    +dense0.value[0]*dense1.value[5]
    +dense0.value[1]*dense1.value[4]
    -dense0.value[4]*dense1.value[1]
    +dense0.value[5]*dense1.value[0]
;
    dense.value[6] =
    +dense0.value[0]*dense1.value[6]
    +dense0.value[2]*dense1.value[4]
    -dense0.value[4]*dense1.value[2]
    +dense0.value[6]*dense1.value[0]
;
    dense.value[7] =
    +dense0.value[0]*dense1.value[7]
    +dense0.value[1]*dense1.value[6]
    -dense0.value[2]*dense1.value[5]
    +dense0.value[3]*dense1.value[4]
    +dense0.value[4]*dense1.value[3]
    -dense0.value[5]*dense1.value[2]
    +dense0.value[6]*dense1.value[1]
    +dense0.value[7]*dense1.value[0]
;
    dense.value[8] =
    +dense0.value[0]*dense1.value[8]
    +dense0.value[8]*dense1.value[0]
;
    dense.value[9] =
    +dense0.value[0]*dense1.value[9]
    +dense0.value[1]*dense1.value[8]
    -dense0.value[8]*dense1.value[1]
    +dense0.value[9]*dense1.value[0]
;
    dense.value[10] =
    +dense0.value[0]*dense1.value[10]
    +dense0.value[2]*dense1.value[8]
    -dense0.value[8]*dense1.value[2]
    +dense0.value[10]*dense1.value[0]
;
    dense.value[11] =
    +dense0.value[0]*dense1.value[11]
    +dense0.value[1]*dense1.value[10]
    -dense0.value[2]*dense1.value[9]
    +dense0.value[3]*dense1.value[8]
    +dense0.value[8]*dense1.value[3]
    -dense0.value[9]*dense1.value[2]
    +dense0.value[10]*dense1.value[1]
    +dense0.value[11]*dense1.value[0]
;
    dense.value[12] =
    +dense0.value[0]*dense1.value[12]
    +dense0.value[4]*dense1.value[8]
    -dense0.value[8]*dense1.value[4]
    +dense0.value[12]*dense1.value[0]
;
    dense.value[13] =
    +dense0.value[0]*dense1.value[13]
    +dense0.value[1]*dense1.value[12]
    -dense0.value[4]*dense1.value[9]
    +dense0.value[5]*dense1.value[8]
    +dense0.value[8]*dense1.value[5]
    -dense0.value[9]*dense1.value[4]
    +dense0.value[12]*dense1.value[1]
    +dense0.value[13]*dense1.value[0]
;
    dense.value[14] =
    +dense0.value[0]*dense1.value[14]
    +dense0.value[2]*dense1.value[12]
    -dense0.value[4]*dense1.value[10]
    +dense0.value[6]*dense1.value[8]
    +dense0.value[8]*dense1.value[6]
    -dense0.value[10]*dense1.value[4]
    +dense0.value[12]*dense1.value[2]
    +dense0.value[14]*dense1.value[0]
;
    dense.value[15] =
    +dense0.value[0]*dense1.value[15]
    +dense0.value[1]*dense1.value[14]
    -dense0.value[2]*dense1.value[13]
    +dense0.value[3]*dense1.value[12]
    +dense0.value[4]*dense1.value[11]
    -dense0.value[5]*dense1.value[10]
    +dense0.value[6]*dense1.value[9]
    +dense0.value[7]*dense1.value[8]
    -dense0.value[8]*dense1.value[7]
    +dense0.value[9]*dense1.value[6]
    -dense0.value[10]*dense1.value[5]
    -dense0.value[11]*dense1.value[4]
    +dense0.value[12]*dense1.value[3]
    +dense0.value[13]*dense1.value[2]
    -dense0.value[14]*dense1.value[1]
    +dense0.value[15]*dense1.value[0]
;
    dense.value[16] =
    +dense0.value[0]*dense1.value[16]
    +dense0.value[16]*dense1.value[0]
;
    dense.value[17] =
    +dense0.value[0]*dense1.value[17]
    +dense0.value[1]*dense1.value[16]
    -dense0.value[16]*dense1.value[1]
    +dense0.value[17]*dense1.value[0]
;
    dense.value[18] =
    +dense0.value[0]*dense1.value[18]
    +dense0.value[2]*dense1.value[16]
    -dense0.value[16]*dense1.value[2]
    +dense0.value[18]*dense1.value[0]
;
    dense.value[19] =
    +dense0.value[0]*dense1.value[19]
    +dense0.value[1]*dense1.value[18]
    -dense0.value[2]*dense1.value[17]
    +dense0.value[3]*dense1.value[16]
    +dense0.value[16]*dense1.value[3]
    -dense0.value[17]*dense1.value[2]
    +dense0.value[18]*dense1.value[1]
    +dense0.value[19]*dense1.value[0]
;
    dense.value[20] =
    +dense0.value[0]*dense1.value[20]
    +dense0.value[4]*dense1.value[16]
    -dense0.value[16]*dense1.value[4]
    +dense0.value[20]*dense1.value[0]
;
    dense.value[21] =
    +dense0.value[0]*dense1.value[21]
    +dense0.value[1]*dense1.value[20]
    -dense0.value[4]*dense1.value[17]
    +dense0.value[5]*dense1.value[16]
    +dense0.value[16]*dense1.value[5]
    -dense0.value[17]*dense1.value[4]
    +dense0.value[20]*dense1.value[1]
    +dense0.value[21]*dense1.value[0]
;
    dense.value[22] =
    +dense0.value[0]*dense1.value[22]
    +dense0.value[2]*dense1.value[20]
    -dense0.value[4]*dense1.value[18]
    +dense0.value[6]*dense1.value[16]
    +dense0.value[16]*dense1.value[6]
    -dense0.value[18]*dense1.value[4]
    +dense0.value[20]*dense1.value[2]
    +dense0.value[22]*dense1.value[0]
;
    dense.value[23] =
    +dense0.value[0]*dense1.value[23]
    +dense0.value[1]*dense1.value[22]
    -dense0.value[2]*dense1.value[21]
    +dense0.value[3]*dense1.value[20]
    +dense0.value[4]*dense1.value[19]
    -dense0.value[5]*dense1.value[18]
    +dense0.value[6]*dense1.value[17]
    +dense0.value[7]*dense1.value[16]
    -dense0.value[16]*dense1.value[7]
    +dense0.value[17]*dense1.value[6]
    -dense0.value[18]*dense1.value[5]
    -dense0.value[19]*dense1.value[4]
    +dense0.value[20]*dense1.value[3]
    +dense0.value[21]*dense1.value[2]
    -dense0.value[22]*dense1.value[1]
    +dense0.value[23]*dense1.value[0]
;
    dense.value[24] =
    +dense0.value[0]*dense1.value[24]
    +dense0.value[8]*dense1.value[16]
    -dense0.value[16]*dense1.value[8]
    +dense0.value[24]*dense1.value[0]
;
    dense.value[25] =
    +dense0.value[0]*dense1.value[25]
    +dense0.value[1]*dense1.value[24]
    -dense0.value[8]*dense1.value[17]
    +dense0.value[9]*dense1.value[16]
    +dense0.value[16]*dense1.value[9]
    -dense0.value[17]*dense1.value[8]
    +dense0.value[24]*dense1.value[1]
    +dense0.value[25]*dense1.value[0]
;
    dense.value[26] =
    +dense0.value[0]*dense1.value[26]
    +dense0.value[2]*dense1.value[24]
    -dense0.value[8]*dense1.value[18]
    +dense0.value[10]*dense1.value[16]
    +dense0.value[16]*dense1.value[10]
    -dense0.value[18]*dense1.value[8]
    +dense0.value[24]*dense1.value[2]
    +dense0.value[26]*dense1.value[0]
;
    dense.value[27] =
    +dense0.value[0]*dense1.value[27]
    +dense0.value[1]*dense1.value[26]
    -dense0.value[2]*dense1.value[25]
    +dense0.value[3]*dense1.value[24]
    +dense0.value[8]*dense1.value[19]
    -dense0.value[9]*dense1.value[18]
    +dense0.value[10]*dense1.value[17]
    +dense0.value[11]*dense1.value[16]
    -dense0.value[16]*dense1.value[11]
    +dense0.value[17]*dense1.value[10]
    -dense0.value[18]*dense1.value[9]
    -dense0.value[19]*dense1.value[8]
    +dense0.value[24]*dense1.value[3]
    +dense0.value[25]*dense1.value[2]
    -dense0.value[26]*dense1.value[1]
    +dense0.value[27]*dense1.value[0]
;
    dense.value[28] =
    +dense0.value[0]*dense1.value[28]
    +dense0.value[4]*dense1.value[24]
    -dense0.value[8]*dense1.value[20]
    +dense0.value[12]*dense1.value[16]
    +dense0.value[16]*dense1.value[12]
    -dense0.value[20]*dense1.value[8]
    +dense0.value[24]*dense1.value[4]
    +dense0.value[28]*dense1.value[0]
;
    dense.value[29] =
    +dense0.value[0]*dense1.value[29]
    +dense0.value[1]*dense1.value[28]
    -dense0.value[4]*dense1.value[25]
    +dense0.value[5]*dense1.value[24]
    +dense0.value[8]*dense1.value[21]
    -dense0.value[9]*dense1.value[20]
    +dense0.value[12]*dense1.value[17]
    +dense0.value[13]*dense1.value[16]
    -dense0.value[16]*dense1.value[13]
    +dense0.value[17]*dense1.value[12]
    -dense0.value[20]*dense1.value[9]
    -dense0.value[21]*dense1.value[8]
    +dense0.value[24]*dense1.value[5]
    +dense0.value[25]*dense1.value[4]
    -dense0.value[28]*dense1.value[1]
    +dense0.value[29]*dense1.value[0]
;
    dense.value[30] =
    +dense0.value[0]*dense1.value[30]
    +dense0.value[2]*dense1.value[28]
    -dense0.value[4]*dense1.value[26]
    +dense0.value[6]*dense1.value[24]
    +dense0.value[8]*dense1.value[22]
    -dense0.value[10]*dense1.value[20]
    +dense0.value[12]*dense1.value[18]
    +dense0.value[14]*dense1.value[16]
    -dense0.value[16]*dense1.value[14]
    +dense0.value[18]*dense1.value[12]
    -dense0.value[20]*dense1.value[10]
    -dense0.value[22]*dense1.value[8]
    +dense0.value[24]*dense1.value[6]
    +dense0.value[26]*dense1.value[4]
    -dense0.value[28]*dense1.value[2]
    +dense0.value[30]*dense1.value[0]
;
    dense.value[31] =
    +dense0.value[0]*dense1.value[31]
    +dense0.value[1]*dense1.value[30]
    -dense0.value[2]*dense1.value[29]
    +dense0.value[3]*dense1.value[28]
    +dense0.value[4]*dense1.value[27]
    -dense0.value[5]*dense1.value[26]
    +dense0.value[6]*dense1.value[25]
    +dense0.value[7]*dense1.value[24]
    -dense0.value[8]*dense1.value[23]
    +dense0.value[9]*dense1.value[22]
    -dense0.value[10]*dense1.value[21]
    -dense0.value[11]*dense1.value[20]
    +dense0.value[12]*dense1.value[19]
    +dense0.value[13]*dense1.value[18]
    -dense0.value[14]*dense1.value[17]
    +dense0.value[15]*dense1.value[16]
    +dense0.value[16]*dense1.value[15]
    -dense0.value[17]*dense1.value[14]
    +dense0.value[18]*dense1.value[13]
    +dense0.value[19]*dense1.value[12]
    -dense0.value[20]*dense1.value[11]
    -dense0.value[21]*dense1.value[10]
    +dense0.value[22]*dense1.value[9]
    -dense0.value[23]*dense1.value[8]
    +dense0.value[24]*dense1.value[7]
    +dense0.value[25]*dense1.value[6]
    -dense0.value[26]*dense1.value[5]
    +dense0.value[27]*dense1.value[4]
    +dense0.value[28]*dense1.value[3]
    -dense0.value[29]*dense1.value[2]
    +dense0.value[30]*dense1.value[1]
    +dense0.value[31]*dense1.value[0]
;
    return dense;
}

// grade projection of the product of two multivectors <ab>_r
#define GEN1_DENSE_GRADE0OUTERPRODUCT(dense,dense0,dense1){\
    dense.value[0] =\
    +dense0.value[0]*dense1.value[0]\
;\
}
#define GEN1_DENSE_GRADE1OUTERPRODUCT(dense,dense0,dense1){\
    dense.value[1] =\
    +dense0.value[0]*dense1.value[1]\
    +dense0.value[1]*dense1.value[0]\
;\
    dense.value[2] =\
    +dense0.value[0]*dense1.value[2]\
    +dense0.value[2]*dense1.value[0]\
;\
    dense.value[4] =\
    +dense0.value[0]*dense1.value[4]\
    +dense0.value[4]*dense1.value[0]\
;\
    dense.value[8] =\
    +dense0.value[0]*dense1.value[8]\
    +dense0.value[8]*dense1.value[0]\
;\
    dense.value[16] =\
    +dense0.value[0]*dense1.value[16]\
    +dense0.value[16]*dense1.value[0]\
;\
}
#define GEN1_DENSE_GRADE2OUTERPRODUCT(dense,dense0,dense1){\
    dense.value[3] =\
    +dense0.value[0]*dense1.value[3]\
    +dense0.value[1]*dense1.value[2]\
    -dense0.value[2]*dense1.value[1]\
    +dense0.value[3]*dense1.value[0]\
;\
    dense.value[5] =\
    +dense0.value[0]*dense1.value[5]\
    +dense0.value[1]*dense1.value[4]\
    -dense0.value[4]*dense1.value[1]\
    +dense0.value[5]*dense1.value[0]\
;\
    dense.value[6] =\
    +dense0.value[0]*dense1.value[6]\
    +dense0.value[2]*dense1.value[4]\
    -dense0.value[4]*dense1.value[2]\
    +dense0.value[6]*dense1.value[0]\
;\
    dense.value[9] =\
    +dense0.value[0]*dense1.value[9]\
    +dense0.value[1]*dense1.value[8]\
    -dense0.value[8]*dense1.value[1]\
    +dense0.value[9]*dense1.value[0]\
;\
    dense.value[10] =\
    +dense0.value[0]*dense1.value[10]\
    +dense0.value[2]*dense1.value[8]\
    -dense0.value[8]*dense1.value[2]\
    +dense0.value[10]*dense1.value[0]\
;\
    dense.value[12] =\
    +dense0.value[0]*dense1.value[12]\
    +dense0.value[4]*dense1.value[8]\
    -dense0.value[8]*dense1.value[4]\
    +dense0.value[12]*dense1.value[0]\
;\
    dense.value[17] =\
    +dense0.value[0]*dense1.value[17]\
    +dense0.value[1]*dense1.value[16]\
    -dense0.value[16]*dense1.value[1]\
    +dense0.value[17]*dense1.value[0]\
;\
    dense.value[18] =\
    +dense0.value[0]*dense1.value[18]\
    +dense0.value[2]*dense1.value[16]\
    -dense0.value[16]*dense1.value[2]\
    +dense0.value[18]*dense1.value[0]\
;\
    dense.value[20] =\
    +dense0.value[0]*dense1.value[20]\
    +dense0.value[4]*dense1.value[16]\
    -dense0.value[16]*dense1.value[4]\
    +dense0.value[20]*dense1.value[0]\
;\
    dense.value[24] =\
    +dense0.value[0]*dense1.value[24]\
    +dense0.value[8]*dense1.value[16]\
    -dense0.value[16]*dense1.value[8]\
    +dense0.value[24]*dense1.value[0]\
;\
}
#define GEN1_DENSE_GRADE3OUTERPRODUCT(dense,dense0,dense1){\
    dense.value[7] =\
    +dense0.value[0]*dense1.value[7]\
    +dense0.value[1]*dense1.value[6]\
    -dense0.value[2]*dense1.value[5]\
    +dense0.value[3]*dense1.value[4]\
    +dense0.value[4]*dense1.value[3]\
    -dense0.value[5]*dense1.value[2]\
    +dense0.value[6]*dense1.value[1]\
    +dense0.value[7]*dense1.value[0]\
;\
    dense.value[11] =\
    +dense0.value[0]*dense1.value[11]\
    +dense0.value[1]*dense1.value[10]\
    -dense0.value[2]*dense1.value[9]\
    +dense0.value[3]*dense1.value[8]\
    +dense0.value[8]*dense1.value[3]\
    -dense0.value[9]*dense1.value[2]\
    +dense0.value[10]*dense1.value[1]\
    +dense0.value[11]*dense1.value[0]\
;\
    dense.value[13] =\
    +dense0.value[0]*dense1.value[13]\
    +dense0.value[1]*dense1.value[12]\
    -dense0.value[4]*dense1.value[9]\
    +dense0.value[5]*dense1.value[8]\
    +dense0.value[8]*dense1.value[5]\
    -dense0.value[9]*dense1.value[4]\
    +dense0.value[12]*dense1.value[1]\
    +dense0.value[13]*dense1.value[0]\
;\
    dense.value[14] =\
    +dense0.value[0]*dense1.value[14]\
    +dense0.value[2]*dense1.value[12]\
    -dense0.value[4]*dense1.value[10]\
    +dense0.value[6]*dense1.value[8]\
    +dense0.value[8]*dense1.value[6]\
    -dense0.value[10]*dense1.value[4]\
    +dense0.value[12]*dense1.value[2]\
    +dense0.value[14]*dense1.value[0]\
;\
    dense.value[19] =\
    +dense0.value[0]*dense1.value[19]\
    +dense0.value[1]*dense1.value[18]\
    -dense0.value[2]*dense1.value[17]\
    +dense0.value[3]*dense1.value[16]\
    +dense0.value[16]*dense1.value[3]\
    -dense0.value[17]*dense1.value[2]\
    +dense0.value[18]*dense1.value[1]\
    +dense0.value[19]*dense1.value[0]\
;\
    dense.value[21] =\
    +dense0.value[0]*dense1.value[21]\
    +dense0.value[1]*dense1.value[20]\
    -dense0.value[4]*dense1.value[17]\
    +dense0.value[5]*dense1.value[16]\
    +dense0.value[16]*dense1.value[5]\
    -dense0.value[17]*dense1.value[4]\
    +dense0.value[20]*dense1.value[1]\
    +dense0.value[21]*dense1.value[0]\
;\
    dense.value[22] =\
    +dense0.value[0]*dense1.value[22]\
    +dense0.value[2]*dense1.value[20]\
    -dense0.value[4]*dense1.value[18]\
    +dense0.value[6]*dense1.value[16]\
    +dense0.value[16]*dense1.value[6]\
    -dense0.value[18]*dense1.value[4]\
    +dense0.value[20]*dense1.value[2]\
    +dense0.value[22]*dense1.value[0]\
;\
    dense.value[25] =\
    +dense0.value[0]*dense1.value[25]\
    +dense0.value[1]*dense1.value[24]\
    -dense0.value[8]*dense1.value[17]\
    +dense0.value[9]*dense1.value[16]\
    +dense0.value[16]*dense1.value[9]\
    -dense0.value[17]*dense1.value[8]\
    +dense0.value[24]*dense1.value[1]\
    +dense0.value[25]*dense1.value[0]\
;\
    dense.value[26] =\
    +dense0.value[0]*dense1.value[26]\
    +dense0.value[2]*dense1.value[24]\
    -dense0.value[8]*dense1.value[18]\
    +dense0.value[10]*dense1.value[16]\
    +dense0.value[16]*dense1.value[10]\
    -dense0.value[18]*dense1.value[8]\
    +dense0.value[24]*dense1.value[2]\
    +dense0.value[26]*dense1.value[0]\
;\
    dense.value[28] =\
    +dense0.value[0]*dense1.value[28]\
    +dense0.value[4]*dense1.value[24]\
    -dense0.value[8]*dense1.value[20]\
    +dense0.value[12]*dense1.value[16]\
    +dense0.value[16]*dense1.value[12]\
    -dense0.value[20]*dense1.value[8]\
    +dense0.value[24]*dense1.value[4]\
    +dense0.value[28]*dense1.value[0]\
;\
}
#define GEN1_DENSE_GRADE4OUTERPRODUCT(dense,dense0,dense1){\
    dense.value[15] =\
    +dense0.value[0]*dense1.value[15]\
    +dense0.value[1]*dense1.value[14]\
    -dense0.value[2]*dense1.value[13]\
    +dense0.value[3]*dense1.value[12]\
    +dense0.value[4]*dense1.value[11]\
    -dense0.value[5]*dense1.value[10]\
    +dense0.value[6]*dense1.value[9]\
    +dense0.value[7]*dense1.value[8]\
    -dense0.value[8]*dense1.value[7]\
    +dense0.value[9]*dense1.value[6]\
    -dense0.value[10]*dense1.value[5]\
    -dense0.value[11]*dense1.value[4]\
    +dense0.value[12]*dense1.value[3]\
    +dense0.value[13]*dense1.value[2]\
    -dense0.value[14]*dense1.value[1]\
    +dense0.value[15]*dense1.value[0]\
;\
    dense.value[23] =\
    +dense0.value[0]*dense1.value[23]\
    +dense0.value[1]*dense1.value[22]\
    -dense0.value[2]*dense1.value[21]\
    +dense0.value[3]*dense1.value[20]\
    +dense0.value[4]*dense1.value[19]\
    -dense0.value[5]*dense1.value[18]\
    +dense0.value[6]*dense1.value[17]\
    +dense0.value[7]*dense1.value[16]\
    -dense0.value[16]*dense1.value[7]\
    +dense0.value[17]*dense1.value[6]\
    -dense0.value[18]*dense1.value[5]\
    -dense0.value[19]*dense1.value[4]\
    +dense0.value[20]*dense1.value[3]\
    +dense0.value[21]*dense1.value[2]\
    -dense0.value[22]*dense1.value[1]\
    +dense0.value[23]*dense1.value[0]\
;\
    dense.value[27] =\
    +dense0.value[0]*dense1.value[27]\
    +dense0.value[1]*dense1.value[26]\
    -dense0.value[2]*dense1.value[25]\
    +dense0.value[3]*dense1.value[24]\
    +dense0.value[8]*dense1.value[19]\
    -dense0.value[9]*dense1.value[18]\
    +dense0.value[10]*dense1.value[17]\
    +dense0.value[11]*dense1.value[16]\
    -dense0.value[16]*dense1.value[11]\
    +dense0.value[17]*dense1.value[10]\
    -dense0.value[18]*dense1.value[9]\
    -dense0.value[19]*dense1.value[8]\
    +dense0.value[24]*dense1.value[3]\
    +dense0.value[25]*dense1.value[2]\
    -dense0.value[26]*dense1.value[1]\
    +dense0.value[27]*dense1.value[0]\
;\
    dense.value[29] =\
    +dense0.value[0]*dense1.value[29]\
    +dense0.value[1]*dense1.value[28]\
    -dense0.value[4]*dense1.value[25]\
    +dense0.value[5]*dense1.value[24]\
    +dense0.value[8]*dense1.value[21]\
    -dense0.value[9]*dense1.value[20]\
    +dense0.value[12]*dense1.value[17]\
    +dense0.value[13]*dense1.value[16]\
    -dense0.value[16]*dense1.value[13]\
    +dense0.value[17]*dense1.value[12]\
    -dense0.value[20]*dense1.value[9]\
    -dense0.value[21]*dense1.value[8]\
    +dense0.value[24]*dense1.value[5]\
    +dense0.value[25]*dense1.value[4]\
    -dense0.value[28]*dense1.value[1]\
    +dense0.value[29]*dense1.value[0]\
;\
    dense.value[30] =\
    +dense0.value[0]*dense1.value[30]\
    +dense0.value[2]*dense1.value[28]\
    -dense0.value[4]*dense1.value[26]\
    +dense0.value[6]*dense1.value[24]\
    +dense0.value[8]*dense1.value[22]\
    -dense0.value[10]*dense1.value[20]\
    +dense0.value[12]*dense1.value[18]\
    +dense0.value[14]*dense1.value[16]\
    -dense0.value[16]*dense1.value[14]\
    +dense0.value[18]*dense1.value[12]\
    -dense0.value[20]*dense1.value[10]\
    -dense0.value[22]*dense1.value[8]\
    +dense0.value[24]*dense1.value[6]\
    +dense0.value[26]*dense1.value[4]\
    -dense0.value[28]*dense1.value[2]\
    +dense0.value[30]*dense1.value[0]\
;\
}
#define GEN1_DENSE_GRADE5OUTERPRODUCT(dense,dense0,dense1){\
    dense.value[31] =\
    +dense0.value[0]*dense1.value[31]\
    +dense0.value[1]*dense1.value[30]\
    -dense0.value[2]*dense1.value[29]\
    +dense0.value[3]*dense1.value[28]\
    +dense0.value[4]*dense1.value[27]\
    -dense0.value[5]*dense1.value[26]\
    +dense0.value[6]*dense1.value[25]\
    +dense0.value[7]*dense1.value[24]\
    -dense0.value[8]*dense1.value[23]\
    +dense0.value[9]*dense1.value[22]\
    -dense0.value[10]*dense1.value[21]\
    -dense0.value[11]*dense1.value[20]\
    +dense0.value[12]*dense1.value[19]\
    +dense0.value[13]*dense1.value[18]\
    -dense0.value[14]*dense1.value[17]\
    +dense0.value[15]*dense1.value[16]\
    +dense0.value[16]*dense1.value[15]\
    -dense0.value[17]*dense1.value[14]\
    +dense0.value[18]*dense1.value[13]\
    +dense0.value[19]*dense1.value[12]\
    -dense0.value[20]*dense1.value[11]\
    -dense0.value[21]*dense1.value[10]\
    +dense0.value[22]*dense1.value[9]\
    -dense0.value[23]*dense1.value[8]\
    +dense0.value[24]*dense1.value[7]\
    +dense0.value[25]*dense1.value[6]\
    -dense0.value[26]*dense1.value[5]\
    +dense0.value[27]*dense1.value[4]\
    +dense0.value[28]*dense1.value[3]\
    -dense0.value[29]*dense1.value[2]\
    +dense0.value[30]*dense1.value[1]\
    +dense0.value[31]*dense1.value[0]\
;\
}

static gen1_DenseMultivector gen1_dense_gradeouterproduct(gen1_DenseMultivector dense0, gen1_DenseMultivector dense1, int *grades, Py_ssize_t size){
    gen1_DenseMultivector dense = {{0}};
    for(Py_ssize_t i = 0; i < size; i++){
        switch(grades[i]){
            case 0:
                GEN1_DENSE_GRADE0OUTERPRODUCT(dense,dense0,dense1);
                break;
            case 1:
                GEN1_DENSE_GRADE1OUTERPRODUCT(dense,dense0,dense1);
                break;
            case 2:
                GEN1_DENSE_GRADE2OUTERPRODUCT(dense,dense0,dense1);
                break;
            case 3:
                GEN1_DENSE_GRADE3OUTERPRODUCT(dense,dense0,dense1);
                break;
            case 4:
                GEN1_DENSE_GRADE4OUTERPRODUCT(dense,dense0,dense1);
                break;
            case 5:
                GEN1_DENSE_GRADE5OUTERPRODUCT(dense,dense0,dense1);
                break;
            default:// do nothing for different values
                break;
        }
    }
    return dense;
}


static gen1_DenseMultivector gen1_dense_regressiveproduct(gen1_DenseMultivector dense0, gen1_DenseMultivector dense1){
    gen1_DenseMultivector dense = {{0}};
    dense.value[0] =
    +dense0.value[0]*dense1.value[31]
    +dense0.value[1]*dense1.value[30]
    -dense0.value[2]*dense1.value[29]
    +dense0.value[3]*dense1.value[28]
    +dense0.value[4]*dense1.value[27]
    -dense0.value[5]*dense1.value[26]
    +dense0.value[6]*dense1.value[25]
    +dense0.value[7]*dense1.value[24]
    -dense0.value[8]*dense1.value[23]
    +dense0.value[9]*dense1.value[22]
    -dense0.value[10]*dense1.value[21]
    -dense0.value[11]*dense1.value[20]
    +dense0.value[12]*dense1.value[19]
    +dense0.value[13]*dense1.value[18]
    -dense0.value[14]*dense1.value[17]
    +dense0.value[15]*dense1.value[16]
    +dense0.value[16]*dense1.value[15]
    -dense0.value[17]*dense1.value[14]
    +dense0.value[18]*dense1.value[13]
    +dense0.value[19]*dense1.value[12]
    -dense0.value[20]*dense1.value[11]
    -dense0.value[21]*dense1.value[10]
    +dense0.value[22]*dense1.value[9]
    -dense0.value[23]*dense1.value[8]
    +dense0.value[24]*dense1.value[7]
    +dense0.value[25]*dense1.value[6]
    -dense0.value[26]*dense1.value[5]
    +dense0.value[27]*dense1.value[4]
    +dense0.value[28]*dense1.value[3]
    -dense0.value[29]*dense1.value[2]
    +dense0.value[30]*dense1.value[1]
    +dense0.value[31]*dense1.value[0]
;
    dense.value[1] =
    +dense0.value[1]*dense1.value[31]
    -dense0.value[3]*dense1.value[29]
    +dense0.value[5]*dense1.value[27]
    +dense0.value[7]*dense1.value[25]
    -dense0.value[9]*dense1.value[23]
    -dense0.value[11]*dense1.value[21]
    +dense0.value[13]*dense1.value[19]
    -dense0.value[15]*dense1.value[17]
    +dense0.value[17]*dense1.value[15]
    +dense0.value[19]*dense1.value[13]
    -dense0.value[21]*dense1.value[11]
    +dense0.value[23]*dense1.value[9]
    +dense0.value[25]*dense1.value[7]
    -dense0.value[27]*dense1.value[5]
    +dense0.value[29]*dense1.value[3]
    +dense0.value[31]*dense1.value[1]
;
    dense.value[2] =
    -dense0.value[2]*dense1.value[31]
    +dense0.value[3]*dense1.value[30]
    -dense0.value[6]*dense1.value[27]
    -dense0.value[7]*dense1.value[26]
    +dense0.value[10]*dense1.value[23]
    +dense0.value[11]*dense1.value[22]
    -dense0.value[14]*dense1.value[19]
    +dense0.value[15]*dense1.value[18]
    -dense0.value[18]*dense1.value[15]
    -dense0.value[19]*dense1.value[14]
    +dense0.value[22]*dense1.value[11]
    -dense0.value[23]*dense1.value[10]
    -dense0.value[26]*dense1.value[7]
    +dense0.value[27]*dense1.value[6]
    -dense0.value[30]*dense1.value[3]
    -dense0.value[31]*dense1.value[2]
;
    dense.value[3] =
    -dense0.value[3]*dense1.value[31]
    -dense0.value[7]*dense1.value[27]
    +dense0.value[11]*dense1.value[23]
    -dense0.value[15]*dense1.value[19]
    -dense0.value[19]*dense1.value[15]
    +dense0.value[23]*dense1.value[11]
    -dense0.value[27]*dense1.value[7]
    -dense0.value[31]*dense1.value[3]
;
    dense.value[4] =
    +dense0.value[4]*dense1.value[31]
    -dense0.value[5]*dense1.value[30]
    +dense0.value[6]*dense1.value[29]
    +dense0.value[7]*dense1.value[28]
    -dense0.value[12]*dense1.value[23]
    -dense0.value[13]*dense1.value[22]
    +dense0.value[14]*dense1.value[21]
    -dense0.value[15]*dense1.value[20]
    +dense0.value[20]*dense1.value[15]
    +dense0.value[21]*dense1.value[14]
    -dense0.value[22]*dense1.value[13]
    +dense0.value[23]*dense1.value[12]
    +dense0.value[28]*dense1.value[7]
    -dense0.value[29]*dense1.value[6]
    +dense0.value[30]*dense1.value[5]
    +dense0.value[31]*dense1.value[4]
;
    dense.value[5] =
    +dense0.value[5]*dense1.value[31]
    +dense0.value[7]*dense1.value[29]
    -dense0.value[13]*dense1.value[23]
    +dense0.value[15]*dense1.value[21]
    +dense0.value[21]*dense1.value[15]
    -dense0.value[23]*dense1.value[13]
    +dense0.value[29]*dense1.value[7]
    +dense0.value[31]*dense1.value[5]
;
    dense.value[6] =
    -dense0.value[6]*dense1.value[31]
    -dense0.value[7]*dense1.value[30]
    +dense0.value[14]*dense1.value[23]
    -dense0.value[15]*dense1.value[22]
    -dense0.value[22]*dense1.value[15]
    +dense0.value[23]*dense1.value[14]
    -dense0.value[30]*dense1.value[7]
    -dense0.value[31]*dense1.value[6]
;
    dense.value[7] =
    -dense0.value[7]*dense1.value[31]
    +dense0.value[15]*dense1.value[23]
    -dense0.value[23]*dense1.value[15]
    -dense0.value[31]*dense1.value[7]
;
    dense.value[8] =
    -dense0.value[8]*dense1.value[31]
    +dense0.value[9]*dense1.value[30]
    -dense0.value[10]*dense1.value[29]
    -dense0.value[11]*dense1.value[28]
    +dense0.value[12]*dense1.value[27]
    +dense0.value[13]*dense1.value[26]
    -dense0.value[14]*dense1.value[25]
    +dense0.value[15]*dense1.value[24]
    -dense0.value[24]*dense1.value[15]
    -dense0.value[25]*dense1.value[14]
    +dense0.value[26]*dense1.value[13]
    -dense0.value[27]*dense1.value[12]
    -dense0.value[28]*dense1.value[11]
    +dense0.value[29]*dense1.value[10]
    -dense0.value[30]*dense1.value[9]
    -dense0.value[31]*dense1.value[8]
;
    dense.value[9] =
    -dense0.value[9]*dense1.value[31]
    -dense0.value[11]*dense1.value[29]
    +dense0.value[13]*dense1.value[27]
    -dense0.value[15]*dense1.value[25]
    -dense0.value[25]*dense1.value[15]
    +dense0.value[27]*dense1.value[13]
    -dense0.value[29]*dense1.value[11]
    -dense0.value[31]*dense1.value[9]
;
    dense.value[10] =
    +dense0.value[10]*dense1.value[31]
    +dense0.value[11]*dense1.value[30]
    -dense0.value[14]*dense1.value[27]
    +dense0.value[15]*dense1.value[26]
    +dense0.value[26]*dense1.value[15]
    -dense0.value[27]*dense1.value[14]
    +dense0.value[30]*dense1.value[11]
    +dense0.value[31]*dense1.value[10]
;
    dense.value[11] =
    +dense0.value[11]*dense1.value[31]
    -dense0.value[15]*dense1.value[27]
    +dense0.value[27]*dense1.value[15]
    +dense0.value[31]*dense1.value[11]
;
    dense.value[12] =
    -dense0.value[12]*dense1.value[31]
    -dense0.value[13]*dense1.value[30]
    +dense0.value[14]*dense1.value[29]
    -dense0.value[15]*dense1.value[28]
    -dense0.value[28]*dense1.value[15]
    +dense0.value[29]*dense1.value[14]
    -dense0.value[30]*dense1.value[13]
    -dense0.value[31]*dense1.value[12]
;
    dense.value[13] =
    -dense0.value[13]*dense1.value[31]
    +dense0.value[15]*dense1.value[29]
    -dense0.value[29]*dense1.value[15]
    -dense0.value[31]*dense1.value[13]
;
    dense.value[14] =
    +dense0.value[14]*dense1.value[31]
    -dense0.value[15]*dense1.value[30]
    +dense0.value[30]*dense1.value[15]
    +dense0.value[31]*dense1.value[14]
;
    dense.value[15] =
    +dense0.value[15]*dense1.value[31]
    +dense0.value[31]*dense1.value[15]
;
    dense.value[16] =
    +dense0.value[16]*dense1.value[31]
    -dense0.value[17]*dense1.value[30]
    +dense0.value[18]*dense1.value[29]
    +dense0.value[19]*dense1.value[28]
    -dense0.value[20]*dense1.value[27]
    -dense0.value[21]*dense1.value[26]
    +dense0.value[22]*dense1.value[25]
    -dense0.value[23]*dense1.value[24]
    +dense0.value[24]*dense1.value[23]
    +dense0.value[25]*dense1.value[22]
    -dense0.value[26]*dense1.value[21]
    +dense0.value[27]*dense1.value[20]
    +dense0.value[28]*dense1.value[19]
    -dense0.value[29]*dense1.value[18]
    +dense0.value[30]*dense1.value[17]
    +dense0.value[31]*dense1.value[16]
;
    dense.value[17] =
    +dense0.value[17]*dense1.value[31]
    +dense0.value[19]*dense1.value[29]
    -dense0.value[21]*dense1.value[27]
    +dense0.value[23]*dense1.value[25]
    +dense0.value[25]*dense1.value[23]
    -dense0.value[27]*dense1.value[21]
    +dense0.value[29]*dense1.value[19]
    +dense0.value[31]*dense1.value[17]
;
    dense.value[18] =
    -dense0.value[18]*dense1.value[31]
    -dense0.value[19]*dense1.value[30]
    +dense0.value[22]*dense1.value[27]
    -dense0.value[23]*dense1.value[26]
    -dense0.value[26]*dense1.value[23]
    +dense0.value[27]*dense1.value[22]
    -dense0.value[30]*dense1.value[19]
    -dense0.value[31]*dense1.value[18]
;
    dense.value[19] =
    -dense0.value[19]*dense1.value[31]
    +dense0.value[23]*dense1.value[27]
    -dense0.value[27]*dense1.value[23]
    -dense0.value[31]*dense1.value[19]
;
    dense.value[20] =
    +dense0.value[20]*dense1.value[31]
    +dense0.value[21]*dense1.value[30]
    -dense0.value[22]*dense1.value[29]
    +dense0.value[23]*dense1.value[28]
    +dense0.value[28]*dense1.value[23]
    -dense0.value[29]*dense1.value[22]
    +dense0.value[30]*dense1.value[21]
    +dense0.value[31]*dense1.value[20]
;
    dense.value[21] =
    +dense0.value[21]*dense1.value[31]
    -dense0.value[23]*dense1.value[29]
    +dense0.value[29]*dense1.value[23]
    +dense0.value[31]*dense1.value[21]
;
    dense.value[22] =
    -dense0.value[22]*dense1.value[31]
    +dense0.value[23]*dense1.value[30]
    -dense0.value[30]*dense1.value[23]
    -dense0.value[31]*dense1.value[22]
;
    dense.value[23] =
    -dense0.value[23]*dense1.value[31]
    -dense0.value[31]*dense1.value[23]
;
    dense.value[24] =
    -dense0.value[24]*dense1.value[31]
    -dense0.value[25]*dense1.value[30]
    +dense0.value[26]*dense1.value[29]
    -dense0.value[27]*dense1.value[28]
    -dense0.value[28]*dense1.value[27]
    +dense0.value[29]*dense1.value[26]
    -dense0.value[30]*dense1.value[25]
    -dense0.value[31]*dense1.value[24]
;
    dense.value[25] =
    -dense0.value[25]*dense1.value[31]
    +dense0.value[27]*dense1.value[29]
    -dense0.value[29]*dense1.value[27]
    -dense0.value[31]*dense1.value[25]
;
    dense.value[26] =
    +dense0.value[26]*dense1.value[31]
    -dense0.value[27]*dense1.value[30]
    +dense0.value[30]*dense1.value[27]
    +dense0.value[31]*dense1.value[26]
;
    dense.value[27] =
    +dense0.value[27]*dense1.value[31]
    +dense0.value[31]*dense1.value[27]
;
    dense.value[28] =
    -dense0.value[28]*dense1.value[31]
    +dense0.value[29]*dense1.value[30]
    -dense0.value[30]*dense1.value[29]
    -dense0.value[31]*dense1.value[28]
;
    dense.value[29] =
    -dense0.value[29]*dense1.value[31]
    -dense0.value[31]*dense1.value[29]
;
    dense.value[30] =
    +dense0.value[30]*dense1.value[31]
    +dense0.value[31]*dense1.value[30]
;
    dense.value[31] =
    +dense0.value[31]*dense1.value[31]
;
    return dense;
}

// grade projection of the product of two multivectors <ab>_r
#define GEN1_DENSE_GRADE0REGRESSIVEPRODUCT(dense,dense0,dense1){\
    dense.value[0] =\
    +dense0.value[0]*dense1.value[31]\
    +dense0.value[1]*dense1.value[30]\
    -dense0.value[2]*dense1.value[29]\
    +dense0.value[3]*dense1.value[28]\
    +dense0.value[4]*dense1.value[27]\
    -dense0.value[5]*dense1.value[26]\
    +dense0.value[6]*dense1.value[25]\
    +dense0.value[7]*dense1.value[24]\
    -dense0.value[8]*dense1.value[23]\
    +dense0.value[9]*dense1.value[22]\
    -dense0.value[10]*dense1.value[21]\
    -dense0.value[11]*dense1.value[20]\
    +dense0.value[12]*dense1.value[19]\
    +dense0.value[13]*dense1.value[18]\
    -dense0.value[14]*dense1.value[17]\
    +dense0.value[15]*dense1.value[16]\
    +dense0.value[16]*dense1.value[15]\
    -dense0.value[17]*dense1.value[14]\
    +dense0.value[18]*dense1.value[13]\
    +dense0.value[19]*dense1.value[12]\
    -dense0.value[20]*dense1.value[11]\
    -dense0.value[21]*dense1.value[10]\
    +dense0.value[22]*dense1.value[9]\
    -dense0.value[23]*dense1.value[8]\
    +dense0.value[24]*dense1.value[7]\
    +dense0.value[25]*dense1.value[6]\
    -dense0.value[26]*dense1.value[5]\
    +dense0.value[27]*dense1.value[4]\
    +dense0.value[28]*dense1.value[3]\
    -dense0.value[29]*dense1.value[2]\
    +dense0.value[30]*dense1.value[1]\
    +dense0.value[31]*dense1.value[0]\
;\
}
#define GEN1_DENSE_GRADE1REGRESSIVEPRODUCT(dense,dense0,dense1){\
    dense.value[1] =\
    +dense0.value[1]*dense1.value[31]\
    -dense0.value[3]*dense1.value[29]\
    +dense0.value[5]*dense1.value[27]\
    +dense0.value[7]*dense1.value[25]\
    -dense0.value[9]*dense1.value[23]\
    -dense0.value[11]*dense1.value[21]\
    +dense0.value[13]*dense1.value[19]\
    -dense0.value[15]*dense1.value[17]\
    +dense0.value[17]*dense1.value[15]\
    +dense0.value[19]*dense1.value[13]\
    -dense0.value[21]*dense1.value[11]\
    +dense0.value[23]*dense1.value[9]\
    +dense0.value[25]*dense1.value[7]\
    -dense0.value[27]*dense1.value[5]\
    +dense0.value[29]*dense1.value[3]\
    +dense0.value[31]*dense1.value[1]\
;\
    dense.value[2] =\
    -dense0.value[2]*dense1.value[31]\
    +dense0.value[3]*dense1.value[30]\
    -dense0.value[6]*dense1.value[27]\
    -dense0.value[7]*dense1.value[26]\
    +dense0.value[10]*dense1.value[23]\
    +dense0.value[11]*dense1.value[22]\
    -dense0.value[14]*dense1.value[19]\
    +dense0.value[15]*dense1.value[18]\
    -dense0.value[18]*dense1.value[15]\
    -dense0.value[19]*dense1.value[14]\
    +dense0.value[22]*dense1.value[11]\
    -dense0.value[23]*dense1.value[10]\
    -dense0.value[26]*dense1.value[7]\
    +dense0.value[27]*dense1.value[6]\
    -dense0.value[30]*dense1.value[3]\
    -dense0.value[31]*dense1.value[2]\
;\
    dense.value[4] =\
    +dense0.value[4]*dense1.value[31]\
    -dense0.value[5]*dense1.value[30]\
    +dense0.value[6]*dense1.value[29]\
    +dense0.value[7]*dense1.value[28]\
    -dense0.value[12]*dense1.value[23]\
    -dense0.value[13]*dense1.value[22]\
    +dense0.value[14]*dense1.value[21]\
    -dense0.value[15]*dense1.value[20]\
    +dense0.value[20]*dense1.value[15]\
    +dense0.value[21]*dense1.value[14]\
    -dense0.value[22]*dense1.value[13]\
    +dense0.value[23]*dense1.value[12]\
    +dense0.value[28]*dense1.value[7]\
    -dense0.value[29]*dense1.value[6]\
    +dense0.value[30]*dense1.value[5]\
    +dense0.value[31]*dense1.value[4]\
;\
    dense.value[8] =\
    -dense0.value[8]*dense1.value[31]\
    +dense0.value[9]*dense1.value[30]\
    -dense0.value[10]*dense1.value[29]\
    -dense0.value[11]*dense1.value[28]\
    +dense0.value[12]*dense1.value[27]\
    +dense0.value[13]*dense1.value[26]\
    -dense0.value[14]*dense1.value[25]\
    +dense0.value[15]*dense1.value[24]\
    -dense0.value[24]*dense1.value[15]\
    -dense0.value[25]*dense1.value[14]\
    +dense0.value[26]*dense1.value[13]\
    -dense0.value[27]*dense1.value[12]\
    -dense0.value[28]*dense1.value[11]\
    +dense0.value[29]*dense1.value[10]\
    -dense0.value[30]*dense1.value[9]\
    -dense0.value[31]*dense1.value[8]\
;\
    dense.value[16] =\
    +dense0.value[16]*dense1.value[31]\
    -dense0.value[17]*dense1.value[30]\
    +dense0.value[18]*dense1.value[29]\
    +dense0.value[19]*dense1.value[28]\
    -dense0.value[20]*dense1.value[27]\
    -dense0.value[21]*dense1.value[26]\
    +dense0.value[22]*dense1.value[25]\
    -dense0.value[23]*dense1.value[24]\
    +dense0.value[24]*dense1.value[23]\
    +dense0.value[25]*dense1.value[22]\
    -dense0.value[26]*dense1.value[21]\
    +dense0.value[27]*dense1.value[20]\
    +dense0.value[28]*dense1.value[19]\
    -dense0.value[29]*dense1.value[18]\
    +dense0.value[30]*dense1.value[17]\
    +dense0.value[31]*dense1.value[16]\
;\
}
#define GEN1_DENSE_GRADE2REGRESSIVEPRODUCT(dense,dense0,dense1){\
    dense.value[3] =\
    -dense0.value[3]*dense1.value[31]\
    -dense0.value[7]*dense1.value[27]\
    +dense0.value[11]*dense1.value[23]\
    -dense0.value[15]*dense1.value[19]\
    -dense0.value[19]*dense1.value[15]\
    +dense0.value[23]*dense1.value[11]\
    -dense0.value[27]*dense1.value[7]\
    -dense0.value[31]*dense1.value[3]\
;\
    dense.value[5] =\
    +dense0.value[5]*dense1.value[31]\
    +dense0.value[7]*dense1.value[29]\
    -dense0.value[13]*dense1.value[23]\
    +dense0.value[15]*dense1.value[21]\
    +dense0.value[21]*dense1.value[15]\
    -dense0.value[23]*dense1.value[13]\
    +dense0.value[29]*dense1.value[7]\
    +dense0.value[31]*dense1.value[5]\
;\
    dense.value[6] =\
    -dense0.value[6]*dense1.value[31]\
    -dense0.value[7]*dense1.value[30]\
    +dense0.value[14]*dense1.value[23]\
    -dense0.value[15]*dense1.value[22]\
    -dense0.value[22]*dense1.value[15]\
    +dense0.value[23]*dense1.value[14]\
    -dense0.value[30]*dense1.value[7]\
    -dense0.value[31]*dense1.value[6]\
;\
    dense.value[9] =\
    -dense0.value[9]*dense1.value[31]\
    -dense0.value[11]*dense1.value[29]\
    +dense0.value[13]*dense1.value[27]\
    -dense0.value[15]*dense1.value[25]\
    -dense0.value[25]*dense1.value[15]\
    +dense0.value[27]*dense1.value[13]\
    -dense0.value[29]*dense1.value[11]\
    -dense0.value[31]*dense1.value[9]\
;\
    dense.value[10] =\
    +dense0.value[10]*dense1.value[31]\
    +dense0.value[11]*dense1.value[30]\
    -dense0.value[14]*dense1.value[27]\
    +dense0.value[15]*dense1.value[26]\
    +dense0.value[26]*dense1.value[15]\
    -dense0.value[27]*dense1.value[14]\
    +dense0.value[30]*dense1.value[11]\
    +dense0.value[31]*dense1.value[10]\
;\
    dense.value[12] =\
    -dense0.value[12]*dense1.value[31]\
    -dense0.value[13]*dense1.value[30]\
    +dense0.value[14]*dense1.value[29]\
    -dense0.value[15]*dense1.value[28]\
    -dense0.value[28]*dense1.value[15]\
    +dense0.value[29]*dense1.value[14]\
    -dense0.value[30]*dense1.value[13]\
    -dense0.value[31]*dense1.value[12]\
;\
    dense.value[17] =\
    +dense0.value[17]*dense1.value[31]\
    +dense0.value[19]*dense1.value[29]\
    -dense0.value[21]*dense1.value[27]\
    +dense0.value[23]*dense1.value[25]\
    +dense0.value[25]*dense1.value[23]\
    -dense0.value[27]*dense1.value[21]\
    +dense0.value[29]*dense1.value[19]\
    +dense0.value[31]*dense1.value[17]\
;\
    dense.value[18] =\
    -dense0.value[18]*dense1.value[31]\
    -dense0.value[19]*dense1.value[30]\
    +dense0.value[22]*dense1.value[27]\
    -dense0.value[23]*dense1.value[26]\
    -dense0.value[26]*dense1.value[23]\
    +dense0.value[27]*dense1.value[22]\
    -dense0.value[30]*dense1.value[19]\
    -dense0.value[31]*dense1.value[18]\
;\
    dense.value[20] =\
    +dense0.value[20]*dense1.value[31]\
    +dense0.value[21]*dense1.value[30]\
    -dense0.value[22]*dense1.value[29]\
    +dense0.value[23]*dense1.value[28]\
    +dense0.value[28]*dense1.value[23]\
    -dense0.value[29]*dense1.value[22]\
    +dense0.value[30]*dense1.value[21]\
    +dense0.value[31]*dense1.value[20]\
;\
    dense.value[24] =\
    -dense0.value[24]*dense1.value[31]\
    -dense0.value[25]*dense1.value[30]\
    +dense0.value[26]*dense1.value[29]\
    -dense0.value[27]*dense1.value[28]\
    -dense0.value[28]*dense1.value[27]\
    +dense0.value[29]*dense1.value[26]\
    -dense0.value[30]*dense1.value[25]\
    -dense0.value[31]*dense1.value[24]\
;\
}
#define GEN1_DENSE_GRADE3REGRESSIVEPRODUCT(dense,dense0,dense1){\
    dense.value[7] =\
    -dense0.value[7]*dense1.value[31]\
    +dense0.value[15]*dense1.value[23]\
    -dense0.value[23]*dense1.value[15]\
    -dense0.value[31]*dense1.value[7]\
;\
    dense.value[11] =\
    +dense0.value[11]*dense1.value[31]\
    -dense0.value[15]*dense1.value[27]\
    +dense0.value[27]*dense1.value[15]\
    +dense0.value[31]*dense1.value[11]\
;\
    dense.value[13] =\
    -dense0.value[13]*dense1.value[31]\
    +dense0.value[15]*dense1.value[29]\
    -dense0.value[29]*dense1.value[15]\
    -dense0.value[31]*dense1.value[13]\
;\
    dense.value[14] =\
    +dense0.value[14]*dense1.value[31]\
    -dense0.value[15]*dense1.value[30]\
    +dense0.value[30]*dense1.value[15]\
    +dense0.value[31]*dense1.value[14]\
;\
    dense.value[19] =\
    -dense0.value[19]*dense1.value[31]\
    +dense0.value[23]*dense1.value[27]\
    -dense0.value[27]*dense1.value[23]\
    -dense0.value[31]*dense1.value[19]\
;\
    dense.value[21] =\
    +dense0.value[21]*dense1.value[31]\
    -dense0.value[23]*dense1.value[29]\
    +dense0.value[29]*dense1.value[23]\
    +dense0.value[31]*dense1.value[21]\
;\
    dense.value[22] =\
    -dense0.value[22]*dense1.value[31]\
    +dense0.value[23]*dense1.value[30]\
    -dense0.value[30]*dense1.value[23]\
    -dense0.value[31]*dense1.value[22]\
;\
    dense.value[25] =\
    -dense0.value[25]*dense1.value[31]\
    +dense0.value[27]*dense1.value[29]\
    -dense0.value[29]*dense1.value[27]\
    -dense0.value[31]*dense1.value[25]\
;\
    dense.value[26] =\
    +dense0.value[26]*dense1.value[31]\
    -dense0.value[27]*dense1.value[30]\
    +dense0.value[30]*dense1.value[27]\
    +dense0.value[31]*dense1.value[26]\
;\
    dense.value[28] =\
    -dense0.value[28]*dense1.value[31]\
    +dense0.value[29]*dense1.value[30]\
    -dense0.value[30]*dense1.value[29]\
    -dense0.value[31]*dense1.value[28]\
;\
}
#define GEN1_DENSE_GRADE4REGRESSIVEPRODUCT(dense,dense0,dense1){\
    dense.value[15] =\
    +dense0.value[15]*dense1.value[31]\
    +dense0.value[31]*dense1.value[15]\
;\
    dense.value[23] =\
    -dense0.value[23]*dense1.value[31]\
    -dense0.value[31]*dense1.value[23]\
;\
    dense.value[27] =\
    +dense0.value[27]*dense1.value[31]\
    +dense0.value[31]*dense1.value[27]\
;\
    dense.value[29] =\
    -dense0.value[29]*dense1.value[31]\
    -dense0.value[31]*dense1.value[29]\
;\
    dense.value[30] =\
    +dense0.value[30]*dense1.value[31]\
    +dense0.value[31]*dense1.value[30]\
;\
}
#define GEN1_DENSE_GRADE5REGRESSIVEPRODUCT(dense,dense0,dense1){\
    dense.value[31] =\
    +dense0.value[31]*dense1.value[31]\
;\
}

static gen1_DenseMultivector gen1_dense_graderegressiveproduct(gen1_DenseMultivector dense0, gen1_DenseMultivector dense1, int *grades, Py_ssize_t size){
    gen1_DenseMultivector dense = {{0}};
    for(Py_ssize_t i = 0; i < size; i++){
        switch(grades[i]){
            case 0:
                GEN1_DENSE_GRADE0REGRESSIVEPRODUCT(dense,dense0,dense1);
                break;
            case 1:
                GEN1_DENSE_GRADE1REGRESSIVEPRODUCT(dense,dense0,dense1);
                break;
            case 2:
                GEN1_DENSE_GRADE2REGRESSIVEPRODUCT(dense,dense0,dense1);
                break;
            case 3:
                GEN1_DENSE_GRADE3REGRESSIVEPRODUCT(dense,dense0,dense1);
                break;
            case 4:
                GEN1_DENSE_GRADE4REGRESSIVEPRODUCT(dense,dense0,dense1);
                break;
            case 5:
                GEN1_DENSE_GRADE5REGRESSIVEPRODUCT(dense,dense0,dense1);
                break;
            default:// do nothing for different values
                break;
        }
    }
    return dense;
}


static int atomic_dense1_add(void *out, void *data0, PyAlgebraObject *GA, Py_ssize_t size){
    gen1_DenseMultivector *dense_array = (gen1_DenseMultivector *)data0;
    gen1_DenseMultivector dense = {{0}};
    gen1_DenseMultivector *dense_out = (gen1_DenseMultivector *)out;

    for(Py_ssize_t i = 0; i < size; i++){
        dense.value[0] += dense_array[i].value[0];
        dense.value[1] += dense_array[i].value[1];
        dense.value[2] += dense_array[i].value[2];
        dense.value[3] += dense_array[i].value[3];
        dense.value[4] += dense_array[i].value[4];
        dense.value[5] += dense_array[i].value[5];
        dense.value[6] += dense_array[i].value[6];
        dense.value[7] += dense_array[i].value[7];
        dense.value[8] += dense_array[i].value[8];
        dense.value[9] += dense_array[i].value[9];
        dense.value[10] += dense_array[i].value[10];
        dense.value[11] += dense_array[i].value[11];
        dense.value[12] += dense_array[i].value[12];
        dense.value[13] += dense_array[i].value[13];
        dense.value[14] += dense_array[i].value[14];
        dense.value[15] += dense_array[i].value[15];
        dense.value[16] += dense_array[i].value[16];
        dense.value[17] += dense_array[i].value[17];
        dense.value[18] += dense_array[i].value[18];
        dense.value[19] += dense_array[i].value[19];
        dense.value[20] += dense_array[i].value[20];
        dense.value[21] += dense_array[i].value[21];
        dense.value[22] += dense_array[i].value[22];
        dense.value[23] += dense_array[i].value[23];
        dense.value[24] += dense_array[i].value[24];
        dense.value[25] += dense_array[i].value[25];
        dense.value[26] += dense_array[i].value[26];
        dense.value[27] += dense_array[i].value[27];
        dense.value[28] += dense_array[i].value[28];
        dense.value[29] += dense_array[i].value[29];
        dense.value[30] += dense_array[i].value[30];
        dense.value[31] += dense_array[i].value[31];
    }

    *dense_out = dense;
    return 1;
}


static int binary_dense1_add(void *out, void *data0, void *data1, PyAlgebraObject *GA, int sign){
    gen1_DenseMultivector *dense0 = (gen1_DenseMultivector *)data0;
    gen1_DenseMultivector *dense1 = (gen1_DenseMultivector *)data1;
    gen1_DenseMultivector *dense_out = (gen1_DenseMultivector *)out;

    gen1_DenseMultivector dense = {{0}};
    if(sign == -1){
        dense.value[0] = dense0->value[0] - dense1->value[0];
        dense.value[1] = dense0->value[1] - dense1->value[1];
        dense.value[2] = dense0->value[2] - dense1->value[2];
        dense.value[3] = dense0->value[3] - dense1->value[3];
        dense.value[4] = dense0->value[4] - dense1->value[4];
        dense.value[5] = dense0->value[5] - dense1->value[5];
        dense.value[6] = dense0->value[6] - dense1->value[6];
        dense.value[7] = dense0->value[7] - dense1->value[7];
        dense.value[8] = dense0->value[8] - dense1->value[8];
        dense.value[9] = dense0->value[9] - dense1->value[9];
        dense.value[10] = dense0->value[10] - dense1->value[10];
        dense.value[11] = dense0->value[11] - dense1->value[11];
        dense.value[12] = dense0->value[12] - dense1->value[12];
        dense.value[13] = dense0->value[13] - dense1->value[13];
        dense.value[14] = dense0->value[14] - dense1->value[14];
        dense.value[15] = dense0->value[15] - dense1->value[15];
        dense.value[16] = dense0->value[16] - dense1->value[16];
        dense.value[17] = dense0->value[17] - dense1->value[17];
        dense.value[18] = dense0->value[18] - dense1->value[18];
        dense.value[19] = dense0->value[19] - dense1->value[19];
        dense.value[20] = dense0->value[20] - dense1->value[20];
        dense.value[21] = dense0->value[21] - dense1->value[21];
        dense.value[22] = dense0->value[22] - dense1->value[22];
        dense.value[23] = dense0->value[23] - dense1->value[23];
        dense.value[24] = dense0->value[24] - dense1->value[24];
        dense.value[25] = dense0->value[25] - dense1->value[25];
        dense.value[26] = dense0->value[26] - dense1->value[26];
        dense.value[27] = dense0->value[27] - dense1->value[27];
        dense.value[28] = dense0->value[28] - dense1->value[28];
        dense.value[29] = dense0->value[29] - dense1->value[29];
        dense.value[30] = dense0->value[30] - dense1->value[30];
        dense.value[31] = dense0->value[31] - dense1->value[31];
    }else if(sign == 1){
        dense.value[0] = dense0->value[0] + dense1->value[0];
        dense.value[1] = dense0->value[1] + dense1->value[1];
        dense.value[2] = dense0->value[2] + dense1->value[2];
        dense.value[3] = dense0->value[3] + dense1->value[3];
        dense.value[4] = dense0->value[4] + dense1->value[4];
        dense.value[5] = dense0->value[5] + dense1->value[5];
        dense.value[6] = dense0->value[6] + dense1->value[6];
        dense.value[7] = dense0->value[7] + dense1->value[7];
        dense.value[8] = dense0->value[8] + dense1->value[8];
        dense.value[9] = dense0->value[9] + dense1->value[9];
        dense.value[10] = dense0->value[10] + dense1->value[10];
        dense.value[11] = dense0->value[11] + dense1->value[11];
        dense.value[12] = dense0->value[12] + dense1->value[12];
        dense.value[13] = dense0->value[13] + dense1->value[13];
        dense.value[14] = dense0->value[14] + dense1->value[14];
        dense.value[15] = dense0->value[15] + dense1->value[15];
        dense.value[16] = dense0->value[16] + dense1->value[16];
        dense.value[17] = dense0->value[17] + dense1->value[17];
        dense.value[18] = dense0->value[18] + dense1->value[18];
        dense.value[19] = dense0->value[19] + dense1->value[19];
        dense.value[20] = dense0->value[20] + dense1->value[20];
        dense.value[21] = dense0->value[21] + dense1->value[21];
        dense.value[22] = dense0->value[22] + dense1->value[22];
        dense.value[23] = dense0->value[23] + dense1->value[23];
        dense.value[24] = dense0->value[24] + dense1->value[24];
        dense.value[25] = dense0->value[25] + dense1->value[25];
        dense.value[26] = dense0->value[26] + dense1->value[26];
        dense.value[27] = dense0->value[27] + dense1->value[27];
        dense.value[28] = dense0->value[28] + dense1->value[28];
        dense.value[29] = dense0->value[29] + dense1->value[29];
        dense.value[30] = dense0->value[30] + dense1->value[30];
        dense.value[31] = dense0->value[31] + dense1->value[31];
    } else{
        dense.value[0] = dense0->value[0] + sign*dense1->value[0];
        dense.value[1] = dense0->value[1] + sign*dense1->value[1];
        dense.value[2] = dense0->value[2] + sign*dense1->value[2];
        dense.value[3] = dense0->value[3] + sign*dense1->value[3];
        dense.value[4] = dense0->value[4] + sign*dense1->value[4];
        dense.value[5] = dense0->value[5] + sign*dense1->value[5];
        dense.value[6] = dense0->value[6] + sign*dense1->value[6];
        dense.value[7] = dense0->value[7] + sign*dense1->value[7];
        dense.value[8] = dense0->value[8] + sign*dense1->value[8];
        dense.value[9] = dense0->value[9] + sign*dense1->value[9];
        dense.value[10] = dense0->value[10] + sign*dense1->value[10];
        dense.value[11] = dense0->value[11] + sign*dense1->value[11];
        dense.value[12] = dense0->value[12] + sign*dense1->value[12];
        dense.value[13] = dense0->value[13] + sign*dense1->value[13];
        dense.value[14] = dense0->value[14] + sign*dense1->value[14];
        dense.value[15] = dense0->value[15] + sign*dense1->value[15];
        dense.value[16] = dense0->value[16] + sign*dense1->value[16];
        dense.value[17] = dense0->value[17] + sign*dense1->value[17];
        dense.value[18] = dense0->value[18] + sign*dense1->value[18];
        dense.value[19] = dense0->value[19] + sign*dense1->value[19];
        dense.value[20] = dense0->value[20] + sign*dense1->value[20];
        dense.value[21] = dense0->value[21] + sign*dense1->value[21];
        dense.value[22] = dense0->value[22] + sign*dense1->value[22];
        dense.value[23] = dense0->value[23] + sign*dense1->value[23];
        dense.value[24] = dense0->value[24] + sign*dense1->value[24];
        dense.value[25] = dense0->value[25] + sign*dense1->value[25];
        dense.value[26] = dense0->value[26] + sign*dense1->value[26];
        dense.value[27] = dense0->value[27] + sign*dense1->value[27];
        dense.value[28] = dense0->value[28] + sign*dense1->value[28];
        dense.value[29] = dense0->value[29] + sign*dense1->value[29];
        dense.value[30] = dense0->value[30] + sign*dense1->value[30];
        dense.value[31] = dense0->value[31] + sign*dense1->value[31];
    }

    *dense_out = dense;
    return 1;
}


static int binary_dense1_scalaradd(void *out, void *data0, PyAlgebraObject *GA, ga_float value, int sign){
    gen1_DenseMultivector *dense0 = (gen1_DenseMultivector*)data0;
    gen1_DenseMultivector *dense_out = (gen1_DenseMultivector*)out;

    gen1_DenseMultivector dense = {{0}};
    if(sign == -1){
        dense.value[0] = -dense0->value[0];
        dense.value[1] = -dense0->value[1];
        dense.value[2] = -dense0->value[2];
        dense.value[3] = -dense0->value[3];
        dense.value[4] = -dense0->value[4];
        dense.value[5] = -dense0->value[5];
        dense.value[6] = -dense0->value[6];
        dense.value[7] = -dense0->value[7];
        dense.value[8] = -dense0->value[8];
        dense.value[9] = -dense0->value[9];
        dense.value[10] = -dense0->value[10];
        dense.value[11] = -dense0->value[11];
        dense.value[12] = -dense0->value[12];
        dense.value[13] = -dense0->value[13];
        dense.value[14] = -dense0->value[14];
        dense.value[15] = -dense0->value[15];
        dense.value[16] = -dense0->value[16];
        dense.value[17] = -dense0->value[17];
        dense.value[18] = -dense0->value[18];
        dense.value[19] = -dense0->value[19];
        dense.value[20] = -dense0->value[20];
        dense.value[21] = -dense0->value[21];
        dense.value[22] = -dense0->value[22];
        dense.value[23] = -dense0->value[23];
        dense.value[24] = -dense0->value[24];
        dense.value[25] = -dense0->value[25];
        dense.value[26] = -dense0->value[26];
        dense.value[27] = -dense0->value[27];
        dense.value[28] = -dense0->value[28];
        dense.value[29] = -dense0->value[29];
        dense.value[30] = -dense0->value[30];
        dense.value[31] = -dense0->value[31];
    }else if(sign == 1){
        dense.value[0] = dense0->value[0];
        dense.value[1] = dense0->value[1];
        dense.value[2] = dense0->value[2];
        dense.value[3] = dense0->value[3];
        dense.value[4] = dense0->value[4];
        dense.value[5] = dense0->value[5];
        dense.value[6] = dense0->value[6];
        dense.value[7] = dense0->value[7];
        dense.value[8] = dense0->value[8];
        dense.value[9] = dense0->value[9];
        dense.value[10] = dense0->value[10];
        dense.value[11] = dense0->value[11];
        dense.value[12] = dense0->value[12];
        dense.value[13] = dense0->value[13];
        dense.value[14] = dense0->value[14];
        dense.value[15] = dense0->value[15];
        dense.value[16] = dense0->value[16];
        dense.value[17] = dense0->value[17];
        dense.value[18] = dense0->value[18];
        dense.value[19] = dense0->value[19];
        dense.value[20] = dense0->value[20];
        dense.value[21] = dense0->value[21];
        dense.value[22] = dense0->value[22];
        dense.value[23] = dense0->value[23];
        dense.value[24] = dense0->value[24];
        dense.value[25] = dense0->value[25];
        dense.value[26] = dense0->value[26];
        dense.value[27] = dense0->value[27];
        dense.value[28] = dense0->value[28];
        dense.value[29] = dense0->value[29];
        dense.value[30] = dense0->value[30];
        dense.value[31] = dense0->value[31];
    } else{
        dense.value[0] = sign*dense0->value[0];
        dense.value[1] = sign*dense0->value[1];
        dense.value[2] = sign*dense0->value[2];
        dense.value[3] = sign*dense0->value[3];
        dense.value[4] = sign*dense0->value[4];
        dense.value[5] = sign*dense0->value[5];
        dense.value[6] = sign*dense0->value[6];
        dense.value[7] = sign*dense0->value[7];
        dense.value[8] = sign*dense0->value[8];
        dense.value[9] = sign*dense0->value[9];
        dense.value[10] = sign*dense0->value[10];
        dense.value[11] = sign*dense0->value[11];
        dense.value[12] = sign*dense0->value[12];
        dense.value[13] = sign*dense0->value[13];
        dense.value[14] = sign*dense0->value[14];
        dense.value[15] = sign*dense0->value[15];
        dense.value[16] = sign*dense0->value[16];
        dense.value[17] = sign*dense0->value[17];
        dense.value[18] = sign*dense0->value[18];
        dense.value[19] = sign*dense0->value[19];
        dense.value[20] = sign*dense0->value[20];
        dense.value[21] = sign*dense0->value[21];
        dense.value[22] = sign*dense0->value[22];
        dense.value[23] = sign*dense0->value[23];
        dense.value[24] = sign*dense0->value[24];
        dense.value[25] = sign*dense0->value[25];
        dense.value[26] = sign*dense0->value[26];
        dense.value[27] = sign*dense0->value[27];
        dense.value[28] = sign*dense0->value[28];
        dense.value[29] = sign*dense0->value[29];
        dense.value[30] = sign*dense0->value[30];
        dense.value[31] = sign*dense0->value[31];
    }
    dense.value[0] += value;
    
    *dense_out = dense;
    return 1;
}

static int binary_dense1_scalarproduct(void *out, void *data0, PyAlgebraObject *GA, ga_float value){
    gen1_DenseMultivector* dense0 = (gen1_DenseMultivector*)data0;
    gen1_DenseMultivector* dense_out = (gen1_DenseMultivector*)out;
    gen1_DenseMultivector dense = {{0}};

    dense.value[0] = value*dense0->value[0];
    dense.value[1] = value*dense0->value[1];
    dense.value[2] = value*dense0->value[2];
    dense.value[3] = value*dense0->value[3];
    dense.value[4] = value*dense0->value[4];
    dense.value[5] = value*dense0->value[5];
    dense.value[6] = value*dense0->value[6];
    dense.value[7] = value*dense0->value[7];
    dense.value[8] = value*dense0->value[8];
    dense.value[9] = value*dense0->value[9];
    dense.value[10] = value*dense0->value[10];
    dense.value[11] = value*dense0->value[11];
    dense.value[12] = value*dense0->value[12];
    dense.value[13] = value*dense0->value[13];
    dense.value[14] = value*dense0->value[14];
    dense.value[15] = value*dense0->value[15];
    dense.value[16] = value*dense0->value[16];
    dense.value[17] = value*dense0->value[17];
    dense.value[18] = value*dense0->value[18];
    dense.value[19] = value*dense0->value[19];
    dense.value[20] = value*dense0->value[20];
    dense.value[21] = value*dense0->value[21];
    dense.value[22] = value*dense0->value[22];
    dense.value[23] = value*dense0->value[23];
    dense.value[24] = value*dense0->value[24];
    dense.value[25] = value*dense0->value[25];
    dense.value[26] = value*dense0->value[26];
    dense.value[27] = value*dense0->value[27];
    dense.value[28] = value*dense0->value[28];
    dense.value[29] = value*dense0->value[29];
    dense.value[30] = value*dense0->value[30];
    dense.value[31] = value*dense0->value[31];

    *dense_out = dense;
    return 1;
}

static int unary_dense1_reverse(void *out, void *data0, PyAlgebraObject *GA){
    gen1_DenseMultivector* dense0 = (gen1_DenseMultivector*)data0;
    gen1_DenseMultivector* dense_out = (gen1_DenseMultivector*)out;
    gen1_DenseMultivector dense = {{0}};

    dense.value[0] = dense0->value[0];
    dense.value[1] = dense0->value[1];
    dense.value[2] = dense0->value[2];
    dense.value[3] = -dense0->value[3];
    dense.value[4] = dense0->value[4];
    dense.value[5] = -dense0->value[5];
    dense.value[6] = -dense0->value[6];
    dense.value[7] = -dense0->value[7];
    dense.value[8] = dense0->value[8];
    dense.value[9] = -dense0->value[9];
    dense.value[10] = -dense0->value[10];
    dense.value[11] = -dense0->value[11];
    dense.value[12] = -dense0->value[12];
    dense.value[13] = -dense0->value[13];
    dense.value[14] = -dense0->value[14];
    dense.value[15] = dense0->value[15];
    dense.value[16] = dense0->value[16];
    dense.value[17] = -dense0->value[17];
    dense.value[18] = -dense0->value[18];
    dense.value[19] = -dense0->value[19];
    dense.value[20] = -dense0->value[20];
    dense.value[21] = -dense0->value[21];
    dense.value[22] = -dense0->value[22];
    dense.value[23] = dense0->value[23];
    dense.value[24] = -dense0->value[24];
    dense.value[25] = -dense0->value[25];
    dense.value[26] = -dense0->value[26];
    dense.value[27] = dense0->value[27];
    dense.value[28] = -dense0->value[28];
    dense.value[29] = dense0->value[29];
    dense.value[30] = dense0->value[30];
    dense.value[31] = dense0->value[31];

    *dense_out = dense;
    return 1;
}

static int unary_dense1_dual(void *out, void *data0, PyAlgebraObject *GA){
    gen1_DenseMultivector* dense0 = (gen1_DenseMultivector*)data0;
    gen1_DenseMultivector* dense_out = (gen1_DenseMultivector*)out;
    gen1_DenseMultivector dense = {{0}};

    dense.value[31] = dense0->value[0];
    dense.value[30] = dense0->value[1];
    dense.value[29] = -dense0->value[2];
    dense.value[28] = -dense0->value[3];
    dense.value[27] = dense0->value[4];
    dense.value[26] = dense0->value[5];
    dense.value[25] = -dense0->value[6];
    dense.value[24] = -dense0->value[7];
    dense.value[23] = -dense0->value[8];
    dense.value[22] = -dense0->value[9];
    dense.value[21] = dense0->value[10];
    dense.value[20] = dense0->value[11];
    dense.value[19] = -dense0->value[12];
    dense.value[18] = -dense0->value[13];
    dense.value[17] = dense0->value[14];
    dense.value[16] = dense0->value[15];
    dense.value[15] = dense0->value[16];
    dense.value[14] = dense0->value[17];
    dense.value[13] = -dense0->value[18];
    dense.value[12] = -dense0->value[19];
    dense.value[11] = dense0->value[20];
    dense.value[10] = dense0->value[21];
    dense.value[9] = -dense0->value[22];
    dense.value[8] = -dense0->value[23];
    dense.value[7] = -dense0->value[24];
    dense.value[6] = -dense0->value[25];
    dense.value[5] = dense0->value[26];
    dense.value[4] = dense0->value[27];
    dense.value[3] = -dense0->value[28];
    dense.value[2] = -dense0->value[29];
    dense.value[1] = dense0->value[30];
    dense.value[0] = dense0->value[31];

    *dense_out = dense;
    return 1;
}

static int unary_dense1_undual(void *out, void *data0, PyAlgebraObject *GA){
    gen1_DenseMultivector* dense0 = (gen1_DenseMultivector*)data0;
    gen1_DenseMultivector* dense_out = (gen1_DenseMultivector*)out;
    gen1_DenseMultivector dense = {{0}};

    dense.value[31] = dense0->value[0];
    dense.value[30] = dense0->value[1];
    dense.value[29] = -dense0->value[2];
    dense.value[28] = -dense0->value[3];
    dense.value[27] = dense0->value[4];
    dense.value[26] = dense0->value[5];
    dense.value[25] = -dense0->value[6];
    dense.value[24] = -dense0->value[7];
    dense.value[23] = -dense0->value[8];
    dense.value[22] = -dense0->value[9];
    dense.value[21] = dense0->value[10];
    dense.value[20] = dense0->value[11];
    dense.value[19] = -dense0->value[12];
    dense.value[18] = -dense0->value[13];
    dense.value[17] = dense0->value[14];
    dense.value[16] = dense0->value[15];
    dense.value[15] = dense0->value[16];
    dense.value[14] = dense0->value[17];
    dense.value[13] = -dense0->value[18];
    dense.value[12] = -dense0->value[19];
    dense.value[11] = dense0->value[20];
    dense.value[10] = dense0->value[21];
    dense.value[9] = -dense0->value[22];
    dense.value[8] = -dense0->value[23];
    dense.value[7] = -dense0->value[24];
    dense.value[6] = -dense0->value[25];
    dense.value[5] = dense0->value[26];
    dense.value[4] = dense0->value[27];
    dense.value[3] = -dense0->value[28];
    dense.value[2] = -dense0->value[29];
    dense.value[1] = dense0->value[30];
    dense.value[0] = dense0->value[31];

    *dense_out = dense;
    return 1;
}


static gen1_BladesMultivector gen1_blades_geometricproduct(gen1_BladesMultivector blades0, gen1_BladesMultivector blades1){
    gen1_BladesMultivector blades = {{0},{0},{0},{0},{0},{0},};

    blades.value0[0] =
    +blades0.value0[0]*blades1.value0[0]
    +blades0.value1[0]*blades1.value1[0]
    +blades0.value1[1]*blades1.value1[1]
    -blades0.value2[0]*blades1.value2[0]
    +blades0.value1[2]*blades1.value1[2]
    -blades0.value2[1]*blades1.value2[1]
    -blades0.value2[2]*blades1.value2[2]
    -blades0.value3[0]*blades1.value3[0]
    +blades0.value1[3]*blades1.value1[3]
    -blades0.value2[3]*blades1.value2[3]
    -blades0.value2[4]*blades1.value2[4]
    -blades0.value3[1]*blades1.value3[1]
    -blades0.value2[5]*blades1.value2[5]
    -blades0.value3[2]*blades1.value3[2]
    -blades0.value3[3]*blades1.value3[3]
    +blades0.value4[0]*blades1.value4[0]
    -blades0.value1[4]*blades1.value1[4]
    +blades0.value2[6]*blades1.value2[6]
    +blades0.value2[7]*blades1.value2[7]
    +blades0.value3[4]*blades1.value3[4]
    +blades0.value2[8]*blades1.value2[8]
    +blades0.value3[5]*blades1.value3[5]
    +blades0.value3[6]*blades1.value3[6]
    -blades0.value4[1]*blades1.value4[1]
    +blades0.value2[9]*blades1.value2[9]
    +blades0.value3[7]*blades1.value3[7]
    +blades0.value3[8]*blades1.value3[8]
    -blades0.value4[2]*blades1.value4[2]
    +blades0.value3[9]*blades1.value3[9]
    -blades0.value4[3]*blades1.value4[3]
    -blades0.value4[4]*blades1.value4[4]
    -blades0.value5[0]*blades1.value5[0]
;
    blades.value1[0] =
    +blades0.value0[0]*blades1.value1[0]
    +blades0.value1[0]*blades1.value0[0]
    -blades0.value1[1]*blades1.value2[0]
    +blades0.value2[0]*blades1.value1[1]
    -blades0.value1[2]*blades1.value2[1]
    +blades0.value2[1]*blades1.value1[2]
    -blades0.value2[2]*blades1.value3[0]
    -blades0.value3[0]*blades1.value2[2]
    -blades0.value1[3]*blades1.value2[3]
    +blades0.value2[3]*blades1.value1[3]
    -blades0.value2[4]*blades1.value3[1]
    -blades0.value3[1]*blades1.value2[4]
    -blades0.value2[5]*blades1.value3[2]
    -blades0.value3[2]*blades1.value2[5]
    +blades0.value3[3]*blades1.value4[0]
    -blades0.value4[0]*blades1.value3[3]
    +blades0.value1[4]*blades1.value2[6]
    -blades0.value2[6]*blades1.value1[4]
    +blades0.value2[7]*blades1.value3[4]
    +blades0.value3[4]*blades1.value2[7]
    +blades0.value2[8]*blades1.value3[5]
    +blades0.value3[5]*blades1.value2[8]
    -blades0.value3[6]*blades1.value4[1]
    +blades0.value4[1]*blades1.value3[6]
    +blades0.value2[9]*blades1.value3[7]
    +blades0.value3[7]*blades1.value2[9]
    -blades0.value3[8]*blades1.value4[2]
    +blades0.value4[2]*blades1.value3[8]
    -blades0.value3[9]*blades1.value4[3]
    +blades0.value4[3]*blades1.value3[9]
    -blades0.value4[4]*blades1.value5[0]
    -blades0.value5[0]*blades1.value4[4]
;
    blades.value1[1] =
    +blades0.value0[0]*blades1.value1[1]
    +blades0.value1[0]*blades1.value2[0]
    +blades0.value1[1]*blades1.value0[0]
    -blades0.value2[0]*blades1.value1[0]
    -blades0.value1[2]*blades1.value2[2]
    +blades0.value2[1]*blades1.value3[0]
    +blades0.value2[2]*blades1.value1[2]
    +blades0.value3[0]*blades1.value2[1]
    -blades0.value1[3]*blades1.value2[4]
    +blades0.value2[3]*blades1.value3[1]
    +blades0.value2[4]*blades1.value1[3]
    +blades0.value3[1]*blades1.value2[3]
    -blades0.value2[5]*blades1.value3[3]
    -blades0.value3[2]*blades1.value4[0]
    -blades0.value3[3]*blades1.value2[5]
    +blades0.value4[0]*blades1.value3[2]
    +blades0.value1[4]*blades1.value2[7]
    -blades0.value2[6]*blades1.value3[4]
    -blades0.value2[7]*blades1.value1[4]
    -blades0.value3[4]*blades1.value2[6]
    +blades0.value2[8]*blades1.value3[6]
    +blades0.value3[5]*blades1.value4[1]
    +blades0.value3[6]*blades1.value2[8]
    -blades0.value4[1]*blades1.value3[5]
    +blades0.value2[9]*blades1.value3[8]
    +blades0.value3[7]*blades1.value4[2]
    +blades0.value3[8]*blades1.value2[9]
    -blades0.value4[2]*blades1.value3[7]
    -blades0.value3[9]*blades1.value4[4]
    +blades0.value4[3]*blades1.value5[0]
    +blades0.value4[4]*blades1.value3[9]
    +blades0.value5[0]*blades1.value4[3]
;
    blades.value2[0] =
    +blades0.value0[0]*blades1.value2[0]
    +blades0.value1[0]*blades1.value1[1]
    -blades0.value1[1]*blades1.value1[0]
    +blades0.value2[0]*blades1.value0[0]
    +blades0.value1[2]*blades1.value3[0]
    -blades0.value2[1]*blades1.value2[2]
    +blades0.value2[2]*blades1.value2[1]
    +blades0.value3[0]*blades1.value1[2]
    +blades0.value1[3]*blades1.value3[1]
    -blades0.value2[3]*blades1.value2[4]
    +blades0.value2[4]*blades1.value2[3]
    +blades0.value3[1]*blades1.value1[3]
    -blades0.value2[5]*blades1.value4[0]
    -blades0.value3[2]*blades1.value3[3]
    +blades0.value3[3]*blades1.value3[2]
    -blades0.value4[0]*blades1.value2[5]
    -blades0.value1[4]*blades1.value3[4]
    +blades0.value2[6]*blades1.value2[7]
    -blades0.value2[7]*blades1.value2[6]
    -blades0.value3[4]*blades1.value1[4]
    +blades0.value2[8]*blades1.value4[1]
    +blades0.value3[5]*blades1.value3[6]
    -blades0.value3[6]*blades1.value3[5]
    +blades0.value4[1]*blades1.value2[8]
    +blades0.value2[9]*blades1.value4[2]
    +blades0.value3[7]*blades1.value3[8]
    -blades0.value3[8]*blades1.value3[7]
    +blades0.value4[2]*blades1.value2[9]
    +blades0.value3[9]*blades1.value5[0]
    -blades0.value4[3]*blades1.value4[4]
    +blades0.value4[4]*blades1.value4[3]
    +blades0.value5[0]*blades1.value3[9]
;
    blades.value1[2] =
    +blades0.value0[0]*blades1.value1[2]
    +blades0.value1[0]*blades1.value2[1]
    +blades0.value1[1]*blades1.value2[2]
    -blades0.value2[0]*blades1.value3[0]
    +blades0.value1[2]*blades1.value0[0]
    -blades0.value2[1]*blades1.value1[0]
    -blades0.value2[2]*blades1.value1[1]
    -blades0.value3[0]*blades1.value2[0]
    -blades0.value1[3]*blades1.value2[5]
    +blades0.value2[3]*blades1.value3[2]
    +blades0.value2[4]*blades1.value3[3]
    +blades0.value3[1]*blades1.value4[0]
    +blades0.value2[5]*blades1.value1[3]
    +blades0.value3[2]*blades1.value2[3]
    +blades0.value3[3]*blades1.value2[4]
    -blades0.value4[0]*blades1.value3[1]
    +blades0.value1[4]*blades1.value2[8]
    -blades0.value2[6]*blades1.value3[5]
    -blades0.value2[7]*blades1.value3[6]
    -blades0.value3[4]*blades1.value4[1]
    -blades0.value2[8]*blades1.value1[4]
    -blades0.value3[5]*blades1.value2[6]
    -blades0.value3[6]*blades1.value2[7]
    +blades0.value4[1]*blades1.value3[4]
    +blades0.value2[9]*blades1.value3[9]
    +blades0.value3[7]*blades1.value4[3]
    +blades0.value3[8]*blades1.value4[4]
    -blades0.value4[2]*blades1.value5[0]
    +blades0.value3[9]*blades1.value2[9]
    -blades0.value4[3]*blades1.value3[7]
    -blades0.value4[4]*blades1.value3[8]
    -blades0.value5[0]*blades1.value4[2]
;
    blades.value2[1] =
    +blades0.value0[0]*blades1.value2[1]
    +blades0.value1[0]*blades1.value1[2]
    -blades0.value1[1]*blades1.value3[0]
    +blades0.value2[0]*blades1.value2[2]
    -blades0.value1[2]*blades1.value1[0]
    +blades0.value2[1]*blades1.value0[0]
    -blades0.value2[2]*blades1.value2[0]
    -blades0.value3[0]*blades1.value1[1]
    +blades0.value1[3]*blades1.value3[2]
    -blades0.value2[3]*blades1.value2[5]
    +blades0.value2[4]*blades1.value4[0]
    +blades0.value3[1]*blades1.value3[3]
    +blades0.value2[5]*blades1.value2[3]
    +blades0.value3[2]*blades1.value1[3]
    -blades0.value3[3]*blades1.value3[1]
    +blades0.value4[0]*blades1.value2[4]
    -blades0.value1[4]*blades1.value3[5]
    +blades0.value2[6]*blades1.value2[8]
    -blades0.value2[7]*blades1.value4[1]
    -blades0.value3[4]*blades1.value3[6]
    -blades0.value2[8]*blades1.value2[6]
    -blades0.value3[5]*blades1.value1[4]
    +blades0.value3[6]*blades1.value3[4]
    -blades0.value4[1]*blades1.value2[7]
    +blades0.value2[9]*blades1.value4[3]
    +blades0.value3[7]*blades1.value3[9]
    -blades0.value3[8]*blades1.value5[0]
    +blades0.value4[2]*blades1.value4[4]
    -blades0.value3[9]*blades1.value3[7]
    +blades0.value4[3]*blades1.value2[9]
    -blades0.value4[4]*blades1.value4[2]
    -blades0.value5[0]*blades1.value3[8]
;
    blades.value2[2] =
    +blades0.value0[0]*blades1.value2[2]
    +blades0.value1[0]*blades1.value3[0]
    +blades0.value1[1]*blades1.value1[2]
    -blades0.value2[0]*blades1.value2[1]
    -blades0.value1[2]*blades1.value1[1]
    +blades0.value2[1]*blades1.value2[0]
    +blades0.value2[2]*blades1.value0[0]
    +blades0.value3[0]*blades1.value1[0]
    +blades0.value1[3]*blades1.value3[3]
    -blades0.value2[3]*blades1.value4[0]
    -blades0.value2[4]*blades1.value2[5]
    -blades0.value3[1]*blades1.value3[2]
    +blades0.value2[5]*blades1.value2[4]
    +blades0.value3[2]*blades1.value3[1]
    +blades0.value3[3]*blades1.value1[3]
    -blades0.value4[0]*blades1.value2[3]
    -blades0.value1[4]*blades1.value3[6]
    +blades0.value2[6]*blades1.value4[1]
    +blades0.value2[7]*blades1.value2[8]
    +blades0.value3[4]*blades1.value3[5]
    -blades0.value2[8]*blades1.value2[7]
    -blades0.value3[5]*blades1.value3[4]
    -blades0.value3[6]*blades1.value1[4]
    +blades0.value4[1]*blades1.value2[6]
    +blades0.value2[9]*blades1.value4[4]
    +blades0.value3[7]*blades1.value5[0]
    +blades0.value3[8]*blades1.value3[9]
    -blades0.value4[2]*blades1.value4[3]
    -blades0.value3[9]*blades1.value3[8]
    +blades0.value4[3]*blades1.value4[2]
    +blades0.value4[4]*blades1.value2[9]
    +blades0.value5[0]*blades1.value3[7]
;
    blades.value3[0] =
    +blades0.value0[0]*blades1.value3[0]
    +blades0.value1[0]*blades1.value2[2]
    -blades0.value1[1]*blades1.value2[1]
    +blades0.value2[0]*blades1.value1[2]
    +blades0.value1[2]*blades1.value2[0]
    -blades0.value2[1]*blades1.value1[1]
    +blades0.value2[2]*blades1.value1[0]
    +blades0.value3[0]*blades1.value0[0]
    -blades0.value1[3]*blades1.value4[0]
    +blades0.value2[3]*blades1.value3[3]
    -blades0.value2[4]*blades1.value3[2]
    -blades0.value3[1]*blades1.value2[5]
    +blades0.value2[5]*blades1.value3[1]
    +blades0.value3[2]*blades1.value2[4]
    -blades0.value3[3]*blades1.value2[3]
    +blades0.value4[0]*blades1.value1[3]
    +blades0.value1[4]*blades1.value4[1]
    -blades0.value2[6]*blades1.value3[6]
    +blades0.value2[7]*blades1.value3[5]
    +blades0.value3[4]*blades1.value2[8]
    -blades0.value2[8]*blades1.value3[4]
    -blades0.value3[5]*blades1.value2[7]
    +blades0.value3[6]*blades1.value2[6]
    -blades0.value4[1]*blades1.value1[4]
    +blades0.value2[9]*blades1.value5[0]
    +blades0.value3[7]*blades1.value4[4]
    -blades0.value3[8]*blades1.value4[3]
    +blades0.value4[2]*blades1.value3[9]
    +blades0.value3[9]*blades1.value4[2]
    -blades0.value4[3]*blades1.value3[8]
    +blades0.value4[4]*blades1.value3[7]
    +blades0.value5[0]*blades1.value2[9]
;
    blades.value1[3] =
    +blades0.value0[0]*blades1.value1[3]
    +blades0.value1[0]*blades1.value2[3]
    +blades0.value1[1]*blades1.value2[4]
    -blades0.value2[0]*blades1.value3[1]
    +blades0.value1[2]*blades1.value2[5]
    -blades0.value2[1]*blades1.value3[2]
    -blades0.value2[2]*blades1.value3[3]
    -blades0.value3[0]*blades1.value4[0]
    +blades0.value1[3]*blades1.value0[0]
    -blades0.value2[3]*blades1.value1[0]
    -blades0.value2[4]*blades1.value1[1]
    -blades0.value3[1]*blades1.value2[0]
    -blades0.value2[5]*blades1.value1[2]
    -blades0.value3[2]*blades1.value2[1]
    -blades0.value3[3]*blades1.value2[2]
    +blades0.value4[0]*blades1.value3[0]
    +blades0.value1[4]*blades1.value2[9]
    -blades0.value2[6]*blades1.value3[7]
    -blades0.value2[7]*blades1.value3[8]
    -blades0.value3[4]*blades1.value4[2]
    -blades0.value2[8]*blades1.value3[9]
    -blades0.value3[5]*blades1.value4[3]
    -blades0.value3[6]*blades1.value4[4]
    +blades0.value4[1]*blades1.value5[0]
    -blades0.value2[9]*blades1.value1[4]
    -blades0.value3[7]*blades1.value2[6]
    -blades0.value3[8]*blades1.value2[7]
    +blades0.value4[2]*blades1.value3[4]
    -blades0.value3[9]*blades1.value2[8]
    +blades0.value4[3]*blades1.value3[5]
    +blades0.value4[4]*blades1.value3[6]
    +blades0.value5[0]*blades1.value4[1]
;
    blades.value2[3] =
    +blades0.value0[0]*blades1.value2[3]
    +blades0.value1[0]*blades1.value1[3]
    -blades0.value1[1]*blades1.value3[1]
    +blades0.value2[0]*blades1.value2[4]
    -blades0.value1[2]*blades1.value3[2]
    +blades0.value2[1]*blades1.value2[5]
    -blades0.value2[2]*blades1.value4[0]
    -blades0.value3[0]*blades1.value3[3]
    -blades0.value1[3]*blades1.value1[0]
    +blades0.value2[3]*blades1.value0[0]
    -blades0.value2[4]*blades1.value2[0]
    -blades0.value3[1]*blades1.value1[1]
    -blades0.value2[5]*blades1.value2[1]
    -blades0.value3[2]*blades1.value1[2]
    +blades0.value3[3]*blades1.value3[0]
    -blades0.value4[0]*blades1.value2[2]
    -blades0.value1[4]*blades1.value3[7]
    +blades0.value2[6]*blades1.value2[9]
    -blades0.value2[7]*blades1.value4[2]
    -blades0.value3[4]*blades1.value3[8]
    -blades0.value2[8]*blades1.value4[3]
    -blades0.value3[5]*blades1.value3[9]
    +blades0.value3[6]*blades1.value5[0]
    -blades0.value4[1]*blades1.value4[4]
    -blades0.value2[9]*blades1.value2[6]
    -blades0.value3[7]*blades1.value1[4]
    +blades0.value3[8]*blades1.value3[4]
    -blades0.value4[2]*blades1.value2[7]
    +blades0.value3[9]*blades1.value3[5]
    -blades0.value4[3]*blades1.value2[8]
    +blades0.value4[4]*blades1.value4[1]
    +blades0.value5[0]*blades1.value3[6]
;
    blades.value2[4] =
    +blades0.value0[0]*blades1.value2[4]
    +blades0.value1[0]*blades1.value3[1]
    +blades0.value1[1]*blades1.value1[3]
    -blades0.value2[0]*blades1.value2[3]
    -blades0.value1[2]*blades1.value3[3]
    +blades0.value2[1]*blades1.value4[0]
    +blades0.value2[2]*blades1.value2[5]
    +blades0.value3[0]*blades1.value3[2]
    -blades0.value1[3]*blades1.value1[1]
    +blades0.value2[3]*blades1.value2[0]
    +blades0.value2[4]*blades1.value0[0]
    +blades0.value3[1]*blades1.value1[0]
    -blades0.value2[5]*blades1.value2[2]
    -blades0.value3[2]*blades1.value3[0]
    -blades0.value3[3]*blades1.value1[2]
    +blades0.value4[0]*blades1.value2[1]
    -blades0.value1[4]*blades1.value3[8]
    +blades0.value2[6]*blades1.value4[2]
    +blades0.value2[7]*blades1.value2[9]
    +blades0.value3[4]*blades1.value3[7]
    -blades0.value2[8]*blades1.value4[4]
    -blades0.value3[5]*blades1.value5[0]
    -blades0.value3[6]*blades1.value3[9]
    +blades0.value4[1]*blades1.value4[3]
    -blades0.value2[9]*blades1.value2[7]
    -blades0.value3[7]*blades1.value3[4]
    -blades0.value3[8]*blades1.value1[4]
    +blades0.value4[2]*blades1.value2[6]
    +blades0.value3[9]*blades1.value3[6]
    -blades0.value4[3]*blades1.value4[1]
    -blades0.value4[4]*blades1.value2[8]
    -blades0.value5[0]*blades1.value3[5]
;
    blades.value3[1] =
    +blades0.value0[0]*blades1.value3[1]
    +blades0.value1[0]*blades1.value2[4]
    -blades0.value1[1]*blades1.value2[3]
    +blades0.value2[0]*blades1.value1[3]
    +blades0.value1[2]*blades1.value4[0]
    -blades0.value2[1]*blades1.value3[3]
    +blades0.value2[2]*blades1.value3[2]
    +blades0.value3[0]*blades1.value2[5]
    +blades0.value1[3]*blades1.value2[0]
    -blades0.value2[3]*blades1.value1[1]
    +blades0.value2[4]*blades1.value1[0]
    +blades0.value3[1]*blades1.value0[0]
    -blades0.value2[5]*blades1.value3[0]
    -blades0.value3[2]*blades1.value2[2]
    +blades0.value3[3]*blades1.value2[1]
    -blades0.value4[0]*blades1.value1[2]
    +blades0.value1[4]*blades1.value4[2]
    -blades0.value2[6]*blades1.value3[8]
    +blades0.value2[7]*blades1.value3[7]
    +blades0.value3[4]*blades1.value2[9]
    -blades0.value2[8]*blades1.value5[0]
    -blades0.value3[5]*blades1.value4[4]
    +blades0.value3[6]*blades1.value4[3]
    -blades0.value4[1]*blades1.value3[9]
    -blades0.value2[9]*blades1.value3[4]
    -blades0.value3[7]*blades1.value2[7]
    +blades0.value3[8]*blades1.value2[6]
    -blades0.value4[2]*blades1.value1[4]
    -blades0.value3[9]*blades1.value4[1]
    +blades0.value4[3]*blades1.value3[6]
    -blades0.value4[4]*blades1.value3[5]
    -blades0.value5[0]*blades1.value2[8]
;
    blades.value2[5] =
    +blades0.value0[0]*blades1.value2[5]
    +blades0.value1[0]*blades1.value3[2]
    +blades0.value1[1]*blades1.value3[3]
    -blades0.value2[0]*blades1.value4[0]
    +blades0.value1[2]*blades1.value1[3]
    -blades0.value2[1]*blades1.value2[3]
    -blades0.value2[2]*blades1.value2[4]
    -blades0.value3[0]*blades1.value3[1]
    -blades0.value1[3]*blades1.value1[2]
    +blades0.value2[3]*blades1.value2[1]
    +blades0.value2[4]*blades1.value2[2]
    +blades0.value3[1]*blades1.value3[0]
    +blades0.value2[5]*blades1.value0[0]
    +blades0.value3[2]*blades1.value1[0]
    +blades0.value3[3]*blades1.value1[1]
    -blades0.value4[0]*blades1.value2[0]
    -blades0.value1[4]*blades1.value3[9]
    +blades0.value2[6]*blades1.value4[3]
    +blades0.value2[7]*blades1.value4[4]
    +blades0.value3[4]*blades1.value5[0]
    +blades0.value2[8]*blades1.value2[9]
    +blades0.value3[5]*blades1.value3[7]
    +blades0.value3[6]*blades1.value3[8]
    -blades0.value4[1]*blades1.value4[2]
    -blades0.value2[9]*blades1.value2[8]
    -blades0.value3[7]*blades1.value3[5]
    -blades0.value3[8]*blades1.value3[6]
    +blades0.value4[2]*blades1.value4[1]
    -blades0.value3[9]*blades1.value1[4]
    +blades0.value4[3]*blades1.value2[6]
    +blades0.value4[4]*blades1.value2[7]
    +blades0.value5[0]*blades1.value3[4]
;
    blades.value3[2] =
    +blades0.value0[0]*blades1.value3[2]
    +blades0.value1[0]*blades1.value2[5]
    -blades0.value1[1]*blades1.value4[0]
    +blades0.value2[0]*blades1.value3[3]
    -blades0.value1[2]*blades1.value2[3]
    +blades0.value2[1]*blades1.value1[3]
    -blades0.value2[2]*blades1.value3[1]
    -blades0.value3[0]*blades1.value2[4]
    +blades0.value1[3]*blades1.value2[1]
    -blades0.value2[3]*blades1.value1[2]
    +blades0.value2[4]*blades1.value3[0]
    +blades0.value3[1]*blades1.value2[2]
    +blades0.value2[5]*blades1.value1[0]
    +blades0.value3[2]*blades1.value0[0]
    -blades0.value3[3]*blades1.value2[0]
    +blades0.value4[0]*blades1.value1[1]
    +blades0.value1[4]*blades1.value4[3]
    -blades0.value2[6]*blades1.value3[9]
    +blades0.value2[7]*blades1.value5[0]
    +blades0.value3[4]*blades1.value4[4]
    +blades0.value2[8]*blades1.value3[7]
    +blades0.value3[5]*blades1.value2[9]
    -blades0.value3[6]*blades1.value4[2]
    +blades0.value4[1]*blades1.value3[8]
    -blades0.value2[9]*blades1.value3[5]
    -blades0.value3[7]*blades1.value2[8]
    +blades0.value3[8]*blades1.value4[1]
    -blades0.value4[2]*blades1.value3[6]
    +blades0.value3[9]*blades1.value2[6]
    -blades0.value4[3]*blades1.value1[4]
    +blades0.value4[4]*blades1.value3[4]
    +blades0.value5[0]*blades1.value2[7]
;
    blades.value3[3] =
    +blades0.value0[0]*blades1.value3[3]
    +blades0.value1[0]*blades1.value4[0]
    +blades0.value1[1]*blades1.value2[5]
    -blades0.value2[0]*blades1.value3[2]
    -blades0.value1[2]*blades1.value2[4]
    +blades0.value2[1]*blades1.value3[1]
    +blades0.value2[2]*blades1.value1[3]
    +blades0.value3[0]*blades1.value2[3]
    +blades0.value1[3]*blades1.value2[2]
    -blades0.value2[3]*blades1.value3[0]
    -blades0.value2[4]*blades1.value1[2]
    -blades0.value3[1]*blades1.value2[1]
    +blades0.value2[5]*blades1.value1[1]
    +blades0.value3[2]*blades1.value2[0]
    +blades0.value3[3]*blades1.value0[0]
    -blades0.value4[0]*blades1.value1[0]
    +blades0.value1[4]*blades1.value4[4]
    -blades0.value2[6]*blades1.value5[0]
    -blades0.value2[7]*blades1.value3[9]
    -blades0.value3[4]*blades1.value4[3]
    +blades0.value2[8]*blades1.value3[8]
    +blades0.value3[5]*blades1.value4[2]
    +blades0.value3[6]*blades1.value2[9]
    -blades0.value4[1]*blades1.value3[7]
    -blades0.value2[9]*blades1.value3[6]
    -blades0.value3[7]*blades1.value4[1]
    -blades0.value3[8]*blades1.value2[8]
    +blades0.value4[2]*blades1.value3[5]
    +blades0.value3[9]*blades1.value2[7]
    -blades0.value4[3]*blades1.value3[4]
    -blades0.value4[4]*blades1.value1[4]
    -blades0.value5[0]*blades1.value2[6]
;
    blades.value4[0] =
    +blades0.value0[0]*blades1.value4[0]
    +blades0.value1[0]*blades1.value3[3]
    -blades0.value1[1]*blades1.value3[2]
    +blades0.value2[0]*blades1.value2[5]
    +blades0.value1[2]*blades1.value3[1]
    -blades0.value2[1]*blades1.value2[4]
    +blades0.value2[2]*blades1.value2[3]
    +blades0.value3[0]*blades1.value1[3]
    -blades0.value1[3]*blades1.value3[0]
    +blades0.value2[3]*blades1.value2[2]
    -blades0.value2[4]*blades1.value2[1]
    -blades0.value3[1]*blades1.value1[2]
    +blades0.value2[5]*blades1.value2[0]
    +blades0.value3[2]*blades1.value1[1]
    -blades0.value3[3]*blades1.value1[0]
    +blades0.value4[0]*blades1.value0[0]
    -blades0.value1[4]*blades1.value5[0]
    +blades0.value2[6]*blades1.value4[4]
    -blades0.value2[7]*blades1.value4[3]
    -blades0.value3[4]*blades1.value3[9]
    +blades0.value2[8]*blades1.value4[2]
    +blades0.value3[5]*blades1.value3[8]
    -blades0.value3[6]*blades1.value3[7]
    +blades0.value4[1]*blades1.value2[9]
    -blades0.value2[9]*blades1.value4[1]
    -blades0.value3[7]*blades1.value3[6]
    +blades0.value3[8]*blades1.value3[5]
    -blades0.value4[2]*blades1.value2[8]
    -blades0.value3[9]*blades1.value3[4]
    +blades0.value4[3]*blades1.value2[7]
    -blades0.value4[4]*blades1.value2[6]
    -blades0.value5[0]*blades1.value1[4]
;
    blades.value1[4] =
    +blades0.value0[0]*blades1.value1[4]
    +blades0.value1[0]*blades1.value2[6]
    +blades0.value1[1]*blades1.value2[7]
    -blades0.value2[0]*blades1.value3[4]
    +blades0.value1[2]*blades1.value2[8]
    -blades0.value2[1]*blades1.value3[5]
    -blades0.value2[2]*blades1.value3[6]
    -blades0.value3[0]*blades1.value4[1]
    +blades0.value1[3]*blades1.value2[9]
    -blades0.value2[3]*blades1.value3[7]
    -blades0.value2[4]*blades1.value3[8]
    -blades0.value3[1]*blades1.value4[2]
    -blades0.value2[5]*blades1.value3[9]
    -blades0.value3[2]*blades1.value4[3]
    -blades0.value3[3]*blades1.value4[4]
    +blades0.value4[0]*blades1.value5[0]
    +blades0.value1[4]*blades1.value0[0]
    -blades0.value2[6]*blades1.value1[0]
    -blades0.value2[7]*blades1.value1[1]
    -blades0.value3[4]*blades1.value2[0]
    -blades0.value2[8]*blades1.value1[2]
    -blades0.value3[5]*blades1.value2[1]
    -blades0.value3[6]*blades1.value2[2]
    +blades0.value4[1]*blades1.value3[0]
    -blades0.value2[9]*blades1.value1[3]
    -blades0.value3[7]*blades1.value2[3]
    -blades0.value3[8]*blades1.value2[4]
    +blades0.value4[2]*blades1.value3[1]
    -blades0.value3[9]*blades1.value2[5]
    +blades0.value4[3]*blades1.value3[2]
    +blades0.value4[4]*blades1.value3[3]
    +blades0.value5[0]*blades1.value4[0]
;
    blades.value2[6] =
    +blades0.value0[0]*blades1.value2[6]
    +blades0.value1[0]*blades1.value1[4]
    -blades0.value1[1]*blades1.value3[4]
    +blades0.value2[0]*blades1.value2[7]
    -blades0.value1[2]*blades1.value3[5]
    +blades0.value2[1]*blades1.value2[8]
    -blades0.value2[2]*blades1.value4[1]
    -blades0.value3[0]*blades1.value3[6]
    -blades0.value1[3]*blades1.value3[7]
    +blades0.value2[3]*blades1.value2[9]
    -blades0.value2[4]*blades1.value4[2]
    -blades0.value3[1]*blades1.value3[8]
    -blades0.value2[5]*blades1.value4[3]
    -blades0.value3[2]*blades1.value3[9]
    +blades0.value3[3]*blades1.value5[0]
    -blades0.value4[0]*blades1.value4[4]
    -blades0.value1[4]*blades1.value1[0]
    +blades0.value2[6]*blades1.value0[0]
    -blades0.value2[7]*blades1.value2[0]
    -blades0.value3[4]*blades1.value1[1]
    -blades0.value2[8]*blades1.value2[1]
    -blades0.value3[5]*blades1.value1[2]
    +blades0.value3[6]*blades1.value3[0]
    -blades0.value4[1]*blades1.value2[2]
    -blades0.value2[9]*blades1.value2[3]
    -blades0.value3[7]*blades1.value1[3]
    +blades0.value3[8]*blades1.value3[1]
    -blades0.value4[2]*blades1.value2[4]
    +blades0.value3[9]*blades1.value3[2]
    -blades0.value4[3]*blades1.value2[5]
    +blades0.value4[4]*blades1.value4[0]
    +blades0.value5[0]*blades1.value3[3]
;
    blades.value2[7] =
    +blades0.value0[0]*blades1.value2[7]
    +blades0.value1[0]*blades1.value3[4]
    +blades0.value1[1]*blades1.value1[4]
    -blades0.value2[0]*blades1.value2[6]
    -blades0.value1[2]*blades1.value3[6]
    +blades0.value2[1]*blades1.value4[1]
    +blades0.value2[2]*blades1.value2[8]
    +blades0.value3[0]*blades1.value3[5]
    -blades0.value1[3]*blades1.value3[8]
    +blades0.value2[3]*blades1.value4[2]
    +blades0.value2[4]*blades1.value2[9]
    +blades0.value3[1]*blades1.value3[7]
    -blades0.value2[5]*blades1.value4[4]
    -blades0.value3[2]*blades1.value5[0]
    -blades0.value3[3]*blades1.value3[9]
    +blades0.value4[0]*blades1.value4[3]
    -blades0.value1[4]*blades1.value1[1]
    +blades0.value2[6]*blades1.value2[0]
    +blades0.value2[7]*blades1.value0[0]
    +blades0.value3[4]*blades1.value1[0]
    -blades0.value2[8]*blades1.value2[2]
    -blades0.value3[5]*blades1.value3[0]
    -blades0.value3[6]*blades1.value1[2]
    +blades0.value4[1]*blades1.value2[1]
    -blades0.value2[9]*blades1.value2[4]
    -blades0.value3[7]*blades1.value3[1]
    -blades0.value3[8]*blades1.value1[3]
    +blades0.value4[2]*blades1.value2[3]
    +blades0.value3[9]*blades1.value3[3]
    -blades0.value4[3]*blades1.value4[0]
    -blades0.value4[4]*blades1.value2[5]
    -blades0.value5[0]*blades1.value3[2]
;
    blades.value3[4] =
    +blades0.value0[0]*blades1.value3[4]
    +blades0.value1[0]*blades1.value2[7]
    -blades0.value1[1]*blades1.value2[6]
    +blades0.value2[0]*blades1.value1[4]
    +blades0.value1[2]*blades1.value4[1]
    -blades0.value2[1]*blades1.value3[6]
    +blades0.value2[2]*blades1.value3[5]
    +blades0.value3[0]*blades1.value2[8]
    +blades0.value1[3]*blades1.value4[2]
    -blades0.value2[3]*blades1.value3[8]
    +blades0.value2[4]*blades1.value3[7]
    +blades0.value3[1]*blades1.value2[9]
    -blades0.value2[5]*blades1.value5[0]
    -blades0.value3[2]*blades1.value4[4]
    +blades0.value3[3]*blades1.value4[3]
    -blades0.value4[0]*blades1.value3[9]
    +blades0.value1[4]*blades1.value2[0]
    -blades0.value2[6]*blades1.value1[1]
    +blades0.value2[7]*blades1.value1[0]
    +blades0.value3[4]*blades1.value0[0]
    -blades0.value2[8]*blades1.value3[0]
    -blades0.value3[5]*blades1.value2[2]
    +blades0.value3[6]*blades1.value2[1]
    -blades0.value4[1]*blades1.value1[2]
    -blades0.value2[9]*blades1.value3[1]
    -blades0.value3[7]*blades1.value2[4]
    +blades0.value3[8]*blades1.value2[3]
    -blades0.value4[2]*blades1.value1[3]
    -blades0.value3[9]*blades1.value4[0]
    +blades0.value4[3]*blades1.value3[3]
    -blades0.value4[4]*blades1.value3[2]
    -blades0.value5[0]*blades1.value2[5]
;
    blades.value2[8] =
    +blades0.value0[0]*blades1.value2[8]
    +blades0.value1[0]*blades1.value3[5]
    +blades0.value1[1]*blades1.value3[6]
    -blades0.value2[0]*blades1.value4[1]
    +blades0.value1[2]*blades1.value1[4]
    -blades0.value2[1]*blades1.value2[6]
    -blades0.value2[2]*blades1.value2[7]
    -blades0.value3[0]*blades1.value3[4]
    -blades0.value1[3]*blades1.value3[9]
    +blades0.value2[3]*blades1.value4[3]
    +blades0.value2[4]*blades1.value4[4]
    +blades0.value3[1]*blades1.value5[0]
    +blades0.value2[5]*blades1.value2[9]
    +blades0.value3[2]*blades1.value3[7]
    +blades0.value3[3]*blades1.value3[8]
    -blades0.value4[0]*blades1.value4[2]
    -blades0.value1[4]*blades1.value1[2]
    +blades0.value2[6]*blades1.value2[1]
    +blades0.value2[7]*blades1.value2[2]
    +blades0.value3[4]*blades1.value3[0]
    +blades0.value2[8]*blades1.value0[0]
    +blades0.value3[5]*blades1.value1[0]
    +blades0.value3[6]*blades1.value1[1]
    -blades0.value4[1]*blades1.value2[0]
    -blades0.value2[9]*blades1.value2[5]
    -blades0.value3[7]*blades1.value3[2]
    -blades0.value3[8]*blades1.value3[3]
    +blades0.value4[2]*blades1.value4[0]
    -blades0.value3[9]*blades1.value1[3]
    +blades0.value4[3]*blades1.value2[3]
    +blades0.value4[4]*blades1.value2[4]
    +blades0.value5[0]*blades1.value3[1]
;
    blades.value3[5] =
    +blades0.value0[0]*blades1.value3[5]
    +blades0.value1[0]*blades1.value2[8]
    -blades0.value1[1]*blades1.value4[1]
    +blades0.value2[0]*blades1.value3[6]
    -blades0.value1[2]*blades1.value2[6]
    +blades0.value2[1]*blades1.value1[4]
    -blades0.value2[2]*blades1.value3[4]
    -blades0.value3[0]*blades1.value2[7]
    +blades0.value1[3]*blades1.value4[3]
    -blades0.value2[3]*blades1.value3[9]
    +blades0.value2[4]*blades1.value5[0]
    +blades0.value3[1]*blades1.value4[4]
    +blades0.value2[5]*blades1.value3[7]
    +blades0.value3[2]*blades1.value2[9]
    -blades0.value3[3]*blades1.value4[2]
    +blades0.value4[0]*blades1.value3[8]
    +blades0.value1[4]*blades1.value2[1]
    -blades0.value2[6]*blades1.value1[2]
    +blades0.value2[7]*blades1.value3[0]
    +blades0.value3[4]*blades1.value2[2]
    +blades0.value2[8]*blades1.value1[0]
    +blades0.value3[5]*blades1.value0[0]
    -blades0.value3[6]*blades1.value2[0]
    +blades0.value4[1]*blades1.value1[1]
    -blades0.value2[9]*blades1.value3[2]
    -blades0.value3[7]*blades1.value2[5]
    +blades0.value3[8]*blades1.value4[0]
    -blades0.value4[2]*blades1.value3[3]
    +blades0.value3[9]*blades1.value2[3]
    -blades0.value4[3]*blades1.value1[3]
    +blades0.value4[4]*blades1.value3[1]
    +blades0.value5[0]*blades1.value2[4]
;
    blades.value3[6] =
    +blades0.value0[0]*blades1.value3[6]
    +blades0.value1[0]*blades1.value4[1]
    +blades0.value1[1]*blades1.value2[8]
    -blades0.value2[0]*blades1.value3[5]
    -blades0.value1[2]*blades1.value2[7]
    +blades0.value2[1]*blades1.value3[4]
    +blades0.value2[2]*blades1.value1[4]
    +blades0.value3[0]*blades1.value2[6]
    +blades0.value1[3]*blades1.value4[4]
    -blades0.value2[3]*blades1.value5[0]
    -blades0.value2[4]*blades1.value3[9]
    -blades0.value3[1]*blades1.value4[3]
    +blades0.value2[5]*blades1.value3[8]
    +blades0.value3[2]*blades1.value4[2]
    +blades0.value3[3]*blades1.value2[9]
    -blades0.value4[0]*blades1.value3[7]
    +blades0.value1[4]*blades1.value2[2]
    -blades0.value2[6]*blades1.value3[0]
    -blades0.value2[7]*blades1.value1[2]
    -blades0.value3[4]*blades1.value2[1]
    +blades0.value2[8]*blades1.value1[1]
    +blades0.value3[5]*blades1.value2[0]
    +blades0.value3[6]*blades1.value0[0]
    -blades0.value4[1]*blades1.value1[0]
    -blades0.value2[9]*blades1.value3[3]
    -blades0.value3[7]*blades1.value4[0]
    -blades0.value3[8]*blades1.value2[5]
    +blades0.value4[2]*blades1.value3[2]
    +blades0.value3[9]*blades1.value2[4]
    -blades0.value4[3]*blades1.value3[1]
    -blades0.value4[4]*blades1.value1[3]
    -blades0.value5[0]*blades1.value2[3]
;
    blades.value4[1] =
    +blades0.value0[0]*blades1.value4[1]
    +blades0.value1[0]*blades1.value3[6]
    -blades0.value1[1]*blades1.value3[5]
    +blades0.value2[0]*blades1.value2[8]
    +blades0.value1[2]*blades1.value3[4]
    -blades0.value2[1]*blades1.value2[7]
    +blades0.value2[2]*blades1.value2[6]
    +blades0.value3[0]*blades1.value1[4]
    -blades0.value1[3]*blades1.value5[0]
    +blades0.value2[3]*blades1.value4[4]
    -blades0.value2[4]*blades1.value4[3]
    -blades0.value3[1]*blades1.value3[9]
    +blades0.value2[5]*blades1.value4[2]
    +blades0.value3[2]*blades1.value3[8]
    -blades0.value3[3]*blades1.value3[7]
    +blades0.value4[0]*blades1.value2[9]
    -blades0.value1[4]*blades1.value3[0]
    +blades0.value2[6]*blades1.value2[2]
    -blades0.value2[7]*blades1.value2[1]
    -blades0.value3[4]*blades1.value1[2]
    +blades0.value2[8]*blades1.value2[0]
    +blades0.value3[5]*blades1.value1[1]
    -blades0.value3[6]*blades1.value1[0]
    +blades0.value4[1]*blades1.value0[0]
    -blades0.value2[9]*blades1.value4[0]
    -blades0.value3[7]*blades1.value3[3]
    +blades0.value3[8]*blades1.value3[2]
    -blades0.value4[2]*blades1.value2[5]
    -blades0.value3[9]*blades1.value3[1]
    +blades0.value4[3]*blades1.value2[4]
    -blades0.value4[4]*blades1.value2[3]
    -blades0.value5[0]*blades1.value1[3]
;
    blades.value2[9] =
    +blades0.value0[0]*blades1.value2[9]
    +blades0.value1[0]*blades1.value3[7]
    +blades0.value1[1]*blades1.value3[8]
    -blades0.value2[0]*blades1.value4[2]
    +blades0.value1[2]*blades1.value3[9]
    -blades0.value2[1]*blades1.value4[3]
    -blades0.value2[2]*blades1.value4[4]
    -blades0.value3[0]*blades1.value5[0]
    +blades0.value1[3]*blades1.value1[4]
    -blades0.value2[3]*blades1.value2[6]
    -blades0.value2[4]*blades1.value2[7]
    -blades0.value3[1]*blades1.value3[4]
    -blades0.value2[5]*blades1.value2[8]
    -blades0.value3[2]*blades1.value3[5]
    -blades0.value3[3]*blades1.value3[6]
    +blades0.value4[0]*blades1.value4[1]
    -blades0.value1[4]*blades1.value1[3]
    +blades0.value2[6]*blades1.value2[3]
    +blades0.value2[7]*blades1.value2[4]
    +blades0.value3[4]*blades1.value3[1]
    +blades0.value2[8]*blades1.value2[5]
    +blades0.value3[5]*blades1.value3[2]
    +blades0.value3[6]*blades1.value3[3]
    -blades0.value4[1]*blades1.value4[0]
    +blades0.value2[9]*blades1.value0[0]
    +blades0.value3[7]*blades1.value1[0]
    +blades0.value3[8]*blades1.value1[1]
    -blades0.value4[2]*blades1.value2[0]
    +blades0.value3[9]*blades1.value1[2]
    -blades0.value4[3]*blades1.value2[1]
    -blades0.value4[4]*blades1.value2[2]
    -blades0.value5[0]*blades1.value3[0]
;
    blades.value3[7] =
    +blades0.value0[0]*blades1.value3[7]
    +blades0.value1[0]*blades1.value2[9]
    -blades0.value1[1]*blades1.value4[2]
    +blades0.value2[0]*blades1.value3[8]
    -blades0.value1[2]*blades1.value4[3]
    +blades0.value2[1]*blades1.value3[9]
    -blades0.value2[2]*blades1.value5[0]
    -blades0.value3[0]*blades1.value4[4]
    -blades0.value1[3]*blades1.value2[6]
    +blades0.value2[3]*blades1.value1[4]
    -blades0.value2[4]*blades1.value3[4]
    -blades0.value3[1]*blades1.value2[7]
    -blades0.value2[5]*blades1.value3[5]
    -blades0.value3[2]*blades1.value2[8]
    +blades0.value3[3]*blades1.value4[1]
    -blades0.value4[0]*blades1.value3[6]
    +blades0.value1[4]*blades1.value2[3]
    -blades0.value2[6]*blades1.value1[3]
    +blades0.value2[7]*blades1.value3[1]
    +blades0.value3[4]*blades1.value2[4]
    +blades0.value2[8]*blades1.value3[2]
    +blades0.value3[5]*blades1.value2[5]
    -blades0.value3[6]*blades1.value4[0]
    +blades0.value4[1]*blades1.value3[3]
    +blades0.value2[9]*blades1.value1[0]
    +blades0.value3[7]*blades1.value0[0]
    -blades0.value3[8]*blades1.value2[0]
    +blades0.value4[2]*blades1.value1[1]
    -blades0.value3[9]*blades1.value2[1]
    +blades0.value4[3]*blades1.value1[2]
    -blades0.value4[4]*blades1.value3[0]
    -blades0.value5[0]*blades1.value2[2]
;
    blades.value3[8] =
    +blades0.value0[0]*blades1.value3[8]
    +blades0.value1[0]*blades1.value4[2]
    +blades0.value1[1]*blades1.value2[9]
    -blades0.value2[0]*blades1.value3[7]
    -blades0.value1[2]*blades1.value4[4]
    +blades0.value2[1]*blades1.value5[0]
    +blades0.value2[2]*blades1.value3[9]
    +blades0.value3[0]*blades1.value4[3]
    -blades0.value1[3]*blades1.value2[7]
    +blades0.value2[3]*blades1.value3[4]
    +blades0.value2[4]*blades1.value1[4]
    +blades0.value3[1]*blades1.value2[6]
    -blades0.value2[5]*blades1.value3[6]
    -blades0.value3[2]*blades1.value4[1]
    -blades0.value3[3]*blades1.value2[8]
    +blades0.value4[0]*blades1.value3[5]
    +blades0.value1[4]*blades1.value2[4]
    -blades0.value2[6]*blades1.value3[1]
    -blades0.value2[7]*blades1.value1[3]
    -blades0.value3[4]*blades1.value2[3]
    +blades0.value2[8]*blades1.value3[3]
    +blades0.value3[5]*blades1.value4[0]
    +blades0.value3[6]*blades1.value2[5]
    -blades0.value4[1]*blades1.value3[2]
    +blades0.value2[9]*blades1.value1[1]
    +blades0.value3[7]*blades1.value2[0]
    +blades0.value3[8]*blades1.value0[0]
    -blades0.value4[2]*blades1.value1[0]
    -blades0.value3[9]*blades1.value2[2]
    +blades0.value4[3]*blades1.value3[0]
    +blades0.value4[4]*blades1.value1[2]
    +blades0.value5[0]*blades1.value2[1]
;
    blades.value4[2] =
    +blades0.value0[0]*blades1.value4[2]
    +blades0.value1[0]*blades1.value3[8]
    -blades0.value1[1]*blades1.value3[7]
    +blades0.value2[0]*blades1.value2[9]
    +blades0.value1[2]*blades1.value5[0]
    -blades0.value2[1]*blades1.value4[4]
    +blades0.value2[2]*blades1.value4[3]
    +blades0.value3[0]*blades1.value3[9]
    +blades0.value1[3]*blades1.value3[4]
    -blades0.value2[3]*blades1.value2[7]
    +blades0.value2[4]*blades1.value2[6]
    +blades0.value3[1]*blades1.value1[4]
    -blades0.value2[5]*blades1.value4[1]
    -blades0.value3[2]*blades1.value3[6]
    +blades0.value3[3]*blades1.value3[5]
    -blades0.value4[0]*blades1.value2[8]
    -blades0.value1[4]*blades1.value3[1]
    +blades0.value2[6]*blades1.value2[4]
    -blades0.value2[7]*blades1.value2[3]
    -blades0.value3[4]*blades1.value1[3]
    +blades0.value2[8]*blades1.value4[0]
    +blades0.value3[5]*blades1.value3[3]
    -blades0.value3[6]*blades1.value3[2]
    +blades0.value4[1]*blades1.value2[5]
    +blades0.value2[9]*blades1.value2[0]
    +blades0.value3[7]*blades1.value1[1]
    -blades0.value3[8]*blades1.value1[0]
    +blades0.value4[2]*blades1.value0[0]
    +blades0.value3[9]*blades1.value3[0]
    -blades0.value4[3]*blades1.value2[2]
    +blades0.value4[4]*blades1.value2[1]
    +blades0.value5[0]*blades1.value1[2]
;
    blades.value3[9] =
    +blades0.value0[0]*blades1.value3[9]
    +blades0.value1[0]*blades1.value4[3]
    +blades0.value1[1]*blades1.value4[4]
    -blades0.value2[0]*blades1.value5[0]
    +blades0.value1[2]*blades1.value2[9]
    -blades0.value2[1]*blades1.value3[7]
    -blades0.value2[2]*blades1.value3[8]
    -blades0.value3[0]*blades1.value4[2]
    -blades0.value1[3]*blades1.value2[8]
    +blades0.value2[3]*blades1.value3[5]
    +blades0.value2[4]*blades1.value3[6]
    +blades0.value3[1]*blades1.value4[1]
    +blades0.value2[5]*blades1.value1[4]
    +blades0.value3[2]*blades1.value2[6]
    +blades0.value3[3]*blades1.value2[7]
    -blades0.value4[0]*blades1.value3[4]
    +blades0.value1[4]*blades1.value2[5]
    -blades0.value2[6]*blades1.value3[2]
    -blades0.value2[7]*blades1.value3[3]
    -blades0.value3[4]*blades1.value4[0]
    -blades0.value2[8]*blades1.value1[3]
    -blades0.value3[5]*blades1.value2[3]
    -blades0.value3[6]*blades1.value2[4]
    +blades0.value4[1]*blades1.value3[1]
    +blades0.value2[9]*blades1.value1[2]
    +blades0.value3[7]*blades1.value2[1]
    +blades0.value3[8]*blades1.value2[2]
    -blades0.value4[2]*blades1.value3[0]
    +blades0.value3[9]*blades1.value0[0]
    -blades0.value4[3]*blades1.value1[0]
    -blades0.value4[4]*blades1.value1[1]
    -blades0.value5[0]*blades1.value2[0]
;
    blades.value4[3] =
    +blades0.value0[0]*blades1.value4[3]
    +blades0.value1[0]*blades1.value3[9]
    -blades0.value1[1]*blades1.value5[0]
    +blades0.value2[0]*blades1.value4[4]
    -blades0.value1[2]*blades1.value3[7]
    +blades0.value2[1]*blades1.value2[9]
    -blades0.value2[2]*blades1.value4[2]
    -blades0.value3[0]*blades1.value3[8]
    +blades0.value1[3]*blades1.value3[5]
    -blades0.value2[3]*blades1.value2[8]
    +blades0.value2[4]*blades1.value4[1]
    +blades0.value3[1]*blades1.value3[6]
    +blades0.value2[5]*blades1.value2[6]
    +blades0.value3[2]*blades1.value1[4]
    -blades0.value3[3]*blades1.value3[4]
    +blades0.value4[0]*blades1.value2[7]
    -blades0.value1[4]*blades1.value3[2]
    +blades0.value2[6]*blades1.value2[5]
    -blades0.value2[7]*blades1.value4[0]
    -blades0.value3[4]*blades1.value3[3]
    -blades0.value2[8]*blades1.value2[3]
    -blades0.value3[5]*blades1.value1[3]
    +blades0.value3[6]*blades1.value3[1]
    -blades0.value4[1]*blades1.value2[4]
    +blades0.value2[9]*blades1.value2[1]
    +blades0.value3[7]*blades1.value1[2]
    -blades0.value3[8]*blades1.value3[0]
    +blades0.value4[2]*blades1.value2[2]
    -blades0.value3[9]*blades1.value1[0]
    +blades0.value4[3]*blades1.value0[0]
    -blades0.value4[4]*blades1.value2[0]
    -blades0.value5[0]*blades1.value1[1]
;
    blades.value4[4] =
    +blades0.value0[0]*blades1.value4[4]
    +blades0.value1[0]*blades1.value5[0]
    +blades0.value1[1]*blades1.value3[9]
    -blades0.value2[0]*blades1.value4[3]
    -blades0.value1[2]*blades1.value3[8]
    +blades0.value2[1]*blades1.value4[2]
    +blades0.value2[2]*blades1.value2[9]
    +blades0.value3[0]*blades1.value3[7]
    +blades0.value1[3]*blades1.value3[6]
    -blades0.value2[3]*blades1.value4[1]
    -blades0.value2[4]*blades1.value2[8]
    -blades0.value3[1]*blades1.value3[5]
    +blades0.value2[5]*blades1.value2[7]
    +blades0.value3[2]*blades1.value3[4]
    +blades0.value3[3]*blades1.value1[4]
    -blades0.value4[0]*blades1.value2[6]
    -blades0.value1[4]*blades1.value3[3]
    +blades0.value2[6]*blades1.value4[0]
    +blades0.value2[7]*blades1.value2[5]
    +blades0.value3[4]*blades1.value3[2]
    -blades0.value2[8]*blades1.value2[4]
    -blades0.value3[5]*blades1.value3[1]
    -blades0.value3[6]*blades1.value1[3]
    +blades0.value4[1]*blades1.value2[3]
    +blades0.value2[9]*blades1.value2[2]
    +blades0.value3[7]*blades1.value3[0]
    +blades0.value3[8]*blades1.value1[2]
    -blades0.value4[2]*blades1.value2[1]
    -blades0.value3[9]*blades1.value1[1]
    +blades0.value4[3]*blades1.value2[0]
    +blades0.value4[4]*blades1.value0[0]
    +blades0.value5[0]*blades1.value1[0]
;
    blades.value5[0] =
    +blades0.value0[0]*blades1.value5[0]
    +blades0.value1[0]*blades1.value4[4]
    -blades0.value1[1]*blades1.value4[3]
    +blades0.value2[0]*blades1.value3[9]
    +blades0.value1[2]*blades1.value4[2]
    -blades0.value2[1]*blades1.value3[8]
    +blades0.value2[2]*blades1.value3[7]
    +blades0.value3[0]*blades1.value2[9]
    -blades0.value1[3]*blades1.value4[1]
    +blades0.value2[3]*blades1.value3[6]
    -blades0.value2[4]*blades1.value3[5]
    -blades0.value3[1]*blades1.value2[8]
    +blades0.value2[5]*blades1.value3[4]
    +blades0.value3[2]*blades1.value2[7]
    -blades0.value3[3]*blades1.value2[6]
    +blades0.value4[0]*blades1.value1[4]
    +blades0.value1[4]*blades1.value4[0]
    -blades0.value2[6]*blades1.value3[3]
    +blades0.value2[7]*blades1.value3[2]
    +blades0.value3[4]*blades1.value2[5]
    -blades0.value2[8]*blades1.value3[1]
    -blades0.value3[5]*blades1.value2[4]
    +blades0.value3[6]*blades1.value2[3]
    -blades0.value4[1]*blades1.value1[3]
    +blades0.value2[9]*blades1.value3[0]
    +blades0.value3[7]*blades1.value2[2]
    -blades0.value3[8]*blades1.value2[1]
    +blades0.value4[2]*blades1.value1[2]
    +blades0.value3[9]*blades1.value2[0]
    -blades0.value4[3]*blades1.value1[1]
    +blades0.value4[4]*blades1.value1[0]
    +blades0.value5[0]*blades1.value0[0]
;
    return blades;
}

#define GEN1_BLADES_GRADE0GEOMETRICPRODUCT(blades,blades0,blades1){\
    blades.value0[0] =\
    +blades0.value0[0]*blades1.value0[0]\
    +blades0.value1[0]*blades1.value1[0]\
    +blades0.value1[1]*blades1.value1[1]\
    -blades0.value2[0]*blades1.value2[0]\
    +blades0.value1[2]*blades1.value1[2]\
    -blades0.value2[1]*blades1.value2[1]\
    -blades0.value2[2]*blades1.value2[2]\
    -blades0.value3[0]*blades1.value3[0]\
    +blades0.value1[3]*blades1.value1[3]\
    -blades0.value2[3]*blades1.value2[3]\
    -blades0.value2[4]*blades1.value2[4]\
    -blades0.value3[1]*blades1.value3[1]\
    -blades0.value2[5]*blades1.value2[5]\
    -blades0.value3[2]*blades1.value3[2]\
    -blades0.value3[3]*blades1.value3[3]\
    +blades0.value4[0]*blades1.value4[0]\
    -blades0.value1[4]*blades1.value1[4]\
    +blades0.value2[6]*blades1.value2[6]\
    +blades0.value2[7]*blades1.value2[7]\
    +blades0.value3[4]*blades1.value3[4]\
    +blades0.value2[8]*blades1.value2[8]\
    +blades0.value3[5]*blades1.value3[5]\
    +blades0.value3[6]*blades1.value3[6]\
    -blades0.value4[1]*blades1.value4[1]\
    +blades0.value2[9]*blades1.value2[9]\
    +blades0.value3[7]*blades1.value3[7]\
    +blades0.value3[8]*blades1.value3[8]\
    -blades0.value4[2]*blades1.value4[2]\
    +blades0.value3[9]*blades1.value3[9]\
    -blades0.value4[3]*blades1.value4[3]\
    -blades0.value4[4]*blades1.value4[4]\
    -blades0.value5[0]*blades1.value5[0]\
;\
}
#define GEN1_BLADES_GRADE1GEOMETRICPRODUCT(blades,blades0,blades1){\
    blades.value1[0] =\
    +blades0.value0[0]*blades1.value1[0]\
    +blades0.value1[0]*blades1.value0[0]\
    -blades0.value1[1]*blades1.value2[0]\
    +blades0.value2[0]*blades1.value1[1]\
    -blades0.value1[2]*blades1.value2[1]\
    +blades0.value2[1]*blades1.value1[2]\
    -blades0.value2[2]*blades1.value3[0]\
    -blades0.value3[0]*blades1.value2[2]\
    -blades0.value1[3]*blades1.value2[3]\
    +blades0.value2[3]*blades1.value1[3]\
    -blades0.value2[4]*blades1.value3[1]\
    -blades0.value3[1]*blades1.value2[4]\
    -blades0.value2[5]*blades1.value3[2]\
    -blades0.value3[2]*blades1.value2[5]\
    +blades0.value3[3]*blades1.value4[0]\
    -blades0.value4[0]*blades1.value3[3]\
    +blades0.value1[4]*blades1.value2[6]\
    -blades0.value2[6]*blades1.value1[4]\
    +blades0.value2[7]*blades1.value3[4]\
    +blades0.value3[4]*blades1.value2[7]\
    +blades0.value2[8]*blades1.value3[5]\
    +blades0.value3[5]*blades1.value2[8]\
    -blades0.value3[6]*blades1.value4[1]\
    +blades0.value4[1]*blades1.value3[6]\
    +blades0.value2[9]*blades1.value3[7]\
    +blades0.value3[7]*blades1.value2[9]\
    -blades0.value3[8]*blades1.value4[2]\
    +blades0.value4[2]*blades1.value3[8]\
    -blades0.value3[9]*blades1.value4[3]\
    +blades0.value4[3]*blades1.value3[9]\
    -blades0.value4[4]*blades1.value5[0]\
    -blades0.value5[0]*blades1.value4[4]\
;\
    blades.value1[1] =\
    +blades0.value0[0]*blades1.value1[1]\
    +blades0.value1[0]*blades1.value2[0]\
    +blades0.value1[1]*blades1.value0[0]\
    -blades0.value2[0]*blades1.value1[0]\
    -blades0.value1[2]*blades1.value2[2]\
    +blades0.value2[1]*blades1.value3[0]\
    +blades0.value2[2]*blades1.value1[2]\
    +blades0.value3[0]*blades1.value2[1]\
    -blades0.value1[3]*blades1.value2[4]\
    +blades0.value2[3]*blades1.value3[1]\
    +blades0.value2[4]*blades1.value1[3]\
    +blades0.value3[1]*blades1.value2[3]\
    -blades0.value2[5]*blades1.value3[3]\
    -blades0.value3[2]*blades1.value4[0]\
    -blades0.value3[3]*blades1.value2[5]\
    +blades0.value4[0]*blades1.value3[2]\
    +blades0.value1[4]*blades1.value2[7]\
    -blades0.value2[6]*blades1.value3[4]\
    -blades0.value2[7]*blades1.value1[4]\
    -blades0.value3[4]*blades1.value2[6]\
    +blades0.value2[8]*blades1.value3[6]\
    +blades0.value3[5]*blades1.value4[1]\
    +blades0.value3[6]*blades1.value2[8]\
    -blades0.value4[1]*blades1.value3[5]\
    +blades0.value2[9]*blades1.value3[8]\
    +blades0.value3[7]*blades1.value4[2]\
    +blades0.value3[8]*blades1.value2[9]\
    -blades0.value4[2]*blades1.value3[7]\
    -blades0.value3[9]*blades1.value4[4]\
    +blades0.value4[3]*blades1.value5[0]\
    +blades0.value4[4]*blades1.value3[9]\
    +blades0.value5[0]*blades1.value4[3]\
;\
    blades.value1[2] =\
    +blades0.value0[0]*blades1.value1[2]\
    +blades0.value1[0]*blades1.value2[1]\
    +blades0.value1[1]*blades1.value2[2]\
    -blades0.value2[0]*blades1.value3[0]\
    +blades0.value1[2]*blades1.value0[0]\
    -blades0.value2[1]*blades1.value1[0]\
    -blades0.value2[2]*blades1.value1[1]\
    -blades0.value3[0]*blades1.value2[0]\
    -blades0.value1[3]*blades1.value2[5]\
    +blades0.value2[3]*blades1.value3[2]\
    +blades0.value2[4]*blades1.value3[3]\
    +blades0.value3[1]*blades1.value4[0]\
    +blades0.value2[5]*blades1.value1[3]\
    +blades0.value3[2]*blades1.value2[3]\
    +blades0.value3[3]*blades1.value2[4]\
    -blades0.value4[0]*blades1.value3[1]\
    +blades0.value1[4]*blades1.value2[8]\
    -blades0.value2[6]*blades1.value3[5]\
    -blades0.value2[7]*blades1.value3[6]\
    -blades0.value3[4]*blades1.value4[1]\
    -blades0.value2[8]*blades1.value1[4]\
    -blades0.value3[5]*blades1.value2[6]\
    -blades0.value3[6]*blades1.value2[7]\
    +blades0.value4[1]*blades1.value3[4]\
    +blades0.value2[9]*blades1.value3[9]\
    +blades0.value3[7]*blades1.value4[3]\
    +blades0.value3[8]*blades1.value4[4]\
    -blades0.value4[2]*blades1.value5[0]\
    +blades0.value3[9]*blades1.value2[9]\
    -blades0.value4[3]*blades1.value3[7]\
    -blades0.value4[4]*blades1.value3[8]\
    -blades0.value5[0]*blades1.value4[2]\
;\
    blades.value1[3] =\
    +blades0.value0[0]*blades1.value1[3]\
    +blades0.value1[0]*blades1.value2[3]\
    +blades0.value1[1]*blades1.value2[4]\
    -blades0.value2[0]*blades1.value3[1]\
    +blades0.value1[2]*blades1.value2[5]\
    -blades0.value2[1]*blades1.value3[2]\
    -blades0.value2[2]*blades1.value3[3]\
    -blades0.value3[0]*blades1.value4[0]\
    +blades0.value1[3]*blades1.value0[0]\
    -blades0.value2[3]*blades1.value1[0]\
    -blades0.value2[4]*blades1.value1[1]\
    -blades0.value3[1]*blades1.value2[0]\
    -blades0.value2[5]*blades1.value1[2]\
    -blades0.value3[2]*blades1.value2[1]\
    -blades0.value3[3]*blades1.value2[2]\
    +blades0.value4[0]*blades1.value3[0]\
    +blades0.value1[4]*blades1.value2[9]\
    -blades0.value2[6]*blades1.value3[7]\
    -blades0.value2[7]*blades1.value3[8]\
    -blades0.value3[4]*blades1.value4[2]\
    -blades0.value2[8]*blades1.value3[9]\
    -blades0.value3[5]*blades1.value4[3]\
    -blades0.value3[6]*blades1.value4[4]\
    +blades0.value4[1]*blades1.value5[0]\
    -blades0.value2[9]*blades1.value1[4]\
    -blades0.value3[7]*blades1.value2[6]\
    -blades0.value3[8]*blades1.value2[7]\
    +blades0.value4[2]*blades1.value3[4]\
    -blades0.value3[9]*blades1.value2[8]\
    +blades0.value4[3]*blades1.value3[5]\
    +blades0.value4[4]*blades1.value3[6]\
    +blades0.value5[0]*blades1.value4[1]\
;\
    blades.value1[4] =\
    +blades0.value0[0]*blades1.value1[4]\
    +blades0.value1[0]*blades1.value2[6]\
    +blades0.value1[1]*blades1.value2[7]\
    -blades0.value2[0]*blades1.value3[4]\
    +blades0.value1[2]*blades1.value2[8]\
    -blades0.value2[1]*blades1.value3[5]\
    -blades0.value2[2]*blades1.value3[6]\
    -blades0.value3[0]*blades1.value4[1]\
    +blades0.value1[3]*blades1.value2[9]\
    -blades0.value2[3]*blades1.value3[7]\
    -blades0.value2[4]*blades1.value3[8]\
    -blades0.value3[1]*blades1.value4[2]\
    -blades0.value2[5]*blades1.value3[9]\
    -blades0.value3[2]*blades1.value4[3]\
    -blades0.value3[3]*blades1.value4[4]\
    +blades0.value4[0]*blades1.value5[0]\
    +blades0.value1[4]*blades1.value0[0]\
    -blades0.value2[6]*blades1.value1[0]\
    -blades0.value2[7]*blades1.value1[1]\
    -blades0.value3[4]*blades1.value2[0]\
    -blades0.value2[8]*blades1.value1[2]\
    -blades0.value3[5]*blades1.value2[1]\
    -blades0.value3[6]*blades1.value2[2]\
    +blades0.value4[1]*blades1.value3[0]\
    -blades0.value2[9]*blades1.value1[3]\
    -blades0.value3[7]*blades1.value2[3]\
    -blades0.value3[8]*blades1.value2[4]\
    +blades0.value4[2]*blades1.value3[1]\
    -blades0.value3[9]*blades1.value2[5]\
    +blades0.value4[3]*blades1.value3[2]\
    +blades0.value4[4]*blades1.value3[3]\
    +blades0.value5[0]*blades1.value4[0]\
;\
}
#define GEN1_BLADES_GRADE2GEOMETRICPRODUCT(blades,blades0,blades1){\
    blades.value2[0] =\
    +blades0.value0[0]*blades1.value2[0]\
    +blades0.value1[0]*blades1.value1[1]\
    -blades0.value1[1]*blades1.value1[0]\
    +blades0.value2[0]*blades1.value0[0]\
    +blades0.value1[2]*blades1.value3[0]\
    -blades0.value2[1]*blades1.value2[2]\
    +blades0.value2[2]*blades1.value2[1]\
    +blades0.value3[0]*blades1.value1[2]\
    +blades0.value1[3]*blades1.value3[1]\
    -blades0.value2[3]*blades1.value2[4]\
    +blades0.value2[4]*blades1.value2[3]\
    +blades0.value3[1]*blades1.value1[3]\
    -blades0.value2[5]*blades1.value4[0]\
    -blades0.value3[2]*blades1.value3[3]\
    +blades0.value3[3]*blades1.value3[2]\
    -blades0.value4[0]*blades1.value2[5]\
    -blades0.value1[4]*blades1.value3[4]\
    +blades0.value2[6]*blades1.value2[7]\
    -blades0.value2[7]*blades1.value2[6]\
    -blades0.value3[4]*blades1.value1[4]\
    +blades0.value2[8]*blades1.value4[1]\
    +blades0.value3[5]*blades1.value3[6]\
    -blades0.value3[6]*blades1.value3[5]\
    +blades0.value4[1]*blades1.value2[8]\
    +blades0.value2[9]*blades1.value4[2]\
    +blades0.value3[7]*blades1.value3[8]\
    -blades0.value3[8]*blades1.value3[7]\
    +blades0.value4[2]*blades1.value2[9]\
    +blades0.value3[9]*blades1.value5[0]\
    -blades0.value4[3]*blades1.value4[4]\
    +blades0.value4[4]*blades1.value4[3]\
    +blades0.value5[0]*blades1.value3[9]\
;\
    blades.value2[1] =\
    +blades0.value0[0]*blades1.value2[1]\
    +blades0.value1[0]*blades1.value1[2]\
    -blades0.value1[1]*blades1.value3[0]\
    +blades0.value2[0]*blades1.value2[2]\
    -blades0.value1[2]*blades1.value1[0]\
    +blades0.value2[1]*blades1.value0[0]\
    -blades0.value2[2]*blades1.value2[0]\
    -blades0.value3[0]*blades1.value1[1]\
    +blades0.value1[3]*blades1.value3[2]\
    -blades0.value2[3]*blades1.value2[5]\
    +blades0.value2[4]*blades1.value4[0]\
    +blades0.value3[1]*blades1.value3[3]\
    +blades0.value2[5]*blades1.value2[3]\
    +blades0.value3[2]*blades1.value1[3]\
    -blades0.value3[3]*blades1.value3[1]\
    +blades0.value4[0]*blades1.value2[4]\
    -blades0.value1[4]*blades1.value3[5]\
    +blades0.value2[6]*blades1.value2[8]\
    -blades0.value2[7]*blades1.value4[1]\
    -blades0.value3[4]*blades1.value3[6]\
    -blades0.value2[8]*blades1.value2[6]\
    -blades0.value3[5]*blades1.value1[4]\
    +blades0.value3[6]*blades1.value3[4]\
    -blades0.value4[1]*blades1.value2[7]\
    +blades0.value2[9]*blades1.value4[3]\
    +blades0.value3[7]*blades1.value3[9]\
    -blades0.value3[8]*blades1.value5[0]\
    +blades0.value4[2]*blades1.value4[4]\
    -blades0.value3[9]*blades1.value3[7]\
    +blades0.value4[3]*blades1.value2[9]\
    -blades0.value4[4]*blades1.value4[2]\
    -blades0.value5[0]*blades1.value3[8]\
;\
    blades.value2[2] =\
    +blades0.value0[0]*blades1.value2[2]\
    +blades0.value1[0]*blades1.value3[0]\
    +blades0.value1[1]*blades1.value1[2]\
    -blades0.value2[0]*blades1.value2[1]\
    -blades0.value1[2]*blades1.value1[1]\
    +blades0.value2[1]*blades1.value2[0]\
    +blades0.value2[2]*blades1.value0[0]\
    +blades0.value3[0]*blades1.value1[0]\
    +blades0.value1[3]*blades1.value3[3]\
    -blades0.value2[3]*blades1.value4[0]\
    -blades0.value2[4]*blades1.value2[5]\
    -blades0.value3[1]*blades1.value3[2]\
    +blades0.value2[5]*blades1.value2[4]\
    +blades0.value3[2]*blades1.value3[1]\
    +blades0.value3[3]*blades1.value1[3]\
    -blades0.value4[0]*blades1.value2[3]\
    -blades0.value1[4]*blades1.value3[6]\
    +blades0.value2[6]*blades1.value4[1]\
    +blades0.value2[7]*blades1.value2[8]\
    +blades0.value3[4]*blades1.value3[5]\
    -blades0.value2[8]*blades1.value2[7]\
    -blades0.value3[5]*blades1.value3[4]\
    -blades0.value3[6]*blades1.value1[4]\
    +blades0.value4[1]*blades1.value2[6]\
    +blades0.value2[9]*blades1.value4[4]\
    +blades0.value3[7]*blades1.value5[0]\
    +blades0.value3[8]*blades1.value3[9]\
    -blades0.value4[2]*blades1.value4[3]\
    -blades0.value3[9]*blades1.value3[8]\
    +blades0.value4[3]*blades1.value4[2]\
    +blades0.value4[4]*blades1.value2[9]\
    +blades0.value5[0]*blades1.value3[7]\
;\
    blades.value2[3] =\
    +blades0.value0[0]*blades1.value2[3]\
    +blades0.value1[0]*blades1.value1[3]\
    -blades0.value1[1]*blades1.value3[1]\
    +blades0.value2[0]*blades1.value2[4]\
    -blades0.value1[2]*blades1.value3[2]\
    +blades0.value2[1]*blades1.value2[5]\
    -blades0.value2[2]*blades1.value4[0]\
    -blades0.value3[0]*blades1.value3[3]\
    -blades0.value1[3]*blades1.value1[0]\
    +blades0.value2[3]*blades1.value0[0]\
    -blades0.value2[4]*blades1.value2[0]\
    -blades0.value3[1]*blades1.value1[1]\
    -blades0.value2[5]*blades1.value2[1]\
    -blades0.value3[2]*blades1.value1[2]\
    +blades0.value3[3]*blades1.value3[0]\
    -blades0.value4[0]*blades1.value2[2]\
    -blades0.value1[4]*blades1.value3[7]\
    +blades0.value2[6]*blades1.value2[9]\
    -blades0.value2[7]*blades1.value4[2]\
    -blades0.value3[4]*blades1.value3[8]\
    -blades0.value2[8]*blades1.value4[3]\
    -blades0.value3[5]*blades1.value3[9]\
    +blades0.value3[6]*blades1.value5[0]\
    -blades0.value4[1]*blades1.value4[4]\
    -blades0.value2[9]*blades1.value2[6]\
    -blades0.value3[7]*blades1.value1[4]\
    +blades0.value3[8]*blades1.value3[4]\
    -blades0.value4[2]*blades1.value2[7]\
    +blades0.value3[9]*blades1.value3[5]\
    -blades0.value4[3]*blades1.value2[8]\
    +blades0.value4[4]*blades1.value4[1]\
    +blades0.value5[0]*blades1.value3[6]\
;\
    blades.value2[4] =\
    +blades0.value0[0]*blades1.value2[4]\
    +blades0.value1[0]*blades1.value3[1]\
    +blades0.value1[1]*blades1.value1[3]\
    -blades0.value2[0]*blades1.value2[3]\
    -blades0.value1[2]*blades1.value3[3]\
    +blades0.value2[1]*blades1.value4[0]\
    +blades0.value2[2]*blades1.value2[5]\
    +blades0.value3[0]*blades1.value3[2]\
    -blades0.value1[3]*blades1.value1[1]\
    +blades0.value2[3]*blades1.value2[0]\
    +blades0.value2[4]*blades1.value0[0]\
    +blades0.value3[1]*blades1.value1[0]\
    -blades0.value2[5]*blades1.value2[2]\
    -blades0.value3[2]*blades1.value3[0]\
    -blades0.value3[3]*blades1.value1[2]\
    +blades0.value4[0]*blades1.value2[1]\
    -blades0.value1[4]*blades1.value3[8]\
    +blades0.value2[6]*blades1.value4[2]\
    +blades0.value2[7]*blades1.value2[9]\
    +blades0.value3[4]*blades1.value3[7]\
    -blades0.value2[8]*blades1.value4[4]\
    -blades0.value3[5]*blades1.value5[0]\
    -blades0.value3[6]*blades1.value3[9]\
    +blades0.value4[1]*blades1.value4[3]\
    -blades0.value2[9]*blades1.value2[7]\
    -blades0.value3[7]*blades1.value3[4]\
    -blades0.value3[8]*blades1.value1[4]\
    +blades0.value4[2]*blades1.value2[6]\
    +blades0.value3[9]*blades1.value3[6]\
    -blades0.value4[3]*blades1.value4[1]\
    -blades0.value4[4]*blades1.value2[8]\
    -blades0.value5[0]*blades1.value3[5]\
;\
    blades.value2[5] =\
    +blades0.value0[0]*blades1.value2[5]\
    +blades0.value1[0]*blades1.value3[2]\
    +blades0.value1[1]*blades1.value3[3]\
    -blades0.value2[0]*blades1.value4[0]\
    +blades0.value1[2]*blades1.value1[3]\
    -blades0.value2[1]*blades1.value2[3]\
    -blades0.value2[2]*blades1.value2[4]\
    -blades0.value3[0]*blades1.value3[1]\
    -blades0.value1[3]*blades1.value1[2]\
    +blades0.value2[3]*blades1.value2[1]\
    +blades0.value2[4]*blades1.value2[2]\
    +blades0.value3[1]*blades1.value3[0]\
    +blades0.value2[5]*blades1.value0[0]\
    +blades0.value3[2]*blades1.value1[0]\
    +blades0.value3[3]*blades1.value1[1]\
    -blades0.value4[0]*blades1.value2[0]\
    -blades0.value1[4]*blades1.value3[9]\
    +blades0.value2[6]*blades1.value4[3]\
    +blades0.value2[7]*blades1.value4[4]\
    +blades0.value3[4]*blades1.value5[0]\
    +blades0.value2[8]*blades1.value2[9]\
    +blades0.value3[5]*blades1.value3[7]\
    +blades0.value3[6]*blades1.value3[8]\
    -blades0.value4[1]*blades1.value4[2]\
    -blades0.value2[9]*blades1.value2[8]\
    -blades0.value3[7]*blades1.value3[5]\
    -blades0.value3[8]*blades1.value3[6]\
    +blades0.value4[2]*blades1.value4[1]\
    -blades0.value3[9]*blades1.value1[4]\
    +blades0.value4[3]*blades1.value2[6]\
    +blades0.value4[4]*blades1.value2[7]\
    +blades0.value5[0]*blades1.value3[4]\
;\
    blades.value2[6] =\
    +blades0.value0[0]*blades1.value2[6]\
    +blades0.value1[0]*blades1.value1[4]\
    -blades0.value1[1]*blades1.value3[4]\
    +blades0.value2[0]*blades1.value2[7]\
    -blades0.value1[2]*blades1.value3[5]\
    +blades0.value2[1]*blades1.value2[8]\
    -blades0.value2[2]*blades1.value4[1]\
    -blades0.value3[0]*blades1.value3[6]\
    -blades0.value1[3]*blades1.value3[7]\
    +blades0.value2[3]*blades1.value2[9]\
    -blades0.value2[4]*blades1.value4[2]\
    -blades0.value3[1]*blades1.value3[8]\
    -blades0.value2[5]*blades1.value4[3]\
    -blades0.value3[2]*blades1.value3[9]\
    +blades0.value3[3]*blades1.value5[0]\
    -blades0.value4[0]*blades1.value4[4]\
    -blades0.value1[4]*blades1.value1[0]\
    +blades0.value2[6]*blades1.value0[0]\
    -blades0.value2[7]*blades1.value2[0]\
    -blades0.value3[4]*blades1.value1[1]\
    -blades0.value2[8]*blades1.value2[1]\
    -blades0.value3[5]*blades1.value1[2]\
    +blades0.value3[6]*blades1.value3[0]\
    -blades0.value4[1]*blades1.value2[2]\
    -blades0.value2[9]*blades1.value2[3]\
    -blades0.value3[7]*blades1.value1[3]\
    +blades0.value3[8]*blades1.value3[1]\
    -blades0.value4[2]*blades1.value2[4]\
    +blades0.value3[9]*blades1.value3[2]\
    -blades0.value4[3]*blades1.value2[5]\
    +blades0.value4[4]*blades1.value4[0]\
    +blades0.value5[0]*blades1.value3[3]\
;\
    blades.value2[7] =\
    +blades0.value0[0]*blades1.value2[7]\
    +blades0.value1[0]*blades1.value3[4]\
    +blades0.value1[1]*blades1.value1[4]\
    -blades0.value2[0]*blades1.value2[6]\
    -blades0.value1[2]*blades1.value3[6]\
    +blades0.value2[1]*blades1.value4[1]\
    +blades0.value2[2]*blades1.value2[8]\
    +blades0.value3[0]*blades1.value3[5]\
    -blades0.value1[3]*blades1.value3[8]\
    +blades0.value2[3]*blades1.value4[2]\
    +blades0.value2[4]*blades1.value2[9]\
    +blades0.value3[1]*blades1.value3[7]\
    -blades0.value2[5]*blades1.value4[4]\
    -blades0.value3[2]*blades1.value5[0]\
    -blades0.value3[3]*blades1.value3[9]\
    +blades0.value4[0]*blades1.value4[3]\
    -blades0.value1[4]*blades1.value1[1]\
    +blades0.value2[6]*blades1.value2[0]\
    +blades0.value2[7]*blades1.value0[0]\
    +blades0.value3[4]*blades1.value1[0]\
    -blades0.value2[8]*blades1.value2[2]\
    -blades0.value3[5]*blades1.value3[0]\
    -blades0.value3[6]*blades1.value1[2]\
    +blades0.value4[1]*blades1.value2[1]\
    -blades0.value2[9]*blades1.value2[4]\
    -blades0.value3[7]*blades1.value3[1]\
    -blades0.value3[8]*blades1.value1[3]\
    +blades0.value4[2]*blades1.value2[3]\
    +blades0.value3[9]*blades1.value3[3]\
    -blades0.value4[3]*blades1.value4[0]\
    -blades0.value4[4]*blades1.value2[5]\
    -blades0.value5[0]*blades1.value3[2]\
;\
    blades.value2[8] =\
    +blades0.value0[0]*blades1.value2[8]\
    +blades0.value1[0]*blades1.value3[5]\
    +blades0.value1[1]*blades1.value3[6]\
    -blades0.value2[0]*blades1.value4[1]\
    +blades0.value1[2]*blades1.value1[4]\
    -blades0.value2[1]*blades1.value2[6]\
    -blades0.value2[2]*blades1.value2[7]\
    -blades0.value3[0]*blades1.value3[4]\
    -blades0.value1[3]*blades1.value3[9]\
    +blades0.value2[3]*blades1.value4[3]\
    +blades0.value2[4]*blades1.value4[4]\
    +blades0.value3[1]*blades1.value5[0]\
    +blades0.value2[5]*blades1.value2[9]\
    +blades0.value3[2]*blades1.value3[7]\
    +blades0.value3[3]*blades1.value3[8]\
    -blades0.value4[0]*blades1.value4[2]\
    -blades0.value1[4]*blades1.value1[2]\
    +blades0.value2[6]*blades1.value2[1]\
    +blades0.value2[7]*blades1.value2[2]\
    +blades0.value3[4]*blades1.value3[0]\
    +blades0.value2[8]*blades1.value0[0]\
    +blades0.value3[5]*blades1.value1[0]\
    +blades0.value3[6]*blades1.value1[1]\
    -blades0.value4[1]*blades1.value2[0]\
    -blades0.value2[9]*blades1.value2[5]\
    -blades0.value3[7]*blades1.value3[2]\
    -blades0.value3[8]*blades1.value3[3]\
    +blades0.value4[2]*blades1.value4[0]\
    -blades0.value3[9]*blades1.value1[3]\
    +blades0.value4[3]*blades1.value2[3]\
    +blades0.value4[4]*blades1.value2[4]\
    +blades0.value5[0]*blades1.value3[1]\
;\
    blades.value2[9] =\
    +blades0.value0[0]*blades1.value2[9]\
    +blades0.value1[0]*blades1.value3[7]\
    +blades0.value1[1]*blades1.value3[8]\
    -blades0.value2[0]*blades1.value4[2]\
    +blades0.value1[2]*blades1.value3[9]\
    -blades0.value2[1]*blades1.value4[3]\
    -blades0.value2[2]*blades1.value4[4]\
    -blades0.value3[0]*blades1.value5[0]\
    +blades0.value1[3]*blades1.value1[4]\
    -blades0.value2[3]*blades1.value2[6]\
    -blades0.value2[4]*blades1.value2[7]\
    -blades0.value3[1]*blades1.value3[4]\
    -blades0.value2[5]*blades1.value2[8]\
    -blades0.value3[2]*blades1.value3[5]\
    -blades0.value3[3]*blades1.value3[6]\
    +blades0.value4[0]*blades1.value4[1]\
    -blades0.value1[4]*blades1.value1[3]\
    +blades0.value2[6]*blades1.value2[3]\
    +blades0.value2[7]*blades1.value2[4]\
    +blades0.value3[4]*blades1.value3[1]\
    +blades0.value2[8]*blades1.value2[5]\
    +blades0.value3[5]*blades1.value3[2]\
    +blades0.value3[6]*blades1.value3[3]\
    -blades0.value4[1]*blades1.value4[0]\
    +blades0.value2[9]*blades1.value0[0]\
    +blades0.value3[7]*blades1.value1[0]\
    +blades0.value3[8]*blades1.value1[1]\
    -blades0.value4[2]*blades1.value2[0]\
    +blades0.value3[9]*blades1.value1[2]\
    -blades0.value4[3]*blades1.value2[1]\
    -blades0.value4[4]*blades1.value2[2]\
    -blades0.value5[0]*blades1.value3[0]\
;\
}
#define GEN1_BLADES_GRADE3GEOMETRICPRODUCT(blades,blades0,blades1){\
    blades.value3[0] =\
    +blades0.value0[0]*blades1.value3[0]\
    +blades0.value1[0]*blades1.value2[2]\
    -blades0.value1[1]*blades1.value2[1]\
    +blades0.value2[0]*blades1.value1[2]\
    +blades0.value1[2]*blades1.value2[0]\
    -blades0.value2[1]*blades1.value1[1]\
    +blades0.value2[2]*blades1.value1[0]\
    +blades0.value3[0]*blades1.value0[0]\
    -blades0.value1[3]*blades1.value4[0]\
    +blades0.value2[3]*blades1.value3[3]\
    -blades0.value2[4]*blades1.value3[2]\
    -blades0.value3[1]*blades1.value2[5]\
    +blades0.value2[5]*blades1.value3[1]\
    +blades0.value3[2]*blades1.value2[4]\
    -blades0.value3[3]*blades1.value2[3]\
    +blades0.value4[0]*blades1.value1[3]\
    +blades0.value1[4]*blades1.value4[1]\
    -blades0.value2[6]*blades1.value3[6]\
    +blades0.value2[7]*blades1.value3[5]\
    +blades0.value3[4]*blades1.value2[8]\
    -blades0.value2[8]*blades1.value3[4]\
    -blades0.value3[5]*blades1.value2[7]\
    +blades0.value3[6]*blades1.value2[6]\
    -blades0.value4[1]*blades1.value1[4]\
    +blades0.value2[9]*blades1.value5[0]\
    +blades0.value3[7]*blades1.value4[4]\
    -blades0.value3[8]*blades1.value4[3]\
    +blades0.value4[2]*blades1.value3[9]\
    +blades0.value3[9]*blades1.value4[2]\
    -blades0.value4[3]*blades1.value3[8]\
    +blades0.value4[4]*blades1.value3[7]\
    +blades0.value5[0]*blades1.value2[9]\
;\
    blades.value3[1] =\
    +blades0.value0[0]*blades1.value3[1]\
    +blades0.value1[0]*blades1.value2[4]\
    -blades0.value1[1]*blades1.value2[3]\
    +blades0.value2[0]*blades1.value1[3]\
    +blades0.value1[2]*blades1.value4[0]\
    -blades0.value2[1]*blades1.value3[3]\
    +blades0.value2[2]*blades1.value3[2]\
    +blades0.value3[0]*blades1.value2[5]\
    +blades0.value1[3]*blades1.value2[0]\
    -blades0.value2[3]*blades1.value1[1]\
    +blades0.value2[4]*blades1.value1[0]\
    +blades0.value3[1]*blades1.value0[0]\
    -blades0.value2[5]*blades1.value3[0]\
    -blades0.value3[2]*blades1.value2[2]\
    +blades0.value3[3]*blades1.value2[1]\
    -blades0.value4[0]*blades1.value1[2]\
    +blades0.value1[4]*blades1.value4[2]\
    -blades0.value2[6]*blades1.value3[8]\
    +blades0.value2[7]*blades1.value3[7]\
    +blades0.value3[4]*blades1.value2[9]\
    -blades0.value2[8]*blades1.value5[0]\
    -blades0.value3[5]*blades1.value4[4]\
    +blades0.value3[6]*blades1.value4[3]\
    -blades0.value4[1]*blades1.value3[9]\
    -blades0.value2[9]*blades1.value3[4]\
    -blades0.value3[7]*blades1.value2[7]\
    +blades0.value3[8]*blades1.value2[6]\
    -blades0.value4[2]*blades1.value1[4]\
    -blades0.value3[9]*blades1.value4[1]\
    +blades0.value4[3]*blades1.value3[6]\
    -blades0.value4[4]*blades1.value3[5]\
    -blades0.value5[0]*blades1.value2[8]\
;\
    blades.value3[2] =\
    +blades0.value0[0]*blades1.value3[2]\
    +blades0.value1[0]*blades1.value2[5]\
    -blades0.value1[1]*blades1.value4[0]\
    +blades0.value2[0]*blades1.value3[3]\
    -blades0.value1[2]*blades1.value2[3]\
    +blades0.value2[1]*blades1.value1[3]\
    -blades0.value2[2]*blades1.value3[1]\
    -blades0.value3[0]*blades1.value2[4]\
    +blades0.value1[3]*blades1.value2[1]\
    -blades0.value2[3]*blades1.value1[2]\
    +blades0.value2[4]*blades1.value3[0]\
    +blades0.value3[1]*blades1.value2[2]\
    +blades0.value2[5]*blades1.value1[0]\
    +blades0.value3[2]*blades1.value0[0]\
    -blades0.value3[3]*blades1.value2[0]\
    +blades0.value4[0]*blades1.value1[1]\
    +blades0.value1[4]*blades1.value4[3]\
    -blades0.value2[6]*blades1.value3[9]\
    +blades0.value2[7]*blades1.value5[0]\
    +blades0.value3[4]*blades1.value4[4]\
    +blades0.value2[8]*blades1.value3[7]\
    +blades0.value3[5]*blades1.value2[9]\
    -blades0.value3[6]*blades1.value4[2]\
    +blades0.value4[1]*blades1.value3[8]\
    -blades0.value2[9]*blades1.value3[5]\
    -blades0.value3[7]*blades1.value2[8]\
    +blades0.value3[8]*blades1.value4[1]\
    -blades0.value4[2]*blades1.value3[6]\
    +blades0.value3[9]*blades1.value2[6]\
    -blades0.value4[3]*blades1.value1[4]\
    +blades0.value4[4]*blades1.value3[4]\
    +blades0.value5[0]*blades1.value2[7]\
;\
    blades.value3[3] =\
    +blades0.value0[0]*blades1.value3[3]\
    +blades0.value1[0]*blades1.value4[0]\
    +blades0.value1[1]*blades1.value2[5]\
    -blades0.value2[0]*blades1.value3[2]\
    -blades0.value1[2]*blades1.value2[4]\
    +blades0.value2[1]*blades1.value3[1]\
    +blades0.value2[2]*blades1.value1[3]\
    +blades0.value3[0]*blades1.value2[3]\
    +blades0.value1[3]*blades1.value2[2]\
    -blades0.value2[3]*blades1.value3[0]\
    -blades0.value2[4]*blades1.value1[2]\
    -blades0.value3[1]*blades1.value2[1]\
    +blades0.value2[5]*blades1.value1[1]\
    +blades0.value3[2]*blades1.value2[0]\
    +blades0.value3[3]*blades1.value0[0]\
    -blades0.value4[0]*blades1.value1[0]\
    +blades0.value1[4]*blades1.value4[4]\
    -blades0.value2[6]*blades1.value5[0]\
    -blades0.value2[7]*blades1.value3[9]\
    -blades0.value3[4]*blades1.value4[3]\
    +blades0.value2[8]*blades1.value3[8]\
    +blades0.value3[5]*blades1.value4[2]\
    +blades0.value3[6]*blades1.value2[9]\
    -blades0.value4[1]*blades1.value3[7]\
    -blades0.value2[9]*blades1.value3[6]\
    -blades0.value3[7]*blades1.value4[1]\
    -blades0.value3[8]*blades1.value2[8]\
    +blades0.value4[2]*blades1.value3[5]\
    +blades0.value3[9]*blades1.value2[7]\
    -blades0.value4[3]*blades1.value3[4]\
    -blades0.value4[4]*blades1.value1[4]\
    -blades0.value5[0]*blades1.value2[6]\
;\
    blades.value3[4] =\
    +blades0.value0[0]*blades1.value3[4]\
    +blades0.value1[0]*blades1.value2[7]\
    -blades0.value1[1]*blades1.value2[6]\
    +blades0.value2[0]*blades1.value1[4]\
    +blades0.value1[2]*blades1.value4[1]\
    -blades0.value2[1]*blades1.value3[6]\
    +blades0.value2[2]*blades1.value3[5]\
    +blades0.value3[0]*blades1.value2[8]\
    +blades0.value1[3]*blades1.value4[2]\
    -blades0.value2[3]*blades1.value3[8]\
    +blades0.value2[4]*blades1.value3[7]\
    +blades0.value3[1]*blades1.value2[9]\
    -blades0.value2[5]*blades1.value5[0]\
    -blades0.value3[2]*blades1.value4[4]\
    +blades0.value3[3]*blades1.value4[3]\
    -blades0.value4[0]*blades1.value3[9]\
    +blades0.value1[4]*blades1.value2[0]\
    -blades0.value2[6]*blades1.value1[1]\
    +blades0.value2[7]*blades1.value1[0]\
    +blades0.value3[4]*blades1.value0[0]\
    -blades0.value2[8]*blades1.value3[0]\
    -blades0.value3[5]*blades1.value2[2]\
    +blades0.value3[6]*blades1.value2[1]\
    -blades0.value4[1]*blades1.value1[2]\
    -blades0.value2[9]*blades1.value3[1]\
    -blades0.value3[7]*blades1.value2[4]\
    +blades0.value3[8]*blades1.value2[3]\
    -blades0.value4[2]*blades1.value1[3]\
    -blades0.value3[9]*blades1.value4[0]\
    +blades0.value4[3]*blades1.value3[3]\
    -blades0.value4[4]*blades1.value3[2]\
    -blades0.value5[0]*blades1.value2[5]\
;\
    blades.value3[5] =\
    +blades0.value0[0]*blades1.value3[5]\
    +blades0.value1[0]*blades1.value2[8]\
    -blades0.value1[1]*blades1.value4[1]\
    +blades0.value2[0]*blades1.value3[6]\
    -blades0.value1[2]*blades1.value2[6]\
    +blades0.value2[1]*blades1.value1[4]\
    -blades0.value2[2]*blades1.value3[4]\
    -blades0.value3[0]*blades1.value2[7]\
    +blades0.value1[3]*blades1.value4[3]\
    -blades0.value2[3]*blades1.value3[9]\
    +blades0.value2[4]*blades1.value5[0]\
    +blades0.value3[1]*blades1.value4[4]\
    +blades0.value2[5]*blades1.value3[7]\
    +blades0.value3[2]*blades1.value2[9]\
    -blades0.value3[3]*blades1.value4[2]\
    +blades0.value4[0]*blades1.value3[8]\
    +blades0.value1[4]*blades1.value2[1]\
    -blades0.value2[6]*blades1.value1[2]\
    +blades0.value2[7]*blades1.value3[0]\
    +blades0.value3[4]*blades1.value2[2]\
    +blades0.value2[8]*blades1.value1[0]\
    +blades0.value3[5]*blades1.value0[0]\
    -blades0.value3[6]*blades1.value2[0]\
    +blades0.value4[1]*blades1.value1[1]\
    -blades0.value2[9]*blades1.value3[2]\
    -blades0.value3[7]*blades1.value2[5]\
    +blades0.value3[8]*blades1.value4[0]\
    -blades0.value4[2]*blades1.value3[3]\
    +blades0.value3[9]*blades1.value2[3]\
    -blades0.value4[3]*blades1.value1[3]\
    +blades0.value4[4]*blades1.value3[1]\
    +blades0.value5[0]*blades1.value2[4]\
;\
    blades.value3[6] =\
    +blades0.value0[0]*blades1.value3[6]\
    +blades0.value1[0]*blades1.value4[1]\
    +blades0.value1[1]*blades1.value2[8]\
    -blades0.value2[0]*blades1.value3[5]\
    -blades0.value1[2]*blades1.value2[7]\
    +blades0.value2[1]*blades1.value3[4]\
    +blades0.value2[2]*blades1.value1[4]\
    +blades0.value3[0]*blades1.value2[6]\
    +blades0.value1[3]*blades1.value4[4]\
    -blades0.value2[3]*blades1.value5[0]\
    -blades0.value2[4]*blades1.value3[9]\
    -blades0.value3[1]*blades1.value4[3]\
    +blades0.value2[5]*blades1.value3[8]\
    +blades0.value3[2]*blades1.value4[2]\
    +blades0.value3[3]*blades1.value2[9]\
    -blades0.value4[0]*blades1.value3[7]\
    +blades0.value1[4]*blades1.value2[2]\
    -blades0.value2[6]*blades1.value3[0]\
    -blades0.value2[7]*blades1.value1[2]\
    -blades0.value3[4]*blades1.value2[1]\
    +blades0.value2[8]*blades1.value1[1]\
    +blades0.value3[5]*blades1.value2[0]\
    +blades0.value3[6]*blades1.value0[0]\
    -blades0.value4[1]*blades1.value1[0]\
    -blades0.value2[9]*blades1.value3[3]\
    -blades0.value3[7]*blades1.value4[0]\
    -blades0.value3[8]*blades1.value2[5]\
    +blades0.value4[2]*blades1.value3[2]\
    +blades0.value3[9]*blades1.value2[4]\
    -blades0.value4[3]*blades1.value3[1]\
    -blades0.value4[4]*blades1.value1[3]\
    -blades0.value5[0]*blades1.value2[3]\
;\
    blades.value3[7] =\
    +blades0.value0[0]*blades1.value3[7]\
    +blades0.value1[0]*blades1.value2[9]\
    -blades0.value1[1]*blades1.value4[2]\
    +blades0.value2[0]*blades1.value3[8]\
    -blades0.value1[2]*blades1.value4[3]\
    +blades0.value2[1]*blades1.value3[9]\
    -blades0.value2[2]*blades1.value5[0]\
    -blades0.value3[0]*blades1.value4[4]\
    -blades0.value1[3]*blades1.value2[6]\
    +blades0.value2[3]*blades1.value1[4]\
    -blades0.value2[4]*blades1.value3[4]\
    -blades0.value3[1]*blades1.value2[7]\
    -blades0.value2[5]*blades1.value3[5]\
    -blades0.value3[2]*blades1.value2[8]\
    +blades0.value3[3]*blades1.value4[1]\
    -blades0.value4[0]*blades1.value3[6]\
    +blades0.value1[4]*blades1.value2[3]\
    -blades0.value2[6]*blades1.value1[3]\
    +blades0.value2[7]*blades1.value3[1]\
    +blades0.value3[4]*blades1.value2[4]\
    +blades0.value2[8]*blades1.value3[2]\
    +blades0.value3[5]*blades1.value2[5]\
    -blades0.value3[6]*blades1.value4[0]\
    +blades0.value4[1]*blades1.value3[3]\
    +blades0.value2[9]*blades1.value1[0]\
    +blades0.value3[7]*blades1.value0[0]\
    -blades0.value3[8]*blades1.value2[0]\
    +blades0.value4[2]*blades1.value1[1]\
    -blades0.value3[9]*blades1.value2[1]\
    +blades0.value4[3]*blades1.value1[2]\
    -blades0.value4[4]*blades1.value3[0]\
    -blades0.value5[0]*blades1.value2[2]\
;\
    blades.value3[8] =\
    +blades0.value0[0]*blades1.value3[8]\
    +blades0.value1[0]*blades1.value4[2]\
    +blades0.value1[1]*blades1.value2[9]\
    -blades0.value2[0]*blades1.value3[7]\
    -blades0.value1[2]*blades1.value4[4]\
    +blades0.value2[1]*blades1.value5[0]\
    +blades0.value2[2]*blades1.value3[9]\
    +blades0.value3[0]*blades1.value4[3]\
    -blades0.value1[3]*blades1.value2[7]\
    +blades0.value2[3]*blades1.value3[4]\
    +blades0.value2[4]*blades1.value1[4]\
    +blades0.value3[1]*blades1.value2[6]\
    -blades0.value2[5]*blades1.value3[6]\
    -blades0.value3[2]*blades1.value4[1]\
    -blades0.value3[3]*blades1.value2[8]\
    +blades0.value4[0]*blades1.value3[5]\
    +blades0.value1[4]*blades1.value2[4]\
    -blades0.value2[6]*blades1.value3[1]\
    -blades0.value2[7]*blades1.value1[3]\
    -blades0.value3[4]*blades1.value2[3]\
    +blades0.value2[8]*blades1.value3[3]\
    +blades0.value3[5]*blades1.value4[0]\
    +blades0.value3[6]*blades1.value2[5]\
    -blades0.value4[1]*blades1.value3[2]\
    +blades0.value2[9]*blades1.value1[1]\
    +blades0.value3[7]*blades1.value2[0]\
    +blades0.value3[8]*blades1.value0[0]\
    -blades0.value4[2]*blades1.value1[0]\
    -blades0.value3[9]*blades1.value2[2]\
    +blades0.value4[3]*blades1.value3[0]\
    +blades0.value4[4]*blades1.value1[2]\
    +blades0.value5[0]*blades1.value2[1]\
;\
    blades.value3[9] =\
    +blades0.value0[0]*blades1.value3[9]\
    +blades0.value1[0]*blades1.value4[3]\
    +blades0.value1[1]*blades1.value4[4]\
    -blades0.value2[0]*blades1.value5[0]\
    +blades0.value1[2]*blades1.value2[9]\
    -blades0.value2[1]*blades1.value3[7]\
    -blades0.value2[2]*blades1.value3[8]\
    -blades0.value3[0]*blades1.value4[2]\
    -blades0.value1[3]*blades1.value2[8]\
    +blades0.value2[3]*blades1.value3[5]\
    +blades0.value2[4]*blades1.value3[6]\
    +blades0.value3[1]*blades1.value4[1]\
    +blades0.value2[5]*blades1.value1[4]\
    +blades0.value3[2]*blades1.value2[6]\
    +blades0.value3[3]*blades1.value2[7]\
    -blades0.value4[0]*blades1.value3[4]\
    +blades0.value1[4]*blades1.value2[5]\
    -blades0.value2[6]*blades1.value3[2]\
    -blades0.value2[7]*blades1.value3[3]\
    -blades0.value3[4]*blades1.value4[0]\
    -blades0.value2[8]*blades1.value1[3]\
    -blades0.value3[5]*blades1.value2[3]\
    -blades0.value3[6]*blades1.value2[4]\
    +blades0.value4[1]*blades1.value3[1]\
    +blades0.value2[9]*blades1.value1[2]\
    +blades0.value3[7]*blades1.value2[1]\
    +blades0.value3[8]*blades1.value2[2]\
    -blades0.value4[2]*blades1.value3[0]\
    +blades0.value3[9]*blades1.value0[0]\
    -blades0.value4[3]*blades1.value1[0]\
    -blades0.value4[4]*blades1.value1[1]\
    -blades0.value5[0]*blades1.value2[0]\
;\
}
#define GEN1_BLADES_GRADE4GEOMETRICPRODUCT(blades,blades0,blades1){\
    blades.value4[0] =\
    +blades0.value0[0]*blades1.value4[0]\
    +blades0.value1[0]*blades1.value3[3]\
    -blades0.value1[1]*blades1.value3[2]\
    +blades0.value2[0]*blades1.value2[5]\
    +blades0.value1[2]*blades1.value3[1]\
    -blades0.value2[1]*blades1.value2[4]\
    +blades0.value2[2]*blades1.value2[3]\
    +blades0.value3[0]*blades1.value1[3]\
    -blades0.value1[3]*blades1.value3[0]\
    +blades0.value2[3]*blades1.value2[2]\
    -blades0.value2[4]*blades1.value2[1]\
    -blades0.value3[1]*blades1.value1[2]\
    +blades0.value2[5]*blades1.value2[0]\
    +blades0.value3[2]*blades1.value1[1]\
    -blades0.value3[3]*blades1.value1[0]\
    +blades0.value4[0]*blades1.value0[0]\
    -blades0.value1[4]*blades1.value5[0]\
    +blades0.value2[6]*blades1.value4[4]\
    -blades0.value2[7]*blades1.value4[3]\
    -blades0.value3[4]*blades1.value3[9]\
    +blades0.value2[8]*blades1.value4[2]\
    +blades0.value3[5]*blades1.value3[8]\
    -blades0.value3[6]*blades1.value3[7]\
    +blades0.value4[1]*blades1.value2[9]\
    -blades0.value2[9]*blades1.value4[1]\
    -blades0.value3[7]*blades1.value3[6]\
    +blades0.value3[8]*blades1.value3[5]\
    -blades0.value4[2]*blades1.value2[8]\
    -blades0.value3[9]*blades1.value3[4]\
    +blades0.value4[3]*blades1.value2[7]\
    -blades0.value4[4]*blades1.value2[6]\
    -blades0.value5[0]*blades1.value1[4]\
;\
    blades.value4[1] =\
    +blades0.value0[0]*blades1.value4[1]\
    +blades0.value1[0]*blades1.value3[6]\
    -blades0.value1[1]*blades1.value3[5]\
    +blades0.value2[0]*blades1.value2[8]\
    +blades0.value1[2]*blades1.value3[4]\
    -blades0.value2[1]*blades1.value2[7]\
    +blades0.value2[2]*blades1.value2[6]\
    +blades0.value3[0]*blades1.value1[4]\
    -blades0.value1[3]*blades1.value5[0]\
    +blades0.value2[3]*blades1.value4[4]\
    -blades0.value2[4]*blades1.value4[3]\
    -blades0.value3[1]*blades1.value3[9]\
    +blades0.value2[5]*blades1.value4[2]\
    +blades0.value3[2]*blades1.value3[8]\
    -blades0.value3[3]*blades1.value3[7]\
    +blades0.value4[0]*blades1.value2[9]\
    -blades0.value1[4]*blades1.value3[0]\
    +blades0.value2[6]*blades1.value2[2]\
    -blades0.value2[7]*blades1.value2[1]\
    -blades0.value3[4]*blades1.value1[2]\
    +blades0.value2[8]*blades1.value2[0]\
    +blades0.value3[5]*blades1.value1[1]\
    -blades0.value3[6]*blades1.value1[0]\
    +blades0.value4[1]*blades1.value0[0]\
    -blades0.value2[9]*blades1.value4[0]\
    -blades0.value3[7]*blades1.value3[3]\
    +blades0.value3[8]*blades1.value3[2]\
    -blades0.value4[2]*blades1.value2[5]\
    -blades0.value3[9]*blades1.value3[1]\
    +blades0.value4[3]*blades1.value2[4]\
    -blades0.value4[4]*blades1.value2[3]\
    -blades0.value5[0]*blades1.value1[3]\
;\
    blades.value4[2] =\
    +blades0.value0[0]*blades1.value4[2]\
    +blades0.value1[0]*blades1.value3[8]\
    -blades0.value1[1]*blades1.value3[7]\
    +blades0.value2[0]*blades1.value2[9]\
    +blades0.value1[2]*blades1.value5[0]\
    -blades0.value2[1]*blades1.value4[4]\
    +blades0.value2[2]*blades1.value4[3]\
    +blades0.value3[0]*blades1.value3[9]\
    +blades0.value1[3]*blades1.value3[4]\
    -blades0.value2[3]*blades1.value2[7]\
    +blades0.value2[4]*blades1.value2[6]\
    +blades0.value3[1]*blades1.value1[4]\
    -blades0.value2[5]*blades1.value4[1]\
    -blades0.value3[2]*blades1.value3[6]\
    +blades0.value3[3]*blades1.value3[5]\
    -blades0.value4[0]*blades1.value2[8]\
    -blades0.value1[4]*blades1.value3[1]\
    +blades0.value2[6]*blades1.value2[4]\
    -blades0.value2[7]*blades1.value2[3]\
    -blades0.value3[4]*blades1.value1[3]\
    +blades0.value2[8]*blades1.value4[0]\
    +blades0.value3[5]*blades1.value3[3]\
    -blades0.value3[6]*blades1.value3[2]\
    +blades0.value4[1]*blades1.value2[5]\
    +blades0.value2[9]*blades1.value2[0]\
    +blades0.value3[7]*blades1.value1[1]\
    -blades0.value3[8]*blades1.value1[0]\
    +blades0.value4[2]*blades1.value0[0]\
    +blades0.value3[9]*blades1.value3[0]\
    -blades0.value4[3]*blades1.value2[2]\
    +blades0.value4[4]*blades1.value2[1]\
    +blades0.value5[0]*blades1.value1[2]\
;\
    blades.value4[3] =\
    +blades0.value0[0]*blades1.value4[3]\
    +blades0.value1[0]*blades1.value3[9]\
    -blades0.value1[1]*blades1.value5[0]\
    +blades0.value2[0]*blades1.value4[4]\
    -blades0.value1[2]*blades1.value3[7]\
    +blades0.value2[1]*blades1.value2[9]\
    -blades0.value2[2]*blades1.value4[2]\
    -blades0.value3[0]*blades1.value3[8]\
    +blades0.value1[3]*blades1.value3[5]\
    -blades0.value2[3]*blades1.value2[8]\
    +blades0.value2[4]*blades1.value4[1]\
    +blades0.value3[1]*blades1.value3[6]\
    +blades0.value2[5]*blades1.value2[6]\
    +blades0.value3[2]*blades1.value1[4]\
    -blades0.value3[3]*blades1.value3[4]\
    +blades0.value4[0]*blades1.value2[7]\
    -blades0.value1[4]*blades1.value3[2]\
    +blades0.value2[6]*blades1.value2[5]\
    -blades0.value2[7]*blades1.value4[0]\
    -blades0.value3[4]*blades1.value3[3]\
    -blades0.value2[8]*blades1.value2[3]\
    -blades0.value3[5]*blades1.value1[3]\
    +blades0.value3[6]*blades1.value3[1]\
    -blades0.value4[1]*blades1.value2[4]\
    +blades0.value2[9]*blades1.value2[1]\
    +blades0.value3[7]*blades1.value1[2]\
    -blades0.value3[8]*blades1.value3[0]\
    +blades0.value4[2]*blades1.value2[2]\
    -blades0.value3[9]*blades1.value1[0]\
    +blades0.value4[3]*blades1.value0[0]\
    -blades0.value4[4]*blades1.value2[0]\
    -blades0.value5[0]*blades1.value1[1]\
;\
    blades.value4[4] =\
    +blades0.value0[0]*blades1.value4[4]\
    +blades0.value1[0]*blades1.value5[0]\
    +blades0.value1[1]*blades1.value3[9]\
    -blades0.value2[0]*blades1.value4[3]\
    -blades0.value1[2]*blades1.value3[8]\
    +blades0.value2[1]*blades1.value4[2]\
    +blades0.value2[2]*blades1.value2[9]\
    +blades0.value3[0]*blades1.value3[7]\
    +blades0.value1[3]*blades1.value3[6]\
    -blades0.value2[3]*blades1.value4[1]\
    -blades0.value2[4]*blades1.value2[8]\
    -blades0.value3[1]*blades1.value3[5]\
    +blades0.value2[5]*blades1.value2[7]\
    +blades0.value3[2]*blades1.value3[4]\
    +blades0.value3[3]*blades1.value1[4]\
    -blades0.value4[0]*blades1.value2[6]\
    -blades0.value1[4]*blades1.value3[3]\
    +blades0.value2[6]*blades1.value4[0]\
    +blades0.value2[7]*blades1.value2[5]\
    +blades0.value3[4]*blades1.value3[2]\
    -blades0.value2[8]*blades1.value2[4]\
    -blades0.value3[5]*blades1.value3[1]\
    -blades0.value3[6]*blades1.value1[3]\
    +blades0.value4[1]*blades1.value2[3]\
    +blades0.value2[9]*blades1.value2[2]\
    +blades0.value3[7]*blades1.value3[0]\
    +blades0.value3[8]*blades1.value1[2]\
    -blades0.value4[2]*blades1.value2[1]\
    -blades0.value3[9]*blades1.value1[1]\
    +blades0.value4[3]*blades1.value2[0]\
    +blades0.value4[4]*blades1.value0[0]\
    +blades0.value5[0]*blades1.value1[0]\
;\
}
#define GEN1_BLADES_GRADE5GEOMETRICPRODUCT(blades,blades0,blades1){\
    blades.value5[0] =\
    +blades0.value0[0]*blades1.value5[0]\
    +blades0.value1[0]*blades1.value4[4]\
    -blades0.value1[1]*blades1.value4[3]\
    +blades0.value2[0]*blades1.value3[9]\
    +blades0.value1[2]*blades1.value4[2]\
    -blades0.value2[1]*blades1.value3[8]\
    +blades0.value2[2]*blades1.value3[7]\
    +blades0.value3[0]*blades1.value2[9]\
    -blades0.value1[3]*blades1.value4[1]\
    +blades0.value2[3]*blades1.value3[6]\
    -blades0.value2[4]*blades1.value3[5]\
    -blades0.value3[1]*blades1.value2[8]\
    +blades0.value2[5]*blades1.value3[4]\
    +blades0.value3[2]*blades1.value2[7]\
    -blades0.value3[3]*blades1.value2[6]\
    +blades0.value4[0]*blades1.value1[4]\
    +blades0.value1[4]*blades1.value4[0]\
    -blades0.value2[6]*blades1.value3[3]\
    +blades0.value2[7]*blades1.value3[2]\
    +blades0.value3[4]*blades1.value2[5]\
    -blades0.value2[8]*blades1.value3[1]\
    -blades0.value3[5]*blades1.value2[4]\
    +blades0.value3[6]*blades1.value2[3]\
    -blades0.value4[1]*blades1.value1[3]\
    +blades0.value2[9]*blades1.value3[0]\
    +blades0.value3[7]*blades1.value2[2]\
    -blades0.value3[8]*blades1.value2[1]\
    +blades0.value4[2]*blades1.value1[2]\
    +blades0.value3[9]*blades1.value2[0]\
    -blades0.value4[3]*blades1.value1[1]\
    +blades0.value4[4]*blades1.value1[0]\
    +blades0.value5[0]*blades1.value0[0]\
;\
}

static gen1_BladesMultivector gen1_blades_gradegeometricproduct(gen1_BladesMultivector blades0, gen1_BladesMultivector blades1, int *grades, Py_ssize_t size){
    gen1_BladesMultivector blades = blades1zero;
    for(Py_ssize_t i = 0; i < size; i++){
        switch(grades[i]){
            case 0:
                GEN1_BLADES_GRADE0GEOMETRICPRODUCT(blades,blades0,blades1);
                break;
            case 1:
                GEN1_BLADES_GRADE1GEOMETRICPRODUCT(blades,blades0,blades1);
                break;
            case 2:
                GEN1_BLADES_GRADE2GEOMETRICPRODUCT(blades,blades0,blades1);
                break;
            case 3:
                GEN1_BLADES_GRADE3GEOMETRICPRODUCT(blades,blades0,blades1);
                break;
            case 4:
                GEN1_BLADES_GRADE4GEOMETRICPRODUCT(blades,blades0,blades1);
                break;
            case 5:
                GEN1_BLADES_GRADE5GEOMETRICPRODUCT(blades,blades0,blades1);
                break;
            default:// do nothing for different values
                break;
        }
    }
    return blades;
}


static gen1_BladesMultivector gen1_blades_innerproduct(gen1_BladesMultivector blades0, gen1_BladesMultivector blades1){
    gen1_BladesMultivector blades = {{0},{0},{0},{0},{0},{0},};

    blades.value0[0] =
    +blades0.value1[0]*blades1.value1[0]
    +blades0.value1[1]*blades1.value1[1]
    -blades0.value2[0]*blades1.value2[0]
    +blades0.value1[2]*blades1.value1[2]
    -blades0.value2[1]*blades1.value2[1]
    -blades0.value2[2]*blades1.value2[2]
    -blades0.value3[0]*blades1.value3[0]
    +blades0.value1[3]*blades1.value1[3]
    -blades0.value2[3]*blades1.value2[3]
    -blades0.value2[4]*blades1.value2[4]
    -blades0.value3[1]*blades1.value3[1]
    -blades0.value2[5]*blades1.value2[5]
    -blades0.value3[2]*blades1.value3[2]
    -blades0.value3[3]*blades1.value3[3]
    +blades0.value4[0]*blades1.value4[0]
    -blades0.value1[4]*blades1.value1[4]
    +blades0.value2[6]*blades1.value2[6]
    +blades0.value2[7]*blades1.value2[7]
    +blades0.value3[4]*blades1.value3[4]
    +blades0.value2[8]*blades1.value2[8]
    +blades0.value3[5]*blades1.value3[5]
    +blades0.value3[6]*blades1.value3[6]
    -blades0.value4[1]*blades1.value4[1]
    +blades0.value2[9]*blades1.value2[9]
    +blades0.value3[7]*blades1.value3[7]
    +blades0.value3[8]*blades1.value3[8]
    -blades0.value4[2]*blades1.value4[2]
    +blades0.value3[9]*blades1.value3[9]
    -blades0.value4[3]*blades1.value4[3]
    -blades0.value4[4]*blades1.value4[4]
    -blades0.value5[0]*blades1.value5[0]
;
    blades.value1[0] =
    -blades0.value1[1]*blades1.value2[0]
    +blades0.value2[0]*blades1.value1[1]
    -blades0.value1[2]*blades1.value2[1]
    +blades0.value2[1]*blades1.value1[2]
    -blades0.value2[2]*blades1.value3[0]
    -blades0.value3[0]*blades1.value2[2]
    -blades0.value1[3]*blades1.value2[3]
    +blades0.value2[3]*blades1.value1[3]
    -blades0.value2[4]*blades1.value3[1]
    -blades0.value3[1]*blades1.value2[4]
    -blades0.value2[5]*blades1.value3[2]
    -blades0.value3[2]*blades1.value2[5]
    +blades0.value3[3]*blades1.value4[0]
    -blades0.value4[0]*blades1.value3[3]
    +blades0.value1[4]*blades1.value2[6]
    -blades0.value2[6]*blades1.value1[4]
    +blades0.value2[7]*blades1.value3[4]
    +blades0.value3[4]*blades1.value2[7]
    +blades0.value2[8]*blades1.value3[5]
    +blades0.value3[5]*blades1.value2[8]
    -blades0.value3[6]*blades1.value4[1]
    +blades0.value4[1]*blades1.value3[6]
    +blades0.value2[9]*blades1.value3[7]
    +blades0.value3[7]*blades1.value2[9]
    -blades0.value3[8]*blades1.value4[2]
    +blades0.value4[2]*blades1.value3[8]
    -blades0.value3[9]*blades1.value4[3]
    +blades0.value4[3]*blades1.value3[9]
    -blades0.value4[4]*blades1.value5[0]
    -blades0.value5[0]*blades1.value4[4]
;
    blades.value1[1] =
    +blades0.value1[0]*blades1.value2[0]
    -blades0.value2[0]*blades1.value1[0]
    -blades0.value1[2]*blades1.value2[2]
    +blades0.value2[1]*blades1.value3[0]
    +blades0.value2[2]*blades1.value1[2]
    +blades0.value3[0]*blades1.value2[1]
    -blades0.value1[3]*blades1.value2[4]
    +blades0.value2[3]*blades1.value3[1]
    +blades0.value2[4]*blades1.value1[3]
    +blades0.value3[1]*blades1.value2[3]
    -blades0.value2[5]*blades1.value3[3]
    -blades0.value3[2]*blades1.value4[0]
    -blades0.value3[3]*blades1.value2[5]
    +blades0.value4[0]*blades1.value3[2]
    +blades0.value1[4]*blades1.value2[7]
    -blades0.value2[6]*blades1.value3[4]
    -blades0.value2[7]*blades1.value1[4]
    -blades0.value3[4]*blades1.value2[6]
    +blades0.value2[8]*blades1.value3[6]
    +blades0.value3[5]*blades1.value4[1]
    +blades0.value3[6]*blades1.value2[8]
    -blades0.value4[1]*blades1.value3[5]
    +blades0.value2[9]*blades1.value3[8]
    +blades0.value3[7]*blades1.value4[2]
    +blades0.value3[8]*blades1.value2[9]
    -blades0.value4[2]*blades1.value3[7]
    -blades0.value3[9]*blades1.value4[4]
    +blades0.value4[3]*blades1.value5[0]
    +blades0.value4[4]*blades1.value3[9]
    +blades0.value5[0]*blades1.value4[3]
;
    blades.value2[0] =
    +blades0.value1[2]*blades1.value3[0]
    +blades0.value3[0]*blades1.value1[2]
    +blades0.value1[3]*blades1.value3[1]
    +blades0.value3[1]*blades1.value1[3]
    -blades0.value2[5]*blades1.value4[0]
    -blades0.value4[0]*blades1.value2[5]
    -blades0.value1[4]*blades1.value3[4]
    -blades0.value3[4]*blades1.value1[4]
    +blades0.value2[8]*blades1.value4[1]
    +blades0.value4[1]*blades1.value2[8]
    +blades0.value2[9]*blades1.value4[2]
    +blades0.value4[2]*blades1.value2[9]
    +blades0.value3[9]*blades1.value5[0]
    +blades0.value5[0]*blades1.value3[9]
;
    blades.value1[2] =
    +blades0.value1[0]*blades1.value2[1]
    +blades0.value1[1]*blades1.value2[2]
    -blades0.value2[0]*blades1.value3[0]
    -blades0.value2[1]*blades1.value1[0]
    -blades0.value2[2]*blades1.value1[1]
    -blades0.value3[0]*blades1.value2[0]
    -blades0.value1[3]*blades1.value2[5]
    +blades0.value2[3]*blades1.value3[2]
    +blades0.value2[4]*blades1.value3[3]
    +blades0.value3[1]*blades1.value4[0]
    +blades0.value2[5]*blades1.value1[3]
    +blades0.value3[2]*blades1.value2[3]
    +blades0.value3[3]*blades1.value2[4]
    -blades0.value4[0]*blades1.value3[1]
    +blades0.value1[4]*blades1.value2[8]
    -blades0.value2[6]*blades1.value3[5]
    -blades0.value2[7]*blades1.value3[6]
    -blades0.value3[4]*blades1.value4[1]
    -blades0.value2[8]*blades1.value1[4]
    -blades0.value3[5]*blades1.value2[6]
    -blades0.value3[6]*blades1.value2[7]
    +blades0.value4[1]*blades1.value3[4]
    +blades0.value2[9]*blades1.value3[9]
    +blades0.value3[7]*blades1.value4[3]
    +blades0.value3[8]*blades1.value4[4]
    -blades0.value4[2]*blades1.value5[0]
    +blades0.value3[9]*blades1.value2[9]
    -blades0.value4[3]*blades1.value3[7]
    -blades0.value4[4]*blades1.value3[8]
    -blades0.value5[0]*blades1.value4[2]
;
    blades.value2[1] =
    -blades0.value1[1]*blades1.value3[0]
    -blades0.value3[0]*blades1.value1[1]
    +blades0.value1[3]*blades1.value3[2]
    +blades0.value2[4]*blades1.value4[0]
    +blades0.value3[2]*blades1.value1[3]
    +blades0.value4[0]*blades1.value2[4]
    -blades0.value1[4]*blades1.value3[5]
    -blades0.value2[7]*blades1.value4[1]
    -blades0.value3[5]*blades1.value1[4]
    -blades0.value4[1]*blades1.value2[7]
    +blades0.value2[9]*blades1.value4[3]
    -blades0.value3[8]*blades1.value5[0]
    +blades0.value4[3]*blades1.value2[9]
    -blades0.value5[0]*blades1.value3[8]
;
    blades.value2[2] =
    +blades0.value1[0]*blades1.value3[0]
    +blades0.value3[0]*blades1.value1[0]
    +blades0.value1[3]*blades1.value3[3]
    -blades0.value2[3]*blades1.value4[0]
    +blades0.value3[3]*blades1.value1[3]
    -blades0.value4[0]*blades1.value2[3]
    -blades0.value1[4]*blades1.value3[6]
    +blades0.value2[6]*blades1.value4[1]
    -blades0.value3[6]*blades1.value1[4]
    +blades0.value4[1]*blades1.value2[6]
    +blades0.value2[9]*blades1.value4[4]
    +blades0.value3[7]*blades1.value5[0]
    +blades0.value4[4]*blades1.value2[9]
    +blades0.value5[0]*blades1.value3[7]
;
    blades.value3[0] =
    -blades0.value1[3]*blades1.value4[0]
    +blades0.value4[0]*blades1.value1[3]
    +blades0.value1[4]*blades1.value4[1]
    -blades0.value4[1]*blades1.value1[4]
    +blades0.value2[9]*blades1.value5[0]
    +blades0.value5[0]*blades1.value2[9]
;
    blades.value1[3] =
    +blades0.value1[0]*blades1.value2[3]
    +blades0.value1[1]*blades1.value2[4]
    -blades0.value2[0]*blades1.value3[1]
    +blades0.value1[2]*blades1.value2[5]
    -blades0.value2[1]*blades1.value3[2]
    -blades0.value2[2]*blades1.value3[3]
    -blades0.value3[0]*blades1.value4[0]
    -blades0.value2[3]*blades1.value1[0]
    -blades0.value2[4]*blades1.value1[1]
    -blades0.value3[1]*blades1.value2[0]
    -blades0.value2[5]*blades1.value1[2]
    -blades0.value3[2]*blades1.value2[1]
    -blades0.value3[3]*blades1.value2[2]
    +blades0.value4[0]*blades1.value3[0]
    +blades0.value1[4]*blades1.value2[9]
    -blades0.value2[6]*blades1.value3[7]
    -blades0.value2[7]*blades1.value3[8]
    -blades0.value3[4]*blades1.value4[2]
    -blades0.value2[8]*blades1.value3[9]
    -blades0.value3[5]*blades1.value4[3]
    -blades0.value3[6]*blades1.value4[4]
    +blades0.value4[1]*blades1.value5[0]
    -blades0.value2[9]*blades1.value1[4]
    -blades0.value3[7]*blades1.value2[6]
    -blades0.value3[8]*blades1.value2[7]
    +blades0.value4[2]*blades1.value3[4]
    -blades0.value3[9]*blades1.value2[8]
    +blades0.value4[3]*blades1.value3[5]
    +blades0.value4[4]*blades1.value3[6]
    +blades0.value5[0]*blades1.value4[1]
;
    blades.value2[3] =
    -blades0.value1[1]*blades1.value3[1]
    -blades0.value1[2]*blades1.value3[2]
    -blades0.value2[2]*blades1.value4[0]
    -blades0.value3[1]*blades1.value1[1]
    -blades0.value3[2]*blades1.value1[2]
    -blades0.value4[0]*blades1.value2[2]
    -blades0.value1[4]*blades1.value3[7]
    -blades0.value2[7]*blades1.value4[2]
    -blades0.value2[8]*blades1.value4[3]
    +blades0.value3[6]*blades1.value5[0]
    -blades0.value3[7]*blades1.value1[4]
    -blades0.value4[2]*blades1.value2[7]
    -blades0.value4[3]*blades1.value2[8]
    +blades0.value5[0]*blades1.value3[6]
;
    blades.value2[4] =
    +blades0.value1[0]*blades1.value3[1]
    -blades0.value1[2]*blades1.value3[3]
    +blades0.value2[1]*blades1.value4[0]
    +blades0.value3[1]*blades1.value1[0]
    -blades0.value3[3]*blades1.value1[2]
    +blades0.value4[0]*blades1.value2[1]
    -blades0.value1[4]*blades1.value3[8]
    +blades0.value2[6]*blades1.value4[2]
    -blades0.value2[8]*blades1.value4[4]
    -blades0.value3[5]*blades1.value5[0]
    -blades0.value3[8]*blades1.value1[4]
    +blades0.value4[2]*blades1.value2[6]
    -blades0.value4[4]*blades1.value2[8]
    -blades0.value5[0]*blades1.value3[5]
;
    blades.value3[1] =
    +blades0.value1[2]*blades1.value4[0]
    -blades0.value4[0]*blades1.value1[2]
    +blades0.value1[4]*blades1.value4[2]
    -blades0.value2[8]*blades1.value5[0]
    -blades0.value4[2]*blades1.value1[4]
    -blades0.value5[0]*blades1.value2[8]
;
    blades.value2[5] =
    +blades0.value1[0]*blades1.value3[2]
    +blades0.value1[1]*blades1.value3[3]
    -blades0.value2[0]*blades1.value4[0]
    +blades0.value3[2]*blades1.value1[0]
    +blades0.value3[3]*blades1.value1[1]
    -blades0.value4[0]*blades1.value2[0]
    -blades0.value1[4]*blades1.value3[9]
    +blades0.value2[6]*blades1.value4[3]
    +blades0.value2[7]*blades1.value4[4]
    +blades0.value3[4]*blades1.value5[0]
    -blades0.value3[9]*blades1.value1[4]
    +blades0.value4[3]*blades1.value2[6]
    +blades0.value4[4]*blades1.value2[7]
    +blades0.value5[0]*blades1.value3[4]
;
    blades.value3[2] =
    -blades0.value1[1]*blades1.value4[0]
    +blades0.value4[0]*blades1.value1[1]
    +blades0.value1[4]*blades1.value4[3]
    +blades0.value2[7]*blades1.value5[0]
    -blades0.value4[3]*blades1.value1[4]
    +blades0.value5[0]*blades1.value2[7]
;
    blades.value3[3] =
    +blades0.value1[0]*blades1.value4[0]
    -blades0.value4[0]*blades1.value1[0]
    +blades0.value1[4]*blades1.value4[4]
    -blades0.value2[6]*blades1.value5[0]
    -blades0.value4[4]*blades1.value1[4]
    -blades0.value5[0]*blades1.value2[6]
;
    blades.value4[0] =
    -blades0.value1[4]*blades1.value5[0]
    -blades0.value5[0]*blades1.value1[4]
;
    blades.value1[4] =
    +blades0.value1[0]*blades1.value2[6]
    +blades0.value1[1]*blades1.value2[7]
    -blades0.value2[0]*blades1.value3[4]
    +blades0.value1[2]*blades1.value2[8]
    -blades0.value2[1]*blades1.value3[5]
    -blades0.value2[2]*blades1.value3[6]
    -blades0.value3[0]*blades1.value4[1]
    +blades0.value1[3]*blades1.value2[9]
    -blades0.value2[3]*blades1.value3[7]
    -blades0.value2[4]*blades1.value3[8]
    -blades0.value3[1]*blades1.value4[2]
    -blades0.value2[5]*blades1.value3[9]
    -blades0.value3[2]*blades1.value4[3]
    -blades0.value3[3]*blades1.value4[4]
    +blades0.value4[0]*blades1.value5[0]
    -blades0.value2[6]*blades1.value1[0]
    -blades0.value2[7]*blades1.value1[1]
    -blades0.value3[4]*blades1.value2[0]
    -blades0.value2[8]*blades1.value1[2]
    -blades0.value3[5]*blades1.value2[1]
    -blades0.value3[6]*blades1.value2[2]
    +blades0.value4[1]*blades1.value3[0]
    -blades0.value2[9]*blades1.value1[3]
    -blades0.value3[7]*blades1.value2[3]
    -blades0.value3[8]*blades1.value2[4]
    +blades0.value4[2]*blades1.value3[1]
    -blades0.value3[9]*blades1.value2[5]
    +blades0.value4[3]*blades1.value3[2]
    +blades0.value4[4]*blades1.value3[3]
    +blades0.value5[0]*blades1.value4[0]
;
    blades.value2[6] =
    -blades0.value1[1]*blades1.value3[4]
    -blades0.value1[2]*blades1.value3[5]
    -blades0.value2[2]*blades1.value4[1]
    -blades0.value1[3]*blades1.value3[7]
    -blades0.value2[4]*blades1.value4[2]
    -blades0.value2[5]*blades1.value4[3]
    +blades0.value3[3]*blades1.value5[0]
    -blades0.value3[4]*blades1.value1[1]
    -blades0.value3[5]*blades1.value1[2]
    -blades0.value4[1]*blades1.value2[2]
    -blades0.value3[7]*blades1.value1[3]
    -blades0.value4[2]*blades1.value2[4]
    -blades0.value4[3]*blades1.value2[5]
    +blades0.value5[0]*blades1.value3[3]
;
    blades.value2[7] =
    +blades0.value1[0]*blades1.value3[4]
    -blades0.value1[2]*blades1.value3[6]
    +blades0.value2[1]*blades1.value4[1]
    -blades0.value1[3]*blades1.value3[8]
    +blades0.value2[3]*blades1.value4[2]
    -blades0.value2[5]*blades1.value4[4]
    -blades0.value3[2]*blades1.value5[0]
    +blades0.value3[4]*blades1.value1[0]
    -blades0.value3[6]*blades1.value1[2]
    +blades0.value4[1]*blades1.value2[1]
    -blades0.value3[8]*blades1.value1[3]
    +blades0.value4[2]*blades1.value2[3]
    -blades0.value4[4]*blades1.value2[5]
    -blades0.value5[0]*blades1.value3[2]
;
    blades.value3[4] =
    +blades0.value1[2]*blades1.value4[1]
    +blades0.value1[3]*blades1.value4[2]
    -blades0.value2[5]*blades1.value5[0]
    -blades0.value4[1]*blades1.value1[2]
    -blades0.value4[2]*blades1.value1[3]
    -blades0.value5[0]*blades1.value2[5]
;
    blades.value2[8] =
    +blades0.value1[0]*blades1.value3[5]
    +blades0.value1[1]*blades1.value3[6]
    -blades0.value2[0]*blades1.value4[1]
    -blades0.value1[3]*blades1.value3[9]
    +blades0.value2[3]*blades1.value4[3]
    +blades0.value2[4]*blades1.value4[4]
    +blades0.value3[1]*blades1.value5[0]
    +blades0.value3[5]*blades1.value1[0]
    +blades0.value3[6]*blades1.value1[1]
    -blades0.value4[1]*blades1.value2[0]
    -blades0.value3[9]*blades1.value1[3]
    +blades0.value4[3]*blades1.value2[3]
    +blades0.value4[4]*blades1.value2[4]
    +blades0.value5[0]*blades1.value3[1]
;
    blades.value3[5] =
    -blades0.value1[1]*blades1.value4[1]
    +blades0.value1[3]*blades1.value4[3]
    +blades0.value2[4]*blades1.value5[0]
    +blades0.value4[1]*blades1.value1[1]
    -blades0.value4[3]*blades1.value1[3]
    +blades0.value5[0]*blades1.value2[4]
;
    blades.value3[6] =
    +blades0.value1[0]*blades1.value4[1]
    +blades0.value1[3]*blades1.value4[4]
    -blades0.value2[3]*blades1.value5[0]
    -blades0.value4[1]*blades1.value1[0]
    -blades0.value4[4]*blades1.value1[3]
    -blades0.value5[0]*blades1.value2[3]
;
    blades.value4[1] =
    -blades0.value1[3]*blades1.value5[0]
    -blades0.value5[0]*blades1.value1[3]
;
    blades.value2[9] =
    +blades0.value1[0]*blades1.value3[7]
    +blades0.value1[1]*blades1.value3[8]
    -blades0.value2[0]*blades1.value4[2]
    +blades0.value1[2]*blades1.value3[9]
    -blades0.value2[1]*blades1.value4[3]
    -blades0.value2[2]*blades1.value4[4]
    -blades0.value3[0]*blades1.value5[0]
    +blades0.value3[7]*blades1.value1[0]
    +blades0.value3[8]*blades1.value1[1]
    -blades0.value4[2]*blades1.value2[0]
    +blades0.value3[9]*blades1.value1[2]
    -blades0.value4[3]*blades1.value2[1]
    -blades0.value4[4]*blades1.value2[2]
    -blades0.value5[0]*blades1.value3[0]
;
    blades.value3[7] =
    -blades0.value1[1]*blades1.value4[2]
    -blades0.value1[2]*blades1.value4[3]
    -blades0.value2[2]*blades1.value5[0]
    +blades0.value4[2]*blades1.value1[1]
    +blades0.value4[3]*blades1.value1[2]
    -blades0.value5[0]*blades1.value2[2]
;
    blades.value3[8] =
    +blades0.value1[0]*blades1.value4[2]
    -blades0.value1[2]*blades1.value4[4]
    +blades0.value2[1]*blades1.value5[0]
    -blades0.value4[2]*blades1.value1[0]
    +blades0.value4[4]*blades1.value1[2]
    +blades0.value5[0]*blades1.value2[1]
;
    blades.value4[2] =
    +blades0.value1[2]*blades1.value5[0]
    +blades0.value5[0]*blades1.value1[2]
;
    blades.value3[9] =
    +blades0.value1[0]*blades1.value4[3]
    +blades0.value1[1]*blades1.value4[4]
    -blades0.value2[0]*blades1.value5[0]
    -blades0.value4[3]*blades1.value1[0]
    -blades0.value4[4]*blades1.value1[1]
    -blades0.value5[0]*blades1.value2[0]
;
    blades.value4[3] =
    -blades0.value1[1]*blades1.value5[0]
    -blades0.value5[0]*blades1.value1[1]
;
    blades.value4[4] =
    +blades0.value1[0]*blades1.value5[0]
    +blades0.value5[0]*blades1.value1[0]
;
    return blades;
}

#define GEN1_BLADES_GRADE0INNERPRODUCT(blades,blades0,blades1){\
    blades.value0[0] =\
    +blades0.value1[0]*blades1.value1[0]\
    +blades0.value1[1]*blades1.value1[1]\
    -blades0.value2[0]*blades1.value2[0]\
    +blades0.value1[2]*blades1.value1[2]\
    -blades0.value2[1]*blades1.value2[1]\
    -blades0.value2[2]*blades1.value2[2]\
    -blades0.value3[0]*blades1.value3[0]\
    +blades0.value1[3]*blades1.value1[3]\
    -blades0.value2[3]*blades1.value2[3]\
    -blades0.value2[4]*blades1.value2[4]\
    -blades0.value3[1]*blades1.value3[1]\
    -blades0.value2[5]*blades1.value2[5]\
    -blades0.value3[2]*blades1.value3[2]\
    -blades0.value3[3]*blades1.value3[3]\
    +blades0.value4[0]*blades1.value4[0]\
    -blades0.value1[4]*blades1.value1[4]\
    +blades0.value2[6]*blades1.value2[6]\
    +blades0.value2[7]*blades1.value2[7]\
    +blades0.value3[4]*blades1.value3[4]\
    +blades0.value2[8]*blades1.value2[8]\
    +blades0.value3[5]*blades1.value3[5]\
    +blades0.value3[6]*blades1.value3[6]\
    -blades0.value4[1]*blades1.value4[1]\
    +blades0.value2[9]*blades1.value2[9]\
    +blades0.value3[7]*blades1.value3[7]\
    +blades0.value3[8]*blades1.value3[8]\
    -blades0.value4[2]*blades1.value4[2]\
    +blades0.value3[9]*blades1.value3[9]\
    -blades0.value4[3]*blades1.value4[3]\
    -blades0.value4[4]*blades1.value4[4]\
    -blades0.value5[0]*blades1.value5[0]\
;\
}
#define GEN1_BLADES_GRADE1INNERPRODUCT(blades,blades0,blades1){\
    blades.value1[0] =\
    -blades0.value1[1]*blades1.value2[0]\
    +blades0.value2[0]*blades1.value1[1]\
    -blades0.value1[2]*blades1.value2[1]\
    +blades0.value2[1]*blades1.value1[2]\
    -blades0.value2[2]*blades1.value3[0]\
    -blades0.value3[0]*blades1.value2[2]\
    -blades0.value1[3]*blades1.value2[3]\
    +blades0.value2[3]*blades1.value1[3]\
    -blades0.value2[4]*blades1.value3[1]\
    -blades0.value3[1]*blades1.value2[4]\
    -blades0.value2[5]*blades1.value3[2]\
    -blades0.value3[2]*blades1.value2[5]\
    +blades0.value3[3]*blades1.value4[0]\
    -blades0.value4[0]*blades1.value3[3]\
    +blades0.value1[4]*blades1.value2[6]\
    -blades0.value2[6]*blades1.value1[4]\
    +blades0.value2[7]*blades1.value3[4]\
    +blades0.value3[4]*blades1.value2[7]\
    +blades0.value2[8]*blades1.value3[5]\
    +blades0.value3[5]*blades1.value2[8]\
    -blades0.value3[6]*blades1.value4[1]\
    +blades0.value4[1]*blades1.value3[6]\
    +blades0.value2[9]*blades1.value3[7]\
    +blades0.value3[7]*blades1.value2[9]\
    -blades0.value3[8]*blades1.value4[2]\
    +blades0.value4[2]*blades1.value3[8]\
    -blades0.value3[9]*blades1.value4[3]\
    +blades0.value4[3]*blades1.value3[9]\
    -blades0.value4[4]*blades1.value5[0]\
    -blades0.value5[0]*blades1.value4[4]\
;\
    blades.value1[1] =\
    +blades0.value1[0]*blades1.value2[0]\
    -blades0.value2[0]*blades1.value1[0]\
    -blades0.value1[2]*blades1.value2[2]\
    +blades0.value2[1]*blades1.value3[0]\
    +blades0.value2[2]*blades1.value1[2]\
    +blades0.value3[0]*blades1.value2[1]\
    -blades0.value1[3]*blades1.value2[4]\
    +blades0.value2[3]*blades1.value3[1]\
    +blades0.value2[4]*blades1.value1[3]\
    +blades0.value3[1]*blades1.value2[3]\
    -blades0.value2[5]*blades1.value3[3]\
    -blades0.value3[2]*blades1.value4[0]\
    -blades0.value3[3]*blades1.value2[5]\
    +blades0.value4[0]*blades1.value3[2]\
    +blades0.value1[4]*blades1.value2[7]\
    -blades0.value2[6]*blades1.value3[4]\
    -blades0.value2[7]*blades1.value1[4]\
    -blades0.value3[4]*blades1.value2[6]\
    +blades0.value2[8]*blades1.value3[6]\
    +blades0.value3[5]*blades1.value4[1]\
    +blades0.value3[6]*blades1.value2[8]\
    -blades0.value4[1]*blades1.value3[5]\
    +blades0.value2[9]*blades1.value3[8]\
    +blades0.value3[7]*blades1.value4[2]\
    +blades0.value3[8]*blades1.value2[9]\
    -blades0.value4[2]*blades1.value3[7]\
    -blades0.value3[9]*blades1.value4[4]\
    +blades0.value4[3]*blades1.value5[0]\
    +blades0.value4[4]*blades1.value3[9]\
    +blades0.value5[0]*blades1.value4[3]\
;\
    blades.value1[2] =\
    +blades0.value1[0]*blades1.value2[1]\
    +blades0.value1[1]*blades1.value2[2]\
    -blades0.value2[0]*blades1.value3[0]\
    -blades0.value2[1]*blades1.value1[0]\
    -blades0.value2[2]*blades1.value1[1]\
    -blades0.value3[0]*blades1.value2[0]\
    -blades0.value1[3]*blades1.value2[5]\
    +blades0.value2[3]*blades1.value3[2]\
    +blades0.value2[4]*blades1.value3[3]\
    +blades0.value3[1]*blades1.value4[0]\
    +blades0.value2[5]*blades1.value1[3]\
    +blades0.value3[2]*blades1.value2[3]\
    +blades0.value3[3]*blades1.value2[4]\
    -blades0.value4[0]*blades1.value3[1]\
    +blades0.value1[4]*blades1.value2[8]\
    -blades0.value2[6]*blades1.value3[5]\
    -blades0.value2[7]*blades1.value3[6]\
    -blades0.value3[4]*blades1.value4[1]\
    -blades0.value2[8]*blades1.value1[4]\
    -blades0.value3[5]*blades1.value2[6]\
    -blades0.value3[6]*blades1.value2[7]\
    +blades0.value4[1]*blades1.value3[4]\
    +blades0.value2[9]*blades1.value3[9]\
    +blades0.value3[7]*blades1.value4[3]\
    +blades0.value3[8]*blades1.value4[4]\
    -blades0.value4[2]*blades1.value5[0]\
    +blades0.value3[9]*blades1.value2[9]\
    -blades0.value4[3]*blades1.value3[7]\
    -blades0.value4[4]*blades1.value3[8]\
    -blades0.value5[0]*blades1.value4[2]\
;\
    blades.value1[3] =\
    +blades0.value1[0]*blades1.value2[3]\
    +blades0.value1[1]*blades1.value2[4]\
    -blades0.value2[0]*blades1.value3[1]\
    +blades0.value1[2]*blades1.value2[5]\
    -blades0.value2[1]*blades1.value3[2]\
    -blades0.value2[2]*blades1.value3[3]\
    -blades0.value3[0]*blades1.value4[0]\
    -blades0.value2[3]*blades1.value1[0]\
    -blades0.value2[4]*blades1.value1[1]\
    -blades0.value3[1]*blades1.value2[0]\
    -blades0.value2[5]*blades1.value1[2]\
    -blades0.value3[2]*blades1.value2[1]\
    -blades0.value3[3]*blades1.value2[2]\
    +blades0.value4[0]*blades1.value3[0]\
    +blades0.value1[4]*blades1.value2[9]\
    -blades0.value2[6]*blades1.value3[7]\
    -blades0.value2[7]*blades1.value3[8]\
    -blades0.value3[4]*blades1.value4[2]\
    -blades0.value2[8]*blades1.value3[9]\
    -blades0.value3[5]*blades1.value4[3]\
    -blades0.value3[6]*blades1.value4[4]\
    +blades0.value4[1]*blades1.value5[0]\
    -blades0.value2[9]*blades1.value1[4]\
    -blades0.value3[7]*blades1.value2[6]\
    -blades0.value3[8]*blades1.value2[7]\
    +blades0.value4[2]*blades1.value3[4]\
    -blades0.value3[9]*blades1.value2[8]\
    +blades0.value4[3]*blades1.value3[5]\
    +blades0.value4[4]*blades1.value3[6]\
    +blades0.value5[0]*blades1.value4[1]\
;\
    blades.value1[4] =\
    +blades0.value1[0]*blades1.value2[6]\
    +blades0.value1[1]*blades1.value2[7]\
    -blades0.value2[0]*blades1.value3[4]\
    +blades0.value1[2]*blades1.value2[8]\
    -blades0.value2[1]*blades1.value3[5]\
    -blades0.value2[2]*blades1.value3[6]\
    -blades0.value3[0]*blades1.value4[1]\
    +blades0.value1[3]*blades1.value2[9]\
    -blades0.value2[3]*blades1.value3[7]\
    -blades0.value2[4]*blades1.value3[8]\
    -blades0.value3[1]*blades1.value4[2]\
    -blades0.value2[5]*blades1.value3[9]\
    -blades0.value3[2]*blades1.value4[3]\
    -blades0.value3[3]*blades1.value4[4]\
    +blades0.value4[0]*blades1.value5[0]\
    -blades0.value2[6]*blades1.value1[0]\
    -blades0.value2[7]*blades1.value1[1]\
    -blades0.value3[4]*blades1.value2[0]\
    -blades0.value2[8]*blades1.value1[2]\
    -blades0.value3[5]*blades1.value2[1]\
    -blades0.value3[6]*blades1.value2[2]\
    +blades0.value4[1]*blades1.value3[0]\
    -blades0.value2[9]*blades1.value1[3]\
    -blades0.value3[7]*blades1.value2[3]\
    -blades0.value3[8]*blades1.value2[4]\
    +blades0.value4[2]*blades1.value3[1]\
    -blades0.value3[9]*blades1.value2[5]\
    +blades0.value4[3]*blades1.value3[2]\
    +blades0.value4[4]*blades1.value3[3]\
    +blades0.value5[0]*blades1.value4[0]\
;\
}
#define GEN1_BLADES_GRADE2INNERPRODUCT(blades,blades0,blades1){\
    blades.value2[0] =\
    +blades0.value1[2]*blades1.value3[0]\
    +blades0.value3[0]*blades1.value1[2]\
    +blades0.value1[3]*blades1.value3[1]\
    +blades0.value3[1]*blades1.value1[3]\
    -blades0.value2[5]*blades1.value4[0]\
    -blades0.value4[0]*blades1.value2[5]\
    -blades0.value1[4]*blades1.value3[4]\
    -blades0.value3[4]*blades1.value1[4]\
    +blades0.value2[8]*blades1.value4[1]\
    +blades0.value4[1]*blades1.value2[8]\
    +blades0.value2[9]*blades1.value4[2]\
    +blades0.value4[2]*blades1.value2[9]\
    +blades0.value3[9]*blades1.value5[0]\
    +blades0.value5[0]*blades1.value3[9]\
;\
    blades.value2[1] =\
    -blades0.value1[1]*blades1.value3[0]\
    -blades0.value3[0]*blades1.value1[1]\
    +blades0.value1[3]*blades1.value3[2]\
    +blades0.value2[4]*blades1.value4[0]\
    +blades0.value3[2]*blades1.value1[3]\
    +blades0.value4[0]*blades1.value2[4]\
    -blades0.value1[4]*blades1.value3[5]\
    -blades0.value2[7]*blades1.value4[1]\
    -blades0.value3[5]*blades1.value1[4]\
    -blades0.value4[1]*blades1.value2[7]\
    +blades0.value2[9]*blades1.value4[3]\
    -blades0.value3[8]*blades1.value5[0]\
    +blades0.value4[3]*blades1.value2[9]\
    -blades0.value5[0]*blades1.value3[8]\
;\
    blades.value2[2] =\
    +blades0.value1[0]*blades1.value3[0]\
    +blades0.value3[0]*blades1.value1[0]\
    +blades0.value1[3]*blades1.value3[3]\
    -blades0.value2[3]*blades1.value4[0]\
    +blades0.value3[3]*blades1.value1[3]\
    -blades0.value4[0]*blades1.value2[3]\
    -blades0.value1[4]*blades1.value3[6]\
    +blades0.value2[6]*blades1.value4[1]\
    -blades0.value3[6]*blades1.value1[4]\
    +blades0.value4[1]*blades1.value2[6]\
    +blades0.value2[9]*blades1.value4[4]\
    +blades0.value3[7]*blades1.value5[0]\
    +blades0.value4[4]*blades1.value2[9]\
    +blades0.value5[0]*blades1.value3[7]\
;\
    blades.value2[3] =\
    -blades0.value1[1]*blades1.value3[1]\
    -blades0.value1[2]*blades1.value3[2]\
    -blades0.value2[2]*blades1.value4[0]\
    -blades0.value3[1]*blades1.value1[1]\
    -blades0.value3[2]*blades1.value1[2]\
    -blades0.value4[0]*blades1.value2[2]\
    -blades0.value1[4]*blades1.value3[7]\
    -blades0.value2[7]*blades1.value4[2]\
    -blades0.value2[8]*blades1.value4[3]\
    +blades0.value3[6]*blades1.value5[0]\
    -blades0.value3[7]*blades1.value1[4]\
    -blades0.value4[2]*blades1.value2[7]\
    -blades0.value4[3]*blades1.value2[8]\
    +blades0.value5[0]*blades1.value3[6]\
;\
    blades.value2[4] =\
    +blades0.value1[0]*blades1.value3[1]\
    -blades0.value1[2]*blades1.value3[3]\
    +blades0.value2[1]*blades1.value4[0]\
    +blades0.value3[1]*blades1.value1[0]\
    -blades0.value3[3]*blades1.value1[2]\
    +blades0.value4[0]*blades1.value2[1]\
    -blades0.value1[4]*blades1.value3[8]\
    +blades0.value2[6]*blades1.value4[2]\
    -blades0.value2[8]*blades1.value4[4]\
    -blades0.value3[5]*blades1.value5[0]\
    -blades0.value3[8]*blades1.value1[4]\
    +blades0.value4[2]*blades1.value2[6]\
    -blades0.value4[4]*blades1.value2[8]\
    -blades0.value5[0]*blades1.value3[5]\
;\
    blades.value2[5] =\
    +blades0.value1[0]*blades1.value3[2]\
    +blades0.value1[1]*blades1.value3[3]\
    -blades0.value2[0]*blades1.value4[0]\
    +blades0.value3[2]*blades1.value1[0]\
    +blades0.value3[3]*blades1.value1[1]\
    -blades0.value4[0]*blades1.value2[0]\
    -blades0.value1[4]*blades1.value3[9]\
    +blades0.value2[6]*blades1.value4[3]\
    +blades0.value2[7]*blades1.value4[4]\
    +blades0.value3[4]*blades1.value5[0]\
    -blades0.value3[9]*blades1.value1[4]\
    +blades0.value4[3]*blades1.value2[6]\
    +blades0.value4[4]*blades1.value2[7]\
    +blades0.value5[0]*blades1.value3[4]\
;\
    blades.value2[6] =\
    -blades0.value1[1]*blades1.value3[4]\
    -blades0.value1[2]*blades1.value3[5]\
    -blades0.value2[2]*blades1.value4[1]\
    -blades0.value1[3]*blades1.value3[7]\
    -blades0.value2[4]*blades1.value4[2]\
    -blades0.value2[5]*blades1.value4[3]\
    +blades0.value3[3]*blades1.value5[0]\
    -blades0.value3[4]*blades1.value1[1]\
    -blades0.value3[5]*blades1.value1[2]\
    -blades0.value4[1]*blades1.value2[2]\
    -blades0.value3[7]*blades1.value1[3]\
    -blades0.value4[2]*blades1.value2[4]\
    -blades0.value4[3]*blades1.value2[5]\
    +blades0.value5[0]*blades1.value3[3]\
;\
    blades.value2[7] =\
    +blades0.value1[0]*blades1.value3[4]\
    -blades0.value1[2]*blades1.value3[6]\
    +blades0.value2[1]*blades1.value4[1]\
    -blades0.value1[3]*blades1.value3[8]\
    +blades0.value2[3]*blades1.value4[2]\
    -blades0.value2[5]*blades1.value4[4]\
    -blades0.value3[2]*blades1.value5[0]\
    +blades0.value3[4]*blades1.value1[0]\
    -blades0.value3[6]*blades1.value1[2]\
    +blades0.value4[1]*blades1.value2[1]\
    -blades0.value3[8]*blades1.value1[3]\
    +blades0.value4[2]*blades1.value2[3]\
    -blades0.value4[4]*blades1.value2[5]\
    -blades0.value5[0]*blades1.value3[2]\
;\
    blades.value2[8] =\
    +blades0.value1[0]*blades1.value3[5]\
    +blades0.value1[1]*blades1.value3[6]\
    -blades0.value2[0]*blades1.value4[1]\
    -blades0.value1[3]*blades1.value3[9]\
    +blades0.value2[3]*blades1.value4[3]\
    +blades0.value2[4]*blades1.value4[4]\
    +blades0.value3[1]*blades1.value5[0]\
    +blades0.value3[5]*blades1.value1[0]\
    +blades0.value3[6]*blades1.value1[1]\
    -blades0.value4[1]*blades1.value2[0]\
    -blades0.value3[9]*blades1.value1[3]\
    +blades0.value4[3]*blades1.value2[3]\
    +blades0.value4[4]*blades1.value2[4]\
    +blades0.value5[0]*blades1.value3[1]\
;\
    blades.value2[9] =\
    +blades0.value1[0]*blades1.value3[7]\
    +blades0.value1[1]*blades1.value3[8]\
    -blades0.value2[0]*blades1.value4[2]\
    +blades0.value1[2]*blades1.value3[9]\
    -blades0.value2[1]*blades1.value4[3]\
    -blades0.value2[2]*blades1.value4[4]\
    -blades0.value3[0]*blades1.value5[0]\
    +blades0.value3[7]*blades1.value1[0]\
    +blades0.value3[8]*blades1.value1[1]\
    -blades0.value4[2]*blades1.value2[0]\
    +blades0.value3[9]*blades1.value1[2]\
    -blades0.value4[3]*blades1.value2[1]\
    -blades0.value4[4]*blades1.value2[2]\
    -blades0.value5[0]*blades1.value3[0]\
;\
}
#define GEN1_BLADES_GRADE3INNERPRODUCT(blades,blades0,blades1){\
    blades.value3[0] =\
    -blades0.value1[3]*blades1.value4[0]\
    +blades0.value4[0]*blades1.value1[3]\
    +blades0.value1[4]*blades1.value4[1]\
    -blades0.value4[1]*blades1.value1[4]\
    +blades0.value2[9]*blades1.value5[0]\
    +blades0.value5[0]*blades1.value2[9]\
;\
    blades.value3[1] =\
    +blades0.value1[2]*blades1.value4[0]\
    -blades0.value4[0]*blades1.value1[2]\
    +blades0.value1[4]*blades1.value4[2]\
    -blades0.value2[8]*blades1.value5[0]\
    -blades0.value4[2]*blades1.value1[4]\
    -blades0.value5[0]*blades1.value2[8]\
;\
    blades.value3[2] =\
    -blades0.value1[1]*blades1.value4[0]\
    +blades0.value4[0]*blades1.value1[1]\
    +blades0.value1[4]*blades1.value4[3]\
    +blades0.value2[7]*blades1.value5[0]\
    -blades0.value4[3]*blades1.value1[4]\
    +blades0.value5[0]*blades1.value2[7]\
;\
    blades.value3[3] =\
    +blades0.value1[0]*blades1.value4[0]\
    -blades0.value4[0]*blades1.value1[0]\
    +blades0.value1[4]*blades1.value4[4]\
    -blades0.value2[6]*blades1.value5[0]\
    -blades0.value4[4]*blades1.value1[4]\
    -blades0.value5[0]*blades1.value2[6]\
;\
    blades.value3[4] =\
    +blades0.value1[2]*blades1.value4[1]\
    +blades0.value1[3]*blades1.value4[2]\
    -blades0.value2[5]*blades1.value5[0]\
    -blades0.value4[1]*blades1.value1[2]\
    -blades0.value4[2]*blades1.value1[3]\
    -blades0.value5[0]*blades1.value2[5]\
;\
    blades.value3[5] =\
    -blades0.value1[1]*blades1.value4[1]\
    +blades0.value1[3]*blades1.value4[3]\
    +blades0.value2[4]*blades1.value5[0]\
    +blades0.value4[1]*blades1.value1[1]\
    -blades0.value4[3]*blades1.value1[3]\
    +blades0.value5[0]*blades1.value2[4]\
;\
    blades.value3[6] =\
    +blades0.value1[0]*blades1.value4[1]\
    +blades0.value1[3]*blades1.value4[4]\
    -blades0.value2[3]*blades1.value5[0]\
    -blades0.value4[1]*blades1.value1[0]\
    -blades0.value4[4]*blades1.value1[3]\
    -blades0.value5[0]*blades1.value2[3]\
;\
    blades.value3[7] =\
    -blades0.value1[1]*blades1.value4[2]\
    -blades0.value1[2]*blades1.value4[3]\
    -blades0.value2[2]*blades1.value5[0]\
    +blades0.value4[2]*blades1.value1[1]\
    +blades0.value4[3]*blades1.value1[2]\
    -blades0.value5[0]*blades1.value2[2]\
;\
    blades.value3[8] =\
    +blades0.value1[0]*blades1.value4[2]\
    -blades0.value1[2]*blades1.value4[4]\
    +blades0.value2[1]*blades1.value5[0]\
    -blades0.value4[2]*blades1.value1[0]\
    +blades0.value4[4]*blades1.value1[2]\
    +blades0.value5[0]*blades1.value2[1]\
;\
    blades.value3[9] =\
    +blades0.value1[0]*blades1.value4[3]\
    +blades0.value1[1]*blades1.value4[4]\
    -blades0.value2[0]*blades1.value5[0]\
    -blades0.value4[3]*blades1.value1[0]\
    -blades0.value4[4]*blades1.value1[1]\
    -blades0.value5[0]*blades1.value2[0]\
;\
}
#define GEN1_BLADES_GRADE4INNERPRODUCT(blades,blades0,blades1){\
    blades.value4[0] =\
    -blades0.value1[4]*blades1.value5[0]\
    -blades0.value5[0]*blades1.value1[4]\
;\
    blades.value4[1] =\
    -blades0.value1[3]*blades1.value5[0]\
    -blades0.value5[0]*blades1.value1[3]\
;\
    blades.value4[2] =\
    +blades0.value1[2]*blades1.value5[0]\
    +blades0.value5[0]*blades1.value1[2]\
;\
    blades.value4[3] =\
    -blades0.value1[1]*blades1.value5[0]\
    -blades0.value5[0]*blades1.value1[1]\
;\
    blades.value4[4] =\
    +blades0.value1[0]*blades1.value5[0]\
    +blades0.value5[0]*blades1.value1[0]\
;\
}
#define GEN1_BLADES_GRADE5INNERPRODUCT(blades,blades0,blades1){\
}

static gen1_BladesMultivector gen1_blades_gradeinnerproduct(gen1_BladesMultivector blades0, gen1_BladesMultivector blades1, int *grades, Py_ssize_t size){
    gen1_BladesMultivector blades = blades1zero;
    for(Py_ssize_t i = 0; i < size; i++){
        switch(grades[i]){
            case 0:
                GEN1_BLADES_GRADE0INNERPRODUCT(blades,blades0,blades1);
                break;
            case 1:
                GEN1_BLADES_GRADE1INNERPRODUCT(blades,blades0,blades1);
                break;
            case 2:
                GEN1_BLADES_GRADE2INNERPRODUCT(blades,blades0,blades1);
                break;
            case 3:
                GEN1_BLADES_GRADE3INNERPRODUCT(blades,blades0,blades1);
                break;
            case 4:
                GEN1_BLADES_GRADE4INNERPRODUCT(blades,blades0,blades1);
                break;
            case 5:
                GEN1_BLADES_GRADE5INNERPRODUCT(blades,blades0,blades1);
                break;
            default:// do nothing for different values
                break;
        }
    }
    return blades;
}


static gen1_BladesMultivector gen1_blades_outerproduct(gen1_BladesMultivector blades0, gen1_BladesMultivector blades1){
    gen1_BladesMultivector blades = {{0},{0},{0},{0},{0},{0},};

    blades.value0[0] =
    +blades0.value0[0]*blades1.value0[0]
;
    blades.value1[0] =
    +blades0.value0[0]*blades1.value1[0]
    +blades0.value1[0]*blades1.value0[0]
;
    blades.value1[1] =
    +blades0.value0[0]*blades1.value1[1]
    +blades0.value1[1]*blades1.value0[0]
;
    blades.value2[0] =
    +blades0.value0[0]*blades1.value2[0]
    +blades0.value1[0]*blades1.value1[1]
    -blades0.value1[1]*blades1.value1[0]
    +blades0.value2[0]*blades1.value0[0]
;
    blades.value1[2] =
    +blades0.value0[0]*blades1.value1[2]
    +blades0.value1[2]*blades1.value0[0]
;
    blades.value2[1] =
    +blades0.value0[0]*blades1.value2[1]
    +blades0.value1[0]*blades1.value1[2]
    -blades0.value1[2]*blades1.value1[0]
    +blades0.value2[1]*blades1.value0[0]
;
    blades.value2[2] =
    +blades0.value0[0]*blades1.value2[2]
    +blades0.value1[1]*blades1.value1[2]
    -blades0.value1[2]*blades1.value1[1]
    +blades0.value2[2]*blades1.value0[0]
;
    blades.value3[0] =
    +blades0.value0[0]*blades1.value3[0]
    +blades0.value1[0]*blades1.value2[2]
    -blades0.value1[1]*blades1.value2[1]
    +blades0.value2[0]*blades1.value1[2]
    +blades0.value1[2]*blades1.value2[0]
    -blades0.value2[1]*blades1.value1[1]
    +blades0.value2[2]*blades1.value1[0]
    +blades0.value3[0]*blades1.value0[0]
;
    blades.value1[3] =
    +blades0.value0[0]*blades1.value1[3]
    +blades0.value1[3]*blades1.value0[0]
;
    blades.value2[3] =
    +blades0.value0[0]*blades1.value2[3]
    +blades0.value1[0]*blades1.value1[3]
    -blades0.value1[3]*blades1.value1[0]
    +blades0.value2[3]*blades1.value0[0]
;
    blades.value2[4] =
    +blades0.value0[0]*blades1.value2[4]
    +blades0.value1[1]*blades1.value1[3]
    -blades0.value1[3]*blades1.value1[1]
    +blades0.value2[4]*blades1.value0[0]
;
    blades.value3[1] =
    +blades0.value0[0]*blades1.value3[1]
    +blades0.value1[0]*blades1.value2[4]
    -blades0.value1[1]*blades1.value2[3]
    +blades0.value2[0]*blades1.value1[3]
    +blades0.value1[3]*blades1.value2[0]
    -blades0.value2[3]*blades1.value1[1]
    +blades0.value2[4]*blades1.value1[0]
    +blades0.value3[1]*blades1.value0[0]
;
    blades.value2[5] =
    +blades0.value0[0]*blades1.value2[5]
    +blades0.value1[2]*blades1.value1[3]
    -blades0.value1[3]*blades1.value1[2]
    +blades0.value2[5]*blades1.value0[0]
;
    blades.value3[2] =
    +blades0.value0[0]*blades1.value3[2]
    +blades0.value1[0]*blades1.value2[5]
    -blades0.value1[2]*blades1.value2[3]
    +blades0.value2[1]*blades1.value1[3]
    +blades0.value1[3]*blades1.value2[1]
    -blades0.value2[3]*blades1.value1[2]
    +blades0.value2[5]*blades1.value1[0]
    +blades0.value3[2]*blades1.value0[0]
;
    blades.value3[3] =
    +blades0.value0[0]*blades1.value3[3]
    +blades0.value1[1]*blades1.value2[5]
    -blades0.value1[2]*blades1.value2[4]
    +blades0.value2[2]*blades1.value1[3]
    +blades0.value1[3]*blades1.value2[2]
    -blades0.value2[4]*blades1.value1[2]
    +blades0.value2[5]*blades1.value1[1]
    +blades0.value3[3]*blades1.value0[0]
;
    blades.value4[0] =
    +blades0.value0[0]*blades1.value4[0]
    +blades0.value1[0]*blades1.value3[3]
    -blades0.value1[1]*blades1.value3[2]
    +blades0.value2[0]*blades1.value2[5]
    +blades0.value1[2]*blades1.value3[1]
    -blades0.value2[1]*blades1.value2[4]
    +blades0.value2[2]*blades1.value2[3]
    +blades0.value3[0]*blades1.value1[3]
    -blades0.value1[3]*blades1.value3[0]
    +blades0.value2[3]*blades1.value2[2]
    -blades0.value2[4]*blades1.value2[1]
    -blades0.value3[1]*blades1.value1[2]
    +blades0.value2[5]*blades1.value2[0]
    +blades0.value3[2]*blades1.value1[1]
    -blades0.value3[3]*blades1.value1[0]
    +blades0.value4[0]*blades1.value0[0]
;
    blades.value1[4] =
    +blades0.value0[0]*blades1.value1[4]
    +blades0.value1[4]*blades1.value0[0]
;
    blades.value2[6] =
    +blades0.value0[0]*blades1.value2[6]
    +blades0.value1[0]*blades1.value1[4]
    -blades0.value1[4]*blades1.value1[0]
    +blades0.value2[6]*blades1.value0[0]
;
    blades.value2[7] =
    +blades0.value0[0]*blades1.value2[7]
    +blades0.value1[1]*blades1.value1[4]
    -blades0.value1[4]*blades1.value1[1]
    +blades0.value2[7]*blades1.value0[0]
;
    blades.value3[4] =
    +blades0.value0[0]*blades1.value3[4]
    +blades0.value1[0]*blades1.value2[7]
    -blades0.value1[1]*blades1.value2[6]
    +blades0.value2[0]*blades1.value1[4]
    +blades0.value1[4]*blades1.value2[0]
    -blades0.value2[6]*blades1.value1[1]
    +blades0.value2[7]*blades1.value1[0]
    +blades0.value3[4]*blades1.value0[0]
;
    blades.value2[8] =
    +blades0.value0[0]*blades1.value2[8]
    +blades0.value1[2]*blades1.value1[4]
    -blades0.value1[4]*blades1.value1[2]
    +blades0.value2[8]*blades1.value0[0]
;
    blades.value3[5] =
    +blades0.value0[0]*blades1.value3[5]
    +blades0.value1[0]*blades1.value2[8]
    -blades0.value1[2]*blades1.value2[6]
    +blades0.value2[1]*blades1.value1[4]
    +blades0.value1[4]*blades1.value2[1]
    -blades0.value2[6]*blades1.value1[2]
    +blades0.value2[8]*blades1.value1[0]
    +blades0.value3[5]*blades1.value0[0]
;
    blades.value3[6] =
    +blades0.value0[0]*blades1.value3[6]
    +blades0.value1[1]*blades1.value2[8]
    -blades0.value1[2]*blades1.value2[7]
    +blades0.value2[2]*blades1.value1[4]
    +blades0.value1[4]*blades1.value2[2]
    -blades0.value2[7]*blades1.value1[2]
    +blades0.value2[8]*blades1.value1[1]
    +blades0.value3[6]*blades1.value0[0]
;
    blades.value4[1] =
    +blades0.value0[0]*blades1.value4[1]
    +blades0.value1[0]*blades1.value3[6]
    -blades0.value1[1]*blades1.value3[5]
    +blades0.value2[0]*blades1.value2[8]
    +blades0.value1[2]*blades1.value3[4]
    -blades0.value2[1]*blades1.value2[7]
    +blades0.value2[2]*blades1.value2[6]
    +blades0.value3[0]*blades1.value1[4]
    -blades0.value1[4]*blades1.value3[0]
    +blades0.value2[6]*blades1.value2[2]
    -blades0.value2[7]*blades1.value2[1]
    -blades0.value3[4]*blades1.value1[2]
    +blades0.value2[8]*blades1.value2[0]
    +blades0.value3[5]*blades1.value1[1]
    -blades0.value3[6]*blades1.value1[0]
    +blades0.value4[1]*blades1.value0[0]
;
    blades.value2[9] =
    +blades0.value0[0]*blades1.value2[9]
    +blades0.value1[3]*blades1.value1[4]
    -blades0.value1[4]*blades1.value1[3]
    +blades0.value2[9]*blades1.value0[0]
;
    blades.value3[7] =
    +blades0.value0[0]*blades1.value3[7]
    +blades0.value1[0]*blades1.value2[9]
    -blades0.value1[3]*blades1.value2[6]
    +blades0.value2[3]*blades1.value1[4]
    +blades0.value1[4]*blades1.value2[3]
    -blades0.value2[6]*blades1.value1[3]
    +blades0.value2[9]*blades1.value1[0]
    +blades0.value3[7]*blades1.value0[0]
;
    blades.value3[8] =
    +blades0.value0[0]*blades1.value3[8]
    +blades0.value1[1]*blades1.value2[9]
    -blades0.value1[3]*blades1.value2[7]
    +blades0.value2[4]*blades1.value1[4]
    +blades0.value1[4]*blades1.value2[4]
    -blades0.value2[7]*blades1.value1[3]
    +blades0.value2[9]*blades1.value1[1]
    +blades0.value3[8]*blades1.value0[0]
;
    blades.value4[2] =
    +blades0.value0[0]*blades1.value4[2]
    +blades0.value1[0]*blades1.value3[8]
    -blades0.value1[1]*blades1.value3[7]
    +blades0.value2[0]*blades1.value2[9]
    +blades0.value1[3]*blades1.value3[4]
    -blades0.value2[3]*blades1.value2[7]
    +blades0.value2[4]*blades1.value2[6]
    +blades0.value3[1]*blades1.value1[4]
    -blades0.value1[4]*blades1.value3[1]
    +blades0.value2[6]*blades1.value2[4]
    -blades0.value2[7]*blades1.value2[3]
    -blades0.value3[4]*blades1.value1[3]
    +blades0.value2[9]*blades1.value2[0]
    +blades0.value3[7]*blades1.value1[1]
    -blades0.value3[8]*blades1.value1[0]
    +blades0.value4[2]*blades1.value0[0]
;
    blades.value3[9] =
    +blades0.value0[0]*blades1.value3[9]
    +blades0.value1[2]*blades1.value2[9]
    -blades0.value1[3]*blades1.value2[8]
    +blades0.value2[5]*blades1.value1[4]
    +blades0.value1[4]*blades1.value2[5]
    -blades0.value2[8]*blades1.value1[3]
    +blades0.value2[9]*blades1.value1[2]
    +blades0.value3[9]*blades1.value0[0]
;
    blades.value4[3] =
    +blades0.value0[0]*blades1.value4[3]
    +blades0.value1[0]*blades1.value3[9]
    -blades0.value1[2]*blades1.value3[7]
    +blades0.value2[1]*blades1.value2[9]
    +blades0.value1[3]*blades1.value3[5]
    -blades0.value2[3]*blades1.value2[8]
    +blades0.value2[5]*blades1.value2[6]
    +blades0.value3[2]*blades1.value1[4]
    -blades0.value1[4]*blades1.value3[2]
    +blades0.value2[6]*blades1.value2[5]
    -blades0.value2[8]*blades1.value2[3]
    -blades0.value3[5]*blades1.value1[3]
    +blades0.value2[9]*blades1.value2[1]
    +blades0.value3[7]*blades1.value1[2]
    -blades0.value3[9]*blades1.value1[0]
    +blades0.value4[3]*blades1.value0[0]
;
    blades.value4[4] =
    +blades0.value0[0]*blades1.value4[4]
    +blades0.value1[1]*blades1.value3[9]
    -blades0.value1[2]*blades1.value3[8]
    +blades0.value2[2]*blades1.value2[9]
    +blades0.value1[3]*blades1.value3[6]
    -blades0.value2[4]*blades1.value2[8]
    +blades0.value2[5]*blades1.value2[7]
    +blades0.value3[3]*blades1.value1[4]
    -blades0.value1[4]*blades1.value3[3]
    +blades0.value2[7]*blades1.value2[5]
    -blades0.value2[8]*blades1.value2[4]
    -blades0.value3[6]*blades1.value1[3]
    +blades0.value2[9]*blades1.value2[2]
    +blades0.value3[8]*blades1.value1[2]
    -blades0.value3[9]*blades1.value1[1]
    +blades0.value4[4]*blades1.value0[0]
;
    blades.value5[0] =
    +blades0.value0[0]*blades1.value5[0]
    +blades0.value1[0]*blades1.value4[4]
    -blades0.value1[1]*blades1.value4[3]
    +blades0.value2[0]*blades1.value3[9]
    +blades0.value1[2]*blades1.value4[2]
    -blades0.value2[1]*blades1.value3[8]
    +blades0.value2[2]*blades1.value3[7]
    +blades0.value3[0]*blades1.value2[9]
    -blades0.value1[3]*blades1.value4[1]
    +blades0.value2[3]*blades1.value3[6]
    -blades0.value2[4]*blades1.value3[5]
    -blades0.value3[1]*blades1.value2[8]
    +blades0.value2[5]*blades1.value3[4]
    +blades0.value3[2]*blades1.value2[7]
    -blades0.value3[3]*blades1.value2[6]
    +blades0.value4[0]*blades1.value1[4]
    +blades0.value1[4]*blades1.value4[0]
    -blades0.value2[6]*blades1.value3[3]
    +blades0.value2[7]*blades1.value3[2]
    +blades0.value3[4]*blades1.value2[5]
    -blades0.value2[8]*blades1.value3[1]
    -blades0.value3[5]*blades1.value2[4]
    +blades0.value3[6]*blades1.value2[3]
    -blades0.value4[1]*blades1.value1[3]
    +blades0.value2[9]*blades1.value3[0]
    +blades0.value3[7]*blades1.value2[2]
    -blades0.value3[8]*blades1.value2[1]
    +blades0.value4[2]*blades1.value1[2]
    +blades0.value3[9]*blades1.value2[0]
    -blades0.value4[3]*blades1.value1[1]
    +blades0.value4[4]*blades1.value1[0]
    +blades0.value5[0]*blades1.value0[0]
;
    return blades;
}

#define GEN1_BLADES_GRADE0OUTERPRODUCT(blades,blades0,blades1){\
    blades.value0[0] =\
    +blades0.value0[0]*blades1.value0[0]\
;\
}
#define GEN1_BLADES_GRADE1OUTERPRODUCT(blades,blades0,blades1){\
    blades.value1[0] =\
    +blades0.value0[0]*blades1.value1[0]\
    +blades0.value1[0]*blades1.value0[0]\
;\
    blades.value1[1] =\
    +blades0.value0[0]*blades1.value1[1]\
    +blades0.value1[1]*blades1.value0[0]\
;\
    blades.value1[2] =\
    +blades0.value0[0]*blades1.value1[2]\
    +blades0.value1[2]*blades1.value0[0]\
;\
    blades.value1[3] =\
    +blades0.value0[0]*blades1.value1[3]\
    +blades0.value1[3]*blades1.value0[0]\
;\
    blades.value1[4] =\
    +blades0.value0[0]*blades1.value1[4]\
    +blades0.value1[4]*blades1.value0[0]\
;\
}
#define GEN1_BLADES_GRADE2OUTERPRODUCT(blades,blades0,blades1){\
    blades.value2[0] =\
    +blades0.value0[0]*blades1.value2[0]\
    +blades0.value1[0]*blades1.value1[1]\
    -blades0.value1[1]*blades1.value1[0]\
    +blades0.value2[0]*blades1.value0[0]\
;\
    blades.value2[1] =\
    +blades0.value0[0]*blades1.value2[1]\
    +blades0.value1[0]*blades1.value1[2]\
    -blades0.value1[2]*blades1.value1[0]\
    +blades0.value2[1]*blades1.value0[0]\
;\
    blades.value2[2] =\
    +blades0.value0[0]*blades1.value2[2]\
    +blades0.value1[1]*blades1.value1[2]\
    -blades0.value1[2]*blades1.value1[1]\
    +blades0.value2[2]*blades1.value0[0]\
;\
    blades.value2[3] =\
    +blades0.value0[0]*blades1.value2[3]\
    +blades0.value1[0]*blades1.value1[3]\
    -blades0.value1[3]*blades1.value1[0]\
    +blades0.value2[3]*blades1.value0[0]\
;\
    blades.value2[4] =\
    +blades0.value0[0]*blades1.value2[4]\
    +blades0.value1[1]*blades1.value1[3]\
    -blades0.value1[3]*blades1.value1[1]\
    +blades0.value2[4]*blades1.value0[0]\
;\
    blades.value2[5] =\
    +blades0.value0[0]*blades1.value2[5]\
    +blades0.value1[2]*blades1.value1[3]\
    -blades0.value1[3]*blades1.value1[2]\
    +blades0.value2[5]*blades1.value0[0]\
;\
    blades.value2[6] =\
    +blades0.value0[0]*blades1.value2[6]\
    +blades0.value1[0]*blades1.value1[4]\
    -blades0.value1[4]*blades1.value1[0]\
    +blades0.value2[6]*blades1.value0[0]\
;\
    blades.value2[7] =\
    +blades0.value0[0]*blades1.value2[7]\
    +blades0.value1[1]*blades1.value1[4]\
    -blades0.value1[4]*blades1.value1[1]\
    +blades0.value2[7]*blades1.value0[0]\
;\
    blades.value2[8] =\
    +blades0.value0[0]*blades1.value2[8]\
    +blades0.value1[2]*blades1.value1[4]\
    -blades0.value1[4]*blades1.value1[2]\
    +blades0.value2[8]*blades1.value0[0]\
;\
    blades.value2[9] =\
    +blades0.value0[0]*blades1.value2[9]\
    +blades0.value1[3]*blades1.value1[4]\
    -blades0.value1[4]*blades1.value1[3]\
    +blades0.value2[9]*blades1.value0[0]\
;\
}
#define GEN1_BLADES_GRADE3OUTERPRODUCT(blades,blades0,blades1){\
    blades.value3[0] =\
    +blades0.value0[0]*blades1.value3[0]\
    +blades0.value1[0]*blades1.value2[2]\
    -blades0.value1[1]*blades1.value2[1]\
    +blades0.value2[0]*blades1.value1[2]\
    +blades0.value1[2]*blades1.value2[0]\
    -blades0.value2[1]*blades1.value1[1]\
    +blades0.value2[2]*blades1.value1[0]\
    +blades0.value3[0]*blades1.value0[0]\
;\
    blades.value3[1] =\
    +blades0.value0[0]*blades1.value3[1]\
    +blades0.value1[0]*blades1.value2[4]\
    -blades0.value1[1]*blades1.value2[3]\
    +blades0.value2[0]*blades1.value1[3]\
    +blades0.value1[3]*blades1.value2[0]\
    -blades0.value2[3]*blades1.value1[1]\
    +blades0.value2[4]*blades1.value1[0]\
    +blades0.value3[1]*blades1.value0[0]\
;\
    blades.value3[2] =\
    +blades0.value0[0]*blades1.value3[2]\
    +blades0.value1[0]*blades1.value2[5]\
    -blades0.value1[2]*blades1.value2[3]\
    +blades0.value2[1]*blades1.value1[3]\
    +blades0.value1[3]*blades1.value2[1]\
    -blades0.value2[3]*blades1.value1[2]\
    +blades0.value2[5]*blades1.value1[0]\
    +blades0.value3[2]*blades1.value0[0]\
;\
    blades.value3[3] =\
    +blades0.value0[0]*blades1.value3[3]\
    +blades0.value1[1]*blades1.value2[5]\
    -blades0.value1[2]*blades1.value2[4]\
    +blades0.value2[2]*blades1.value1[3]\
    +blades0.value1[3]*blades1.value2[2]\
    -blades0.value2[4]*blades1.value1[2]\
    +blades0.value2[5]*blades1.value1[1]\
    +blades0.value3[3]*blades1.value0[0]\
;\
    blades.value3[4] =\
    +blades0.value0[0]*blades1.value3[4]\
    +blades0.value1[0]*blades1.value2[7]\
    -blades0.value1[1]*blades1.value2[6]\
    +blades0.value2[0]*blades1.value1[4]\
    +blades0.value1[4]*blades1.value2[0]\
    -blades0.value2[6]*blades1.value1[1]\
    +blades0.value2[7]*blades1.value1[0]\
    +blades0.value3[4]*blades1.value0[0]\
;\
    blades.value3[5] =\
    +blades0.value0[0]*blades1.value3[5]\
    +blades0.value1[0]*blades1.value2[8]\
    -blades0.value1[2]*blades1.value2[6]\
    +blades0.value2[1]*blades1.value1[4]\
    +blades0.value1[4]*blades1.value2[1]\
    -blades0.value2[6]*blades1.value1[2]\
    +blades0.value2[8]*blades1.value1[0]\
    +blades0.value3[5]*blades1.value0[0]\
;\
    blades.value3[6] =\
    +blades0.value0[0]*blades1.value3[6]\
    +blades0.value1[1]*blades1.value2[8]\
    -blades0.value1[2]*blades1.value2[7]\
    +blades0.value2[2]*blades1.value1[4]\
    +blades0.value1[4]*blades1.value2[2]\
    -blades0.value2[7]*blades1.value1[2]\
    +blades0.value2[8]*blades1.value1[1]\
    +blades0.value3[6]*blades1.value0[0]\
;\
    blades.value3[7] =\
    +blades0.value0[0]*blades1.value3[7]\
    +blades0.value1[0]*blades1.value2[9]\
    -blades0.value1[3]*blades1.value2[6]\
    +blades0.value2[3]*blades1.value1[4]\
    +blades0.value1[4]*blades1.value2[3]\
    -blades0.value2[6]*blades1.value1[3]\
    +blades0.value2[9]*blades1.value1[0]\
    +blades0.value3[7]*blades1.value0[0]\
;\
    blades.value3[8] =\
    +blades0.value0[0]*blades1.value3[8]\
    +blades0.value1[1]*blades1.value2[9]\
    -blades0.value1[3]*blades1.value2[7]\
    +blades0.value2[4]*blades1.value1[4]\
    +blades0.value1[4]*blades1.value2[4]\
    -blades0.value2[7]*blades1.value1[3]\
    +blades0.value2[9]*blades1.value1[1]\
    +blades0.value3[8]*blades1.value0[0]\
;\
    blades.value3[9] =\
    +blades0.value0[0]*blades1.value3[9]\
    +blades0.value1[2]*blades1.value2[9]\
    -blades0.value1[3]*blades1.value2[8]\
    +blades0.value2[5]*blades1.value1[4]\
    +blades0.value1[4]*blades1.value2[5]\
    -blades0.value2[8]*blades1.value1[3]\
    +blades0.value2[9]*blades1.value1[2]\
    +blades0.value3[9]*blades1.value0[0]\
;\
}
#define GEN1_BLADES_GRADE4OUTERPRODUCT(blades,blades0,blades1){\
    blades.value4[0] =\
    +blades0.value0[0]*blades1.value4[0]\
    +blades0.value1[0]*blades1.value3[3]\
    -blades0.value1[1]*blades1.value3[2]\
    +blades0.value2[0]*blades1.value2[5]\
    +blades0.value1[2]*blades1.value3[1]\
    -blades0.value2[1]*blades1.value2[4]\
    +blades0.value2[2]*blades1.value2[3]\
    +blades0.value3[0]*blades1.value1[3]\
    -blades0.value1[3]*blades1.value3[0]\
    +blades0.value2[3]*blades1.value2[2]\
    -blades0.value2[4]*blades1.value2[1]\
    -blades0.value3[1]*blades1.value1[2]\
    +blades0.value2[5]*blades1.value2[0]\
    +blades0.value3[2]*blades1.value1[1]\
    -blades0.value3[3]*blades1.value1[0]\
    +blades0.value4[0]*blades1.value0[0]\
;\
    blades.value4[1] =\
    +blades0.value0[0]*blades1.value4[1]\
    +blades0.value1[0]*blades1.value3[6]\
    -blades0.value1[1]*blades1.value3[5]\
    +blades0.value2[0]*blades1.value2[8]\
    +blades0.value1[2]*blades1.value3[4]\
    -blades0.value2[1]*blades1.value2[7]\
    +blades0.value2[2]*blades1.value2[6]\
    +blades0.value3[0]*blades1.value1[4]\
    -blades0.value1[4]*blades1.value3[0]\
    +blades0.value2[6]*blades1.value2[2]\
    -blades0.value2[7]*blades1.value2[1]\
    -blades0.value3[4]*blades1.value1[2]\
    +blades0.value2[8]*blades1.value2[0]\
    +blades0.value3[5]*blades1.value1[1]\
    -blades0.value3[6]*blades1.value1[0]\
    +blades0.value4[1]*blades1.value0[0]\
;\
    blades.value4[2] =\
    +blades0.value0[0]*blades1.value4[2]\
    +blades0.value1[0]*blades1.value3[8]\
    -blades0.value1[1]*blades1.value3[7]\
    +blades0.value2[0]*blades1.value2[9]\
    +blades0.value1[3]*blades1.value3[4]\
    -blades0.value2[3]*blades1.value2[7]\
    +blades0.value2[4]*blades1.value2[6]\
    +blades0.value3[1]*blades1.value1[4]\
    -blades0.value1[4]*blades1.value3[1]\
    +blades0.value2[6]*blades1.value2[4]\
    -blades0.value2[7]*blades1.value2[3]\
    -blades0.value3[4]*blades1.value1[3]\
    +blades0.value2[9]*blades1.value2[0]\
    +blades0.value3[7]*blades1.value1[1]\
    -blades0.value3[8]*blades1.value1[0]\
    +blades0.value4[2]*blades1.value0[0]\
;\
    blades.value4[3] =\
    +blades0.value0[0]*blades1.value4[3]\
    +blades0.value1[0]*blades1.value3[9]\
    -blades0.value1[2]*blades1.value3[7]\
    +blades0.value2[1]*blades1.value2[9]\
    +blades0.value1[3]*blades1.value3[5]\
    -blades0.value2[3]*blades1.value2[8]\
    +blades0.value2[5]*blades1.value2[6]\
    +blades0.value3[2]*blades1.value1[4]\
    -blades0.value1[4]*blades1.value3[2]\
    +blades0.value2[6]*blades1.value2[5]\
    -blades0.value2[8]*blades1.value2[3]\
    -blades0.value3[5]*blades1.value1[3]\
    +blades0.value2[9]*blades1.value2[1]\
    +blades0.value3[7]*blades1.value1[2]\
    -blades0.value3[9]*blades1.value1[0]\
    +blades0.value4[3]*blades1.value0[0]\
;\
    blades.value4[4] =\
    +blades0.value0[0]*blades1.value4[4]\
    +blades0.value1[1]*blades1.value3[9]\
    -blades0.value1[2]*blades1.value3[8]\
    +blades0.value2[2]*blades1.value2[9]\
    +blades0.value1[3]*blades1.value3[6]\
    -blades0.value2[4]*blades1.value2[8]\
    +blades0.value2[5]*blades1.value2[7]\
    +blades0.value3[3]*blades1.value1[4]\
    -blades0.value1[4]*blades1.value3[3]\
    +blades0.value2[7]*blades1.value2[5]\
    -blades0.value2[8]*blades1.value2[4]\
    -blades0.value3[6]*blades1.value1[3]\
    +blades0.value2[9]*blades1.value2[2]\
    +blades0.value3[8]*blades1.value1[2]\
    -blades0.value3[9]*blades1.value1[1]\
    +blades0.value4[4]*blades1.value0[0]\
;\
}
#define GEN1_BLADES_GRADE5OUTERPRODUCT(blades,blades0,blades1){\
    blades.value5[0] =\
    +blades0.value0[0]*blades1.value5[0]\
    +blades0.value1[0]*blades1.value4[4]\
    -blades0.value1[1]*blades1.value4[3]\
    +blades0.value2[0]*blades1.value3[9]\
    +blades0.value1[2]*blades1.value4[2]\
    -blades0.value2[1]*blades1.value3[8]\
    +blades0.value2[2]*blades1.value3[7]\
    +blades0.value3[0]*blades1.value2[9]\
    -blades0.value1[3]*blades1.value4[1]\
    +blades0.value2[3]*blades1.value3[6]\
    -blades0.value2[4]*blades1.value3[5]\
    -blades0.value3[1]*blades1.value2[8]\
    +blades0.value2[5]*blades1.value3[4]\
    +blades0.value3[2]*blades1.value2[7]\
    -blades0.value3[3]*blades1.value2[6]\
    +blades0.value4[0]*blades1.value1[4]\
    +blades0.value1[4]*blades1.value4[0]\
    -blades0.value2[6]*blades1.value3[3]\
    +blades0.value2[7]*blades1.value3[2]\
    +blades0.value3[4]*blades1.value2[5]\
    -blades0.value2[8]*blades1.value3[1]\
    -blades0.value3[5]*blades1.value2[4]\
    +blades0.value3[6]*blades1.value2[3]\
    -blades0.value4[1]*blades1.value1[3]\
    +blades0.value2[9]*blades1.value3[0]\
    +blades0.value3[7]*blades1.value2[2]\
    -blades0.value3[8]*blades1.value2[1]\
    +blades0.value4[2]*blades1.value1[2]\
    +blades0.value3[9]*blades1.value2[0]\
    -blades0.value4[3]*blades1.value1[1]\
    +blades0.value4[4]*blades1.value1[0]\
    +blades0.value5[0]*blades1.value0[0]\
;\
}

static gen1_BladesMultivector gen1_blades_gradeouterproduct(gen1_BladesMultivector blades0, gen1_BladesMultivector blades1, int *grades, Py_ssize_t size){
    gen1_BladesMultivector blades = blades1zero;
    for(Py_ssize_t i = 0; i < size; i++){
        switch(grades[i]){
            case 0:
                GEN1_BLADES_GRADE0OUTERPRODUCT(blades,blades0,blades1);
                break;
            case 1:
                GEN1_BLADES_GRADE1OUTERPRODUCT(blades,blades0,blades1);
                break;
            case 2:
                GEN1_BLADES_GRADE2OUTERPRODUCT(blades,blades0,blades1);
                break;
            case 3:
                GEN1_BLADES_GRADE3OUTERPRODUCT(blades,blades0,blades1);
                break;
            case 4:
                GEN1_BLADES_GRADE4OUTERPRODUCT(blades,blades0,blades1);
                break;
            case 5:
                GEN1_BLADES_GRADE5OUTERPRODUCT(blades,blades0,blades1);
                break;
            default:// do nothing for different values
                break;
        }
    }
    return blades;
}


static gen1_BladesMultivector gen1_blades_regressiveproduct(gen1_BladesMultivector blades0, gen1_BladesMultivector blades1){
    gen1_BladesMultivector blades = {{0},{0},{0},{0},{0},{0},};

    blades.value0[0] =
    +blades0.value0[0]*blades1.value5[0]
    +blades0.value1[0]*blades1.value4[4]
    -blades0.value1[1]*blades1.value4[3]
    +blades0.value2[0]*blades1.value3[9]
    +blades0.value1[2]*blades1.value4[2]
    -blades0.value2[1]*blades1.value3[8]
    +blades0.value2[2]*blades1.value3[7]
    +blades0.value3[0]*blades1.value2[9]
    -blades0.value1[3]*blades1.value4[1]
    +blades0.value2[3]*blades1.value3[6]
    -blades0.value2[4]*blades1.value3[5]
    -blades0.value3[1]*blades1.value2[8]
    +blades0.value2[5]*blades1.value3[4]
    +blades0.value3[2]*blades1.value2[7]
    -blades0.value3[3]*blades1.value2[6]
    +blades0.value4[0]*blades1.value1[4]
    +blades0.value1[4]*blades1.value4[0]
    -blades0.value2[6]*blades1.value3[3]
    +blades0.value2[7]*blades1.value3[2]
    +blades0.value3[4]*blades1.value2[5]
    -blades0.value2[8]*blades1.value3[1]
    -blades0.value3[5]*blades1.value2[4]
    +blades0.value3[6]*blades1.value2[3]
    -blades0.value4[1]*blades1.value1[3]
    +blades0.value2[9]*blades1.value3[0]
    +blades0.value3[7]*blades1.value2[2]
    -blades0.value3[8]*blades1.value2[1]
    +blades0.value4[2]*blades1.value1[2]
    +blades0.value3[9]*blades1.value2[0]
    -blades0.value4[3]*blades1.value1[1]
    +blades0.value4[4]*blades1.value1[0]
    +blades0.value5[0]*blades1.value0[0]
;
    blades.value1[0] =
    +blades0.value1[0]*blades1.value5[0]
    -blades0.value2[0]*blades1.value4[3]
    +blades0.value2[1]*blades1.value4[2]
    +blades0.value3[0]*blades1.value3[7]
    -blades0.value2[3]*blades1.value4[1]
    -blades0.value3[1]*blades1.value3[5]
    +blades0.value3[2]*blades1.value3[4]
    -blades0.value4[0]*blades1.value2[6]
    +blades0.value2[6]*blades1.value4[0]
    +blades0.value3[4]*blades1.value3[2]
    -blades0.value3[5]*blades1.value3[1]
    +blades0.value4[1]*blades1.value2[3]
    +blades0.value3[7]*blades1.value3[0]
    -blades0.value4[2]*blades1.value2[1]
    +blades0.value4[3]*blades1.value2[0]
    +blades0.value5[0]*blades1.value1[0]
;
    blades.value1[1] =
    -blades0.value1[1]*blades1.value5[0]
    +blades0.value2[0]*blades1.value4[4]
    -blades0.value2[2]*blades1.value4[2]
    -blades0.value3[0]*blades1.value3[8]
    +blades0.value2[4]*blades1.value4[1]
    +blades0.value3[1]*blades1.value3[6]
    -blades0.value3[3]*blades1.value3[4]
    +blades0.value4[0]*blades1.value2[7]
    -blades0.value2[7]*blades1.value4[0]
    -blades0.value3[4]*blades1.value3[3]
    +blades0.value3[6]*blades1.value3[1]
    -blades0.value4[1]*blades1.value2[4]
    -blades0.value3[8]*blades1.value3[0]
    +blades0.value4[2]*blades1.value2[2]
    -blades0.value4[4]*blades1.value2[0]
    -blades0.value5[0]*blades1.value1[1]
;
    blades.value2[0] =
    -blades0.value2[0]*blades1.value5[0]
    -blades0.value3[0]*blades1.value4[2]
    +blades0.value3[1]*blades1.value4[1]
    -blades0.value4[0]*blades1.value3[4]
    -blades0.value3[4]*blades1.value4[0]
    +blades0.value4[1]*blades1.value3[1]
    -blades0.value4[2]*blades1.value3[0]
    -blades0.value5[0]*blades1.value2[0]
;
    blades.value1[2] =
    +blades0.value1[2]*blades1.value5[0]
    -blades0.value2[1]*blades1.value4[4]
    +blades0.value2[2]*blades1.value4[3]
    +blades0.value3[0]*blades1.value3[9]
    -blades0.value2[5]*blades1.value4[1]
    -blades0.value3[2]*blades1.value3[6]
    +blades0.value3[3]*blades1.value3[5]
    -blades0.value4[0]*blades1.value2[8]
    +blades0.value2[8]*blades1.value4[0]
    +blades0.value3[5]*blades1.value3[3]
    -blades0.value3[6]*blades1.value3[2]
    +blades0.value4[1]*blades1.value2[5]
    +blades0.value3[9]*blades1.value3[0]
    -blades0.value4[3]*blades1.value2[2]
    +blades0.value4[4]*blades1.value2[1]
    +blades0.value5[0]*blades1.value1[2]
;
    blades.value2[1] =
    +blades0.value2[1]*blades1.value5[0]
    +blades0.value3[0]*blades1.value4[3]
    -blades0.value3[2]*blades1.value4[1]
    +blades0.value4[0]*blades1.value3[5]
    +blades0.value3[5]*blades1.value4[0]
    -blades0.value4[1]*blades1.value3[2]
    +blades0.value4[3]*blades1.value3[0]
    +blades0.value5[0]*blades1.value2[1]
;
    blades.value2[2] =
    -blades0.value2[2]*blades1.value5[0]
    -blades0.value3[0]*blades1.value4[4]
    +blades0.value3[3]*blades1.value4[1]
    -blades0.value4[0]*blades1.value3[6]
    -blades0.value3[6]*blades1.value4[0]
    +blades0.value4[1]*blades1.value3[3]
    -blades0.value4[4]*blades1.value3[0]
    -blades0.value5[0]*blades1.value2[2]
;
    blades.value3[0] =
    -blades0.value3[0]*blades1.value5[0]
    +blades0.value4[0]*blades1.value4[1]
    -blades0.value4[1]*blades1.value4[0]
    -blades0.value5[0]*blades1.value3[0]
;
    blades.value1[3] =
    -blades0.value1[3]*blades1.value5[0]
    +blades0.value2[3]*blades1.value4[4]
    -blades0.value2[4]*blades1.value4[3]
    -blades0.value3[1]*blades1.value3[9]
    +blades0.value2[5]*blades1.value4[2]
    +blades0.value3[2]*blades1.value3[8]
    -blades0.value3[3]*blades1.value3[7]
    +blades0.value4[0]*blades1.value2[9]
    -blades0.value2[9]*blades1.value4[0]
    -blades0.value3[7]*blades1.value3[3]
    +blades0.value3[8]*blades1.value3[2]
    -blades0.value4[2]*blades1.value2[5]
    -blades0.value3[9]*blades1.value3[1]
    +blades0.value4[3]*blades1.value2[4]
    -blades0.value4[4]*blades1.value2[3]
    -blades0.value5[0]*blades1.value1[3]
;
    blades.value2[3] =
    -blades0.value2[3]*blades1.value5[0]
    -blades0.value3[1]*blades1.value4[3]
    +blades0.value3[2]*blades1.value4[2]
    -blades0.value4[0]*blades1.value3[7]
    -blades0.value3[7]*blades1.value4[0]
    +blades0.value4[2]*blades1.value3[2]
    -blades0.value4[3]*blades1.value3[1]
    -blades0.value5[0]*blades1.value2[3]
;
    blades.value2[4] =
    +blades0.value2[4]*blades1.value5[0]
    +blades0.value3[1]*blades1.value4[4]
    -blades0.value3[3]*blades1.value4[2]
    +blades0.value4[0]*blades1.value3[8]
    +blades0.value3[8]*blades1.value4[0]
    -blades0.value4[2]*blades1.value3[3]
    +blades0.value4[4]*blades1.value3[1]
    +blades0.value5[0]*blades1.value2[4]
;
    blades.value3[1] =
    +blades0.value3[1]*blades1.value5[0]
    -blades0.value4[0]*blades1.value4[2]
    +blades0.value4[2]*blades1.value4[0]
    +blades0.value5[0]*blades1.value3[1]
;
    blades.value2[5] =
    -blades0.value2[5]*blades1.value5[0]
    -blades0.value3[2]*blades1.value4[4]
    +blades0.value3[3]*blades1.value4[3]
    -blades0.value4[0]*blades1.value3[9]
    -blades0.value3[9]*blades1.value4[0]
    +blades0.value4[3]*blades1.value3[3]
    -blades0.value4[4]*blades1.value3[2]
    -blades0.value5[0]*blades1.value2[5]
;
    blades.value3[2] =
    -blades0.value3[2]*blades1.value5[0]
    +blades0.value4[0]*blades1.value4[3]
    -blades0.value4[3]*blades1.value4[0]
    -blades0.value5[0]*blades1.value3[2]
;
    blades.value3[3] =
    +blades0.value3[3]*blades1.value5[0]
    -blades0.value4[0]*blades1.value4[4]
    +blades0.value4[4]*blades1.value4[0]
    +blades0.value5[0]*blades1.value3[3]
;
    blades.value4[0] =
    +blades0.value4[0]*blades1.value5[0]
    +blades0.value5[0]*blades1.value4[0]
;
    blades.value1[4] =
    +blades0.value1[4]*blades1.value5[0]
    -blades0.value2[6]*blades1.value4[4]
    +blades0.value2[7]*blades1.value4[3]
    +blades0.value3[4]*blades1.value3[9]
    -blades0.value2[8]*blades1.value4[2]
    -blades0.value3[5]*blades1.value3[8]
    +blades0.value3[6]*blades1.value3[7]
    -blades0.value4[1]*blades1.value2[9]
    +blades0.value2[9]*blades1.value4[1]
    +blades0.value3[7]*blades1.value3[6]
    -blades0.value3[8]*blades1.value3[5]
    +blades0.value4[2]*blades1.value2[8]
    +blades0.value3[9]*blades1.value3[4]
    -blades0.value4[3]*blades1.value2[7]
    +blades0.value4[4]*blades1.value2[6]
    +blades0.value5[0]*blades1.value1[4]
;
    blades.value2[6] =
    +blades0.value2[6]*blades1.value5[0]
    +blades0.value3[4]*blades1.value4[3]
    -blades0.value3[5]*blades1.value4[2]
    +blades0.value4[1]*blades1.value3[7]
    +blades0.value3[7]*blades1.value4[1]
    -blades0.value4[2]*blades1.value3[5]
    +blades0.value4[3]*blades1.value3[4]
    +blades0.value5[0]*blades1.value2[6]
;
    blades.value2[7] =
    -blades0.value2[7]*blades1.value5[0]
    -blades0.value3[4]*blades1.value4[4]
    +blades0.value3[6]*blades1.value4[2]
    -blades0.value4[1]*blades1.value3[8]
    -blades0.value3[8]*blades1.value4[1]
    +blades0.value4[2]*blades1.value3[6]
    -blades0.value4[4]*blades1.value3[4]
    -blades0.value5[0]*blades1.value2[7]
;
    blades.value3[4] =
    -blades0.value3[4]*blades1.value5[0]
    +blades0.value4[1]*blades1.value4[2]
    -blades0.value4[2]*blades1.value4[1]
    -blades0.value5[0]*blades1.value3[4]
;
    blades.value2[8] =
    +blades0.value2[8]*blades1.value5[0]
    +blades0.value3[5]*blades1.value4[4]
    -blades0.value3[6]*blades1.value4[3]
    +blades0.value4[1]*blades1.value3[9]
    +blades0.value3[9]*blades1.value4[1]
    -blades0.value4[3]*blades1.value3[6]
    +blades0.value4[4]*blades1.value3[5]
    +blades0.value5[0]*blades1.value2[8]
;
    blades.value3[5] =
    +blades0.value3[5]*blades1.value5[0]
    -blades0.value4[1]*blades1.value4[3]
    +blades0.value4[3]*blades1.value4[1]
    +blades0.value5[0]*blades1.value3[5]
;
    blades.value3[6] =
    -blades0.value3[6]*blades1.value5[0]
    +blades0.value4[1]*blades1.value4[4]
    -blades0.value4[4]*blades1.value4[1]
    -blades0.value5[0]*blades1.value3[6]
;
    blades.value4[1] =
    -blades0.value4[1]*blades1.value5[0]
    -blades0.value5[0]*blades1.value4[1]
;
    blades.value2[9] =
    -blades0.value2[9]*blades1.value5[0]
    -blades0.value3[7]*blades1.value4[4]
    +blades0.value3[8]*blades1.value4[3]
    -blades0.value4[2]*blades1.value3[9]
    -blades0.value3[9]*blades1.value4[2]
    +blades0.value4[3]*blades1.value3[8]
    -blades0.value4[4]*blades1.value3[7]
    -blades0.value5[0]*blades1.value2[9]
;
    blades.value3[7] =
    -blades0.value3[7]*blades1.value5[0]
    +blades0.value4[2]*blades1.value4[3]
    -blades0.value4[3]*blades1.value4[2]
    -blades0.value5[0]*blades1.value3[7]
;
    blades.value3[8] =
    +blades0.value3[8]*blades1.value5[0]
    -blades0.value4[2]*blades1.value4[4]
    +blades0.value4[4]*blades1.value4[2]
    +blades0.value5[0]*blades1.value3[8]
;
    blades.value4[2] =
    +blades0.value4[2]*blades1.value5[0]
    +blades0.value5[0]*blades1.value4[2]
;
    blades.value3[9] =
    -blades0.value3[9]*blades1.value5[0]
    +blades0.value4[3]*blades1.value4[4]
    -blades0.value4[4]*blades1.value4[3]
    -blades0.value5[0]*blades1.value3[9]
;
    blades.value4[3] =
    -blades0.value4[3]*blades1.value5[0]
    -blades0.value5[0]*blades1.value4[3]
;
    blades.value4[4] =
    +blades0.value4[4]*blades1.value5[0]
    +blades0.value5[0]*blades1.value4[4]
;
    blades.value5[0] =
    +blades0.value5[0]*blades1.value5[0]
;
    return blades;
}

#define GEN1_BLADES_GRADE0REGRESSIVEPRODUCT(blades,blades0,blades1){\
    blades.value0[0] =\
    +blades0.value0[0]*blades1.value5[0]\
    +blades0.value1[0]*blades1.value4[4]\
    -blades0.value1[1]*blades1.value4[3]\
    +blades0.value2[0]*blades1.value3[9]\
    +blades0.value1[2]*blades1.value4[2]\
    -blades0.value2[1]*blades1.value3[8]\
    +blades0.value2[2]*blades1.value3[7]\
    +blades0.value3[0]*blades1.value2[9]\
    -blades0.value1[3]*blades1.value4[1]\
    +blades0.value2[3]*blades1.value3[6]\
    -blades0.value2[4]*blades1.value3[5]\
    -blades0.value3[1]*blades1.value2[8]\
    +blades0.value2[5]*blades1.value3[4]\
    +blades0.value3[2]*blades1.value2[7]\
    -blades0.value3[3]*blades1.value2[6]\
    +blades0.value4[0]*blades1.value1[4]\
    +blades0.value1[4]*blades1.value4[0]\
    -blades0.value2[6]*blades1.value3[3]\
    +blades0.value2[7]*blades1.value3[2]\
    +blades0.value3[4]*blades1.value2[5]\
    -blades0.value2[8]*blades1.value3[1]\
    -blades0.value3[5]*blades1.value2[4]\
    +blades0.value3[6]*blades1.value2[3]\
    -blades0.value4[1]*blades1.value1[3]\
    +blades0.value2[9]*blades1.value3[0]\
    +blades0.value3[7]*blades1.value2[2]\
    -blades0.value3[8]*blades1.value2[1]\
    +blades0.value4[2]*blades1.value1[2]\
    +blades0.value3[9]*blades1.value2[0]\
    -blades0.value4[3]*blades1.value1[1]\
    +blades0.value4[4]*blades1.value1[0]\
    +blades0.value5[0]*blades1.value0[0]\
;\
}
#define GEN1_BLADES_GRADE1REGRESSIVEPRODUCT(blades,blades0,blades1){\
    blades.value1[0] =\
    +blades0.value1[0]*blades1.value5[0]\
    -blades0.value2[0]*blades1.value4[3]\
    +blades0.value2[1]*blades1.value4[2]\
    +blades0.value3[0]*blades1.value3[7]\
    -blades0.value2[3]*blades1.value4[1]\
    -blades0.value3[1]*blades1.value3[5]\
    +blades0.value3[2]*blades1.value3[4]\
    -blades0.value4[0]*blades1.value2[6]\
    +blades0.value2[6]*blades1.value4[0]\
    +blades0.value3[4]*blades1.value3[2]\
    -blades0.value3[5]*blades1.value3[1]\
    +blades0.value4[1]*blades1.value2[3]\
    +blades0.value3[7]*blades1.value3[0]\
    -blades0.value4[2]*blades1.value2[1]\
    +blades0.value4[3]*blades1.value2[0]\
    +blades0.value5[0]*blades1.value1[0]\
;\
    blades.value1[1] =\
    -blades0.value1[1]*blades1.value5[0]\
    +blades0.value2[0]*blades1.value4[4]\
    -blades0.value2[2]*blades1.value4[2]\
    -blades0.value3[0]*blades1.value3[8]\
    +blades0.value2[4]*blades1.value4[1]\
    +blades0.value3[1]*blades1.value3[6]\
    -blades0.value3[3]*blades1.value3[4]\
    +blades0.value4[0]*blades1.value2[7]\
    -blades0.value2[7]*blades1.value4[0]\
    -blades0.value3[4]*blades1.value3[3]\
    +blades0.value3[6]*blades1.value3[1]\
    -blades0.value4[1]*blades1.value2[4]\
    -blades0.value3[8]*blades1.value3[0]\
    +blades0.value4[2]*blades1.value2[2]\
    -blades0.value4[4]*blades1.value2[0]\
    -blades0.value5[0]*blades1.value1[1]\
;\
    blades.value1[2] =\
    +blades0.value1[2]*blades1.value5[0]\
    -blades0.value2[1]*blades1.value4[4]\
    +blades0.value2[2]*blades1.value4[3]\
    +blades0.value3[0]*blades1.value3[9]\
    -blades0.value2[5]*blades1.value4[1]\
    -blades0.value3[2]*blades1.value3[6]\
    +blades0.value3[3]*blades1.value3[5]\
    -blades0.value4[0]*blades1.value2[8]\
    +blades0.value2[8]*blades1.value4[0]\
    +blades0.value3[5]*blades1.value3[3]\
    -blades0.value3[6]*blades1.value3[2]\
    +blades0.value4[1]*blades1.value2[5]\
    +blades0.value3[9]*blades1.value3[0]\
    -blades0.value4[3]*blades1.value2[2]\
    +blades0.value4[4]*blades1.value2[1]\
    +blades0.value5[0]*blades1.value1[2]\
;\
    blades.value1[3] =\
    -blades0.value1[3]*blades1.value5[0]\
    +blades0.value2[3]*blades1.value4[4]\
    -blades0.value2[4]*blades1.value4[3]\
    -blades0.value3[1]*blades1.value3[9]\
    +blades0.value2[5]*blades1.value4[2]\
    +blades0.value3[2]*blades1.value3[8]\
    -blades0.value3[3]*blades1.value3[7]\
    +blades0.value4[0]*blades1.value2[9]\
    -blades0.value2[9]*blades1.value4[0]\
    -blades0.value3[7]*blades1.value3[3]\
    +blades0.value3[8]*blades1.value3[2]\
    -blades0.value4[2]*blades1.value2[5]\
    -blades0.value3[9]*blades1.value3[1]\
    +blades0.value4[3]*blades1.value2[4]\
    -blades0.value4[4]*blades1.value2[3]\
    -blades0.value5[0]*blades1.value1[3]\
;\
    blades.value1[4] =\
    +blades0.value1[4]*blades1.value5[0]\
    -blades0.value2[6]*blades1.value4[4]\
    +blades0.value2[7]*blades1.value4[3]\
    +blades0.value3[4]*blades1.value3[9]\
    -blades0.value2[8]*blades1.value4[2]\
    -blades0.value3[5]*blades1.value3[8]\
    +blades0.value3[6]*blades1.value3[7]\
    -blades0.value4[1]*blades1.value2[9]\
    +blades0.value2[9]*blades1.value4[1]\
    +blades0.value3[7]*blades1.value3[6]\
    -blades0.value3[8]*blades1.value3[5]\
    +blades0.value4[2]*blades1.value2[8]\
    +blades0.value3[9]*blades1.value3[4]\
    -blades0.value4[3]*blades1.value2[7]\
    +blades0.value4[4]*blades1.value2[6]\
    +blades0.value5[0]*blades1.value1[4]\
;\
}
#define GEN1_BLADES_GRADE2REGRESSIVEPRODUCT(blades,blades0,blades1){\
    blades.value2[0] =\
    -blades0.value2[0]*blades1.value5[0]\
    -blades0.value3[0]*blades1.value4[2]\
    +blades0.value3[1]*blades1.value4[1]\
    -blades0.value4[0]*blades1.value3[4]\
    -blades0.value3[4]*blades1.value4[0]\
    +blades0.value4[1]*blades1.value3[1]\
    -blades0.value4[2]*blades1.value3[0]\
    -blades0.value5[0]*blades1.value2[0]\
;\
    blades.value2[1] =\
    +blades0.value2[1]*blades1.value5[0]\
    +blades0.value3[0]*blades1.value4[3]\
    -blades0.value3[2]*blades1.value4[1]\
    +blades0.value4[0]*blades1.value3[5]\
    +blades0.value3[5]*blades1.value4[0]\
    -blades0.value4[1]*blades1.value3[2]\
    +blades0.value4[3]*blades1.value3[0]\
    +blades0.value5[0]*blades1.value2[1]\
;\
    blades.value2[2] =\
    -blades0.value2[2]*blades1.value5[0]\
    -blades0.value3[0]*blades1.value4[4]\
    +blades0.value3[3]*blades1.value4[1]\
    -blades0.value4[0]*blades1.value3[6]\
    -blades0.value3[6]*blades1.value4[0]\
    +blades0.value4[1]*blades1.value3[3]\
    -blades0.value4[4]*blades1.value3[0]\
    -blades0.value5[0]*blades1.value2[2]\
;\
    blades.value2[3] =\
    -blades0.value2[3]*blades1.value5[0]\
    -blades0.value3[1]*blades1.value4[3]\
    +blades0.value3[2]*blades1.value4[2]\
    -blades0.value4[0]*blades1.value3[7]\
    -blades0.value3[7]*blades1.value4[0]\
    +blades0.value4[2]*blades1.value3[2]\
    -blades0.value4[3]*blades1.value3[1]\
    -blades0.value5[0]*blades1.value2[3]\
;\
    blades.value2[4] =\
    +blades0.value2[4]*blades1.value5[0]\
    +blades0.value3[1]*blades1.value4[4]\
    -blades0.value3[3]*blades1.value4[2]\
    +blades0.value4[0]*blades1.value3[8]\
    +blades0.value3[8]*blades1.value4[0]\
    -blades0.value4[2]*blades1.value3[3]\
    +blades0.value4[4]*blades1.value3[1]\
    +blades0.value5[0]*blades1.value2[4]\
;\
    blades.value2[5] =\
    -blades0.value2[5]*blades1.value5[0]\
    -blades0.value3[2]*blades1.value4[4]\
    +blades0.value3[3]*blades1.value4[3]\
    -blades0.value4[0]*blades1.value3[9]\
    -blades0.value3[9]*blades1.value4[0]\
    +blades0.value4[3]*blades1.value3[3]\
    -blades0.value4[4]*blades1.value3[2]\
    -blades0.value5[0]*blades1.value2[5]\
;\
    blades.value2[6] =\
    +blades0.value2[6]*blades1.value5[0]\
    +blades0.value3[4]*blades1.value4[3]\
    -blades0.value3[5]*blades1.value4[2]\
    +blades0.value4[1]*blades1.value3[7]\
    +blades0.value3[7]*blades1.value4[1]\
    -blades0.value4[2]*blades1.value3[5]\
    +blades0.value4[3]*blades1.value3[4]\
    +blades0.value5[0]*blades1.value2[6]\
;\
    blades.value2[7] =\
    -blades0.value2[7]*blades1.value5[0]\
    -blades0.value3[4]*blades1.value4[4]\
    +blades0.value3[6]*blades1.value4[2]\
    -blades0.value4[1]*blades1.value3[8]\
    -blades0.value3[8]*blades1.value4[1]\
    +blades0.value4[2]*blades1.value3[6]\
    -blades0.value4[4]*blades1.value3[4]\
    -blades0.value5[0]*blades1.value2[7]\
;\
    blades.value2[8] =\
    +blades0.value2[8]*blades1.value5[0]\
    +blades0.value3[5]*blades1.value4[4]\
    -blades0.value3[6]*blades1.value4[3]\
    +blades0.value4[1]*blades1.value3[9]\
    +blades0.value3[9]*blades1.value4[1]\
    -blades0.value4[3]*blades1.value3[6]\
    +blades0.value4[4]*blades1.value3[5]\
    +blades0.value5[0]*blades1.value2[8]\
;\
    blades.value2[9] =\
    -blades0.value2[9]*blades1.value5[0]\
    -blades0.value3[7]*blades1.value4[4]\
    +blades0.value3[8]*blades1.value4[3]\
    -blades0.value4[2]*blades1.value3[9]\
    -blades0.value3[9]*blades1.value4[2]\
    +blades0.value4[3]*blades1.value3[8]\
    -blades0.value4[4]*blades1.value3[7]\
    -blades0.value5[0]*blades1.value2[9]\
;\
}
#define GEN1_BLADES_GRADE3REGRESSIVEPRODUCT(blades,blades0,blades1){\
    blades.value3[0] =\
    -blades0.value3[0]*blades1.value5[0]\
    +blades0.value4[0]*blades1.value4[1]\
    -blades0.value4[1]*blades1.value4[0]\
    -blades0.value5[0]*blades1.value3[0]\
;\
    blades.value3[1] =\
    +blades0.value3[1]*blades1.value5[0]\
    -blades0.value4[0]*blades1.value4[2]\
    +blades0.value4[2]*blades1.value4[0]\
    +blades0.value5[0]*blades1.value3[1]\
;\
    blades.value3[2] =\
    -blades0.value3[2]*blades1.value5[0]\
    +blades0.value4[0]*blades1.value4[3]\
    -blades0.value4[3]*blades1.value4[0]\
    -blades0.value5[0]*blades1.value3[2]\
;\
    blades.value3[3] =\
    +blades0.value3[3]*blades1.value5[0]\
    -blades0.value4[0]*blades1.value4[4]\
    +blades0.value4[4]*blades1.value4[0]\
    +blades0.value5[0]*blades1.value3[3]\
;\
    blades.value3[4] =\
    -blades0.value3[4]*blades1.value5[0]\
    +blades0.value4[1]*blades1.value4[2]\
    -blades0.value4[2]*blades1.value4[1]\
    -blades0.value5[0]*blades1.value3[4]\
;\
    blades.value3[5] =\
    +blades0.value3[5]*blades1.value5[0]\
    -blades0.value4[1]*blades1.value4[3]\
    +blades0.value4[3]*blades1.value4[1]\
    +blades0.value5[0]*blades1.value3[5]\
;\
    blades.value3[6] =\
    -blades0.value3[6]*blades1.value5[0]\
    +blades0.value4[1]*blades1.value4[4]\
    -blades0.value4[4]*blades1.value4[1]\
    -blades0.value5[0]*blades1.value3[6]\
;\
    blades.value3[7] =\
    -blades0.value3[7]*blades1.value5[0]\
    +blades0.value4[2]*blades1.value4[3]\
    -blades0.value4[3]*blades1.value4[2]\
    -blades0.value5[0]*blades1.value3[7]\
;\
    blades.value3[8] =\
    +blades0.value3[8]*blades1.value5[0]\
    -blades0.value4[2]*blades1.value4[4]\
    +blades0.value4[4]*blades1.value4[2]\
    +blades0.value5[0]*blades1.value3[8]\
;\
    blades.value3[9] =\
    -blades0.value3[9]*blades1.value5[0]\
    +blades0.value4[3]*blades1.value4[4]\
    -blades0.value4[4]*blades1.value4[3]\
    -blades0.value5[0]*blades1.value3[9]\
;\
}
#define GEN1_BLADES_GRADE4REGRESSIVEPRODUCT(blades,blades0,blades1){\
    blades.value4[0] =\
    +blades0.value4[0]*blades1.value5[0]\
    +blades0.value5[0]*blades1.value4[0]\
;\
    blades.value4[1] =\
    -blades0.value4[1]*blades1.value5[0]\
    -blades0.value5[0]*blades1.value4[1]\
;\
    blades.value4[2] =\
    +blades0.value4[2]*blades1.value5[0]\
    +blades0.value5[0]*blades1.value4[2]\
;\
    blades.value4[3] =\
    -blades0.value4[3]*blades1.value5[0]\
    -blades0.value5[0]*blades1.value4[3]\
;\
    blades.value4[4] =\
    +blades0.value4[4]*blades1.value5[0]\
    +blades0.value5[0]*blades1.value4[4]\
;\
}
#define GEN1_BLADES_GRADE5REGRESSIVEPRODUCT(blades,blades0,blades1){\
    blades.value5[0] =\
    +blades0.value5[0]*blades1.value5[0]\
;\
}

static gen1_BladesMultivector gen1_blades_graderegressiveproduct(gen1_BladesMultivector blades0, gen1_BladesMultivector blades1, int *grades, Py_ssize_t size){
    gen1_BladesMultivector blades = blades1zero;
    for(Py_ssize_t i = 0; i < size; i++){
        switch(grades[i]){
            case 0:
                GEN1_BLADES_GRADE0REGRESSIVEPRODUCT(blades,blades0,blades1);
                break;
            case 1:
                GEN1_BLADES_GRADE1REGRESSIVEPRODUCT(blades,blades0,blades1);
                break;
            case 2:
                GEN1_BLADES_GRADE2REGRESSIVEPRODUCT(blades,blades0,blades1);
                break;
            case 3:
                GEN1_BLADES_GRADE3REGRESSIVEPRODUCT(blades,blades0,blades1);
                break;
            case 4:
                GEN1_BLADES_GRADE4REGRESSIVEPRODUCT(blades,blades0,blades1);
                break;
            case 5:
                GEN1_BLADES_GRADE5REGRESSIVEPRODUCT(blades,blades0,blades1);
                break;
            default:// do nothing for different values
                break;
        }
    }
    return blades;
}


static int atomic_blades1_add(void *out, void *data0, PyAlgebraObject *GA, Py_ssize_t size){
    gen1_BladesMultivector *blades_array = (gen1_BladesMultivector *)data0;
    gen1_BladesMultivector *blades_out = (gen1_BladesMultivector *)out;
    gen1_BladesMultivector blades = {{0},{0},{0},{0},{0},{0},};

    for(Py_ssize_t i = 0; i < size; i++){
       blades.value0[0] += blades_array[i].value0[0];
       blades.value1[0] += blades_array[i].value1[0];
       blades.value1[1] += blades_array[i].value1[1];
       blades.value1[2] += blades_array[i].value1[2];
       blades.value1[3] += blades_array[i].value1[3];
       blades.value1[4] += blades_array[i].value1[4];
       blades.value2[0] += blades_array[i].value2[0];
       blades.value2[1] += blades_array[i].value2[1];
       blades.value2[2] += blades_array[i].value2[2];
       blades.value2[3] += blades_array[i].value2[3];
       blades.value2[4] += blades_array[i].value2[4];
       blades.value2[5] += blades_array[i].value2[5];
       blades.value2[6] += blades_array[i].value2[6];
       blades.value2[7] += blades_array[i].value2[7];
       blades.value2[8] += blades_array[i].value2[8];
       blades.value2[9] += blades_array[i].value2[9];
       blades.value3[0] += blades_array[i].value3[0];
       blades.value3[1] += blades_array[i].value3[1];
       blades.value3[2] += blades_array[i].value3[2];
       blades.value3[3] += blades_array[i].value3[3];
       blades.value3[4] += blades_array[i].value3[4];
       blades.value3[5] += blades_array[i].value3[5];
       blades.value3[6] += blades_array[i].value3[6];
       blades.value3[7] += blades_array[i].value3[7];
       blades.value3[8] += blades_array[i].value3[8];
       blades.value3[9] += blades_array[i].value3[9];
       blades.value4[0] += blades_array[i].value4[0];
       blades.value4[1] += blades_array[i].value4[1];
       blades.value4[2] += blades_array[i].value4[2];
       blades.value4[3] += blades_array[i].value4[3];
       blades.value4[4] += blades_array[i].value4[4];
       blades.value5[0] += blades_array[i].value5[0];
    }

    *blades_out = blades;
    return 1;
}

static int binary_blades1_add(void *out, void *data0, void *data1, PyAlgebraObject *GA, int sign){
    gen1_BladesMultivector *blades0 = (gen1_BladesMultivector *)data0;
    gen1_BladesMultivector *blades1 = (gen1_BladesMultivector *)data1;
    gen1_BladesMultivector *blades_out = (gen1_BladesMultivector *)out;

    gen1_BladesMultivector blades = {{0},{0},{0},{0},{0},{0},};

    if(sign == -1){
        blades.value0[0] = blades0->value0[0] - blades1->value0[0];
        blades.value1[0] = blades0->value1[0] - blades1->value1[0];
        blades.value1[1] = blades0->value1[1] - blades1->value1[1];
        blades.value1[2] = blades0->value1[2] - blades1->value1[2];
        blades.value1[3] = blades0->value1[3] - blades1->value1[3];
        blades.value1[4] = blades0->value1[4] - blades1->value1[4];
        blades.value2[0] = blades0->value2[0] - blades1->value2[0];
        blades.value2[1] = blades0->value2[1] - blades1->value2[1];
        blades.value2[2] = blades0->value2[2] - blades1->value2[2];
        blades.value2[3] = blades0->value2[3] - blades1->value2[3];
        blades.value2[4] = blades0->value2[4] - blades1->value2[4];
        blades.value2[5] = blades0->value2[5] - blades1->value2[5];
        blades.value2[6] = blades0->value2[6] - blades1->value2[6];
        blades.value2[7] = blades0->value2[7] - blades1->value2[7];
        blades.value2[8] = blades0->value2[8] - blades1->value2[8];
        blades.value2[9] = blades0->value2[9] - blades1->value2[9];
        blades.value3[0] = blades0->value3[0] - blades1->value3[0];
        blades.value3[1] = blades0->value3[1] - blades1->value3[1];
        blades.value3[2] = blades0->value3[2] - blades1->value3[2];
        blades.value3[3] = blades0->value3[3] - blades1->value3[3];
        blades.value3[4] = blades0->value3[4] - blades1->value3[4];
        blades.value3[5] = blades0->value3[5] - blades1->value3[5];
        blades.value3[6] = blades0->value3[6] - blades1->value3[6];
        blades.value3[7] = blades0->value3[7] - blades1->value3[7];
        blades.value3[8] = blades0->value3[8] - blades1->value3[8];
        blades.value3[9] = blades0->value3[9] - blades1->value3[9];
        blades.value4[0] = blades0->value4[0] - blades1->value4[0];
        blades.value4[1] = blades0->value4[1] - blades1->value4[1];
        blades.value4[2] = blades0->value4[2] - blades1->value4[2];
        blades.value4[3] = blades0->value4[3] - blades1->value4[3];
        blades.value4[4] = blades0->value4[4] - blades1->value4[4];
        blades.value5[0] = blades0->value5[0] - blades1->value5[0];
    }else if(sign == 1){
        blades.value0[0] = blades0->value0[0] + blades1->value0[0];
        blades.value1[0] = blades0->value1[0] + blades1->value1[0];
        blades.value1[1] = blades0->value1[1] + blades1->value1[1];
        blades.value1[2] = blades0->value1[2] + blades1->value1[2];
        blades.value1[3] = blades0->value1[3] + blades1->value1[3];
        blades.value1[4] = blades0->value1[4] + blades1->value1[4];
        blades.value2[0] = blades0->value2[0] + blades1->value2[0];
        blades.value2[1] = blades0->value2[1] + blades1->value2[1];
        blades.value2[2] = blades0->value2[2] + blades1->value2[2];
        blades.value2[3] = blades0->value2[3] + blades1->value2[3];
        blades.value2[4] = blades0->value2[4] + blades1->value2[4];
        blades.value2[5] = blades0->value2[5] + blades1->value2[5];
        blades.value2[6] = blades0->value2[6] + blades1->value2[6];
        blades.value2[7] = blades0->value2[7] + blades1->value2[7];
        blades.value2[8] = blades0->value2[8] + blades1->value2[8];
        blades.value2[9] = blades0->value2[9] + blades1->value2[9];
        blades.value3[0] = blades0->value3[0] + blades1->value3[0];
        blades.value3[1] = blades0->value3[1] + blades1->value3[1];
        blades.value3[2] = blades0->value3[2] + blades1->value3[2];
        blades.value3[3] = blades0->value3[3] + blades1->value3[3];
        blades.value3[4] = blades0->value3[4] + blades1->value3[4];
        blades.value3[5] = blades0->value3[5] + blades1->value3[5];
        blades.value3[6] = blades0->value3[6] + blades1->value3[6];
        blades.value3[7] = blades0->value3[7] + blades1->value3[7];
        blades.value3[8] = blades0->value3[8] + blades1->value3[8];
        blades.value3[9] = blades0->value3[9] + blades1->value3[9];
        blades.value4[0] = blades0->value4[0] + blades1->value4[0];
        blades.value4[1] = blades0->value4[1] + blades1->value4[1];
        blades.value4[2] = blades0->value4[2] + blades1->value4[2];
        blades.value4[3] = blades0->value4[3] + blades1->value4[3];
        blades.value4[4] = blades0->value4[4] + blades1->value4[4];
        blades.value5[0] = blades0->value5[0] + blades1->value5[0];
    }else{
        blades.value0[0] = blades0->value0[0] + sign*blades1->value0[0];
        blades.value1[0] = blades0->value1[0] + sign*blades1->value1[0];
        blades.value1[1] = blades0->value1[1] + sign*blades1->value1[1];
        blades.value1[2] = blades0->value1[2] + sign*blades1->value1[2];
        blades.value1[3] = blades0->value1[3] + sign*blades1->value1[3];
        blades.value1[4] = blades0->value1[4] + sign*blades1->value1[4];
        blades.value2[0] = blades0->value2[0] + sign*blades1->value2[0];
        blades.value2[1] = blades0->value2[1] + sign*blades1->value2[1];
        blades.value2[2] = blades0->value2[2] + sign*blades1->value2[2];
        blades.value2[3] = blades0->value2[3] + sign*blades1->value2[3];
        blades.value2[4] = blades0->value2[4] + sign*blades1->value2[4];
        blades.value2[5] = blades0->value2[5] + sign*blades1->value2[5];
        blades.value2[6] = blades0->value2[6] + sign*blades1->value2[6];
        blades.value2[7] = blades0->value2[7] + sign*blades1->value2[7];
        blades.value2[8] = blades0->value2[8] + sign*blades1->value2[8];
        blades.value2[9] = blades0->value2[9] + sign*blades1->value2[9];
        blades.value3[0] = blades0->value3[0] + sign*blades1->value3[0];
        blades.value3[1] = blades0->value3[1] + sign*blades1->value3[1];
        blades.value3[2] = blades0->value3[2] + sign*blades1->value3[2];
        blades.value3[3] = blades0->value3[3] + sign*blades1->value3[3];
        blades.value3[4] = blades0->value3[4] + sign*blades1->value3[4];
        blades.value3[5] = blades0->value3[5] + sign*blades1->value3[5];
        blades.value3[6] = blades0->value3[6] + sign*blades1->value3[6];
        blades.value3[7] = blades0->value3[7] + sign*blades1->value3[7];
        blades.value3[8] = blades0->value3[8] + sign*blades1->value3[8];
        blades.value3[9] = blades0->value3[9] + sign*blades1->value3[9];
        blades.value4[0] = blades0->value4[0] + sign*blades1->value4[0];
        blades.value4[1] = blades0->value4[1] + sign*blades1->value4[1];
        blades.value4[2] = blades0->value4[2] + sign*blades1->value4[2];
        blades.value4[3] = blades0->value4[3] + sign*blades1->value4[3];
        blades.value4[4] = blades0->value4[4] + sign*blades1->value4[4];
        blades.value5[0] = blades0->value5[0] + sign*blades1->value5[0];
    }
    *blades_out = blades;
    return 1;
}


static int binary_blades1_scalaradd(void *out, void *data0, PyAlgebraObject *GA, ga_float value, int sign){
    gen1_BladesMultivector *blades0 = (gen1_BladesMultivector *)data0;
    gen1_BladesMultivector *blades_out = (gen1_BladesMultivector *)out;

    gen1_BladesMultivector blades = {{0},{0},{0},{0},{0},{0},};

    if(sign == -1){
        blades.value0[0] = -blades0->value0[0];
        blades.value1[0] = -blades0->value1[0];
        blades.value1[1] = -blades0->value1[1];
        blades.value1[2] = -blades0->value1[2];
        blades.value1[3] = -blades0->value1[3];
        blades.value1[4] = -blades0->value1[4];
        blades.value2[0] = -blades0->value2[0];
        blades.value2[1] = -blades0->value2[1];
        blades.value2[2] = -blades0->value2[2];
        blades.value2[3] = -blades0->value2[3];
        blades.value2[4] = -blades0->value2[4];
        blades.value2[5] = -blades0->value2[5];
        blades.value2[6] = -blades0->value2[6];
        blades.value2[7] = -blades0->value2[7];
        blades.value2[8] = -blades0->value2[8];
        blades.value2[9] = -blades0->value2[9];
        blades.value3[0] = -blades0->value3[0];
        blades.value3[1] = -blades0->value3[1];
        blades.value3[2] = -blades0->value3[2];
        blades.value3[3] = -blades0->value3[3];
        blades.value3[4] = -blades0->value3[4];
        blades.value3[5] = -blades0->value3[5];
        blades.value3[6] = -blades0->value3[6];
        blades.value3[7] = -blades0->value3[7];
        blades.value3[8] = -blades0->value3[8];
        blades.value3[9] = -blades0->value3[9];
        blades.value4[0] = -blades0->value4[0];
        blades.value4[1] = -blades0->value4[1];
        blades.value4[2] = -blades0->value4[2];
        blades.value4[3] = -blades0->value4[3];
        blades.value4[4] = -blades0->value4[4];
        blades.value5[0] = -blades0->value5[0];
    }else if(sign == 1){
        blades.value0[0] = blades0->value0[0];
        blades.value1[0] = blades0->value1[0];
        blades.value1[1] = blades0->value1[1];
        blades.value1[2] = blades0->value1[2];
        blades.value1[3] = blades0->value1[3];
        blades.value1[4] = blades0->value1[4];
        blades.value2[0] = blades0->value2[0];
        blades.value2[1] = blades0->value2[1];
        blades.value2[2] = blades0->value2[2];
        blades.value2[3] = blades0->value2[3];
        blades.value2[4] = blades0->value2[4];
        blades.value2[5] = blades0->value2[5];
        blades.value2[6] = blades0->value2[6];
        blades.value2[7] = blades0->value2[7];
        blades.value2[8] = blades0->value2[8];
        blades.value2[9] = blades0->value2[9];
        blades.value3[0] = blades0->value3[0];
        blades.value3[1] = blades0->value3[1];
        blades.value3[2] = blades0->value3[2];
        blades.value3[3] = blades0->value3[3];
        blades.value3[4] = blades0->value3[4];
        blades.value3[5] = blades0->value3[5];
        blades.value3[6] = blades0->value3[6];
        blades.value3[7] = blades0->value3[7];
        blades.value3[8] = blades0->value3[8];
        blades.value3[9] = blades0->value3[9];
        blades.value4[0] = blades0->value4[0];
        blades.value4[1] = blades0->value4[1];
        blades.value4[2] = blades0->value4[2];
        blades.value4[3] = blades0->value4[3];
        blades.value4[4] = blades0->value4[4];
        blades.value5[0] = blades0->value5[0];
    }else{
        blades.value0[0] = sign*blades0->value0[0];
        blades.value1[0] = sign*blades0->value1[0];
        blades.value1[1] = sign*blades0->value1[1];
        blades.value1[2] = sign*blades0->value1[2];
        blades.value1[3] = sign*blades0->value1[3];
        blades.value1[4] = sign*blades0->value1[4];
        blades.value2[0] = sign*blades0->value2[0];
        blades.value2[1] = sign*blades0->value2[1];
        blades.value2[2] = sign*blades0->value2[2];
        blades.value2[3] = sign*blades0->value2[3];
        blades.value2[4] = sign*blades0->value2[4];
        blades.value2[5] = sign*blades0->value2[5];
        blades.value2[6] = sign*blades0->value2[6];
        blades.value2[7] = sign*blades0->value2[7];
        blades.value2[8] = sign*blades0->value2[8];
        blades.value2[9] = sign*blades0->value2[9];
        blades.value3[0] = sign*blades0->value3[0];
        blades.value3[1] = sign*blades0->value3[1];
        blades.value3[2] = sign*blades0->value3[2];
        blades.value3[3] = sign*blades0->value3[3];
        blades.value3[4] = sign*blades0->value3[4];
        blades.value3[5] = sign*blades0->value3[5];
        blades.value3[6] = sign*blades0->value3[6];
        blades.value3[7] = sign*blades0->value3[7];
        blades.value3[8] = sign*blades0->value3[8];
        blades.value3[9] = sign*blades0->value3[9];
        blades.value4[0] = sign*blades0->value4[0];
        blades.value4[1] = sign*blades0->value4[1];
        blades.value4[2] = sign*blades0->value4[2];
        blades.value4[3] = sign*blades0->value4[3];
        blades.value4[4] = sign*blades0->value4[4];
        blades.value5[0] = sign*blades0->value5[0];
    }
    blades.value0[0] += value;
    *blades_out = blades;
    return 1;
}


static int binary_blades1_scalarproduct(void *out, void *data0, PyAlgebraObject *GA, ga_float value){
    gen1_BladesMultivector *blades0 = (gen1_BladesMultivector *)data0;
    gen1_BladesMultivector *blades_out = (gen1_BladesMultivector *)out;

    gen1_BladesMultivector blades = {{0},{0},{0},{0},{0},{0},};

    blades.value0[0] = value*blades0->value0[0];
    blades.value1[0] = value*blades0->value1[0];
    blades.value1[1] = value*blades0->value1[1];
    blades.value1[2] = value*blades0->value1[2];
    blades.value1[3] = value*blades0->value1[3];
    blades.value1[4] = value*blades0->value1[4];
    blades.value2[0] = value*blades0->value2[0];
    blades.value2[1] = value*blades0->value2[1];
    blades.value2[2] = value*blades0->value2[2];
    blades.value2[3] = value*blades0->value2[3];
    blades.value2[4] = value*blades0->value2[4];
    blades.value2[5] = value*blades0->value2[5];
    blades.value2[6] = value*blades0->value2[6];
    blades.value2[7] = value*blades0->value2[7];
    blades.value2[8] = value*blades0->value2[8];
    blades.value2[9] = value*blades0->value2[9];
    blades.value3[0] = value*blades0->value3[0];
    blades.value3[1] = value*blades0->value3[1];
    blades.value3[2] = value*blades0->value3[2];
    blades.value3[3] = value*blades0->value3[3];
    blades.value3[4] = value*blades0->value3[4];
    blades.value3[5] = value*blades0->value3[5];
    blades.value3[6] = value*blades0->value3[6];
    blades.value3[7] = value*blades0->value3[7];
    blades.value3[8] = value*blades0->value3[8];
    blades.value3[9] = value*blades0->value3[9];
    blades.value4[0] = value*blades0->value4[0];
    blades.value4[1] = value*blades0->value4[1];
    blades.value4[2] = value*blades0->value4[2];
    blades.value4[3] = value*blades0->value4[3];
    blades.value4[4] = value*blades0->value4[4];
    blades.value5[0] = value*blades0->value5[0];
    *blades_out = blades;
    return 1;
}

static int unary_blades1_reverse(void *out, void *data0, PyAlgebraObject *GA){
    gen1_BladesMultivector *blades0 = (gen1_BladesMultivector *)data0;
    gen1_BladesMultivector *blades_out = (gen1_BladesMultivector *)out;

    gen1_BladesMultivector blades = {{0},{0},{0},{0},{0},{0},};

    blades.value0[0] = blades0->value0[0];
    blades.value1[0] = blades0->value1[0];
    blades.value1[1] = blades0->value1[1];
    blades.value1[2] = blades0->value1[2];
    blades.value1[3] = blades0->value1[3];
    blades.value1[4] = blades0->value1[4];
    blades.value2[0] = -blades0->value2[0];
    blades.value2[1] = -blades0->value2[1];
    blades.value2[2] = -blades0->value2[2];
    blades.value2[3] = -blades0->value2[3];
    blades.value2[4] = -blades0->value2[4];
    blades.value2[5] = -blades0->value2[5];
    blades.value2[6] = -blades0->value2[6];
    blades.value2[7] = -blades0->value2[7];
    blades.value2[8] = -blades0->value2[8];
    blades.value2[9] = -blades0->value2[9];
    blades.value3[0] = -blades0->value3[0];
    blades.value3[1] = -blades0->value3[1];
    blades.value3[2] = -blades0->value3[2];
    blades.value3[3] = -blades0->value3[3];
    blades.value3[4] = -blades0->value3[4];
    blades.value3[5] = -blades0->value3[5];
    blades.value3[6] = -blades0->value3[6];
    blades.value3[7] = -blades0->value3[7];
    blades.value3[8] = -blades0->value3[8];
    blades.value3[9] = -blades0->value3[9];
    blades.value4[0] = blades0->value4[0];
    blades.value4[1] = blades0->value4[1];
    blades.value4[2] = blades0->value4[2];
    blades.value4[3] = blades0->value4[3];
    blades.value4[4] = blades0->value4[4];
    blades.value5[0] = blades0->value5[0];
    
    *blades_out = blades;
    return 1;
}


static int unary_blades1_dual(void *out, void *data0, PyAlgebraObject *GA){
    gen1_BladesMultivector *blades0 = (gen1_BladesMultivector *)data0;
    gen1_BladesMultivector *blades_out = (gen1_BladesMultivector *)out;

    gen1_BladesMultivector blades = {{0},{0},{0},{0},{0},{0},};

    blades.value5[0] =  blades0->value0[0];
    blades.value4[4] =  blades0->value1[0];
    blades.value4[3] = -blades0->value1[1];
    blades.value3[9] = -blades0->value2[0];
    blades.value4[2] =  blades0->value1[2];
    blades.value3[8] =  blades0->value2[1];
    blades.value3[7] = -blades0->value2[2];
    blades.value2[9] = -blades0->value3[0];
    blades.value4[1] = -blades0->value1[3];
    blades.value3[6] = -blades0->value2[3];
    blades.value3[5] =  blades0->value2[4];
    blades.value2[8] =  blades0->value3[1];
    blades.value3[4] = -blades0->value2[5];
    blades.value2[7] = -blades0->value3[2];
    blades.value2[6] =  blades0->value3[3];
    blades.value1[4] =  blades0->value4[0];
    blades.value4[0] =  blades0->value1[4];
    blades.value3[3] =  blades0->value2[6];
    blades.value3[2] = -blades0->value2[7];
    blades.value2[5] = -blades0->value3[4];
    blades.value3[1] =  blades0->value2[8];
    blades.value2[4] =  blades0->value3[5];
    blades.value2[3] = -blades0->value3[6];
    blades.value1[3] = -blades0->value4[1];
    blades.value3[0] = -blades0->value2[9];
    blades.value2[2] = -blades0->value3[7];
    blades.value2[1] =  blades0->value3[8];
    blades.value1[2] =  blades0->value4[2];
    blades.value2[0] = -blades0->value3[9];
    blades.value1[1] = -blades0->value4[3];
    blades.value1[0] =  blades0->value4[4];
    blades.value0[0] =  blades0->value5[0];
    *blades_out = blades;
    return 1;
}

static int unary_blades1_undual(void *out, void *data0, PyAlgebraObject *GA){
    gen1_BladesMultivector *blades0 = (gen1_BladesMultivector*)data0;
    gen1_BladesMultivector *blades_out = (gen1_BladesMultivector*)out;

    gen1_BladesMultivector blades = {{0},{0},{0},{0},{0},{0},};

    blades.value5[0] =  blades0->value0[0];
    blades.value4[4] =  blades0->value1[0];
    blades.value4[3] = -blades0->value1[1];
    blades.value3[9] = -blades0->value2[0];
    blades.value4[2] =  blades0->value1[2];
    blades.value3[8] =  blades0->value2[1];
    blades.value3[7] = -blades0->value2[2];
    blades.value2[9] = -blades0->value3[0];
    blades.value4[1] = -blades0->value1[3];
    blades.value3[6] = -blades0->value2[3];
    blades.value3[5] =  blades0->value2[4];
    blades.value2[8] =  blades0->value3[1];
    blades.value3[4] = -blades0->value2[5];
    blades.value2[7] = -blades0->value3[2];
    blades.value2[6] =  blades0->value3[3];
    blades.value1[4] =  blades0->value4[0];
    blades.value4[0] =  blades0->value1[4];
    blades.value3[3] =  blades0->value2[6];
    blades.value3[2] = -blades0->value2[7];
    blades.value2[5] = -blades0->value3[4];
    blades.value3[1] =  blades0->value2[8];
    blades.value2[4] =  blades0->value3[5];
    blades.value2[3] = -blades0->value3[6];
    blades.value1[3] = -blades0->value4[1];
    blades.value3[0] = -blades0->value2[9];
    blades.value2[2] = -blades0->value3[7];
    blades.value2[1] =  blades0->value3[8];
    blades.value1[2] =  blades0->value4[2];
    blades.value2[0] = -blades0->value3[9];
    blades.value1[1] = -blades0->value4[3];
    blades.value1[0] =  blades0->value4[4];
    blades.value0[0] =  blades0->value5[0];
    *blades_out = blades;
    return 1;
}




static int binary_dense1_product(void *out, void *data0, void *data1, PyAlgebraObject *GA, ProductType ptype){
    gen1_DenseMultivector *pdense0 = (gen1_DenseMultivector*)data0;
    gen1_DenseMultivector *pdense1 = (gen1_DenseMultivector*)data1;
    gen1_DenseMultivector *pdense  = (gen1_DenseMultivector*)out;

    if(!pdense0 || !pdense1 || !pdense){
        return 0; // raise error
    }

    switch(ptype){
        case ProductType_geometric:
            *pdense = gen1_dense_geometricproduct(*pdense0,*pdense1);
            break;
        case ProductType_inner:
            *pdense = gen1_dense_innerproduct(*pdense0,*pdense1);
            break;
        case ProductType_outer:
            *pdense = gen1_dense_outerproduct(*pdense0,*pdense1);
            break;
        case ProductType_regressive:
            *pdense = gen1_dense_regressiveproduct(*pdense0,*pdense1);
            break;
        default:
            return 0;
    }

    return 1;
}
static int binary_blades1_product(void *out, void *data0, void *data1, PyAlgebraObject *GA, ProductType ptype){
    gen1_BladesMultivector *pblades0 = (gen1_BladesMultivector*)data0;
    gen1_BladesMultivector *pblades1 = (gen1_BladesMultivector*)data1;
    gen1_BladesMultivector *pblades  = (gen1_BladesMultivector*)out;

    if(!pblades0 || !pblades1 || !pblades){
        return 0; // raise error
    }

    switch(ptype){
        case ProductType_geometric:
            *pblades = gen1_blades_geometricproduct(*pblades0,*pblades1);
            break;
        case ProductType_inner:
            *pblades = gen1_blades_innerproduct(*pblades0,*pblades1);
            break;
        case ProductType_outer:
            *pblades = gen1_blades_outerproduct(*pblades0,*pblades1);
            break;
        case ProductType_regressive:
            *pblades = gen1_blades_regressiveproduct(*pblades0,*pblades1);
            break;
        default:
            return 0;
    }

    return 1;
}


static int binary_dense1_gradeproduct(void *out, void *data0, void *data1, PyAlgebraObject *GA, ProductType ptype, GradeProjectMap gpmap){
    gen1_DenseMultivector *pdense0 = (gen1_DenseMultivector*)data0;
    gen1_DenseMultivector *pdense1 = (gen1_DenseMultivector*)data1;
    gen1_DenseMultivector *pdense  = (gen1_DenseMultivector*)out;

    gen1_DenseMultivector projdense0 = {{0}};
    gen1_DenseMultivector projdense1 = {{0}};

    
    if(!pdense0 || !pdense1 || !pdense){
        return 0; // raise error
    }
    // grade projection of the input
    if(gen1_dense_gradeproject(&projdense0,pdense0,gpmap.grades0,gpmap.size0) == -1) return 0;
    if(gen1_dense_gradeproject(&projdense1,pdense1,gpmap.grades0,gpmap.size0) == -1) return 0;


    switch(ptype){
        case ProductType_geometric:
            *pdense = gen1_dense_gradegeometricproduct(projdense0,projdense1,gpmap.grades,gpmap.size);
            break;
        case ProductType_inner:
            *pdense = gen1_dense_gradeinnerproduct(projdense0,projdense1,gpmap.grades,gpmap.size);
            break;
        case ProductType_outer:
            *pdense = gen1_dense_gradeouterproduct(projdense0,projdense1,gpmap.grades,gpmap.size);
            break;
        case ProductType_regressive:
            *pdense = gen1_dense_graderegressiveproduct(projdense0,projdense1,gpmap.grades,gpmap.size);
            break;
        default:
            return 0;
    }

    return 1;
}
static int binary_blades1_gradeproduct(void *out, void *data0, void *data1, PyAlgebraObject *GA, ProductType ptype, GradeProjectMap gpmap){
    gen1_BladesMultivector *pblades0 = (gen1_BladesMultivector*)data0;
    gen1_BladesMultivector *pblades1 = (gen1_BladesMultivector*)data1;
    gen1_BladesMultivector *pblades  = (gen1_BladesMultivector*)out;

    gen1_BladesMultivector projblades0 =  blades1zero;
    gen1_BladesMultivector projblades1 =  blades1zero;

    
    if(!pblades0 || !pblades1 || !pblades){
        return 0; // raise error
    }
    // grade projection of the input
    if(gen1_blades_gradeproject(&projblades0,pblades0,gpmap.grades0,gpmap.size0) == -1) return 0;
    if(gen1_blades_gradeproject(&projblades1,pblades1,gpmap.grades0,gpmap.size0) == -1) return 0;


    switch(ptype){
        case ProductType_geometric:
            *pblades = gen1_blades_gradegeometricproduct(projblades0,projblades1,gpmap.grades,gpmap.size);
            break;
        case ProductType_inner:
            *pblades = gen1_blades_gradeinnerproduct(projblades0,projblades1,gpmap.grades,gpmap.size);
            break;
        case ProductType_outer:
            *pblades = gen1_blades_gradeouterproduct(projblades0,projblades1,gpmap.grades,gpmap.size);
            break;
        case ProductType_regressive:
            *pblades = gen1_blades_graderegressiveproduct(projblades0,projblades1,gpmap.grades,gpmap.size);
            break;
        default:
            return 0;
    }

    return 1;
}



static int ternary_dense1_product(void *out, void *data0, void *data1, void *data2, PyAlgebraObject *GA, ProductType ptype){
    gen1_DenseMultivector *pdense0 = (gen1_DenseMultivector*)data0;
    gen1_DenseMultivector *pdense1 = (gen1_DenseMultivector*)data1;
    gen1_DenseMultivector *pdense2 = (gen1_DenseMultivector*)data2;
    gen1_DenseMultivector *pdense  = (gen1_DenseMultivector*)out;
    
    if(!pdense0 || !pdense1 || !pdense2 || !pdense){
        return 0; // raise error
    }

    switch(ptype){
        case ProductType_geometric:
            *pdense = gen1_dense_geometricproduct(*pdense0,*pdense1);
            *pdense = gen1_dense_geometricproduct(*pdense,*pdense2);
            break;
        case ProductType_inner:
            *pdense = gen1_dense_innerproduct(*pdense0,*pdense1);
            *pdense = gen1_dense_innerproduct(*pdense,*pdense2);
            break;
        case ProductType_outer:
            *pdense = gen1_dense_outerproduct(*pdense0,*pdense1);
            *pdense = gen1_dense_outerproduct(*pdense,*pdense2);
            break;
        case ProductType_regressive:
            *pdense = gen1_dense_regressiveproduct(*pdense0,*pdense1);
            *pdense = gen1_dense_regressiveproduct(*pdense,*pdense2);
            break;
        default:
            return 0;
    }

    return 1;
}
static int ternary_blades1_product(void *out, void *data0, void *data1, void *data2, PyAlgebraObject *GA, ProductType ptype){
    gen1_BladesMultivector *pblades0 = (gen1_BladesMultivector*)data0;
    gen1_BladesMultivector *pblades1 = (gen1_BladesMultivector*)data1;
    gen1_BladesMultivector *pblades2 = (gen1_BladesMultivector*)data2;
    gen1_BladesMultivector *pblades  = (gen1_BladesMultivector*)out;
    
    if(!pblades0 || !pblades1 || !pblades2 || !pblades){
        return 0; // raise error
    }

    switch(ptype){
        case ProductType_geometric:
            *pblades = gen1_blades_geometricproduct(*pblades0,*pblades1);
            *pblades = gen1_blades_geometricproduct(*pblades,*pblades2);
            break;
        case ProductType_inner:
            *pblades = gen1_blades_innerproduct(*pblades0,*pblades1);
            *pblades = gen1_blades_innerproduct(*pblades,*pblades2);
            break;
        case ProductType_outer:
            *pblades = gen1_blades_outerproduct(*pblades0,*pblades1);
            *pblades = gen1_blades_outerproduct(*pblades,*pblades2);
            break;
        case ProductType_regressive:
            *pblades = gen1_blades_regressiveproduct(*pblades0,*pblades1);
            *pblades = gen1_blades_regressiveproduct(*pblades,*pblades2);
            break;
        default:
            return 0;
    }

    return 1;
}

static int unary_dense1_gradeproject(void *out, void *self, PyAlgebraObject *GA, int *grades, Py_ssize_t size){
    
    //gen1_DenseMultivector dense = {{0}};
    gen1_DenseMultivector *pdense = (gen1_DenseMultivector *)out;
    gen1_DenseMultivector *pdense0 = (gen1_DenseMultivector*)self;

    if(gen1_dense_gradeproject(pdense,pdense0,grades,size) == -1) return 0;
    
    return 1;
}
static int unary_blades1_gradeproject(void *out, void *self, PyAlgebraObject *GA, int *grades, Py_ssize_t size){
    
    //gen1_BladesMultivector blades =  blades1zero;
    gen1_BladesMultivector *pblades = (gen1_BladesMultivector *)out;
    gen1_BladesMultivector *pblades0 = (gen1_BladesMultivector*)self;

    if(gen1_blades_gradeproject(pblades,pblades0,grades,size) == -1) return 0;
    
    return 1;
}

static int atomic_dense1_product(void *out, void *data0, PyAlgebraObject *GA, Py_ssize_t size, ProductType ptype){
    if(size < 2) return 0;
    
    gen1_DenseMultivector *pdense0 = (gen1_DenseMultivector*)data0;
    gen1_DenseMultivector *pdense = (gen1_DenseMultivector*)out;
    gen1_DenseMultivector dense;
    if(!out  || !pdense){
        return 0; // raise memory error
    }
    switch(ptype){
        case ProductType_geometric:
            dense = gen1_dense_geometricproduct(*pdense0,pdense0[1]);
            for(Py_ssize_t i = 2; i < size; i++){
                dense = gen1_dense_geometricproduct(dense,pdense0[i]);
            }
            break;
        case ProductType_inner:
            dense = gen1_dense_innerproduct(*pdense0,pdense0[1]);
            for(Py_ssize_t i = 2; i < size; i++){
                dense = gen1_dense_innerproduct(dense,pdense0[i]);
            }
            break;
        case ProductType_outer:
            dense = gen1_dense_outerproduct(*pdense0,pdense0[1]);
            for(Py_ssize_t i = 2; i < size; i++){
                dense = gen1_dense_outerproduct(dense,pdense0[i]);
            }
            break;
        case ProductType_regressive:
            dense = gen1_dense_regressiveproduct(*pdense0,pdense0[1]);
            for(Py_ssize_t i = 2; i < size; i++){
                dense = gen1_dense_regressiveproduct(dense,pdense0[i]);
            }
            break;
        default:
            return 0;
    }
    *pdense = dense;

    return 1;
}
static int atomic_blades1_product(void *out, void *data0, PyAlgebraObject *GA, Py_ssize_t size, ProductType ptype){
    if(size < 2) return 0;
    
    gen1_BladesMultivector *pblades0 = (gen1_BladesMultivector*)data0;
    gen1_BladesMultivector *pblades = (gen1_BladesMultivector*)out;
    gen1_BladesMultivector blades;
    if(!out  || !pblades){
        return 0; // raise memory error
    }
    switch(ptype){
        case ProductType_geometric:
            blades = gen1_blades_geometricproduct(*pblades0,pblades0[1]);
            for(Py_ssize_t i = 2; i < size; i++){
                blades = gen1_blades_geometricproduct(blades,pblades0[i]);
            }
            break;
        case ProductType_inner:
            blades = gen1_blades_innerproduct(*pblades0,pblades0[1]);
            for(Py_ssize_t i = 2; i < size; i++){
                blades = gen1_blades_innerproduct(blades,pblades0[i]);
            }
            break;
        case ProductType_outer:
            blades = gen1_blades_outerproduct(*pblades0,pblades0[1]);
            for(Py_ssize_t i = 2; i < size; i++){
                blades = gen1_blades_outerproduct(blades,pblades0[i]);
            }
            break;
        case ProductType_regressive:
            blades = gen1_blades_regressiveproduct(*pblades0,pblades0[1]);
            for(Py_ssize_t i = 2; i < size; i++){
                blades = gen1_blades_regressiveproduct(blades,pblades0[i]);
            }
            break;
        default:
            return 0;
    }
    *pblades = blades;

    return 1;
}



static PyMultivectorMath_Funcs dense0_math_funcs = {
    .atomic_add = atomic_dense0_add,
    .atomic_product =  atomic_dense0_product,
    .add =  binary_dense0_add,
    .product =  binary_dense0_product,
    .grade_project =  unary_dense0_gradeproject,
    .scalar_product = binary_dense0_scalarproduct,
    .scalar_add = binary_dense0_scalaradd,
    .reverse = unary_dense0_reverse,
    .dual = unary_dense0_dual,
    .undual = unary_dense0_undual,
    .ternary_product = ternary_dense0_product,
    .graded_product = binary_dense0_gradeproduct,
};

static PyMultivectorMath_Funcs blades0_math_funcs = {
    .atomic_add = atomic_blades0_add,
    .atomic_product =  atomic_blades0_product,
    .add =  binary_blades0_add,
    .product =  binary_blades0_product,
    .grade_project =  unary_blades0_gradeproject,
    .scalar_product = binary_blades0_scalarproduct,
    .scalar_add = binary_blades0_scalaradd,
    .reverse = unary_blades0_reverse,
    .dual = unary_blades0_dual,
    .undual = unary_blades0_undual,
    .ternary_product = ternary_blades0_product,
    .graded_product = binary_blades0_gradeproduct,
};


static PyMultivectorData_Funcs dense0_data_funcs = {
  .iter_next = dense0_iternext,
  .iter_init = dense0_iterinit,
  .init = dense0_init,
  .cast = cast_to_dense0,
};

static PyMultivectorData_Funcs blades0_data_funcs = {
  .iter_next = blades0_iternext,
  .iter_init = blades0_iterinit,
  .init = blades0_init,
  .cast = cast_to_blades0,
};


static const PyMultivectorSubType dense0_subtype = {
    .math_funcs = &dense0_math_funcs,
    .data_funcs = &dense0_data_funcs,
    .name = "3DVGA",
    .type_name = "dense0",
    .generated = 1,
    .metric = {1,1,1,},
    .msize = 3,
    .ntype = 3,
    .asize = 8,
    .basic_size = sizeof(gen0_DenseMultivector),
};

static const PyMultivectorSubType blades0_subtype = {
    .math_funcs = &blades0_math_funcs,
    .data_funcs = &blades0_data_funcs,
    .name = "3DVGA",
    .type_name = "blades0",
    .generated = 1,
    .metric = {1,1,1,},
    .msize = 3,
    .ntype = 4,
    .asize = 8,
    .basic_size = sizeof(gen0_BladesMultivector),
};


static PyMultivectorMath_Funcs dense1_math_funcs = {
    .atomic_add = atomic_dense1_add,
    .atomic_product =  atomic_dense1_product,
    .add =  binary_dense1_add,
    .product =  binary_dense1_product,
    .grade_project =  unary_dense1_gradeproject,
    .scalar_product = binary_dense1_scalarproduct,
    .scalar_add = binary_dense1_scalaradd,
    .reverse = unary_dense1_reverse,
    .dual = unary_dense1_dual,
    .undual = unary_dense1_undual,
    .ternary_product = ternary_dense1_product,
    .graded_product = binary_dense1_gradeproduct,
};

static PyMultivectorMath_Funcs blades1_math_funcs = {
    .atomic_add = atomic_blades1_add,
    .atomic_product =  atomic_blades1_product,
    .add =  binary_blades1_add,
    .product =  binary_blades1_product,
    .grade_project =  unary_blades1_gradeproject,
    .scalar_product = binary_blades1_scalarproduct,
    .scalar_add = binary_blades1_scalaradd,
    .reverse = unary_blades1_reverse,
    .dual = unary_blades1_dual,
    .undual = unary_blades1_undual,
    .ternary_product = ternary_blades1_product,
    .graded_product = binary_blades1_gradeproduct,
};


static PyMultivectorData_Funcs dense1_data_funcs = {
  .iter_next = dense1_iternext,
  .iter_init = dense1_iterinit,
  .init = dense1_init,
  .cast = cast_to_dense1,
};

static PyMultivectorData_Funcs blades1_data_funcs = {
  .iter_next = blades1_iternext,
  .iter_init = blades1_iterinit,
  .init = blades1_init,
  .cast = cast_to_blades1,
};


static const PyMultivectorSubType dense1_subtype = {
    .math_funcs = &dense1_math_funcs,
    .data_funcs = &dense1_data_funcs,
    .name = "3DCGA",
    .type_name = "dense1",
    .generated = 1,
    .metric = {1,1,1,1,-1,},
    .msize = 5,
    .ntype = 5,
    .asize = 32,
    .basic_size = sizeof(gen1_DenseMultivector),
};

static const PyMultivectorSubType blades1_subtype = {
    .math_funcs = &blades1_math_funcs,
    .data_funcs = &blades1_data_funcs,
    .name = "3DCGA",
    .type_name = "blades1",
    .generated = 1,
    .metric = {1,1,1,1,-1,},
    .msize = 5,
    .ntype = 6,
    .asize = 32,
    .basic_size = sizeof(gen1_BladesMultivector),
};


// PyMultivectorSubType gen_subtypes_array[4] = {
//////   dense0_subtype,
////   blades0_subtype,
////////   dense1_subtype,
////   blades1_subtype,
////};

 PyMultivectorSubType gen_subtypes_array[4] = {  {
    .math_funcs = &dense0_math_funcs,
    .data_funcs = &dense0_data_funcs,
    .name = "3DVGA",
    .type_name = "dense0",
    .generated = 1,
    .metric = {1,1,1,},
    .msize = 3,
    .ntype = 3,
    .asize = 8,
    .basic_size = sizeof(gen0_DenseMultivector),
},  {
    .math_funcs = &blades0_math_funcs,
    .data_funcs = &blades0_data_funcs,
    .name = "3DVGA",
    .type_name = "blades0",
    .generated = 1,
    .metric = {1,1,1,},
    .msize = 3,
    .ntype = 4,
    .asize = 8,
    .basic_size = sizeof(gen0_BladesMultivector),
},  {
    .math_funcs = &dense1_math_funcs,
    .data_funcs = &dense1_data_funcs,
    .name = "3DCGA",
    .type_name = "dense1",
    .generated = 1,
    .metric = {1,1,1,1,-1,},
    .msize = 5,
    .ntype = 5,
    .asize = 32,
    .basic_size = sizeof(gen1_DenseMultivector),
},  {
    .math_funcs = &blades1_math_funcs,
    .data_funcs = &blades1_data_funcs,
    .name = "3DCGA",
    .type_name = "blades1",
    .generated = 1,
    .metric = {1,1,1,1,-1,},
    .msize = 5,
    .ntype = 6,
    .asize = 32,
    .basic_size = sizeof(gen1_BladesMultivector),
},};

PyMultivectorMixedMath_Funcs gen_multivector_mixed_fn = {
  .add = NULL,
  .product = NULL,
  .atomic_add = NULL,
  .atomic_product = NULL,
  .type_names = {"dense0","blades0","dense1","blades1",NULL},
};