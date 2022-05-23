/**
 * Project 2 - Binary Trees
 * DiscriminatorTree.cpp
 * Implementation for the DTree class.
 */

#include "dtree.h"

void DNode::clearProperties() {
    _size = 0;
    _numVacant = 0;
    _vacant = false;
    _left = nullptr;
    _right = nullptr;
}

/**
 * Destructor, deletes all dynamic memory.
 */
DTree::~DTree() {
    clear(_root);
    _root = nullptr;
}

DTree& DTree::operator=(const DTree& rhs) {
    if(this != &rhs) {
        clear();
        copyTree(_root, rhs._root);
    }
    return *this;
}

/**
 * Dynamically allocates a new DNode in the tree. 
 * Should also update heights and detect imbalances in the traversal path
 * an insertion.
 * @param newAcct Account object to be contained within the new DNode
 * @return true if the account was inserted, false otherwise
 */
bool DTree::insert(Account newAcct) {
    return insert(newAcct, _root);
}
bool DTree::insert(Account newAcct, DNode*& node) {
//    DNode* imbalanced = nullptr;
    bool inserted = false;

    /* if new node is at a leaf position, drop here, and DON'T CHECK the rest of the function
    since there would NOT be a vacant node, NOR would the node need to travel any more, NOR 
    would we need to update or balance */
    if(node == nullptr) 
    {
        node = new DNode(newAcct);
        return true;
    } 
    else 
    {
        /* Can we fit the new account into a vacant node? */
        /* AND if so... again, don't need to check THE REST (hence return true) since already balanaced */
        int l_disc = getNodeAttribute(node->_left, min_disc);
        int r_disc = getNodeAttribute(node->_right, max_disc);
        if(retrieve(node->getDiscriminator()) == nullptr && node->isVacant() && 
           newAcct.getDiscriminator() > l_disc && newAcct.getDiscriminator() < r_disc) {
            node->_account = newAcct;
            node->_vacant = false;
            node->_numVacant -= 1;
            return true;
        }
    }
    
    /* Now, if we got here, it's not a leaf, YET, so start looking */
    /* But notice AS we are moving left and right we are then going to check if balanced */
    /* using the REST of the code below, EVEN if there is a duplicate!! */
    /* notice no "return" from here out, so it ALWAYS updatesSize and NumVacant and checks */
    /* imbalanace */
    if(newAcct.getDiscriminator() < node->getDiscriminator()) 
    { inserted = insert(newAcct, node->_left); } 
    else if(newAcct.getDiscriminator() > node->getDiscriminator()) 
    { inserted = insert(newAcct, node->_right); } 
    else if(!node->isVacant()) // then a duplicate!! 
    { inserted = false; }

    updateSize(node);
    updateNumVacant(node);

    /* TODO: This does not check of the parent is imbalanced first */
    if(checkImbalance(node)) 
    { rebalance(node); }

    return inserted;
}

/**
 * Removes the specified DNode from the tree.
 * @param disc discriminator to match
 * @param removed DNode object to hold removed account
 * @return true if an account was removed, false otherwise
 */
bool DTree::remove(int disc, DNode*& removed) {
    return remove(disc, removed, _root);
}
bool DTree::remove(int disc, DNode*& removed, DNode* node) {
    bool success = false;
    if(node == nullptr) {
        return false;
    } else if(disc < node->getDiscriminator()) {
        success = remove(disc, removed, node->_left);
    } else if(disc > node->getDiscriminator()) {
        success = remove(disc, removed, node->_right);
    } else {
        removed = node;
        node->_vacant = true;
        node->_numVacant += 1;
        success = true;
    }
    updateNumVacant(node);
    return success;
}

/**
 * Retrieves the specified Account within a DNode.
 * @param disc discriminator int to search for
 * @return DNode with a matching discriminator, nullptr otherwise
 */
DNode* DTree::retrieve(int disc) {
    return retrieve(disc, _root);
}
DNode* DTree::retrieve(int disc, DNode* node) {
    if(node == nullptr) {
        return nullptr;
    } else if(disc < node->getDiscriminator()) {
        return retrieve(disc, node->_left);
    } else if(disc > node->getDiscriminator()) {
        return retrieve(disc, node->_right);
    } else {
        if(node->isVacant()) {
            return nullptr;
        }
        return node;
    }
}

/**
 * Helper for the destructor to clear dynamic memory.
 */
void DTree::clear() {
 clear(_root);
}
void DTree::clear(DNode* node) {
    if(node == nullptr) return;
    clear(node->_left);
    clear(node->_right);
    delete node;
}

/**
 * Prints all accounts' details within the DTree.
 */
void DTree::printAccounts() const {
    printAccounts(_root);
}
void DTree::printAccounts(DNode* node) const {
    if(node == nullptr) return;
    printAccounts(node->_left);
    (!node->isVacant())? cout << node->getAccount() << endl : cout << "";
    printAccounts(node->_right);
}

/**
 * Dump the DTree in the '()' notation.
 */
void DTree::dump(DNode* node) const {
    if(node == nullptr) return;
    cout << "(";
    dump(node->_left);
    cout << node->getAccount().getDiscriminator() << ":" << node->getSize() << ":" << node->getNumVacant();
    dump(node->_right);
    cout << ")";
}

/**
 * Returns the number of valid users in the tree.
 * @return number of non-vacant nodes
 */
int DTree::getNumUsers() const {
    return (_root->_size-_root->_numVacant);
}

/**
 * Updates the size of a node based on the imedaite children's sizes
 * @param node DNode object in which the size will be updated
 */
void DTree::updateSize(DNode* node) {
    int l_size = getNodeAttribute(node->_left, size);
    int r_size = getNodeAttribute(node->_right, size);
    //dump();
    //cout << endl << node->getDiscriminator() << endl;
    //cout << l_size << " " << r_size << endl;
    node->_size = 1 + l_size + r_size;
}


/**
 * Updates the number of vacant nodes in a node's subtree based on the immediate children
 * @param node DNode object in which the number of vacant nodes in the subtree will be updated
 */
void DTree::updateNumVacant(DNode* node) {
    int l_vacant = getNodeAttribute(node->_left, vacant);
    int r_vacant = getNodeAttribute(node->_right, vacant);
    node->_numVacant = l_vacant + r_vacant + ((node->isVacant())? 1:0);
}

/**
 * Checks for an imbalance, defined by 'Discord' rules, at the specified node.
 * @param checkImbalance DNode object to inspect for an imbalance
 * @return (can change) returns true if an imbalance occured, false otherwise
 */
bool DTree::checkImbalance(DNode* node) {
    int l_size = getNodeAttribute(node->_left, size);
    int r_size = getNodeAttribute(node->_right, size);
    if(l_size < 4 && r_size < 4) return false;
    return (l_size >= 1.5*r_size || l_size >= 1.5*r_size);
}

/**
 * Begins and manages the rebalancing process for a 'Discrd' tree (pass by reference).
 * @param node DNode root of the subtree to balance
 */
void DTree::rebalance(DNode*& node) {
    DNode** tmp = new DNode * [getNodeAttribute(node, users)];
    int z = 0;
    treeToArray(node, tmp, z);
    arrayToTree(node, tmp, getNodeAttribute(node, users)-1);
    delete[] tmp;
}

/* Find a way to not require passing in 0? */
/**
 * Moves the tree to a sorted array
 * @param node DNode to start rebalancing at
 * @param arr array pointer to the empty array
 * @param i counter to keep track of the array index
 */
void DTree::treeToArray(DNode* node, DNode** arr, int& i) {
    if(node == nullptr) return;
    treeToArray(node->_left, arr, i);
    if(!node->isVacant()) {
        arr[i] = node;
        i++;
    }
    treeToArray(node->_right, arr, i);
    if(node->isVacant()) {
        delete node;
        node = nullptr;
    }
}

/**
 * Moves the nodes in a sorted array into a tree
 * @param node root DNode of the tree
 * @param arr array pointer to the sorted array
 * @param upper upper bound index of array subsection 
 * @param lower=0 lower bound index of the array subsection
 */
void DTree::arrayToTree(DNode*& node, DNode** arr, int upper, int lower) {
    if(lower > upper) return;

    int mid = (upper + lower)/2;
    node = arr[mid];
    node->clearProperties();

    arrayToTree(node->_left, arr, mid-1, lower);
    arrayToTree(node->_right, arr, upper, mid+1);
    updateSize(node);
    updateNumVacant(node);
}

void DTree::copyTree(DNode*& dest, DNode* src) {
    if(src == nullptr) return;
    dest = new DNode(*src);
    copyTree(dest->_left, src->_left);
    copyTree(dest->_right, src->_right);
}

int DTree::getNodeAttribute(DNode* node, nodeAttrib attrib) {
    switch (attrib)
    {
    case size:
        return (node != nullptr)? node->getSize() : DEFAULT_SIZE-1;
        break;
    case vacant:
        return (node != nullptr)? node->getNumVacant() : DEFAULT_NUM_VACANT;
        break;
    case users:
        return (node != nullptr)? node->getSize()-node->getNumVacant() : DEFAULT_SIZE;
        break;
    case min_disc:
        return (node != nullptr)? node->getDiscriminator() : MIN_DISC-1;
        break;
    case max_disc:
        return (node != nullptr)? node->getDiscriminator() : MAX_DISC+1;
        break;
    default:
        return -1;
        break;
    }
}

/**
 * Overloaded << operator for an Account to print out the account details
 * @param sout ostream object
 * @param acct Account objec to print
 * @return ostream object containing stream of account details
 */
ostream& operator<<(ostream& sout, const Account& acct) {
    sout << "Account name: " << acct.getUsername() << 
            "\n\tDiscriminator: " << acct.getDiscriminator() <<
            "\n\tNitro: " << acct.hasNitro() << 
            "\n\tBadge: " << acct.getBadge() << 
            "\n\tStatus: " << acct.getStatus();
    return sout;
}
