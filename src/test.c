
#ifndef NULL
    #define NULL ((void *)0)
#endif


#include "./cxx/ai_functional.hpp"
#include "./cxx/embed_python.hpp"


int main()
{
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
}