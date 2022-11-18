#pragma once

#include "ipsa_gateway.h"
#include "ipsa_stage_manager.h"
#include "ipsa_table_manager.h"

class IpsaGatewayManager {
public:
    IpsaHeaderManager* header_manager;
    IpsaStageManager* stage_manager; // after initialization
    IpsaTableManager* table_manager;
    int global_gateway_id;
    std::vector<IpsaGateway> gateways;
    IpsaGatewayManager(IpsaHeaderManager* _header_manager, IpsaStageManager* _stage_manager, IpsaTableManager* _table_manager):
        header_manager(_header_manager), stage_manager(_stage_manager), table_manager(_table_manager) {}
    void load();
};

void IpsaGatewayManager::load() {
    global_gateway_id = 0;
    auto make_gateway_entry = [&](const Rp4SwitchEntry& entry) -> std::shared_ptr<IpsaGatewayEntry> {
        if (entry.value->isTableStmt()) {
            auto name = std::static_pointer_cast<Rp4SwitchTableStmt>(entry.value)->name;
            return std::make_shared<IpsaGatewayTableEntry>(
                table_manager->lookup(name)->table_id
            );
        } else {
            auto x = std::static_pointer_cast<Rp4SwitchStageStmt>(entry.value);
            if (x->to_none) {
                return std::make_shared<IpsaGatewayStageEntry>(-1);
            } else {
                return std::make_shared<IpsaGatewayStageEntry>(
                    stage_manager->lookup(x->name)->stage_id
                );
            }
        }
    };
    for (auto& stage : stage_manager->logical_stages) {
        auto& matcher = stage.def->matcher;
        IpsaGateway gateway;
        for (auto op : matcher.ops) {
            gateway.expressions.push_back(expr_to_expr(op, header_manager));
        }
        gateway.next_table.default_entry = nullptr;
        for (auto& entry : matcher.switch_entries) {
            if (entry.key->isDefault()) {
                gateway.next_table.default_entry = make_gateway_entry(entry);
            } else {
                gateway.next_table.entries.push_back(
                    IpsaNextTableEntry(
                        entry.key->get_key(),
                        make_gateway_entry(entry)
                    )
                );
            }
        }
        if (gateway.next_table.default_entry.get() == nullptr) {
            gateway.next_table.default_entry = gateway.next_table.entries[0].value;
        }
        gateways.push_back(std::move(gateway));
        stage.gateway_id = global_gateway_id++;
    }
}