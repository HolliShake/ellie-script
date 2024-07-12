#include <stdio.h>
#include <stdlib.h>
#include <stdlib.h>
#include "src/global.h"
#include "core/console.h"
#include "utfc/utfc.h"
#include "quickjs/quickjs-libc.h"
#include "quickjs/quickjs.h"

struct ErrorPosition {
    int32_t line;
    int32_t colm;
};

static struct ErrorPosition parse_error_location(const char* error_from_stack) {
    const char open  = '(';
    const char next  = '<';
    const char close = ')';

    char* buffer = str__new("");

    char chr, *old;
    for (int i = 0; (chr = str__char_code_at((char*) error_from_stack, i)) != '\0'; i++) {
        if (chr == open) {
            if (str__char_code_at((char*) error_from_stack, i + 1) != next) {
                continue;
            }
            int j = 0;
            while ((chr = str__char_code_at((char*) error_from_stack, i++)) != close) {
                buffer = str__concat_char(old = buffer, chr);
                Ellie_free(old);
                j++;
            }
            buffer = str__concat_char(old = buffer, ')');
            Ellie_free(old);
            break;
        }
    }

    int start = 0;
    int index = 0;
    int ldone = 0;

    char* number = str__new("");

    struct ErrorPosition data = (struct ErrorPosition) {
        .line = 0,
        .colm = 0
    };

    while (index < str__length(buffer)) {
        if ((!start) && str__char_code_at(buffer, index) == ':') {
            start = 1;
            ldone = 0;
            ++index;
            continue;
        }
        else if (start && str__char_code_at(buffer, index) == ':') {
            start = 1;
            ldone = 1;
            Ellie_free(number);
            number = str__new("");
            ++index;
            continue;
        }
        
        if (start) {
            int empty_index = 0;
            int j = index;
            char current = str__char_code_at(buffer, j);
            while (j < str__length(buffer) && (current >= '0' && current <= '9')) {
                number = str__concat_char(number, current);
                current = str__char_code_at(buffer, ++j);
            }
            if (!ldone) {
                data.line = atoi(number);
            } else {
                data.colm = atoi(number);
            }

            index = j;

            if (ldone) {
                break;
            }
        }
        else {
            index ++;
        }
    }
    
    return data;
}

static void ellie_init_globals(JSContext *ctx) {
    register_console(ctx);
}

static JSContext *JS_NewCustomContext(JSRuntime *rt)
{
    JSContext *ctx = JS_NewContextRaw(rt);
    if (!ctx)
        return NULL;
    JS_AddIntrinsicBaseObjects(ctx);
    JS_AddIntrinsicDate(ctx);
    JS_AddIntrinsicEval(ctx);
    JS_AddIntrinsicJSON(ctx);
    JS_AddIntrinsicMapSet(ctx);
    JS_AddIntrinsicTypedArrays(ctx);
    JS_AddIntrinsicPromise(ctx);
    JS_AddIntrinsicBigInt(ctx);
    JS_AddIntrinsicWeakRef(ctx);
    JS_AddPerformance(ctx);
    return ctx;
}

#include "src/generator.h"

int main(int argc, char *argv[]) {
    GC_INIT();

    if (argc != 2) {
        fprintf(stderr, "Usage: %s <filename>\n", argv[0]);
        return 2;
    }

    char* file_pathp = argv[1];
    char* file_datap = fileio__read_file(file_pathp);

    generator_t* g = generator_create(file_pathp, file_datap);
    char* ellie_code = generator_generate(g);

    JSRuntime *runtime = JS_NewRuntime();
    if (!runtime) {
        fprintf(stderr, "Failed to initialize QuickJS runtime\n");
        return 1;
    }

    js_std_set_worker_new_context_func(JS_NewCustomContext);
    js_std_init_handlers(runtime);
    JS_SetModuleLoaderFunc(runtime, NULL, js_module_loader, NULL);

    JSContext *context = JS_NewCustomContext(runtime);
    if (!context) {
        fprintf(stderr, "Failed to initialize QuickJS context.\n");
        JS_FreeRuntime(runtime);
        return 1;
    }

    // Initialize global objects
    ellie_init_globals(context);
    
    JSValue eresult = JS_Eval(context, ellie_code, strlen(ellie_code), str__format("<%s>", file_pathp), JS_EVAL_TYPE_MODULE);
            eresult = js_std_await(context, eresult);
    int32_t result_int;

    js_std_loop(context);

    // Check for exceptions
    if (JS_IsException(eresult)) {
        JSValue exception = JS_GetException(context);
        JSValue stack;
        int32_t err_ln, err_cl;

        if (JS_IsObject(exception)) {
            struct ErrorPosition ep = parse_error_location(JS_ToCString(context, (stack = JS_GetPropertyStr(context, exception, "stack"))));
            err_ln = ep.line;
            err_cl = ep.colm;
            JS_FreeValue(context, stack);
        }

        const char *error = JS_ToCString(context, exception);
        fprintf(stderr, "[%s:%d:%d] Error: %s.\n", file_pathp, err_ln, err_cl, error);
        result_int = 1;
        JS_FreeCString(context, error);
        JS_FreeValue(context, exception);
    } else {
        if (JS_ToInt32(context, &result_int, eresult)) {
            fprintf(stderr, "Failed to convert result to int.\n");
        } else {
            printf("Program finished with exit code 0x%x.\n", result_int);
        }
    }

    // Free resources
    js_std_free_handlers(runtime);
    JS_FreeValue(context, eresult);
    JS_FreeContext(context);
    JS_FreeRuntime(runtime);
    GC_gcollect();
    printf("Done.");
    return result_int;
}