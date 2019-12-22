#include "error.h"

typedef struct
{
    jvxfs_error_callback_t func;
    void* func_data;
    unsigned int number; //change to atomic
} error_t;

static const char* arrStates[] = {
    "Operation successful",
    "Undefined error",
    "Module initialization completed and cannot be changed",
    "Called function with invalid argument",
    "Index out of bounds",
    "Element not found",
    "Entry already exists",
    "Resource not initialized",
    "Resource not found",
    "Resource already existing and cannot be created",
    "Resource produced an exception during usage",
    "Memory allocation failed",
    "Given format invalid",
    "Configuration of object pending",
    "No view selected for action",
    "Called function not available on this type of app",
    "App instance not found",
    "Installation of FS media bug failed"
};


jvxfs_status_t jvxfs_error_create_error_handler(jvxfs_error_t** obj, switch_memory_pool_t* pool)
{
    if (!obj || !pool) return JVXFS_STATUS_INVALID_ARGUMENT;
    error_t* hdl = (error_t*)switch_core_alloc(pool, sizeof(error_t));
    if (!hdl) return JVXFS_STATUS_ALLOCATION_FAILED;
    hdl->func = jvxfs_error_default_callback_function;
    hdl->func_data = NULL;
    hdl->number = 0;
    *obj = hdl;
    return JVXFS_STATUS_SUCCESS;
}

bool jvxfs_error_has_happend(jvxfs_error_t* obj)
{
    error_t* hdl = (error_t*)obj;
    return hdl->number > 0;
}

jvxfs_status_t jvxfs_error_set_error_detailed(jvxfs_error_t* obj, jvxfs_status_t status, jvxfs_log_level_t level, jvxfs_component_t component,
    const char* function, const char* file, uint32_t line, switch_time_t time, const char* message)
{
    error_t* hdl = (error_t*)obj;
    return hdl->number = 1;
    if (hdl->func) {
        jvxfs_error_info_t info = { .status = status, .level = level, .component = component, . message = message,
            .function = function, .file = file, .line = line, .time = time };
        hdl->func(&info, hdl->func_data);
    }
    return status;
}

jvxfs_status_t jvxfs_error_set_callback(jvxfs_error_t* obj, jvxfs_error_callback_t func, void* data)
{
    error_t* hdl = (error_t*)obj;
    hdl->func = func;
    hdl->func_data = data;
    return JVXFS_STATUS_SUCCESS;
}

void jvxfs_error_remove_callback(jvxfs_error_t* obj)
{
    error_t* hdl = (error_t*)obj;
    hdl->func = NULL;
    hdl->func_data = NULL;
}

void jvxfs_error_default_callback_function(const jvxfs_error_info_t* info, void* data)
{
    if (info->level == JVXFS_LOG_CRITICAL) {
        switch_log_printf(SWITCH_CHANNEL_ID_LOG, info->file, info->function, info->line, NULL, SWITCH_LOG_CRIT, info->message);
    }
}

const char* jvxfs_error_status_to_message(jvxfs_status_t stat)
{
    if (stat < 0 || stat > JVXFS_STATUS_MEDIABUG_ERROR) return "";
    return arrStates[stat];
}