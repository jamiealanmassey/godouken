/* godouken_data_model.h */

#include "godouken_data_types.h"

#include "core/ustring.h"
#include "core/object.h"
#include "core/map.h"

#ifndef GODOUKEN_DATA_MODEL_H
#define GODOUKEN_DATA_MODEL_H

typedef Map<String, GodoukenData *> GodoukenIndexer;

class GodoukenDataModel : public Object {
    GDCLASS(GodoukenDataModel, Object);

protected:
    uint32_t model_script_hash;
    GodoukenDataNodeBase *model_node_root;
    GodoukenIndexer model_node_indices;

public:
    void node_index_refresh(GodoukenData *p_node);
    void node_index_insert(GodoukenData *p_node);

    void script_parse(const String &p_script_file);

    void model_serialise(const String &p_file_name, const String &p_file_location);
    void model_deserialise(const String &p_file_name, const String &p_file_location);

    GodoukenDataNodeBase *get_node_root();
    GodoukenDataNodeBase *get_node_from_indexer(const String &p_entry_name);

public:
    GodoukenDataModel();
    ~GodoukenDataModel();
};


#endif // GODOUKEN_DATA_MODEL_H
