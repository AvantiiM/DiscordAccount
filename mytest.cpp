/*
 * mytest.cpp
 *
 *  Created on: Nov 20, 2021
 *      Author: Avanti
 */

#include "mytest.h"

bool Mytest::testBasicUTreeInsert(UTree& utree) {
	string s[] = {"Timpura", "kapish", "Chutts", "Chubbs", "Nayee", "Pikkaso", "Lemon"};
    bool allInserted = true;
    for(int i = 0; i < 7; i++) {
        Account newAcct = Account(s[i], 1000+i, 0, "", "");
        if(!utree.insert(newAcct)) {
            cout << "Insertion on node " << s[i] << " did not return true" << endl;
            allInserted = false;
            break;
        }
    }
    return allInserted;

}
bool Mytest::testBasicUTreeRemove(UTree& utree) {
    DNode* dnode;
	utree.removeUser("Timpura", 1000, dnode);
	if(dnode->getUsername() == "Timpura") return true;

	return false;

}
bool Mytest::testRetrieval(UTree& utree) {
    DNode* node;
	node = utree.retrieveUser("Chubbs", 1003);
	if(node->getUsername() == "Chubbs") return true;
	return false;
}
