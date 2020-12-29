/* godouken_script_translator.h */

#include "godouken_data_types.h"

#include "core/object.h"
#include "core/script_language.h"

#include "modules/gdscript/gdscript.h"

#ifndef GODOUKEN_SCRIPT_TRANSLATOR_H
#define GODOUKEN_SCRIPT_TRANSLATOR_H

class GodoukenScriptTranslator : public Object {
    GDCLASS(GodoukenScriptTranslator, Object);

protected:
    GDScript* translator_script;
    int32_t translator_script_line_begin;
    int32_t translator_script_line_end;

    List<String> reserved_godot_methods;
    List<String> reserved_godot_types;

public:
    class GodoukenDataModel *script_translate();

public:
    GodoukenScriptTranslator();
    ~GodoukenScriptTranslator();
};


#endif // GODOUKEN_SCRIPT_TRANSLATOR_H
