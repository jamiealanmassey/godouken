/* godouken_data_model.cpp */

#include "godouken_data_model.h"

void GodoukenDataModel::node_index_refresh(GodoukenData *p_node) {}
void GodoukenDataModel::node_index_insert(GodoukenData *p_node) {}

void GodoukenDataModel::script_parse(const String &p_script_file) {}

void GodoukenDataModel::model_serialise(const String &p_file_name, const String &p_file_location) {}
void GodoukenDataModel::model_deserialise(const String &p_file_name, const String &p_file_location) {}

GodoukenDataNodeBase *GodoukenDataModel::get_node_root() { return nullptr; }
GodoukenDataNodeBase *GodoukenDataModel::get_node_from_indexer(const String &p_entry_name) { return nullptr; }

GodoukenDataModel::GodoukenDataModel() {}
GodoukenDataModel::~GodoukenDataModel() {}
