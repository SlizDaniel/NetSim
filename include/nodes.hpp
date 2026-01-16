//
// Created by slizd on 7.01.2026.
//

#ifndef NETSIM_NODES_HPP
#define NETSIM_NODES_HPP

#include <map>
#include <memory>

#include "storage_types.hpp"

class IPackageReceiver{

public:
    virtual void receive_package(Package&& p) = 0;

    virtual ElementID get_id() const = 0;

    virtual const_iterator begin() const = 0;

    virtual const_iterator end() const = 0;

    virtual const_iterator cbegin() const = 0;

    virtual const_iterator cend() const = 0;

    virtual ~IPackageReceiver() = default;
};

class ReceiverPreferences {
public:
    using preferences_t = std::map<IPackageReceiver*, double>;
    using const_iterator = preferences_t::const_iterator;
    const preferences_t& get_preferences() const;


private:
    preferences_t preferences;
};


class Storehouse : IPackageReceiver {
public:
    Storehouse(ElementID id, std::unique_ptr<IPackageStockpile> d) {
        this -> id_ = id;
        this -> d_ = std::move(d);
    }
    void receive_package(Package&& p) override;
    ElementID get_id() const override {
        return id_;
    }

    const_iterator begin() const override {
        return d_->begin();
    };
    const_iterator end() const override {
        return d_->end();
    }
    const_iterator cbegin() const override {
        return d_->cbegin();
    }
    const_iterator cend() const override {
        return d_->cend();
    }

private:
    ElementID id_;
    std::unique_ptr<IPackageStockpile> d_;
};



#endif //NETSIM_NODES_HPP