#ifndef _QUEUE_H_
#define _QUEUE_H_

#include <stdexcept>
#include <utility>

template <typename _Ty>
class Queue {
public:
	Queue() {
		this->queue_ = new _Ty[defaultSize];
		this->arraySize_ = defaultSize;
		this->front_ = this->back_ = 0;
		this->empty_ = true;
	}
	Queue(std::initializer_list<_Ty> list) {
		if (list.size() > arraySize_) {
			this->arraySize_ = defaultSize * (static_cast<size_t>(list.size() / defaultSize) + 1);
			this->queue_ = new _Ty[this->arraySize_];
		}
		else {
			this->queue_ = new _Ty[defaultSize];
			this->arraySize_ = defaultSize;
		}
		size_t i = 0;
		for (auto it = list.begin(); it != list.end(); ++it) {
			this->queue_[i] = *it;
			i++;
		}
		this->front_ = 0;
		this->back_ = list.size() - 1;
		this->empty_ = false;
	}
	~Queue() {
		if (nullptr != this->queue_) {
			delete[] this->queue_;
			this->queue_ = nullptr;
		}
	}
	_Ty& front() const {
		if (this->empty_) {
			throw std::out_of_range("queue is empty!");
		}
		return this->queue_[this->front_];
	}
	void push(const _Ty& value) {
		if (!this->empty_) {
			this->back_++;
		}
		else {
			this->empty_ = false;
		}
		/*当队列容量不足时自动扩大*/
		if (this->back_ >= this->arraySize_) {
			_Ty* buffer = new _Ty[this->arraySize_ + defaultSize];
			for (int i = this->front_; i < this->back_; i++) {
				buffer[i] = this->queue_[i];
			}
			delete[] this->queue_;
			this->queue_ = buffer;
			this->arraySize_ += defaultSize;
		}
		this->queue_[this->back_] = value;
	}
	void push(_Ty&& value) {
		if (!this->empty_) {
			this->back_++;
		}
		else {
			this->empty_ = false;
		}
		/*当队列容量不足时自动扩大*/
		if (this->back_ >= this->arraySize_) {
			_Ty* buffer = new _Ty[this->arraySize_ + defaultSize];
			for (int i = this->front_; i < this->back_; i++) {
				buffer[i] = this->queue_[i];
			}
			delete[] this->queue_;
			this->queue_ = buffer;
			this->arraySize_ += defaultSize;
		}
		this->queue_[this->back_] = std::move(value);
	}
	void pop() {
		if (this->empty_) {
			throw std::out_of_range("queue is empty!");
		}
		if (this->back_ == this->front_) {
			this->empty_ = true;
		}
		else {
			this->front_++;
		}
		/*当队列容量过大时，自动减少容量*/
		if (this->front_ >= defaultSize) {
			_Ty* buffer = new _Ty[this->arraySize_ - defaultSize];
			for (int i = this->front_; i <= this->back_; i++) {
				buffer[i - defaultSize] = this->queue_[i];
			}
			delete[] this->queue_;
			this->queue_ = buffer;
			this->front_ -= defaultSize;
			this->back_ -= defaultSize;
			this->arraySize_ -= defaultSize;
		}
	}
	size_t size() const {
		if (this->empty_) {
			return 0;
		}
		else
		{
			return this->back_ - this->front_ + 1;//元素个数为最后一个元素的index减去第一个元素的index再加一
		}
	}
	void clear() {
		if (nullptr != this->queue_) {
			delete[] this->queue_;
		}
		this->queue_ = new _Ty[defaultSize];
		this->arraySize_ = defaultSize;
		this->front_ = this->back_ = 0;
		this->empty_ = true;
	}
	bool empty() const {
		return this->empty_;
	}
private:
	_Ty* queue_;
	size_t arraySize_;//存储数据的数组大小
	size_t front_;
	size_t back_;
	bool empty_;
	static constexpr size_t defaultSize = 100;
};

#endif //!_QUEUE_H_