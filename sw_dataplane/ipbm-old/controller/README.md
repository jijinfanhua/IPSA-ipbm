# Controller Source file / API
This file describes the interface the data plane should expose to controller.
注：本实验先完成一个控制器的情况，多控制器模式后续完成。先完成增删改流表项、下发processor需要的信息的基本结构。

主要技术
- gRPC: https://grpc.io/ (https://github.com/grpc)
- protobuf: https://developers.google.com/protocol-buffers/docs/overview (https://github.com/protocolbuffers/protobuf)
- nanomsg: https://github.com/nanomsg/nanomsg

## The basic API
**1. Create new flow table in the data plane**

```c++
 AddFlowTableRes addNewFlowTable(MemType mem_type, uint32_t residing_pool_table_id, uint32_t key_length, 
                                                                      uint32_t value_length, uint32_t action_data_num)
 ```
   - mem_type: SRAM/TCAM
   - residing_pool_table_id: tell device that this flow table should be place on which logic table in memory pool
   - key_length: the length of match fields
   - value_length: action pointer, action data, meter, counter.

**2. Add/Delete/modify flow entries**

```c++
FlowEntryModRes modifyFlowEntry(MemType memType, uint32_t tableId, const FlowEntry* srcFlow, FlowEntryModType modType,
                                              const FlowEntry* modFlow)
```
  - memType: SRAM/TCAM
  - tableId: logic table id in memory pool
  - srcFlow: the source flow entry
  - modType: add/delete/modify
  - modFlow: only used in mode modify
  
**3. Add a protocol in parsing graph**

```c++
ParserModRes addParserProtocol(const Header& header, const RepeatedPtrField<Field>& fields,
                                                                const RepeatedPtrField<NextHeader>& next_headers)
```

  - header: the protocol info
  - fields: fields that the protocol have
  - next_headers: next header identification

**4. Add match info in the new processor**

```c++
MatcherModRes addMatcher(uint32_t processor_id, const RepeatedPtrField<MatchField>& match_fields,
                                                                    MemType table_type, uint32_t table_id)
```

  - processor_id: which stage
  - match_fields: matching fields that the matcher use
  - table_type: SRAM/TCAM
  - table_id: logic table id in memory pool

**5. add actions in executer**

```c++
ExecuterModRes addAction(uint32_t processor_id, uint32_t action_id, const RepeatedField<int32> *instruction_sets,
                              const RepeatedPtrField<ParameterElement>& parameter_elements)
```

  - processor_id: which stage
  - action_id: index
  - instruction_sets: basic instruction like + -
  - parser_elements: parameters

## Run test
To generate pb.h, pb.cc, grpc.pb.h, grpc.pb.cc, you can 

```shell
sh proto.sh
```
or

```shell
protoc -I ./proto --grpc_out=. --plugin=protoc-gen-grpc=`which grpc_cpp_plugin` proto/controller_dataplane_api.proto
protoc -I ./proto --cpp_out=. proto/controller_dataplane_api.proto
```

To genenrate executables, you can

```shell
make
```

and then, 

```shell
./server
./client
```
to test the `server.cc`, `client.cc`

## Message Type
1. **Controller-to-Switch**：由控制器初始化，switch回不回复视情况而定
    - `Configuration`：控制器set/query交换机的配置参数，交换机对其进行回应；
    - `modify-State`：add/delete/modify flow/group entries, insert/remove action buckets of group;
    - `Read-State`：multipart message sequence, 收集current configuration, statistics, capabilities;
    - `Packet-out`：这个类型的消息必须包含一个完整的packet或者一个指向交换机中buffer的ID；必须包含a list of actions，如果是空则drop；
    - `Barrier`：确保消息顺序正确；提示操作完成；
2. **Asynchronous**：交换机发送该类型消息指示packet arrival或者switch state change
    - `Packet-in`：将packet的控制信息发送给控制器，经常出现在流表项action或者table-miss时；如果packet-in配置为缓存packet并且交换机由足够的空间对其进行缓存，则packet-in携带packet header的一部分以及用来指示packet位置的buffer ID；当交换机存储不够或者不能缓存packet时，则需要将整个packet发送到控制器。buffered packet会被Packet-out或者Flow-mod的消息类型处理，或者一段时间后自动过期。
    - `Flow-Removed`：通知控制器一条流表项已经从流表中移除。`OFPFF_SEND_FLOW_REM`,产生于控制器发出的flow delete请求以及交换机中表项的自动过期；
    - `Port-status`：通知控制器port的变化。例如port down事件；
    - `Flow-monitor`：通知流表的变化。
3. **Symmetric**：两个方向
    - `Hello`：连接；
    - `Echo`：确定controller-switch connetction的正常；测试延迟或带宽；
    - `Error`：大部分用在交换机不能完成控制器的请求时。

## protobuf Message
1. `ModifyFlowEntry`: add, delete or modify a flow entry
2. `AddParser`: add an entry in the `parser_table`, and then add entries in `field_table` and `next_header_table`;
3. `DeleteparserEntry`:
4. `AddMatcherInfo`: initiate a new processor when inserting a new function;
5. `ModifyMatcherInfo`: mostly used in adjusting the mapping from processor to sram/tcam tables;
6. `DeleteMatcherInfo`:
7. `AddActionInfo`: add actions for executer in specific processor;
