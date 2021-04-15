#ifndef RENDER_WITH_H
#define RENDER_WITH_H


/* 只有继承了这个类，才可以使用 with 语法 */
class With {
public:
    virtual void in() = 0;
    virtual void out() = 0;
};


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

#define with(T, obj) for(WithWrap<T> __with_##T##__(obj); (__with_##T##__).flag; (__with_##T##__).flag = false)


#endif //RENDER_WITH_H
