/* godouken.h */

#ifndef GODOUKEN_H
#define GODOUKEN_H

#include "core/object.h"

class Godouken : public Object {
	GDCLASS(Godouken, Object);

protected:
	static void _bind_methods();

public:
	Godouken();
	~Godouken();
};

#endif // GODOUKEN_H
