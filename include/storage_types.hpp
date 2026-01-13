//
// Created by slizd on 7.01.2026.
//

#ifndef NETSIM_STORAGE_TYPES_HPP
#define NETSIM_STORAGE_TYPES_HPP
#include <cstddef>
#include <list>
#include "package.hpp"

using const_iterator = std::list<Package>::const_iterator;

enum class PackageQueueType {
    FIFO,
    LIFO
};

class IPackageStockpile {
    public:
        virtual ~IPackageStockpile() {}
        virtual void push(Package&& p) = 0;
        virtual bool empty() const = 0;
        virtual size_t size();
        virtual const_iterator begin() const = 0;
        virtual const_iterator end() const = 0;
};

class IPackageQueue : public IPackageStockpile{
public:
    ~IPackageQueue() override = default;

    virtual Package pop() = 0;
    virtual PackageQueueType get_queue_type() const = 0;
};


#endif //NETSIM_STORAGE_TYPES_HPP