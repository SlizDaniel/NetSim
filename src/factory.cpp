//
// Created by micha on 19.01.2026.
//

#include "factory.hpp"
#include <stdexcept>
#include "nodes.hpp"

template<typename Node>
typename NodeCollection<Node>::iterator NodeCollection<Node>::find_by_id(ElementID id) {
    return std::find_if(container.begin(), container.end(), [id](const Node& node) {
        return node.get_id() == id;
    });
}

template<typename Node>
typename NodeCollection<Node>::const_iterator NodeCollection<Node>::find_by_id(ElementID id) const{
    return std::find_if(container.begin(), container.end(), [id](const Node& node) {
        return node.get_id() == id;
    });
}

template<typename Node>
void NodeCollection<Node>::remove_by_id(ElementID id){
    auto id_x = find_by_id(id);
    if (id_x != container.end()) {
        container.erase(id_x);
    }
}

bool has_reachable_storehouse(const PackageSender* sender,std::map<const PackageSender*,NodeColor>& node_colors){
    if (node_colors[sender]==NodeColor::VERIFIED) {
        return true;
    }
    node_colors[sender]=NodeColor::VISITED;
    if (sender->receiver_preferences_.get_preferences().empty()) {
        throw std::logic_error("Brak zdefiniowanych odbiorcow!");
    }
    bool has_neighbors = false;
    for (auto &pair : sender->receiver_preferences_.get_preferences()) {
        auto* receiver=pair.first;
        if (receiver->get_receiver_type()==ReceiverTypes::STOREHOUSE) {
            has_neighbors=true;
        }
        else if (receiver->get_receiver_type()==ReceiverTypes::WORKER) {
            auto worker_ptr = dynamic_cast<Worker*>(receiver);
            auto reworker_ptr = dynamic_cast<PackageSender*>(worker_ptr);

            if (static_cast<const void*>(reworker_ptr) == static_cast<const void*>(sender)) {
                continue;
            }
            has_neighbors=true;
            if (node_colors[worker_ptr]==NodeColor::UNVISITED) {
                has_reachable_storehouse(worker_ptr, node_colors);
            }
        }
    }
    if (has_neighbors) {
        node_colors[sender]=NodeColor::VERIFIED;
        return true;
    }
    else {
        throw std::logic_error("Brak prawidlowych odbiorcow");
    }
}

bool Factory::is_consistent(){
    std::map<const PackageSender*, NodeColor> node_colors;

    for (auto &worker : workers) {
        node_colors[&worker] = NodeColor::UNVISITED;
    }
    for (auto &ramp : ramps) {
        node_colors[&ramp] = NodeColor::UNVISITED;
    }
    try {
        for (auto &ramp : ramps) {
            has_reachable_storehouse (&ramp , node_colors);
            }
        }
    catch (const std::logic_error&) {
        return false;
    }
    return true;
};

void Factory::do_deliveries(Time t) {
    for (Ramp &r : ramps) {
        r.deliver_goods(t);
    }
}

void Factory::do_package_passing() {
    for (Worker &w : workers) {
        w.send_package();
    }
    for (Ramp &r : ramps) {
        r.send_package();
    }
}

void Factory::do_work(Time t) {
    for (Worker &w : workers) {
        w.do_work(t);
    }
}
