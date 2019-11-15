#ifndef NODETYPES_H
#define NODETYPES_H

enum nodeTypes {

    tag_stmt                        = 1001,
    tag_ntype                       = 1002,
    tag_file                        = 1003,
    tag_package                     = 1004,
    tag_imports                     = 1005,
    tag_import                      = 1006,
    tag_import_stmt                 = 1007,
    tag_arg_type                    = 1008,
    tag_hidden_import               = 1009,
    tag_ocomma                      = 1010,
    tag_osemi                       = 1011,
    tag_fnlitdcl                    = 1012,
    tag_error                       = 1013,
    tag_case                        = 1014,
    tag_caseblock                   = 1015,
    tag_hidden_import_list          = 1016,
    tag_import_stmt_list            = 1017,
    tag_compound_stmt               = 1018,
    tag_dotname                     = 1019,
    tag_embed                       = 1020,
    tag_expr                        = 1021,
    tag_complitexpr                 = 1022,
    tag_bare_complitexpr            = 1023,
    tag_expr_or_type                = 1024,
    tag_import_package              = 1025,
    tag_import_safety               = 1026,
    tag_import_there                = 1027,
    tag_fndcl                       = 1028,
    tag_hidden_fndcl                = 1029,
    tag_fnliteral                   = 1030,
    tag_lconst                      = 1031,
    tag_for_body                    = 1032,
    tag_for_header                  = 1033,
    tag_for_stmt                    = 1034,
    tag_if_header                   = 1035,
    tag_if_stmt                     = 1036,
    tag_return                      = 1037,
    tag_interfacedcl                = 1038,
    tag_keyval                      = 1039,
    tag_labelname                   = 1040,
    tag_name                        = 1041,
    tag_name_or_type                = 1042,
    tag_non_expr_type               = 1043,
    tag_new_name                    = 1044,
    tag_dcl_name                    = 1045,
    tag_oexpr                       = 1046,
    tag_typedclname                 = 1047,
    tag_onew_name                   = 1048,
    tag_osimple_stmt                = 1049,
    tag_pexpr                       = 1050,
    tag_pexpr_no_paren              = 1051,
    tag_pseudocall                  = 1052,
    tag_range_stmt                  = 1053,
    tag_select_stmt                 = 1054,
    tag_simple_stmt                 = 1055,
    tag_switch_stmt                 = 1056,
    tag_uexpr                       = 1057,
    tag_xfndcl                      = 1058,
    tag_typedcl                     = 1059,
    tag_start_complit               = 1060,
    tag_xdcl                        = 1061,
    tag_fnbody                      = 1062,
    tag_fnres                       = 1063,
    tag_loop_body                   = 1064,
    tag_dcl_name_list               = 1065,
    tag_new_name_list               = 1066,
    tag_expr_list                   = 1067,
    tag_keyval_list                 = 1068,
    tag_braced_keyval_list          = 1069,
    tag_expr_or_type_list           = 1070,
    tag_xdcl_list                   = 1071,
    tag_oexpr_list                  = 1072,
    tag_caseblock_list              = 1073,
    tag_elseif                      = 1074,
    tag_elseif_list                 = 1075,
    tag_else                        = 1076,
    tag_stmt_list                   = 1077,
    tag_oarg_type_list_ocomma       = 1078,
    tag_arg_type_list               = 1079,
    tag_interfacedcl_list           = 1080,
    tag_vardcl                      = 1081,
    tag_vardcl_list                 = 1082,
    tag_structdcl                   = 1083,
    tag_structdcl_list              = 1084,
    tag_common_dcl                  = 1085,
    tag_constdcl                    = 1086,
    tag_constdcl1                   = 1087,
    tag_constdcl_list               = 1088,
    tag_typedcl_list                = 1089,
    tag_convtype                    = 1090,
    tag_comptype                    = 1091,
    tag_dotdotdot                   = 1092,
    tag_indcl                       = 1093,
    tag_interfacetype               = 1094,
    tag_structtype                  = 1095,
    tag_ptrtype                     = 1096,
    tag_recvchantype                = 1097,
    tag_non_recvchantype            = 1098,
    tag_othertype                   = 1099,
    tag_fnret_type                  = 1100,
    tag_fntype                      = 1101,
    tag_hidden_importsym            = 1102,
    tag_hidden_pkg_importsym        = 1103,
    tag_hidden_constant             = 1104,
    tag_hidden_literal              = 1105,
    tag_hidden_funarg               = 1106,
    tag_hidden_interfacedcl         = 1107,
    tag_hidden_structdcl            = 1108,
    tag_hidden_funres               = 1109,
    tag_ohidden_funres              = 1110,
    tag_hidden_funarg_list          = 1111,
    tag_ohidden_funarg_list         = 1112,
    tag_hidden_interfacedcl_list    = 1113,
    tag_ohidden_interfacedcl_list   = 1114,
    tag_hidden_structdcl_list       = 1115,
    tag_ohidden_structdcl_list      = 1116,
    tag_hidden_type                 = 1117,
    tag_hidden_type_misc            = 1118,
    tag_hidden_pkgtype              = 1119,
    tag_hidden_type_func            = 1120,
    tag_hidden_type_recv_chan       = 1121,
    tag_hidden_type_non_recv_chan   = 1122,
    tag_bracedkeyval_list           = 1123,
    tag_packname                    = 1124,
    tag_arg_type_list_ocomma        = 1125,
    tag_hidden_funrees              = 1126,
    tag_bad_expr                    = 1127,
    tag_bare_complicitexpr          = 1128,
    tag_complicitexpr               = 1129,
    tag_import_here                 = 1130,
    tag_pexpr_no_paren_dot          = 1131,
    tag_simple_stmt2                = 1132,
    tag_assignment                  = 1133,
    tag_vardcl_init                 = 1134,
    tag_othertype_arr               = 1135,
    tag_othertype_map               = 1136,
    tag_common_dcl_2                = 1137,
    tag_pseudocall_args             = 1138,
    tag_empty                       = 2000
};

#endif // NODETYPES_H
