
/*
 * Copyright (C) Carbyn
 */

#include <ngx_config.h>
#include <ngx_core.h>
#include <ngx_http.h>


static char *ngx_http_shithole(ngx_conf_t *cf, ngx_command_t *cmd,
    void *conf);

static ngx_command_t  ngx_http_shithole_commands[] = {

    { ngx_string("shithole"),
      NGX_HTTP_MAIN_CONF|NGX_HTTP_SRV_CONF|NGX_CONF_NOARGS,
      ngx_http_shithole,
      0,
      0,
      NULL },

      ngx_null_command
 };


static ngx_http_module_t  ngx_http_shithole_module_ctx = {
    NULL,                                   /* preconfiguration */
    NULL,                                   /* postconfiguration */

    NULL,                                   /* create main configuration */
    NULL,                                   /* init main configuration */

    NULL,                                   /* create server configuration */
    NULL,                                   /* merge server configuration */

    NULL,                                   /* create location configuration */
    NULL                                    /* merge location configuration */
};


ngx_module_t  ngx_http_shithole_module = {
    NGX_MODULE_V1,
    &ngx_http_shithole_module_ctx,  /* module context */
    ngx_http_shithole_commands,     /* module directives */
    NGX_HTTP_MODULE,                /* module type */
    NULL,                           /* init master */
    NULL,                           /* init module */
    NULL,                           /* init process */
    NULL,                           /* init thread */
    NULL,                           /* exit thread */
    NULL,                           /* exit process */
    NULL,                           /* exit master */
    NGX_MODULE_V1_PADDING
};


static ngx_int_t
ngx_http_shithole_handler(ngx_http_request_t *r)
{
    char              hostname[NGX_MAXHOSTNAMELEN];
    ngx_table_elt_t  *h;

    ngx_log_debug(NGX_LOG_DEBUG_HTTP, r->connection->log, 0,
                  "hostname is: fuck");

    if (gethostname(hostname, NGX_MAXHOSTNAMELEN) == -1) {
        return NGX_ERROR;
    }

    hostname[NGX_MAXHOSTNAMELEN - 1] = '\0';

    h = ngx_list_push(&r->headers_out.headers);
    if (h == NULL) {
        return NGX_ERROR;
    }

    h->hash = 1;
    ngx_str_set(&h->key, "Shithole");
    ngx_str_set(&h->value, "fuck");

    return NGX_OK;
}

static char *
ngx_http_shithole(ngx_conf_t *cf, ngx_command_t *cmd, void *conf)
{
    ngx_http_core_loc_conf_t  *clcf;

    clcf = ngx_http_conf_get_module_loc_conf(cf, ngx_http_core_module);
    clcf->handler = ngx_http_shithole_handler;

    return NGX_CONF_OK;
}
