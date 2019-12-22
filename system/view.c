#include <string.h>
#include <switch.h>
#include "error.h"
#include "app.h"
#include "session.h"
#include "view_private.h"
#include "view.h"

static void print_2_human(view_priv_t* hdl, const char* data);
static switch_status_t print_2_machine(view_priv_t* hdl, const char* data);

jvxfs_view_input_t jvxfs_view_get_origin(jvxfs_view_t* view)
{
    view_priv_t* hdl = (view_priv_t*)view;
    return hdl->origin;
}

jvxfs_view_multi_output_t jvxfs_view_get_destinations(jvxfs_view_t* view)
{
    view_priv_t* hdl = (view_priv_t*)view;
    return hdl->dest;
}

void jvxfs_view_set_destinations(jvxfs_view_t* view, jvxfs_view_multi_output_t dest)
{
    view_priv_t* hdl = (view_priv_t*)view;
    hdl->dest = dest;
}

void jvxfs_view_set_all_destinations(jvxfs_view_t* view)
{
    view_priv_t* hdl = (view_priv_t*)view;
    hdl->dest = JVXFS_VIEW_OUT_EVENT | JVXFS_VIEW_OUT_CONSOLE | JVXFS_VIEW_OUT_LOG;
}

jvxfs_status_t jvxfs_view_write_human_readable(jvxfs_view_t* view, const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    jvxfs_status_t res = jvxfs_view_vwrite_human_readable(view, fmt, args);
    va_end(args);
    return res;
}

jvxfs_status_t jvxfs_view_vwrite_human_readable(jvxfs_view_t* view, const char* fmt, va_list args)
{
    view_priv_t* hdl = (view_priv_t*)view;
    char *data = switch_vmprintf(fmt, args);
    if (!data) {
        return jvxfs_error_set_error(hdl->err, JVXFS_STATUS_ALLOCATION_FAILED, JVXFS_LOG_ERROR, JVXFS_COMP_VIEW,
            "Could not buffer string.");
    }
    print_2_human(hdl, data);
    free(data);
    return JVXFS_STATUS_SUCCESS;
}

jvxfs_status_t jvxfs_view_write_machine_readable(jvxfs_view_t* view, const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    jvxfs_status_t res = jvxfs_view_vwrite_machine_readable(view, fmt, args);
    va_end(args);
    return res;
}

jvxfs_status_t jvxfs_view_vwrite_machine_readable(jvxfs_view_t* view, const char* fmt, va_list args)
{
    view_priv_t* hdl = (view_priv_t*)view;
    char *data = switch_vmprintf(fmt, args);
    if (!data) {
        return jvxfs_error_set_error(hdl->err, JVXFS_STATUS_ALLOCATION_FAILED, JVXFS_LOG_ERROR, JVXFS_COMP_VIEW,
            "Could not buffer string.");
    }
    jvxfs_status_t res = print_2_machine(hdl, data);
    free(data);
    return res;
}

jvxfs_status_t jvxfs_view_write_to_all(jvxfs_view_t* view, const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    jvxfs_status_t res = jvxfs_view_vwrite_to_all(view, fmt, args);
    va_end(args);
    return res;
}

jvxfs_status_t jvxfs_view_vwrite_to_all(jvxfs_view_t* view, const char* fmt, va_list args)
{
    view_priv_t* hdl = (view_priv_t*)view;
    char *data = switch_vmprintf(fmt, args);
    if (!data) {
        return jvxfs_error_set_error(hdl->err, JVXFS_STATUS_ALLOCATION_FAILED, JVXFS_LOG_ERROR, JVXFS_COMP_VIEW,
            "Could not buffer string.");
    }
    print_2_human(hdl, data);
    jvxfs_status_t res = print_2_machine(hdl, data);
    free(data);
    return res;
}


void print_2_human(view_priv_t* hdl, const char* data)
{
    jvxfs_view_multi_output_t dest = jvxfs_view_get_destinations(hdl);
    if (dest & JVXFS_VIEW_OUT_CONSOLE) {
        if (hdl->console) {
            hdl->console->raw_write_function(hdl->console, (uint8_t*)data, strlen(data));
        } else {
            switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_CONSOLE, data);
        }
    }
}

switch_status_t print_2_machine(view_priv_t* hdl, const char* data)
{
    jvxfs_view_multi_output_t dest = jvxfs_view_get_destinations(hdl);
    if (dest & JVXFS_VIEW_OUT_EVENT) {
        switch_event_t* event;
		switch_event_create_subclass(&event, SWITCH_EVENT_CUSTOM, "jvxfsFramework");
		switch_event_add_header_string(event, SWITCH_STACK_BOTTOM, "Protocol", "1.0");
        switch_event_add_header_string(event, SWITCH_STACK_BOTTOM, "Level", "0");
        switch_event_add_header_string(event, SWITCH_STACK_BOTTOM, "App", jvxfs_app_get_name(hdl->data->app));
        if (hdl->data->session) {
            switch_event_add_header_string(event, SWITCH_STACK_BOTTOM, "Session", switch_core_session_get_uuid(hdl->data->session));
            switch_event_add_header_string(event, SWITCH_STACK_BOTTOM, "Extension", jvxfs_session_get_extension_name(hdl->data->session));
            switch_event_add_header_string(event, SWITCH_STACK_BOTTOM, "Phonenumber", jvxfs_session_get_extension_number(hdl->data->session));
        }
        switch_event_add_header_string(event, SWITCH_STACK_BOTTOM, "Directive", hdl->data->directive);
        switch_event_add_header_string(event, SWITCH_STACK_BOTTOM, "Content", data);
        switch_status_t status = switch_event_fire(&event);
		if (status != SWITCH_STATUS_SUCCESS)
		{
            return jvxfs_error_set_error(hdl->err, JVXFS_STATUS_RESOURCE_EXCEPTION, JVXFS_LOG_ERROR, JVXFS_COMP_VIEW,
            "Could not fire event.");
        }
    }
    return JVXFS_STATUS_SUCCESS;
}