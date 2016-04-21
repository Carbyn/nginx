
/*
 * Copyright (C) Carbyn
 */

#include <ngx_config.h>
#include <ngx_core.h>
#include <ngx_http.h>


static ngx_int_t ngx_http_yak_init(ngx_conf_t *cf);


static ngx_http_module_t  ngx_http_yak_module_ctx = {
    NULL,                                   /* preconfiguration */
    ngx_http_yak_init,                      /* postconfiguration */

    NULL,                                   /* create main configuration */
    NULL,                                   /* init main configuration */

    NULL,                                   /* create server configuration */
    NULL,                                   /* merge server configuration */

    NULL,                                   /* create location configuration */
    NULL                                    /* merge location configuration */
};


ngx_module_t  ngx_http_yak_module = {
    NGX_MODULE_V1,
    &ngx_http_yak_module_ctx,       /* module context */
    NULL,                           /* module directives */
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


static ngx_int_t
ngx_http_yak_header_filter(ngx_http_request_t *r)
{
    ngx_table_elt_t  *h;

    h = ngx_list_push(&r->headers_out.headers);
    if (h == NULL) {
        return NGX_ERROR;
    }

    ngx_str_set(&h->key, "Fuck-You");
    ngx_str_set(&h->value, "shithole");
    h->hash = 1;

    return ngx_http_next_header_filter(r);
}


static ngx_int_t ngx_http_yak_init(ngx_conf_t *cf)
{
    ngx_http_next_header_filter = ngx_http_top_header_filter;
    ngx_http_top_header_filter = ngx_http_yak_header_filter;

    return NGX_OK;
}
