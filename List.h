#ifndef _LIST_H_
#define _LIST_H_

#include <stdexcept>
#include <type_traits>

template<typename _Ty>
class List {
public:
	//仿照标准库实现的Iterator和List
	class Iterator {
	public:
		friend class List;
		Iterator() = default;
		_Ty operator*() {
			return ptr_->val_;
		}
		_Ty* operator->() const {
			return &(ptr_->val_);
		}
		Iterator& operator++() {
			ptr_ = ptr_->next_;
			return *this;
		}
		Iterator operator++(int) {
			Iterator temp = *this;
			ptr_ = ptr_->next_;
			return temp;
		}
		Iterator& operator--() {
			ptr_ = ptr_->prev_;
			return *this;
		}
		Iterator operator--(int) {
			Iterator temp = *this;
			ptr_ = ptr_->prev_;
			return temp;
		}
		Iterator& operator=(const Iterator& other) {
			this->ptr_ = other.ptr_;
			return *this;
		}
		bool operator==(const Iterator& right) const {
			return this->ptr_ == right.ptr_;
		}
		bool operator!=(const Iterator& right) const {
			return !(*this == right);
		}
		~Iterator() = default;
	private:
		//存储数据的节点
		struct _Node {
			_Node() {
				if constexpr (std::is_class_v<_Ty>) {
					val_ = _Ty();
				}
				else {
					val_ = static_cast<_Ty>(0);
				}
			}
			_Node(_Ty val) :val_(val) {};
			_Node& operator=(const _Node& node) {
				this->val_ = node.val_;
				this->prev_ = node.prev_;
				this->next_ = node.next_;
				return *this;
			}
			_Node& operator=(_Node&& node) {
				this->val_ = std::move(node.val_);
				this->prev_ = node.prev_;
				this->next_ = node.next_;
				return *this;
			}
			_Ty val_;
			_Node* prev_ = nullptr;
			_Node* next_ = nullptr;
		};
		_Node* ptr_;//指向节点的指针
	};

	using ListIterator = typename List<_Ty>::Iterator;
	using Node = typename List<_Ty>::Iterator::_Node;
	using NodePtr = typename List<_Ty>::Iterator::_Node*;

	List() {
		this->begin_.ptr_ = this->end_.ptr_ = new Node;
		this->size_ = 0;
	}
	List(std::initializer_list<_Ty> list) {
		//先插入第一个
		NodePtr ptr = new Node;//ptr后续作为新成员的前一项，以确保链表的链接
		ptr->val_ = *(list.begin());
		this->begin_.ptr_ = ptr;
		this->end_.ptr_ = new Node;
		this->begin_.ptr_->next_ = this->end_.ptr_;
		this->end_.ptr_->prev_ = this->begin_.ptr_;

		//如果有剩余的值，插入剩余的
		for (auto it = std::next(list.begin()); it != list.end(); ++it) {
			NodePtr newPtr = new Node;
			newPtr->val_ = *it;
			ptr->next_ = newPtr;
			newPtr->prev_ = ptr;
			newPtr->next_ = this->end_.ptr_;
			this->end_.ptr_->prev_ = newPtr;
			ptr = newPtr;//更新ptr，进行下一次遍历
		}

		//修改大小
		this->size_ = list.size();
	}
	ListIterator begin() {
		return this->begin_;
	}
	ListIterator end() {
		return this->end_;
	}
	void pushBack(const _Ty& val) {
		if (0 == this->size_) {
			this->begin_.ptr_ = new Node;
			this->begin_.ptr_->val_ = val;
			this->begin_.ptr_->next_ = this->end_.ptr_;
			this->end_.ptr_->prev_ = this->begin_.ptr_;
			this->size_ = 1;
		}
		else {
			NodePtr newPtr = new Node;
			newPtr->val_ = val;
			NodePtr lastPtr = this->end_.ptr_->prev_;//lastPtr是原来链表的最后一项
			lastPtr->next_ = newPtr;
			newPtr->prev_ = lastPtr;
			newPtr->next_ = this->end_.ptr_;
			this->end_->ptr_->prev_ = newPtr;
			this->size_++;
		}
	}
	void pushBack(_Ty&& val) {
		if (0 == this->size_) {
			this->begin_.ptr_ = new Node;
			this->begin_.ptr_->val_ = std::move(val);
			this->begin_.ptr_->next_ = this->end_.ptr_;
			this->end_.ptr_->prev_ = this->begin_.ptr_;
			this->size_ = 1;
		}
		else {
			NodePtr newPtr = new Node;
			newPtr->val_ = std::move(val);
			NodePtr lastPtr = this->end_.ptr_->prev_;//lastPtr是原来链表的最后一项
			lastPtr->next_ = newPtr;
			newPtr->prev_ = lastPtr;
			newPtr->next_ = this->end_.ptr_;
			this->end_.ptr_->prev_ = newPtr;
			this->size_++;
		}
	}
	void pushFront(const _Ty& val) {
		if (0 == this->size_) {
			this->begin_.ptr_ = new Node;
			this->begin_.ptr_->val_ = val;
			this->begin_.ptr_->next_ = this->end_.ptr_;
			this->end_.ptr_->prev_ = this->begin_.ptr_;
			this->size_ = 1;
		}
		else {
			NodePtr newPtr = new Node;
			newPtr->val_ = val;
			NodePtr firstPtr = this->begin_.ptr_;//firstPtr是原来链表的第一项
			firstPtr->prev_ = newPtr;
			newPtr->next_ = firstPtr;
			this->begin_.ptr_ = newPtr;
			this->size_++;
		}
	}
	void pushFront(_Ty&& val) {
		if (0 == this->size_) {
			this->begin_.ptr_ = new Node;
			this->begin_.ptr_->val_ = std::move(val);
			this->begin_.ptr_->next_ = this->end_.ptr_;
			this->end_.ptr_->prev_ = this->begin_.ptr_;
			this->size_ = 1;
		}
		else {
			NodePtr newPtr = new Node;
			newPtr->val_ = std::move(val);
			NodePtr firstPtr = this->begin_.ptr_;//firstPtr是原来链表的第一项
			firstPtr->prev_ = newPtr;
			newPtr->next_ = firstPtr;
			this->begin_.ptr_ = newPtr;
			this->size_++;
		}
	}
	ListIterator insert(ListIterator where, const _Ty& val) {
		if (where == this->end_) {
			throw std::invalid_argument("invalid insert place!");
		}
		//先检查插入位置是否在列表中存在
		for (auto it = this->begin_; it != this->end_; ++it) {
			//如果存在就插入
			if (it == where) {
				ListIterator newIt{};
				newIt.ptr_ = new Node;
				newIt.ptr_->val_ = val;
				NodePtr formerNextPtr = where.ptr_->next_;
				where.ptr_->next_ = newIt.ptr_;
				newIt.ptr_->prev_ = where.ptr_;
				newIt.ptr_->next_ = formerNextPtr;
				formerNextPtr->prev_ = newIt.ptr_;
				this->size_++;
				return newIt;
			}
		}
		throw std::invalid_argument("invalid insert place!");
	}
	void remove(ListIterator target) {
		if (target == this->end_) {
			throw std::invalid_argument("invalid remove place!");
		}
		if (target == this->begin_) {
			if (begin_.ptr_) {
				NodePtr nextPtr = this->begin_.ptr_->next_;
				nextPtr->prev_ = nullptr;
				delete this->begin_.ptr_;
				this->begin_.ptr_ = nextPtr;
				this->size_--;
			}
			return;
		}
		//先检查插入位置是否在列表中存在
		for (auto it = this->begin_; it != this->end_; ++it) {
			//如果存在就删除
			if (it == target) {
				target.ptr_->prev_->next_ = target.ptr_->next_;
				target.ptr_->next_->prev_ = target.ptr_->prev_;
				delete target.ptr_;
				this->size_--;
				return;
			}
		}
		throw std::invalid_argument("invalid remove place!");
	}
	void clear() {
		//如果列表为空就直接跳过
		if (0 != this->size_) {
			NodePtr ptr = this->begin_.ptr_;
			while (nullptr != ptr) {
				NodePtr next = ptr->next_;
				delete ptr;
				ptr = next;
			}
			this->begin_.ptr_ = this->end_.ptr_ = new Node;
			this->size_ = 0;
		}
	}
	size_t size() const {
		return this->size_;
	}
	bool empty() const {
		return 0 == this->size_;
	}
	~List() {
		this->clear();
	}
private:
	ListIterator begin_;
	ListIterator end_;//end是一个特殊的迭代器，表示无效位置
	size_t size_;
};

template<typename _First, typename..._Res>//类模板实参推导
List(_First, _Res...) -> List<typename std::enable_if_t<(std::is_same_v<_First, _Res> && ...), _First>>;

#endif //!_LIST_H_