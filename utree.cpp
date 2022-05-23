/**
 * Project 2 - Binary Trees
 * UserTree.h
 * Implementation for the UTree class.
 */

#include "utree.h"
#include <iostream>
#include <string>
#include <exception>
/**
 * Destructor, deletes all dynamic memory.
 */
UTree::~UTree() {
	clear(_root);
	_root = nullptr;
}

/**
 * Sources a .csv file to populate Account objects and insert them into the UTree.
 * @param infile path to .csv file containing database of accounts
 * @param append true to append to an existing tree structure or false to clear before importing
 */
void UTree::loadData(string infile, bool append) {
	std::ifstream instream(infile);
	string line;
	char delim = ',';
	const int numFields = 5;
	string fields[numFields];

	/* Check to make sure the file was opened */
	if (!instream.is_open()) {
		std::cerr << __FUNCTION__ << ": File " << infile
				<< " could not be opened or located" << endl;
		exit(-1);
	}

	/* Should we append or clear? */
	if (!append)
		this->clear();

	/* Read in the data from the .csv file and insert into the UTree */
	while (std::getline(instream, line)) {
		std::stringstream buffer(line);

		/* Quick check to make sure each line is formatted correctly */
		int delimCount = 0;
		for (unsigned int c = 0; c < buffer.str().length(); c++)
			if (buffer.str()[c] == delim)
				delimCount++;
		if (delimCount != numFields - 1) {
//			throw std::invalid_argument(
//					"Malformed input file detected - ensure each line contains 5 fields deliminated by a ','");
		}

		/* Populate the account attributes -
		 * Each line always has 5 sections of data */
		for (int i = 0; i < numFields; i++) {
			std::getline(buffer, line, delim);
			fields[i] = line;
		}
		Account newAcct = Account(fields[0], std::stoi(fields[1]),
				std::stoi(fields[2]), fields[3], fields[4]);
		this->insert(newAcct);
	}
}

/**
 * Dynamically allocates a new UNode in the tree and passes insertion into DTree. 
 * Should also update heights and detect imbalances in the traversal path after
 * an insertion.
 * @param newAcct Account object to be inserted into the corresponding DTree
 * @return true if the account was inserted, false otherwise
 */
//To insert nodes into the UTree
bool UTree::insert(Account newAcct) {

	if(insert(newAcct, _root) != nullptr) return true;
	else return false;
}

//Helper function for insert function
UNode* UTree::insert(Account newAcct, UNode*& node) {
	if (node == nullptr) {
		node = new UNode();
		if (node->getDTree())
			node->getDTree()->insert(newAcct);
		return node;
	} else {
		UNode *temp = retrieve(newAcct.getUsername());
		if (temp != nullptr) {
			if (temp->getDTree() == nullptr) {
				temp->_dtree = new DTree();
			}
			temp->getDTree()->insert(newAcct);
			return temp;
		}
	}
	/* Now, if we got here, it's not a leaf, YET, so start looking */
	/* But notice AS we are moving left and right we are then going to check if balanced */
	/* using the REST of the code below, EVEN if there is a duplicate!! */
	/* notice no "return" from here out, so it ALWAYS updatesSize and NumVacant and checks */
	/* imbalanace */
	if (newAcct.getUsername() < node->getUsername()) {
		node->_left = insert(newAcct, node->_left);
	} else if (newAcct.getUsername() > node->getUsername()) {
		node->_right = insert(newAcct, node->_right);
	} else {
		return nullptr;
	}

	updateHeight(node);
	if (checkImbalance(node) > 1 || checkImbalance(node) < -1) {
		rebalance(node);
	}

	return node;

}

// Find the minimum value node on the left branch
UNode* UTree::minValueNode(UNode *node) {
	if (node == nullptr)
		return nullptr;
	else if (node->_left == nullptr)
		return node;
	else
		return minValueNode(node->_left);
}

/**
 * Removes a user with a matching username and discriminator.
 * @param username username to match
 * @param disc discriminator to match
 * @param removed DNode object to hold removed account
 * @return true if an account was removed, false otherwise
 */
//Method to remove users from the utree
bool UTree::removeUser(string username, int disc, DNode *&removed) {
	UNode *node = retrieve(username);
	if (node == nullptr) {
		return false;
	} else {
		node->getDTree()->remove(disc, removed);
		if (node->getDTree()->getNumUsers() == 0) {
			//node->getDTree()->clear();
			node = removeUser(username, _root);
		}
	}
	if (removed == nullptr)
		return false;
	else
		return true;
}

//Helper function for removeUser
UNode* UTree::removeUser(string username, UNode *node) {
	UNode *temp;
	if (node == nullptr) {
		return node;
	} else if (username < node->getUsername()) {
		node->_left = removeUser(username, node->_left);
	} else if (username > node->getUsername()) {
		node->_right = removeUser(username, node->_right);
	} else if (node->_right && node->_left) {
		temp = minValueNode(node->_right);
		node->_dtree = temp->_dtree;
		node->_right = removeUser(node->getUsername(), temp->_right);
	} else {
		temp = node;
		if (node->_left == nullptr)
			node = node->_right;
		else if (node->_right == nullptr)
			node = node->_left;
		delete temp;
	}
	if (node == nullptr) {
		return node;
	}
	updateHeight(node);
	int diff = checkImbalance(node);
	if (diff > 1 && checkImbalance(node->_left) >= 0) {
		node = rotateRight(node);
	} else if (diff > 1 && checkImbalance(node->_left) < 0) {
		node->_left = rotateLeft(node->_left);
		node = rotateRight(node);
	} else if (diff < -1 && checkImbalance(node->_right) <= 0) {
		node = rotateLeft(node);
	} else if (diff < -1 && checkImbalance(node->_right) > 0) {
		node->_right = rotateRight(node->_right);
		node = rotateLeft(node);
	}

	return node;
}

/**
 * Retrieves a set of users within a UNode.
 * @param username username to match
 * @return UNode with a matching username, nullptr otherwise
 */
UNode* UTree::retrieve(string username) {
	return retrieve(username, _root);

}

//Helper function for UTree method retrieve
UNode* UTree::retrieve(string username, UNode *node) {
	if (node == nullptr) {
		return nullptr;
	} else if (username == node->getUsername()) {
		return node;
	} else if (username < node->getUsername()) {
		return retrieve(username, node->_left);
	} else if (username > node->getUsername()) {
		return retrieve(username, node->_right);
	} else {
		return retrieve(username, node);
	}

}

/**
 * Retrieves the specified Account within a DNode.
 * @param username username to match
 * @param disc discriminator to match
 * @return DNode with a matching username and discriminator, nullptr otherwise
 */
DNode* UTree::retrieveUser(string username, int disc) {
	UNode *unode = retrieve(username);
	return unode->getDTree()->retrieve(disc);
}

/**
 * Returns the number of users with a specific username.
 * @param username username to match
 * @return number of users with the specified username
 */
int UTree::numUsers(string username) {
	UNode *node = retrieve(username);
	return node->getDTree()->getNumUsers();
}

/**
 * Helper for the destructor to clear dynamic memory.
 */
void UTree::clear() {
	clear(_root);
}

//clears the UTree
void UTree::clear(UNode *node) {
	if (node == nullptr)
		return;
	clear(node->_left);
	clear(node->_right);
	delete node;
}

/**
 * Prints all accounts' details within every DTree.
 */
void UTree::printUsers() const {
	printUsers(_root);
}
//Prints  users in the UTree
void UTree::printUsers(UNode *node) const {
	if (node == nullptr)
		return;
	printUsers(node->_left);
	node->getDTree()->printAccounts();
	printUsers(node->_right);

}

/**
 * Dumps the UTree in the '()' notation.
 */
void UTree::dump(UNode *node) const {
	if (node == nullptr)
		return;
	cout << "(";
	dump(node->_left);
	cout << node->getUsername() << ":" << node->getHeight() << ":"
			<< node->getDTree()->getNumUsers();
	dump(node->_right);
	cout << ")";
}

/**
 * Updates the height of the specified node.
 * @param node UNode object in which the height will be updated
 */
void UTree::updateHeight(UNode *node) {
	node->_height = 1 + max(getHeight(node->_left), getHeight(node->_right));
}

//Returns the height of the node
int UTree::getHeight(UNode *node) {
	if (node == nullptr) {
		return -1;
	} else {
		return node->getHeight();
	}
}

//get max of two integers
int UTree::max(int a, int b) {
	return (a > b) ? a : b;
}

/**
 * Checks for an imbalance, defined by AVL rules, at the specified node.
 * @param node UNode object to inspect for an imbalance
 * @return (can change) returns true if an imbalance occured, false otherwise
 */
int UTree::checkImbalance(UNode *node) {
	if (node == nullptr) {
		return 0;
	} else { //if(node->_left != nullptr && node->_right != nullptr) {
		int r = 0;
		int l = 0;
		if (node->_left == nullptr)
			l = 0;
		else {
			l = max(1, getHeight(node->_left));
		}
		if (node->_right == nullptr)
			l = 0;
		else {
			r = max(1, getHeight(node->_right));
		}
		return l - r;
	}
}

//----------------
/**
 * Begins and manages the rebalance procedure for an AVL tree (pass by reference).
 * @param node UNode object where an imbalance occurred
 */
void UTree::rebalance(UNode *&node) {
	int diff = checkImbalance(node);
	if (diff > 1) {
		if (checkImbalance(node->_left) > 0) {
			node = rotateLeft(node);
		} else {
			node = rotateLR(node);
		}
	} else if (diff < -1) {
		if (checkImbalance(node->_right) > 0) {
			node = rotateRL(node);
		} else {
			node = rotateRight(node);
		}
	}
}

//Rotates the nodes left then right
UNode* UTree::rotateLR(UNode *p) {
	UNode *t = p->_left;
	p->_left = rotateRight(t);
	return rotateLeft(p);

}
//Rotates the nodes right then left
UNode* UTree::rotateRL(UNode *p) {
	UNode *t = p->_right;
	p->_right = rotateLeft(t);
	return rotateRight(p);

}

//Rotates the nodes one time to the right
UNode* UTree::rotateRight(UNode *y) {
	UNode *t = y->_right;
	y->_right = t->_left;
	t->_left = y;
	t->_height = max(getHeight(t->_left), getHeight(t->_right)) + 1;
	y->_height = max(getHeight(y->_left), getHeight(y->_right)) + 1;
	return t;
}

//Rotates the nodes one time to the left
UNode* UTree::rotateLeft(UNode *x) {
	UNode *t = x->_left;
	x->_left = t->_right;
	t->_right = x;
	t->_height = max(getHeight(t->_left), getHeight(t->_right)) + 1;
	x->_height = max(getHeight(x->_left), getHeight(x->_right)) + 1;
	return t;

}

/**
 * Begins and manages the rebalance procedure for an AVL tree (returns a pointer).
 * @param node UNode object where an imbalance occurred
 * @return UNode object replacing the unbalanced node's position in the tree
 */
//UNode* UTree::rebalance(UNode* node) {
//}
//----------------
