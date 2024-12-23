#ifndef _UNORDERED_MAP_H_
#define _UNORDERED_MAP_H_

#include <stdexcept>
#include <type_traits>
#include <string>
#include <optional>

#include "List.h"

template<typename _Key, typename _Val>
class UnorderedMap {
public:
	struct Node {
		Node() {
			if constexpr (std::is_class_v<_Key>) {
				key_ = _Key();
			}
			else {
				key_ = static_cast<_Key>(0);
			}
			if constexpr (std::is_class_v<_Val>) {
				val_ = _Val();
			}
			else {
				val_ = static_cast<_Val>(0);
			}
		}
		Node(_Key key, _Val val) : key_(key), val_(val) {};
		Node& operator=(const Node& node) {
			this->key_ = node.key_;
			this->val_ = node.val_;
			return *this;
		}
		Node& operator=(Node&& node) {
			this->key_ = std::move(node.key_);
			this->val_ = std::move(node.val_);
			return *this;
		}
		_Key key_;
		_Val val_;
	};
	UnorderedMap() {
		for (size_t i = 0; i < arraySize; i++) {
			this->array_[i] = new List<Node>;
		}
	}
	UnorderedMap(std::initializer_list<Node> list) {
		for (size_t i = 0; i < arraySize; i++) {
			this->array_[i] = new List<Node>;
		}
		for (auto& node : list) {
			size_t hashValue = hashFunction(node.key_);
			if (this->array_[hashValue]->empty()) {
				this->array_[hashValue]->pushBack({ node.key_, node.val_ });
			}
			else {
				bool keyIsExist = false;//键值存在就更新值
				for (auto it = this->array_[hashValue]->begin(); it != this->array_[hashValue]->end(); ++it) {
					if (it->key_ == node.key_) {
						it->val_ = node.val_;
						keyIsExist = true;
						break;
					}
				}
				if (!keyIsExist) {//键值不存在就新建值
					this->array_[hashValue]->pushBack({ node.key_, node.val_ });
				}
			}
		}
	}
	_Val& operator[](const _Key& key) {
		size_t hashValue = hashFunction(key);
		if (this->array_[hashValue]->empty()) {
			this->array_[hashValue]->pushBack({ key, static_cast<_Val>(0) });
			return this->array_[hashValue]->begin()->val_;
		}
		else {
			auto it = this->array_[hashValue]->begin();
			for (; it != this->array_[hashValue]->end(); ++it) {
				if (it->key_ == key) {
					return it->val_;
				}
			}
			this->array_[hashValue]->pushBack({ key, static_cast<_Val>(0) });
			return (--it)->val_;//it原来指向end,因为pushBack，所以end的前一项是新插入的一项，这样做是为了确保返回的是列表中的引用
		}
	}
	_Val& operator[](_Key&& key) {
		size_t hashValue = hashFunction(std::forward<_Key>(key));
		if (this->array_[hashValue]->empty()) {
			this->array_[hashValue]->pushBack({ std::move(key), static_cast<_Val>(0) });
			return this->array_[hashValue]->begin()->val_;
		}
		else {
			auto it = this->array_[hashValue]->begin();
			for (; it != this->array_[hashValue]->end(); ++it) {
				if (it->key_ == key) {
					return it->val_;
				}
			}
			this->array_[hashValue]->pushBack({ std::move(key), static_cast<_Val>(0) });
			return (--it)->val_;//it原来指向end,因为pushBack，所以end的前一项是新插入的一项，这样做是为了确保返回的是列表中的引用
		}
	}
	std::optional<typename List<Node>::Iterator> find(const _Key& key) {
		size_t hashValue = hashFunction(key);
		if (this->array_[hashValue]->empty()) {
			return std::nullopt;
		}
		else {
			for (auto it = this->array_[hashValue]->begin(); it != this->array_[hashValue]->end(); ++it) {
				if (it->key_ == key) {
					return it;
				}
			}
			return std::nullopt;
		}
	}
	_Val& at(const _Key& key) {
		size_t hashValue = hashFunction(key);
		if (this->array_[hashValue]->empty()) {
			throw std::out_of_range("invalid key!");
		}
		else {
			for (auto it = this->array_[hashValue]->begin(); it != this->array_[hashValue]->end(); ++it) {
				if (it->key_ == key) {
					return it->val_;
				}
			}
			throw std::out_of_range("invalid key!");
		}
	}
	void remove(const _Key& key) {
		size_t hashValue = hashFunction(key);
		if (this->array_[hashValue]->empty()) {
			throw std::out_of_range("invalid key!");
		}
		else {
			for (auto it = this->array_[hashValue]->begin(); it != this->array_[hashValue]->end(); ++it) {
				if (it->key_ == key) {
					this->array_[hashValue]->remove(it);
					return;
				}
			}
			throw std::out_of_range("invalid key!");
		}
	}
	void remove(typename List<Node>::Iterator it) {
		size_t hashValue = hashFunction(it->key_);
		this->array_[hashValue]->remove(it);
	}
	~UnorderedMap() {
		for (size_t i = 0; i < arraySize; i++) {
			if (this->array_[i]) {
				delete this->array_[i];
				this->array_[i] = nullptr;
			}
		}
	}
private:
	size_t getStringValue(const char* str) {//将字符串的每一个字符的值求和作为key
		size_t sum = 0;
		size_t offset = 0;
		while ('\0' != *(str + offset)) {
			sum += *(str + offset);
			offset++;
		}
		return sum;
	}
	size_t getStringValue(const std::string& str) {//将字符串的每一个字符的值求和作为key
		size_t sum = 0;
		for (size_t i = 0; i < str.size(); i++) {
			sum += str[i];
		}
		return sum;
	}
	size_t hashFunction(const _Key& key) {
		if constexpr (std::is_arithmetic_v<_Key>) {
			return static_cast<size_t>(key % arraySize);
		}
		else if constexpr (std::is_same_v<char*, std::remove_const_t<_Key>> || std::is_same_v<const char*, std::remove_const_t<_Key>>) {
			return static_cast<size_t>(getStringValue(key) % arraySize);
		}
		else if constexpr (std::is_same_v<std::string, std::remove_const_t<_Key>>) {
			return static_cast<size_t>(getStringValue(key) % arraySize);
		}
		else {
			static_assert(false, "unsupport key type!");
		}
	}
	static constexpr size_t arraySize = 97;
	List<Node>* array_[arraySize];
};

#endif //!_UNORDERED_MAP_H_