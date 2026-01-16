//
// Created by slizd on 7.01.2026.
//

#ifndef NETSIM_NODES_HPP
#define NETSIM_NODES_HPP

#include <map>
#include <memory>
#include <optional>

#include "storage_types.hpp"

enum class ReceiverTypes {
    WORKER , STOREHOUSE
};

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

    static ReceiverTypes get_receiver_type () {return ReceiverTypes::STOREHOUSE;}

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

class Worker : public PackageSender,  public IPackageQueue {
public:
    Worker(ElementID id , TimeOffset pd , std::unique_ptr<IPackageQueue> q);

    Time do_work ();

    TimeOffset get_processing_duration () const {return processing_duration;}

    Time get_package_processing_start_time () const {return package_processing_start_time;}

    ElementID get_id () const {return id_;}

    static ReceiverTypes get_receiver_type () {return ReceiverTypes::WORKER;}

    void receive_package (Package&& p) override;

    Package receive_package () const override ;

    const_iterator end () const override;

    const_iterator begin () const override;

    const_iterator cend () const override;

    const_iterator cbegin() const override;
private:
    ElementID id_;

    TimeOffset processing_duration;

    Time package_processing_start_time;

    std::unique_ptr<IPackageQueue> queue;

    std::optional<Package> processing_buffer;
};


#endif //NETSIM_NODES_HPP