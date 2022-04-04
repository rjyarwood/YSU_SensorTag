/*
 @licstart  The following is the entire license notice for the JavaScript code in this file.

 The MIT License (MIT)

 Copyright (C) 1997-2020 by Dimitri van Heesch

 Permission is hereby granted, free of charge, to any person obtaining a copy of this software
 and associated documentation files (the "Software"), to deal in the Software without restriction,
 including without limitation the rights to use, copy, modify, merge, publish, distribute,
 sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in all copies or
 substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING
 BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
 DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

 @licend  The above is the entire license notice for the JavaScript code in this file
*/
var NAVTREE =
[
  [ "YSU SensorTag v1", "index.html", [
    [ "YSU SensorTag", "index.html", [
      [ "Needed Software", "index.html#autotoc_md1", null ],
      [ "Installation", "index.html#autotoc_md2", [
        [ "1. TI Setup", "index.html#autotoc_md3", null ],
        [ "2. Git Setup", "index.html#autotoc_md4", null ]
      ] ],
      [ "Opening an Example", "index.html#autotoc_md5", null ],
      [ "Documentation", "index.html#autotoc_md6", null ],
      [ "Adding a board", "index.html#autotoc_md7", null ]
    ] ],
    [ "New Sensor Guide", "md_readmes_new_sensor_guide.html", [
      [ "Developing For a New Board", "md_readmes_new_sensor_guide.html#autotoc_md9", null ],
      [ "Creating a Project", "md_readmes_new_sensor_guide.html#autotoc_md10", null ],
      [ "Adding a Sensor", "md_readmes_new_sensor_guide.html#autotoc_md11", null ],
      [ "Common Fixes", "md_readmes_new_sensor_guide.html#autotoc_md14", null ],
      [ "Known Issues", "md_readmes_new_sensor_guide.html#autotoc_md15", null ]
    ] ],
    [ "API: device related", "_d_e_v.html", [
      [ "Device API summary", "_d_e_v.html#dev_api", null ],
      [ "SNP power up Indication", "_d_e_v.html#SNP_powerUp_ind", null ],
      [ "SNP mask Event", "_d_e_v.html#SNP_maskEvent", null ],
      [ "SNP Event indication", "_d_e_v.html#SNP_Event_ind", [
        [ "about ATT MTU size", "_d_e_v.html#SNP_event_MTU", null ],
        [ "about the error event indication", "_d_e_v.html#SNP_event_Error", null ]
      ] ],
      [ "Get SNP Revision Command", "_d_e_v.html#SNP_getRevision", null ],
      [ "Get SNP Status Command", "_d_e_v.html#SNP_getStatus", null ],
      [ "Get Random Number Command", "_d_e_v.html#SNP_getRand", null ],
      [ "SNP test Command", "_d_e_v.html#SNP_test", null ],
      [ "SNP encapsulated HCI Command", "_d_e_v.html#SNP_HCI", null ],
      [ "SNP synchronous invalid command indication", "_d_e_v.html#Sync_error", null ]
    ] ],
    [ "API: GAP related", "_g_a_p.html", [
      [ "GAP API summary", "_g_a_p.html#gap_api", null ],
      [ "Start Advertisement", "_g_a_p.html#SNP_startAdv", null ],
      [ "Set Advertisement Data", "_g_a_p.html#SNP_setAdvData", null ],
      [ "Stop Advertisement", "_g_a_p.html#SNP_stopAdv", null ],
      [ "Update connection parameters", "_g_a_p.html#SNP_updateConnParam", null ],
      [ "Terminate Connection", "_g_a_p.html#SNP_terminateConn", null ],
      [ "Set GAP parameters", "_g_a_p.html#SNP_setGAPparam", null ],
      [ "Get GAP parameters", "_g_a_p.html#SNP_getGAPparam", null ],
      [ "Set Security parameters", "_g_a_p.html#SNP_setSecurityParam", null ],
      [ "set White List Filter Policy", "_g_a_p.html#SNP_setWhiteList", null ],
      [ "Send Security Request", "_g_a_p.html#SNP_sendSecurityRequest_req", null ],
      [ "Set Authentication Data", "_g_a_p.html#SNP_setAuthData", null ]
    ] ],
    [ "API: GATT related", "_g_a_t_t.html", [
      [ "GATT API summary", "_g_a_t_t.html#gatt_api", null ],
      [ "Add a Service", "_g_a_t_t.html#SNP_addService", null ],
      [ "Add a Characteristic to a service", "_g_a_t_t.html#SNP_addCharValueDecl", null ],
      [ "Add a optional attribute to a characteristic", "_g_a_t_t.html#SNP_addCharDescDecl", null ],
      [ "Register a Service", "_g_a_t_t.html#SNP_registerService", null ],
      [ "Get Attribute Value", "_g_a_t_t.html#SNP_getAttrValue", null ],
      [ "Set GATT parameters", "_g_a_t_t.html#SNP_setAttrValue", null ],
      [ "Characteristic read indication", "_g_a_t_t.html#SNP_charRead", null ],
      [ "Characteristic write indication", "_g_a_t_t.html#SNP_charWrite", null ],
      [ "Sending Notification or Indication", "_g_a_t_t.html#SNP_sendNotifInd", null ],
      [ "Set GATT parameters", "_g_a_t_t.html#SNP_setGATTparam", null ],
      [ "Get GATT parameters", "_g_a_t_t.html#SNP_getGATTparam", null ],
      [ "CCCD update indication", "_g_a_t_t.html#SNP_charConfigUpdated", null ]
    ] ],
    [ "Bug List", "bug.html", null ],
    [ "Todo List", "todo.html", null ],
    [ "Modules", "modules.html", "modules" ],
    [ "Namespaces", "namespaces.html", [
      [ "Namespace List", "namespaces.html", "namespaces_dup" ],
      [ "Namespace Members", "namespacemembers.html", [
        [ "All", "namespacemembers.html", null ],
        [ "Functions", "namespacemembers_func.html", null ]
      ] ]
    ] ],
    [ "Classes", "annotated.html", [
      [ "Class List", "annotated.html", "annotated_dup" ],
      [ "Class Index", "classes.html", null ],
      [ "Class Members", "functions.html", [
        [ "All", "functions.html", "functions_dup" ],
        [ "Variables", "functions_vars.html", "functions_vars" ]
      ] ]
    ] ],
    [ "Files", "files.html", [
      [ "File List", "files.html", "files_dup" ],
      [ "File Members", "globals.html", [
        [ "All", "globals.html", "globals_dup" ],
        [ "Functions", "globals_func.html", null ],
        [ "Variables", "globals_vars.html", null ],
        [ "Typedefs", "globals_type.html", null ],
        [ "Enumerations", "globals_enum.html", null ],
        [ "Enumerator", "globals_eval.html", null ],
        [ "Macros", "globals_defs.html", null ]
      ] ]
    ] ]
  ] ]
];

var NAVTREEINDEX =
[
"_b_o_o_s_t_x_l___c_c2650_m_a_8h.html",
"cc1350lp_2sensortag_2ccs_2app_2ysu__sensortag_2ysu__sensortag_8h_source.html",
"dir_b346821309a9b98c086a7db9c130865d.html",
"group___a_t_t___m_e_t_h_o_d___d_e_f_i_n_e_s.html#ga9300c3e8279bb840e4b225c85c699b4f",
"group___g_a_p___d_i_s_c___f_i_l_t_e_r___p_o_l_i_c_y___d_e_f_i_n_e_s.html#ga070652cfb925acfab5e961c593737b91",
"group___g_a_p_r_o_l_e_s___p_e_r_i_p_h_e_r_a_l___a_p_i.html#gacfeeaa2854f216e75fb92130ae6a6a7e",
"group___o_b_s_e_r_v_e_r___p_r_o_f_i_l_e___a_p_i.html#ga01b3cd38e7b0be6b25e31d6d31716e58",
"group___s_n_p___a_l_l_o_w_e_d___h_c_i.html#ga24e62f736d96c4c3e9c3b9fbb4068ffe",
"group__adxl__conf.html#ga484db64e1c3e6501f8442b6ad521b041",
"namespacemembers.html",
"struct___i_call___gap_signable__.html#a1ef2df13207bdab2c525f1aa3d91619b",
"struct__icall__fetch__msg__args__t.html#ad764d0a4444a085b5e34a5e8eabbab46",
"struct_watchdog_c_c26_x_x___object.html#a93d0a7f9f1de80b9e7873c57ad9c8d71",
"structgap_est_link_req_event__t.html#acf6d0bf9087d2e8ca33eb707c35a14e7",
"structl2cap_info_rsp__t.html#ace4ae9ef60cf7d358b3038f794acdb77",
"structxdc__runtime___memory___heap_proxy___module____.html",
"ysu1stk_2sensortag_2ccs_2app_2_a_d_x_l343_2adxl343_8h_source.html"
];

var SYNCONMSG = 'click to disable panel synchronisation';
var SYNCOFFMSG = 'click to enable panel synchronisation';