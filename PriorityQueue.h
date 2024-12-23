#ifndef _PRIORITY_QUEUE_H_
#define _PRIORITY_QUEUE_H_

#include <stdexcept>

template<typename _Ty>
class PriorityQueue {
public:
	PriorityQueue() {
		this->queue_ = new _Ty[defaultSize];
		this->arraySize_ = defaultSize;
		this->top_ = 0;
		this->empty_ = true;
	}
	PriorityQueue(std::initializer_list<_Ty> list) {
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
		this->top_ = list.size() - 1;
		this->empty_ = false;
		makeHeap();
	}
	void push(const _Ty& value) {
		if (!this->empty_) {
			this->top_++;
		}
		else {
			this->empty_ = false;
		}
		/*当队列容量不足时自动扩大*/
		if (this->top_ >= this->arraySize_) {
			_Ty* buffer = new _Ty[this->arraySize_ + defaultSize];
			for (int i = 0; i < this->top_; i++) {
				buffer[i] = this->queue_[i];
			}
			delete[] this->queue_;
			this->queue_ = buffer;
			this->arraySize_ += defaultSize;
		}
		this->queue_[this->top_] = value;
		makeHeap();//每一次插入值后重新建堆
	}
	void push(_Ty&& value) {
		if (!this->empty_) {
			this->top_++;
		}
		else {
			this->empty_ = false;
		}
		/*当队列容量不足时自动扩大*/
		if (this->top_ >= this->arraySize_) {
			_Ty* buffer = new _Ty[this->arraySize_ + defaultSize];
			for (int i = 0; i < this->top_; i++) {
				buffer[i] = this->queue_[i];
			}
			delete[] this->queue_;
			this->queue_ = buffer;
			this->arraySize_ += defaultSize;
		}
		this->queue_[this->top_] = std::move(value);
		makeHeap();//每一次插入值后重新建堆
	}
	void pop() {
		if (this->empty_) {
			throw std::out_of_range("queue is empty!");
		}
		if (0 == this->top_) {
			this->empty_ = true;
		}
		else {
			//使用最大堆实现，堆顶是最大值，即数组的零位置
			//将最大值与最后一个值交换，然后降低top_的位置，做到弹出最大值
			swap(0, this->top_);
			this->top_--;
			checkNode(0);//最小值被交换到堆顶，由上到下检查最大堆的性质
		}
		/*当队列容量过大时，自动减少容量*/
		if (this->arraySize_ - this->top_ > defaultSize) {
			_Ty* buffer = new _Ty[this->arraySize_ - defaultSize];
			for (int i = 0; i <= this->top_; i++) {
				buffer[i - defaultSize] = this->queue_[i];
			}
			delete[] this->queue_;
			this->queue_ = buffer;
			this->arraySize_ -= defaultSize;
		}
	}
	const _Ty& top() {//返回const防止在外部改变值
		if (this->empty_) {
			throw std::out_of_range("queue is empty!");
		}
		return this->queue_[0];//使用最大堆实现，堆顶是最大值，即数组的零位置
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
	~PriorityQueue() {
		if (this->queue_) {
			delete[] this->queue_;
			this->queue_ = nullptr;
		}
	}
private:
	void swap(size_t index1, size_t index2) {
		_Ty temp = this->queue_[index1];
		this->queue_[index1] = this->queue_[index2];
		this->queue_[index2] = temp;
	}
	void checkNode(size_t index) {
		size_t leftIndex = (index * 2) + 1;
		size_t rightIndex = (index * 2) + 2;
		if (rightIndex <= this->top_) {//如果这个节点有右子树
			if (this->queue_[index] < this->queue_[rightIndex]) {
				swap(index, rightIndex);
				checkNode(rightIndex);
			}
		}
		if (leftIndex <= this->top_) {//如果这个节点有左子树
			if (this->queue_[index] < this->queue_[leftIndex]) {
				swap(index, leftIndex);
				checkNode(leftIndex);
			}
		}
	}
	void makeHeap() {
		if (this->empty_ || 0 == this->top_) {
			return;
		}
		for (size_t i = (this->top_ - 1) / 2; i > 0; i--) {//不能写i >= 0,因为size_t类型在等于零的时候减一会溢出，导致一直大于等于零
			checkNode(i);
		}
		checkNode(0);//在此处单独对零位置判断
	}
	_Ty* queue_;
	size_t arraySize_;
	size_t top_;
	bool empty_;
	static constexpr size_t defaultSize = 100;
};

#endif //!_PRIORITY_QUEUE_H_