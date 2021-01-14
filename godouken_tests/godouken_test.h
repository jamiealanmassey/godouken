/* godouken_test.h */

#ifndef GODOUKEN_TEST_H
#define GODOUKEN_TEST_H

#include "core/object.h"

class GodoukenTest : public Object {
	GDCLASS(GodoukenTest, Object);

protected:
	static void _bind_methods();

public:
	int32_t execute_tests();

public:
	GodoukenTest();
	~GodoukenTest();
};

#endif // GODOUKEN_TEST_H
