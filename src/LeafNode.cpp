#include "RecordPtr.hpp"
#include "LeafNode.hpp"

LeafNode::LeafNode(const TreePtr &tree_ptr) : TreeNode(LEAF, tree_ptr) {
    this->data_pointers.clear();
    this->next_leaf_ptr = NULL_PTR;
    if(!is_null(tree_ptr))
        this->load();
}

//returns max key within this leaf
Key LeafNode::max() {
    auto it = this->data_pointers.rbegin();
    return it->first;
}

//inserts <key, record_ptr> to leaf. If overflow occurs, leaf is split
//split node is returned
//TODO: LeafNode::insert_key to be implemented
TreePtr LeafNode::insert_key(const Key &key, const RecordPtr &record_ptr) {

    // this->data_pointers.insert({key,record_ptr});
    this->data_pointers[key] = record_ptr;
    this->size = this->data_pointers.size();

    TreePtr new_leaf = NULL_PTR; //if leaf is split, new_leaf = ptr to new split node ptr

    this->dump();
    if(FANOUT<this->size){
        LeafNode* new_leaf_node = new LeafNode();
        new_leaf = new_leaf_node->tree_ptr;
        auto iter = this->data_pointers.begin();
        for(int i=0;i<this->data_pointers.size()/2;i++){
            iter++;
        }
        while(iter!=this->data_pointers.end()){
            new_leaf_node->data_pointers[iter->first] = iter->second;

            iter++;
        }
        
        new_leaf_node->size = new_leaf_node->data_pointers.size();


        iter = new_leaf_node->data_pointers.begin();
        while(iter!=new_leaf_node->data_pointers.end()){
            this->data_pointers.erase(iter->first);
            iter++;
        }
        this->size = this->data_pointers.size();
        new_leaf_node->next_leaf_ptr = this->next_leaf_ptr;
        this->next_leaf_ptr = new_leaf_node->tree_ptr;
        this->dump();
        new_leaf_node->dump();
        return new_leaf_node->tree_ptr;
    }
    return new_leaf;
}


// merges the given cell with the given cell
void LeafNode::merge_above(TreePtr& tree_ptr) {
    // TreeNode* above_node = TreeNode::tree_node_factory(above_ptr);
    LeafNode above_node = LeafNode(tree_ptr);
    auto itr = above_node.data_pointers.begin();
    while(itr!=above_node.data_pointers.end()){
        this->data_pointers[itr->first]=itr->second;
        itr++;
    }
    
    this->size = this->data_pointers.size();
    this->next_leaf_ptr = above_node.next_leaf_ptr;
    this->dump();
    above_node.data_pointers.clear();
    above_node.size =0;
    above_node.dump();
}

// distributes with the given cell with the given cell
void LeafNode::distribute_above(TreePtr& tree_ptr) {
    LeafNode above_node = LeafNode(tree_ptr);
    auto itr1 = above_node.data_pointers.begin();
    while(itr1!=above_node.data_pointers.end()){
        cout<<itr1->first<<" "<<itr1->second<<"\n";
        itr1++;
    }
    // DONE PRINTING
    if(this->size<above_node.size){

        int num = 0;
        auto itr = above_node.data_pointers.begin();
        for(int i=0;i<(above_node.size-this->size)/2;i++){
            this->data_pointers[itr->first]=itr->second;

            itr++;
        }
        itr = this->data_pointers.begin();
        while(itr!=this->data_pointers.end()){
            above_node.data_pointers.erase(itr->first);
            itr++;
        }
        this->size = this->data_pointers.size();
        above_node.size = above_node.data_pointers.size();
        this->dump();
        above_node.dump();
    }
    else{
        // move from above to below

        int num = 0;
        auto itr = this->data_pointers.rbegin();
        for(int i=0;i<(this->size-above_node.size)/2;i++){
            above_node.data_pointers[itr->first] = itr->second;
            num++;
            itr++;
        }

        auto iter = above_node.data_pointers.begin();
        while(iter!=above_node.data_pointers.end()){
            this->data_pointers.erase(iter->first);
            iter++;
        }

        this->size = this->data_pointers.size();
        above_node.size = above_node.data_pointers.size();
        this->dump();
        above_node.dump();
    }
}


//key is deleted from leaf if exists
//TODO: LeafNode::delete_key to be implemented
void LeafNode::delete_key(const Key &key) {

    this->data_pointers.erase(key);
    this->size = this->data_pointers.size();
    this->dump();
}

//runs range query on leaf
void LeafNode::range(ostream &os, const Key &min_key, const Key &max_key) const {
    BLOCK_ACCESSES++;
    for(const auto& data_pointer : this->data_pointers){
        if(data_pointer.first >= min_key && data_pointer.first <= max_key)
            data_pointer.second.write_data(os);
        if(data_pointer.first > max_key)
            return;
    }
    if(!is_null(this->next_leaf_ptr)){
        auto next_leaf_node = new LeafNode(this->next_leaf_ptr);
        next_leaf_node->range(os, min_key, max_key);
        delete next_leaf_node;
    }
}

//exports node - used for grading
void LeafNode::export_node(ostream &os) {
    TreeNode::export_node(os);
    for(const auto& data_pointer : this->data_pointers){
        os << data_pointer.first << " ";
    }
    os << endl;
}

//writes leaf as a mermaid chart
void LeafNode::chart(ostream &os) {
    string chart_node = this->tree_ptr + "[" + this->tree_ptr + BREAK;
    chart_node += "size: " + to_string(this->size) + BREAK;
    for(const auto& data_pointer: this->data_pointers) {
        chart_node += to_string(data_pointer.first) + " ";
    }
    chart_node += "]";
    os << chart_node << endl;
}

ostream& LeafNode::write(ostream &os) const {
    TreeNode::write(os);
    for(const auto & data_pointer : this->data_pointers){
        if(&os == &cout)
            os << "\n" << data_pointer.first << ": ";
        else
            os << "\n" << data_pointer.first << " ";
        os << data_pointer.second;
    }
    os << endl;
    os << this->next_leaf_ptr << endl;
    return os;
}

istream& LeafNode::read(istream& is){
    TreeNode::read(is);
    this->data_pointers.clear();
    for(int i = 0; i < this->size; i++){
        Key key = DELETE_MARKER;
        RecordPtr record_ptr;
        if(&is == &cin)
            cout << "K: ";
        is >> key;
        if(&is == &cin)
            cout << "P: ";
        is >> record_ptr;
        this->data_pointers.insert(pair<Key,RecordPtr>(key, record_ptr));
    }
    is >> this->next_leaf_ptr;
    return is;
}