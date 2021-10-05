#include <memory>
#include <typeindex>
#include <type_traits>

struct Any
{
    Any(void) : tindex_(std::type_index(typeid(void))) {}
    Any(const Any& rhs) : ptr_(rhs.ptr_), tindex_(rhs.tindex_) {}
    Any(Any&& rhs) : ptr_(std::move(rhs.ptr_)), tindex_(rhs.tindex_) {}

    template<typename U, class = typename std::enable_if<!std::is_same<typename std::decay<U>::type, Any>::value, U>::type>
    Any(U&& value)
        : ptr_(new Derived<typename std::decay<U>::type>(std::forward<U>(value))), tindex_(std::type_index(typeid(typename std::decay<U>::type)))
    {
    }

    Any& operator=(const Any& rhs)
    {
        if (ptr_ == rhs.ptr_) return *this;
        ptr_ = rhs.ptr_;
        tindex_ = rhs.tindex_;
        return *this;
    }

    template<typename U, class = typename std::enable_if<!std::is_same<typename std::decay<U>::type, Any>::value, U>::type>
    void Reset(U&& value)
    {
        ptr_.reset(new Derived<typename std::decay<U>::type>(std::forward<U>(value)));
        tindex_ = std::type_index(typeid(typename std::decay<U>::type));
    }

    bool IsNull() const
    {
        return !bool(ptr_);
    }

    template<class U>
    bool Is() const
    {
        return tindex_ == std::type_index(typeid(U));
    }

    template<class U>
    U& AnyCast()
    {
        if (!Is<U>() || IsNull())
        {
            static U unexpected_obj;
            return unexpected_obj;
        }
        auto derived = dynamic_cast<Derived<U>*>(ptr_.get());
        return derived->value_;
    }

private:
    struct Base;
    typedef std::shared_ptr<Base> BasePtr;

    struct Base
    {
        virtual ~Base() {}
    };

    template<typename T>
    struct Derived : Base
    {
        template<typename U>
        Derived(U&& value) : value_(std::forward<U>(value))
        {
        }

        T value_;
    };

    BasePtr ptr_;
    std::type_index tindex_;
};
