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
		"#\n"
		"export(int) var health = 100\n"
		"export(int) var health_max = 100\n"
		"export(float) var movement_speed = 350\n"
		"\n"
		"var velocity : Vector2\n"
		"var is_dead  : bool\n"
		"";

GodoukenTest *GodoukenTest::singleton = nullptr;

TEST(TRANSLATOR_TESTS, TEST_EVALUATION_PROPERTY1) {
	nlohmann::json &script_result = GodoukenTest::get_singleton()->script_translator->evaluate("property_test_script1.gd", "res://godouken/test/scripts/");

	// ASSERT_TRUE(script_result.contains("script"));
	// ASSERT_TRUE(script_result["script"].contains("properties"));
	// ASSERT_TRUE(script_result["script"]["properties"].size() > 0);

	ASSERT_EQ(script_result["script"]["properties"][0]["name"], "health");
	ASSERT_EQ(script_result["script"]["properties"][0]["description"]["brief"], "stores the humanoid health");
	ASSERT_EQ(script_result["script"]["properties"][0]["description"]["detailed"], "stores the humanoid health");
	ASSERT_EQ(script_result["script"]["properties"][0]["tags"]["is_godot"], true);
	ASSERT_EQ(script_result["script"]["properties"][0]["tags"]["is_exported"], true);
	ASSERT_EQ(script_result["script"]["properties"][0]["type_info"]["name"], "int");
	ASSERT_EQ(script_result["script"]["properties"][0]["type_info"]["href"], "https://docs.godotengine.org/en/3.2/classes/class_int.html");
}

TEST(TRANSLATOR_TESTS, TEST_EVALUATION_PROPERTY2) {
	// GodoukenTranslatorV2 *script_translator = memnew(GodoukenTranslatorV2);
	// script_translator->evaluate_property();
}

TEST(TRANSLATOR_TESTS, TEST_EVALUATION_METHOD) {

}

TEST(TRANSLATOR_TESTS, TEST_EVALUATION_SIGNAL) {

}

TEST(TRANSLATOR_TESTS, TEST_EVALUATION) {

}

void GodoukenTest::_bind_methods() {

}

int32_t GodoukenTest::execute_tests() {
	const String &dir_test_scripts = "res://godouken/test/scripts/";
	DirAccess *dir = DirAccess::create(DirAccess::AccessType::ACCESS_RESOURCES);
	if (!dir->dir_exists(dir_test_scripts)) {
		dir->make_dir_recursive(dir_test_scripts);
	}

	if (!dir->file_exists(dir_test_scripts + "property_test_script1.gd")) {
		FileAccess *file = FileAccess::create(FileAccess::AccessType::ACCESS_RESOURCES);
		file->reopen(dir_test_scripts + "property_test_script1.gd", FileAccess::WRITE);
		file->store_string(property_script_1);
		file->close();
		memdelete(file);
	}

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
