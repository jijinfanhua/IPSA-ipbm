#pragma once

#include "ipsa_module.h"
#include "ipsa_tableupdate_key.h"

class IpsaTableUpdater : public IpsaModule {
public:
    IpsaTableUpdateType type;
    int proc_id;
    int matcher_id;
    Rp4MatchType match_type = MT_EXACT;
    std::vector<IpsaTableUpdateKey> keys;
    int action_id;
    std::vector<IpsaTableUpdateActionParameter> action_para;
    IpsaTableUpdater(IpsaTableUpdateType _type, int _proc_id, int _matcher_id, int _action_id):
        type(_type), proc_id(_proc_id), matcher_id(_matcher_id), action_id(_action_id) {}
    virtual std::shared_ptr<IpsaValue> toIpsaValue() const {
        std::map<std::string, std::shared_ptr<IpsaValue>> dst = {
            {"type", makeValue(to_string(type))},
            {"proc_id", makeValue(proc_id)},
            {"matcher_id", makeValue(matcher_id)},
            {"match_type", makeValue(to_string(match_type))},
            {"keys", makeValue(keys)},
            {"action_id", makeValue(action_id)},
            {"action_para", makeValue(action_para)}
        };
        return makeValue(dst);
    }
};