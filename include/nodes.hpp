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

    virtual IPackageStockpile::const_iterator begin() const = 0;

    virtual IPackageStockpile::const_iterator end() const = 0;

    virtual IPackageStockpile::const_iterator cbegin() const = 0;

    virtual IPackageStockpile::const_iterator cend() const = 0;

    virtual ~IPackageReceiver() = default;
};

class ReceiverPreferences {
public:
    using preferences_t = std::map<IPackageReceiver*, double>;
    using const_iterator = preferences_t::const_iterator;

    ReceiverPreferences(ProbabilityGenerator pg) : pg_(std::move(pg)) {};

    void add_receiver(IPackageReceiver* ptr);
    void remove_receiver(IPackageReceiver* ptr);

    static IPackageReceiver* choose_receiver();
    const preferences_t& get_preferences() const;

    const_iterator begin() {
        return preferences_t_.begin();
    };
    const_iterator end() {
        return preferences_t_.end();
    };

private:
    std::map<IPackageReceiver*, double> preferences_t_;
    ProbabilityGenerator pg_;
};


class Storehouse : public IPackageReceiver {
public:
    Storehouse(ElementID id, std::unique_ptr<IPackageStockpile> d) {
        this -> id_ = id;
        this -> d_ = std::move(d);
    }
    void receive_package(Package&& p) override;
    ElementID get_id() const override {
        return id_;
    }

    IPackageStockpile::const_iterator begin() const override {
        return d_->begin();
    };
    IPackageStockpile::const_iterator end() const override {
        return d_->end();
    }
    IPackageStockpile::const_iterator cbegin() const override {
        return d_->cbegin();
    }
    IPackageStockpile::const_iterator cend() const override {
        return d_->cend();
    }

private:
    ElementID id_;
    std::unique_ptr<IPackageStockpile> d_;
};



#endif //NETSIM_NODES_HPP