
#include <cstdlib>

template <typename T> 
struct LinkNode
{
	T data;
	LinkNode<T>* succ;
	LinkNode(){}
	LinkNode(T e,LinkNode<T>* s = NULL)
	{
		data = e;
		succ = s;
	}
	LinkNode<T>* insertAsSucc ( T const& e ); //������ǰ�ڵ�Tǰ�I���½ڵ�
	
};

template <typename T> //��e������ǰ�ڵ�T��I��ء��ǰ�ڵ������б�ک���ڱ�β�ڵ�trailer��
LinkNode<T>* LinkNode<T>::insertAsSucc ( T const& e ) 
{
	LinkNode<T>* x = new LinkNode ( e, succ ); //�����½ڵ�
	succ = x; //ک����������
	return x; //ކ���½ڵ�mλ��
}

template <typename T> 
class LinkList
{
private:
	int _size;
	LinkNode<T>* header;
	
protected:
	void init();
	
public:
	int clear();
	~LinkList();
	LinkList()
	{
		init();
	}
	int size() const { return _size; }
	T& operator[] ( int r ) const; //���أ�֧��ѭ��ڪ�ʣ�Ч�ʵͣ�
	LinkNode<T>* first() const { return header->succ; } //�׽ڵ�λ��
	T remove ( LinkNode<T>* p ); //�����Ϸ�λ��p���m�ڵ�,ކ�ر������ڵ�
	LinkNode<T>* insertAsFirst ( T const& e );
	LinkNode<T>* insertA ( LinkNode<T>* p, T const& e );
	
	
	
/*
	void enqueue(T const&e);
	T deLinkList();
	int size();
	T& front();
*/
};

template <typename T>
void LinkList<T>::init()
{
	header = new LinkNode<T>;
	header->succ = NULL;
	_size = 0;
}

template <typename T> //�����±����������ͨ����ֱ��ڪ���б�ڵ㣨��ڱ㣬Ч�ʵͣ������ã�
T& LinkList<T>::operator[] ( int r ) const 
{ //assert: 0 <= r < size
	LinkNode<T>* p = first(); //���׽ڵ����
	while ( 0 < r-- ) p = p->succ; //˳����r���ڵ㼴��
	return p->data; //Ŀ��ڵ㣬ކ����������Ԫ��
}

template <typename T> 
T LinkList<T>::remove ( LinkNode<T>* p ) 
{ //�����Ϸ��ڵ�p��ކ������ֵ
	T e = p->data; //�����������ڵ�m��ֵ���ٶ�T���Ϳ�ֱ�Ӹ�ֵ��
	LinkNode<T>* p1 = header; 
	while (( p1->succ != p )&&(p1->succ != NULL)) p1 = p1->succ; 
	if((p != NULL)&&(p1->succ == p)) p1->succ = p->succ;
	delete p; _size--; //�ͷŽڵ㣬�����_ģ
	return e; //ކ�ر����m��ֵ
}

template <typename T> 
LinkNode<T>* LinkList<T>::insertAsFirst ( T const& e )
{ _size++; return header->insertAsSucc ( e ); } //e�����׽ڵ�I��


template <typename T> 
LinkNode<T>* LinkList<T>::insertA ( LinkNode<T>* p, T const& e )
{ _size++; return p->insertAsSucc ( e ); } //e����p�m��̓I�루After��

template <typename T> 
LinkList<T>::~LinkList() //�б햆����
{ clear(); delete header; } //����б��ͷ�ͷ��β�ڱ��ڵ�

template <typename T> 
int LinkList<T>::clear() 
{ //����б�
	int oldSize = _size;
	while ( 0 < _size ) remove ( header->succ ); //���������׽ڵ㣬ֱ���б���
	return oldSize;
}


/*

template <typename T>
void Queue<T>::enqueue(T const&e)
{
	Node<T>* Qn = new Node<T>(e,trailer->pred,trailer);
	this->trailer->pred->succ = Qn;
	this->trailer->pred = Qn;
	_size++;
}

template <typename T>
T Queue<T>::dequeue()
{
	if(_size > 0)
	{
		T da = this->header->succ->data;
		this->header->succ->succ->pred = this->header;
		this->header->succ = this->header->succ->succ;
		_size--;
		return da;
	}
}

template <typename T>
T& Queue<T>::front()
{
	return this->header->succ.data; 
}
*/ 










