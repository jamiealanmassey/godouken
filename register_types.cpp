/* register_types.cpp */

#include "register_types.h"

#include "core/class_db.h"

#include "godouken.h"

void register_godouken_types() {
	ClassDB::register_class<Godouken>();
}

void unregister_godouken_types() {
	// empty
}
