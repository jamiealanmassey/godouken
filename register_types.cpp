/* register_types.cpp */

#include "register_types.h"

#include "core/class_db.h"

#include "godouken.h"
#include "godouken_data_model.h"
#include "godouken_script_translator.h"
#include "godouken_tests/godouken_test.h"

void register_godouken_types() {
	ClassDB::register_class<Godouken>();
	ClassDB::register_class<GodoukenDataModel>();
	ClassDB::register_class<GodoukenScriptTranslator>();
	ClassDB::register_class<GodoukenTest>();
}

void unregister_godouken_types() {
	// empty
}
