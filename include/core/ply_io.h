/*

 Header for PLY polygon files.

 - Greg Turk

 A PLY file contains a single polygonal _object_.

 An object is composed of lists of _elements_.  Typical elements are
 vertices, faces, edges and materials.

 Each type of element for a given object has one or more _properties_
 associated with the element type.  For instance, a vertex element may
 have as properties three floating-point values x,y,z and three unsigned
 chars for red, green and blue.

 -----------------------------------------------------------------------

 Copyright (c) 1998 Georgia Institute of Technology.  All rights reserved.   
 
 Permission to use, copy, modify and distribute this software and its   
 documentation for any purpose is hereby granted without fee, provided   
 that the above copyright notice and this permission notice appear in   
 all copies of this software and that you do not sell the software.   
 
 THE SOFTWARE IS PROVIDED "AS IS" AND WITHOUT WARRANTY OF ANY KIND,   
 EXPRESS, IMPLIED OR OTHERWISE, INCLUDING WITHOUT LIMITATION, ANY   
 WARRANTY OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.   

 */

#ifndef __PLY_H__
#define __PLY_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stddef.h>

#define PLY_ASCII      1        /* ascii PLY file */
#define PLY_BINARY_BE  2        /* binary PLY file, big endian */
#define PLY_BINARY_LE  3        /* binary PLY file, little endian */

#define PLY_OKAY    0           /* ply routine worked okay */
#define PLY_ERROR  -1           /* error in ply routine */

/* scalar data types supported by PLY format */

#define StartType  0
#define Int8       1
#define Int16      2
#define Int32      3
#define Uint8      4
#define Uint16     5
#define Uint32     6
#define Float32    7
#define Float64    8
#define EndType    9

#define  PLY_SCALAR  0
#define  PLY_LIST    1
#define  PLY_STRING  2

typedef struct _plyVertex {
	float x[3];             // the usual 3-space position of a vertex
	float n[3];
	float vel[3];
	unsigned char red;
	unsigned char green;
	unsigned char blue;
	unsigned char alpha;
	_plyVertex() {
		x[0] = 0;
		x[1] = 0;
		x[2] = 0;
		n[0] = 0;
		n[1] = 0;
		n[2] = 0;
		vel[0] = 0;
		vel[1] = 0;
		vel[2] = 0;
		red = 0;
		green = 0;
		blue = 0;
		alpha = 0;
	}
} plyVertex;
typedef struct _plyParticle {
	float x[3];             // the usual 3-space position of a vertex
	float n[3];
	float radius;
	_plyParticle() {
		x[0] = 0;
		x[1] = 0;
		x[2] = 0;
		n[0] = 0;
		n[1] = 0;
		n[2] = 0;
		radius = 0.0f;
	}
} plyParticle;

typedef struct _plyFace {
	unsigned char nverts;    // number of vertex indices in list
	unsigned char nvels;    // number of vertex indices in list
	int *verts;              // vertex index list
	float* velocity;
	_plyFace() {
		nverts = 0;
		verts = NULL;
		velocity = NULL;
		nvels = 3;
	}
} plyFace;
typedef struct _plyFaceTexutre {
	unsigned char nverts;    // number of vertex indices in list
	unsigned char nvels;    // number of vertex indices in list

	int *verts;              // vertex index list
	float* velocity;
	unsigned char uvcount;
	float* uvs;
	_plyFaceTexutre() {
		nverts = 0;
		uvs = NULL;
		verts = NULL;
		velocity = NULL;
		uvcount = 6;
		nvels = 3;
	}
} plyFaceTexture;

typedef struct PlyProperty { /* description of a property */

	char *name; /* property name */
	int external_type; /* file's data type */
	int internal_type; /* program's data type */
	int offset; /* offset bytes of prop in a struct */

	int is_list; /* 0 = scalar, 1 = list, 2 = char string */
	int count_external; /* file's count type */
	int count_internal; /* program's count type */
	int count_offset; /* offset byte for list count */

} PlyProperty;

typedef struct PlyElement { /* description of an element */
	char *name; /* element name */
	int num; /* number of elements in this object */
	int size; /* size of element (bytes) or -1 if variable */
	int nprops; /* number of properties for this element */
	PlyProperty **props; /* list of properties in the file */
	char *store_prop; /* flags: property wanted by user? */
	int other_offset; /* offset to un-asked-for props, or -1 if none*/
	int other_size; /* size of other_props structure */
} PlyElement;

typedef struct PlyOtherProp { /* describes other properties in an element */
	char *name; /* element name */
	int size; /* size of other_props */
	int nprops; /* number of properties in other_props */
	PlyProperty **props; /* list of properties in other_props */
} PlyOtherProp;

typedef struct OtherData { /* for storing other_props for an other element */
	void *other_props;
} OtherData;

typedef struct OtherElem { /* data for one "other" element */
	char *elem_name; /* names of other elements */
	int elem_count; /* count of instances of each element */
	OtherData **other_data; /* actual property data for the elements */
	PlyOtherProp *other_props; /* description of the property data */
} OtherElem;

typedef struct PlyOtherElems { /* "other" elements, not interpreted by user */
	int num_elems; /* number of other elements */
	OtherElem *other_list; /* list of data for other elements */
} PlyOtherElems;

#define AVERAGE_RULE  1
#define MAJORITY_RULE 2
#define MINIMUM_RULE  3
#define MAXIMUM_RULE  4
#define SAME_RULE     5
#define RANDOM_RULE   6

typedef struct PlyPropRules { /* rules for combining "other" properties */
	PlyElement *elem; /* element whose rules we are making */
	int *rule_list; /* types of rules (AVERAGE_PLY, MAJORITY_PLY, etc.) */
	int nprops; /* number of properties we're combining so far */
	int max_props; /* maximum number of properties we have room for now */
	void **props; /* list of properties we're combining */
	float *weights; /* list of weights of the properties */
} PlyPropRules;

typedef struct PlyRuleList {
	char *name; /* name of the rule */
	char *element; /* name of element that rule applies to */
	char *property; /* name of property that rule applies to */
	struct PlyRuleList *next; /* pointer for linked list of rules */
} PlyRuleList;

typedef struct PlyFile { /* description of PLY file */
	FILE *fp; /* file pointer */
	int file_type; /* ascii or binary */
	float version; /* version number of file */
	int num_elem_types; /* number of element types of object */
	PlyElement **elems; /* list of elements */
	int num_comments; /* number of comments */
	char **comments; /* list of comments */
	int num_obj_info; /* number of items of object information */
	char **obj_info; /* list of object info items */
	PlyElement *which_elem; /* element we're currently reading or writing */
	PlyOtherElems *other_elems; /* "other" elements from a PLY file */
	PlyPropRules *current_rules; /* current propagation rules */
	PlyRuleList *rule_list; /* rule list from user */
} PlyFile;

/* memory allocation */
/*
 extern char *my_alloc();
 */
void* myalloc(size_t mem_size);

void myfree(void* ptr);
void* myrealloc(void* ptr, size_t mem_size);
/* old routines */

#if 0
extern PlyFile *ply_write(FILE *, int, char **, int);
extern PlyFile *ply_read(FILE *, int *, char ***);
extern PlyFile *ply_open_for_reading( char *, int *, char ***, int *, float *);
extern void ply_close(PlyFile *);
extern PlyOtherProp *ply_get_other_properties(PlyFile *, char *, int);
#endif

extern void ply_describe_property(PlyFile *, char *, PlyProperty *);
extern void ply_get_property(PlyFile *, char *, PlyProperty *);
extern void ply_get_element(PlyFile *, void *);

/*** delcaration of routines ***/

PlyOtherElems *get_other_element_ply(PlyFile *);

PlyFile *read_ply(FILE *);
PlyFile *write_ply(FILE *, int, char **, int);
extern PlyFile *open_for_writing_ply(const char *, int, char **, int);
void close_ply(PlyFile *);
void free_ply(PlyFile *);

void get_info_ply(PlyFile *, float *, int *);
void free_other_elements_ply(PlyOtherElems *);

void append_comment_ply(PlyFile *, char *);
void append_obj_info_ply(PlyFile *, char *);
void copy_comments_ply(PlyFile *, PlyFile *);
void copy_obj_info_ply(PlyFile *, PlyFile *);
char **get_comments_ply(PlyFile *, int *);
char **get_obj_info_ply(PlyFile *, int *);

char **get_element_list_ply(PlyFile *, int *);
void setup_property_ply(PlyFile *, PlyProperty *);
void get_element_ply(PlyFile *, void *);
char *setup_element_read_ply(PlyFile *, int, int *);
PlyOtherProp *get_other_properties_ply(PlyFile *, int);

void element_count_ply(PlyFile *, char *, int);
void describe_element_ply(PlyFile *, char *, int);
void describe_property_ply(PlyFile *, PlyProperty *);
void describe_other_properties_ply(PlyFile *, PlyOtherProp *, int);
void describe_other_elements_ply(PlyFile *, PlyOtherElems *);
void get_element_setup_ply(PlyFile *, char *, int, PlyProperty *);
PlyProperty **get_element_description_ply(PlyFile *, char *, int*, int*);
void element_layout_ply(PlyFile *, char *, int, int, PlyProperty *);

void header_complete_ply(PlyFile *);
void put_element_setup_ply(PlyFile *, char *);
void put_element_ply(PlyFile *, void *);
void put_other_elements_ply(PlyFile *);

PlyPropRules *init_rule_ply(PlyFile *, char *);
void modify_rule_ply(PlyPropRules *, char *, int);
void start_props_ply(PlyFile *, PlyPropRules *);
void weight_props_ply(PlyFile *, float, void *);
void *get_new_props_ply(PlyFile *);
void set_prop_rules_ply(PlyFile *, PlyRuleList *);
PlyRuleList *append_prop_rule(PlyRuleList *, char *, char *);
int matches_rule_name(char *);

int equal_strings(char *, char *);
char *recreate_command_line(int, char *argv[]);
/* returns 1 if strings are equal, 0 if not */
int equal_strings(char *, char *);

/* find an element in a plyfile's list */
PlyElement *find_element(PlyFile *, char *);

/* find a property in an element's list */
PlyProperty *find_property(PlyElement *, char *, int *);

/* write to a file the word describing a PLY file data type */
void write_scalar_type(FILE *, int);

/* read a line from a file and break it up into separate words */
char **get_words(FILE *, int *, char **);

/* write an item to a file */
void write_binary_item(FILE *, int, unsigned int, double, int);
void write_ascii_item(FILE *, int, unsigned int, double, int);

/* add information to a PLY file descriptor */
void add_element(PlyFile *, char **, int);
void add_property(PlyFile *, char **, int);
void add_comment(PlyFile *, char *);
void add_obj_info(PlyFile *, char *);

/* copy a property */
void copy_property(PlyProperty *, PlyProperty *);

/* store a value into where a pointer and a type specify */
void store_item(char *, int, int, unsigned int, double);

/* return the value of a stored item */
void get_stored_item(void *, int, int *, unsigned int *, double *);

/* return the value stored in an item, given ptr to it and its type */
double get_item_value(char *, int);

/* get binary or ascii item and store it according to ptr and type */
void get_ascii_item(char *, int, int *, unsigned int *, double *);
void get_binary_item(FILE *, int, int *, unsigned int *, double *);

/* get a bunch of elements from a file */
void ascii_get_element(PlyFile *, char *);
void binary_get_element(PlyFile *, char *);

/* memory allocation */
//static char *my_alloc(int, int, char *);
PlyFile *ply_open_for_reading(const char *filename, int *nelems,
		char ***elem_names, int *file_type, float *version);

#ifdef __cplusplus
}

#endif
#endif /* !__PLY_H__ */

