#ifndef __MODULE_TEST_H__
#define __MODULE_TEST_H__

#define __forceinline __attribute__((always_inline))

#define FILE_TEST 1

#if defined(FILE_TEST)

    #define GET_MACRO(_1,_2,_3,NAME,...) NAME

    #//define TEST_FUNCTION_MAGIC_NUMBER 0xFDFDAAAA

    #define DECLARE_DEBUG_TEMP_VAR(MAGIC) void *__debug_temp_var##MAGIC = NULL
    #define EXTERN_DEBUG_TEMP_VAR(MAGIC) extern void* GET_DEBUG_TEMP_VAR(MAGIC)
    #define GET_DEBUG_TEMP_VAR(MAGIC) __debug_temp_var##MAGIC


    #define DEF_TEST_FUNCTION(FUNCTION_NAME, FUNCTION_RET_TYPE_DEF, FUNCTION_INPUT_ARGS_DEF, FUNCTION_CONTENT)\
        FUNCTION_RET_TYPE_DEF FUNCTION_NAME##_##0xFDFDAAAA FUNCTION_INPUT_ARGS_DEF FUNCTION_CONTENT
        

    #define CALL_TEST_FUNCTION(FUNCTION_NAME, FUNCTION_INPUT_ARGS) do{ FUNCTION_NAME##_##0xFDFDAAAA(FUNCTION_INPUT_ARGS) }while(0);


    #if defined(__cplusplus)
        #define LOG_TEST(X) do{ std::cout << "debug " << __FILE__ << ":" << __LINE__ << ":" << X << std::endl; }while(0);
    #else
        #define LOG_TEST(X) do{ printf("debug %s:%d: %s\n", __FILE__, __LINE__, X); }while(0);
    #endif

    #define ANOTATION_LOGIC_1(SITE)                   ANOTATION_LOGIC_IMPL(SITE, "", "")
    #define ANOTATION_LOGIC_2(SITE, NAME)             ANOTATION_LOGIC_IMPL(SITE, NAME, "")
    #define ANOTATION_LOGIC_3(SITE, NAME, CONTENT)    ANOTATION_LOGIC_IMPL(SITE, NAME, CONTENT)

    #define ANOTATION_LOGIC(...) GET_MACRO(__VA_ARGS__, ANOTATION_LOGIC_3, ANOTATION_LOGIC_2, ANOTATION_LOGIC_1)(__VA_ARGS__)
    #define ANOTATION_LOGIC_IMPL(SITE, NAME, CONTENT)

#else

    #define GET_MACRO(_1,_2,_3,NAME,...) NAME

    #define DECLARE_DEBUG_TEMP_VAR(MAGIC) 
    #define EXTERN_DEBUG_TEMP_VAR(MAGIC) 
    #define GET_DEBUG_TEMP_VAR(MAGIC) 

    #define DEF_TEST_FUNCTION(FUNCTION_NAME, FUNCTION_RET_TYPE_DEF, FUNCTION_INPUT_ARGS_DEF, FUNCTION_CONTENT) 
    #define CALL_TEST_FUNCTION(FUNTION_NAME, FUNCTION_INPUT_ARGS) do{ }while(0);

    #define LOG_TEST(X) do{ }while(0);

    #define ANOTATION_LOGIC_1(SITE)                   ANOTATION_LOGIC_IMPL(SITE, "", "")
    #define ANOTATION_LOGIC_2(SITE, NAME)             ANOTATION_LOGIC_IMPL(SITE, NAME, "")
    #define ANOTATION_LOGIC_3(SITE, NAME, CONTENT)    ANOTATION_LOGIC_IMPL(SITE, NAME, CONTENT)

    #define ANOTATION_LOGIC(...) GET_MACRO(__VA_ARGS__, ANOTATION_LOGIC_3, ANOTATION_LOGIC_2, ANOTATION_LOGIC_1)(__VA_ARGS__)
    #define ANOTATION_LOGIC_IMPL(SITE, NAME, CONTENT)

#endif





#endif