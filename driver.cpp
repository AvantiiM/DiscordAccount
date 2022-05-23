#include "dtree.h"
#include "utree.h"
#include "mytest.h"


int main() {
    Mytest tester;

    UTree utree;

    cout << "-------Testing UTree insertion..." << endl;
    if(tester.testBasicUTreeInsert(utree)) {
        cout << "test passed" << endl;
    } else {
        cout << "test failed" << endl;
    }
    utree.dump();
    cout << endl;
    cout << "-------Testing UTree removal..." << endl;
    if(tester.testBasicUTreeRemove(utree)) {
        cout << "test passed" << endl;
    } else {
        cout << "test failed" << endl;
    }

    utree.dump();
    cout << endl;
    return 0;
}
