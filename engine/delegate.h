#ifndef RENDER_DELEGATE_H
#define RENDER_DELEGATE_H

#include <vector>
#include <memory>


/* 委托的接口 */
template<typename... Args>
class IDelegate {
public:
    virtual ~IDelegate() = default;
    virtual void invoke(Args... args) = 0;
    virtual bool equal(IDelegate<Args...> *delegate) const = 0;
    virtual bool isType(const std::type_info &_type) const = 0;

};

/* 多个委托 */
template<typename ... Args>
class DelegateList {
public:
    ~DelegateList() {
        for (IDelegate<Args...> *d: this->delegates) {
            delete d;
        }
    }

    void add(IDelegate<Args...> *delegate) {
        // 是否已经有这个委托了
        for (IDelegate<Args...> *d: this->delegates) {
            if (d->equal(delegate)) {
                delete delegate;
                return;
            }
        }

        // 添加到委托列表
        this->delegates.push_back(delegate);
    }

    void invoke(Args... args) {
        for (IDelegate<Args...> *d: this->delegates) {
            d->invoke(args...);
        }
    }

private:
    std::vector<IDelegate<Args...>*> delegates;
};

/* 普通函数和类静态方法的委托 */
template<typename ... Args>
class FuncDelegate : public IDelegate<Args...> {
public:
    typedef void (*Func)(Args...);

    explicit FuncDelegate(Func func) : func(func) {}

    void invoke(Args... args) override {
        this->func(args...);
    }

    bool equal(IDelegate<Args...> *delegate) const override {
        // 首先判断类型是否相同
        if (!delegate->isType(typeid(FuncDelegate<Args...>)))
            return false;
        auto *cast = static_cast<FuncDelegate<Args...>*>(delegate);
        return cast->func == this->func;
    }

    bool isType(const std::type_info &_type) const override {
        return typeid(FuncDelegate<Args...>) == _type;
    }

private:
    Func func;

};


/* 类方法的委托 */
template<typename T, typename ... Args>
class MethodDelegate : public IDelegate<Args...> {
public:
    typedef void (T::*Method)(Args...);

    MethodDelegate(std::shared_ptr<T> obj, Method method) : obj(obj), method(method) {}

    void invoke(Args... args) override {
        auto bare_ptr = this->obj.get();
        (bare_ptr->*this->method)(args...);
    }

    bool equal(IDelegate<Args...> *delegate) const override {
        if (!delegate->isType(typeid(MethodDelegate<T, Args...>)))
            return false;
        auto cast = static_cast<MethodDelegate<T, Args...>*>(delegate);
        return cast->obj == this->obj && cast->method == this->method;
    }

    bool isType(const std::type_info &_type) const override {
        return typeid(MethodDelegate<T, Args...>) == _type;
    }

private:
    std::shared_ptr<T> obj;
    Method method;
};


#endif //RENDER_DELEGATE_H
