#include "godouken_test.h"
#include "gtest/gtest.h"

#include "core/script_language.h"
#include "core/object.h"
#include "core/map.h"
#include "core/vector.h"
#include "core/array.h"
#include "core/list.h"
#include "core/ustring.h"
#include "core/dictionary.h"
#include "core/ustring.h"
#include "core/string_builder.h"
#include "core/os/file_access.h"
#include <core/os/dir_access.h>

#include "../third_party/json.hpp"
#include "../godouken_script_translator.h"

static const String &property_script_1 =
		"##! @name    : Humanoid\n"
		"##! @created : 11/04/2019\n"
		"##! @version : 1.0\n"
		"##! @author  : Jamie Massey\n"
		"##! @brief   : Defines a generic inheritable script that can be utilised by organisms in the game\n"
		"##!          : providing a simplistic health-based system and movement via Godot physics\n"
		"#\n"
		"\n"
		"extends KinematicBody2D\n"
		"\n"
		"##! @brief : Defines health\n"
		"##! @detailed : Defines health that can be modified to destroy entity\n"
		"#\n\n"
		"signal basic_signal\n"
		"\n"
		"var health = 100\n"
		"export(int) var health_max = 100\n"
		"export(float) var movement_speed = 350\n"
		"\n"
		"var velocity : Vector2\n"
		"var is_dead  : bool\n"
		"\n"
		"##! @brief: This is a basic function\n"
		"##! @detailed: This is a basic function's detailed description\n"
		"#\n"
		"func basic_function(param1, param2 : int):\n"
		"\tpass\n"
		"";

GodoukenTest *GodoukenTest::singleton = nullptr;

TEST(TRANSLATOR_TESTS, TEST_EVALUATION_METADATA) {

}

TEST(TRANSLATOR_TESTS, TEST_EVALUATION_PROPERTIES) {
	nlohmann::json script_result;
	GodoukenTest::get_singleton()->script_translator->evaluate(script_result, "property_test_script1.gd", "res://godouken/test/scripts/");

	ASSERT_TRUE(script_result.contains("script"));
	ASSERT_TRUE(script_result["script"].contains("properties"));
	ASSERT_TRUE(script_result["script"]["properties"].size() == 5);

	ASSERT_EQ(script_result["script"]["properties"][0]["name"], "health");
	ASSERT_EQ(script_result["script"]["properties"][0]["description"]["brief"], "Defines health");
	ASSERT_EQ(script_result["script"]["properties"][0]["description"]["detailed"], "Defines health that can be modified to destroy entity");
	ASSERT_EQ(script_result["script"]["properties"][0]["tags"]["is_godot"], false);
	ASSERT_EQ(script_result["script"]["properties"][0]["tags"]["is_exported"], false);
	ASSERT_EQ(script_result["script"]["properties"][0]["type_info"]["name"], "nil");
	ASSERT_EQ(script_result["script"]["properties"][0]["type_info"]["href"], "");

	ASSERT_EQ(script_result["script"]["properties"][1]["name"], "health_max");
	ASSERT_EQ(script_result["script"]["properties"][1]["description"]["brief"], "");
	ASSERT_EQ(script_result["script"]["properties"][1]["description"]["detailed"], "");
	ASSERT_EQ(script_result["script"]["properties"][1]["tags"]["is_godot"], true);
	ASSERT_EQ(script_result["script"]["properties"][1]["tags"]["is_exported"], true);
	ASSERT_EQ(script_result["script"]["properties"][1]["type_info"]["name"], "int");
	ASSERT_EQ(script_result["script"]["properties"][1]["type_info"]["href"], "https://docs.godotengine.org/en/stable/classes/class_int.html");

	ASSERT_EQ(script_result["script"]["properties"][2]["name"], "movement_speed");
	ASSERT_EQ(script_result["script"]["properties"][2]["description"]["brief"], "");
	ASSERT_EQ(script_result["script"]["properties"][2]["description"]["detailed"], "");
	ASSERT_EQ(script_result["script"]["properties"][2]["tags"]["is_godot"], true);
	ASSERT_EQ(script_result["script"]["properties"][2]["tags"]["is_exported"], true);
	ASSERT_EQ(script_result["script"]["properties"][2]["type_info"]["name"], "float");
	ASSERT_EQ(script_result["script"]["properties"][2]["type_info"]["href"], "https://docs.godotengine.org/en/stable/classes/class_float.html");

	ASSERT_EQ(script_result["script"]["properties"][3]["name"], "velocity");
	ASSERT_EQ(script_result["script"]["properties"][3]["description"]["brief"], "");
	ASSERT_EQ(script_result["script"]["properties"][3]["description"]["detailed"], "");
	ASSERT_EQ(script_result["script"]["properties"][3]["tags"]["is_godot"], true);
	ASSERT_EQ(script_result["script"]["properties"][3]["tags"]["is_exported"], false);
	ASSERT_EQ(script_result["script"]["properties"][3]["type_info"]["name"], "vector2");
	ASSERT_EQ(script_result["script"]["properties"][3]["type_info"]["href"], "https://docs.godotengine.org/en/stable/classes/class_vector2.html");

	ASSERT_EQ(script_result["script"]["properties"][4]["name"], "is_dead");
	ASSERT_EQ(script_result["script"]["properties"][4]["description"]["brief"], "");
	ASSERT_EQ(script_result["script"]["properties"][4]["description"]["detailed"], "");
	ASSERT_EQ(script_result["script"]["properties"][4]["tags"]["is_godot"], true);
	ASSERT_EQ(script_result["script"]["properties"][4]["tags"]["is_exported"], false);
	ASSERT_EQ(script_result["script"]["properties"][4]["type_info"]["name"], "bool");
	ASSERT_EQ(script_result["script"]["properties"][4]["type_info"]["href"], "https://docs.godotengine.org/en/stable/classes/class_bool.html");
}

TEST(TRANSLATOR_TESTS, TEST_EVALUATION_METHODS) {
	nlohmann::json script_result;
	GodoukenTest::get_singleton()->script_translator->evaluate(script_result, "property_test_script1.gd", "res://godouken/test/scripts/");

	ASSERT_TRUE(script_result.contains("script"));
	ASSERT_TRUE(script_result["script"].contains("methods"));
	ASSERT_TRUE(script_result["script"]["methods"].size() == 1);
	
	ASSERT_EQ(script_result["script"]["methods"][0]["name"], "basic_function");
	ASSERT_EQ(script_result["script"]["methods"][0]["description"]["brief"], "This is a basic function");
	ASSERT_EQ(script_result["script"]["methods"][0]["description"]["detailed"], "This is a basic function's detailed description");
	ASSERT_EQ(script_result["script"]["methods"][0]["tags"]["is_godot"], false);
	ASSERT_EQ(script_result["script"]["methods"][0]["tags"]["is_internal"], false);
	ASSERT_EQ(script_result["script"]["methods"][0]["tags"]["is_overrider"], false);
	ASSERT_EQ(script_result["script"]["methods"][0]["tags"]["cl_overrider"], false);
	ASSERT_EQ(script_result["script"]["methods"][0]["type_info"]["name"], "");
	ASSERT_EQ(script_result["script"]["methods"][0]["type_info"]["href"], "");

	ASSERT_TRUE(script_result["script"]["methods"][0].contains("parameters"));
	ASSERT_TRUE(script_result["script"]["methods"][0]["parameters"].size() == 2);
	
	ASSERT_EQ(script_result["script"]["methods"][0]["parameters"][0]["name"], "param1");
	ASSERT_EQ(script_result["script"]["methods"][0]["parameters"][0]["type_info"]["name"], "nil");
	ASSERT_EQ(script_result["script"]["methods"][0]["parameters"][0]["type_info"]["href"], "");

	ASSERT_EQ(script_result["script"]["methods"][0]["parameters"][0]["name"], "param2");
	ASSERT_EQ(script_result["script"]["methods"][0]["parameters"][0]["type_info"]["name"], "int");
	ASSERT_EQ(script_result["script"]["methods"][0]["parameters"][0]["type_info"]["href"], "https://docs.godotengine.org/en/stable/classes/class_int.html");
}

TEST(TRANSLATOR_TESTS, TEST_EVALUATION_SIGNALS) {
	nlohmann::json script_result;
	GodoukenTest::get_singleton()->script_translator->evaluate(script_result, "property_test_script1.gd", "res://godouken/test/scripts/");

	ASSERT_TRUE(script_result.contains("script"));
	ASSERT_TRUE(script_result["script"].contains("signals"));
	ASSERT_TRUE(script_result["script"]["signals"].size() == 1);
	
	ASSERT_EQ(script_result["script"]["signals"][0]["name"], "basic_signal");
}

void GodoukenTest::_bind_methods() {

}

int32_t GodoukenTest::execute_tests() {
	const String &dir_test_scripts = "res://godouken/test/scripts/";
	DirAccess *dir = DirAccess::create(DirAccess::AccessType::ACCESS_RESOURCES);
	if (!dir->dir_exists(dir_test_scripts)) {
		dir->make_dir_recursive(dir_test_scripts);
	}

	FileAccess *file = FileAccess::create(FileAccess::AccessType::ACCESS_RESOURCES);
	file->reopen(dir_test_scripts + "property_test_script1.gd", FileAccess::WRITE);
	file->store_string(property_script_1);
	file->close();
	memdelete(file);

	::testing::InitGoogleTest();
	::testing::GTEST_FLAG(output) = "xml:/google_tests/result.xml";
	return RUN_ALL_TESTS();
}

GodoukenTest *GodoukenTest::get_singleton() {
	if (!singleton) {
		singleton = memnew(GodoukenTest);
	}

	return singleton;
}

GodoukenTest::GodoukenTest() {
	script_translator = memnew(GodoukenTranslatorV2);
}

GodoukenTest::~GodoukenTest() {
	memdelete(script_translator);
}
