#include "../system/session.h"
#include "../system/error.h"
#include "../system/app.h"
#include "../utils/observer.h"
#include "sp_config.h"
#include "sp_processor.h"

typedef struct
{
    jvxfs_app_t* app;
    switch_core_session_t* session;
    switch_media_bug_t* bug;
    jvxfs_sigproc_state_t state;
    jvxfs_error_t* err;
    jvxfs_sigprog_config_t* config;
    jvxfs_observer_handle_t* mode_obs;
    switch_atomic_t mode;
    jvxfs_algorithm_vtable_t* vtable;
} proc_t;

static void set_state(proc_t* hdl, jvxfs_sigproc_state_t state);
static switch_bool_t media_bug_callback(switch_media_bug_t* bug, void* handle, switch_abc_type_t type);
static jvxfs_status_t install_media_bug(proc_t* hdl);
static void init_algo(proc_t* hdl);
static void destroy_processor(proc_t* hdl);


jvxfs_status_t jvxfs_sigproc_create_processor(jvxfs_sigproc_processor_t** obj, jvxfs_app_t* app, jvxfs_error_t* err,
    switch_core_session_t* session, const char* args, void* data)
{
    proc_t* hdl = (proc_t*)switch_core_session_alloc(session, sizeof(proc_t));
    if (!hdl) {
        return jvxfs_error_set_error(err, JVXFS_STATUS_ALLOCATION_FAILED, JVXFS_LOG_CRITICAL, JVXFS_COMP_SP_PROCESSOR,
            "Could not create session app instance.");
    }
    hdl->app = app;
    hdl->session = session;
    hdl->bug = NULL;
    hdl->state = JVXFS_SP_FAILED;
    hdl->err = err;
    hdl->vtable = (jvxfs_algorithm_vtable_t*)data;
    jvxfs_status_t res = jvxfs_app_get_sigproc_config(app, &hdl->config);
    if (res != JVXFS_STATUS_SUCCESS) return res;
    res = jvxfs_observer_create(&hdl->mode_obs, hdl, err, switch_core_session_get_pool(session));
    if (res != JVXFS_STATUS_SUCCESS) return res;
    switch_atomic_set(&hdl->mode, JVXFS_SP_ALGO_ON);
    set_state(hdl, JVXFS_SP_CONSTRUCTING);
//TODO: configure algorithm before media bug!
    res = install_media_bug(hdl);
    if (res != JVXFS_STATUS_SUCCESS) {
        set_state(hdl, JVXFS_SP_FAILED);
        return res;
    }
    *obj = hdl;
    return res;
}

switch_core_session_t* jvxfs_sigproc_get_session(jvxfs_sigproc_processor_t* proc)
{
    proc_t* hdl = (proc_t*)proc;
    return hdl->session;
}

jvxfs_sigproc_state_t jvxfs_sigproc_get_state(jvxfs_sigproc_processor_t* proc)
{
    proc_t* hdl = (proc_t*)proc;
    return hdl->state;
}

jvxfs_sigproc_algo_mode_t jvxfs_sigproc_get_mode(jvxfs_sigproc_processor_t* proc)
{
    proc_t* hdl = (proc_t*)proc;
    return switch_atomic_read(&hdl->mode);
}

jvxfs_status_t jvxfs_sigproc_set_mode(jvxfs_sigproc_processor_t* proc, jvxfs_sigproc_algo_mode_t mode)
{
    proc_t* hdl = (proc_t*)proc;
    if (mode < JVXFS_SP_ALGO_OFF || mode > JVXFS_SP_ALGO_MUTE) {
        return jvxfs_error_set_error(hdl->err, JVXFS_STATUS_INVALID_ARGUMENT, JVXFS_LOG_ERROR, JVXFS_COMP_SP_PROCESSOR,
            "Unknown algorithm mode.");
    }
    switch_atomic_set(&hdl->mode, mode);
    jvxfs_observer_notify(hdl->mode_obs);
    return JVXFS_STATUS_SUCCESS;
}

jvxfs_status_t jvxfs_sigproc_add_mode_observer(jvxfs_sigproc_processor_t* proc, jvxfs_sigproc_mode_observer_t func, void* data)
{
    proc_t* hdl = (proc_t*)proc;
    return jvxfs_observer_add(hdl->mode_obs, func, data);
}

void jvxfs_sigproc_remove_mode_observer(jvxfs_sigproc_processor_t* proc, jvxfs_sigproc_mode_observer_t func)
{
    proc_t* hdl = (proc_t*)proc;
    jvxfs_observer_remove(hdl->mode_obs, func);
}


void set_state(proc_t* hdl, jvxfs_sigproc_state_t state)
{
    hdl->state = state;
}

switch_bool_t media_bug_callback(switch_media_bug_t* bug, void* handle, switch_abc_type_t type)
{
    proc_t* hdl = (proc_t*)handle;
    switch (type) {
	case SWITCH_ABC_TYPE_INIT:
        init_algo(hdl);
		break;
	case SWITCH_ABC_TYPE_CLOSE:
        destroy_processor(hdl);
		break;
	case SWITCH_ABC_TYPE_READ:
	case SWITCH_ABC_TYPE_WRITE:
		break;
	case SWITCH_ABC_TYPE_READ_REPLACE:
		break;
	case SWITCH_ABC_TYPE_WRITE_REPLACE:
		//do sp stuff
		/*{
			switch_frame_t* frame = switch_core_media_bug_get_write_replace_frame(bug);
			if (objHandle->isFilterOn() && frame && frame->data)
				{	
					objHandle->process(static_cast<short*>(frame->data));
					switch_core_media_bug_set_write_replace_frame(bug, frame);
				}
		}*/
		break;
	default:
		break;
	}
    return SWITCH_TRUE;
}

jvxfs_status_t install_media_bug(proc_t* hdl)
{
    jvxfs_sigproc_buffer_t buf = jvxfs_sigproc_buffers_channel(hdl->config);
    switch_media_bug_flag_t bug_flags;
    switch (buf) {
        case JVXFS_SP_BUFFER_DOWNLINK:
            bug_flags = SMBF_WRITE_REPLACE;
            break;
        case JVXFS_SP_BUFFER_UPLINK:
            bug_flags = SMBF_READ_REPLACE;
            break;
        case JVXFS_SP_BUFFER_BOTH_LINKS:

        default:
            bug_flags = SMBF_PRUNE;
    }
    switch_status_t status = switch_core_media_bug_add(hdl->session, jvxfs_app_get_name(hdl->app), NULL, 
		media_bug_callback, hdl, 0, bug_flags, &(hdl->bug));
    if (status != SWITCH_STATUS_SUCCESS) {
        return jvxfs_error_set_error(hdl->err, JVXFS_STATUS_MEDIABUG_ERROR, JVXFS_LOG_CRITICAL, JVXFS_COMP_SP_PROCESSOR,
            "FS returned bad state.");
    }
    return JVXFS_STATUS_SUCCESS;
}

void init_algo(proc_t* hdl)
{
    set_state(hdl, JVXFS_SP_INITIALIZING);
    hdl->vtable->initialize(NULL, NULL);
}

void destroy_processor(proc_t* hdl)
{
    set_state(hdl, JVXFS_SP_DESTRUCTING);
    if (hdl->vtable->destruct) hdl->vtable->destruct(NULL);
    jvxfs_observer_destroy(&hdl->mode_obs);
}