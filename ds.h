#ifndef DS_H
#define DS_H

#include <stdarg.h>
#include <string.h>
#include "../common/error_enum.h"
#include "dstypes.h"

/*

Make sure to call Destroy() for any of these objects (when and if it is required) before you delete their pointers

*/

///////////////////////////////////////////////////////////////////////////////////////////////
class SERIALIZED_MEMORY;

enum SERIALIZE_FLAGS
{
	SERIALIZE_FLAG_REMEMBER_THIS=1,//remember this pointer so it can be resolved later
	SERIALIZE_FLAG_COMPLETE,//do complete serialization/deserialization of object
};

class OBJECT
{
	friend class SERIALIZED_MEMORY;
protected:
	void* m_address;
	int m_ref;
public:
	OBJECT() : m_ref(0) { m_address=this; }
	virtual ~OBJECT(){}

	static OBJECT* create()
	{
		OBJECT* o = new OBJECT;
		if(!o)
			throw 0;
		o->m_ref++;
		return o;
	}
	inline void deref()
	{
		if(m_ref>0)
		{
			m_ref--;
			if(m_ref==0)
				delete this;
		}
	}
	inline void deref2()
	{
		if (m_ref>0)
		{
			m_ref--;
			if (m_ref == 0)
			{
				if (*(int*)(this) == 0xdeadbeef)
				{
					int xx = 0;
					xx = 1;
				}
				*(int*)(this) = 0xdeadbeef;
				//delete this;
			}
		}
	}
	inline OBJECT* refptr() { m_ref++; return this; }
	inline int refcount() const { return m_ref; }
	inline void* address() const { return m_address; }
	inline void setAddress() { m_address=this; }

	virtual void serialize(SERIALIZED_MEMORY &mem, int flag);
	virtual void deserialize(SERIALIZED_MEMORY &mem, int flag);
};

///////////////////////////////////////////////////////////////////////////////////////////////

class EXCEPTION : public OBJECT
{
public:
	EXCEPTION(){}
	EXCEPTION(int error) { m_error=error; }
	virtual ~EXCEPTION(){}

	int m_error;
};

///////////////////////////////////////////////////////////////////////////////////////////////

typedef int(*VISITNODE)(OBJECT* obj, void * user_data);
typedef int (*COMPARATOR)(const OBJECT* visited_obj, const OBJECT* user_obj);
typedef void(*DESTROYNODE)(OBJECT* obj);
//typedef void(*PRINTMYDATA)(OBJECT* obj, char balancefactor, FILEIO &fp);

class TREE_NODE
{
public:
	TREE_NODE *m_left;
	TREE_NODE *m_right;
	TREE_NODE *m_parent;

	OBJECT *m_data;

	virtual TREE_NODE *get_left()=0;
	virtual TREE_NODE *get_right()=0;
	virtual TREE_NODE *get_parent()=0;

	TREE_NODE(){}
	virtual ~TREE_NODE(){}

	int IsLeaf() { if(!m_left && !m_right) return 1; else return 0; }
};

class ABSTRACT_TREE : public OBJECT
{
protected:
	int m_count;
	int m_error;

	int Traverse(VISITNODE visitnode, TREE_NODE* root, void * user_data, int traversal_type=TREE_PREORDER_TRAVERSAL);
	void Destroy(TREE_NODE* root, DESTROYNODE destroynode=0);
	int LocateNode(COMPARATOR comparator, OBJECT* p, TREE_NODE* root, TREE_NODE* &node);
	virtual TREE_NODE *get_root()=0;
public:
	enum TREE_TRAVERSAL
	{
		TREE_PREORDER_TRAVERSAL,
		TREE_POSTORDER_TRAVERSAL,
	};
	ABSTRACT_TREE(){ m_count=0; m_error=0; }
	virtual ~ABSTRACT_TREE(){}

	//LocateNode returns 0 and node points to the found node, otherwise return value is non-zero and node is null
	int LocateNode(COMPARATOR comparator, OBJECT** p);
	virtual int GetCount() const {return m_count;}
};

///////////////////////////////////////////////////////////////////////////////////////////////

class BINARY_TREE_NODE : public TREE_NODE
{
public:
	virtual TREE_NODE *get_left(){ return m_left; }
	virtual TREE_NODE *get_right(){ return m_right; }
	virtual TREE_NODE *get_parent(){ return m_parent; }

	BINARY_TREE_NODE() { m_data=0; m_right=0; m_left=0; m_parent=0; }
	BINARY_TREE_NODE(OBJECT *data) {m_data=data; m_right=0; m_left=0; m_parent=0; }
	virtual ~BINARY_TREE_NODE() {}
};

class BINARY_TREE : public ABSTRACT_TREE
{
protected:
	BINARY_TREE_NODE* m_root;

	BINARY_TREE_NODE* RemoveLeftMostChild(BINARY_TREE_NODE* root);
	BINARY_TREE_NODE* RemoveRightMostChild(BINARY_TREE_NODE* root);
public:
	int Insert(COMPARATOR comparator, OBJECT* p);
	OBJECT* Remove(COMPARATOR comparator, OBJECT* p);
	void Destroy(DESTROYNODE destroynode=0) { ABSTRACT_TREE::Destroy(m_root, destroynode); }
	//If the VISITNODE function returns 0, then traversal stops
	virtual int Traverse(VISITNODE visitnode, void * user_data, int traversal_type=TREE_PREORDER_TRAVERSAL)
	{ return ABSTRACT_TREE::Traverse(visitnode, m_root, user_data, traversal_type); }
	OBJECT* GetRootObject() { return m_root ? m_root->m_data:0; }
	virtual TREE_NODE* get_root(){ return (TREE_NODE*)m_root; }

	BINARY_TREE() : ABSTRACT_TREE() {m_root=0;}
	virtual ~BINARY_TREE(){}
};

///////////////////////////////////////////////////////////////////////////////////////////////

class AVL_TREE_NODE : public TREE_NODE
{
public:
	virtual TREE_NODE *get_left(){ return m_left; }
	virtual TREE_NODE *get_right(){ return m_right; }
	virtual TREE_NODE *get_parent(){ return m_parent; }
	unsigned int m_height;
	char m_balancefactor;

	AVL_TREE_NODE() { m_data=0; m_right=0; m_left=0; m_parent=0; m_height=0, m_balancefactor=0; }
	AVL_TREE_NODE(OBJECT *data) {m_data=data; m_right=0; m_left=0; m_parent=0; m_height=0, m_balancefactor=0; }
	virtual ~AVL_TREE_NODE() {}
};

class AVL_TREE : public ABSTRACT_TREE
{
protected:
	AVL_TREE_NODE* Insert(AVL_TREE_NODE* pnode, AVL_TREE_NODE* pparent, COMPARATOR avl_tree_function_compare, int &was_inserted);
	void TraversalInorder(AVL_TREE_NODE* pnode, VISITNODE avl_tree_function_traverse);
	//void Print(AVL_TREE_NODE* pnode, FILEIO &fp, int x, int y, PRINTMYDATA PrintMyData=0);
	void BalanceTree(AVL_TREE_NODE* pnode);
	AVL_TREE_NODE* BalanceSubTree(AVL_TREE_NODE* pnode);
	AVL_TREE_NODE* RotateRight(AVL_TREE_NODE* pnode);
	AVL_TREE_NODE* RotateLeft(AVL_TREE_NODE* pnode);
	void UpdateNodeAndParents(AVL_TREE_NODE* pnode);
	void UpdateNode(AVL_TREE_NODE* pnode);
	void DeleteNode(AVL_TREE_NODE* pdel);
	int Delete(void* OBJECT, AVL_TREE_NODE* pnode);

	AVL_TREE_NODE *m_root;

public:
	void Insert(OBJECT* pdata, COMPARATOR avl_tree_function_compare);
	void TraversalInorder(VISITNODE avl_tree_function_traverse);
	//void Print(FILEIO &fp, PRINTMYDATA PrintMyData=0);
	int PrintGetColumns();
	int PrintGetRows();
	unsigned int GetMaxHeight(){ return m_root ? m_root->m_height : 0; }
	int Delete(OBJECT* pdata);
	//If the VISITNODE function returns 0, then traversal stops
	virtual int Traverse(VISITNODE visitnode, void * user_data, int traversal_type=TREE_PREORDER_TRAVERSAL)
	{ return ABSTRACT_TREE::Traverse(visitnode, m_root, user_data, traversal_type); }
	void Destroy(DESTROYNODE destroynode=0) { ABSTRACT_TREE::Destroy(m_root, destroynode); }
	virtual TREE_NODE* get_root(){ return (TREE_NODE*)m_root; }

	AVL_TREE() : ABSTRACT_TREE() {m_root=0;}
	virtual ~AVL_TREE() {}
};

///////////////////////////////////////////////////////////////////////////////////////////////

class DS_NODE
{
public:
	OBJECT *m_data;
	DS_NODE *m_next;
	DS_NODE *m_prev;

	DS_NODE(){ m_data=0; m_next=0; m_prev=0; }
	DS_NODE(OBJECT *data){ m_data=data; m_next=0; m_prev=0; }
	virtual ~DS_NODE() {}
};

typedef DS_NODE* ITERATOR;

///////////////////////////////////////////////////////////////////////////////////////////////

class STACK
{
	DS_NODE *m_first;
	int m_count;
public:
	STACK(){m_first=0; m_count=0;}
	virtual ~STACK(){}
	int Push(OBJECT* p);
	OBJECT* Pop();
	void Destroy();
	int GetCount() const {return m_count;}
};

///////////////////////////////////////////////////////////////////////////////////////////////

class QUE
{
protected:
	DS_NODE *m_head;
	DS_NODE *m_tail;
	int m_max;
	int m_count;
public:
	QUE(){m_head=0; m_tail=0; m_count=0; m_max=-1;}
	virtual ~QUE(){}
	int Enque(OBJECT* p);
	OBJECT* Deque();
	void Destroy();
	int GetMax() { return m_max; }
	void SetMax(int max) { m_max = max; }
	int GetCount() const {return m_count;}
};

///////////////////////////////////////////////////////////////////////////////////////////////

class LIST : public OBJECT
{
	friend class SORTED_LIST;

private:
	DS_NODE *m_head;
	DS_NODE *m_tail;
	int m_count;

	int InsertNext(DS_NODE* m, OBJECT* p);
	int InsertPrev(DS_NODE* m, OBJECT* p);
	OBJECT* RemoveNext(DS_NODE* m);
	OBJECT* RemovePrev(DS_NODE* m);
	OBJECT* RemoveNode(DS_NODE* m);
	DS_NODE* GetNodeAt(int i) const;
	DS_NODE* head() const { return m_head; }
	DS_NODE* tail() const { return m_tail; }
public:
	LIST(){m_head=0; m_tail=0; m_count=0;}
	static LIST* create();
	virtual ~LIST() { if(m_ref==0) Destroy(0); }
	inline int Insert(OBJECT* p) { return Insert2(p, 0); }
	int Insert2(OBJECT *p, OBJECT ***ref);
	OBJECT* Replace(ITERATOR it, OBJECT* p);
	OBJECT* Remove(OBJECT* p);
	OBJECT* Remove(ITERATOR it);
	OBJECT* GetFirst(ITERATOR &it) const;
	OBJECT* GetNext(ITERATOR &it) const;
	OBJECT* GetLast(ITERATOR &it) const;
	OBJECT* GetPrev(ITERATOR &it) const;
	void Destroy(bool deref_data);
	int GetCount() const {return m_count;}
	OBJECT* operator[](int i) { return GetAt(i); }
	OBJECT* GetAt(int i) { DS_NODE* n=GetNodeAt(i); return n ? n->m_data:0; }
};

///////////////////////////////////////////////////////////////////////////////////////////////

class SORTED_LIST : public OBJECT
{
	LIST m_ll;
	DS_NODE** m_index;
	int m_index_size;

	int GetPosition(int &i, DS_NODE** n, int &res, OBJECT *p) const;
	DS_NODE* GetNodeAt(int i) const;
	inline int grow() { return (m_index_size<m_ll.m_count+1) ? growby(1024) : 1; }
	int growby(int inc);
public:
	SORTED_LIST() : OBJECT(), m_index(0), m_index_size(0) {}
	static SORTED_LIST* create();
	virtual ~SORTED_LIST() { if(m_ref==0) Destroy(); }
	virtual int Compare(const OBJECT* visited_obj, const OBJECT* user_obj) const = 0;

	inline int Insert(OBJECT *p, bool duplicate_ok) { return Insert2(p, 0, duplicate_ok); }
	int Insert2(OBJECT *p, OBJECT ***ref, bool duplicate_ok);
	int Find(OBJECT *p, OBJECT ***ref) const;
	OBJECT* Remove(OBJECT *p);
	OBJECT* Remove(int i);
	//OBJECT* Replace(int i, OBJECT* with);
	OBJECT* operator[](int i) { return GetAt(i); }
	OBJECT* GetAt(int i) const { DS_NODE* n=GetNodeAt(i); return n ? n->m_data:0; }
	void Destroy();
	int GetCount() const { return m_ll.GetCount(); }
	int InsertFirst(OBJECT *p, OBJECT ***ref);
	int InsertLast(OBJECT *p, OBJECT ***ref);
};

///////////////////////////////////////////////////////////////////////////////////////////////

class STRING : public OBJECT
{
	char_t *m_buffer;
	int m_bufferlen;
	int m_stringlen;
#ifdef UNICODE
	mutable char *m_ansistr;
#endif

	bool grow(int newlen, bool keepContent);

public:
	STRING();
	static STRING* create();
	STRING(const STRING &s);
	STRING(const char_t *s, const int size=-1)
		: m_buffer(0)
		, m_bufferlen(0)
		, m_stringlen(0)
#ifdef UNICODE
		, m_ansistr(0)
#endif
	{ 
		set(s, size); 
	}
	virtual ~STRING();
	bool init(int size=32);
	int set(const char_t *s, int size=-1);
	int set(STRING &s) { return set((const char_t*)s); }
	int append(const char_t ch);
	int append(const char_t *s, int size=-1);
	int prepend(const char_t *s, int size=-1);
	inline int append(const STRING &s) { return append((const char_t*)s); }
	inline int operator+=(const STRING &x) { return append(x); }
	inline char_t operator[](int i) { return m_buffer ? m_buffer[i] : 0; }
    inline char_t getAt(int i) { return m_buffer ? m_buffer[i] : 0; }
    inline void setAt(int i, char_t ch) { if(m_buffer && i>=0 && i<(int)strlenW(m_buffer)) {m_buffer[i]=ch; m_stringlen=(int)strlenW(m_buffer);}}
	int destroy();
	int clear();
	int substr(char_t *needle) const;
	char_t* strchr(char_t ch) const;
	char_t* strrchr(char_t ch) const;
	int strcmp(const char_t *s2) const;
	int strcmp(const char *s2);
	const char_t* strstr(const char_t *substring) const;
	bool operator==(const STRING & x);
	bool operator==(const char_t* x);
	bool operator!=(const STRING & x);
	bool operator!=(const char_t* x);
	STRING &operator=(const STRING & x);// { Set(x); return *this; }
	inline int size() const {return m_stringlen;}
	inline const char_t* s() const {return m_stringlen ? m_buffer:TXT("");}
	inline operator const char_t*() const {return m_stringlen ? m_buffer:TXT("");}
	int sprintf( const char_t *buffer, ... );
	int explode(char_t *delimiter, STRING *arr, int &num_strings);//fills an array of strings, each of which is a substring of this string formed by splitting it on boundaries formed by the delimiter
	int substr(STRING &out_string, int start, int length=-1);
	void rtrim(const char_t *s);//reverse trim, removes string s from end of string
    void ltrim(const char_t *s);//removes string s from beginning of string
    void trim(const char_t *s) { rtrim(s); ltrim(s); }
    int atoi() const;
    bool parseNameValuePair(char_t* name, char_t separator, int &value) const;
    bool parseNameValuePair(char_t* name, char_t separator, STRING &value) const;
    void replace(const char_t search, const char_t replace, int count);
    void rreplace(const char_t search, const char_t replace, int count);
	const char* ansi() const;
#ifdef UNICODE
	int set(const char* s, int size=-1);
#endif

	virtual void serialize(SERIALIZED_MEMORY &mem, int flag);
	virtual void deserialize(SERIALIZED_MEMORY &mem, int flag);
};

class STRING_LIST : public SORTED_LIST
{
	STRING* Find(const STRING &s, int first, int last);
	virtual int Compare(const OBJECT* visited_obj, const OBJECT* user_obj);
public:
	STRING* Find(const STRING &s) { int n=GetCount(); return n>0 ? Find(s, 0, n-1) : 0; }
};

class OBJECT_LIST : public SORTED_LIST
{
public:
	OBJECT* Find(const OBJECT* p, int first, int last);
	OBJECT* Find(const OBJECT* p) { int n=GetCount(); return n>0 ? Find(p, 0, n-1) : 0; }
	virtual int Compare(const OBJECT* v1/*visited_obj*/, const OBJECT* v2/*user_obj*/) { if(v1<v2) return 1; else if(v1>v2) return -1; else return 0; }
};

///////////////////////////////////////////////////////////////////////////////////

class ADDRESS_LIST : public SORTED_LIST
{
public:
	OBJECT* Find(const OBJECT* p, int first, int last);
	OBJECT* Find(const OBJECT* p) { int n=GetCount(); return n>0 ? Find(p, 0, n-1) : 0; }
	virtual int Compare(const OBJECT* v/*visited_obj*/, const OBJECT* u/*user_obj*/) const { if(v->address()<u->address()) return 1; else if(v->address()>u->address()) return -1; else return 0; }
};

//////////////////////////////////////////////////////////////////////////////////////////////////
class DS_KEY : public OBJECT
{
	friend class KEY_LIST;

	int m_key;
	OBJECT* m_value;
	DS_KEY(int key, OBJECT* value) : m_key(key), m_value(value) {}
public:
	static DS_KEY* create(int key, OBJECT* value);
	inline const OBJECT* Value() const { return m_value; }
};

class KEY_LIST
{
	LIST	m_keys;
public:
	virtual ~KEY_LIST();
	int Insert(int key, OBJECT* value);
	const OBJECT* Get(int key);
	void Destroy(bool deref_data);
};
//////////////////////////////////////////////////////////////////////////////////////////////////

class POINTER : public OBJECT
{
	OBJECT **m_pointer;//need to resolve this later
public:
	POINTER(OBJECT **pointer) : OBJECT(), m_pointer(pointer) {}
	inline OBJECT **getp() const { return m_pointer; }
	inline void setp(OBJECT *p) { *m_pointer=p; }
};

#define SERIALIZED_MEMORY_BLOCK_SIZE 4194304//4MB

class SERIALIZED_MEMORY
{
	char* m_buffer;
	int m_bytes;
	int m_pos;
	int m_num_unresolved;
	int m_time_diff;
	int m_block_size;

	bool grow(int bytes);

public:
	SERIALIZED_MEMORY(int blocksize=SERIALIZED_MEMORY_BLOCK_SIZE) : m_buffer(0), m_bytes(0), m_pos(0), m_num_unresolved(0), m_block_size(blocksize) {}
	~SERIALIZED_MEMORY();

	//How to Serialize
	//In the serialize() function of your object:
	//write() count of objects, then loop that many times to serialize each object
	//write() data variables
	//writep() object addresses, followed by remembera() to store the address in m_al
	//At the end of serialize() function, iterate through m_al, and serialize each object

	//How to Deserialize
	//In the deserialize() function of your object:
	//Make sure to call OBJECT::deserialize()
	//read() count of objects, then loop that many times to deserialize each object
	//read() data variables
	//readp() object addresses, followed by resolvep() to remember which address needs to be resolved later
	//At the end of deserialize() function, call resolvea() to resolve object addresses to the actual object in memory
	//The object in memory comes from m_al, OBJECT::deserialize() inserted the actual object pointer into m_al

	//m_al
	//deserializing: contains address as read from deserialization process and a corresponding object pointer
	//serializing: contains unresolved pointers encountered while serializing objects, because of calls to remembera() in serialize()
	ADDRESS_LIST m_al;
	LIST m_pl;//contains reference to address, it is replaced with corresponding object pointer from m_al in resolvea()

	inline bool load(char* buffer, int buffer_size) { m_buffer=buffer, m_bytes=buffer_size; m_pos=0; m_num_unresolved=0; return true; }
	bool load(char_t *file);
	bool resolvep(OBJECT** pointer);
	int resolvea();//resolve address to actual objects
	bool prepend(char* data, int bytes);
	bool write(char* data, int bytes);
	bool read(char* data, int bytes);
	inline void remembera(OBJECT *pointer) { if(pointer) m_al.Insert(pointer, false); }
	inline bool writep(OBJECT* pointer) { return write((char*)&pointer, sizeof(void*)); }
	inline bool readp(OBJECT** pointer) { return read((char*)pointer, sizeof(void*)); }
	inline void setpos(int i) { m_pos=i; }
	inline char* data() const { return m_buffer; }
	inline int pos() const { return m_pos; }
	inline void setTimeDiff(int time_diff) { m_time_diff=time_diff; }
	inline int timeDiff() const { return m_time_diff; }
	inline ADDRESS_LIST &al() { return m_al; }
#ifdef USE_ZLIB
	int compress();
	int decompress();
#endif//USE_ZLIB
};

///////////////////////////////////////////////////////////////////////////////////

int GetArgumentsSize(va_list args, const char_t *format);

#endif//DS_H
