/* godouken_data_model.cpp */

#include "editor/editor_node.h"
#include "editor/editor_scale.h"
#include "core/os/file_access.h"
#include "core/class_db.h"

#include "third_party/inja.hpp"

#include "godouken_data_model.h"
#include "godouken_script_translator.h"

#include "stencils/godouken_stencil_class_sidebar.h"
#include "stencils/godouken_stencil_class.h"
#include "stencils/godouken_stencil_style.h"

Vector<String> GodoukenDataEntry::extract_breadcrumbs(const String &p_directory_path) {
	return p_directory_path.split("//");
}

void GodoukenDataModel::data() {
	Vector<String> directories;
	Vector<GodoukenDataEntry> directories_scripts;
	directories.push_back("res://");
	DirAccess *dir = DirAccess::create(DirAccess::AccessType::ACCESS_RESOURCES);
	while (!directories.empty()) {
		dir->change_dir(directories[0]);
		dir->list_dir_begin();
		directories.remove(0);
		
		String dir_filename = "";
		while (!(dir_filename = dir->get_next()).empty()) {
			if (dir_filename[0] != '.') {
				if (dir->current_is_dir()) {
					directories.push_back(dir->get_current_dir_without_drive() + "/" + dir_filename);
				}
				else {
					if (dir_filename.find(".gd") >= 0) {
						GodoukenDataEntry directory_script;
						directory_script.data_directory = dir->get_current_dir_without_drive() + "/";
						directory_script.data_file = dir_filename;
						directory_script.data_name = dir_filename.substr(0, dir_filename.find_char('.'));
						directories_scripts.push_back(directory_script);
					}
				}
			}
		}
	}

	for (uint32_t i = 0; i < directories_scripts.size(); i++) {
		GodoukenTranslator *godouken_translator = memnew(GodoukenTranslator);
		GodoukenDataEntry directory_script = directories_scripts[i];
		godouken_model.insert(directory_script.data_name, directory_script);
		godouken_translator->evaluate(godouken_model[directory_script.data_name].data_json, directory_script.data_file, directory_script.data_directory);
		memdelete(godouken_translator);
	}

	nlohmann::json sidebar_json = nlohmann::json::object();
	sidebar_json["data"]["scripts"] = nlohmann::json::array();
	
	for (Map<String, GodoukenDataEntry>::Element *E = godouken_model.front(); E; E = E->next()) {
		inja::Environment env;

		GodoukenDataEntry &script_entry = E->value();
		script_entry.data_json["data"]["project"]["title"] = ProjectSettings::get_singleton()->get_setting("application/config/name").operator String().utf8();
		script_entry.data_json["data"]["script"]["breadcrumbs"] = nlohmann::json::array();
		if (script_entry.data_json["data"]["script"]["name_sm"] != "") {
			script_entry.data_json["data"]["script"]["breadcrumbs"].push_back(".");
			Vector<String> breadcrumbs = script_entry.data_directory.replace_first("res://", "").split("/");
			for (uint32_t i = 0; i < breadcrumbs.size(); i++) {
				const String &str_append = i == breadcrumbs.size() - 1 ? script_entry.data_file : "";
				const String &str_breadcrumb = breadcrumbs[i] + str_append;
				script_entry.data_json["data"]["script"]["breadcrumbs"].push_back(str_breadcrumb.utf8());
			}
			
			nlohmann::json sidebar_entry;
			sidebar_entry["name_fl"] = script_entry.data_json["data"]["script"]["name"];
			sidebar_entry["name_sm"] = script_entry.data_json["data"]["script"]["name_sm"];
			sidebar_json["data"]["scripts"].push_back(sidebar_entry);
		}
		
		const std::string result = env.render(godouken_stencil_class, script_entry.data_json);
		const String &dir_html = "res://docs/html/";
		dir->change_dir(dir_html);
		if (!dir->dir_exists(dir_html)) {
			dir->make_dir_recursive(dir_html);
		}

		FileAccess *file = FileAccess::create(FileAccess::AccessType::ACCESS_RESOURCES);
		file->reopen(dir_html + script_entry.data_name.replace(" ", "_") +".html", FileAccess::WRITE);
		file->store_string(result.c_str());
		file->close();
		memdelete(file);
	}

	inja::Environment env_style;
	const std::string style = env_style.render(godouken_stencil_style, nullptr);
	const String &dir_css = "res://docs/css/";
	dir->change_dir(dir_css);
	if (!dir->dir_exists(dir_css)) {
		dir->make_dir_recursive(dir_css);
	}

	FileAccess *file_css = FileAccess::create(FileAccess::AccessType::ACCESS_RESOURCES);
	file_css->reopen(dir_css + "main.css", FileAccess::WRITE);
	file_css->store_string(style.c_str());
	file_css->close();
	memdelete(file_css);
	
	inja::Environment env_sidebar;
	const std::string sidebar = env_sidebar.render(godouken_stencil_class_sidebar, sidebar_json);
	const String &dir_generic = "res://docs/html/generic/";
	dir->change_dir(dir_generic);
	if (!dir->dir_exists(dir_generic)) {
		dir->make_dir_recursive(dir_generic);
	}

	FileAccess *file_sidebar = FileAccess::create(FileAccess::AccessType::ACCESS_RESOURCES);
	file_sidebar->reopen(dir_generic + "sidebar.html", FileAccess::WRITE);
	file_sidebar->store_string(sidebar.c_str());
	file_sidebar->close();
	memdelete(file_sidebar);
	
	memdelete(dir);
}

void GodoukenDataModel::node_index_refresh(GodoukenData *p_node) {}
void GodoukenDataModel::node_index_insert(GodoukenData *p_node) {}

void GodoukenDataModel::script_parse(const String &p_script_file) {}

void GodoukenDataModel::model_serialise(const String &p_file_name, const String &p_file_location) {}
void GodoukenDataModel::model_deserialise(const String &p_file_name, const String &p_file_location) {}

GodoukenDataNodeBase *GodoukenDataModel::get_node_root() { return nullptr; }
GodoukenDataNodeBase *GodoukenDataModel::get_node_from_indexer(const String &p_entry_name) { return nullptr; }

GodoukenDataModel::GodoukenDataModel() {}
GodoukenDataModel::~GodoukenDataModel() {}
