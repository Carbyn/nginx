
/*
 * Copyright (C) Carbyn
 */

#include <ngx_config.h>
#include <ngx_core.h>
#include <ngx_http.h>


typedef struct {
    ngx_str_t  cmd;
    ngx_int_t  mem;
} ngx_http_yak_conf_t;


static char *ngx_http_yak_set(ngx_conf_t *cf, ngx_command_t *cmd, 
    void *conf);

static void *ngx_http_yak_create_srv_conf(ngx_conf_t *cf);
static char *ngx_http_yak_merge_srv_conf(ngx_conf_t *cf, void *parent, 
    void *child);

static ngx_int_t ngx_http_yak_init(ngx_conf_t *cf);


static ngx_command_t  ngx_http_yak_commands[] = {

    { ngx_string("yak"),
      NGX_HTTP_SRV_CONF|NGX_CONF_TAKE2,
      ngx_http_yak_set,
      NGX_HTTP_SRV_CONF_OFFSET,
      0,
      NULL },

      ngx_null_command
};


static ngx_http_module_t  ngx_http_yak_module_ctx = {
    NULL,                                   /* preconfiguration */
    ngx_http_yak_init,                      /* postconfiguration */

    NULL,
    NULL,                                   /* init main configuration */

    ngx_http_yak_create_srv_conf,           /* create server configuration */
    ngx_http_yak_merge_srv_conf,            /* merge server configuration */

    NULL,
    NULL,
};


ngx_module_t  ngx_http_yak_module = {
    NGX_MODULE_V1,
    &ngx_http_yak_module_ctx,       /* module context */
    ngx_http_yak_commands,          /* module directives */
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


static ngx_http_output_header_filter_pt  ngx_http_next_header_filter;


static char *
ngx_http_yak_set(ngx_conf_t *cf, ngx_command_t *cmd, void *conf)
{
    ngx_http_yak_conf_t  *yak_conf = conf;

    ngx_str_t  *value;

    value = cf->args->elts;

    if (ngx_strcasecmp(value[1].data, (u_char *) "on") == 0 
        || ngx_strcmp(value[1].data, "1") == 0) {
        yak_conf->mem = 1;
    } else {
        yak_conf->mem = 0;
    }
    yak_conf->cmd = value[2];

    ngx_conf_log_error(NGX_LOG_INFO, cf, 0, "yak set mem=%d, cmd=\"%V\"", 
                       yak_conf->mem, &yak_conf->cmd);

    return NGX_CONF_OK;
}

static ngx_int_t max_value_len = 1024;
static u_char *out;

static ngx_int_t
ngx_http_yak(ngx_http_request_t *r)
{
    FILE                 *pp;
    char                  buf[max_value_len];
    ngx_table_elt_t      *h;
    ngx_http_yak_conf_t  *yak_conf;

    yak_conf = ngx_http_get_module_srv_conf(r, ngx_http_yak_module);
    ngx_log_error(NGX_LOG_INFO, r->connection->log, 0,
                  "yak mem=%d, cmd=\"%V\"", yak_conf->mem, &yak_conf->cmd);

    if (yak_conf->mem == 0 || out == NULL) {
        if (yak_conf->cmd.len == 0) {
            ngx_log_error(NGX_LOG_EMERG, r->connection->log, 0,
                          "yak invalid param");
            return ngx_http_next_header_filter(r);
        }

        pp = popen((const char *)yak_conf->cmd.data, "r");
        if (pp == NULL) {
            ngx_log_error(NGX_LOG_EMERG, r->connection->log, 0,
                          "yak cmd returns failure. cmd=\"%V\"", &yak_conf->cmd);
            return ngx_http_next_header_filter(r);
        }

        out = (u_char *)fgets(buf, sizeof(buf), pp);
        pclose(pp);

        if (out == NULL) {
            ngx_log_error(NGX_LOG_EMERG, r->connection->log, 0,
                          "yak get cmd's output error. cmd=\"%V\"", &yak_conf->cmd);
            return ngx_http_next_header_filter(r);
        }
    }

    h = ngx_list_push(&r->headers_out.headers);
    if (h == NULL) {
        ngx_log_error(NGX_LOG_EMERG, r->connection->log, 0,
                      "yak add header error");
        return ngx_http_next_header_filter(r);
    }

    h->hash = 1;
    ngx_str_set(&h->key, "Yak");

    h->value.len = ngx_strlen(out);
    h->value.data = out;
    h->value.data[h->value.len] = '\0';

    return ngx_http_next_header_filter(r);
}

static ngx_int_t 
ngx_http_yak_init(ngx_conf_t *cf)
{
    ngx_http_next_header_filter = ngx_http_top_header_filter;
    ngx_http_top_header_filter = ngx_http_yak;

    return NGX_OK;
}

static void *
ngx_http_yak_create_srv_conf(ngx_conf_t *cf)
{
    ngx_http_yak_conf_t  *yak_conf = NULL;
    yak_conf = ngx_pcalloc(cf->pool, sizeof(ngx_http_yak_conf_t));
    if (yak_conf == NULL) {
        return NULL;
    }

    ngx_str_null(&yak_conf->cmd);
    yak_conf->mem = 0;
    ngx_conf_log_error(NGX_LOG_INFO, cf, 0,
                  "yak create srv mem=%d, cmd=\"%V\"", yak_conf->mem, &yak_conf->cmd);

    return yak_conf;
}

static char *
ngx_http_yak_merge_srv_conf(ngx_conf_t *cf, void *parent, void *child)
{
    ngx_http_yak_conf_t  *prev = parent;
    ngx_http_yak_conf_t  *conf = child;

    ngx_conf_merge_str_value(conf->cmd, prev->cmd, "");
    ngx_conf_merge_value(conf->mem, prev->mem, 0);
    ngx_conf_log_error(NGX_LOG_INFO, cf, 0,
                       "yak merge srv mem: pmem=%d, pcmd=\"%V\", cmem=%d, ccmd=\"%V\"", 
                       prev->mem, &prev->cmd, conf->mem, &conf->cmd);

    return NGX_CONF_OK;
}
