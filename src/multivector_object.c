#include "multivector_object.h"
#include "multivector_types.h"
#include "common.h"
#include "listobject.h"
#include "pyerrors.h"
#include "pyport.h"
#include "pytypedefs.h"
#include "types.h"
#include <string.h>



// Multivector Array Initializers
static PyMvObject *init_multivector_array(PyAlgebraObject *GA, Py_ssize_t ndims, Py_ssize_t *strides, Py_ssize_t *shapes){
	if(!GA) return NULL; 
    PyMvObject *array_obj = (PyMvObject*)PyMem_RawMalloc(sizeof(PyMvObject));
    if(!array_obj) return NULL;

    // Copy strides and copy shapes if not null
    if(shapes != NULL){
        array_obj->shapes = (Py_ssize_t*)PyMem_RawMalloc(ndims*sizeof(Py_ssize_t));
        for(Py_ssize_t i = 0; i < ndims; i++)
            array_obj->shapes[i] = shapes[i];
    }else
        array_obj->shapes = NULL;
    if(strides != NULL){
        array_obj->strides = (Py_ssize_t*)PyMem_RawMalloc((ndims+1)*sizeof(Py_ssize_t));
        for(Py_ssize_t i = 0; i < ndims + 1; i++)
            array_obj->strides[i] = strides[i];
    }else
        array_obj->strides = NULL;

	array_obj->data = NULL;
	array_obj->ns = ndims;
	// set type and increase reference count
    array_obj->GA = GA;
    // Set the mixed type operations table
    array_obj->mixed = GA->mixed;

    Py_XINCREF((PyObject*)array_obj->GA);
	Py_SET_TYPE(array_obj, &PyMultivectorType);
	Py_SET_REFCNT((PyObject*)array_obj,1);

	return array_obj;
}

static int alloc_mvarray_data(PyMultivectorObject *obj){
    // Alloc memory for the data
    if(obj->strides != NULL){
        obj->data = (void*)PyMem_RawMalloc(obj->strides[0]*obj->type->basic_size);
        if(!obj->data) return 0;

        //Initializing each multivector to NULL otherwise freeing can cause trouble
        gainitfunc init = obj->type->data_funcs->init;
        if(!init) return 0;
        for(Py_ssize_t i = 0; i < obj->strides[0]; i++){
            if(!init(obj->data + i*obj->type->basic_size,obj->GA,NULL,NULL,0))
                return 0;
        }
    }
    return 1;
}

PyMultivectorObject *new_multivector_array(PyAlgebraObject *GA, char *type,  Py_ssize_t ndims, Py_ssize_t *strides, Py_ssize_t *shapes){
    PyMultivectorObject *self = init_multivector_array(GA, ndims, strides, shapes);
    if(!self) return NULL;
    if(!get_multivector_type_table(GA, type, &self->type)) return NULL;
    if(!alloc_mvarray_data(self)) return NULL;
    return self;
}

PyMultivectorObject *new_mvarray_inherit_type(PyAlgebraObject *GA,  Py_ssize_t ndims, Py_ssize_t *strides, Py_ssize_t *shapes, PyMultivectorSubType *type){
    PyMultivectorObject *self = init_multivector_array(GA, ndims, strides, shapes);
    if(!self) return NULL;
    self->type = type;
    if(!alloc_mvarray_data(self)) return NULL;
    return self;
}

// Single multivector initializers and destructors
PyMultivectorObject *init_multivector(PyAlgebraObject *GA){
    // Initializes a single multivector, also allocs memory for the data
    Py_ssize_t strides = 1;
    PyMultivectorObject *self = init_multivector_array(GA,0,&strides,NULL);
    if(!self) return NULL;
    return self;
}

PyMultivectorObject *new_multivector(PyAlgebraObject *GA, char *type){
    PyMultivectorObject *self = init_multivector(GA);
    if(!self) return NULL;
    if(!get_multivector_type_table(GA, type, &self->type)) return NULL;
    if(!alloc_mvarray_data(self)) return NULL;
    return self;
}

PyMultivectorObject *new_multivector_inherit_type(PyAlgebraObject *GA, PyMultivectorSubType *type){
    PyMultivectorObject *self = init_multivector(GA);
    if(!self || !type) return NULL;
    self->type = type;
    // Can only allocate memory after type is set
    if(!alloc_mvarray_data(self)) return NULL;
    return self;
}

// Creates a multivector array of the same shape as the mvarray
PyMultivectorObject *new_mvarray_from_mvarray(PyMvObject *mvarray){
    PyMultivectorObject *self = init_multivector_array(mvarray->GA, mvarray->ns, mvarray->strides, mvarray->shapes);
    if(!self) return NULL;
    self->type = mvarray->type;
    if(!alloc_mvarray_data(self)) return NULL;
    return self;
}

static void multivector_array_dealloc(PyMvObject *self){
	void *data = self->data;
    gafreefunc free_type = self->type->data_funcs->free;
    if(free_type){
        for(Py_ssize_t i = 0; i < self->strides[0]; i++){
            free_type(self->data + i*self->type->basic_size);
        }
    }

	Py_XDECREF((PyObject*)self->GA);
	PyMem_RawFree(self->strides);
	PyMem_RawFree(self->shapes);
	PyMem_RawFree(data);
	PyMem_RawFree(self);
}

PyMultivectorIter *init_multivector_array_iters(PyMvObject *self){
    PyMultivectorIter *iters = (PyMultivectorIter*)PyMem_RawMalloc(self->strides[0]*sizeof(PyMultivectorIter));
    gaiterinitfunc iter_init = self->type->data_funcs->iter_init;
    for(Py_ssize_t i = 0; i < self->strides[0]; i++){
        iters[i] = iter_init(self->data + i*self->type->basic_size, self->type);
    }
    return iters;
}

void free_multivector_array_iter(PyMultivectorIter *iters, Py_ssize_t size){
    for(Py_ssize_t i = 0; i < size; i++){
        PyMem_RawFree(iters[i].index);
    }
    PyMem_RawFree(iters);
}

int cast_mvarray(PyMvObject *from, PyMvObject *to){
    PyMultivectorIter *iters = init_multivector_array_iters(from);
    gacastfunc cast = to->type->data_funcs->cast;
    for(Py_ssize_t i = 0; i < from->strides[0]; i++){
        if(!cast(iters+i,to->data + i*to->type->basic_size,to->GA)){
            free_multivector_array_iter(iters,from->strides[0]);
            return 0;
        }
    }
    free_multivector_array_iter(iters,from->strides[0]);
    return 1;
}

PyMvObject *cast_mvarray_inherit_type(PyMvObject *from, PyMultivectorSubType *type){
    PyMvObject *to = new_mvarray_inherit_type(from->GA,from->ns,from->strides,from->shapes,type);
    if(!cast_mvarray(from,to)){
        multivector_array_dealloc(to);
        return NULL;
    }
    return to;
}

PyMvObject *cast_mvarray_to_type(PyMvObject *from, char *type){
    PyMvObject *to = new_multivector_array(from->GA,type,from->ns,from->strides,from->shapes);
    if(!cast_mvarray(from,to)){
        multivector_array_dealloc(to);
        return NULL;
    }
    return to;
}


/* Multivector array functions */
static Py_ssize_t get_ndims(PyObject *data){
    Py_ssize_t ndims = 0;
    PyObject *sublist = data;
    while(PyList_Check(sublist)){
        if((sublist = PyList_GetItem(sublist,0)) == NULL) break; // Break on empty list
        ndims++;
    }
    return ndims;
}

static Py_ssize_t *get_shapes(PyObject *data, Py_ssize_t ndims){
    Py_ssize_t *shapes = (Py_ssize_t*)PyMem_RawMalloc(ndims*sizeof(Py_ssize_t));
    if(!shapes) return NULL;
    PyObject *sublist = data;
    Py_ssize_t i = 0;
    while(PyList_Check(sublist)){
        shapes[i] = PyList_Size(sublist);
        if(!shapes[i]) break;
        sublist = PyList_GetItem(sublist,0);
        i++;
        if(i >= ndims) break;
    }
    return shapes;
}

static int iterate_nested_lists(PyObject *list,
                                ga_float **array, 
                                Py_ssize_t *strides,
                                Py_ssize_t *shape,
                                Py_ssize_t index, 
                                Py_ssize_t dim,
								Py_ssize_t ndims,
                                Py_ssize_t nbasis){
    
    if(PyList_Size(list) != shape[dim]) return -1; // Not the right shape
    if(!PyList_Check(list)) return -1; // Deepest nested element 
    
    if(dim == ndims){
        Py_ssize_t size = parse_list_as_values(list, &array[index]);
        if(size <= 0 && size != nbasis) return -1;
    }else{
        for(Py_ssize_t i = 0; i < (Py_ssize_t)PyList_Size(list); i++){
			PyObject *sublist = PyList_GetItem(list, i);
			int flag = iterate_nested_lists(sublist, array, strides, shape, index + i*strides[dim+1], dim+1,ndims,nbasis);
			if(flag == -1) return -1;
		}
    }

    return 0;
}

static Py_ssize_t *get_strides(Py_ssize_t ndims, Py_ssize_t *shapes){
    Py_ssize_t *strides = (Py_ssize_t*)PyMem_RawMalloc((ndims+1)*sizeof(Py_ssize_t));
    if(!strides) return NULL;
    strides[ndims] = 1;

    for(Py_ssize_t i = ndims; i >= 1; i--)
        strides[i-1] = strides[i]*shapes[i-1];
    return strides;
}


int multiple_arrays_iter_next(PyMultipleArrayIter *iter){
    iter->index[iter->ns-1]++;
    for(Py_ssize_t i = iter->ns-1; i >= 0 && iter->index[i] >= iter->shapes[i]; i--){
        if(i == 0)
            return 0; // last iteration
        iter->index[i] = 0;
        iter->index[i-1]++;
        iter->dim = i-1;
    }

    for(Py_ssize_t j = 0; j < iter->nm; j++){
        void *data = iter->arrays[j].data0;
        Py_ssize_t index = 0;
        for(Py_ssize_t i = 0; i < iter->ns; i++)
            index += iter->arrays[j].strides[i+1]*iter->index[i];
        iter->arrays[j].data = data+index*iter->arrays[j].basic_size;
    }
    return 1;
}

static int set_listsoflists_element(PyObject *element, PyObject *list, Py_ssize_t *index, Py_ssize_t *shape, Py_ssize_t size){
    PyObject *sublist = list;
    PyObject *subsublist;
    for(Py_ssize_t i = 0; i < size-1; i++){ 
        subsublist = PyList_GetItem(sublist, index[i]);
        if(subsublist == NULL){ // when empty list 
            // Create list at index[i]
            subsublist = PyList_New(shape[i+1]); 
            PyList_SetItem(sublist,index[i],subsublist);
        }
        sublist = subsublist;
    }
    PyList_SetItem(sublist,index[size-1],element); // Last index
    return 1;
}


static PyMultipleArrayIter init_single_array_iter(PyMvObject *self){
    PyMultipleArrayIter iter;
    iter.arrays = (PyMvBasicArray*)PyMem_RawMalloc(sizeof(PyMvBasicArray));
    iter.arrays->data = self->data;
    iter.arrays->data0 = self->data;
    iter.arrays->strides = (Py_ssize_t*)PyMem_RawMalloc((self->ns+1)*sizeof(Py_ssize_t));
    
    for(Py_ssize_t i = 0; i < self->ns + 1; i++)
        iter.arrays->strides[i] = self->strides[i];
    
    iter.arrays->ns = self->ns;
    iter.arrays->basic_size = self->type->basic_size;
    iter.nm = 1; // The number of arrays
    iter.shapes = self->shapes;
    iter.index = (Py_ssize_t*)PyMem_RawMalloc(self->ns*sizeof(Py_ssize_t));
    for(Py_ssize_t i = 0; i < self->ns; i++){
        iter.index[i] = 0;
    }
    iter.dim = -1;
    iter.ns = self->ns;
    
    return iter;
}

static void free_multiple_arrays_iter(PyMultipleArrayIter iter){
    for(Py_ssize_t i = 0; i < iter.nm; i++)
        PyMem_RawFree(iter.arrays[i].strides);
    
    PyMem_RawFree(iter.arrays);
    PyMem_RawFree(iter.index);
}

static int get_value_bitmap_from_mv(PyMultivectorObject *data, ga_float *value, int *bitmap) {
	// Takes a multivector and writes the first non-zero element into value and bitmap

	PyMultivectorIter *iter = init_multivector_iter(data, 1);
	if (!iter)
		return -1;
	while(iter->next(iter)){
		*value = iter->value;
		*bitmap = iter->bitmap;
		if(*value != 0.0)
			break;
	}
	
	free_multivector_iter(iter, 1);
	return 1;
}

int parse_list_as_multivectors(PyObject *basis, ga_float **values, int **bitmaps){
    if (!PyList_Check(basis))
		return -1;
	Py_ssize_t size = PyList_Size(basis);
    *bitmaps = (int *)PyMem_RawMalloc(size * sizeof(int));
	*values = (ga_float *)PyMem_RawMalloc(size * sizeof(ga_float));
    for (Py_ssize_t i = 0; i < size; i++) {
		PyObject *basis_i = PyList_GetItem(basis, i);

		if (Py_IS_TYPE(basis_i, &PyMultivectorType)) {
			if (!get_value_bitmap_from_mv((PyMvObject *)basis_i,&(*values)[i], &(*bitmaps)[i])) {
				PyMem_RawFree(*bitmaps);
				PyMem_RawFree(*values);
				return -1;
			}
        }else if (PyFloat_Check(basis_i)) {
			(*values)[i] = (ga_float)PyFloat_AsDouble(basis_i);
			(*bitmaps)[i] = 0;
		} else if (PyLong_Check(basis_i)) {
			(*values)[i] = (ga_float)PyLong_AsLong(basis_i);
			(*bitmaps)[i] = 0;
		} else {
			PyMem_RawFree(*bitmaps);
			PyMem_RawFree(*values);
			return -1;
		}
	}
	return size;
}

// Parses a list of lists and converts it to an array of specified types
PyObject *algebra_multivector(PyAlgebraObject *self, PyObject *args, PyObject *kwds) {
	static char *kwlist[] = {"values", "basis", "grades","dtype", NULL};
	PyObject *values = NULL, *basis = NULL, *grades = NULL;
	int *bitmaps_int = NULL;
	int *grades_int = NULL;
	ga_float *values_float = NULL;
	ga_float *values_basis = NULL;
	Py_ssize_t size, bsize;
	char *type_name = NULL;

	if (!PyArg_ParseTupleAndKeywords(args, kwds, "O|OOs", kwlist, &values, &basis, &grades, &type_name))
		return NULL;

	if (!values)
		PyErr_SetString(PyExc_ValueError, "Values must be non empty");

	if (grades && basis) {
		PyErr_SetString(PyExc_ValueError, "Can only define multivectors through "
										  "basis blades or grades, not both!");
		return NULL;
	}

    int basis_is_mv = 0;
	
	if (basis) {
        // Defining multivectors through some basis
		if (!PyList_Check(basis)) {
			PyErr_SetString(PyExc_ValueError,"Basis must be a list and of the same size as values");
			return NULL;
		}
        bsize = parse_list_as_multivectors(basis, &values_basis, &bitmaps_int);
		if (bsize <= 0){
            bsize = parse_list_as_bitmaps(basis, &bitmaps_int);
                if(bsize <= 0) {
                    PyErr_SetString(PyExc_TypeError, "Error parsing basis list as bitmaps");
                    return NULL;
                }
        }else
            basis_is_mv = 1;
        
	} else if (grades) {
		
		Py_ssize_t gsize = parse_list_as_grades(self, grades, &grades_int);
		if (gsize <= 0) {
			PyMem_RawFree(values_float);
			PyErr_SetString(PyExc_ValueError,"Error parsing grades, invalid value or empty");
			return NULL;
		}
		Py_ssize_t mv_size =
				parse_list_as_basis_grades(*self, grades_int, &bitmaps_int, gsize);

        bsize = mv_size;
		PyMem_RawFree(grades_int);

	} else {
		// Defining a multivector through the whole algebra basis
		size = self->asize;
		bitmaps_int = (int *)PyMem_RawMalloc(size * sizeof(int));
		for (int i = 0; i < size; i++)
			bitmaps_int[i] = i;
        bsize = size;
	}
	if(!type_name)
		type_name = self->mdefault.type_name;
    
    // Determine strides and shapes from the nested lists
    Py_ssize_t ndims = get_ndims(values);
    if(ndims <= 0){
        PyErr_SetString(PyExc_ValueError,"Error geting dims");
        return NULL;
    }
    Py_ssize_t *shapes = get_shapes(values,ndims);
    if(!shapes){
        PyErr_SetString(PyExc_ValueError,"Error geting shapes");
        return NULL;
    }
    Py_ssize_t *strides = get_strides(ndims-1,shapes);
    if(!strides){
        PyErr_SetString(PyExc_ValueError,"Error geting strides");
        return NULL;
    }
    
    ga_float **values_float_array = (ga_float**)PyMem_RawMalloc(strides[0]*sizeof(ga_float*));
    
    // Get the values from the nested list
    if(iterate_nested_lists(values,values_float_array,strides,shapes,0,0,ndims-1,bsize) == -1) {
        PyErr_SetString(PyExc_ValueError,"Error iterating nested lists, shape of the list might not be correct!!!");
        return NULL;
    }

    // Copies shapes up to i=ndims-2. Copies strides up to i=ndims-1
    PyMvObject *mv_array = new_multivector_array(self,type_name,ndims-1,strides,shapes);
    if(!mv_array){
        PyErr_SetString(PyExc_ValueError,"Error creating new multivector array");
        return NULL;
    }
    
    gainitfunc init = mv_array->type->data_funcs->init;
    if (!init){
        PyMem_RawFree(values_float_array);
        PyMem_RawFree(bitmaps_int);
        PyErr_SetString(PyExc_NotImplementedError,"The initializer for this type is not implemented");
        return NULL; // raise not implemented error
    }

    // If the basis is a multivector list multiply each basis element by the values array. 
    if(basis_is_mv){
        for(Py_ssize_t i = 0; i < strides[0]; i++){
            for(Py_ssize_t j = 0;j < bsize; j++){
                values_float_array[i][j] *= values_basis[j];
            }
        }
        PyMem_RawFree(values_basis);
    }
    
    // Write the data into the multivector array
    for(Py_ssize_t i = 0; i < strides[0]; i++){
        if(!init(mv_array->data + i*mv_array->type->basic_size,self,bitmaps_int,values_float_array[i],bsize)){
            // dealloc the memory from all the initialized objects
            multivector_array_dealloc(mv_array);
            PyErr_SetString(PyExc_ValueError,"Error initializing a single multivector!");
            mv_array = NULL;
            break;
        }
    }
    
    for(Py_ssize_t i = 0; i < strides[0]; i++)
        PyMem_RawFree(values_float_array[i]);
    PyMem_RawFree(values_float_array);
    PyMem_RawFree(bitmaps_int);
    PyMem_RawFree(shapes);
    PyMem_RawFree(strides);

    return (PyObject*)mv_array;
}



char *type_iter_repr(PyMultivectorIter *iter, PrintTypeMV ptype, Py_ssize_t dsize){
    char *out_str;
    if(ptype == PrintTypeMV_reduced){
        if(dsize){
            char **str_blade = (char**)PyMem_RawMalloc(dsize*sizeof(char*));
            Py_ssize_t len = 0;
            char sep[] = " + ";

            Py_ssize_t i = 0;
            while(iter->next(iter)){
                // only skip small values if its not sparse or blades type
                if(iter->type != MultivectorType_sparse && iter->type != MultivectorType_blades)
                    // maybe should add this as an option when creating the algebra
                    if(ABS(iter->value) < 1e-12) continue; // don't print small values

                char *value = PyOS_double_to_string((double)iter->value,'g',16,0,NULL);
                if(iter->bitmap){
                    char *bitmap = bitmap_to_string(iter->bitmap);
                    Py_ssize_t size = strlen(bitmap) + strlen(value)+3;
                    str_blade[i] = (char*)PyMem_RawMalloc(size*sizeof(char));
                    PyOS_snprintf(str_blade[i],size,"%s^%s",value,bitmap);
                    PyMem_RawFree(bitmap);
                    PyMem_Free(value);
                }else{
                    Py_ssize_t size = strlen(value) + 1;
                    str_blade[i] = (char*)PyMem_RawMalloc(size*sizeof(char));
                    strcpy(str_blade[i],value);
                    PyMem_Free(value);
                }
                len += strlen(str_blade[i]);
                i++;
            }
            if(iter->type != MultivectorType_sparse && iter->type != MultivectorType_blades)
                dsize = i;
            if(!dsize){
                PyMem_RawFree(str_blade);
                out_str = PyMem_RawMalloc(4*sizeof(char));
                out_str[3] = '\0';
                strcpy(out_str,"0.0");
                return out_str;
            }
            len += dsize*3 + 3;
            out_str = (char*)PyMem_RawMalloc(len*sizeof(char));

            Py_ssize_t j = 0;
            for(i = 0; i < dsize-1; i++){
                strcpy(out_str + j, str_blade[i]);
                j += strlen(str_blade[i]);
                strcpy(out_str + j, sep);
                j += strlen(sep);
            }
            strcpy(out_str + j, str_blade[i]);
            j += strlen(str_blade[i]);

            for(Py_ssize_t i = 0; i < dsize; i++)
                PyMem_RawFree(str_blade[i]);
            PyMem_RawFree(str_blade);

            return out_str;
        }else{ 
            out_str = PyMem_RawMalloc(4*sizeof(char));
            out_str[3] = '\0';
            strcpy(out_str,"0.0");
            return out_str;
        }
    }else if(ptype == PrintTypeMV_normal){
        if(dsize){
            char **str_bitmap = (char**)PyMem_RawMalloc(dsize*sizeof(char*));
            char **str_value =  (char**)PyMem_RawMalloc(dsize*sizeof(char*));
            Py_ssize_t len_bitmap = 0, len_value = 0;
            Py_ssize_t i = 0;
            while(iter->next(iter)){
                str_bitmap[i] = bitmap_to_string(iter->bitmap);
                len_bitmap += strlen(str_bitmap[i]);
                str_value[i] = PyOS_double_to_string((double)iter->value,'g',6,0,NULL);
                len_value += strlen(str_value[i]);
                i++;
            }
            len_value += dsize + 3;
            len_bitmap += dsize + 3;
            char *format_value = (char*)PyMem_RawMalloc((len_value)*sizeof(char));
            char *format_bitmap = (char*)PyMem_RawMalloc((len_bitmap)*sizeof(char));

            Py_ssize_t j = 0, k = 0; i = 0;
            for(i = 0; i < dsize - 1; i++){
                strcpy(format_bitmap + j, str_bitmap[i]);
                j += strlen(str_bitmap[i]);
                format_bitmap[j++] = ',';

                strcpy(format_value + k, str_value[i]);
                k += strlen(str_value[i]);
                format_value[k++] = ',';
            }
            strcpy(format_bitmap + j, str_bitmap[i]);
            j += strlen(str_bitmap[i]);
            format_bitmap[j] = '\0';


            strcpy(format_value + k, str_value[i]);
            k += strlen(str_value[i]);
            format_value[k] = '\0';

            char *format = ".multivector([%s],blades=[%s],dtype=%s)";
            size_t size = len_value + len_bitmap + strlen(format);
            char *format_out = (char*)PyMem_RawMalloc(size*sizeof(char));

            PyOS_snprintf(format_out,size,format,format_value,format_bitmap,iter->type_name);

            for(Py_ssize_t i = 0; i < dsize; i++){
                PyMem_RawFree(str_bitmap[i]);
                PyMem_Free(str_value[i]);
            }
            PyMem_RawFree(str_bitmap);
            PyMem_RawFree(str_value);
            PyMem_RawFree(format_value);
            PyMem_RawFree(format_bitmap);
            return format_out;
        }else {
            const char* zero = ".multivector(0.0)";
            out_str = PyMem_RawMalloc((strlen(zero)+1)*sizeof(char));
            out_str[strlen(zero)] = '\0';
            strcpy(out_str,zero);
            return out_str;
        };
    }
    return NULL; // raise error
}

/*
  Method calls of PyMultivectorType:
    print(a) -> multivector_repr(self): prints the multivector
    a(2)     -> multivector_grade_project(self): grade projection
  Method calls for the number protocol:
    a*b -> multivector_geometric_product: geometric product
    a^b -> multivector_outer_product: wedge product
    a|b -> multivector_inner_product: inner product
    a+b -> multivector_add: multivector addition
    a-b -> multivector_subtract: multivector subtraction
    ~a  -> multivector_invert: reverse the order of the basis vector by changing the sign
*/

// Prints the array of multivectors as a list of lists
PyObject *multivector_repr(PyMvObject *self){
    PrintTypeMV ptype = self->GA->print_type_mv;
    PyObject *out = Py_BuildValue("s","");
    char *mv_str;
    char out_str[1000000];
    out_str[0] = '\0';

    const char *lbracket = "[";
    //const char *rbracket_nl = "],\n";
    const char *rbracket = "]";

    Py_ssize_t last_dim = -9;

    for(Py_ssize_t i = 0; i < self->ns; i++)
        strcat(out_str,lbracket);

    gaiterinitfunc iter_init = self->type->data_funcs->iter_init;

    if(self->ns > 0){
        PyMultipleArrayIter arr_iter = init_single_array_iter(self);
        do{            
            
            Py_ssize_t index = 0;
            for(Py_ssize_t i = 0; i < arr_iter.ns; i++)
                index += self->strides[i+1]*arr_iter.index[i];

            PyMultivectorIter iter = iter_init(arr_iter.arrays->data,self->type);
            mv_str = type_iter_repr(&iter,ptype,iter.niters);
            
            if(last_dim != arr_iter.dim && last_dim != -9){
                out_str[strlen(out_str) - 1] = ']';
                for(Py_ssize_t i = 0; i < arr_iter.ns - arr_iter.dim-2; i++)
                    strcat(out_str,rbracket);
                strcat(out_str,",");
                strcat(out_str,"\n");
                strcat(out_str,lbracket);
                for(Py_ssize_t i = 0; i < arr_iter.ns - arr_iter.dim-2; i++)
                    strcat(out_str,lbracket);
            }
            strcat(out_str,lbracket);
            strcat(out_str,mv_str);
            strcat(out_str,rbracket);
            strcat(out_str,",");
            strcat(out_str,"\n");
            PyMem_RawFree(mv_str);
            PyMem_RawFree(iter.index);
            
            last_dim = arr_iter.dim;
        }while(multiple_arrays_iter_next(&arr_iter));
        free_multiple_arrays_iter(arr_iter);

        out_str[strlen(out_str) - 2] = ']';
        out_str[strlen(out_str) - 1] = '\0';
        for(Py_ssize_t i = 0; i < self->ns - 1; i++)
           strcat(out_str,rbracket);
        //strcat(out_str,")");
    }else{
        PyMultivectorIter iter = iter_init(self->data,self->type);
        mv_str = type_iter_repr(&iter,ptype,iter.niters);
        strcpy(out_str,mv_str);
        PyMem_RawFree(mv_str);
        PyMem_RawFree(iter.index);
    }

    out = Py_BuildValue("s",out_str);

    if(ptype == PrintTypeMV_normal){
        PyErr_SetString(PyExc_NotImplementedError, "Not implemented for multivector arrays");
        return NULL;
    }else if(ptype == PrintTypeMV_reduced){
        return out;
    }else {
        PyErr_SetString(PyExc_ValueError,"The selected print type is not valid");
        return NULL;
    }
}

static int get_scalar(PyObject *self, ga_float *value){
    if(PyFloat_Check(self)){
        *value = (ga_float)PyFloat_AsDouble(self);
        return 1;
    }
    if(PyLong_Check(self)){
        *value = (ga_float)PyLong_AsDouble(self);
        return 1;
    }
    return 0;
}


PyObject *list_from_mvarray(PyMvObject *dense, Py_ssize_t *grade_bool, Py_ssize_t size){
    gaiterinitfunc iter_init = dense->type->data_funcs->iter_init;
    PyMultipleArrayIter arr_iter = init_single_array_iter(dense);
    PyObject * values_list = PyList_New(dense->shapes[0]);

    do{
        PyMultivectorIter iter = iter_init(arr_iter.arrays->data,dense->type);
        Py_ssize_t j = 0;
        PyObject *element = PyList_New(size);
        while(iter.next(&iter)){
            if(grade_bool[GRADE(iter.bitmap)] && j < size){
                PyObject *value = PyFloat_FromDouble(iter.value);
                PyList_SetItem(element,j,value);
                j++;
            }
            
        } PyMem_RawFree(iter.index);
        set_listsoflists_element(element, values_list, arr_iter.index, arr_iter.shapes, arr_iter.ns);
       
    }while(multiple_arrays_iter_next(&arr_iter));

	free_multiple_arrays_iter(arr_iter);
    return values_list;
}

PyObject *grade_from_multivector(PyMultivectorIter iter){
    PyObject *element;
    int grade = -1;
    while(iter.next(&iter)){
            if(grade == -1){ //First iteration
                if(iter.value != 0.0)
                    grade = iter.grade;
            }else if(grade != iter.grade){
                if(iter.value != 0.0){
                    PyMem_RawFree(iter.index);
                    return PyLong_FromLong(-1);
                }
            }
        }if(grade == -1) grade = 0;
        element = PyLong_FromLong(grade);
        PyMem_RawFree(iter.index);
        return element;
}

PyObject *grade_list_from_mvarray(PyMvObject *self){
    gaiterinitfunc iter_init = self->type->data_funcs->iter_init;
    PyMultipleArrayIter arr_iter = init_single_array_iter(self);
    PyObject * values_list = PyList_New(self->shapes[0]);
    PyObject *element;
    do{
        PyMultivectorIter iter = iter_init(arr_iter.arrays->data,self->type);
        element = grade_from_multivector(iter);
        set_listsoflists_element(element, values_list, arr_iter.index, arr_iter.shapes, arr_iter.ns);
    }while(multiple_arrays_iter_next(&arr_iter));

	free_multiple_arrays_iter(arr_iter);
    return values_list;
}

PyObject* multivector_list(PyMvObject *self, PyObject *args, PyObject *kwds){
    static char *kwlist[] = {"grades","bitmap",NULL};
    PyObject *grades = NULL;
    int *grades_int = NULL;
    PyObject *list = NULL;
    PyObject *values_list = NULL;
    PyObject *bitmap = NULL;
    PyMultivectorObject *dense;
    PyMultivectorIter iter;
	Py_ssize_t *grade_bool = NULL;
	Py_ssize_t size;
    int free_mv = 1;

    int as_bitmap = 0;
    
    if(!PyArg_ParseTupleAndKeywords(args, kwds, "|Op", kwlist, &grades,&as_bitmap))
        return NULL;

    // Cast to dense if the multivector is not dense and is not a generated type
    if(!self->type->generated && strcmp(self->type->type_name,"dense")){

        dense = new_multivector_array(self->GA,"dense",self->ns,self->strides,self->shapes);

        if(!dense){
            PyErr_SetString(PyExc_TypeError,"Error populating types table");
            return NULL;
        }
        if(!cast_mvarray(self, dense)){
            PyErr_SetString(PyExc_TypeError,"Error casting the multivector");
            return NULL;    
        }
    }else {
        free_mv = 0;
        dense = self;
    }
    
    if(grades){
        size = parse_list_as_grades(self->GA, grades,&grades_int);
        if(size <= 0){
            PyErr_SetString(PyExc_TypeError, "Error parsing grades");
            return NULL;
        }

		grade_bool = get_grade_bool(grades_int, size, MAX_GRADE(self->GA) + 1);
        PyMem_RawFree(grades_int);
		size = self->GA->asize;
    	Py_ssize_t psize = 0;
    	for (Py_ssize_t i = 0; i < size; i++) {
      		if (grade_bool[GRADE(i)])
        		psize++;
    	}
		size = psize;
    }else{
        size = self->GA->asize;
        grade_bool  = (Py_ssize_t*)PyMem_RawMalloc((MAX_GRADE(self->GA) + 1)*sizeof(Py_ssize_t));
		for(Py_ssize_t i = 0; i < MAX_GRADE(self->GA) + 1; i++) 
			grade_bool[i] = 1;
    }
    if(dense->strides[0] == 1)
        values_list = PyList_New(size);

    bitmap = PyList_New(size);
    iter = dense->type->data_funcs->iter_init(dense->data,dense->type);
	
	Py_ssize_t j = 0;
	ga_float basis_value = 1;
    while(iter.next(&iter)){
        if(grade_bool[GRADE(iter.bitmap)] && j < size){
            if(dense->strides[0] == 1){
                PyObject *value = PyFloat_FromDouble(iter.value);
			    PyList_SetItem(values_list,j,value);
            }
			if(as_bitmap){
				PyObject *bitmap_obj = PyLong_FromLong(iter.bitmap);
            	PyList_SetItem(bitmap,j,bitmap_obj);
			}else{
				PyMultivectorObject *mv = new_multivector(self->GA,self->type->type_name);
				if(!mv){
					// free memory
					Py_XDECREF(list);
					Py_XDECREF(bitmap);
					PyErr_SetString(PyExc_TypeError, "Cannot populate the types");
					return NULL; 
				}
				gainitfunc init = mv->type->data_funcs->init;
				init(mv->data,self->GA,&iter.bitmap,&basis_value,1);
				PyList_SetItem(bitmap,j,(PyObject*)mv);
			}
			j++;
		}else if (j > size) {
        	break;
      	}
    }
    PyMem_RawFree(iter.index);

    
    if(dense->strides[0] > 1)
        values_list = list_from_mvarray(dense,grade_bool,size);
    

    PyObject *tuple = PyTuple_New(2);
    PyTuple_SetItem(tuple,0,values_list);
    PyTuple_SetItem(tuple,1,bitmap);
	PyMem_RawFree(grade_bool);
    if(free_mv)
        Py_XDECREF((PyObject*)dense);

    return tuple;
}

PyObject* multivector_grade(PyMultivectorObject *self, PyObject *Py_UNUSED(ignored)){
    PyObject *element = NULL;
    if(self->strides[0] == 1){
        PyMultivectorIter iter = self->type->data_funcs->iter_init(self->data,self->type);
        element = grade_from_multivector(iter);
    } else {
        element = grade_list_from_mvarray(self);
    }
    return element;
}


static PyMvObject *multivector_mixed_product(PyMvObject *left, PyMvObject *right, ProductType ptype, int isleft_single){
    int isleft_bigger = is_bigger_metric(left->GA,right->GA);
    PyAlgebraObject *GA = NULL;
    PyMvObject *out = NULL;
    Py_ssize_t left_inc = 1,right_inc = 1;
    Py_ssize_t size = -1;
    gamixedprodfunc mixed_product = NULL;

    if(isleft_bigger == -1){
        return NULL;
    }else {
        gaiterinitfunc iter_init_left = left->type->data_funcs->iter_init;
        gaiterinitfunc iter_init_right = right->type->data_funcs->iter_init;
        Py_ssize_t *strides, *shapes, ns;
        if(isleft_single == 1){
            strides = right->strides;
            shapes = right->shapes;
            ns = right->ns;
            size = right->strides[0];
            left_inc = 0;
        }else if(isleft_single == 0){
            strides = left->strides;
            shapes = left->shapes;
            ns = left->ns;
            size = left->strides[0];
            right_inc = 0;
        }else{
            strides = left->strides;
            shapes = left->shapes;
            ns = left->ns;
            size = left->strides[0];
        }
        
        if(isleft_bigger){
            out = new_multivector_array(left->GA, "sparse",ns,strides,shapes);
            GA = left->GA;
            mixed_product = left->mixed->product;
        }
        else{
            out = new_multivector_array(right->GA,"sparse",ns,strides,shapes);
            GA = right->GA;
            mixed_product = right->mixed->product;
        }

        if(mixed_product){
            for(Py_ssize_t i = 0; i < size; i++){
                PyMultivectorIter iter_left = iter_init_left(left->data + i*left->type->basic_size*left_inc,left->type);
                PyMultivectorIter iter_right = iter_init_right(right->data + + i*right->type->basic_size*right_inc,right->type);
                
                if(!mixed_product(out->data + i*out->type->basic_size,&iter_left,&iter_right,GA,ptype)){
                    return NULL;
                }
                PyMem_RawFree(iter_left.index);
                PyMem_RawFree(iter_right.index);
            }

            return out;
        }else 
            return NULL;
        
    }
} 

static PyMvObject *multivector_casttype_product(PyMvObject *left, PyMvObject *right, ProductType ptype, int isleft_single){
    PyMvObject *left_cast = NULL;
    PyMvObject *right_cast = NULL;
    PyMvObject *out = NULL;
    PyAlgebraObject *GA = NULL;
    gaprodfunc product = NULL;
    int cast_left = -1;
    Py_ssize_t left_inc = 1,right_inc = 1;
    Py_ssize_t size = -1;

    Py_ssize_t *strides, *shapes, ns;
    if(isleft_single == 1){ // The left mvarray is only one mv
        strides = right->strides;
        shapes = right->shapes;
        ns = right->ns;
        size = right->strides[0];
        left_inc = 0; // Do not increment the left array indices
    }else if(isleft_single == 0){ // The right mvarray is only one mv
        strides = left->strides;
        shapes = left->shapes;
        ns = left->ns;
        size = left->strides[0];
        right_inc = 0; // Do not increment the right array indices
    }else{ // Same shape mvarrays
        strides = left->strides;
        shapes = left->shapes;
        ns = left->ns;
        size = left->strides[0];
    }

    if(left->type->generated || right->type->generated){
        int isleft_bigger = is_bigger_metric(left->GA,right->GA);
        if(isleft_bigger == -1)
           return NULL;
        if(isleft_bigger){
            cast_left = 0;
            left_cast = left;
            right_cast = cast_mvarray_inherit_type(right,left->type);
            GA = left->GA;
            product = left->type->math_funcs->product;
            out = new_mvarray_inherit_type(left->GA, ns, strides, shapes, left->type);
        }else{
            cast_left = 1;
            left_cast = cast_mvarray_inherit_type(left,right->type);
            right_cast = right;
            GA = right->GA;
            product = right->type->math_funcs->product;
            out = new_mvarray_inherit_type(right->GA, ns, strides, shapes, right->type);
        }
    }else if(left->GA == right->GA){
        left_cast = left;
        right_cast = right;
        GA = left->GA;
        product = left->type->math_funcs->product;
        out = new_mvarray_inherit_type(left->GA, ns, strides, shapes, left->type);
    }else goto failure;

    for(Py_ssize_t i = 0; i < size; i++){
        if(!product(out->data + i*out->type->basic_size,
                    left_cast->data + i*left_cast->type->basic_size*left_inc,
                    right_cast->data + i*right_cast->type->basic_size*right_inc,GA,ptype))
                    goto failure;
    }
    goto success;

failure:
    multivector_array_dealloc(out);
    out = NULL;

success:
    if(cast_left != -1){
        if(cast_left)
            multivector_array_dealloc(left_cast);
        else
            multivector_array_dealloc(right_cast);
    }

    return out;
}

// Multiplication by a scalar
static PyMvObject* multivector_scalar_product(PyMvObject *data, ga_float scalar, ProductType ptype, int scalar_left){
    PyMvObject *out = NULL;
    PyMvObject *scalar_mv = NULL;
    gaprodfunc product = NULL; 
    gascalarfunc scalar_product = NULL;

    out = new_mvarray_inherit_type(data->GA, data->ns, data->strides, data->shapes, data->type);

    if(ptype == ProductType_inner){ // return 0 if inner product with scalar
        return out;
    }else if(ptype == ProductType_regressive){
        product = data->type->math_funcs->product;
        if(!product){
            multivector_array_dealloc(out);
            return NULL;
        }
        int bitmap = 0;
        scalar_mv = new_multivector_inherit_type(data->GA, data->type);
        scalar_mv->type->data_funcs->init(scalar_mv->data,data->GA,&bitmap,&scalar,1);
        if(scalar_left){
            for(Py_ssize_t i = 0; i < data->strides[0]; i++){
                if(!product(out->data + i*out->type->basic_size,scalar_mv->data,data->data + i*data->type->basic_size,data->GA,ptype)){
                    multivector_array_dealloc(out);
                    return NULL;
                }
            }
            
        } else{
            for(Py_ssize_t i = 0; i < data->strides[0]; i++){
                if(!product(out->data + i*out->type->basic_size,data->data + i*data->type->basic_size,scalar_mv->data,data->GA,ptype)){
                    multivector_array_dealloc(out);
                    return NULL;
                }
            }
        }
        multivector_array_dealloc(scalar_mv);
    }else{
        scalar_product = data->type->math_funcs->scalar_product;
        if(!scalar_product){
            multivector_array_dealloc(out);
            return NULL;
        }
        for(Py_ssize_t i = 0; i < data->strides[0]; i++){
            if(!scalar_product(out->data + i*out->type->basic_size,data->data + i*data->type->basic_size,data->GA,scalar)){
                multivector_array_dealloc(out);
                return NULL;
            }
        }
    }
    return out;
}

//Element wise multiplication by a scalar array
static PyMvObject* multivector_scalar_array_product(PyMvObject *left, PyMvObject *right){
    PyMvObject *out = NULL;
    PyMvObject *scalar_mv = NULL;
    gaprodfunc product = NULL; 
    gascalarfunc scalar_product = NULL;
    PyMvObject *data = NULL;
    PyMvObject *scalar = NULL;
    if(!strcmp("scalar",left->type->type_name)){
        scalar = left;
        data = right;
    }else if(!strcmp("scalar",right->type->type_name)){
        scalar = right;
        data = left;
    }else return NULL;

    out = new_mvarray_inherit_type(data->GA, data->ns, data->strides, data->shapes, data->type);
    
    scalar_product = data->type->math_funcs->scalar_product;
    if(!scalar_product){
        multivector_array_dealloc(out);
        return NULL;
    }
    for(Py_ssize_t i = 0; i < data->strides[0]; i++){
        ScalarMultivector *scalar_mv = INDEX_DATA(scalar, i);
        if(!scalar_product(INDEX_DATA(out, i),INDEX_DATA(data, i),data->GA,*scalar_mv)){
            multivector_array_dealloc(out);
            return NULL;
        }
    }
    
    return out;
}



static int compare_shapes(PyObject *data0, PyObject *data1){
    PyMvObject *mv0 = (PyMvObject*)data0;
    PyMvObject *mv1 = (PyMvObject*)data1;
    if(mv0->ns != mv1->ns) return 0;
    if(mv0->strides[0] != mv1->strides[0]) return 0;
    for(Py_ssize_t i = 0; i < mv0->ns; i++){
        if(mv0->shapes[i] != mv1->shapes[i]) return 0;
    }
    return 1;
}

static PyObject *multivector_product(PyObject *left, PyObject *right, ProductType ptype){
    PyMultivectorObject *data0 = NULL;
    PyMvObject *out = NULL;
    ga_float value = 0;
    int scalar_left = -1;
    int isleft_single = -1;
    
    if(get_scalar(right,&value)) // check if right is a scalar
        data0 = (PyMultivectorObject*)left,scalar_left = 1;
    else if(get_scalar(left,&value)) // check if left is a scalar
        data0 = (PyMultivectorObject*)right,scalar_left = 0;

    if(scalar_left != -1){ // One of the arguments is a scalar
        out = multivector_scalar_product(data0,value,ptype,scalar_left);
        if(!out){
            PyErr_SetString(PyExc_TypeError,"Something wrong computing the product with a scalar!");
            return NULL;
        }
        return (PyObject*)out;
    }

    if(!PyObject_TypeCheck(left,Py_TYPE(right))){
        PyErr_SetString(PyExc_TypeError,"Operands must be of the same type or int or ga_float");
        return NULL;
    }
    if(!compare_shapes(left, right)){
        if(((PyMvObject*)left)->strides[0] == 1)
            isleft_single = 1;
        else if(((PyMvObject*)right)->strides[0] == 1)
            isleft_single = 0;
        else{
            PyErr_SetString(PyExc_TypeError,"Multivector arrays must be of the same shape, or one of them is a single multivector");
            return NULL;
        }
    }
    PyMvObject *left_mv = (PyMultivectorObject*)left;
    PyMvObject *right_mv = (PyMultivectorObject*)right;
    if(!strcmp("scalar",left_mv->type->type_name) || !strcmp("scalar",right_mv->type->type_name)){
        out = multivector_scalar_array_product(left_mv,right_mv);
        if(!out){
            PyErr_SetString(PyExc_TypeError,"Error taking the product with a scalar array!");
            return NULL;
        }
        return (PyObject*)out;
    }

    if(left_mv->GA == right_mv->GA || left_mv->type->generated || right_mv->type->generated)
        out = multivector_casttype_product(left_mv, right_mv, ptype,isleft_single);
    else 
        out = multivector_mixed_product(left_mv, right_mv, ptype,isleft_single);
    
    if(!out){
        PyErr_SetString(PyExc_TypeError,"Probably Incompatible Algebras!");
        return NULL;
    }
    return (PyObject*)out;
}

static PyMvObject* multivector_mixed_addsubtract(PyMvObject *left, PyMvObject *right, int sign, int isleft_single){
    int isleft_bigger = is_bigger_metric(left->GA,right->GA);
    PyAlgebraObject *GA = NULL;
    PyMvObject *out = NULL;
    Py_ssize_t left_inc = 1,right_inc = 1;
    Py_ssize_t size = left->strides[0];
    gamixedaddfunc mixed_add = NULL;

    if(isleft_bigger == -1){
        return NULL;
    }else {
        gaiterinitfunc iter_init_left = left->type->data_funcs->iter_init;
        gaiterinitfunc iter_init_right = right->type->data_funcs->iter_init;
        Py_ssize_t *strides, *shapes, ns;
        
        if(isleft_single == 1){
            strides = right->strides;
            shapes = right->shapes;
            ns = right->ns;
            size = right->strides[0];
            left_inc = 0;
        }else if(isleft_single == 0){
            strides = left->strides;
            shapes = left->shapes;
            ns = left->ns;
            size = left->strides[0];
            right_inc = 0;
        }else{
            strides = left->strides;
            shapes = left->shapes;
            ns = left->ns;
            size = left->strides[0];
        }

        if(isleft_bigger){
            out = new_multivector_array(left->GA,"sparse",ns,strides,shapes);
            GA = left->GA;
            mixed_add = left->mixed->add;
        }
        else{
            out = new_multivector_array(right->GA,"sparse",ns,strides,shapes);
            GA = right->GA;
            mixed_add = right->mixed->add;
        }

        if(isleft_single == 1) {
            size = right->strides[0];
            left_inc = 0;
        }
        else if(isleft_single == 0) {
            size = left->strides[0];
            right_inc = 0;
        }
        
        if(mixed_add){
            for(Py_ssize_t i = 0; i < size; i++){
                PyMultivectorIter iter_left = iter_init_left(left->data + i*left->type->basic_size*left_inc,left->type);
                PyMultivectorIter iter_right = iter_init_right(right->data + + i*right->type->basic_size*right_inc,right->type);
                
                if(!mixed_add(out->data + i*out->type->basic_size,&iter_left,&iter_right,GA,sign)){
                    return NULL;
                }
                PyMem_RawFree(iter_left.index);
                PyMem_RawFree(iter_right.index);
            }
            return out;
            
        }else 
            return NULL;
    }
}

static PyMvObject *multivector_casttype_addsubtract(PyMvObject *left, PyMvObject *right, int sign, int isleft_single){
    PyMvObject *left_cast = NULL;
    PyMvObject *right_cast = NULL;
    PyMvObject *out = NULL;
    PyAlgebraObject *GA = NULL;
    gaprodfunc add = NULL;
    int cast_left = -1;
    Py_ssize_t left_inc = 1,right_inc = 1;
    Py_ssize_t size = left->strides[0];

    Py_ssize_t *strides, *shapes, ns;
    if(isleft_single == 1){
        strides = right->strides;
        shapes = right->shapes;
        ns = right->ns;
        size = right->strides[0];
        left_inc = 0;
    }else if(isleft_single == 0){
        strides = left->strides;
        shapes = left->shapes;
        ns = left->ns;
        size = left->strides[0];
        right_inc = 0;
    }else{
        strides = left->strides;
        shapes = left->shapes;
        ns = left->ns;
        size = left->strides[0];
    }
    
    if(left->type->generated || right->type->generated){
        int isleft_bigger = is_bigger_metric(left->GA,right->GA);
        if(isleft_bigger == -1)
           return NULL;
        
        if(isleft_bigger){
            cast_left = 0;
            left_cast = left;
            right_cast = cast_mvarray_inherit_type(right,left->type);
            GA = left->GA;
            add = left->type->math_funcs->add;
            out = new_mvarray_inherit_type(left->GA, ns, strides, shapes, left->type);
        }else{
            cast_left = 1;
            left_cast = cast_mvarray_inherit_type(left,right->type);
            right_cast = right;
            GA = right->GA;
            add = right->type->math_funcs->add;
            out = new_mvarray_inherit_type(right->GA, ns, strides, shapes, right->type);
        }
    }else if(left->GA == right->GA){
        left_cast = left;
        right_cast = right;
        GA = left->GA;
        add = left->type->math_funcs->add;
        out = new_mvarray_inherit_type(left->GA, ns, strides, shapes, left->type);
    }else goto failure;

    if(isleft_single == 1) {
        size = right->strides[0];
        left_inc = 0;
    }
    else if(isleft_single == 0) {
        size = left->strides[0];
        right_inc = 0;
    }

    for(Py_ssize_t i = 0; i < size; i++){
        if(!add(out->data + i*out->type->basic_size,
                left_cast->data + i*left_cast->type->basic_size*left_inc,
                right_cast->data + i*right_cast->type->basic_size*right_inc,GA,sign))
            goto failure;
        
    }

    goto success;

failure:
    multivector_array_dealloc(out);
    out = NULL;

success:
    if(cast_left != -1){
        if(cast_left)
            multivector_array_dealloc(left_cast);
        else
            multivector_array_dealloc(right_cast);
    }

    return out;
}

// Addition by a scalar
static PyMvObject* multivector_scalar_add(PyMvObject *data, ga_float scalar, int sign){
    PyMvObject *out = NULL;
    gascalaraddfunc scalar_add = NULL;

    out = new_mvarray_inherit_type(data->GA, data->ns, data->strides, data->shapes, data->type);
    scalar_add = data->type->math_funcs->scalar_add;
    if(!scalar_add){
        multivector_array_dealloc(out);
        return NULL;
    }
    for(Py_ssize_t i = 0; i < data->strides[0]; i++){
        if(!scalar_add(out->data + i*out->type->basic_size,data->data + i*data->type->basic_size,data->GA,scalar,sign)){
            multivector_array_dealloc(out);
            return NULL;
        }
    }
    
    return out;
}

// Element wise addition by a scalar array
static PyMvObject* multivector_scalar_array_add(PyMvObject *left, PyMvObject *right, int sign){
    PyMvObject *data = NULL;
    PyMvObject *scalar = NULL;
    PyMvObject *out = NULL;
    gascalaraddfunc scalar_add = NULL;
    int sign_scalar = 1;
    int sign_data = 1;

    if(!strcmp("scalar",left->type->type_name)){
        data = (PyMultivectorObject*)right;
        scalar = (PyMultivectorObject*)left;
        sign_data = sign; // Multiply the multivectors by the sign
        
    }else if(!strcmp("scalar",right->type->type_name)){
        data = (PyMultivectorObject*)left;
        scalar = (PyMultivectorObject*)right;
        sign_scalar = sign; // Multiply the scalars by the sign
    }else return NULL;
    
    out = new_mvarray_inherit_type(data->GA, data->ns, data->strides, data->shapes, data->type);
    scalar_add = data->type->math_funcs->scalar_add;
    if(!scalar_add){
        multivector_array_dealloc(out);
        return NULL;
    }
    for(Py_ssize_t i = 0; i < data->strides[0]; i++){
        ScalarMultivector *scalar_mv = INDEX_DATA(scalar, i);
        if(!scalar_add(INDEX_DATA(out, i),INDEX_DATA(data, i),data->GA,sign_scalar*(*scalar_mv),sign_data)){
            multivector_array_dealloc(out);
            return NULL;
        }
    }
    
    return out;
}

static PyObject *multivector_add_subtract(PyObject *left, PyObject *right, int sign){
    PyMultivectorObject *data0 = NULL;
    PyMvObject *out = NULL;
    ga_float value = 0;
    int scalar_left = -1;
    int isleft_single = -1;
    
    if(get_scalar(right,&value)){ // check if right is a scalar
        data0 = (PyMultivectorObject*)left,scalar_left = 1;
        value *= sign; // multiply by sign
        sign = 1;
    }
    else if(get_scalar(left,&value)) // check if left is a scalar
        data0 = (PyMultivectorObject*)right,scalar_left = 0;

    if(scalar_left != -1){ // One of the arguments is a scalar
        out = multivector_scalar_add(data0,value,sign);
        if(!out){
            PyErr_SetString(PyExc_TypeError,"Something wrong computing the sum/subtraction with a scalar!");
            return NULL;
        }
        return (PyObject*)out;
    }

    if(!PyObject_TypeCheck(left,Py_TYPE(right))){
        PyErr_SetString(PyExc_TypeError,"Operands must be of the same type or int or ga_float");
        return NULL;
    }
    if(!compare_shapes(left, right)){
        if(((PyMvObject*)left)->strides[0] == 1)
            isleft_single = 1;
        else if(((PyMvObject*)right)->strides[0] == 1)
            isleft_single = 0;
        else{
            PyErr_SetString(PyExc_TypeError,"Multivector arrays must be of the same shape, or one of them is a single multivector");
            return NULL;
        }
    }
    PyMvObject *left_mv = (PyMultivectorObject*)left;
    PyMvObject *right_mv = (PyMultivectorObject*)right;

    if(!strcmp("scalar",left_mv->type->type_name) || !strcmp("scalar",right_mv->type->type_name)){
        out = multivector_scalar_array_add(left_mv, right_mv,sign);
        if(!out){
           PyErr_SetString(PyExc_TypeError,"Probably Incompatible Algebras!");
            return NULL;
        }
        return (PyObject*)out;
    }


    if(left_mv->GA == right_mv->GA || left_mv->type->generated || right_mv->type->generated)
        out = multivector_casttype_addsubtract(left_mv, right_mv, sign,isleft_single);
    else 
        out = multivector_mixed_addsubtract(left_mv, right_mv, sign,isleft_single);
    
    if(!out){
        PyErr_SetString(PyExc_TypeError,"Probably Incompatible Algebras!");
        return NULL;
    }
    return (PyObject*)out;
}


PyObject *multivector_outer_product(PyObject *left, PyObject *right){
    return multivector_product(left,right,ProductType_outer);
}

PyObject *multivector_inner_product(PyObject *left, PyObject *right){
    return multivector_product(left,right,ProductType_inner);
}

PyObject *multivector_geometric_product(PyObject *left, PyObject *right){
    return multivector_product(left,right,ProductType_geometric);
}

PyObject *multivector_regressive_product(PyObject *left, PyObject *right){
    return multivector_product(left,right,ProductType_regressive);
}

PyObject *multivector_cast(PyMultivectorObject *self, PyObject *args) {
    char *type_name = NULL;
    PyMultivectorObject *out;
    if(!PyArg_ParseTuple(args, "s", &type_name))
        return NULL;

    out = cast_mvarray_to_type(self,type_name);
    if(!out){
        PyErr_SetString(PyExc_ValueError, "Type name probably incorrect!!");
        return NULL;
    }
    return (PyObject*)out;
}

PyMvObject *multivector_scalar_grade_projection(PyMvObject *self){
    Py_ssize_t size = self->strides[0];
    PyMultivectorIter *iters = init_multivector_array_iters(self);
    PyMvObject *out = new_multivector_array(self->GA, "scalar", self->ns, self->strides, self->shapes);
    if(!out) return NULL;
    if(self->type->generated || self->type->ntype == MultivectorType_dense){
        for(Py_ssize_t i = 0; i < size; i++){
            ScalarMultivector *scalar = INDEX_DATA(out, i);
            iters[i].next(&iters[i]);
            if(iters[i].bitmap != 0) return NULL;
            *scalar = iters[i].value;
        }
    }else{
        for(Py_ssize_t i = 0; i < size; i++){
            ScalarMultivector *scalar = INDEX_DATA(out, i);
            *scalar = 0;
            while(iters[i].next(&iters[i]))
                if(iters[i].bitmap == 0)
                    *scalar += iters[i].value;
        }
    }

    free_multivector_iter(iters, size);
    return out;
}

PyObject *multivector_grade_project(PyMultivectorObject *self, PyObject *args, PyObject *kwds){
    static char *kwlist[] = {"grades",NULL};
    int *grades = NULL;
    PyObject *grades_obj = NULL;
    Py_ssize_t size = -1;
    PyMultivectorObject *out = NULL;
    if(!PyArg_ParseTupleAndKeywords(args, kwds, "O", kwlist, &grades_obj))
        return NULL; // raise error

    size = parse_list_as_grades(self->GA,grades_obj,&grades);
    if(size <= 0){ 
        PyErr_SetString(PyExc_TypeError, "Probably invalid grades!");
        return NULL;
    }

    if(size == 1 && grades[0] == 0 && self->strides[0] == 1){
        // If we want grade projection to scalars
        PyMultivectorIter *iter = init_multivector_iter(self,1);
        while(iter->next(iter)){
            if(iter->bitmap == 0){
                ga_float value = iter->value;
                PyMem_RawFree(grades);
                free_multivector_iter(iter,1);
                return (PyObject*)PyFloat_FromDouble((double)value);
            }
        }
        PyMem_RawFree(grades);
        free_multivector_iter(iter,1);
        return (PyObject*)PyFloat_FromDouble((double)0.0);

    }else if(size == 1 && grades[0] == 0){
        out = multivector_scalar_grade_projection(self);
        if(!out){
            PyErr_SetString(PyExc_TypeError, "Probably invalid scalar type!");
            return NULL;
        }
        PyMem_RawFree(grades);
        return (PyObject*)out;
    }

    gaunarygradefunc grade_project = self->type->math_funcs->grade_project;
    if(grade_project){
        out = new_mvarray_from_mvarray(self);
        if(!out){
            PyErr_SetString(PyExc_TypeError, "Error creating array!");
            return NULL; // raise error
        }

        for(Py_ssize_t i = 0; i < self->strides[0]; i++){
            if(!grade_project(INDEX_DATA(out,i),INDEX_DATA(self,i),self->GA,grades,size)){
                PyErr_SetString(PyExc_TypeError, "Error projecting multivector array to the specified grades!");
                multivector_array_dealloc(out);
                return NULL; // raise error
            }
        }
        
    }else{
        return NULL; // raise error
    }

    PyMem_RawFree(grades);
    return (PyObject*)out;
}

PyObject *multivector_add(PyObject *left, PyObject *right){
    return (PyObject*)multivector_add_subtract(left,right,1);
}

PyObject *multivector_subtract(PyObject *left, PyObject *right){
    return (PyObject*)multivector_add_subtract(left,right,-1);
}

PyObject *multivector_invert(PyMultivectorObject *self){
    gaunaryfunc reverse = self->type->math_funcs->reverse;
    PyMultivectorObject *out;
    if(reverse){
        out = new_mvarray_from_mvarray(self);
        if(!out){
            PyErr_SetString(PyExc_TypeError, "Error creating array!");
            return NULL; // raise error
        }

        for(Py_ssize_t i = 0; i < self->strides[0]; i++){
            if(!reverse(INDEX_DATA(out,i),INDEX_DATA(self,i),self->GA)){
                PyErr_SetString(PyExc_TypeError, "Error reversing multivector array!");
                multivector_array_dealloc(out);
                return NULL; // raise error
            }
        }
    }else
        return NULL; // raise error
    
    return (PyObject*)out;
}

PyObject* multivector_dual(PyMultivectorObject *self, PyObject *Py_UNUSED(ignored)){
    gaunaryfunc dual = self->type->math_funcs->dual;
    PyMultivectorObject *out = NULL;
    if(dual){
        out = new_mvarray_from_mvarray(self);
        if(!out){
            PyErr_SetString(PyExc_TypeError, "Error creating array!");
            return NULL; // raise error
        }

        for(Py_ssize_t i = 0; i < self->strides[0]; i++){
            if(!dual(INDEX_DATA(out,i),INDEX_DATA(self,i),self->GA)){
                PyErr_SetString(PyExc_TypeError, "Error dualizing multivector array!");
                multivector_array_dealloc(out);
                return NULL; // raise error
            }
        }
    }else{
        PyErr_SetString(PyExc_TypeError, "OPeration not available for the specified type!");
        return NULL; // raise error
    }
    return (PyObject*)out;
}

PyObject* multivector_undual(PyMultivectorObject *self, PyObject *Py_UNUSED(ignored)){
    gaunaryfunc undual = self->type->math_funcs->undual;
    PyMultivectorObject *out = NULL;
    if(undual){
        out = new_mvarray_from_mvarray(self);
        if(!out){
            PyErr_SetString(PyExc_TypeError, "Error creating array!");
            return NULL; // raise error
        }

        for(Py_ssize_t i = 0; i < self->strides[0]; i++){
            if(!undual(INDEX_DATA(out,i),INDEX_DATA(self,i),self->GA)){
                PyErr_SetString(PyExc_TypeError, "Error undualizing multivector array!");
                multivector_array_dealloc(out);
                return NULL; // raise error
            }
        }
    }else{
        PyErr_SetString(PyExc_TypeError, "Operation not available for the specified type!");
        return NULL; // raise error
    }
    return (PyObject*)out;
}

static PyObject *multivector_sign(PyMultivectorObject *self, ga_float value){
    gascalarfunc scalar_product = self->type->math_funcs->scalar_product;
    PyMultivectorObject *out = NULL;
    if(scalar_product){
        out = new_mvarray_from_mvarray(self);
        if(!out){
            PyErr_SetString(PyExc_TypeError, "Error creating array!");
            return NULL; // raise error
        }

        for(Py_ssize_t i = 0; i < self->strides[0]; i++){
            if(!scalar_product(INDEX_DATA(out,i),INDEX_DATA(self,i),self->GA,value)){
                PyErr_SetString(PyExc_TypeError, "Error scalar_productizing multivector array!");
                multivector_array_dealloc(out);
                return NULL; // raise error
            }
        }
    }else{
        PyErr_SetString(PyExc_TypeError, "OPeration not available for the specified type!");
        return NULL; // raise error
    }
    return (PyObject*)out;
}

PyObject *multivector_negative(PyMultivectorObject *self){
    return multivector_sign(self,-1);
}

PyObject *multivector_positive(PyMultivectorObject *self){
    return multivector_sign(self,1);
}

int get_biggest_algebra_index(PyObject *cls, PyObject *args){
    PyAlgebraObject *biggest_ga;
    Py_ssize_t index = 0;
    int same_algebra_and_type = 1;
    PyMultivectorObject *data0;
    Py_ssize_t size = PyTuple_Size(args);
    PyObject *arg0 = PyTuple_GetItem(args,0);
    int ntype;
    if(!PyObject_IsInstance(arg0,cls)) return -1;
    data0 = (PyMultivectorObject*)arg0;
    biggest_ga = data0->GA;
    ntype = data0->type->ntype;

    for(Py_ssize_t i = 1; i < size; i++){
        PyObject *argi = PyTuple_GetItem(args,i);
        // check if objects are multivectors
        if(!PyObject_IsInstance(argi,cls)){
            PyErr_SetString(PyExc_TypeError,"objects must be an instance of gasparse.multivector");
            return -1;
        }
        data0 = (PyMultivectorObject*)argi;
        // check if object are compatible
        if(biggest_ga != data0->GA){
            int is0_bigger = is_bigger_metric(data0->GA,biggest_ga);
            if(is0_bigger == -1) return -1;
            else if(is0_bigger == 1) biggest_ga = data0->GA, index = i;
            same_algebra_and_type = 0;
        }else if(data0->type->ntype != ntype) same_algebra_and_type = 0;
    }

    if(same_algebra_and_type) return -2;
    return index;
}

static PyObject *multivector_algebra(PyMultivectorObject *self, PyObject *Py_UNUSED(ignored)){
    Py_XINCREF((PyObject*)self->GA);
    return (PyObject*)self->GA;
}

static PyObject *multivector_type(PyMvObject *self, PyObject *Py_UNUSED(ignored)){
	return Py_BuildValue("s", self->type->type_name);
}

PyObject *multivector_sum(PyMvObject *self, PyObject *Py_UNUSED(ignored)){
    // In the mean time should also add axis as an optional argument (when einsum is implemented)

    gaatomicfunc atomic_add = self->type->math_funcs->atomic_add;
    if(!atomic_add){
        PyErr_SetString(PyExc_NotImplementedError,"The atomic sum operation for these types is not implemented");
        return NULL; 
    }
    PyMvObject *out = new_multivector_inherit_type(self->GA, self->type);
    if(!out){
        PyErr_SetString(PyExc_MemoryError,"Error creating new multivector");
        return NULL; 
    }
    if(!atomic_add(out->data,self->data,self->GA,self->strides[0])){
        multivector_array_dealloc(out);
        PyErr_SetString(PyExc_ValueError,"Error atomic adding multivectors");
        return NULL; 
    }
    return (PyObject*)out;
}

PyObject *multivector_atomic_product(PyMvObject *self, ProductType ptype){
    // In the mean time should also add axis as an optional argument (when einsum is implemented)

    gaatomicprodfunc atomic_product = self->type->math_funcs->atomic_product;
    if(!atomic_product){
        PyErr_SetString(PyExc_NotImplementedError,"The atomic product operation for these types is not implemented");
        return NULL; 
    }
    PyMvObject *out = new_multivector_inherit_type(self->GA, self->type);
    if(!out){
        PyErr_SetString(PyExc_MemoryError,"Error creating new multivector");
        return NULL; 
    }
    if(!atomic_product(out->data,self->data,self->GA,self->strides[0],ptype)){
        multivector_array_dealloc(out);
        PyErr_SetString(PyExc_ValueError,"Error atomic producting multivectors");
        return NULL; 
    }
    return (PyObject*)out;
}

PyObject *multivector_atomic_geometric_product(PyMvObject *self, PyObject *Py_UNUSED(ignored)){
    return multivector_atomic_product(self,ProductType_geometric);
}

PyObject *multivector_atomic_outer_product(PyMvObject *self, PyObject *Py_UNUSED(ignored)){
    return multivector_atomic_product(self,ProductType_outer);
}

PyObject *multivector_atomic_inner_product(PyMvObject *self, PyObject *Py_UNUSED(ignored)){
    return multivector_atomic_product(self,ProductType_inner);
}

PyObject *multivector_atomic_regressive_product(PyMvObject *self, PyObject *Py_UNUSED(ignored)){
    return multivector_atomic_product(self,ProductType_regressive);
}
/*
PyObject* multivector_atomic_add(PyObject *cls, PyObject *args){
    Py_ssize_t size = PyTuple_Size(args);
    gaatomicfunc add = NULL;
    gamixedatomicfunc mixed_add = NULL;
    PyMultivectorObject *data0, *data1, *out;
    PyMultivectorObject *data_array;
    Py_ssize_t index;

    if(size <= 1){
        PyErr_SetString(PyExc_ValueError,"number of arguments must be at least two");
        return NULL;
    }
    if((index = get_biggest_algebra_index(cls,args)) == -1)
        return NULL;

    if(index == -2){
        if(size == 2){
            gaaddfunc binary_add;
            data0 = (PyMultivectorObject*)PyTuple_GetItem(args,0);
            data1 = (PyMultivectorObject*)PyTuple_GetItem(args,1);
            binary_add = data0->type->math_funcs->add;
            if(binary_add){
                return (PyObject*)binary_add(data0,data1,1);
            }else{
                PyErr_SetString(PyExc_NotImplementedError,"The binary sum operation for these types is not implemented");
                return NULL;
            }
        }
    }

    data_array = (PyMultivectorObject*)PyMem_RawMalloc(size*sizeof(PyMultivectorObject));
    for(Py_ssize_t i = 0; i < size; i++)
        data_array[i] = *((PyMultivectorObject*)PyTuple_GetItem(args,i));

    if(index >= 0){ // dispatch mixed type operations
        mixed_add = data_array[index].mixed->atomic_add;
        if(mixed_add){
            out = mixed_add(data_array,size,&data_array[index]);
        }else{
            PyMem_RawFree(data_array);
            PyErr_SetString(PyExc_NotImplementedError,"The atomic mixed sum operation for these types is not implemented");
            return NULL; // raise not implemented error
        }
    }
    else{
        add = data_array->type->math_funcs->atomic_add;
        if(add){
            out = add(data_array,size);
        }else{
            PyMem_RawFree(data_array);
            PyErr_SetString(PyExc_NotImplementedError,"The atomic sum operation for these types is not implemented");
            return NULL; // raise not implemented error
        }
    }

    PyMem_RawFree(data_array);
    return (PyObject*)out;
}




static PyObject* multivector_atomic_product(PyObject *cls, PyObject *args, ProductType ptype){
    Py_ssize_t size = PyTuple_Size(args);
    gaatomicprodfunc product = NULL;
    gamixedatomicprodfunc mixed_product = NULL;
    PyMultivectorObject *data0 = NULL, *data1 = NULL, *data2 = NULL, *out = NULL;
    PyMultivectorObject *data_array = NULL;
    Py_ssize_t index;

    if(size <= 1){
        PyErr_SetString(PyExc_ValueError,"number of arguments must be at least two");
        return NULL;
    }
    // check if objects are multivectors
    if((index = get_biggest_algebra_index(cls,args)) == -1)
        return NULL;

    if(index == -2){
        if(size == 2){
            gaprodfunc binary_product;
            data0 = (PyMultivectorObject*)PyTuple_GetItem(args,0);
            data1 = (PyMultivectorObject*)PyTuple_GetItem(args,1);
            binary_product = data0->type->math_funcs->product;
            if(binary_product){
                return (PyObject*)binary_product(data0,data1,ptype);
            }else{
                PyErr_SetString(PyExc_NotImplementedError,"The binary product operation for these types is not implemented");
                return NULL;
            }
        }else if(size == 3){
            gaternaryprodfunc ternary_product;
            data0 = (PyMultivectorObject*)PyTuple_GetItem(args,0);
            data1 = (PyMultivectorObject*)PyTuple_GetItem(args,1);
            data2 = (PyMultivectorObject*)PyTuple_GetItem(args,2);

            ternary_product = data0->type->math_funcs->ternary_product;
            if(ternary_product){
                return (PyObject*)ternary_product(data0,data1,data2,ptype);
            }else{
                PyErr_SetString(PyExc_NotImplementedError,"The ternary product operation for these types is not implemented");
                return NULL;
            }
        }
    }

    data_array = (PyMultivectorObject*)PyMem_RawMalloc(size*sizeof(PyMultivectorObject));
    for(Py_ssize_t i = 0; i < size; i++)
        data_array[i] = *((PyMultivectorObject*)PyTuple_GetItem(args,i));

    if(index >= 0){ // dispatch mixed type operations
        mixed_product = data_array[index].mixed->atomic_product;
        if(mixed_product){
            out = mixed_product(data_array,size,&data_array[index],ptype);
        }else{
            PyMem_RawFree(data_array);
            PyErr_SetString(PyExc_NotImplementedError,"The atomic mixed sum operation for these types is not implemented");
            return NULL; // raise not implemented error
        }
    }else{
        product = data_array->type->math_funcs->atomic_product;
        if(product){
            out = product(data_array,size,ptype);
        }else{
            PyMem_RawFree(data_array);
            PyErr_SetString(PyExc_NotImplementedError,"The atomic product operation for these types is not implemented");
            return NULL; // raise not implemented error
        }
    }

    PyMem_RawFree(data_array);
    return (PyObject*)out;
}


PyObject* multivector_atomic_geometric_product(PyObject *cls, PyObject *args){
    return multivector_atomic_product(cls,args,ProductType_geometric);
}

PyObject* multivector_atomic_outer_product(PyObject *cls, PyObject *args){
    return multivector_atomic_product(cls,args,ProductType_outer);
}

PyObject* multivector_exponential(PyObject *cls, PyObject *args){
    Py_ssize_t size = PyTuple_Size(args);
    gaunaryfunc exponential;
    PyMultivectorObject *data0 = NULL;

    if(size != 1){
        PyErr_SetString(PyExc_ValueError,"number of arguments must be one");
        return NULL;
    }
    // check if objects are multivectors
    if(get_biggest_algebra_index(cls,args) == -1){
        PyErr_SetString(PyExc_ValueError,"the input must be a multivector");
        return NULL;
    }

    data0 = (PyMultivectorObject*)PyTuple_GetItem(args,0);
    exponential = data0->type->math_funcs->exp;
    if(exponential){
        return (PyObject*)exponential(data0);
    }else{
        PyErr_SetString(PyExc_NotImplementedError,"The exponential operation for this type is not implemented");
        return NULL;
    }
    return NULL;
}
*/
int get_multivector_type_table(PyAlgebraObject *ga, char *name, PyMultivectorSubType **subtype){
    
    if(ga->types == NULL) return 0;

    for(Py_ssize_t i = 0; i < ga->tsize; i++){
        if(!strncmp(name,ga->types[i].type_name,strlen(name))){
            *subtype = &ga->types[i];
            return 1;
        }
    }
    return 0;
}

int check_multivector_mixed_type_table(PyMultivectorObject *mv,char *name){
    char *mname;
    Py_ssize_t i = 0;
    if(!strcmp(*mv->mixed->type_names,"any"))
        return 1;
    // The type_names array is null terminated
    while((mname = mv->mixed->type_names[i])){
        if(!strncmp(mname,name,strlen(name)))
            return 1;
        i++;
    }
    return 0;
}

Py_ssize_t parse_list_as_values(PyObject *values, ga_float **values_float) {
	if (!PyList_Check(values)) {
		PyErr_SetString(PyExc_TypeError, "values must be a list");
		return -1;
	}
	Py_ssize_t size = PyList_Size(values);
	if (size <= 0)
		return -1;
	*values_float = (ga_float *)PyMem_RawMalloc(size * sizeof(ga_float));
	for (Py_ssize_t i = 0; i < size; i++) {
		PyObject *value_i = PyList_GetItem(values, i);
		if (PyFloat_Check(value_i))
			(*values_float)[i] = (ga_float)PyFloat_AsDouble(value_i);
		else if (PyLong_Check(value_i))
			(*values_float)[i] = (ga_float)PyLong_AsLong(value_i);
		else {
			//PyErr_SetString(PyExc_TypeError,
			//								"Elements of the list of values must be float or");
			PyMem_RawFree(*values_float);
			return -1;
		}
	}
	return size;
}

int parse_list_as_basis_grades(PyAlgebraObject ga, int *grades, int **bitmaps, Py_ssize_t gsize) {
	// Given an array of grades computes an array of bitmaps
	
	Py_ssize_t size = ga.asize;
	Py_ssize_t psize = 0;
	Py_ssize_t *grade_bool = get_grade_bool(grades, gsize, MAX_GRADE((&ga)) + 1);
	
	for (Py_ssize_t i = 0; i < size; i++) {
		if (grade_bool[GRADE(i)])
			psize++;
	}

	*bitmaps = (int *)PyMem_RawMalloc(psize * sizeof(int));

	Py_ssize_t j = 0;
		for (Py_ssize_t i = 0; i < size; i++) {
			if (grade_bool[GRADE(i)] && j < psize) {
				(*bitmaps)[j] = i;
				j++;
			} else if (j > psize) {
				break;
			}
		}
	PyMem_RawFree(grade_bool);
	return psize;
}


static PyNumberMethods PyMultivectorNumberMethods = {
		.nb_multiply = (binaryfunc)multivector_geometric_product,
		.nb_xor = (binaryfunc)multivector_outer_product,
		.nb_and = (binaryfunc)multivector_regressive_product,
		.nb_or = (binaryfunc)multivector_inner_product,
		.nb_add = (binaryfunc)multivector_add,
		.nb_subtract = (binaryfunc)multivector_subtract,
		.nb_invert = (unaryfunc)multivector_invert,
		.nb_negative = (unaryfunc)multivector_negative,
		.nb_positive = (unaryfunc)multivector_positive,

};


PyDoc_STRVAR(add_doc, "adds a bunch of multivectors.");
PyDoc_STRVAR(dual_doc, "dualizes the multivector.");
PyDoc_STRVAR(undual_doc, "undualizes the multivector.");
PyDoc_STRVAR(product_doc, "multiplies a bunch of multivectors.");
PyDoc_STRVAR(exponential_doc, "takes the exponential of multivectors.");
PyDoc_STRVAR(list_doc,
						 "Returns a list with each coefficient of the multivector.");
PyDoc_STRVAR(cast_doc, "Casts the multivector to the specified type");
PyDoc_STRVAR(grade_doc, "Returns the grade of a multivector");
PyDoc_STRVAR(algebra_doc, "Returns the algebra of a multivector");
PyDoc_STRVAR(type_doc, "Returns a string indicating the type of the multivector");
PyDoc_STRVAR(sum_doc, "Sums all of the multivectors in the array together");
PyDoc_STRVAR(prod_doc, "Geometric multiplies all of the multivectors in the array together, \
                        the first element on the array is the leftmost element to be geometric multipled");
PyDoc_STRVAR(outer_prod_doc, "Outer multiplies all of the multivectors in the array together, \
                              the first element on the array is the leftmost element to be geometric multipled");
PyDoc_STRVAR(inner_prod_doc, "Inner multiplies all of the multivectors in the array together, \
                              the first element on the array is the leftmost element to be geometric multipled.\
                              Carefull with precedence, the multiplication starts in the first element!!");
PyDoc_STRVAR(regressive_prod_doc, "Regressive multiplies all of the multivectors in the array together, \
                                   the first element on the array is the leftmost element to be geometric multipled.\
                                   Carefull with precedence, the multiplication starts in the first element!!");


PyMethodDef multivector_methods[] = {
        {"GA",(PyCFunction)multivector_algebra,METH_NOARGS,algebra_doc},
        {"Type",(PyCFunction)multivector_type,METH_NOARGS,type_doc},
		{"dual", (PyCFunction)multivector_dual, METH_NOARGS, dual_doc},
		{"undual", (PyCFunction)multivector_undual, METH_NOARGS, undual_doc},
        {"sum",(PyCFunction)multivector_sum, METH_NOARGS, sum_doc},
        {"prod",(PyCFunction)multivector_atomic_geometric_product, METH_NOARGS, prod_doc},
        {"outer_prod",(PyCFunction)multivector_atomic_outer_product, METH_NOARGS, outer_prod_doc},
        {"inner_prod",(PyCFunction)multivector_atomic_inner_product, METH_NOARGS, inner_prod_doc},
        {"regressive_prod",(PyCFunction)multivector_atomic_regressive_product, METH_NOARGS, regressive_prod_doc},
		//{"add", (PyCFunction)multivector_atomic_add, METH_VARARGS | METH_CLASS, add_doc},
		//{"geometric_product", (PyCFunction)multivector_atomic_geometric_product,METH_VARARGS | METH_CLASS, product_doc},
		//{"outer_product", (PyCFunction)multivector_atomic_outer_product,METH_VARARGS | METH_CLASS, product_doc},
		//{"exp", (PyCFunction)multivector_exponential, METH_VARARGS | METH_CLASS,exponential_doc},
		{"tolist", (PyCFunction)multivector_list, METH_VARARGS | METH_KEYWORDS,
		 list_doc},
		{"cast", (PyCFunction)multivector_cast, METH_VARARGS, cast_doc},
		{"grade", (PyCFunction)multivector_grade, METH_NOARGS, grade_doc},
		{NULL},
};

PyTypeObject PyMultivectorType = {
		PyVarObject_HEAD_INIT(NULL, 0).tp_name = "gasparse.multivector",
		.tp_doc = PyDoc_STR(
				"Builds a multivector in different types (sparse,dense,blades)"),
		.tp_basicsize = sizeof(PyMultivectorObject),
		.tp_itemsize = 0,
		.tp_flags = Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,
		.tp_dealloc = (destructor)multivector_array_dealloc,
		.tp_repr = (reprfunc)multivector_repr,
		.tp_str = (reprfunc)multivector_repr,
		.tp_call = (ternaryfunc)multivector_grade_project,
		.tp_new = NULL,
		.tp_as_number = &PyMultivectorNumberMethods,
		.tp_methods = multivector_methods};

