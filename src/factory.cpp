//
// Created by micha on 19.01.2026.
//

#include "factory.hpp"
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
