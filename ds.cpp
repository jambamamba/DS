#include <stdio.h>
//#include <wchar.h>
//#include <memory.h>
#include <stdlib.h>
#include "ds.h"
#ifdef USE_ZLIB
#include "../zlib/zlib.h"
#endif

/////////////////////////////////////////////////////////////////////////////////////////////////////////////

int STACK::Push(OBJECT* p)
{
	DS_NODE *n;

	n = new DS_NODE(p);
	if(!n)
		throw 0;//

	if(!m_first)
	{
		m_first=n;
	}
	else
	{
		n->m_next=m_first;
		m_first->m_prev=n;
		m_first=n;
	}

	m_count++;

	return m_count;
}

OBJECT* STACK::Pop()
{
	OBJECT* ret=0;
	DS_NODE* head=m_first;
	if(head)
	{
		ret = head->m_data;
		m_first = m_first->m_next;
		if(m_first)
			m_first->m_prev=0;
		m_count--;
		delete head;
	}
	return ret;
}

void STACK::Destroy()
{
	DS_NODE *p=m_first;
	while(p)
	{
		DS_NODE *q=p;
		p=p->m_next;
		delete q;
	}
	m_first=0;
	m_count=0;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////

int QUE::Enque(OBJECT* p)
{
	DS_NODE *n;

	if(m_max>0 && m_count==m_max)
	{
		return 0;
	}

	n = new DS_NODE(p);
	if(!n)
		throw 0;//

	if(!m_head)
	{
		m_head=n;
		m_tail=n;
	}
	else
	{
		m_tail->m_next=n;
		n->m_prev=m_tail;
		m_tail=n;
	}

	m_count++;

	return m_count;
}

OBJECT* QUE::Deque()
{
	OBJECT* ret=0;
	DS_NODE* head=m_head;
	if(head)
	{
		ret = head->m_data;
		m_head = m_head->m_next;
		if(m_head)
			m_head->m_prev=0;
		m_count--;
		delete head;
	}
	return ret;
}

void QUE::Destroy()
{
	DS_NODE *p=m_head;
	while(p)
	{
		DS_NODE *q=p;
		p=p->m_next;
		delete q;
	}
	m_head=0;
	m_tail=0;
	m_count=0;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////

LIST* LIST::create()
{
	LIST* o = new LIST;
	if(!o)
		throw 0;
	o->m_ref++;
	return o;
}

int LIST::InsertNext(DS_NODE* m, OBJECT* p)
{
	if(!m)
		return 0;

	DS_NODE *n;

	n = new DS_NODE(p);
	if(!n)
		throw 0;//

	n->m_next=m->m_next;
	n->m_prev=m;
	m->m_next=n;
	if(n->m_next)
		n->m_next->m_prev=n;
	else
		m_tail=n;

	m_count++;

	return m_count;
}

int LIST::InsertPrev(DS_NODE* m, OBJECT* p)
{
	if(!m)
		return 0;

	DS_NODE *n;

	n = new DS_NODE(p);
	if(!n)
		throw 0;//

	n->m_prev=m->m_prev;
	m->m_prev=n;
	n->m_next=m;
	if(n->m_prev)
		n->m_prev->m_next=n;
	else
		m_head=n;

	m_count++;

	return m_count;
}

OBJECT* LIST::RemoveNext(DS_NODE* m)
{
	if(!m)
		return 0;

	OBJECT* ret=0;
	DS_NODE* n=m->m_next;

	if(!n)
		return 0;//nothing to remove

	m->m_next=n->m_next;
	if(m->m_next)
		m->m_next->m_prev=m;
	else
		m_tail=m;

	m_count--;

	ret=n->m_data;
	delete n;
	return ret;
}

OBJECT* LIST::RemovePrev(DS_NODE* m)
{
	if(!m)
		return 0;

	OBJECT* ret=0;
	DS_NODE* n=m->m_prev;

	if(!n)
		return 0;//nothing to remove

	m->m_prev=n->m_prev;
	if(m->m_prev)
		m->m_prev->m_next=m;
	else
		m_head=m;

	m_count--;

	ret=n->m_data;
	delete n;
	return ret;
}

int LIST::Insert2(OBJECT* p, OBJECT ***ref)
{
	DS_NODE *n;

	n = new DS_NODE(p);
	if(!n)
		return 0;

	if(!m_head)
	{
		m_head=n;
		m_tail=n;
	}
	else
	{
		m_tail->m_next=n;
		n->m_prev=m_tail;
		m_tail=n;
	}

	if(ref)
		*ref = &n->m_data;

	m_count++;

	return m_count;
}

OBJECT* LIST::Remove(OBJECT* p)
{
	OBJECT* ret=0;
	DS_NODE* i=m_head;
	while(i)
	{
		if(i->m_data!=p)
			i=i->m_next;
		else
		{
			ret = i->m_data;
			if(i->m_prev && i->m_next)
			{
				i->m_prev->m_next=i->m_next;
				i->m_next->m_prev=i->m_prev;
			}
			else if(i->m_prev)
			{//we are at the tail
				i->m_prev->m_next=0;
				m_tail=i->m_prev;
			}
			else if(i->m_next)
			{//we are at the head
				i->m_next->m_prev=0;
				m_head=i->m_next;
			}
			else
			{
				m_head=m_tail=0;
			}
			m_count--;
			delete i;
			break;
		}
	}
	return ret;
}

OBJECT* LIST::RemoveNode(DS_NODE* m)
{
	OBJECT* ret=0;

	if(m)
	{
		ret = m->m_data;
		if(m->m_prev && m->m_next)
		{
			m->m_prev->m_next=m->m_next;
			m->m_next->m_prev=m->m_prev;
		}
		else if(m->m_prev)
		{//we are at the tail
			m->m_prev->m_next=0;
			m_tail=m->m_prev;
		}
		else if(m->m_next)
		{//we are at the head
			m->m_next->m_prev=0;
			m_head=m->m_next;
		}
		else
		{
			m_head=m_tail=0;
		}
		m_count--;
		delete m;
	}

	return ret;
}

DS_NODE* LIST::GetNodeAt(int i) const
{
	int j;
	DS_NODE* n=0;

	if(i==m_count-1)
		return m_tail;
	else if(i==0)
		return m_head;

	for(j=1, n = m_head->m_next; j<m_count; j++, n = n->m_next)
	{
		if(j==i)
			break;
	}

	return j==i ? n : 0;
}

OBJECT* LIST::Replace(ITERATOR it, OBJECT* p)
{
	OBJECT* data;
	DS_NODE* iterator = (DS_NODE*) it;
	data = iterator->m_data;
	iterator->m_data = p;
	return data;
}

OBJECT* LIST::Remove(ITERATOR it)
{
	DS_NODE* iterator = (DS_NODE*) it;
	return RemoveNode(iterator);
}

OBJECT* LIST::GetFirst(ITERATOR &it) const
{
	OBJECT *ret=0;
	it=0;
	if(m_head)
	{
		ret=m_head->m_data;
		it=(ITERATOR)m_head;
	}
	return ret;
}

OBJECT* LIST::GetNext(ITERATOR &it) const
{
	OBJECT *ret=0;
	DS_NODE* iterator = (DS_NODE*) it;
	iterator=iterator->m_next;
	if(iterator)
	{
		ret=iterator->m_data;
		it = (ITERATOR)iterator;
	}
	return ret;
}

OBJECT* LIST::GetLast(ITERATOR &it) const
{
	OBJECT *ret=0;
	it=0;
	if(m_tail)
	{
		ret=m_tail->m_data;
		it=(ITERATOR)m_tail;
	}
	return ret;
}

OBJECT* LIST::GetPrev(ITERATOR &it) const
{
	OBJECT *ret=0;
	DS_NODE* iterator = (DS_NODE*) it;
	iterator=iterator->m_prev;
	if(iterator)
	{
		ret=iterator->m_data;
		it=(ITERATOR)iterator;
	}
	return ret;
}

void LIST::Destroy(bool deref_data)
{
	DS_NODE *p=m_head;
	while(p)
	{
		DS_NODE *q=p;
		p=p->m_next;
		if(deref_data && q->m_data)
			q->m_data->deref();
		delete q;
	}
	m_head=0;
	m_tail=0;
	m_count=0;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////

void SORTED_LIST::Destroy()
{
	if(m_index)
		delete [] m_index;
	m_index=0;
	m_index_size=0;
	m_ll.Destroy(0);
}

int SORTED_LIST::GetPosition(int &i, DS_NODE** n, int &res, OBJECT *p) const
{
	int ret=0;//return 1 if position is found, -1 for node already exists, 0 if not found
	int first=0;
	int last=m_ll.m_count;

	i=0;
	res=-2;

	if(!p)
		goto Exit;

	while(1)
	{
		if(first>=last || i<0 || i>=m_ll.m_count)
			break;

		i=(last-first)/2+first;

		*n=GetNodeAt(i);
		if(!*n)
			goto Exit;//should not happen

		res = Compare((*n)->m_data, p);

		if(i==first || i==last)
			break;

		if(res>0)
			first=i;
		else if(res<0)
			last=i;
		else
			break;
	}

	if(i<0)
		i=0;
	else if(m_ll.m_count>0 && i>=m_ll.m_count)
		i=m_ll.m_count-1;
	ret=1;

Exit:
	return ret;
}

int SORTED_LIST::growby(int inc)
{
	int newsize = m_ll.m_count+inc;
	DS_NODE** index = new DS_NODE* [newsize];
	if(!index)
		return 0;
	memset(index, 0, newsize*sizeof(DS_NODE*));
	if(m_ll.m_count)
		memcpy(index, m_index, m_ll.m_count*sizeof(DS_NODE*));
	if(m_index)
		delete [] m_index;
	m_index=index;
	m_index_size = newsize;

	return 1;
}

int SORTED_LIST::InsertFirst(OBJECT *p, OBJECT ***ref)
{
	if(!grow())
		return 0;

	memmove(&m_index[1], &m_index[0], m_ll.m_count*sizeof(DS_NODE*));

	if(!m_ll.Insert(p))
		return 0;

	m_index[0]=m_ll.m_tail;

	if(ref)
		*ref = &m_ll.tail()->m_data;

	return m_ll.m_count;
}

int SORTED_LIST::InsertLast(OBJECT *p, OBJECT ***ref)
{
	if(!grow())
		return 0;

	if(!m_ll.Insert(p))
		return 0;

	m_index[m_ll.m_count-1]=m_ll.m_tail;

	if(ref)
		*ref = &m_ll.tail()->m_data;

	return m_ll.m_count;
}

int SORTED_LIST::Find(OBJECT *p, OBJECT ***ref) const
{
	int i=0;
	int res=0;
	int pos;
	DS_NODE* n=0;

	if(!p)
		return 0;
	pos=GetPosition(i, &n, res, p);
	if(!pos)
		return 0;
	if(res==0)
	{
		if(ref)
			*ref = &m_index[i]->m_data;
		return 1;
	}
	return 0;
}

int SORTED_LIST::Insert2(OBJECT *p, OBJECT ***ref, bool duplicate_ok)
{
	int i=0;
	int res=0;
	int pos;
	DS_NODE* n=0;

	if(!p)
		return 0;
	pos=GetPosition(i, &n, res, p);
	if(!pos)
		return 0;
	if(res==0 && !duplicate_ok)
	{
		if(ref)
			*ref = &m_index[i]->m_data;
		return 0;
	}
	if(!grow())
		return 0;
	if(!m_ll.Insert(p))
		return 0;

	if(m_ll.m_count==1)
		m_index[0]=m_ll.m_tail;
	else
	{
		if(res>0)
		{
			if(m_ll.m_count-i-2>0)
				memmove(&m_index[i+2], &m_index[i+1], (m_ll.m_count-i-2)*sizeof(DS_NODE*));
			m_index[i+1]=m_ll.m_tail;
		}
		else
		{
			if(m_ll.m_count-i-1>0)
				memmove(&m_index[i+1], &m_index[i], (m_ll.m_count-i-1)*sizeof(DS_NODE*));
			m_index[i]=m_ll.m_tail;
		}
	}

	if(ref)
		*ref = &m_ll.tail()->m_data;

	return m_ll.m_count;
}

OBJECT* SORTED_LIST::Remove(OBJECT *p)
{
	OBJECT* ret=0;
	int i=0;
	int res=0;
	int pos;
	DS_NODE* n=0;

	if(!p)
		return 0;
	pos=GetPosition(i, &n, res, p);
	if(!pos)
		return 0;

	if(n && res==0)
	{
		ret = m_ll.RemoveNode(n);

		memmove(&m_index[i], &m_index[i+1], (m_ll.m_count-i)*sizeof(DS_NODE*));
	}

	return ret;
}

OBJECT* SORTED_LIST::Remove(int i)
{
	OBJECT* ret=0;
	DS_NODE* n=0;

	if(i>=0 && i<m_ll.m_count)
	{
		n=GetNodeAt(i);
		if(n)
		{
			ret = m_ll.RemoveNode(n);
			memmove(&m_index[i], &m_index[i+1], (m_ll.m_count-i)*sizeof(DS_NODE*));
		}
   }

	return ret;
}
/*
OBJECT* SORTED_LIST::Replace(int i, OBJECT* with)
{
	OBJECT* ret=0;
	DS_NODE* n=0;
	ITERATOR it;

	if(i>=0 && i<m_ll.m_count)
	{
		n=GetNodeAt(i);
		if(n)
		{
			ret = m_ll.Replace(it, with);
		}
	}

	return ret;
}
*/
DS_NODE* SORTED_LIST::GetNodeAt(int i) const
{
	if(m_index && i>=0 && i<m_ll.m_count && m_index[i])
		return m_index[i];
	else
		return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////

int ABSTRACT_TREE::Traverse(VISITNODE visitnode, TREE_NODE* root, void * user_data, int traversal_type)
{
	int ret=1;
	TREE_NODE* current=root;
	TREE_NODE* child1;
	TREE_NODE* child2;

	if(!visitnode) { m_error=ERR_TREE_NO_VISIT_FUNCTION; return -1; }
	if(!current) return -1;

	if(traversal_type==TREE_PREORDER_TRAVERSAL)
	{
		child1=current->m_left;
		child2=current->m_right;
	}
	else//if(traversal_type==TREE_POSTORDER_TRAVERSAL)
	{
		child1=current->m_right;
		child2=current->m_left;
	}

	if(ret!=0 && child1)
		ret=Traverse(visitnode, child1, user_data, traversal_type);

	if(ret!=0)
		ret=visitnode(current->m_data, user_data);

	if(ret!=0 && child2)
		ret=Traverse(visitnode, child2, user_data, traversal_type);

	return ret;
}

void ABSTRACT_TREE::Destroy(TREE_NODE* root, DESTROYNODE destroynode)
{
	TREE_NODE* current=root;

	if(!current)
		return;

	if(current->m_left)
		Destroy(current->m_left, destroynode);
	if(current->m_right)
		Destroy(current->m_right, destroynode);
	if(current->m_parent)
	{
		if(current->m_parent->m_left==current)
			current->m_parent->m_left=0;
		else if(current->m_parent->m_right==current)
			current->m_parent->m_right=0;
	}

	if(destroynode)
		destroynode(current->m_data);

	delete current;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////

int BINARY_TREE::Insert(COMPARATOR comparator, OBJECT* p)
{
	int ret;
	BINARY_TREE_NODE* current=m_root;

	if(!p) { m_error=ERR_TREE_NOTHING_TO_INSERT; return 0; }
	if(!comparator) { m_error=ERR_TREE_NO_COMPARE_FUNCTION; return 0; }

	if(!current)
	{
		m_root=new BINARY_TREE_NODE(p);
		if(!m_root)
			throw 0;
	}
	else while(1)
	{
		ret=comparator(current->m_data, p);

		if(ret>0)
		{
			if(current->m_right)
				current=(BINARY_TREE_NODE*)current->get_right();
			else
			{
				current->m_right=new BINARY_TREE_NODE(p);
				if(!current->m_right)
					throw 0;
				current->m_right->m_parent=current;
				break;
			}
		}
		else//if(ret<=0)
		{
			if(current->m_left)
				current=(BINARY_TREE_NODE*)current->get_left();
			else
			{
				current->m_left=new BINARY_TREE_NODE(p);
				if(!current->m_left)
					throw 0;
				current->m_left->m_parent=current;
				break;
			}
		}
	}
	m_count++;
	return m_count;
}

BINARY_TREE_NODE* BINARY_TREE::RemoveLeftMostChild(BINARY_TREE_NODE* root)
{
	BINARY_TREE_NODE* node=root;
	BINARY_TREE_NODE* parent=(BINARY_TREE_NODE*)node->get_parent();
	if(!node)
		goto Exit;
	while(node->m_left)
	{
		parent=node;
		node=(BINARY_TREE_NODE*)node->get_left();
	}
	if(parent)
	{
		if(parent->m_left==node)
			parent->m_left=node->m_right;
		else
			parent->m_right=node->m_right;
	}
	else if(node==m_root)
	{
		m_root=(BINARY_TREE_NODE*)node->get_right();
	}
	if(node->m_right)
	{
		node->m_right->m_parent=parent;
		node->m_right=0;
	}
	node->m_parent=0;
Exit:
	return node;
}

BINARY_TREE_NODE* BINARY_TREE::RemoveRightMostChild(BINARY_TREE_NODE* root)
{
	BINARY_TREE_NODE* node=root;
	BINARY_TREE_NODE* parent=(BINARY_TREE_NODE*)node->get_parent();
	if(!node)
		goto Exit;
	while(node->m_right)
	{
		parent=node;
		node=(BINARY_TREE_NODE*)node->get_right();
	}
	if(parent)
	{
		if(parent->m_right==node)
			parent->m_right=node->m_left;
		else
			parent->m_left=node->m_left;
	}
	else if(node==m_root)
	{
		m_root=(BINARY_TREE_NODE*)node->get_left();
	}
	if(node->m_left)
	{
		node->m_left->m_parent=parent;
		node->m_left=0;
	}
	node->m_parent=0;
Exit:
	return node;
}

int ABSTRACT_TREE::LocateNode(COMPARATOR comparator, OBJECT** p)
{
	int ret;
	TREE_NODE *node;
	ret=LocateNode(comparator, *p, get_root(), node);
	if(ret==0)
		*p=node->m_data;
	return ret;
}

int ABSTRACT_TREE::LocateNode(COMPARATOR comparator, OBJECT* p, TREE_NODE* root, TREE_NODE* &node)
{
	int ret=0;
	if(!comparator) {m_error=ERR_TREE_NO_COMPARE_FUNCTION; goto Exit;}
	else if(!p) {m_error=ERR_TREE_NO_OBJECT_TO_MATCH; goto Exit;}
	else if(!root) {m_error=ERR_TREE_EMPTY; goto Exit;}

	ret=comparator(root->m_data, p);
	if(ret==0)
		node=root;
	else if(ret>0)
		ret=LocateNode(comparator, p, root->get_right(), node);
	else if(ret<0)
		ret=LocateNode(comparator, p, root->get_left(), node);
Exit:
	return ret;
}

OBJECT* BINARY_TREE::Remove(COMPARATOR comparator, OBJECT* p)
{
	OBJECT *obj=0;
	TREE_NODE* del_node=0;
	BINARY_TREE_NODE* replacement=0;

	if(!comparator || !p || !m_root)
		goto Exit;
	if(0!=LocateNode(comparator, p, m_root, del_node) || !del_node)
		goto Exit;
	if(del_node->m_left)
		replacement=RemoveRightMostChild((BINARY_TREE_NODE*)del_node->get_left());
	else if(del_node->m_right)
		replacement=RemoveLeftMostChild((BINARY_TREE_NODE*)del_node->get_right());
	if(replacement)
	{//replace del_node with its replacement node which is a leaf node that was removed from the tree
		obj=del_node->m_data;
		del_node->m_data=replacement->m_data;
		delete replacement;
	}
	else if(del_node->m_parent)
	{//when the del_node has no children, it is a leaf
		if(del_node->m_parent->m_left==del_node)
			del_node->m_parent->m_left=0;
		else
			del_node->m_parent->m_right=0;
		obj=del_node->m_data;
		delete del_node;
	}
	else
	{
		obj=del_node->m_data;
		delete del_node;
		m_root=0;
	}
	m_count--;
Exit:
	return obj;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////

void AVL_TREE::Insert(OBJECT* pdata, COMPARATOR avl_tree_function_compare)
{
	int was_inserted=0;

	if(pdata && avl_tree_function_compare)
	{
		AVL_TREE_NODE *pnode = new AVL_TREE_NODE;
		pnode->m_data=pdata;
		pnode->m_height=0;
		pnode->m_balancefactor=0;
		pnode->m_left=0;
		pnode->m_parent=0;
		pnode->m_right=0;

		m_root=Insert(pnode, m_root, avl_tree_function_compare, was_inserted);
		if(was_inserted)
		{
			UpdateNodeAndParents(pnode);
			BalanceTree(pnode);
		}
	}
}

AVL_TREE_NODE* AVL_TREE::Insert(AVL_TREE_NODE* pnode, AVL_TREE_NODE* pparent, COMPARATOR avl_tree_function_compare, int &was_inserted)
{
	if(!pparent)
	{//we have gone past the leaf node, so we are ready to insert new node.
		m_count++;
		return pnode;
	}

	int res=avl_tree_function_compare(pparent->m_data, pnode->m_data);

	if(res==0)//already exists in tree
	{
		was_inserted=0;
	}
	else if(res<0)
	{
		AVL_TREE_NODE* ret=Insert(pnode, (AVL_TREE_NODE*)pparent->get_left(), avl_tree_function_compare, was_inserted);
		if(pnode==ret)
		{//the returned child node is the same as the one we want to insert, so insert it as left child of parent node.
			pparent->m_left=pnode;
			pnode->m_parent=pparent;
			was_inserted=1;
		}
	}
	else//res>0
	{
		AVL_TREE_NODE* ret=Insert(pnode, (AVL_TREE_NODE*)pparent->get_right(), avl_tree_function_compare, was_inserted);
		if(ret==pnode)
		{//the returned child node is the same as the one we want to insert, so insert it as right child of parent node.
			pparent->m_right=pnode;
			pnode->m_parent=pparent;
			was_inserted=1;
		}
	}

	return pparent;
}

//Balance upwards from pnode
//
void AVL_TREE::BalanceTree(AVL_TREE_NODE* pnode)
{
	AVL_TREE_NODE *pparent;
	int i;

	for(i=0; pnode; i++)
	{
		pparent = (AVL_TREE_NODE*)pnode->get_parent();

		if(pparent)
		{
			if(pparent->m_left==pnode)
				pparent->m_left=BalanceSubTree(pnode);
			else if(pparent->m_right==pnode)
				pparent->m_right=BalanceSubTree(pnode);
		}
		else
		{
			m_root = BalanceSubTree(pnode);
		}
		UpdateNodeAndParents(pnode);
		pnode=pparent;
	}
}

//
AVL_TREE_NODE* AVL_TREE::BalanceSubTree(AVL_TREE_NODE* pnode)
{
	AVL_TREE_NODE* baltree=pnode;
	AVL_TREE_NODE* pparent, *left, *right;

	if(pnode)
	{
		pparent=(AVL_TREE_NODE*)pnode->get_parent();

		if(pnode->m_balancefactor == -2 && pnode->m_left)
		{
			left = (AVL_TREE_NODE*)pnode->get_left();
			if(left->m_balancefactor == -1)
			{
				baltree = RotateRight(pnode);
			}
			else if(left->m_balancefactor == 1 ||
				left->m_balancefactor == 0)
			{
				AVL_TREE_NODE* lc = (AVL_TREE_NODE*)pnode->get_left();
				pnode->m_left = RotateLeft(lc);
				pnode->m_left->m_parent=pnode;
				UpdateNodeAndParents(lc);
				baltree = RotateRight(pnode);
			}
		}
		else if(pnode->m_balancefactor == 2 && pnode->m_right)
		{
			right = (AVL_TREE_NODE*)pnode->get_right();
			if(right->m_balancefactor == 1)
			{
				baltree = RotateLeft(pnode);
			}
			else if(right->m_balancefactor == -1 ||
				right->m_balancefactor == 0)
			{
				AVL_TREE_NODE* rc = (AVL_TREE_NODE*)pnode->get_right();
				pnode->m_right = RotateRight(rc);
				pnode->m_right->m_parent=pnode;
				UpdateNodeAndParents(rc);
				baltree = RotateLeft(pnode);
			}
		}

		if(pnode!=baltree)
		{//some rotation has taken place
			baltree->m_parent=pparent;
		}
	}

	return baltree;
}

AVL_TREE_NODE* AVL_TREE::RotateLeft(AVL_TREE_NODE* pnode)
{
	AVL_TREE_NODE* pright = (AVL_TREE_NODE*)pnode->get_right();
	AVL_TREE_NODE* pparent = (AVL_TREE_NODE*)pnode->get_parent();
	pnode->m_right=pright->m_left;
	if(pnode->m_right)
		pnode->m_right->m_parent=pnode;
	pright->m_left=pnode;
	pright->m_left->m_parent=pright;
	pright->m_parent=pparent;
	return pright;
}

AVL_TREE_NODE* AVL_TREE::RotateRight(AVL_TREE_NODE* pnode)
{
	AVL_TREE_NODE* pleft = (AVL_TREE_NODE*)pnode->get_left();
	AVL_TREE_NODE* pparent = (AVL_TREE_NODE*)pnode->get_parent();
	pnode->m_left=pleft->m_right;
	if(pnode->m_left)
		pnode->m_left->m_parent=pnode;
	pleft->m_right=pnode;
	pleft->m_right->m_parent=pleft;
	pleft->m_parent=pparent;
	return pleft;
}

void AVL_TREE::UpdateNode(AVL_TREE_NODE* pnode)
{
	if(!pnode)
		return;

	AVL_TREE_NODE *left=(AVL_TREE_NODE *)pnode->m_left;
	AVL_TREE_NODE *right=(AVL_TREE_NODE *)pnode->m_right;
	AVL_TREE_NODE *pleft, *pright;

	if(left && right)
	{
		pleft=(AVL_TREE_NODE *)pnode->get_left();
		pright=(AVL_TREE_NODE *)pnode->get_right();

		int height = pleft->m_height > pright->m_height ? pleft->m_height : pright->m_height;
		pnode->m_height = height+1;
		pnode->m_balancefactor = pright->m_height - pleft->m_height;
	}
	else if(left)
	{
		pleft=(AVL_TREE_NODE *)pnode->get_left();

		pnode->m_height = pleft->m_height+1;
		pnode->m_balancefactor = -1 - pleft->m_height;
	}
	else if(right)
	{
		pright=(AVL_TREE_NODE *)pnode->get_right();

		pnode->m_height = pright->m_height+1;
		pnode->m_balancefactor = pright->m_height + 1;
	}
	else
	{
		pnode->m_height = 0;
		pnode->m_balancefactor=0;
	}
}

void AVL_TREE::UpdateNodeAndParents(AVL_TREE_NODE* pnode)
{
	while(pnode)
	{
		UpdateNode(pnode);
		pnode=(AVL_TREE_NODE*)pnode->get_parent();
	}
}

void AVL_TREE::DeleteNode(AVL_TREE_NODE* pdel)
{
	if(!pdel)
		return;

	AVL_TREE_NODE* parent=0;
	AVL_TREE_NODE* pnode=0;
	AVL_TREE_NODE* temp_node=0;
	OBJECT* temp_data=0;

	if(!pdel->m_left && !pdel->m_right)
	{//node to be deleted has no children
		if(pdel->m_parent)
			parent=(AVL_TREE_NODE*)pdel->get_parent();
		else//node to be deleted also does not have a parent, so it is root node
			m_root=0;
	}
	else if(!pdel->m_right)//only has left subtree
	{
		//replace pdel with left-subtree's right most child

		//find right most child of the left subtree
		for(pnode=(AVL_TREE_NODE*)pdel->get_left(); pnode->m_right; pnode=(AVL_TREE_NODE*)pnode->get_right());
		//move this node data to take place of the deleted node data
		temp_data=pdel->m_data;
		pdel->m_data=pnode->m_data;
		pnode->m_data=temp_data;
		//swap pointers
		temp_node=pdel;
		pdel=pnode;
		pnode=temp_node;

		parent=(AVL_TREE_NODE*)pdel->get_parent();
	}
	else //has right subtree or has both left and right subtrees
	{
		//replace pdel with right-subtree's left most child

		//find left most child of the right subtree
		for(pnode=(AVL_TREE_NODE*)pdel->get_right(); pnode->m_left; pnode=(AVL_TREE_NODE*)pnode->get_left());
		//move this node data to take place of the deleted node data
		temp_data=pdel->m_data;
		pdel->m_data=pnode->m_data;
		pnode->m_data=temp_data;
		//swap pointers
		temp_node=pdel;
		pdel=pnode;
		pnode=temp_node;

		parent=(AVL_TREE_NODE*)pdel->get_parent();
	}

	//see if the new node that we are deleting has any children
	if(pdel->m_right && pdel->m_left)
		throw 0;//should not happen because we swaped the delete node with the right|left most node
	else if(pdel->m_right)
		pnode=(AVL_TREE_NODE*)pdel->get_right();
	else if(pdel->m_left)
		pnode=(AVL_TREE_NODE*)pdel->get_left();
	else
		pnode=0;
	//pnode, the child of the node to be deleted,
	//should not be having any children in a balanced tree
	//because pnode already does not have any sibling, so having a child
	//at pnode would mean tree is unbalanced.
	if(pnode)
	{
		if(pnode->m_right || pnode->m_left)
			throw 0;//should not happen in a balanced tree
		pnode->m_height--;//it will be moved up the tree
	}

	if(parent)
	{
		if(parent->m_left==pdel)
			parent->m_left=pnode;//pnode is the child of pdel and it is a leaf
		else//right child
			parent->m_right=pnode;//pnode is the child of pdel and it is a leaf

		UpdateNodeAndParents(parent);

		BalanceTree(parent);
	}

	m_count--;
	delete pdel;
}

int AVL_TREE::Delete(void* pdata, AVL_TREE_NODE* pnode)
{
	int ret=0;

	if(!pnode)
		return ret;

	if(pdata==pnode->m_data)
	{
		DeleteNode(pnode);
		ret = 1;
	}
	else
	{
		ret = Delete(pdata, (AVL_TREE_NODE*)pnode->get_left());
		if(0==ret)//no need to check the right side if ret==1 from left side
			ret = Delete(pdata, (AVL_TREE_NODE*)pnode->get_right());
	}
	return ret;
}

int AVL_TREE::Delete(OBJECT* pdata)
{
	return Delete(pdata, m_root);
}

void AVL_TREE::TraversalInorder(VISITNODE avl_tree_function_traverse)
{
	if(avl_tree_function_traverse)
	{
		TraversalInorder(m_root, avl_tree_function_traverse);
	}
}

void AVL_TREE::TraversalInorder(AVL_TREE_NODE* pnode, VISITNODE avl_tree_function_traverse)
{
	if(pnode)
	{
		//avl_tree_function_traverse(pnode->m_data);
		FILE* fp = fopen("out.txt", "wt");
		if(fp)
		{
			fprintf(fp, "%s [%i:%i]\n", (char*) pnode->m_data, pnode->m_balancefactor, pnode->m_height);
			fclose(fp);
		}
		TraversalInorder((AVL_TREE_NODE*)pnode->get_left(), avl_tree_function_traverse);
		TraversalInorder((AVL_TREE_NODE*)pnode->get_right(), avl_tree_function_traverse);
	}
}

int AVL_TREE::PrintGetColumns()
{
	//int cols = (int) (10*pow(2, m_root->m_height)/*number of leafs*/ );
	int cols = (int) (10*(1 << (m_root->m_height))/*number of leafs*/ );
	return cols;
}

int AVL_TREE::PrintGetRows()
{
	int rows = m_root->m_height+1;
	return rows;
}
/*
//Cannot use FILEIO!
void AVL_TREE::Print(FILEIO &fp, PRINTMYDATA PrintMyData)
{
	//fprintf(fp, "[%i,%i]\r\n", m_root->m_height, m_root->m_balancefactor);
	//return;

	int rows = PrintGetRows();
	int cols = PrintGetColumns();
	int x=cols/2;
	int y=0;
	int i;
	char *blankline = new char [cols];

	memset(blankline, '.', cols);
	blankline[cols-1]=0;

	for(i=0; i<rows; i++)
		fp.printf(TXT("%s\r\n"), blankline);

	Print(m_root, fp, x, y, PrintMyData);

	delete [] blankline;
}

//Cannot use FILEIO!
void AVL_TREE::Print(AVL_TREE_NODE* pnode, FILEIO &fp, int x, int y, PRINTMYDATA PrintMyData)
{
	int cols = PrintGetColumns();
	//static count=0;
	if(pnode)
	{
		fp.seek(y*(cols+2)+x, FILEIO_BEGIN);
		//fprintf(fp, "%s", "x");
		//fprintf(fp, "%s", (char*)pnode->m_data);
		//fprintf(fp, "%c", *(char*)pnode->m_data);
		//fprintf(fp, "%i,%i", x,y);
		//fprintf(fp, "%i", pnode->m_height);
		//fprintf(fp, "%i", *(int*)pnode->m_data);
		//fprintf(fp, "%i(%i,%i)", *(int*)pnode->m_data, pnode->m_balancefactor, pnode->m_height);
		if(PrintMyData)
			PrintMyData(pnode->m_data, pnode->m_balancefactor, fp);
		else
			fp.printf(TXT("%s(%i)"), (char*)pnode->m_data, pnode->m_balancefactor);
		//fprintf(fp, "%s(%i,%i)", (char*)pnode->m_data, pnode->m_balancefactor, pnode->m_height);
		//fprintf(fp, "x(%i,%i)", pnode->m_balancefactor, pnode->m_height);
		//fprintf(fp, "%i", count++);

		//int offset = (int) (cols/(2*pow(2,y+1)));
		int offset = (int) (cols/(1<<(y+1)));
		Print((AVL_TREE_NODE*)pnode->get_left(), fp, x-offset/2, y+1, PrintMyData);
		Print((AVL_TREE_NODE*)pnode->get_right(), fp, x+offset/2, y+1, PrintMyData);
	}
}
*/
/////////////////////////////////////////////////////////////////////////////////////////////////////////////

STRING::STRING()
	: m_buffer(0)
	, m_bufferlen(0)
	, m_stringlen(0)
#ifdef UNICODE
	, m_ansistr(0)
#endif
{
	init();
}

bool STRING::init(int size)
{
	if(size<=0)
		return false;
	m_bufferlen = size*sizeof(char_t);
	char_t* buffer = m_buffer;
	m_buffer = (char_t*) realloc(buffer, m_bufferlen);
	if(!m_buffer)
	{
		if(buffer)
			free(buffer);
		m_bufferlen = 0;
		m_stringlen = 0;
		return false;
	}
	m_buffer[0]=0;
	return true;
}

STRING::STRING(const STRING &s)
	: m_buffer(0)
	, m_bufferlen(0)
	, m_stringlen(0)
#ifdef UNICODE
	, m_ansistr(0)
#endif
{
	set((const char_t*)s);
}

STRING& STRING::operator=(const STRING &x)
{
	set((const char_t*)x);
	return *this;
}

STRING::~STRING()
{
	destroy();
}

STRING* STRING::create()
{
	STRING* o = new STRING;
	if(!o)
		throw 0;
	o->m_ref++;
	return o;
}

int STRING::destroy()
{
	if(m_buffer)
		free(m_buffer);
#ifdef UNICODE
	if(m_ansistr)
		free(m_ansistr);
	m_ansistr = 0;
#endif
	m_bufferlen = 0;
	m_stringlen = 0;
	return 1;
}

int STRING::clear()
{
	if(m_buffer && m_bufferlen)
		memset((char*)m_buffer, 0, m_bufferlen);
	m_stringlen=0;
	return 1;
}

#define STRING_BLOCK_SIZE 256
bool STRING::grow(int newlen, bool keepContent)
{
	int bufferlen = m_bufferlen;
	m_bufferlen = newlen;
	int f1 = m_bufferlen/STRING_BLOCK_SIZE;
	int b1 = f1*STRING_BLOCK_SIZE;
	if(b1<m_bufferlen)
		f1++;
	m_bufferlen = f1*STRING_BLOCK_SIZE;
	char_t* buffer = m_buffer;
	buffer = (char_t*) realloc(m_buffer, m_bufferlen);
	if(!buffer)
	{
		if(m_buffer)
			free(m_buffer);
		m_bufferlen = 0;
		m_stringlen = 0;
		return false;
	}
	else
		m_buffer = buffer;
	if(!keepContent && m_buffer && bufferlen)
	{
		memset(m_buffer, 0, bufferlen);
		m_stringlen=0;
	}
	return true;
}

int STRING::set(const char_t *s, int size)
{
	int len = 0;

	if(s==m_buffer)
		return len;

	clear();

	len = size = (size<0) ? (int) strlenW(s) : size;
	if(len && s && *s)
	{
		len = (len+1)*sizeof(char_t);
		if(m_bufferlen<len)
			if(!grow(len, false))
				return 0;
		memset(m_buffer, 0, m_bufferlen);
		memcpy((char*)m_buffer, (char*)s, size*sizeof(char_t));
		m_stringlen=size;
		m_stringlen = strlenW(m_buffer);
	}

	return len;
}

int STRING::append(const char_t ch)
{
	if((m_stringlen+1)*(int)sizeof(char_t)==m_bufferlen)
	{
		if(!grow(m_bufferlen+sizeof(char_t), true))
			return 0;
	}
	m_buffer[m_stringlen]=ch;
	m_stringlen++;
	m_buffer[m_stringlen]=0;

	return m_stringlen;
}

int STRING::append(const char_t *s, int size)
{
	int len =0;
	int bytes = 0;

	if(s && *s)
	{
		len = size==-1 ? (int) strlenW(s) : size;
		if(len)
		{
			bytes = (m_stringlen+len+1)*sizeof(char_t);
			if(m_bufferlen<bytes)
				if(!grow(bytes, true))
					return 0;

			memcpy((char*)&m_buffer[m_stringlen], (char*)s, len*sizeof(char_t));
			m_stringlen+=len;
			m_buffer[m_stringlen]=0;
		}
	}

	return len;
}

int STRING::prepend(const char_t *s, int size)
{
	int len =0;
	int newsize=0;

	if(s && *s)
	{
		len = size==-1 ? (int) strlenW(s) : size;
		if(len)
		{
			size = len*sizeof(char_t);
			newsize = size+m_stringlen*sizeof(char_t);
			if(m_bufferlen<newsize)
				if(!grow(newsize, true))
					return 0;

			memmove((char*)(m_buffer)+size, (char*)m_buffer, m_stringlen*sizeof(char_t));
			memcpy((char*)m_buffer, (char*)s, size);
			m_stringlen+=len;
			m_buffer[m_stringlen]=0;
		}
	}

	return len;
}

namespace
{
	bool equal(const char_t* s1, int s1_len, const char_t* s2, int s2_len)
	{
		if (!s1 || !s2 || s1_len != s2_len)
		{
			return false;
		}

		int i = 0;
		for (; i < s1_len; i++)
		{
			if (s1[i] != s2[i])
			{
				break;
			}
		}

		if (i == s1_len)
		{
			return true;//they are equal
		}
	}
}//namespace
bool STRING::operator==(const STRING & x)
{
	return equal(m_buffer, m_stringlen, x.m_buffer, x.m_stringlen);
}

bool STRING::operator==(const char_t* x)
{
	return equal(m_buffer, m_stringlen, x, strlenW(x));
}

bool STRING::operator!=(const STRING & x)
{
	return !equal(m_buffer, m_stringlen, x.m_buffer, x.m_stringlen);
}

bool STRING::operator!=(const char_t* x)
{
	return !equal(m_buffer, m_stringlen, x, strlenW(x));
}

int STRING::substr(char_t *needle) const//never tested!
{
	int ret=-1;
	int i;
	if(m_buffer && m_stringlen && needle)
	{
		for(i=0; i<m_stringlen; i++)
		{
			int j = 0;
			while(needle[j] && (i+j < m_stringlen))
			{
				if(needle[j]==m_buffer[i+j])
					j++;
			}
			if(needle[j]==0 && j!=0)
			{
				ret = i;
				break;
			}
		}
	}
	return ret;
}

char_t* STRING::strchr(char_t ch) const
{
	int i;
	if(m_buffer && m_stringlen)
	{
		for(i=0; i<m_stringlen; i++)
		{
			if(ch==m_buffer[i])
				break;
		}
		if(i<m_stringlen)
			return &m_buffer[i];
	}
	return NULL;
}

char_t* STRING::strrchr(char_t ch) const
{
	int i;
	if(m_buffer && m_stringlen)
	{
		for(i=m_stringlen-1; i>=0; i--)
		{
			if(ch==m_buffer[i])
				break;
		}
		if(i>-1)
			return &m_buffer[i];
	}
	return NULL;
}

int STRING::atoi() const
{
    return ::atoiW(m_buffer);
}

bool STRING::parseNameValuePair(char_t* name, char_t separator, STRING &value) const
{
    const char_t* pair = strstr(name);
    if(pair)
    {
        STRING p(pair);
        p.rtrim(TXT(" "));
        p.rtrim(TXT("\t"));
        p.rtrim(TXT("\n"));

        const char_t *t = p.strchr(separator);
        if(t)
        {
            t++;
            STRING t1(t);
            t1.trim(TXT(" "));
            t1.trim(TXT("\t"));
            t1.trim(TXT("\n"));
            value = t1;
            return true;
        }
    }

    return false;
}

bool STRING::parseNameValuePair(char_t* name, char_t separator, int &value) const
{
    STRING _value;
    bool ret = parseNameValuePair(name, separator, _value);
    if(ret)
    {
        value = _value.atoi();
    }
    return ret;
}

void STRING::rtrim(const char_t *s)
{
	if(s)
	{
		int t = strlenW(s);
		if(t<=m_stringlen)
      {
			while(memcmp(&m_buffer[m_stringlen-t], s, t*sizeof(char_t))==0)
         {
            m_stringlen-=t;
				m_buffer[m_stringlen]=0;
         }
      }
	}
}

void STRING::ltrim(const char_t *s)
{
   if(s)
   {
      int start=0;
      int t = strlenW(s);
      if(t<=m_stringlen)
      {
         for(int i=0; i<m_stringlen; )
         {
            if(memcmp(&m_buffer[i], s, t*sizeof(char_t))==0)
            {
               i+=t;
               start = i;
            }
            else
            {
               break;
            }
         }
          memmove(m_buffer, &m_buffer[start], (m_stringlen-start)*sizeof(char_t));
          m_stringlen-=start;
      }
   }
}

void STRING::replace(const char_t search, const char_t replace, int count)
{
    int c = 0;
	if(m_buffer && m_stringlen)
	{
        for(int i=0; i<m_stringlen && (count<0 || count>c); i++)
      {
         if(m_buffer[i]==search)
         {
            m_buffer[i]=replace;
            c++;
         }
      }
   }
   if(replace==TXT('\0'))
       m_stringlen=(int) strlenW(m_buffer);
}

void STRING::rreplace(const char_t search, const char_t replace, int count)
{
    int c = 0;
	if(m_buffer && m_stringlen)
	{
        for(int i=m_stringlen-1; i>=0 && (count<0 || count>c); i--)
      {
         if(m_buffer[i]==search)
         {
            m_buffer[i]=replace;
            c++;
         }
      }
   }
   if(replace==TXT('\0'))
       m_stringlen=(int) strlenW(m_buffer);
}

int STRING::substr(STRING &out_string, int start, int length)
{
	if(start>m_stringlen)
		return 0;
	if(length<0 || start+length>m_stringlen)
		length=m_stringlen-start-1;
	out_string.set(&m_buffer[start], length);
	return length;
}

int STRING::strcmp(const char_t *s2) const
{
	return ::strcmpW(s(), s2);
}

int STRING::strcmp(const char *s2)
{
	return ::strcmpA(ansi(), s2);
}

const char_t* STRING::strstr(const char_t *substring) const
{
    if(!substring)
        return NULL;
    else if(!*substring)
        return m_buffer;
    else for(const char_t* p=m_buffer, *q = substring; *p; p++)
    {
        if(*p == *q)
        {
            for(const char_t* r = p; *r && *r==*q; r++, q++);
            if(!*q)
                return p;
            else
                q = substring;
        }
    }
    return NULL;
}

int STRING::explode(char_t *delimiter, STRING *arr, int &num_strings)
{
	int ret=0;
	int i,j,num_strings_matched=0,matching_string_sz,num_delimiters,d,brk=0,previ=-1;
	if(!arr || num_strings<1)
		return 0;
    if(!m_buffer || !m_stringlen)
        return 0;

	num_delimiters=strlenW(delimiter);
    for(matching_string_sz=0, i=m_stringlen-1, j=num_strings-1;
        i>-1 && brk==0;
        i--, matching_string_sz++)
    {
        for(d=0; d<num_delimiters; d++)
        {
            if(delimiter[d]==m_buffer[i])
            {
               if(i+1!=previ)//if prev was also delimiter, then skip this
               {
				   if(matching_string_sz > 0)
				   {
					   arr[j].set(&m_buffer[i+1], matching_string_sz);
					   num_strings_matched++;
					   j--;
				   }
                  matching_string_sz=-1;
               }
               previ=i;
               if(j==-1)
                  brk=1;
               break;
            }
        }
    }
    if(j>-1 && matching_string_sz>0)
    {
        arr[j].set(m_buffer, matching_string_sz);
        num_strings_matched++;
    }
    ret=i;

	num_strings=num_strings_matched;
	return ret;
}

int STRING::sprintf( const char_t *format, ... )
{
	int ret=0;
	int len=0;
	va_list args;

	if(!format || !*format)
		goto Exit;

	len += (int) (strlenW(format)*sizeof(char_t));

	if(!len)
		goto Exit;

	va_start(args, format);     // Initialize variable arguments.
	len += GetArgumentsSize(args, format);
	va_end(args);              // Reset variable arguments.

	if(!len)
		goto Exit;

	if(m_bufferlen<len+(int)sizeof(char_t)*2)
	{
		if(!grow(len+(int)sizeof(char_t)*2, true))
			return 0;
	}

	memset(m_buffer, 0, m_bufferlen);

	va_start(args, format);     // Initialize variable arguments.
#ifdef UNICODE
#if defined (vswprintf_s)
	vswprintf_s (m_buffer, m_bufferlen/sizeof(char_t), format, args);
#else
	vswprintf(m_buffer, format, args);
#endif
#else
#if (_MSC_VER==1200)
    vsprintf(m_buffer, format, args);
#else
#ifdef UNICODE
    vwprintf (m_buffer, format, args);
#else
    vsprintf (m_buffer, format, args);
#endif
#endif
#endif
	va_end(args);              // Reset variable arguments.

	ret=m_stringlen=(int) strlenW(m_buffer);

Exit:
	return ret;
}

#ifndef UNICODE
const char *STRING::ansi() const
{
	return (char*)m_buffer;
}
#else
const char *STRING::ansi() const
{
	int i;

	char *ansistr = (char*)realloc(m_ansistr, m_stringlen+1);
	if(!ansistr)
	{
		if(m_ansistr)
		{
			free(m_ansistr);
			m_ansistr = 0;
		}
		return 0;
	}
	else
	{
		m_ansistr = ansistr;
	}

	for(i=0; i<m_stringlen; i++)
	{
		ansistr[i] = *((char*)&m_buffer[i]);
	}
	ansistr[i]=0;

	return ansistr;
}

int STRING::set(const char* s, int size)
{
	int len =0;
	int i;
	const char *p;

	if(s==(const char*)m_buffer)
		return len;

	clear();

	if(s && *s)
	{
		if(size==-1)
		{
			p=s;
			size=0;
			while(*p)
			{
				size++;
				p++;
			}
		}
		len = size;
		if(len)
		{
			len = (len+1)*sizeof(char_t);
			if(m_bufferlen<len)
				if(!grow(len, false))
					return 0;
			p=s;
			i=0;
			while(*p && i<size)
			{
				((char*)&m_buffer[i])[0]=*p;
				((char*)&m_buffer[i])[1]=0;
				p++;
				i++;
			}
			m_stringlen=size;
			m_buffer[m_stringlen]=0;
		}
	}

	return len;
}
#endif//UNICODE

void STRING::serialize(SERIALIZED_MEMORY &mem, int flag)
{
	OBJECT::serialize(mem, flag);
	mem.write((char*)&m_stringlen, sizeof(m_stringlen));
	if(m_stringlen>0)
		mem.write((char*)m_buffer, m_stringlen*sizeof(char_t));
}

void STRING::deserialize(SERIALIZED_MEMORY &mem, int flag)
{
	OBJECT::deserialize(mem, flag);
	mem.read((char*)&m_stringlen, sizeof(m_stringlen));
	if(m_stringlen>0)
	{
		int bufferlen=(m_stringlen+1)*sizeof(char_t);
		if(m_buffer && bufferlen>m_bufferlen)
			grow(bufferlen, false);
		if(m_buffer)
		{
			memset((char*)m_buffer, 0, m_bufferlen);
			mem.read((char*)m_buffer, m_stringlen*sizeof(char_t));
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////

STRING* STRING_LIST::Find(const STRING &s, int first, int last)
{
	int ret, half, n=GetCount();
	STRING *res=0;

	if(!s)
		return 0;
	if(!n)
		return 0;
	if(first>last)
		return 0;
	if(first==last)
	{
		res = (STRING*) GetAt(first);
		ret = strcmpW(res->s(), s.s());
		return (ret==0) ? res:0;
	}

	half = (last-first)/2;
	res = (STRING*) GetAt(first+half);
	ret = strcmpW(res->s(), s.s());
	if(ret==0)
		return res;
	else if(ret<0)
		res = Find(s, first, first+half-1);
	else
		res = Find(s, first+half+1, last);
	return res;
}

int STRING_LIST::Compare(const OBJECT* visited_obj, const OBJECT* user_obj)
{
	STRING *s1, *s2;
	s1 = (STRING *)visited_obj;
	s2 = (STRING *)user_obj;

	return strcmpW(s1->s(), s2->s());
}

///////////////////////////////////////////////////////////////////////////////////

OBJECT* OBJECT_LIST::Find(const OBJECT* p, int first, int last)
{
	int ret, half, n=GetCount();
	OBJECT *res=0;

	if(!p)
		return 0;
	if(!n)
		return 0;
	if(first>last)
		return 0;
	if(first==last)
	{
		res = (OBJECT*) GetAt(first);
		if(res>p)
			ret=1;
		else if(res<p)
			ret=-1;
		else
			ret=0;
		return (ret==0) ? res:0;
	}

	half = (last-first)/2;
	res = (OBJECT*) GetAt(first+half);
	if(res>p)
		ret=1;
	else if(res<p)
		ret=-1;
	else
		ret=0;
	if(ret==0)
		return res;
	else if(ret<0)
		res = Find(p, first, first+half-1);
	else
		res = Find(p, first+half+1, last);
	return res;
}

///////////////////////////////////////////////////////////////////////////////////

OBJECT* ADDRESS_LIST::Find(const OBJECT* p, int first, int last)
{
	int ret, half, n=GetCount();
	OBJECT *res=0;

	if(!p)
		return 0;
	if(!n)
		return 0;
	if(first>last)
		return 0;
	if(first==last)
	{
		res = (OBJECT*) GetAt(first);
		if(res->address()<p->address())
			ret=1;
		else if(res->address()>p->address())
			ret=-1;
		else
			ret=0;
		return (ret==0) ? res:0;
	}

	half = (last-first)/2;
	res = (OBJECT*) GetAt(first+half);
	if(res->address()<p->address())
		ret=1;
	else if(res->address()>p->address())
		ret=-1;
	else
		ret=0;
	if(ret==0)
		return res;
	else if(ret<0)
		res = Find(p, first, first+half-1);
	else
 		res = Find(p, first+half+1, last);
	return res;
}

///////////////////////////////////////////////////////////////////////////////////
DS_KEY* DS_KEY::create(int key, OBJECT* value)
{
	DS_KEY* o = new DS_KEY(key, value);
	if(!o)
		return NULL;
	o->m_ref++;
	return o;
}

///////////////////////////////////////////////////////////////////////////////////
KEY_LIST::~KEY_LIST()
{
	Destroy(0);
}

void KEY_LIST::Destroy(bool deref_data)
{
	for(int i=0; i<m_keys.GetCount(); i++)
	{
		DS_KEY *dskey = (DS_KEY *)m_keys.GetAt(i);
		if(deref_data && dskey->m_value)
			dskey->m_value->deref();
		dskey->deref();
	}
}

int KEY_LIST::Insert(int key, OBJECT* value)
{
	DS_KEY* dskey = DS_KEY::create(key, value);
	if(dskey)
	{
		return m_keys.Insert((OBJECT*)dskey);
	}
	return 0;
}

const OBJECT* KEY_LIST::Get(int key)
{
	DS_KEY *found = NULL;
	ITERATOR it;
	DS_KEY* dskey = (DS_KEY*)m_keys.GetFirst(it);
	while(dskey && !found)
	{
		if(dskey->m_key == key)
		{
			found = dskey;
		}
		dskey = (DS_KEY*)m_keys.GetNext(it);
	}

	return found ? found->Value() : NULL;
}
///////////////////////////////////////////////////////////////////////////////////
SERIALIZED_MEMORY::~SERIALIZED_MEMORY()
{
	ITERATOR it;
	POINTER *pointer;

	pointer = (POINTER *)m_pl.GetFirst(it);
	while(pointer)
	{
		delete pointer;
		pointer = (POINTER *)m_pl.GetNext(it);
	}
	m_pl.Destroy(0);
	m_al.Destroy();

	if(m_buffer)
		delete [] m_buffer;
	m_buffer=0;
	m_pos=0;
}

/*
Cannot use FILEIO!
bool SERIALIZED_MEMORY::load(char_t *file)
{
	FILEIO fp;
	if(!fp.open(file, FILEIO_MODE_READ|FILEIO_MODE_OPEN_EXISTING))
		return false;

	m_buffer = new char [ fp.size() ];
	if(!m_buffer)
	{
		fp.close();
		return false;
	}

	m_bytes = fp.read(m_buffer, fp.size());
	m_pos = 0;

	fp.close();

	return true;
}
*/
bool SERIALIZED_MEMORY::grow(int bytes)
{
	if(m_bytes<m_pos+bytes)
	{
		char* buffer;
		int bufferlen = m_bytes;
		m_bytes = m_pos+bytes;
		int f1 = m_bytes/m_block_size;
		int b1 = f1*m_block_size;
		if(b1<m_bytes)
			f1++;
		m_bytes = f1*m_block_size;
		buffer = new char[m_bytes];
		if(!buffer)
			return false;
		if(bufferlen)
			memcpy((char*)buffer, (char*)m_buffer, bufferlen);
		if(m_buffer)
			delete [] m_buffer;
		m_buffer=buffer;
	}
	return true;
}

bool SERIALIZED_MEMORY::prepend(char* data, int bytes)
{
	//Log(TXT("bytes: %i\r\n"), bytes);
	if(!grow(bytes))
		return false;
	memmove(&m_buffer[bytes], m_buffer, m_pos);
	memcpy(m_buffer, data, bytes);
	m_pos+=bytes;
	return true;
}

bool SERIALIZED_MEMORY::write(char* data, int bytes)
{
	//Log(TXT("bytes: %i\r\n"), bytes);
	if(!grow(bytes))
		return false;
	memcpy(&m_buffer[m_pos], data, bytes);
	m_pos+=bytes;
	return true;
}

bool SERIALIZED_MEMORY::read(char* data, int bytes)
{
	if(m_bytes>=m_pos+bytes)
	{
		memcpy(data, &m_buffer[m_pos], bytes);
		m_pos+=bytes;
		return true;
	}
	else
		return false;
}
/*
bool SERIALIZED_MEMORY::writep(OBJECT* pointer)
{
	bool ret =
	//Log(TXT("write p 0x%x\r\n"), p);
	return ret;
}

bool SERIALIZED_MEMORY::readp(OBJECT** pointer)
{
	bool ret = read((char*)pointer, sizeof(void*));
	return ret;
}
*/

bool SERIALIZED_MEMORY::resolvep(OBJECT** pointer)
{
	POINTER *po;
	bool ret=false;
	if(!pointer)
		return 0;
	else
	{
		//Log(TXT("read 0x%x, m_pl.count %i\r\n"), *pointer, m_pl.GetCount());
		po = new POINTER(pointer);
		if(po && m_pl.Insert(po))
			ret=true;
		else
			ret=false;
	}
	return ret;
}
/*
void SERIALIZED_MEMORY::remembera(OBJECT *pointer)
{
	//Log(TXT("remember 0x%x, 0x%x\r\n"), pointer, pointer->address());
#if 0
	if(1)
	{
	int i;
	int address=0, la=0, lai=-1;
	OBJECT *o;
	int count=m_al.GetCount();

	for(i=0; i<count; i++)
	{
		o = m_al.GetAt(i);
		address = o->address();
		if(address>la)
		{
			la=address;
			lai=i;
		}
		//Log(TXT("0x%x;"), address);
	}
	if(la && la!=address)
		int xx=0;
	//Log(TXT("\r\n"));
	}
#endif
	m_al.Insert(pointer, false);
}
*/
int SERIALIZED_MEMORY::resolvea()
{
	bool ret=true;
	int i=0;
	ITERATOR it;
	POINTER *pointer;
	OBJECT o, *r, **ref;

	pointer = (POINTER *)m_pl.GetFirst(it);
	while(pointer)
	{
		ref = pointer->getp();
		if(*ref)
		{
			o.m_address = *ref;
			r = m_al.Find((const OBJECT* )&o);
			if(r)
				pointer->setp(r);
			else
			{
				ret=false;
#if 0
				int i;
				for(i=0; i<m_al.GetCount(); i++)
				{
					r = m_al.GetAt(i);
					if(r->address() == o.m_address)
					{
						pointer->setp(r);
						ret=true;
						break;
					}
				}
#endif
				if(ret==false)
				{
					//(*ref)->serialize(*this, 0);
					pointer->setp(0);
					m_num_unresolved++;
					//throw 0;
				}
			}
		}
		delete pointer;
		pointer = (POINTER *)m_pl.GetNext(it);
		i++;
	}
	for(i=0; i<m_al.GetCount(); i++)
	{
		r = m_al.GetAt(i);
		r->setAddress();
	}
	m_pl.Destroy(0);
	m_al.Destroy();

	if(ret==false)
		m_num_unresolved=-m_num_unresolved;
	return m_num_unresolved;
}

#ifdef USE_ZLIB
void *alloc_func1(voidpf opaque, uInt items, uInt size)
{
	return 0;
}

void free_func1(voidpf opaque, voidpf address)
{
}

int SERIALIZED_MEMORY::compress()
{
	unsigned long comprLen=m_pos;
	unsigned char *compr=0;
	int szint=sizeof(int);
	int buffer_size=m_bytes > (int)(comprLen+szint+szint) ? m_bytes : comprLen+szint+szint;

	if(!m_buffer)
		return 0;

	comprLen=m_pos;
	compr=(unsigned char *)new char [buffer_size];
	if(!compr)
		return 0;

	if(Z_OK!=::compress2(compr+szint+szint, &comprLen, (const Bytef*)m_buffer, m_pos, Z_BEST_COMPRESSION))
	{
		delete [] compr;
		return 0;
	}
	((int*)compr)[0]=comprLen;//put the size of compressed data first
	((int*)compr)[1]=m_pos;//put the size of uncompressed data second

	memcpy(m_buffer, compr, comprLen+szint+szint);
	m_pos=comprLen+szint+szint;

	delete [] compr;

	return comprLen;
}

int SERIALIZED_MEMORY::decompress()
{
	if(!m_buffer)
		return 0;

	char *buffer=m_buffer+m_pos;
	int szint=sizeof(int);
	unsigned long comprLen=((int*)buffer)[0];//get the size of compressed data first
	unsigned char *compr=(unsigned char *)buffer+szint+szint;
	unsigned long uncomprLen=((int*)buffer)[1];//get the size of uncompressed data second
	unsigned long uncomprLen_=uncomprLen;

	if((int)uncomprLen+m_pos>m_bytes)
		return 0;
	compr=(unsigned char *)new char [comprLen];
	if(!compr)
		return 0;
	memcpy(compr, buffer+szint+szint, comprLen);

	if(Z_OK!=::uncompress((unsigned char*)buffer, &uncomprLen, compr, comprLen) || uncomprLen_!=uncomprLen)
	{
		delete [] compr;
		return 0;
	}

	delete [] compr;

	return uncomprLen;
}

#endif//USE_ZLIB
///////////////////////////////////////////////////////////////////////////////////

void OBJECT::serialize(SERIALIZED_MEMORY &mem, int flag)
{
	//Log(TXT("address 0x%x\r\n"), m_address);
	mem.write((char*)&m_address, sizeof(m_address));
	if(flag&SERIALIZE_FLAG_COMPLETE) mem.write((char*)&m_ref, sizeof(m_ref));
}

void OBJECT::deserialize(SERIALIZED_MEMORY &mem, int flag)
{
	mem.read((char*)&m_address, sizeof(m_address));
	if(flag&SERIALIZE_FLAG_COMPLETE) mem.read((char*)&m_ref, sizeof(m_ref));
	//Log(TXT("address 0x%x\r\n"), m_address);
	if(flag & SERIALIZE_FLAG_REMEMBER_THIS)
		mem.remembera(this);
}

///////////////////////////////////////////////////////////////////////////////////

int GetArgumentsSize(va_list args, const char_t *format)
{
	int len=0;
	const char_t *p=format;

	if(!p || !*p)
		goto Exit;

	while (*p!='\0' && *p)
	{
		if (*p++ == '%')
		{
			while (strchrW (TXT("-+ #0"), *p))
				++p;
			if (*p == '*')
			{
				++p;
				//len += abs (va_arg (args, int));
				int val = va_arg (args, int);
				if(val<0)
					val=-val;
				len += val;
			}
			else
#ifdef UNICODE
				len += wcstoul (p, (char_t **) &p, 10);
#else
				len += strtoul (p, (char_t **) &p, 10);
#endif
			if (*p == '.')
			{
				++p;
				if (*p == '*')
				{
					++p;
					//len += abs (va_arg (args, int));
					int val = va_arg (args, int);
					if(val<0)
						val=-val;
					len += val;
				}
				else
#ifdef UNICODE
					len += wcstoul (p, (char_t **) &p, 10);
#else
					len += strtoul (p, (char_t **) &p, 10);
#endif
			}
			while (strchrW (TXT("hlL"), *p))
				++p;
			// Should be big enough for any format specifier except %s and floats.
			len += 30;
			switch (*p)
			{
			case 'd':
			case 'i':
			case 'o':
			case 'u':
			case 'x':
			case 'X':
			case 'c':
				(void) va_arg (args, int);
				break;
			case 'f':
			case 'e':
			case 'E':
			case 'g':
			case 'G':
				(void) va_arg (args, double);
				// Since an ieee double can have an exponent of 307, we'll
				//make the buffer wide enough to cover the gross case.
				len += 307;
				break;
			case 's':
				len += (int) strlenW (va_arg (args, char_t *));
				break;
			case 'p':
			case 'n':
				(void) va_arg (args, char_t *);
				break;
			}
			p++;
		}
	}

Exit:
	return len;
}

