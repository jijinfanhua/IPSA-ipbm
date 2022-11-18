# Pipeline Stage Processor设计
该文档描述每个processor的模板化设计。
## 逻辑结构
1. processor的逻辑结构为`parser->matcher->executer`，其中parser、matcher、executer均为协议无关、模板化设计，即：
    - **parser**：在给定`parsing_headers`（即本stage需要用到的header的集合）的情况下，parser可以将指定的header解析出来，并且是协议无关解析。所以每个processor内置一个parsing graph，其实就是`cur_protocol->next_protocol`的`(offset,length)`集合；
    - **matcher**：同样，控制器需要给出`matching_fields`，从parser解析出的headers里面取出需要的fields；还需要给出`match_type, match_table_pointer`这些信息才能进行匹配；
    - **executer**：通过matcher之后会查出`action_primitive_set_pointer`，其指向组成这个action的指令集，参数集是需要控制器指定：①修改packet header中的内容；②修改metadata内部的内容；
2. parser逻辑
    - 需要解析的header是一个集合`parsing_headers`，因为所需要匹配的字段并不一定在一个header中，可能在多个header中；
    - 为每个packet准备一个`global_headers`的全局变量，存储其已经解析出的header；
    - 在一个stage中，检查需要的header已经在`global_headers`中，则直接取，不需要重新解析
    （此处，两种选择：①`global_headers`存储所有header；②动态packet header vector，使用selector从里面选取header/P4交换机的实现是静态PHV+selector）；
    - 如果在`global_headers`里面没有，则根据当前的`header_id`以及`cur_header.next_header_type`来查找下一个header的长度
    （此处，下一个header可能是不需要的header，我们需要继续向下解析，得到需要的header，直到`parsing_headers`全部满足为止）
    - 将满足`parsing_headers`的所有header放入按包的`header_buffer`中，以供matcher选取需要的fields，即`matching_fields`里面所有的field
3. matcher逻辑
    - 控制器发来`matching_fields`的集合，集合内元素格式为`header_id.field_id : match_type`，均从`header_buffer`中获得；
    - 得到`field_id : match_type`的集合，根据`match_type`确定总的匹配方式；
    - 控制器也要将`match_table_pointer`发送至matcher中以供流表查找；
    - 最后可以得到`action_primitive_set_pointer`的集合，供下一个模块处理；
4. executer逻辑
    - executer得到的是来自matcher的`action_primitive_set_pointer`；
    - 在pointer指向的内容中，其包含指令集和参数集两个信息，指令集使用id来索引，参数集大部分是header, metadata, standard_metadata中的字段，可以使用`header_id.field_id`来表示；
    - 具体的action类型参考 http://flowgrammable.org/sdn/openflow/actions/#tab_ofp_1_4 的Action Types部分，总结下来就是：set_field, decrement, push/pop_field, copy_in/out, forward, drop, clone等，这些都要做成built-in的格式，由参数来确定其具体作用

## parser部分的变量
1. header

	| **header_id** | length | next_header_type_internal_offset | next_header_type_length |
    - header_id：header标识
    - length：header长度，若为正数则为实际长度，若为负数a，则表示header长度由id为-a的field来指示其长度（解决variable length header问题）
    - next_header_type_internal_offset：下一个header的类型在该header内的offset；
    - next_header_type_length：下一个header的类型的长度，这样就能从header取出下一个header的type；
    - 通过header_id可以查到后面的三项；


2. next_header
	| **next_header_type** | next_header_id |
    - next_header_type：例如0x0800指示ipv4；
    - next_header_id：相应的header在系统中的一个独有的id；
    - 通过next_header_type可以查到next_header_id；

3. field

	| **field_id** | field_length | internal_offset |
    - field_id：对字段的标识，所有的header的field在一个表中；
    - field_length：字段长度
    - internal_offset：该字段在该header内的offset；
    - 通过field_id可以查找到后面两项

以上叙述的三个列表，均需要从控制器给出定义，运行时控制器可以对其进行修改。

## matcher部分的变量
1. 字段field
	在parser解析出header_buffer之后，传递到matcher；控制器会下发matching_fields的变量，指示应该使用哪些field进行match。
    - matching_fields: {header_id.field_id: match_type}
    - match_table_pointer: 此处应该指向SRAM/TCAM的某一个逻辑表，其对应关系应该存储在控制器中，方便控制器在运行时插入新的功能或者匹配表；
    - 注意：在这个部分应该有匹配字段组装的过程，将多个field组成一个大的field到match_table中进行匹配
2. 返回值action_primitive_set_pointer
    - 当匹配方式为exact的时候，一条表项既含有match key，也含有其statistics，比如action、action_data、counter、meter等，这些都是需要算进一条表项的宽度的；
    - 当匹配方式为ternary的时候，一条表项只含有match key，其ststistics保存在RAM中的指定地址。
    - 所以，action指示primitive set所处位置（指令集），action_data为参数集

以上部分，控制器需要在运行时修改matching_fields，match_table_pointer变量。

## executer部分的变量
1. primitive set
    - set_field, decrement, push/pop_field, copy_in/out, forward, drop, clone等基本操作要写入交换机中，作为built-in；见 https://opennetworking.org/wp-content/uploads/2014/10/openflow-switch-v1.5.1.pdf P28-P31 5.8 Actions。
    - set-field *field_type* *value*：根据字段类型修改packet的字段；Set-Field actions should always be applied to the outermostpossible header, ("Set VLAN ID" action always sets the ID of the outermost VLAN tag, unless the field type specifies otherwise);
    - copy-field *src_field_type* *dst_field_type*：在header之间或者metadata之间进行copy；from a header field to a packet register pipeline field; from a packet register pipeline field to a header field; from a header field to another header field;
    - change-TTL *ttl*：修改IPv4 TTL，IPv6 Hop Limit或者MPLS TTL，一般情况下修改最外层TTL；set/decrement MPLS TTL, set/decrement IP TTL, copy TTL outwards/inwards；此处的协议均需要使用协议标号+字段标号表示；
    - push-tag/pop-tag *ethertype*：push/pop VLAN header, push/pop MPLS header, push/pop PBB header；此处VLAN MPLS PBB均需要使用标号代替；
    - output *port_no*：指定输出端口号, in egress processing.
    - group *group_id*：Process the packet through the specified group；
    - drop：packet如果没有output action和group action的话，必须drop；
    - set-queue *queue_id*：QoS;
    - meter *meter_id*：Direct packet to the specified meter (5.11)；
    - 每个指令都有编号与之对应；控制器会为一个processor分配几个action，每个action均是指令集和参数集的结合，指令集中的每条指令都在primitive set中
2. parameter_set的编号规则
    - header.field: [integer].[integer]
    - metadata.field：m.[integer]
    - standard_metadata.field：s.[integer]
    - table(processor_id)：t.[integer]
    - number：[integer]
    - metadata（随packet）和standard_metadata（随交换机）都是需要先注册得到标号

控制器需要下发action的primitive_set和parameter_set。

**以上控制器可以改变的部分需要留有接口供远程修改，且每个processor为一个线程**
