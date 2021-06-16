#ifndef RENDER_WITH_H
#define RENDER_WITH_H


/* 只有继承了这个类，才可以使用 with 语法 */
class With {
public:
    virtual void in() = 0;
    virtual void out() = 0;
    virtual ~With() = default;
};


/**
 * 用来调用 With 对象的包装器
 * 在构造时调用 With 对象的 in 方法，在析构时调用 With 对象的 out 方法
 * 该对象由宏定义自动生成
 * @tparam T With 类型的对象
 */
template<class T>
class WithWrap {
public:
    bool flag = true;
    explicit WithWrap(T &obj) : _obj(obj){
        this->_obj.in();
    }
    ~WithWrap() {
        this->_obj.out();
    }

private:
    T &_obj;
};


/**
 * 通过这个宏来使用 with 语法。
 * 在进入语法作用域时，自动调用 obj 的 in 方法；离开时自动调用 out 方法
 * @param T 表示 With 对象的类型
 * @param obj 要使用 with 作用域的对象
 */
#define with(T, obj) for(WithWrap<T> __with_##T##__(obj); (__with_##T##__).flag; (__with_##T##__).flag = false)

// todo 这种应对只是短暂的办法
/* 应对 T 是模版类型的情况 */
#define withT(T, obj) for(WithWrap<T> __with___(obj); (__with___).flag; (__with___).flag = false)

#endif //RENDER_WITH_H
