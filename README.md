
Message definition: BO_
BO_ <message_id> <message_name>: <message_size> <sender_name>
Example:
BO_ 21 accu_stack_info: 8 accu_distri

Signal definition: SG_
SG_ <signal_name> : <start_bit>|<length>@<endianness>+/- (<factor>,<offset>) [<min>|<max>] "<unit>" <receiver_name>
<!-- + for unsigned and - for signed  -->
<!-- real_value = raw × factor + offset -->
<!-- Multplexing Signal SG_ <signal_name> <multiplexer_signal> : ... 
     Other fields are called multiplexed signals 
-->

Example:
SG_ accu_stack_info_stackindex M : 0|4@1+ (1,0) [0|15] ""  micro_auto_box

Attribute definition: BA_
<attribute_name> <object_type> <object_id> <attribute_name> : <data_type>



Value definition: VAL_
VAL_ <signal_name> <value> "<text>";


CM_