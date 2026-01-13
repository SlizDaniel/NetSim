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
        virtual size_t size() const = 0;
        virtual const_iterator begin() const = 0;
        virtual const_iterator end() const = 0;
        virtual const_iterator cbegin() const = 0;
        virtual const_iterator cend() const = 0;
};

class IPackageQueue : public IPackageStockpile{
public:
    ~IPackageQueue() override = default;

    virtual Package pop() = 0;
    //virtual PackageQueueType get_queue_type() const = 0;
};

class PackageQueue : public IPackageQueue {
public:
    explicit PackageQueue(PackageQueueType type) : queue_type_(type) {};
    ~PackageQueue() override = default;

    Package pop() override;

    void push(Package&& p) override {
        this -> package_list_.emplace_back(std::move(p));
    };
    bool empty() const override {
        return this -> package_list_.empty();
    };
    size_t size() const override {
        return this -> package_list_.size();
    };

    const_iterator begin() const override {
        return package_list_.begin();
    };
    const_iterator end() const override {
        return package_list_.end();
    };
    const_iterator cbegin() const override {
        return package_list_.cbegin();
    };
    const_iterator cend() const override {
        return package_list_.cend();
    };
private:
    std::list<Package> package_list_;
    PackageQueueType queue_type_;
};

#endif //NETSIM_STORAGE_TYPES_HPP