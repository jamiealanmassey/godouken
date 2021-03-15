/* godouken_test.h */

#ifndef GODOUKEN_TEST_H
#define GODOUKEN_TEST_H

#include "core/object.h"

class GodoukenTest : public Object {
	GDCLASS(GodoukenTest, Object);

private:
	static GodoukenTest *singleton;

protected:
	static void _bind_methods();

public:
	int32_t execute_tests();

public:
	class GodoukenTranslator *script_translator;

	static GodoukenTest *get_singleton();

public:
	GodoukenTest();
	~GodoukenTest();
};

#endif // GODOUKEN_TEST_H
