/*
* Copyright(C) 2015, Blake C. Lucas, Ph.D. (img.science@gmail.com)
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
* THE SOFTWARE.
*/

/*
This is a rewrite of Greg Turk's PLY mesh file reader. 
It doesn't leak memory like the original, and you can sell software that uses it free of charge.
*/
#ifndef ALLOYPLY_H_
#define ALLOYPLY_H_
#include <stdio.h>
#include <string>
#include <memory>
#include <vector>
#include <fstream>
#include <list>
namespace aly {
	namespace ply {
		enum class FileFormat { ASCII = 1, BINARY_BE = 2, BINARY_LE = 3 };
		template<class C, class R> std::basic_ostream<C, R> & operator <<(
			std::basic_ostream<C, R> & ss, const FileFormat& type) {
			switch (type) {
			case FileFormat::ASCII:
				return ss << "ASCII";
			case FileFormat::BINARY_BE:
				return ss << "Binary Big Endian";
			case FileFormat::BINARY_LE:
				return ss << "Binary Little Endian";
			}
			return ss;
		}
		enum class DataType {
			StartType = 0,
			Int8 = 1,
			Int16 = 2,
			Int32 = 3,
			Uint8 = 4,
			Uint16 = 5,
			Uint32 = 6,
			Float32 = 7,
			Float64 = 8,
			EndType = 9
		};
		enum class Rule { None = 0, Average = 1, Majority = 2, Minimum = 3, Maximum = 4, Same = 5, Random = 6, EndMarker = -1 };

		struct RuleName {
			Rule code;
			std::string name;
		};
		static RuleName rule_name_list[] = {
			Rule::Average, "avg",
			Rule::Random, "rnd",
			Rule::Minimum, "max",
			Rule::Maximum, "min",
			Rule::Majority, "major",
			Rule::Same, "same",
			Rule::EndMarker, "end_marker", };

		const std::string property_type_names[] = { "invalid", "int8", "int16", "int32", "uint8",
			"uint16", "uint32", "float32", "float64" };
		const std::string old_property_type_names[] =
		{ "invalid", "char", "short", "int", "uchar", "ushort", "uint", "float","double" };
		int ply_type_size[] = { 0, 1, 2, 4, 1, 2, 4, 4, 8 };
		template<class C, class R> std::basic_ostream<C, R> & operator <<(
			std::basic_ostream<C, R> & ss, const DataType& type) {
			ss << property_type_names[static_cast<int>(type)];
		}
		template<class C, class R> std::basic_ostream<C, R> & operator <<(
			std::basic_ostream<C, R> & ss, const Rule& type) {
			ss << rule_name_list[static_cast<int>(type)].name;
		}
		enum class SectionType {
			Scalar = 0, List = 1, String = 2
		};
		struct plyVertex {
			float x[3];             // the usual 3-space position of a vertex
			float n[3];
			float vel[3];
			unsigned char red;
			unsigned char green;
			unsigned char blue;
			unsigned char alpha;
			plyVertex() {
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
		};
		struct plyFace {
			unsigned char nverts;    // number of vertex indices in list
			unsigned char nvels;    // number of vertex indices in list
			int *verts;              // vertex index list
			float* velocity;
			plyFace() {
				nverts = 0;
				verts = nullptr;
				velocity = nullptr;
				nvels = 3;
			}
		};
		struct plyParticle {
			float x[3];             // the usual 3-space position of a vertex
			float n[3];
			float radius;
			plyParticle() {
				x[0] = 0;
				x[1] = 0;
				x[2] = 0;
				n[0] = 0;
				n[1] = 0;
				n[2] = 0;
				radius = 0.0f;
			}
		};

		/*
		struct plyFace {
			unsigned char nverts;    // number of vertex indices in list
			unsigned char nvels;    // number of vertex indices in list
			int *verts;              // vertex index list
			float* velocity;
			plyFace() {
				nverts = 0;
				verts = nullptr;
				velocity = nullptr;
				nvels = 3;
			}
		};
		*/
		struct plyFaceTexutre {
			unsigned char nverts;    // number of vertex indices in list
			unsigned char nvels;    // number of vertex indices in list

			int *verts;              // vertex index list
			float* velocity;
			unsigned char uvcount;
			float* uvs;
			plyFaceTexutre() {
				nverts = 0;
				uvs = nullptr;
				verts = nullptr;
				velocity = nullptr;
				uvcount = 6;
				nvels = 3;
			}
		};
		struct PlyProperty { /* description of a property */

			std::string name; /* property name */
			DataType external_type; /* file's data type */
			DataType internal_type; /* program's data type */
			int offset; /* offset bytes of prop in a struct */
			SectionType is_list; /* 0 = scalar, 1 = list, 2 = char string */
			DataType count_external; /* file's count type */
			DataType count_internal; /* program's count type */
			int count_offset; /* offset byte for list count */
		};
		struct PlyElement { /* description of an element */
			std::string name; /* element name */
			int num; /* number of elements in this object */
			int size; /* size of element (bytes) or -1 if variable */
			int nprops; /* number of properties for this element */
			std::vector<std::shared_ptr<PlyProperty>>  props; /* list of properties in the file */
			std::vector<char> store_prop; /* flags: property wanted by user? */
			int other_offset; /* offset to un-asked-for props, or -1 if none*/
			int other_size; /* size of other_props structure */
		};

		struct PlyOtherProp { /* describes other properties in an element */
			std::string name; /* element name */
			int size; /* size of other_props */
			int nprops; /* number of properties in other_props */
			std::vector<std::shared_ptr<PlyProperty>> props; /* list of properties in other_props */
		};

		struct OtherData { /* for storing other_props for an other element */
			std::vector<char> other_props;
		};

		struct OtherElem { /* data for one "other" element */
			std::string elem_name; /* names of other elements */
			int elem_count; /* count of instances of each element */
			std::vector<std::shared_ptr<OtherData>> other_data; /* actual property data for the elements */
			PlyOtherProp* other_props; /* description of the property data */
		};

		struct PlyOtherElems { /* "other" elements, not interpreted by user */
			int num_elems; /* number of other elements */
			std::vector<std::shared_ptr<OtherElem>> other_list; /* list of data for other elements */
		};
		struct PlyPropRules { /* rules for combining "other" properties */
			PlyElement *elem; /* element whose rules we are making */
			std::vector<Rule> rule_list; /* types of rules (AVERAGE_PLY, MAJORITY_PLY, etc.) */
			int nprops; /* number of properties we're combining so far */
			int max_props; /* maximum number of properties we have room for now */
			std::vector<void*> props; /* list of properties we're combining */
			std::vector<float> weights; /* list of weights of the properties */
		};


		struct PlyRuleElement {
			std::string name; /* name of the rule */
			std::string element; /* name of element that rule applies to */
			std::string property; /* name of property that rule applies to */
		};
		struct PlyRuleList: public std::list<
			std::shared_ptr<PlyRuleElement>> {
			std::string name; /* name of the rule */
			std::string element; /* name of element that rule applies to */
			std::string property; /* name of property that rule applies to */
		};

		struct PlyFile { /* description of PLY file */
			FileFormat file_type; /* ascii or binary */
			float version; /* version number of file */
			int num_elem_types; /* number of element types of object */
			std::vector<std::shared_ptr<PlyElement>> elems; /* list of elements */
			int num_comments; /* number of comments */
			std::vector<std::string> comments; /* list of comments */
			int num_obj_info; /* number of items of object information */
			std::vector<std::string> obj_info; /* list of object info items */
			PlyElement *which_elem; /* element we're currently reading or writing */
			std::shared_ptr<PlyOtherElems> other_elems; /* "other" elements from a PLY file */
			std::shared_ptr<PlyPropRules> current_rules; /* current propagation rules */
			std::shared_ptr<PlyRuleList> rule_list; /* rule list from user */
		};
		class PLYReaderWriter {
		public:
			static const int NO_OTHER_PROPS = -1;
			static const int DONT_STORE_PROP = 0;
			static const int STORE_PROP = 1;
			static const int OTHER_PROP = 0;
			static const int NAMED_PROP = 1;
			bool open(const std::string& file);
			bool close();

		protected:
			std::ofstream out;
			std::ifstream in;

			std::unique_ptr<PlyFile> plyFile;
			void describeProperty(const std::string&, PlyProperty *);
		
			void getProperty(const std::string&, PlyProperty *);
			void getElement( void *);
			
			PlyFile* openForReading(const std::string& fileName, std::vector<std::string>* elem_names);
			PlyFile* write(const std::vector<std::string>& elem_names,const FileFormat& file_type);
			void openForWriting(const std::string& fileName, const std::vector<std::string>& elem_names,const FileFormat& file_type);			
		
			void appendComment(const std::string&);
			void appendObjInfo(const std::string&);
			std::vector<std::string> getComments();
			std::vector<std::string> getObjInfo();
			std::vector<std::string> getElementList();
			void setupProperty(PlyProperty *);
			void setupOtherProps(PlyElement *);

			PlyOtherProp *getOtherProperties(PlyElement*,int);
			PlyOtherProp *getOtherProperties(const std::string&, int);
			PlyOtherProp *getOtherProperties(int);
			PlyOtherElems* getOtherElement();
			void elementCount(const std::string&, int);
			void elementLayout(const std::string& elem_name, int nelems,int nprops, PlyProperty *prop_list);
			void describeElement(const std::string&);
			void describeProperty( PlyProperty *);
			void describeOtherProperties( PlyOtherProp *, int);
			void describeOtherElements(PlyOtherElems *);
			void getElementSetup(const std::string& elem_name, std::vector<PlyProperty> prop_list);
			std::vector<std::shared_ptr<PlyProperty>> getElementDescription(const std::string& elem_name);
			void elementLayout(const std::string&, int, int, const std::vector<PlyProperty>& prop_list);

			void headerComplete();
			void putElementSetup(const std::string&);			
			void putOtherElements();

			PlyPropRules *initRule(const std::string&);
			void modifyRule(PlyPropRules *, const std::string&, const Rule&);
			void startProps(PlyPropRules *);
			void weightProps(float, void *);
			std::vector<char> getNewProps();
			void setPropRules(PlyRuleList *);
			PlyRuleList *appendPropRule(PlyRuleList *, const std::string&,const std::string&);
			/* find an element in a plyfile's list */
			PlyElement *findElement(const std::string&);

			/* find a property in an element's list */
			PlyProperty *findProperty(PlyElement *, const std::string&, int *);

			/* write to a file the word describing a PLY file data type */
			void writeScalarType(int);
			/* write to a file the word describing a PLY file data type */
			void writeScalarType(const DataType&);
			/* read a line from a file and break it up into separate words */
			std::vector<std::string> getWords();

			/* write an item to a file */
			void writeBinaryItem(int, unsigned int, double,const DataType& type);
			void writeAsciiItem(int, unsigned int, double, const DataType& type);

			/* add information to a PLY file descriptor */
			void addElement(const std::vector<std::string>&);
			void addProperty(const std::vector<std::string>& words);
			void addComment(const std::string&);
			void addObjInfo(const std::string&);
			DataType getPropType(const std::string& type_name);
			/* copy a property */
			void copyProperty(PlyProperty *, PlyProperty *);

			/* store a value into where a pointer and a type specify */
			void storeItem(char*, const DataType&, int, unsigned int, double);
			std::string setupElementRead(int index, int *elem_count);
			/* return the value of a stored item */
			 void getStoredItem(char *, const DataType&, int *, unsigned int *, double *);

			/* return the value stored in an item, given ptr to it and its type */
			double getItemValue(char*,const DataType&);
			/* get binary or ascii item and store it according to ptr and type */
			void getAsciiItem(const std::string&, const DataType&, int *, unsigned int *, double *);
			void getBinaryItem(const DataType& type, int *, unsigned int *, double *);

			/* get a bunch of elements from a file */
			void asciiGetElement(char*);
			void binaryGetElement(char*);
			void putElement(void *);
		};
	}
}
#endif
