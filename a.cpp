// #include "assert"
#include "iostream"
#include "mutex"
#include "thread"
#include <cassert> // 提供 assert()
#include <memory>  // 提供 std::shared_ptr/std::weak_ptr（可选）
#include <mutex>   // 提供 std::mutex
#include <utility> // 提供 std::swap（可选）
class PtrBase {
public:
  // stl实现的智能指针还会在Counter中放入注册好的deleter
  struct Counter {
    int uses = 0;
    int weaks = 0;
  };
  using p_counter = Counter *;
  using p_mutex = std::mutex *;
  // 堆计数器用来在多个ptr间共享
  p_counter counter;
  // 堆内存mutex保证线程安全, 计数器为nullptr时才回收
  p_mutex mutex;

protected:
  // 这里用到了委托构造的技巧. 需要new计数器和互斥量
  // 注意由于用到new所以可能产生异常. 让异常逃离构造函数很麻烦,
  // 因此用nothrow然后自己处理
  PtrBase() noexcept
      : PtrBase(new(std::nothrow) Counter(), new(std::nothrow) std::mutex()) {}
  PtrBase(std::nullptr_t) noexcept : PtrBase() {}
  PtrBase(p_counter counter, p_mutex mutex) noexcept
      : counter(counter), mutex(mutex) {}

  void increase_shared_count() noexcept {
    if (counter != nullptr) {
      mutex->lock();
      ++(counter->uses);
      mutex->unlock();
    }
  }

  void increase_weak_count() noexcept {
    if (counter != nullptr) {
      mutex->lock();
      ++(counter->weaks);
      mutex->unlock();
    }
  }

  // 只要share计数为0就返回给指针本身以回收目标对象的内存
  bool reduce_shared_count() noexcept {
    bool is_zero = true;
    if (counter != nullptr) {
      mutex->lock();
      --(counter->uses);
      if (counter->uses > 0) {
        is_zero = false;
      }
      mutex->unlock();
    }
    return is_zero;
  }

  // 只有当两种引用都为0时才可以回收计数器本身的内存,
  // 记得所有对堆内存的修改都加锁
  void reduce_weak_count() noexcept {
    if (counter != nullptr) {
      mutex->lock();
      if (counter->weaks > 0) {
        --(counter->weaks);
      }
      if (counter->uses == 0 && counter->weaks == 0) {
        delete counter;
        counter = nullptr;
      }
      mutex->unlock();
    }
  }

  void check_mutex() noexcept {
    if (counter == nullptr) {
      delete mutex;
      mutex = nullptr;
    }
  }

  // new失败的时候做的补救措施
  void revert() noexcept {
    if (mutex != nullptr) {
      reduce_shared_count();
      reduce_weak_count();
      delete mutex;
      mutex = nullptr;
    }
    if (counter != nullptr) {
      delete counter;
      counter = nullptr;
    }
  }

  void swap(PtrBase &other) noexcept {
    std::swap(counter, other.counter);
    std::swap(mutex, other.mutex);
  }
};
template <typename T> class SharedPtr;
// class SharedPtr;
template <typename T> class WeakPtr : public PtrBase {
  using pointer = T *;
  friend class SharedPtr<T>;

private:
  pointer data;

private:
  void _destroy(WeakPtr &target) noexcept {
    if (data != nullptr) {
      target.reduce_weak_count();
      target.check_mutex();
    }
  }

public:
  // 用于外部获取指针原始类型
  typedef T ElementType;
  WeakPtr() noexcept : data(nullptr), PtrBase() {}
  ~WeakPtr() noexcept { _destroy(*this); }
  WeakPtr(const SharedPtr<T> &sptr) noexcept
      : data(sptr.data), PtrBase(sptr.counter, sptr.mutex) {
    if (data != nullptr) {
      // this->
      increase_weak_count();
    }
  }
  WeakPtr &operator=(const SharedPtr<T> &copy) noexcept {
    WeakPtr<T> tmp(copy);
    swap(tmp);
    return *this;
  }
  WeakPtr(const WeakPtr &copy) noexcept
      : data(copy.data), PtrBase(copy.counter, copy.mutex) {
    if (data != nullptr) {
      increase_weak_count();
    }
  }
  WeakPtr &operator=(const WeakPtr &copy) noexcept {
    WeakPtr tmp(copy);
    swap(tmp);
    if (data != nullptr) {
      increase_weak_count();
    }
    return *this;
  }
  WeakPtr &operator=(std::nullptr_t) noexcept {
    reset();
    return *this;
  }

  WeakPtr(WeakPtr &&moving) noexcept : data(nullptr), PtrBase() {
    swap(moving);
    _destroy(moving);
  }
  WeakPtr &operator=(WeakPtr &&moving) noexcept {
    if (this != &moving) {
      swap(moving);
      _destroy(moving);
    }
    return *this;
  }

  SharedPtr<T> lock() noexcept {
    if (expired()) {
      return SharedPtr<T>(nullptr);
      ;
    } else {
      return SharedPtr<T>(*this);
    }
  }

  void reset() noexcept { swap(WeakPtr()); }

  void swap(WeakPtr &other) noexcept {
    std::swap(data, other.data);
    PtrBase::swap(other);
  }

  void swap(WeakPtr &&other) noexcept {
    std::swap(data, other.data);
    PtrBase::swap(other);
  }

  int use_count() const noexcept {
    assert(counter != nullptr);
    return counter->uses;
  }
  bool expired() const noexcept { return counter->uses == 0; }
};
template <typename T> class SharedPtr : public PtrBase {
  using pointer = T *;
  // 需要和WeakPtr形成friend方便两者之间的转型
  friend class WeakPtr<T>;

private:
  // 原生指针
  pointer data;

private:
  // 先减少计数, 如果为0则释放资源
  void _destroy(SharedPtr &target) noexcept {
    if (data != nullptr) {
      if (target.reduce_shared_count()) {
        delete target.data;
        target.data = nullptr;
      }
      target.check_mutex();
    }
  }

  // 给WeakPtr用的构造
  SharedPtr(const WeakPtr<T> &wptr) noexcept
      : data(wptr.data), PtrBase(wptr.counter, wptr.mutex) {
    increase_shared_count();
  }

public:
  // 用于外部获取指针原始类型
  typedef T ElementType;

  // 默认构造函数, 全部赋为nullptr
  SharedPtr() noexcept : data(nullptr), PtrBase(){};

  // 记得显式以防止隐式类型转换
  explicit SharedPtr(const pointer &data) noexcept : data(data), PtrBase() {
    // nullptr代表空间申请失败
    if (counter == nullptr || mutex == nullptr) {
      this->data = nullptr;
      revert();
    }
    if (data != nullptr) {
      increase_shared_count();
    }
  }

  ~SharedPtr() noexcept { _destroy(*this); }

  // 拷贝构造, shared_ptr拷贝后会将计数器+1
  SharedPtr(const SharedPtr &copy) noexcept
      : data(copy.data), PtrBase(copy.counter, copy.mutex) {
    if (data != nullptr) {
      increase_shared_count();
    }
  }
  // 拷贝赋值, 采用copy-swap写法, 由于右值引用的存在, 折叠式写法会造成二义性
  // 旧的内存会由于tmp的析构而释放, 新的内存的申请也在tmp的拷贝构造中完成了
  SharedPtr &operator=(const SharedPtr &copy) noexcept {
    SharedPtr tmp(copy);
    swap(tmp);
    return *this;
  }
  // 用nullptr赋值时相当于清空
  SharedPtr &operator=(std::nullptr_t) noexcept {
    _destroy(*this);
    return *this;
  }

  // 移动构造函数, 由于是构造所以可以直接夺取指针内容
  // 析构的时候由于目标是nullptr所以自然结束掉
  SharedPtr(SharedPtr &&moving) noexcept : data(nullptr), PtrBase() {
    swap(moving);
    _destroy(moving);
  }
  // 移动赋值函数
  SharedPtr &operator=(SharedPtr &&moving) noexcept {
    if (this != &moving) {
      swap(moving);
      _destroy(moving);
    }
    return *this;
  }

  // 老三样
  pointer operator->() const noexcept {
    assert(data != nullptr);
    return data;
  }
  T &operator*() const noexcept {
    assert(data != nullptr);
    return *data;
  }
  pointer get() const noexcept { return data; }

  // 用于交换指针的成员函数
  void swap(SharedPtr &other) noexcept {
    std::swap(data, other.data);
    PtrBase::swap(other);
  }

  void swap(SharedPtr &&other) noexcept {
    std::swap(data, other.data);
    PtrBase::swap(other);
  }

  // 通过与新构造的对象交换来简化代码
  void reset(pointer p = nullptr) noexcept { swap(SharedPtr(p)); }

  // 返回当前计数器次数
  int use_count() const noexcept {
    assert(counter != nullptr);
    return counter->uses;
  }

  explicit operator bool() const noexcept { return data != nullptr; }
};
template <typename T, typename... Args>
// inline UniquePtr<T> MakeUnique(Args &&...args) {
//   return UniquePtr<T>(new T(std::forward<Args>(args)...));
// }
inline SharedPtr<T> MakeShared(Args &&...args) {
  return SharedPtr<T>(new T(std::forward<Args>(args)...));
}
class node {
public:
  int x{5}, y{10};
  node(int x, int y) : y(x) {
    // std::cout << "node constructor called\n";
  }
};
class nodex : public node {
public:
  nodex(int xx1, int yy1, int x, int y) : xx(xx1), yy(yy1), node(x, y) {}
  int xx, yy;
};
int main() {
  SharedPtr<nodex> x(new nodex(1, 2, 3, 4));
  std::cout << x.get()->x << "\n";
}