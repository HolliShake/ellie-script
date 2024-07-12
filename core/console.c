#include "console.h"

#define CONSOLE_MAX_PARAM 255

static JSValue console_log(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv) {
    if (argc > CONSOLE_MAX_PARAM) {
        return JS_ThrowTypeError(ctx, "expected 1 argument, got %d", argc);
    }

    for (int i = 0; i < argc; i++) {
        const char *str = JS_ToCString(ctx, argv[i]);
        if (!str) {
            return JS_EXCEPTION;
        }
        printf("%s", str);
        if (i < argc - 1) {
            printf(" ");
        }
        JS_FreeCString(ctx, str);
    }
    printf("\n");
    return JS_UNDEFINED;
}

static JSValue console_warn(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv) {
    if (argc > CONSOLE_MAX_PARAM) {
        return JS_ThrowTypeError(ctx, "expected 1 argument, got %d", argc);
    }

    for (int i = 0; i < argc; i++) {
        const char *str = JS_ToCString(ctx, argv[i]);
        if (!str) {
            return JS_EXCEPTION;
        }
        printf("\033[32m%s\033[0m", str);
        if (i < argc - 1) {
            printf(" ");
        }
        JS_FreeCString(ctx, str);
    }
    printf("\n");
    return JS_UNDEFINED;
}

static JSValue console_error(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv) {
    if (argc > CONSOLE_MAX_PARAM) {
        return JS_ThrowTypeError(ctx, "expected 1 argument, got %d", argc);
    }

    for (int i = 0; i < argc; i++) {
        const char *str = JS_ToCString(ctx, argv[i]);
        if (!str) {
            return JS_EXCEPTION;
        }
        fprintf(stderr, "\033[31m%s\033[0m", str);
        if (i < argc - 1) {
            printf(" ");
        }
        JS_FreeCString(ctx, str);
    }
    printf("\n");
    return JS_UNDEFINED;
}

static JSValue console_scan(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv) { 
    if (argc != 1) {
        return JS_ThrowTypeError(ctx, "expected 1 argument, got %d", argc);
    }

    char buffer[1024];
    const char* msg = JS_ToCString(ctx, argv[0]);
    printf("%s", msg);
    scanf("%[^\n]", buffer);
    return JS_NewString(ctx, buffer);
}

void register_console(JSContext *ctx) {
    // Console
    JSValue console = JS_NewObject(ctx);
        // console.log
        JSValueConst console_log_fn = JS_NewCFunction(ctx, console_log, "log", CONSOLE_MAX_PARAM);
        JS_SetPropertyStr(ctx, console, "log", console_log_fn);

        // console.warn
        JSValueConst console_warn_fn = JS_NewCFunction(ctx, console_warn, "warn", CONSOLE_MAX_PARAM);
        JS_SetPropertyStr(ctx, console, "warn", console_warn_fn);

        // console.error
        JSValueConst console_error_fn = JS_NewCFunction(ctx, console_error, "error", CONSOLE_MAX_PARAM);
        JS_SetPropertyStr(ctx, console, "error", console_error_fn);

        // console.scan
        JSValueConst console_scan_fn = JS_NewCFunction(ctx, console_scan, "scan", 1);
        JS_SetPropertyStr(ctx, console, "scan", console_scan_fn);

    // Global
    JSValue global = JS_GetGlobalObject(ctx);
    JS_SetPropertyStr(ctx, global, "console", console);
}