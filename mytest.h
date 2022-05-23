#pragma once

#include "dtree.h"
#include "utree.h"

class Mytest {
    friend class Grader;
    friend class Tester;
    friend class driver;
public:
    bool testBasicUTreeInsert(UTree& utree);
    bool testBasicUTreeRemove(UTree& utree);
    bool testRetrieval(UTree& utree);
};
