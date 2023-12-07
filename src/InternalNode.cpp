#include "InternalNode.hpp"

//creates internal node pointed to by tree_ptr or creates a new one
InternalNode::InternalNode(const TreePtr &tree_ptr) : TreeNode(INTERNAL, tree_ptr) {
    this->keys.clear();
    this->tree_pointers.clear();
    if(!is_null(tree_ptr))
        this->load();
}

//max element from tree rooted at this node
Key InternalNode::max() {
    Key max_key = DELETE_MARKER;
    TreeNode* last_tree_node = TreeNode::tree_node_factory(this->tree_pointers[this->size - 1]);
    max_key = last_tree_node->max();
    delete last_tree_node;
    return max_key;
}

//if internal node contains a single child, it is returned
TreePtr InternalNode::single_child_ptr() {
    if(this->size == 1)
        return this->tree_pointers[0];
    return NULL_PTR;
}

//inserts <key, record_ptr> into subtree rooted at this node.
//returns pointer to split node if exists
//TODO: InternalNode::insert_key to be implemented
TreePtr InternalNode::insert_key(const Key &key, const RecordPtr &record_ptr) {
    TreePtr new_tree_ptr = NULL_PTR;

    int index = 0;
    while(index<this->keys.size() && keys[index]<key){
        index++;
    }
    TreeNode* root_node = TreeNode::tree_node_factory(this->tree_pointers[index]);
    TreePtr potential_split_node_ptr = root_node->insert_key(key, record_ptr);
    if(!is_null(potential_split_node_ptr)){
        this->tree_pointers.insert(this->tree_pointers.begin() + index+1, potential_split_node_ptr);
        this->keys.insert(this->keys.begin() + index, root_node->max());
        if(this->keys.size()>index+1){
            this->keys[index+1] = TreeNode::tree_node_factory(this->tree_pointers[index+1])->max();
        }
        this->size = this->tree_pointers.size();
    }
    if(FANOUT<this->size){
        InternalNode* new_internal_node = new InternalNode();
        new_tree_ptr = new_internal_node->tree_ptr;
        index = this->tree_pointers.size()/2;
        for(int i = index;i<this->tree_pointers.size();i++){
            new_internal_node->tree_pointers.push_back(this->tree_pointers[i]);
        }

        for(int i= this->tree_pointers.size()-1;i>=index;i--){
            this->tree_pointers.pop_back();
        }

        this->keys.clear();
        for(int i=0;i<this->tree_pointers.size()-1;i++){
            TreeNode* temp = TreeNode::tree_node_factory(this->tree_pointers[i]);
            this->keys.push_back(temp->max());
        }

        for(int i=0;i<new_internal_node->tree_pointers.size()-1;i++){
            TreeNode* temp = TreeNode::tree_node_factory(new_internal_node->tree_pointers[i]);
            new_internal_node->keys.push_back(temp->max());
        }

        this->size = this->tree_pointers.size();
        new_internal_node->size = new_internal_node->tree_pointers.size();

        new_internal_node->dump();
    }
    this->dump();
    return new_tree_ptr;
}

// merges the given cell with the given cell
void InternalNode::merge_above(TreePtr& tree_ptr) {
    // TreeNode* above_node = TreeNode::tree_node_factory(above_ptr);
    InternalNode above_node = InternalNode(tree_ptr);
    for(int i=0;i<above_node.tree_pointers.size();i++){
        this->tree_pointers.push_back(above_node.tree_pointers[i]);
    }
    this->keys.clear();
    for(int i=0;i<this->tree_pointers.size()-1;i++){
        TreeNode* temp = TreeNode::tree_node_factory(this->tree_pointers[i]);
        this->keys.push_back(temp->max());
    }
    this->size = this->tree_pointers.size();
    this->dump();
    above_node.tree_pointers.clear();
    above_node.keys.clear();
    above_node.size =0;
}

// distributes with the given cell with the given cell
void InternalNode::distribute_above(TreePtr& tree_ptr) {
    InternalNode above_node = InternalNode(tree_ptr);
    if(this->size<above_node.size){
        int num = 0;
        for(int i=0;i<(above_node.size-this->size)/2;i++){
            this->tree_pointers.push_back(above_node.tree_pointers[i]);
            num++;
        }
        for(int i=0;i<num;i++){
            above_node.tree_pointers.erase(above_node.tree_pointers.begin());
        }
        above_node.keys.clear();
        this->keys.clear();
        for(int i=0;i<this->tree_pointers.size()-1;i++){
            TreeNode* temp = TreeNode::tree_node_factory(this->tree_pointers[i]);
            this->keys.push_back(temp->max());
        }
        for(int i=0;i<above_node.tree_pointers.size()-1;i++){
            TreeNode* temp = TreeNode::tree_node_factory(above_node.tree_pointers[i]);
            above_node.keys.push_back(temp->max());
        }
        this->size = this->tree_pointers.size();
        above_node.size = above_node.tree_pointers.size();
        this->dump();
        above_node.dump();
    }
    else{
        // move from above to below
        int num = 0;
        for(int i=0;i<(this->size-above_node.size)/2;i++){
            above_node.tree_pointers.insert(above_node.tree_pointers.begin(), this->tree_pointers[this->tree_pointers.size() -1 -i]);
            num++;
        }
        for(int i=0;i<num;i++){
            this->tree_pointers.pop_back();
        }
        above_node.keys.clear();
        this->keys.clear();
        for(int i=0;i<this->tree_pointers.size()-1;i++){
            TreeNode* temp = TreeNode::tree_node_factory(this->tree_pointers[i]);
            this->keys.push_back(temp->max());
        }
        for(int i=0;i<above_node.tree_pointers.size()-1;i++){
            TreeNode* temp = TreeNode::tree_node_factory(above_node.tree_pointers[i]);
            above_node.keys.push_back(temp->max());
        }
        this->size = this->tree_pointers.size();
        above_node.size = above_node.tree_pointers.size();
        this->dump();
        above_node.dump();
    }
}


//deletes key from subtree rooted at this if exists
//TODO: InternalNode::delete_key to be implemented
void InternalNode::delete_key(const Key &key) {
    TreePtr new_tree_ptr = NULL_PTR;

    int index = 0;
    while(index<this->keys.size() && keys[index]<key){
        index++;
    }
    // index is the index of the wanted leaf
    TreeNode* wanted_node = TreeNode::tree_node_factory(this->tree_pointers[index]);
    wanted_node->delete_key(key);
    // called delete key
    if(wanted_node->size < FANOUT/2 + (int)(FANOUT%2!=0)){
        int flag = 0;
        if(index-1>=0){
            cout<<"her\n";
            TreeNode* below_node = TreeNode::tree_node_factory(this->tree_pointers[index-1]);
            if(below_node->size + wanted_node->size <= FANOUT){
                flag = 1;
                cout<<"her2\n";
                below_node->merge_above(this->tree_pointers[index]);
                cout<<"reached12\n";
                this->tree_pointers.erase(this->tree_pointers.begin()+index);
                cout<<"reached1\n";
                if(this->keys.size()>index){
                    this->keys.erase(this->keys.begin()+index);
                }
                this->size = this->tree_pointers.size();
                cout<<"reached\n";
            }
            else{
                flag = 1;
                below_node->distribute_above(this->tree_pointers[index]);
            }
        }
        else if(flag==0 && index+1<this->tree_pointers.size()){

            TreeNode* above_node = TreeNode::tree_node_factory(this->tree_pointers[index+1]);
            if(above_node->size + wanted_node->size <= FANOUT){

                wanted_node->merge_above(this->tree_pointers[index+1]);
                this->tree_pointers.erase(this->tree_pointers.begin()+index+1);
                if(this->keys.size()>index+1){
                    this->keys.erase(this->keys.begin()+index+1);
                }
                this->size = this->tree_pointers.size();
            }
            else{

                wanted_node->distribute_above(this->tree_pointers[index+1]);
            }
        }
    }
    this->dump();
}

//runs range query on subtree rooted at this node
void InternalNode::range(ostream &os, const Key &min_key, const Key &max_key) const {
    BLOCK_ACCESSES++;
    for(int i = 0; i < this->size - 1; i++){
        if(min_key <= this->keys[i]){
            auto* child_node = TreeNode::tree_node_factory(this->tree_pointers[i]);
            child_node->range(os, min_key, max_key);
            delete child_node;
            return;
        }
    }
    auto* child_node = TreeNode::tree_node_factory(this->tree_pointers[this->size - 1]);
    child_node->range(os, min_key, max_key);
    delete child_node;
}

//exports node - used for grading
void InternalNode::export_node(ostream &os) {
    TreeNode::export_node(os);
    for(int i = 0; i < this->size - 1; i++)
        os << this->keys[i] << " ";
    os << endl;
    for(int i = 0; i < this->size; i++){
        auto child_node = TreeNode::tree_node_factory(this->tree_pointers[i]);
        child_node->export_node(os);
        delete child_node;
    }
}

//writes subtree rooted at this node as a mermaid chart
void InternalNode::chart(ostream &os) {
    string chart_node = this->tree_ptr + "[" + this->tree_ptr + BREAK;
    chart_node += "size: " + to_string(this->size) + BREAK;
    chart_node += "]";
    os << chart_node << endl;

    for(int i = 0; i < this->size; i++) {
        auto tree_node = TreeNode::tree_node_factory(this->tree_pointers[i]);
        tree_node->chart(os);
        delete tree_node;
        string link = this->tree_ptr + "-->|";

        if(i == 0)
            link += "x <= " + to_string(this->keys[i]);
        else if (i == this->size - 1) {
            link += to_string(this->keys[i-1]) + " < x";
        } else {
            link += to_string(this->keys[i-1]) + " < x <= " + to_string(this->keys[i]);
        }
        link += "|" + this->tree_pointers[i];
        os << link << endl;
    }
}

ostream& InternalNode::write(ostream &os) const {
    TreeNode::write(os);
    for(int i = 0; i < this->size - 1; i++){
        if(&os == &cout)
            os << "\nP" << i+1 << ": ";
        os << this->tree_pointers[i] << " ";
        if(&os == &cout)
            os << "\nK" << i+1 << ": ";
        os << this->keys[i] << " ";
    }
    if(&os == &cout)
        os << "\nP" << this->size << ": ";
    os << this->tree_pointers[this->size - 1];
    return os;
}

istream& InternalNode::read(istream& is){
    TreeNode::read(is);
    this->keys.assign(this->size - 1, DELETE_MARKER);
    this->tree_pointers.assign(this->size, NULL_PTR);
    for(int i = 0; i < this->size - 1; i++){
        if(&is == &cin)
            cout << "P" << i+1 << ": ";
        is >> this->tree_pointers[i];
        if(&is == &cin)
            cout << "K" << i+1 << ": ";
        is >> this->keys[i];
    }
    if(&is == &cin)
        cout << "P" << this->size;
    is >> this->tree_pointers[this->size - 1];
    return is;
}
