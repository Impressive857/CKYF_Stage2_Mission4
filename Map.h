#ifndef _MAP_H_
#define _MAP_H_

#include <stdexcept>
#include <utility>

template<typename _Key, typename _Val>
class Map {
public:
	struct Node {
		Node(_Key key, _Val val) :key_(key), val_(val) {};
		_Key key_;
		_Val val_;
		bool isRed_ = true;
		Node* head_ = nullptr;
		Node* left_ = nullptr;
		Node* right_ = nullptr;
	};
	//基于红黑树实现的Map
	Map() {
		this->root_ = nullptr;
	}
	Map(std::initializer_list<Node> list) {
		this->root_ = nullptr;
		for (auto& node : list) {
			if (nullptr == this->root_) {//如果树是空的，直接插入
				this->root_ = new Node(node.key_, node.val_);
				this->root_->isRed_ = false;
			}
			else {
				Node* compareNode = this->root_;
				while (nullptr != compareNode) {
					if (node.key_ < compareNode->key_) {
						if (nullptr == compareNode->left_) {//将较小的key放入左子树
							Node* newNode = new Node(node.key_, node.val_);
							compareNode->left_ = newNode;
							newNode->head_ = compareNode;
							checkBlance(newNode);
							break;
						}
						else {
							compareNode = compareNode->left_;
						}
					}
					else if (node.key_ > compareNode->key_) {//将较大的key放入右子树
						if (nullptr == compareNode->right_) {
							Node* newNode = new Node(node.key_, node.val_);
							compareNode->right_ = newNode;
							newNode->head_ = compareNode;
							checkBlance(newNode);
							break;
						}
						else {
							compareNode = compareNode->right_;
						}
					}
					else {//如果key在树中存在，则更新值
						compareNode->val_ = node.val_;
						break;
					}
				}
			}
		}
	}
	~Map() {
		clearTree(this->root_);
	}
	//如果键值在树中不存在，创建新的键值
	//如果键值在树中存在，返回键值映射的值
	_Val& operator[](const _Key& key) {
		if (nullptr == this->root_) {//如果树是空的，直接插入
			this->root_ = new Node(key, static_cast<_Val>(0));
			this->root_->isRed_ = false;
			return this->root_->val_;
		}
		else {
			Node* compareNode = this->root_;
			while (nullptr != compareNode) {
				if (key < compareNode->key_) {
					if (nullptr == compareNode->left_) {//将较小的key放入左子树
						Node* newNode = new Node(key, static_cast<_Val>(0));
						compareNode->left_ = newNode;
						newNode->head_ = compareNode;
						checkBlance(newNode);
						return newNode->val_;
					}
					else {
						compareNode = compareNode->left_;
					}
				}
				else if (key > compareNode->key_) {//将较大的key放入右子树
					if (nullptr == compareNode->right_) {
						Node* newNode = new Node(key, static_cast<_Val>(0));
						compareNode->right_ = newNode;
						newNode->head_ = compareNode;
						checkBlance(newNode);
						return newNode->val_;
					}
					else {
						compareNode = compareNode->right_;
					}
				}
				else {//如果key在树中存在，则返回值
					return compareNode->val_;
				}
			}
			throw std::runtime_error("unknow error!");//大于小于等于都在前面判定完了，如果执行到这里就是玄学了QAQ
		}
	}
	//如果键值在树中不存在，创建新的键值
	//如果键值在树中存在，返回键值映射的值
	_Val& operator[](_Key&& key) {
		if (nullptr == this->root_) {//如果树是空的，直接插入
			this->root_ = new Node(std::forward<_Key>(key), static_cast<_Val>(0));
			this->root_->isRed_ = false;
			return this->root_->val_;
		}
		else {
			Node* compareNode = this->root_;
			while (nullptr != compareNode) {
				if (key < compareNode->key_) {//将较小的key放入左子树
					if (nullptr == compareNode->left_) {
						Node* newNode = new Node(std::forward<_Key>(key), static_cast<_Val>(0));
						compareNode->left_ = newNode;
						newNode->head_ = compareNode;
						checkBlance(newNode);
						return newNode->val_;
					}
					else {
						compareNode = compareNode->left_;
					}
				}
				else if (key > compareNode->key_) {//将较大的key放入右子树
					if (nullptr == compareNode->right_) {
						Node* newNode = new Node(std::forward<_Key>(key), static_cast<_Val>(0));
						compareNode->right_ = newNode;
						newNode->head_ = compareNode;
						checkBlance(newNode);
						return newNode->val_;
					}
					else {
						compareNode = compareNode->right_;
					}
				}
				else {//如果key在树中存在，则返回值
					return compareNode->val_;
				}
			}
			throw std::runtime_error("unknow error!");//大于小于等于都在前面判定完了，如果执行到这里就是玄学了QAQ
		}
	}
	void remove(const _Key& key) {
		if (nullptr == this->root_) {
			throw std::out_of_range("map is empty!");
		}
		else {
			Node* compareNode = this->root_;
			while (nullptr != compareNode) {
				if (key < compareNode->key_) {
					if (nullptr == compareNode->left_) {
						throw std::out_of_range("key is not exist!");
					}
					else {
						compareNode = compareNode->left_;
					}
				}
				else if (key > compareNode->key_) {
					if (nullptr == compareNode->right_) {
						throw std::out_of_range("key is not exist!");
					}
					else {
						compareNode = compareNode->right_;
					}
				}
				else {//找到节点后进行删除
					removeByPtr(compareNode);
					return;
				}
			}
		}
	}
	void clear() {
		clearTree(this->root_);
	}
private:
	void lRotate(Node* current) {//左旋
		if (nullptr == current) {
			throw std::invalid_argument("rotate node is nullptr!");
		}
		Node* formerRight = current->right_;//原来的右节点
		if (nullptr == formerRight) {
			throw std::invalid_argument("rotate node right is nullptr");
		}
		if (current == this->root_) {
			this->root_ = formerRight;
		}
		if (current->head_) {
			current->head_->left_ = formerRight;
		}
		//如果有冲突的左节点，冲突的左节点变为当前节点的右节点
		if (nullptr != formerRight->left_) {
			Node* conflictLeft = formerRight->left_;//左旋时冲突的左节点
			formerRight->head_ = current->head_;
			conflictLeft->head_ = current;
			current->head_ = formerRight;
			current->right_ = conflictLeft;
			formerRight->left_ = current;
		}
		else {
			formerRight->head_ = current->head_;
			formerRight->left_ = current;
			current->head_ = formerRight;
			current->right_ = nullptr;
		}
	}
	void rRotate(Node* current) {//右旋
		if (nullptr == current) {
			throw std::invalid_argument("rotate node is nullptr!");
		}
		Node* formerLeft = current->left_;//原来的左节点
		if (nullptr == formerLeft) {
			throw std::invalid_argument("rotate node left is nullptr");
		}
		if (current == this->root_) {
			this->root_ = formerLeft;
		}
		if (current->head_) {
			current->head_->right_ = formerLeft;
		}
		//如果有冲突的右节点，冲突的右节点变为当前节点的左节点
		if (nullptr != formerLeft->right_) {
			Node* conflictRight = formerLeft->right_;//右旋时冲突的右节点
			formerLeft->head_ = current->head_;
			conflictRight->head_ = current;
			current->head_ = formerLeft;
			current->left_ = conflictRight;
			formerLeft->right_ = current;
		}
		else {
			formerLeft->head_ = current->head_;
			formerLeft->right_ = current;
			current->head_ = formerLeft;
			current->left_ = nullptr;
		}
	}
	void swap(Node* node1, Node* node2) {
		_Key key = node1->key_;
		_Val val = node1->val_;
		node1->key_ = node2->key_;
		node1->val_ = node2->val_;
		node2->key_ = key;
		node2->val_ = val;
	}
	void checkBlance(Node* current) {
		if (current == this->root_) {
			current->isRed_ = false;
		}
		else if (current->head_ == this->root_ || !(current->isRed_ && current->head_->isRed_)) {
			return;
		}
		else {
			Node* father = current->head_;
			if (!father) {
				throw std::invalid_argument("father is nullptr");
			}
			Node* grandfather = father->head_;
			if (!grandfather) {
				throw std::invalid_argument("grandfather is nullptr!");
			}
			Node* uncle = (grandfather->left_ == father) ? (grandfather->right_) : (grandfather->left_);
			if (nullptr == uncle || !uncle->isRed_) {
				if (father == grandfather->left_ && current == father->left_) {//LL型
					rRotate(grandfather);
					father->isRed_ = !father->isRed_;
					grandfather->isRed_ = !grandfather->isRed_;
				}
				else if (father == grandfather->left_ && current == father->right_) {//LR型
					lRotate(father);
					rRotate(grandfather);
					current->isRed_ = !current->isRed_;
					grandfather->isRed_ = !grandfather->isRed_;
				}
				else if (father == grandfather->right_ && current == father->left_) {//RL型
					rRotate(father);
					lRotate(grandfather);
					current->isRed_ = !current->isRed_;
					grandfather->isRed_ = !grandfather->isRed_;
				}
				else if (father == grandfather->right_ && current == father->right_) {//RR型
					lRotate(grandfather);
					father->isRed_ = !father->isRed_;
					grandfather->isRed_ = !grandfather->isRed_;
				}
			}
			else {
				father->isRed_ = !father->isRed_;
				uncle->isRed_ = !uncle->isRed_;
				grandfather->isRed_ = !grandfather->isRed_;
				checkBlance(grandfather);
			}
		}
	}
	void removeByPtr(Node* current) {
		if (!(current->left_ || current->right_)) {//没子节点的情况
			if (current == this->root_) {//如果删除根节点
				delete current;
				this->root_ = nullptr;
			}
			else {
				if (!current->isRed_) {//删除没有子节点的黑节点
					fixDoubleBlack(current);
				}
				//修复双黑节点后或者节点是红色，直接删除
				if (current == current->head_->left_) {
					current->head_->left_ = nullptr;
				}
				else {
					current->head_->right_ = nullptr;
				}
				delete current;
			}
		}
		else if (current->left_ && !current->right_) {//只有左节点
			if (current == this->root_) {//如果删除根节点
				this->root_ = current->left_;
				current->left_->head_ = nullptr;
				current->left_->isRed_ = false;
				delete current;
			}
			else {
				if (current == current->head_->left_) {
					current->head_->left_ = current->left_;
				}
				else {
					current->head_->right_ = current->left_;
				}
				current->left_->isRed_ = false;
				current->left_->head_ = current->head_;
				delete current;
			}
		}
		else if (current->right_ && !current->left_) {//只有右节点
			if (current == this->root_) {//如果删除根节点
				this->root_ = current->right_;
				current->right_->head_ = nullptr;
				current->right_->isRed_ = false;
				delete current;
			}
			else {
				if (current == current->head_->left_) {
					current->head_->left_ = current->right_;
				}
				else {
					current->head_->right_ = current->right_;
				}
				current->right_->isRed_ = false;
				current->right_->head_ = current->head_;
				delete current;
			}
		}
		else {//左右节点都存在，寻找右子树中的最小左子树和删除节点交换
			Node* minNode = current->right_;
			Node* compareNode = current->right_;
			while (nullptr != compareNode) {
				if (compareNode->key_ < minNode->key_) {
					minNode = compareNode;
				}
				compareNode = compareNode->left_;
			}
			swap(current, compareNode);
			removeByPtr(compareNode);
		}
	}
	void fixDoubleBlack(Node* current) {
		if (current == this->root_) {
			return;
		}
		if (current->isRed_) {
			current->isRed_ = false;
			return;
		}
		Node* father = current->head_;
		Node* bother = (current == father->left_) ? (father->right_) : (father->left_);
		if (bother->isRed_) {//兄弟节点是红色
			bother->isRed_ = !bother->isRed_;
			father->isRed_ = !father->isRed_;
			if (current == father->left_) {//变色后向双黑节点旋转
				lRotate(father);
			}
			else {
				rRotate(father);
			}
			fixDoubleBlack(current);
		}
		else {//兄弟节点是黑色
			if ((nullptr != bother->left_ && bother->left_->isRed_) || (nullptr != bother->right_ && bother->right_->isRed_)) {//如果至少有一个红色节点
				if ((bother == father->left_) && (nullptr != bother->left_ && bother->left_->isRed_)) {//LL型
					bother->left_->isRed_ = bother->isRed_;
					bother->isRed_ = father->isRed_;
					father->isRed_ = false;
					rRotate(father);
				}
				else if ((bother == father->right_) && (nullptr != bother->right_ && bother->right_->isRed_)) {//RR型
					bother->right_->isRed_ = bother->isRed_;
					bother->isRed_ = father->isRed_;
					father->isRed_ = false;
					lRotate(father);
				}if ((bother == father->left_) && (nullptr != bother->right_ && bother->right_->isRed_)) {//LR型
					bother->right_->isRed_ = father->isRed_;
					father->isRed_ = false;
					lRotate(bother);
					rRotate(father);
				}if ((bother == father->right_) && (nullptr != bother->left_ && bother->left_->isRed_)) {//RL型
					bother->left_->isRed_ = father->isRed_;
					father->isRed_ = false;
					rRotate(bother);
					lRotate(father);
				}
			}
			else {//兄弟的子节点都是黑色
				bother->isRed_ = true;
				fixDoubleBlack(father);
			}
		}
	}
	void clearTree(Node* node) {
		if (nullptr == node) {
			return;
		}
		else {
			clearTree(node->left_);
			clearTree(node->right_);
			delete node;
		}
		this->root_ = nullptr;
	}
	Node* root_;
};

#endif //!_MAP_H_