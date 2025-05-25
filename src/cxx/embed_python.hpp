#ifndef __EMBED_PYTHON_HPP__
    #define __EMBED_PYTHON_HPP__

    #include <Python.h>
    #include "module_test.h"

    EXTERN_DEBUG_TEMP_VAR(0xFDFDAAAA);

    #define USE_PYTHON
    #define USE_PYTHON_MODULES

    #if (defined(__cplusplus) && defined(USE_PYTHON)) || defined(USE_PYTHON_MODULES)

        #include <Python.h>


        #define PY_EXEC(code_obj) \
            ({ \
                PyObject *__mod = PyModule_New("__main__"); \
                PyObject *__dict = PyModule_GetDict(__mod); \
                PyEval_EvalCode(code_obj, __dict, __dict); \
                __mod; \
            })

        #define PY_COMPILE(code_str) \
            ({ \
                PyObject *__code = Py_CompileString(code_str, "<embedded>", Py_file_input); \
                __code ? __code : (PyErr_Print(), (PyObject*)NULL); \
            })

        #if 0
        #define PY_COMPILE(code_str) \
            ({ \
                PyObject *__code = Py_CompileString(code_str, "<embedded>", Py_file_input); \
                if (!__code) { \
                    PyErr_Print(); \
                    NULL; \
                } else { \
                    __code; \
                } \
            })
        
        #define PY_COMPILE(code_str) \
            (GET_DEBUG_TEMP_VAR(0xFDFDAAAA) = (void*)Py_CompileString(code_str, "<embedded>", Py_file_input)); \ 
            DEF_TEST_FUNCTION(debug_py_compile, void, (PyObject* py_object), { py_object?NULL:PyErr_Print();}); CALL_TEST_FUNCTION(debug_py_compile, (Py_Object*)GET_DEBUG_TEMP_VAR(0xFDFDAAAA)); \                                                                                                                                 
        
        #endif


        #define PY_GET_FUNC(mod_obj, func_name) \
            PyObject_GetAttrString(mod_obj, func_name)

        #define PY_CALL(func_obj, args) \
            PyObject_CallObject(func_obj, args)



        ANOTATION_LOGIC("PYTHON EMBEDDING", "PYTHON EMBEDDING", {
            /*
                This is the Python embedding module.
                It allows to embed Python code into C++ code.
                It is used to execute Python code in the C++ code.
            */

            /*

            const char* PYTHON_EMBEDDING_CODE = R"(
                import sys
                def any_drone_utility_func(args):
                    print('Hello from embedded Python!')
                    print('Arguments:', args)

                    return args
            )";

            Py_Initialize();

            PyObject *code = PY_COMPILE("def hello(name): print('Hello,', name)");
            PyObject *mod = PY_EXEC(code);
            PyObject *func = PY_GET_FUNC(mod, "hello");

            PyObject *args = PyTuple_Pack(1, PyUnicode_FromString("Pilot"));
            PY_CALL(func, args);

            Py_XDECREF(args);
            Py_XDECREF(func);
            Py_XDECREF(mod);
            Py_XDECREF(code);

            Py_Finalize();            
            */
            
            /*
                This module is used to embed Python code into C++ code.
                It allows to execute Python code in the C++ code.
                It is used to execute Python code in the C++ code.
            */

        })

    #endif





#endif