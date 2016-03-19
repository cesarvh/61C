/* Include the system headers we need */
#include <stdlib.h>
#include <stdio.h>

/* Include our header */
#include "vector.h"

/* Define what our struct is */
struct vector_t {
	size_t size;
	int *data;
};

/* Utility function to handle allocation failures. In this
   case we print a message and exit. */
static void allocation_failed() {
    fprintf(stderr, "Out of memory.\n");
    exit(1);
}

/* Create a new vector */
vector_t *vector_new() {
	vector_t *retval;  

	/* First, we need to allocate the memory for the struct */
	retval = malloc(1 * sizeof(vector_t));

	/* Check our return value to make sure we got memory */
	if(retval == NULL)
                allocation_failed();
	 
	/* Now we need to initialize our data */
	retval->size = 1;
	retval->data = malloc(retval->size * sizeof(int)); // this might be an issue

	/* Check our return value to make sure we got memory */
	if(retval->data == NULL) {
		free(retval);
                allocation_failed();
	}

	retval->data[0] = 0; // this might also be ain issue...?
	
	/* and return... */
	return retval; // or tHIS IDK
}

/* Set a value in the vector. If the extra memory allocation fails, call
   allocation_failed(). */
void vector_set(vector_t *v, size_t loc, int value) {
	/* What do you need to do if the location is greater than the size we have
	 * allocated?  Remember that unset locations should contain a value of 0.
	 */

	/* If we're given a null vector, complain about it and exit */
	if(v == NULL) {
		fprintf(stderr, "vector_get: passed a NULL vector.\n");
                abort();
	}

	/* if the location ISN't greater than the size of the vector...*/
	if (loc >= v->size) {

		// set a temporary variable to that memory chunk
		int * setter = (int *) malloc(sizeof(int) * (loc + 1));
		
		// if its null then we do da erasing
		if (!setter) {
			vector_delete(v);
			allocation_failed();
		}
		int i;
		i = 0;

		// now we go through the setter to copy the data from the vector to the setter
		// until we reach the end of the loc. 
		// we either copy the value or set it to 0 because it would be empty
		while (i < loc) {
			if (i < v->size) {
				setter[i] = v->data[i];
			}else {
				setter[i] = 0;
			}
			i++;
		}
		// we now set the value at the location as we are meant to
		setter[loc] = value;

		// now we free v's data because we're about to give it a new larger one
		// we also increment the size and then set the setter to null because we
		// no longer need it
		free(v->data);
		v->data = setter;
		v->size = loc + 1;
		setter = NULL;
		// NOTE: we cant just free the memory being used by the setter because setter
		// now points to the data that v->data points to. We cant free that so instead we
		// make it stop pointing there.
	}else {
		v->data[loc] = value;
	}


}

/* Free up the memory allocated for the passed vector */
void vector_delete(vector_t *v) {
	/* Remember, you need to free up ALL the memory that is allocated */
	if (v != NULL) {
	free(v->data);
	free(v);
	}	

}

/* Return the value in the vector */
int vector_get(vector_t *v, size_t loc) {

	/* If we are passed a NULL pointer for our vector, complain about it and
         * exit.
	 */
	if(v == NULL) {
		fprintf(stderr, "vector_get: passed a NULL vector.\n");
                abort();
	}

	/* If the requested location is higher than we have allocated, return 0.
	 * Otherwise, return what is in the passed location.
	 */
	if(loc < v->size) {
		return v->data[loc];
	} else {
		return 0;
	}
}


