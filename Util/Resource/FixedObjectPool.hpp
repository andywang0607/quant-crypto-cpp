#ifndef __UTIL_FIXEDOBJECTPOOL_H__
#define __UTIL_FIXEDOBJECTPOOL_H__

#include <vector>

namespace Util::Resource {

template <typename ObjectType>
class FixedObjectPool
{
public:
    FixedObjectPool(size_t capacity)
        : capacity_(capacity)
        , bufferIndex_(0)
        , buffer_(std::vector<char>(capacity * sizeof(ObjectType)))
    {
        freeList_.reserve(capacity);
    }

    FixedObjectPool(const FixedObjectPool &) = delete;
    FixedObjectPool &operator=(const FixedObjectPool &) = delete;

    template <typename... Args>
    ObjectType *construct(Args &&... args)
    {
        if (!freeList_.empty()) {
            ObjectType *ret = freeList_.back();
            new (ret) ObjectType(args...);
            freeList_.pop_back();
            return ret;
        }
        if (bufferIndex_ == capacity_) {
            return nullptr;
        }
        ObjectType *ret = reinterpret_cast<ObjectType *>(&buffer_[sizeof(ObjectType) * bufferIndex_++]);
        new (ret) ObjectType(args...);
        return ret;
    }

    void recycle(ObjectType *obj)
    {
        if (isFromPool(obj)) {
            obj->~ObjectType();
            freeList_.push_back(obj);
        }
    }

    bool isFromPool(ObjectType *obj)
    {
        return ((char *)(&*obj) >= &buffer_[0]) && ((char *)(&*obj) <= &buffer_[(capacity_ - 1) * sizeof(ObjectType)]);
    }

private:
    size_t capacity_;
    size_t bufferIndex_;
    std::vector<char> buffer_;
    std::vector<ObjectType *> freeList_;
};
}

#endif // __UTIL_FIXEDOBJECTPOOL_H__