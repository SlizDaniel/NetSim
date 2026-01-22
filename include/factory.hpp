//
// Created by slizd on 7.01.2026.
//

#ifndef NETSIM_FACTORY_HPP
#define NETSIM_FACTORY_HPP
#include "types.hpp"
#include <list>
#include <algorithm>
#include "nodes.hpp"

enum class NodeColor {
    UNVISITED, VISITED , VERIFIED
};

template <typename Node>
class NodeCollection {
public:
    using container_t = std::list<Node>;
    using iterator = typename container_t::iterator;
    using const_iterator = typename container_t::const_iterator;

    iterator begin() {
        return container.begin();
    }
    iterator end() {
        return container.end();
    }
    const_iterator cbegin() const {
        return container.cbegin();
    }
    const_iterator cend() const {
        return container.cend();
    }



    void add(Node&& node) {
        container.push_back(std::move(node));
    }
    void remove_by_id(ElementID id) {
        auto id_x = find_by_id(id);
        if (id_x != container.end()) {
            container.erase(id_x);
        }
    };
    iterator find_by_id(ElementID id) {
        return std::find_if(container.begin(), container.end(), [id](const Node& node) {
            return node.get_id() == id;
        });
    };
    const_iterator find_by_id(ElementID id) const {
        return std::find_if(container.begin(), container.end(), [id](const Node& node) {
            return node.get_id() == id;
        });
    };

private:
    container_t container;
};

class Factory {
public:
    void add_ramp(Ramp&& ramp) {
        ramps.add(std::move(ramp));
    }
    void add_worker(Worker&& worker) {
        workers.add(std::move(worker));
    }
    void add_storehouse(Storehouse&& storehouse) {
        storehouses.add(std::move(storehouse));
    }

    void remove_ramp(ElementID id) {
        ramps.remove_by_id(id);
    };
    void remove_worker(ElementID id) {
        remove_receiver(workers, id);
    };
    void remove_storehouse(ElementID id) {
        remove_receiver(storehouses, id);
    };

    NodeCollection<Ramp>::iterator find_ramp_by_id(ElementID id) {
        return ramps.find_by_id(id);
    };
    NodeCollection<Ramp>::const_iterator find_ramp_by_id(ElementID id) const {
        return ramps.find_by_id(id);
    };

    NodeCollection<Worker>::iterator find_worker_by_id(ElementID id) {
        return workers.find_by_id(id);
    };
    NodeCollection<Worker>::const_iterator find_worker_by_id(ElementID id) const {
        return workers.find_by_id(id);
    };

    NodeCollection<Storehouse>::iterator find_storehouse_by_id(ElementID id) {
        return storehouses.find_by_id(id);
    };
    NodeCollection<Storehouse>::const_iterator find_storehouse_by_id(ElementID id) const{
        return storehouses.find_by_id(id);
    };



    NodeCollection<Ramp>::const_iterator ramp_cbegin() const {
        return ramps.cbegin();
    };
    NodeCollection<Ramp>::const_iterator ramp_cend() const {
        return ramps.cend();
    };


    NodeCollection<Worker>::const_iterator worker_cbegin() const {
        return workers.cbegin();
    };
    NodeCollection<Worker>::const_iterator worker_cend() const {
        return workers.cend();
    };


    NodeCollection<Storehouse>::const_iterator storehouse_cbegin() const {
        return storehouses.cbegin();
    };
    NodeCollection<Storehouse>::const_iterator storehouse_cend() const {
        return storehouses.cend();
    };

    bool is_consistent();
    void do_deliveries(Time t);
    void do_package_passing();
    void do_work(Time t);
private:
    NodeCollection<Ramp> ramps;
    NodeCollection<Worker> workers;
    NodeCollection<Storehouse> storehouses;

    template <typename Node>
    void remove_receiver(NodeCollection<Node>& collection, ElementID id) {
        auto it = collection.find_by_id(id);


        if (it != collection.end()) {
            IPackageReceiver* receiver_ptr = &(*it);
            for (auto& ramp : ramps) {
                ramp.remove_receiver(receiver_ptr);
            }
            for (auto& worker : workers) {
                worker.remove_receiver(receiver_ptr);
            }

            collection.remove_by_id(id);
        }
    }
};

enum class ElementType {
    WORKER , STOREHOUSE , RAMP , LINK
};

struct ParsedLineData {
    ElementType element_type;
    std::map<std::string , std::string> parameters;
};

ParsedLineData parse_line (std::string& line);

Factory load_factory_structure (std::istream& is);

void save_factory_structure(Factory& factory , std::ostream& os);

#endif //NETSIM_FACTORY_HPP