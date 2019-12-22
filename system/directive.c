#include "directives.h"
#include "app.h"
#include "view.h"

void jvxfs_directive_app_version(jvxfs_view_t* view, jvxfs_directive_data_t* rqst, void* data)
{
    const char* version = jvxfs_app_get_version(rqst->app);
    jvxfs_view_write_to_all(view, "%s", version);
}