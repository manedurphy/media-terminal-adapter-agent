#include "mta_agent_rbus_handlers.h"

static rbusError_t rbus_get_handler(rbusHandle_t handle,
                                    rbusProperty_t property,
                                    rbusGetHandlerOptions_t *opts) {
  (void)handle;
  (void)opts;

  char const *param_name = rbusProperty_GetName(property);
  rbusValue_t value = NULL;
  rbusError_t rc = RBUS_ERROR_ELEMENT_DOES_NOT_EXIST;

  CcspTraceInfo(("GET request for: %s\n", param_name));

  /* Allocate return value */
  rbusValue_Init(&value);
  if (!value) {
    CcspTraceError(("rbusValue_Init failed for %s\n", param_name));
    return RBUS_ERROR_BUS_ERROR;
  }

  CcspTraceDebug(("rbusValue initialized successfully for %s\n", param_name));

  const char *last_dot = strrchr(param_name, '.');
  if (last_dot == NULL) {
    CcspTraceError(("Invalid parameter name: %s\n", param_name));
    rbusValue_Release(value);
    return RBUS_ERROR_INVALID_INPUT;
  }

  // Get the parent object. This is required to know which API to call for a
  // data model value.
  char parent_name[256] = {0};
  strncpy(parent_name, param_name, last_dot - param_name);

  CcspTraceDebug(("Extracted parent object name: %s from parameter name: %s\n",
                  parent_name, param_name));
  parent_object_t *parent_object = get_parent_object(parent_name);
  if (!parent_object) {
    CcspTraceError(("Parent object not found for parameter: %s\n", param_name));
    rbusValue_Release(value);
    return RBUS_ERROR_ELEMENT_DOES_NOT_EXIST;
  }
  CcspTraceDebug(
      ("Found parent object: %s for parameter: %s with %zd child parameters\n",
       parent_object->name, param_name, parent_object->child_parameter_count));

  if (strcmp(parent_object->name, "Device.X_CISCO_COM_MTA_V6") == 0) {
    // All parameters under the "Device.X_CISCO_COM_MTA" parent object

    size_t prefix_len = strlen("Device.X_CISCO_COM_MTA_V6.");
    for (size_t i = 0; i < parent_object->child_parameter_count; i++) {
      char *param_short_name = (char *)(param_name + prefix_len);
      child_parameter_t child_parameter = parent_object->parameters[i];

      if (strcmp(param_name, child_parameter.name) == 0) {
        if (strcmp(child_parameter.type, "string") == 0) {
          char parameter_value[256] = {0};
          ULONG parameter_value_len = sizeof(parameter_value);
          if (X_CISCO_COM_MTA_V6_GetParamStringValue(NULL, param_short_name,
                                                     parameter_value,
                                                     &parameter_value_len)) {
            rbusValue_SetString(value, parameter_value);
            rc = RBUS_ERROR_SUCCESS;
          } else {
            CcspTraceError(
                ("X_CISCO_COM_MTA_V6_GetParamStringValue failed for %s\n",
                 param_name));
            rc = RBUS_ERROR_BUS_ERROR;
          }
        } else if (strcmp(child_parameter.type, "uint32_t") == 0) {
          ULONG val = 0;
          if (X_CISCO_COM_MTA_V6_GetParamUlongValue(NULL, param_short_name,
                                                    &val)) {
            rbusValue_SetUInt32(value, val);
            rc = RBUS_ERROR_SUCCESS;
          } else {
            CcspTraceError(
                ("X_CISCO_COM_MTA_V6_GetParamUlongValue failed for %s\n",
                 param_name));
            rc = RBUS_ERROR_BUS_ERROR;
          }
        } else {
          CcspTraceError(("Unsupported parameter type for %s\n", param_name));
          rc = RBUS_ERROR_BUS_ERROR;
        }
      }
    }
  } else if (strcmp(parent_object->name, "Device.X_CISCO_COM_MTA") == 0) {
    // All parameters under the "Device.X_CISCO_COM_MTA" parent object

    size_t prefix_len = strlen("Device.X_CISCO_COM_MTA.");
    for (size_t i = 0; i < parent_object->child_parameter_count; i++) {
      char *param_short_name = (char *)(param_name + prefix_len);
      child_parameter_t child_parameter = parent_object->parameters[i];

      CcspTraceDebug(("CAN YOU SEE THIS: param_name: %s, child_parameter.name: "
                      "%s, param_short_name: %s, child_parameter.type: %s\n",
                      param_name, child_parameter.name, child_parameter.type,
                      param_short_name));

      if (strcmp(param_name, child_parameter.name) == 0) {
        if (strcmp(child_parameter.type, "string") == 0) {
          char parameter_value[256] = {0};
          ULONG parameter_value_len = sizeof(parameter_value);
          CcspTraceDebug(
              ("Attempting to get string value for parameter: %s using API: "
               "X_CISCO_COM_MTA_GetParamStringValue with short name: %s\n",
               param_name, param_short_name));
          if (X_CISCO_COM_MTA_GetParamStringValue(NULL, param_short_name,
                                                  parameter_value,
                                                  &parameter_value_len)) {
            rbusValue_SetString(value, parameter_value);
            rc = RBUS_ERROR_SUCCESS;
          } else {
            CcspTraceError(
                ("X_CISCO_COM_MTA_GetParamStringValue failed for %s\n",
                 param_name));
            rc = RBUS_ERROR_BUS_ERROR;
          }
        } else if (strcmp(child_parameter.type, "boolean") == 0) {
          BOOL val = FALSE;
          if (X_CISCO_COM_MTA_GetParamBoolValue(NULL, param_short_name, &val)) {
            rbusValue_SetBoolean(value, val);
            rc = RBUS_ERROR_SUCCESS;
          } else {
            CcspTraceError(("X_CISCO_COM_MTA_GetParamBoolValue failed for %s\n",
                            param_name));
            rc = RBUS_ERROR_BUS_ERROR;
          }
        } else if (strcmp(child_parameter.type, "uint32_t") == 0) {
          ULONG val = 0;
          if (X_CISCO_COM_MTA_GetParamUlongValue(NULL, param_short_name,
                                                 &val)) {
            rbusValue_SetUInt32(value, val);
            rc = RBUS_ERROR_SUCCESS;
          } else {
            CcspTraceError(
                ("X_CISCO_COM_MTA_GetParamUlongValue failed for %s\n",
                 param_name));
            rc = RBUS_ERROR_BUS_ERROR;
          }
        } else {
          CcspTraceError(("Unsupported parameter type for %s\n", param_name));
          rc = RBUS_ERROR_BUS_ERROR;
        }
      }
    }
  } else if (strcmp(parent_object->name, "Device.X_CISCO_COM_MTA.DSXLog") ==
             0) {
    // All parameters under the "Device.X_CISCO_COM_MTA.DSXLog" parent object

    size_t prefix_len = strlen("Device.X_CISCO_COM_MTA.DSXLog.");
    for (size_t i = 0; i < parent_object->child_parameter_count; i++) {
      char *param_short_name = (char *)(param_name + prefix_len);
      child_parameter_t child_parameter = parent_object->parameters[i];

      if (strcmp(param_name, child_parameter.name) == 0) {
        if (strcmp(child_parameter.type, "string") == 0) {
          char parameter_value[256] = {0};
          ULONG parameter_value_len = sizeof(parameter_value);
          if (DSXLog_GetParamStringValue(NULL, param_short_name,
                                         parameter_value,
                                         &parameter_value_len)) {
            rbusValue_SetString(value, parameter_value);
            rc = RBUS_ERROR_SUCCESS;
          } else {
            CcspTraceError(
                ("DSXLog_GetParamStringValue failed for %s\n", param_name));
            rc = RBUS_ERROR_BUS_ERROR;
          }
        } else if (strcmp(child_parameter.type, "uint32_t") == 0) {
          ULONG val = 0;
          if (DSXLog_GetParamUlongValue(NULL, param_short_name, &val)) {
            rbusValue_SetUInt32(value, val);
            rc = RBUS_ERROR_SUCCESS;
          } else {
            CcspTraceError(
                ("DSXLog_GetParamUlongValue failed for %s\n", param_name));
            rc = RBUS_ERROR_BUS_ERROR;
          }
        } else {
          CcspTraceError(("Unsupported parameter type for %s\n", param_name));
          rc = RBUS_ERROR_BUS_ERROR;
        }
      }
    }
  } else if (strcmp(parent_object->name, "Device.X_CISCO_COM_MTA.LineTable") ==
             0) {
    // All parameters under the "Device.X_CISCO_COM_MTA.LineTable" parent object

    size_t prefix_len = strlen("Device.X_CISCO_COM_MTA.LineTable.");
    for (size_t i = 0; i < parent_object->child_parameter_count; i++) {
      char *param_short_name = (char *)(param_name + prefix_len);
      child_parameter_t child_parameter = parent_object->parameters[i];

      if (strcmp(param_name, child_parameter.name) == 0) {
        if (strcmp(child_parameter.type, "string") == 0) {
          char parameter_value[256] = {0};
          ULONG parameter_value_len = sizeof(parameter_value);
          if (LineTable_GetParamStringValue(NULL, param_short_name,
                                            parameter_value,
                                            &parameter_value_len)) {
            rbusValue_SetString(value, parameter_value);
            rc = RBUS_ERROR_SUCCESS;
          } else {
            CcspTraceError(
                ("LineTable_GetParamStringValue failed for %s\n", param_name));
            rc = RBUS_ERROR_BUS_ERROR;
          }
        } else if (strcmp(child_parameter.type, "boolean") == 0) {
          BOOL val = FALSE;
          if (LineTable_GetParamBoolValue(NULL, param_short_name, &val)) {
            rbusValue_SetBoolean(value, val);
            rc = RBUS_ERROR_SUCCESS;
          } else {
            CcspTraceError(
                ("LineTable_GetParamBoolValue failed for %s\n", param_name));
            rc = RBUS_ERROR_BUS_ERROR;
          }
        } else if (strcmp(child_parameter.type, "uint32_t") == 0) {
          ULONG val = 0;
          if (LineTable_GetParamUlongValue(NULL, param_short_name, &val)) {
            rbusValue_SetUInt32(value, val);
            rc = RBUS_ERROR_SUCCESS;
          } else {
            CcspTraceError(
                ("LineTable_GetParamUlongValue failed for %s\n", param_name));
            rc = RBUS_ERROR_BUS_ERROR;
          }
        } else {
          CcspTraceError(("Unsupported parameter type for %s\n", param_name));
          rc = RBUS_ERROR_BUS_ERROR;
        }
      }
    }
  } else if (strcmp(parent_object->name,
                    "Device.X_CISCO_COM_MTA.LineTable.CALLP") == 0) {
    // All parameters under the "Device.X_CISCO_COM_MTA.LineTable.CALLP" parent
    // object

    size_t prefix_len = strlen("Device.X_CISCO_COM_MTA.LineTable.CALLP.");
    for (size_t i = 0; i < parent_object->child_parameter_count; i++) {
      char *param_short_name = (char *)(param_name + prefix_len);
      child_parameter_t child_parameter = parent_object->parameters[i];

      if (strcmp(param_name, child_parameter.name) == 0) {
        if (strcmp(child_parameter.type, "string") == 0) {
          char parameter_value[256] = {0};
          ULONG parameter_value_len = sizeof(parameter_value);
          if (CALLP_GetParamStringValue(NULL, param_short_name, parameter_value,
                                        &parameter_value_len)) {
            rbusValue_SetString(value, parameter_value);
            rc = RBUS_ERROR_SUCCESS;
          } else {
            CcspTraceError(
                ("CALLP_GetParamStringValue failed for %s\n", param_name));
            rc = RBUS_ERROR_BUS_ERROR;
          }
        } else {
          CcspTraceError(("Unsupported parameter type for %s\n", param_name));
          rc = RBUS_ERROR_BUS_ERROR;
        }
      }
    }
  } else if (strcmp(parent_object->name,
                    "Device.X_CISCO_COM_MTA.LineTable.VQM") == 0) {
    // All parameters under the "Device.X_CISCO_COM_MTA.LineTable.VQM" parent
    // object

    size_t prefix_len = strlen("Device.X_CISCO_COM_MTA.LineTable.VQM.");
    for (size_t i = 0; i < parent_object->child_parameter_count; i++) {
      char *param_short_name = (char *)(param_name + prefix_len);
      child_parameter_t child_parameter = parent_object->parameters[i];

      if (strcmp(param_name, child_parameter.name) == 0) {
        if (strcmp(child_parameter.type, "boolean") == 0) {
          BOOL val = FALSE;
          if (VQM_GetParamBoolValue(NULL, param_short_name, &val)) {
            rbusValue_SetBoolean(value, val);
            rc = RBUS_ERROR_SUCCESS;
          } else {
            CcspTraceError(
                ("LineTable_GetParamBoolValue failed for %s\n", param_name));
            rc = RBUS_ERROR_BUS_ERROR;
          }
        } else {
          CcspTraceError(("Unsupported parameter type for %s\n", param_name));
          rc = RBUS_ERROR_BUS_ERROR;
        }
      }
    }
  } else if (strcmp(parent_object->name,
                    "Device.X_CISCO_COM_MTA.LineTable.VQM.Calls") == 0) {
    // All parameters under the "Device.X_CISCO_COM_MTA.LineTable.VQM.Calls"
    // parent object

    size_t prefix_len = strlen("Device.X_CISCO_COM_MTA.LineTable.VQM.Calls.");
    for (size_t i = 0; i < parent_object->child_parameter_count; i++) {
      char *param_short_name = (char *)(param_name + prefix_len);
      child_parameter_t child_parameter = parent_object->parameters[i];

      if (strcmp(param_name, child_parameter.name) == 0) {
        if (strcmp(child_parameter.type, "string") == 0) {
          char parameter_value[256] = {0};
          ULONG parameter_value_len = sizeof(parameter_value);
          if (Calls_GetParamStringValue(NULL, param_short_name, parameter_value,
                                        &parameter_value_len)) {
            rbusValue_SetString(value, parameter_value);
            rc = RBUS_ERROR_SUCCESS;
          } else {
            CcspTraceError(
                ("Calls_GetParamStringValue failed for %s\n", param_name));
            rc = RBUS_ERROR_BUS_ERROR;
          }
        } else if (strcmp(child_parameter.type, "boolean") == 0) {
          BOOL val = FALSE;
          if (Calls_GetParamBoolValue(NULL, param_short_name, &val)) {
            rbusValue_SetBoolean(value, val);
            rc = RBUS_ERROR_SUCCESS;
          } else {
            CcspTraceError(
                ("Calls_GetParamBoolValue failed for %s\n", param_name));
            rc = RBUS_ERROR_BUS_ERROR;
          }
        } else if (strcmp(child_parameter.type, "uint32_t") == 0) {
          ULONG val = 0;
          if (Calls_GetParamUlongValue(NULL, param_short_name, &val)) {
            rbusValue_SetUInt32(value, val);
            rc = RBUS_ERROR_SUCCESS;
          } else {
            CcspTraceError(
                ("Calls_GetParamUlongValue failed for %s\n", param_name));
            rc = RBUS_ERROR_BUS_ERROR;
          }
        } else {
          CcspTraceError(("Unsupported parameter type for %s\n", param_name));
          rc = RBUS_ERROR_BUS_ERROR;
        }
      }
    }
  } else if (strcmp(parent_object->name,
                    "Device.X_CISCO_COM_MTA.ServiceClass") == 0) {
    // All parameters under the "Device.X_CISCO_COM_MTA.ServiceClass"
    // parent object

    size_t prefix_len = strlen("Device.X_CISCO_COM_MTA.ServiceClass.");
    for (size_t i = 0; i < parent_object->child_parameter_count; i++) {
      char *param_short_name = (char *)(param_name + prefix_len);
      child_parameter_t child_parameter = parent_object->parameters[i];

      if (strcmp(param_name, child_parameter.name) == 0) {
        if (strcmp(child_parameter.type, "string") == 0) {
          char parameter_value[256] = {0};
          ULONG parameter_value_len = sizeof(parameter_value);
          if (ServiceClass_GetParamStringValue(NULL, param_short_name,
                                               parameter_value,
                                               &parameter_value_len)) {
            rbusValue_SetString(value, parameter_value);
            rc = RBUS_ERROR_SUCCESS;
          } else {
            CcspTraceError(("ServiceClass_GetParamStringValue failed for %s\n",
                            param_name));
            rc = RBUS_ERROR_BUS_ERROR;
          }
        } else {
          CcspTraceError(("Unsupported parameter type for %s\n", param_name));
          rc = RBUS_ERROR_BUS_ERROR;
        }
      }
    }
  } else if (strcmp(parent_object->name,
                    "Device.X_CISCO_COM_MTA.ServiceFlow") == 0) {
    // All parameters under the "Device.X_CISCO_COM_MTA.ServiceFlow"
    // parent object

    size_t prefix_len = strlen("Device.X_CISCO_COM_MTA.ServiceClass.");
    for (size_t i = 0; i < parent_object->child_parameter_count; i++) {
      char *param_short_name = (char *)(param_name + prefix_len);
      child_parameter_t child_parameter = parent_object->parameters[i];

      if (strcmp(param_name, child_parameter.name) == 0) {
        if (strcmp(child_parameter.type, "string") == 0) {
          char parameter_value[256] = {0};
          ULONG parameter_value_len = sizeof(parameter_value);
          if (ServiceFlow_GetParamStringValue(NULL, param_short_name,
                                              parameter_value,
                                              &parameter_value_len)) {
            rbusValue_SetString(value, parameter_value);
            rc = RBUS_ERROR_SUCCESS;
          } else {
            CcspTraceError(("ServiceFlow_GetParamStringValue failed for %s\n",
                            param_name));
            rc = RBUS_ERROR_BUS_ERROR;
          }
        } else if (strcmp(child_parameter.type, "boolean") == 0) {
          BOOL val = FALSE;
          if (ServiceFlow_GetParamBoolValue(NULL, param_short_name, &val)) {
            rbusValue_SetBoolean(value, val);
            rc = RBUS_ERROR_SUCCESS;
          } else {
            CcspTraceError(
                ("ServiceFlow_GetParamBoolValue failed for %s\n", param_name));
            rc = RBUS_ERROR_BUS_ERROR;
          }
        } else if (strcmp(child_parameter.type, "uint32_t") == 0) {
          ULONG val = 0;
          if (ServiceFlow_GetParamUlongValue(NULL, param_short_name, &val)) {
            rbusValue_SetUInt32(value, val);
            rc = RBUS_ERROR_SUCCESS;
          } else {
            CcspTraceError(
                ("ServiceFlow_GetParamUlongValue failed for %s\n", param_name));
            rc = RBUS_ERROR_BUS_ERROR;
          }
        } else {
          CcspTraceError(("Unsupported parameter type for %s\n", param_name));
          rc = RBUS_ERROR_BUS_ERROR;
        }
      }
    }
  } else if (strcmp(parent_object->name, "Device.X_CISCO_COM_MTA.Dect") == 0) {
    // All parameters under the "Device.X_CISCO_COM_MTA.Dect"
    // parent object

    size_t prefix_len = strlen("Device.X_CISCO_COM_MTA.Dect.");
    for (size_t i = 0; i < parent_object->child_parameter_count; i++) {
      char *param_short_name = (char *)(param_name + prefix_len);
      child_parameter_t child_parameter = parent_object->parameters[i];

      if (strcmp(param_name, child_parameter.name) == 0) {
        if (strcmp(child_parameter.type, "string") == 0) {
          char parameter_value[256] = {0};
          ULONG parameter_value_len = sizeof(parameter_value);
          if (Dect_GetParamStringValue(NULL, param_short_name, parameter_value,
                                       &parameter_value_len)) {
            rbusValue_SetString(value, parameter_value);
            rc = RBUS_ERROR_SUCCESS;
          } else {
            CcspTraceError(
                ("Dect_GetParamStringValue failed for %s\n", param_name));
            rc = RBUS_ERROR_BUS_ERROR;
          }
        } else if (strcmp(child_parameter.type, "boolean") == 0) {
          BOOL val = FALSE;
          if (Dect_GetParamBoolValue(NULL, param_short_name, &val)) {
            rbusValue_SetBoolean(value, val);
            rc = RBUS_ERROR_SUCCESS;
          } else {
            CcspTraceError(
                ("Dect_GetParamBoolValue failed for %s\n", param_name));
            rc = RBUS_ERROR_BUS_ERROR;
          }
        } else if (strcmp(child_parameter.type, "uint32_t") == 0) {
          ULONG val = 0;
          if (Dect_GetParamUlongValue(NULL, param_short_name, &val)) {
            rbusValue_SetUInt32(value, val);
            rc = RBUS_ERROR_SUCCESS;
          } else {
            CcspTraceError(
                ("Dect_GetParamUlongValue failed for %s\n", param_name));
            rc = RBUS_ERROR_BUS_ERROR;
          }
        } else {
          CcspTraceError(("Unsupported parameter type for %s\n", param_name));
          rc = RBUS_ERROR_BUS_ERROR;
        }
      }
    }
  } else if (strcmp(parent_object->name,
                    "Device.X_CISCO_COM_MTA.Dect.Handsets") == 0) {
    // All parameters under the "Device.X_CISCO_COM_MTA.Dect.Handsets"
    // parent object

    size_t prefix_len = strlen("Device.X_CISCO_COM_MTA.Dect.Handsets.");
    for (size_t i = 0; i < parent_object->child_parameter_count; i++) {
      char *param_short_name = (char *)(param_name + prefix_len);
      child_parameter_t child_parameter = parent_object->parameters[i];

      if (strcmp(param_name, child_parameter.name) == 0) {
        if (strcmp(child_parameter.type, "string") == 0) {
          char parameter_value[256] = {0};
          ULONG parameter_value_len = sizeof(parameter_value);
          if (Handsets_GetParamStringValue(NULL, param_short_name,
                                           parameter_value,
                                           &parameter_value_len)) {
            rbusValue_SetString(value, parameter_value);
            rc = RBUS_ERROR_SUCCESS;
          } else {
            CcspTraceError(
                ("Handsets_GetParamStringValue failed for %s\n", param_name));
            rc = RBUS_ERROR_BUS_ERROR;
          }
        } else if (strcmp(child_parameter.type, "boolean") == 0) {
          BOOL val = FALSE;
          if (Handsets_GetParamBoolValue(NULL, param_short_name, &val)) {
            rbusValue_SetBoolean(value, val);
            rc = RBUS_ERROR_SUCCESS;
          } else {
            CcspTraceError(
                ("Handsets_GetParamBoolValue failed for %s\n", param_name));
            rc = RBUS_ERROR_BUS_ERROR;
          }
        } else if (strcmp(child_parameter.type, "uint32_t") == 0) {
          ULONG val = 0;
          if (Handsets_GetParamUlongValue(NULL, param_short_name, &val)) {
            rbusValue_SetUInt32(value, val);
            rc = RBUS_ERROR_SUCCESS;
          } else {
            CcspTraceError(
                ("Handsets_GetParamUlongValue failed for %s\n", param_name));
            rc = RBUS_ERROR_BUS_ERROR;
          }
        } else {
          CcspTraceError(("Unsupported parameter type for %s\n", param_name));
          rc = RBUS_ERROR_BUS_ERROR;
        }
      }
    }
  } else if (strcmp(parent_object->name, "Device.X_CISCO_COM_MTA.MTALog") ==
             0) {
    // All parameters under the "Device.X_CISCO_COM_MTA.MTALog"
    // parent object

    size_t prefix_len = strlen("Device.X_CISCO_COM_MTA.MTALog.");
    for (size_t i = 0; i < parent_object->child_parameter_count; i++) {
      char *param_short_name = (char *)(param_name + prefix_len);
      child_parameter_t child_parameter = parent_object->parameters[i];

      if (strcmp(param_name, child_parameter.name) == 0) {
        if (strcmp(child_parameter.type, "string") == 0) {
          char parameter_value[256] = {0};
          ULONG parameter_value_len = sizeof(parameter_value);
          if (MTALog_GetParamStringValue(NULL, param_short_name,
                                         parameter_value,
                                         &parameter_value_len)) {
            rbusValue_SetString(value, parameter_value);
            rc = RBUS_ERROR_SUCCESS;
          } else {
            CcspTraceError(
                ("MTALog_GetParamStringValue failed for %s\n", param_name));
            rc = RBUS_ERROR_BUS_ERROR;
          }
          // } else if (strcmp(child_parameter.type, "boolean") == 0) {
          //   BOOL val = FALSE;
          //   if (MTALog_GetParamBoolValue(NULL, param_short_name, &val)) {
          //     rbusValue_SetBoolean(value, val);
          //     rc = RBUS_ERROR_SUCCESS;
          //   } else {
          //     CcspTraceError(
          //         ("MTALog_GetParamBoolValue failed for %s\n", param_name));
          //     rc = RBUS_ERROR_BUS_ERROR;
          //   }
        } else if (strcmp(child_parameter.type, "uint32_t") == 0) {
          ULONG val = 0;
          if (MTALog_GetParamUlongValue(NULL, param_short_name, &val)) {
            rbusValue_SetUInt32(value, val);
            rc = RBUS_ERROR_SUCCESS;
          } else {
            CcspTraceError(
                ("MTALog_GetParamUlongValue failed for %s\n", param_name));
            rc = RBUS_ERROR_BUS_ERROR;
          }
        } else {
          CcspTraceError(("Unsupported parameter type for %s\n", param_name));
          rc = RBUS_ERROR_BUS_ERROR;
        }
      }
    }
  } else if (strcmp(parent_object->name, "Device.X_CISCO_COM_MTA.DECTLog") ==
             0) {
    // All parameters under the "Device.X_CISCO_COM_MTA.DECTLog"
    // parent object

    size_t prefix_len = strlen("Device.X_CISCO_COM_MTA.DECTLog.");
    for (size_t i = 0; i < parent_object->child_parameter_count; i++) {
      char *param_short_name = (char *)(param_name + prefix_len);
      child_parameter_t child_parameter = parent_object->parameters[i];

      if (strcmp(param_name, child_parameter.name) == 0) {
        if (strcmp(child_parameter.type, "string") == 0) {
          char parameter_value[256] = {0};
          ULONG parameter_value_len = sizeof(parameter_value);
          if (DECTLog_GetParamStringValue(NULL, param_short_name,
                                          parameter_value,
                                          &parameter_value_len)) {
            rbusValue_SetString(value, parameter_value);
            rc = RBUS_ERROR_SUCCESS;
          } else {
            CcspTraceError(
                ("DECTLog_GetParamStringValue failed for %s\n", param_name));
            rc = RBUS_ERROR_BUS_ERROR;
          }
          // } else if (strcmp(child_parameter.type, "boolean") == 0) {
          //   BOOL val = FALSE;
          //   if (DECTLog_GetParamBoolValue(NULL, param_short_name, &val)) {
          //     rbusValue_SetBoolean(value, val);
          //     rc = RBUS_ERROR_SUCCESS;
          //   } else {
          //     CcspTraceError(
          //         ("DECTLog_GetParamBoolValue failed for %s\n", param_name));
          //     rc = RBUS_ERROR_BUS_ERROR;
          //   }
        } else if (strcmp(child_parameter.type, "uint32_t") == 0) {
          ULONG val = 0;
          if (DECTLog_GetParamUlongValue(NULL, param_short_name, &val)) {
            rbusValue_SetUInt32(value, val);
            rc = RBUS_ERROR_SUCCESS;
          } else {
            CcspTraceError(
                ("DECTLog_GetParamUlongValue failed for %s\n", param_name));
            rc = RBUS_ERROR_BUS_ERROR;
          }
        } else {
          CcspTraceError(("Unsupported parameter type for %s\n", param_name));
          rc = RBUS_ERROR_BUS_ERROR;
        }
      }
    }
  } else if (strcmp(parent_object->name, "Device.X_CISCO_COM_MTA.Battery") ==
             0) {
    // All parameters under the "Device.X_CISCO_COM_MTA.Battery"
    // parent object

    size_t prefix_len = strlen("Device.X_CISCO_COM_MTA.Battery.");
    for (size_t i = 0; i < parent_object->child_parameter_count; i++) {
      char *param_short_name = (char *)(param_name + prefix_len);
      child_parameter_t child_parameter = parent_object->parameters[i];

      if (strcmp(param_name, child_parameter.name) == 0) {
        if (strcmp(child_parameter.type, "string") == 0) {
          char parameter_value[256] = {0};
          ULONG parameter_value_len = sizeof(parameter_value);
          if (Battery_GetParamStringValue(NULL, param_short_name,
                                          parameter_value,
                                          &parameter_value_len)) {
            rbusValue_SetString(value, parameter_value);
            rc = RBUS_ERROR_SUCCESS;
          } else {
            CcspTraceError(
                ("Battery_GetParamStringValue failed for %s\n", param_name));
            rc = RBUS_ERROR_BUS_ERROR;
          }
        } else if (strcmp(child_parameter.type, "boolean") == 0) {
          BOOL val = FALSE;
          if (Battery_GetParamBoolValue(NULL, param_short_name, &val)) {
            rbusValue_SetBoolean(value, val);
            rc = RBUS_ERROR_SUCCESS;
          } else {
            CcspTraceError(
                ("Battery_GetParamBoolValue failed for %s\n", param_name));
            rc = RBUS_ERROR_BUS_ERROR;
          }
        } else if (strcmp(child_parameter.type, "uint32_t") == 0) {
          ULONG val = 0;
          if (Battery_GetParamUlongValue(NULL, param_short_name, &val)) {
            rbusValue_SetUInt32(value, val);
            rc = RBUS_ERROR_SUCCESS;
          } else {
            CcspTraceError(
                ("Battery_GetParamUlongValue failed for %s\n", param_name));
            rc = RBUS_ERROR_BUS_ERROR;
          }
        } else {
          CcspTraceError(("Unsupported parameter type for %s\n", param_name));
          rc = RBUS_ERROR_BUS_ERROR;
        }
      } else if (strcmp(parent_object->name, "Device.VoiceService") == 0) {
        // All parameters under the "Device.VoiceService" parent object

        size_t prefix_len = strlen("Device.VoiceService.");
        for (size_t i = 0; i < parent_object->child_parameter_count; i++) {
          char *param_short_name = (char *)(param_name + prefix_len);
          child_parameter_t child_parameter = parent_object->parameters[i];

          if (strcmp(param_name, child_parameter.name) == 0) {
            if (strcmp(child_parameter.type, "string") == 0) {
              char parameter_value[256] = {0};
              ULONG parameter_value_len = sizeof(parameter_value);
              if (VoiceService_GetParamStringValue(NULL, param_short_name,
                                                   parameter_value,
                                                   &parameter_value_len)) {
                rbusValue_SetString(value, parameter_value);
                rc = RBUS_ERROR_SUCCESS;
              } else {
                CcspTraceError(
                    ("VoiceService_GetParamStringValue failed for %s\n",
                     param_name));
                rc = RBUS_ERROR_BUS_ERROR;
              }
            } else {
              CcspTraceError(
                  ("Unsupported parameter type for %s\n", param_name));
              rc = RBUS_ERROR_BUS_ERROR;
            }
          } else if (strcmp(parent_object->name,
                            "Device.X_RDKCENTRAL-COM_MTA") == 0) {
            // All parameters under the "Device.X_RDKCENTRAL-COM_MTA" parent
            // object

            size_t prefix_len = strlen("Device.X_RDKCENTRAL-COM_MTA.");
            for (size_t i = 0; i < parent_object->child_parameter_count; i++) {
              char *param_short_name = (char *)(param_name + prefix_len);
              child_parameter_t child_parameter = parent_object->parameters[i];

              if (strcmp(param_name, child_parameter.name) == 0) {
                if (strcmp(child_parameter.type, "string") == 0) {
                  char parameter_value[256] = {0};
                  ULONG parameter_value_len = sizeof(parameter_value);
                  if (X_RDKCENTRAL_COM_MTA_GetParamStringValue(
                          NULL, param_short_name, parameter_value,
                          &parameter_value_len)) {
                    rbusValue_SetString(value, parameter_value);
                    rc = RBUS_ERROR_SUCCESS;
                  } else {
                    CcspTraceError(("X_RDKCENTRAL_COM_MTA_GetParamStringValue "
                                    "failed for %s\n",
                                    param_name));
                    rc = RBUS_ERROR_BUS_ERROR;
                  }
                } else {
                  CcspTraceError(
                      ("Unsupported parameter type for %s\n", param_name));
                  rc = RBUS_ERROR_BUS_ERROR;
                }
              }
            }
          } else if (strcmp(parent_object->name,
                            "X_RDKCENTRAL-COM_EthernetWAN_MTA") == 0) {
            // All parameters under the "Device.X_RDKCENTRAL-COM_EthernetWAN_MTA
            // parent object

            size_t prefix_len =
                strlen("Device.X_RDKCENTRAL-COM_EthernetWAN_MTA.");
            for (size_t i = 0; i < parent_object->child_parameter_count; i++) {
              char *param_short_name = (char *)(param_name + prefix_len);
              child_parameter_t child_parameter = parent_object->parameters[i];

              if (strcmp(param_name, child_parameter.name) == 0) {
                if (strcmp(child_parameter.type, "string") == 0) {
                  char parameter_value[256] = {0};
                  ULONG parameter_value_len = sizeof(parameter_value);
                  if (EthernetWAN_MTA_GetParamStringValue(
                          NULL, param_short_name, parameter_value,
                          &parameter_value_len)) {
                    rbusValue_SetString(value, parameter_value);
                    rc = RBUS_ERROR_SUCCESS;
                  } else {
                    CcspTraceError(("EthernetWAN_MTA_GetParamStringValue "
                                    "failed for %s\n",
                                    param_name));
                    rc = RBUS_ERROR_BUS_ERROR;
                  }
                } else if (strcmp(child_parameter.type, "uint32_t") == 0) {
                  int val = 0;
                  if (EthernetWAN_MTA_GetParamIntValue(NULL, param_short_name,
                                                       &val)) {
                    rbusValue_SetUInt32(value, val);
                    rc = RBUS_ERROR_SUCCESS;
                  } else {
                    CcspTraceError(
                        ("EthernetWAN_MTA_GetParamIntValue failed for %s\n",
                         param_name));
                    rc = RBUS_ERROR_BUS_ERROR;
                  }
                } else {
                  CcspTraceError(
                      ("Unsupported parameter type for %s\n", param_name));
                  rc = RBUS_ERROR_BUS_ERROR;
                }
              }
            }
          } else {
            CcspTraceError(("Unsupported parent object for %s\n", param_name));
            rc = RBUS_ERROR_BUS_ERROR;
          }
        }
      }
    }
  }

  /* Set property value if successful */
  if (rc == RBUS_ERROR_SUCCESS) {
    rbusProperty_SetValue(property, value);
  }

  return rc;
}

static rbusError_t rbus_set_handler(rbusHandle_t handle,
                                    rbusProperty_t property,
                                    rbusSetHandlerOptions_t *opts) {
  (void)handle;
  (void)property;
  (void)opts;

  return RBUS_ERROR_SUCCESS;
}

rbusGetHandler_t mta_agent_get_handler(void) { return rbus_get_handler; }

rbusSetHandler_t mta_agent_set_handler(void) { return rbus_set_handler; }
