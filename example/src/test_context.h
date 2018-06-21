//
//  test_context.h
//  example
//
//  Created by yutaAsai on 2018/06/22.
//
//

#ifndef test_context_h
#define test_context_h
#include <iostream>
#define _call(x) std::cout << "test_call " << #x << std::endl

struct test_call{
    void setup(){ _call(setup); }
    void update(){ _call(update); }
    void beforeUpdate(){ _call(beforeUpdate); }
    void afterUpdate(){ _call(afterUpdate); }
    void beforeDraw(){ _call(beforeDraw); }
    void afterDraw(){ _call(afterDraw); }
};
#undef _call

#endif /* test_context_h */
