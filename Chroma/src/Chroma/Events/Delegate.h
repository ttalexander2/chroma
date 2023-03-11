#pragma once
#include "DelegateBase.h"

namespace Chroma
{
	template <typename T> class Delegate;
	template <typename T> class MulticastDelegate;
	template <typename O, typename R, typename... A> class Event;

	template<typename Ret, typename ...Args>
	class Delegate<Ret(Args...)> final : private DelegateBase<Ret(Args...)>
	{
	public:
	    Delegate() = default;
	    
	    bool is_null() const { return invocation.stub == nullptr; }
	    bool operator==(void* ptr) const { return (ptr == nullptr) && this->is_null(); }
	    bool operator!=(void* ptr) const { return (ptr != nullptr) || !this->is_null(); }

	    Delegate(const Delegate& other) {
	        other.invocation.clone(invocation);
	    }

	    template <typename Lambda>
	    Delegate(const Lambda& lambda) {
	        assign((void*)(&lambda), lambda_stub<Lambda>);
	    }

	    Delegate& operator=(const Delegate& other)
	    {
	        other.invocation.clone(invocation);
	        return *this;
	    }

	    template <typename Lambda>
	    Delegate& operator=(const Lambda& instance) {
	        assign((void*)(&instance), lambda_stub<Lambda>);
	        return *this;
	    }

	    bool operator==(const Delegate& other) const { 
	        return invocation == other.invocation;
	    }

	    bool operator!=(const Delegate& other) const {
	        return invocation != other.invocation;
	    }

	    bool operator==(const MulticastDelegate<Ret(Args...)>& other) const {
	        return other == *this;
	    }

	    bool operator!=(const MulticastDelegate<Ret(Args...)>& other) const {
	        return other != *this;
	    }

	    template <class T, Ret(T::*TMethod)(Args...)>
	    static Delegate create(T* instance) {
	        return Delegate(instance, method_stub<T, TMethod>);
	    }

	    template <class T, Ret(T::*TMethod)(Args...) const>
	    static Delegate create(T const* instance) {
	        return Delegate(const_cast<T*>(instance), const_method_stub<T, TMethod>);
	    }

	    template <Ret(*TMethod)(Args...)>
	    static Delegate create() {
	        return Delegate(nullptr, function_stub<TMethod>);
	    }

	    template <typename Lambda>
	    static Delegate create(const Lambda& instance) {
	        return Delegate((void*)(&instance), lambda_stub<Lambda>);
	    }

	    template <class T, Ret(T::*TMethod)(Args...)>
	    void bind(T* instance) {
	        invocation.object = instance;
	        invocation.stub = method_stub<T, TMethod>;
	    }

	    template <class T, Ret(T::*TMethod)(Args...) const>
	    void bind(T const* instance) {
	        invocation.object = const_cast<T*>(instance);
	        invocation.stub = const_method_stub<T, TMethod>;
	    }

	    template <Ret(*TMethod)(Args...)>
	    void bind() {
	        invocation.object = nullptr;
	        invocation.stub = function_stub<TMethod>;
	    }

	    template <typename Lambda>
	    void bind(const Lambda& instance) {
	        invocation.object = (void*)(&instance);
	        invocation.stub = lambda_stub<Lambda>;
	    }

	    Ret operator()(Args... args) const {
	        return (*invocation.stub)(invocation.object, args...);
	    }

	    Ret invoke(Args... args) const {
	        return (*invocation.stub)(invocation.object, args...);
	    }

	private:

	    Delegate(void* obj, typename DelegateBase<Ret(Args...)>::StubType stub) {
	        invocation.object = obj;
	        invocation.stub = stub;
	    }

	    void assign(void* obj, typename DelegateBase<Ret(Args...)>::StubType stub) {
	        this->invocation.object = obj;
	        this->invocation.stub = stub;
	    }

	    template <class T, Ret(T::*TMethod)(Args...)>
	    static Ret method_stub(void* this_ptr, Args... args) {
	        T* p = static_cast<T*>(this_ptr);
	        return (p->*TMethod)(args...);
	    }

	    template <class T, Ret(T::*TMethod)(Args...) const>
	    static Ret const_method_stub(void* this_ptr, Args... args) {
	        T* const p = static_cast<T*>(this_ptr);
	        return (p->*TMethod)(args...);
	    }

	    template <Ret(*TMethod)(Args...)>
	    static Ret function_stub(void* this_ptr, Args... args) {
	        return (TMethod)(args...);
	    }

	    template <typename Lambda>
	    static Ret lambda_stub(void* this_ptr, Args... args) {
	        Lambda* p = static_cast<Lambda*>(this_ptr);
	        return (p->operator())(args...);
	    }

	    friend class MulticastDelegate<Ret(Args...)>;
	    template <typename O, typename R, typename... A> friend class event;
	    typename DelegateBase<Ret(Args...)>::InvocationElement invocation;



	};
}

