/* godouken_data_model.cpp */

#include "editor/editor_node.h"
#include "editor/editor_scale.h"
#include "core/os/file_access.h"
#include "core/class_db.h"

#include "third_party/inja.hpp"

#include "godouken_data_model.h"

#include <cwctype>

#include "godouken_script_translator.h"

#include "stencils/godouken_stencil_class_sidebar.h"
#include "stencils/godouken_stencil_class.h"
#include "stencils/godouken_stencil_directory.h"
#include "stencils/godouken_stencil_style.h"

Vector<String> extract_breadcrumbs(const String &p_directory_path) {
	return p_directory_path.replace_first("res://", "").split("/");
}

const String &extract_breadcrumb_html(const String &p_directory_path) {
	return p_directory_path.replace_first("res://", "").replace("/", "_") + ".html";
}

void extract_breadcrumbs(const String &p_directory_path, nlohmann::json &p_breadcrumbs_json) {
	p_breadcrumbs_json["breadcrumbs"] = nlohmann::json::array();
	const Vector<String> breadcrumbs = extract_breadcrumbs((p_directory_path));
	for (uint32_t i = 0; i < breadcrumbs.size(); i++) {
		StringBuilder breadcrumb_url = StringBuilder();
		for (uint32_t x = 0; x < i; x--) {
			breadcrumb_url.append(breadcrumbs[x] + "_");
		}

		breadcrumb_url.append(breadcrumbs[i]);
		breadcrumb_url.append(".html");
		
		nlohmann::json breadcrumb_info = nlohmann::json::object();
		breadcrumb_info["name"] = breadcrumbs[i].utf8();
		breadcrumb_info["url"] = breadcrumb_url.as_string().utf8();
		p_breadcrumbs_json["breadcrumbs"].push_back(breadcrumb_info);
	}
}

void GodoukenDataModel::data() {
	Vector<String> directories;
	Vector<GodoukenDataEntry> directories_scripts;
	//ModelTreeIndexer directory_tree;
	nlohmann::json directory_json = nlohmann::json::object();

	GodoukenDirEntry *root_dir = new GodoukenDirEntry();
	root_dir->dir_name = ".";

	// COLLATE DIRECTORIES, FILES AND DIRECTORY TREE
	directories.push_back("res://");
	DirAccess *dir = DirAccess::create(DirAccess::AccessType::ACCESS_RESOURCES);
	while (!directories.empty()) {
		dir->change_dir(directories[0]);
		dir->list_dir_begin();
		directories.remove(0);
		
		String dir_filename = "";
		while (!(dir_filename = dir->get_next()).empty()) {
			if (dir_filename[0] != '.') {
				const String &str_dir = dir->get_current_dir_without_drive();
				const String &str_path = str_dir + "/" + dir_filename;
				if (dir->current_is_dir()) {
					directories.push_back(str_path);
					tree_indexer(root_dir, extract_breadcrumbs(str_path));
				}
				/*else {
					if (dir_filename.find(".gd") >= 0) {
						GodoukenDataEntry *directory_script = new GodoukenDataEntry();
						directory_script->data_directory = dir->get_current_dir_without_drive() + "/";
						directory_script->data_file = dir_filename;
						directory_script->data_name = dir_filename.substr(0, dir_filename.find_char('.'));
						directories_scripts.push_back(*directory_script);
						tree_indexer(root_dir, extract_breadcrumbs(str_dir), directory_script);
					}
				}*/
			}
		}
	}

	directories.push_back("res://");
	while (!directories.empty()) {
		dir->change_dir(directories[0]);
		dir->list_dir_begin();
		directories.remove(0);

		String dir_filename = "";
		while (!(dir_filename = dir->get_next()).empty()) {
			if (dir_filename[0] != '.') {
				const String &str_dir = dir->get_current_dir_without_drive();
				const String &str_path = str_dir + "/" + dir_filename;
				if (dir->current_is_dir()) {
					directories.push_back(str_path);
				}
				else {
					if (dir_filename.find(".gd") >= 0) {
						GodoukenDataEntry *directory_script = new GodoukenDataEntry();
						directory_script->data_directory = dir->get_current_dir_without_drive() + "/";
						directory_script->data_file = dir_filename;
						directory_script->data_name = dir_filename.substr(0, dir_filename.find_char('.'));
						directories_scripts.push_back(*directory_script);
						tree_indexer(root_dir, extract_breadcrumbs(str_dir), directory_script);
					}
				}
			}
		}
	}

	// CREATE DIRECTORY LISTINGS
	Vector<nlohmann::json> directories_json;
	Vector<GodoukenDirEntry *> dir_evaluation;
	dir_evaluation.push_back(root_dir);
	while (dir_evaluation.size() > 0) {
		GodoukenDirEntry *directory_info = dir_evaluation[0];
		nlohmann::json directory_json = nlohmann::json::object();
		directory_json["entries"]["dirs"] = nlohmann::json::array();
		directory_json["entries"]["abc"] = nlohmann::json::array();
		directory_json["breadcrumbs"] = nlohmann::json::array(); // tree_breadcrumbs(directory_info);
		directory_json["url"] = tree_breadcrumb_html(directory_info).utf8();
		for (uint32_t i = 0; i < directory_info->dir_children.size(); i++) {
			const GodoukenDirEntry *dir_child = directory_info->dir_children[i];
			nlohmann::json directory_child = nlohmann::json::object();
			directory_child["name"] = dir_child->dir_name.utf8();
			directory_child["url"] = tree_breadcrumb_html(dir_child).utf8();
			directory_json["entries"]["dirs"].push_back(directory_child);
		}

		Map<String, nlohmann::json> directory_alpha_jsons;
		for (uint32_t i = 0; i < directory_info->dir_files.size(); i++) {
			GodoukenDataEntry *dir_data_entry = directory_info->dir_files[i];
			String alpha = dir_data_entry->data_name.substr(0, 1);
			if (!directory_alpha_jsons.has(alpha)) {
				directory_alpha_jsons.insert(alpha, nlohmann::json::array());
			}
			
			nlohmann::json directory_child = nlohmann::json::object();
			directory_child["name_script"] = dir_data_entry->data_file.utf8();
			directory_child["name"] = dir_data_entry->data_name.replace("_", " ").capitalize().utf8();
			directory_child["path"] = dir_data_entry->data_directory.utf8();
			directory_alpha_jsons[alpha].push_back(directory_child);
		}

		for (Map<String, nlohmann::json>::Element *E = directory_alpha_jsons.front(); E; E = E->next()) {
			nlohmann::json dir_file_entry = nlohmann::json::object();
			dir_file_entry["alpha"] = E->key().to_upper().utf8();
			dir_file_entry["files"] = E->value();
			directory_json["entries"]["abc"].push_back(dir_file_entry);
		}

		for (uint32_t i = 0; i < directory_info->dir_children.size(); i++) {
			dir_evaluation.push_back(directory_info->dir_children[i]);
		}

		GodoukenDirEntry *node_current = directory_info;
		List<nlohmann::json> breadcrumbs_json;
		do {
			nlohmann::json breadcrumb = nlohmann::json::object();
			breadcrumb["name"] = node_current->dir_name.utf8();
			breadcrumb["url"] = tree_breadcrumb_html(node_current).utf8();
			breadcrumbs_json.push_back(breadcrumb);
			node_current = node_current->dir_parent;
		} while (node_current);

		breadcrumbs_json.invert();
		for (uint32_t i = 0; i < breadcrumbs_json.size(); i++) {
			directory_json["breadcrumbs"].push_back(breadcrumbs_json[i]);
		}
		
		directories_json.push_back(directory_json);
		dir_evaluation.remove(0);
	}

	for (uint32_t i = 0; i < directories_json.size(); i++) {
		//directories_json[i]["data"]["project"]["title"] = ProjectSettings::get_singleton()->get_setting("application/config/name").operator String().utf8();
		
		inja::Environment env;
		const std::string result = env.render(godouken_stencil_directory, directories_json[i]);
		const std::string name = directories_json[i]["url"].get<std::string>();
		const String &dir_html_dir = "res://docs/html/dirs";
		dir->change_dir(dir_html_dir);
		if (!dir->dir_exists(dir_html_dir)) {
			dir->make_dir_recursive(dir_html_dir);
		}

		FileAccess *file = FileAccess::create(FileAccess::AccessType::ACCESS_RESOURCES);
		file->reopen(dir_html_dir + "/" + name.c_str(), FileAccess::WRITE);
		file->store_string(result.c_str());
		file->close();
		memdelete(file);
	}

	// EVALUATE GD SCRIPT FILES AND EXTRACT JSON
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

void GodoukenDataModel::tree_indexer(GodoukenDirEntry *p_node, Vector<String> p_breadcrumbs) {
	GodoukenDirEntry *dir_entry = p_node;
	while (p_breadcrumbs.size() > 0) {
		if (p_breadcrumbs[0] != "") {
			GodoukenDirEntry *dir_found_entry = dir_entry;
			bool dir_is_found = false;
			for (uint32_t i = 0; i < dir_entry->dir_children.size(); i++) {
				if (p_breadcrumbs[0].find(dir_entry->dir_children[i]->dir_name) >= 0) {
					dir_is_found = true;
					dir_found_entry = dir_entry->dir_children[i];
					break;
				}
			}

			if (!dir_is_found) {
				GodoukenDirEntry *dir_child = new GodoukenDirEntry();
				dir_child->dir_name = p_breadcrumbs[0];
				dir_child->dir_parent = dir_entry;
				dir_entry->dir_children.push_back(dir_child);
				dir_entry = dir_child;
			}
			else {
				dir_entry = dir_found_entry;
			}
		}
		
		p_breadcrumbs.remove(0);
	}
}

void GodoukenDataModel::tree_indexer(GodoukenDirEntry *p_node, Vector<String> p_breadcrumbs, GodoukenDataEntry *p_data_entry) {
	bool success;
	tree_indexer(p_node, p_breadcrumbs, p_data_entry, success);
}

void GodoukenDataModel::tree_indexer(GodoukenDirEntry *p_node, Vector<String> p_breadcrumbs, GodoukenDataEntry *p_data_entry, bool &p_success) {
	p_success = false;
	GodoukenDirEntry *dir_entry = p_node;
	while (p_breadcrumbs.size() > 0) {
		GodoukenDirEntry *dir_found_entry = dir_entry;
		bool dir_is_found = false;
		for (uint32_t i = 0; i < dir_entry->dir_children.size(); i++) {
			if (p_breadcrumbs[0].find(dir_entry->dir_children[i]->dir_name) >= 0) {
				dir_is_found = true;
				dir_found_entry = dir_entry->dir_children[i];
				break;
			}
		}

		if (dir_is_found) {
			dir_entry = dir_found_entry;
		}

		p_breadcrumbs.remove(0);
		if (p_breadcrumbs.size() == 0) {
			dir_entry->dir_files.push_back(p_data_entry);
			p_success = true;
		}
	}
}

Vector<String> GodoukenDataModel::tree_breadcrumbs(GodoukenDirEntry *p_node) {
	Vector<String> breadcrumbs;
	GodoukenDirEntry *node_current = p_node;
	breadcrumbs.push_back(node_current->dir_name);
	while (node_current->dir_parent) {
		node_current = node_current->dir_parent;
		breadcrumbs.push_back(node_current->dir_name);
	}

	return breadcrumbs;
}

String GodoukenDataModel::tree_breadcrumb_html(const GodoukenDirEntry *p_node)
{
	String result = "";
	if (p_node && p_node->dir_parent) {
		const GodoukenDirEntry *node_current = p_node;
		String breadcrumb = ".html";
		String breadcrumb_end = "";
		bool breadcrumb_first = true;
		//breadcrumb = node_current->dir_name + breadcrumb;
		do {
			if (node_current->dir_name != ".") {
				breadcrumb_end = breadcrumb_first ? "" : "_";
				breadcrumb = node_current->dir_name + breadcrumb_end + breadcrumb;
				breadcrumb_first = false;
			}

			node_current = node_current->dir_parent;
		} while (node_current->dir_parent);

		result = breadcrumb;
	}
	
	return result;
}

String GodoukenDataModel::tree_breadcrumb_html(const Vector<String> &p_breadcrumbs) {
	StringBuilder breadcrumb_html = StringBuilder();
	String breadcrumb_end = "";
	for (uint32_t i = 1; i < p_breadcrumbs.size(); i++) {
		breadcrumb_end = i != p_breadcrumbs.size() - 1 ? "_" : "";
		breadcrumb_html.append(p_breadcrumbs[i] + breadcrumb_end);
	}

	breadcrumb_html.append(".html");
	return breadcrumb_html.as_string();
}

GodoukenDataModel::GodoukenDataModel() {}
GodoukenDataModel::~GodoukenDataModel() {}
