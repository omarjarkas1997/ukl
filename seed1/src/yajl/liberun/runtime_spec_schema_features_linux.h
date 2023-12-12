// Generated from features-linux.json. Do not edit!
#ifndef RUNTIME_SPEC_SCHEMA_FEATURES_LINUX_SCHEMA_H
#define RUNTIME_SPEC_SCHEMA_FEATURES_LINUX_SCHEMA_H

#include <sys/types.h>
#include <stdint.h>
#include "json_common.h"
#include "runtime_spec_schema_defs_linux.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    bool v1;

    bool v2;

    bool systemd;

    bool systemd_user;

    bool rdma;

    yajl_val _residual;

    unsigned int v1_present : 1;
    unsigned int v2_present : 1;
    unsigned int systemd_present : 1;
    unsigned int systemd_user_present : 1;
    unsigned int rdma_present : 1;
}
runtime_spec_schema_features_linux_cgroup;

void free_runtime_spec_schema_features_linux_cgroup (runtime_spec_schema_features_linux_cgroup *ptr);

runtime_spec_schema_features_linux_cgroup *make_runtime_spec_schema_features_linux_cgroup (yajl_val tree, const struct parser_context *ctx, parser_error *err);

yajl_gen_status gen_runtime_spec_schema_features_linux_cgroup (yajl_gen g, const runtime_spec_schema_features_linux_cgroup *ptr, const struct parser_context *ctx, parser_error *err);

typedef struct {
    bool enabled;

    char **actions;
    size_t actions_len;

    char **operators;
    size_t operators_len;

    char **archs;
    size_t archs_len;

    char **known_flags;
    size_t known_flags_len;

    char **supported_flags;
    size_t supported_flags_len;

    yajl_val _residual;

    unsigned int enabled_present : 1;
}
runtime_spec_schema_features_linux_seccomp;

void free_runtime_spec_schema_features_linux_seccomp (runtime_spec_schema_features_linux_seccomp *ptr);

runtime_spec_schema_features_linux_seccomp *make_runtime_spec_schema_features_linux_seccomp (yajl_val tree, const struct parser_context *ctx, parser_error *err);

yajl_gen_status gen_runtime_spec_schema_features_linux_seccomp (yajl_gen g, const runtime_spec_schema_features_linux_seccomp *ptr, const struct parser_context *ctx, parser_error *err);

typedef struct {
    bool enabled;

    yajl_val _residual;

    unsigned int enabled_present : 1;
}
runtime_spec_schema_features_linux_apparmor;

void free_runtime_spec_schema_features_linux_apparmor (runtime_spec_schema_features_linux_apparmor *ptr);

runtime_spec_schema_features_linux_apparmor *make_runtime_spec_schema_features_linux_apparmor (yajl_val tree, const struct parser_context *ctx, parser_error *err);

yajl_gen_status gen_runtime_spec_schema_features_linux_apparmor (yajl_gen g, const runtime_spec_schema_features_linux_apparmor *ptr, const struct parser_context *ctx, parser_error *err);

typedef struct {
    bool enabled;

    yajl_val _residual;

    unsigned int enabled_present : 1;
}
runtime_spec_schema_features_linux_selinux;

void free_runtime_spec_schema_features_linux_selinux (runtime_spec_schema_features_linux_selinux *ptr);

runtime_spec_schema_features_linux_selinux *make_runtime_spec_schema_features_linux_selinux (yajl_val tree, const struct parser_context *ctx, parser_error *err);

yajl_gen_status gen_runtime_spec_schema_features_linux_selinux (yajl_gen g, const runtime_spec_schema_features_linux_selinux *ptr, const struct parser_context *ctx, parser_error *err);

typedef struct {
    bool enabled;

    yajl_val _residual;

    unsigned int enabled_present : 1;
}
runtime_spec_schema_features_linux_intel_rdt;

void free_runtime_spec_schema_features_linux_intel_rdt (runtime_spec_schema_features_linux_intel_rdt *ptr);

runtime_spec_schema_features_linux_intel_rdt *make_runtime_spec_schema_features_linux_intel_rdt (yajl_val tree, const struct parser_context *ctx, parser_error *err);

yajl_gen_status gen_runtime_spec_schema_features_linux_intel_rdt (yajl_gen g, const runtime_spec_schema_features_linux_intel_rdt *ptr, const struct parser_context *ctx, parser_error *err);

typedef struct {
    char **namespaces;
    size_t namespaces_len;

    char **capabilities;
    size_t capabilities_len;

    runtime_spec_schema_features_linux_cgroup *cgroup;

    runtime_spec_schema_features_linux_seccomp *seccomp;

    runtime_spec_schema_features_linux_apparmor *apparmor;

    runtime_spec_schema_features_linux_selinux *selinux;

    runtime_spec_schema_features_linux_intel_rdt *intel_rdt;

    yajl_val _residual;
}
runtime_spec_schema_features_linux;

void free_runtime_spec_schema_features_linux (runtime_spec_schema_features_linux *ptr);

runtime_spec_schema_features_linux *make_runtime_spec_schema_features_linux (yajl_val tree, const struct parser_context *ctx, parser_error *err);

yajl_gen_status gen_runtime_spec_schema_features_linux (yajl_gen g, const runtime_spec_schema_features_linux *ptr, const struct parser_context *ctx, parser_error *err);

#ifdef __cplusplus
}
#endif

#endif

