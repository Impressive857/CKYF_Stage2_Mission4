#ifndef _STACK_H_
#define _STACK_H_

#include <stdexcept>
#include <type_traits>

template<typename _Ty>
class Stack
{
public:
	Stack() {
		this->empty_ = true;
		this->top_ = 0;
		this->stack_ = new _Ty[defaultSize];
		this->arraySize_ = defaultSize;
	}
	~Stack() {
		if (this->stack_) {
			delete[] this->stack_;
			this->stack_ = nullptr;
		}
	}
	_Ty& top() const {
		if (this->empty_) {
			throw std::out_of_range("stack is empty!");
		}
		return this->stack_[this->top_];
	}
	void push(const _Ty& value) {
		if (!this->empty_) {
			this->top_++;
		}
		else {
			this->empty_ = false;
		}
		/*当栈容量不足时自动扩大*/
		if (this->top_ >= this->arraySize_) {
			_Ty* buffer = new _Ty[this->arraySize_ + defaultSize];
			for (int i = 0; i < this->top_; i++) {
				buffer[i] = this->stack_[i];
			}
			delete[] this->stack_;
			this->stack_ = buffer;
			this->arraySize_ += defaultSize;
		}
		this->stack_[this->top_] = value;
	}
	void push(_Ty&& value) {
		if (!this->empty_) {
			this->top_++;
		}
		else {
			this->empty_ = false;
		}
		/*当栈容量不足时自动扩大*/
		if (this->top_ >= this->arraySize_) {
			_Ty* buffer = new _Ty[this->arraySize_ + defaultSize];
			for (int i = 0; i < this->top_; i++) {
				buffer[i] = this->stack_[i];
			}
			delete[] this->stack_;
			this->stack_ = buffer;
			this->arraySize_ += defaultSize;
		}
		this->stack_[this->top_] = std::move(value);
	}
	void pop() {
		if (this->empty_) {
			throw std::out_of_range("stack is empty!");
		}
		if (0 == this->top_) {
			this->empty_ = true;
		}
		else {
			this->top_--;
		}
		/*当栈容量过大时，自动减少容量*/
		if (this->arraySize_ - this->top_ > defaultSize) {
			_Ty* buffer = new _Ty[this->arraySize_ - defaultSize];
			for (int i = 0; i <= this->top_; i++) {
				buffer[i] = this->stack_[i];
			}
			delete[] this->stack_;
			this->stack_ = buffer;
			this->arraySize_ -= defaultSize;
		}
	}
	void clear() {
		if (this->stack_) {
			delete[] this->stack_;
		}
		this->stack_ = new _Ty[defaultSize];
		this->arraySize_ = defaultSize;
		this->top_ = 0;
		this->empty_ = true;
	}
	size_t size() const {
		if (this->empty_) {
			return 0;
		}
		else {
			return this->top_ + 1;//元素个数为顶元素的index + 1
		}
	}
	bool empty() const {
		return this->empty_;
	}
private:
	_Ty* stack_;
	size_t top_;
	size_t arraySize_;//存储数据的数组大小
	bool empty_;
	static constexpr size_t defaultSize = 100;
};

#endif //!_STACK_H_