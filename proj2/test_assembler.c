#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <CUnit/Basic.h>

#include "src/utils.h"
#include "src/tables.h"
#include "src/translate_utils.h"
#include "src/translate.h"

const char* TMP_FILE = "test_output.txt";
const int BUF_SIZE = 1024;

/****************************************
 *  Helper functions 
 ****************************************/

int do_nothing() {
    return 0;
}

int init_log_file() {
    set_log_file(TMP_FILE);
    return 0;
}

int check_lines_equal(char **arr, int num) {
    char buf[BUF_SIZE];

    FILE *f = fopen(TMP_FILE, "r");
    if (!f) {
        CU_FAIL("Could not open temporary file");
        return 0;
    }
    for (int i = 0; i < num; i++) {
        if (!fgets(buf, BUF_SIZE, f)) {
            CU_FAIL("Reached end of file");
            return 0;
        }
        CU_ASSERT(!strncmp(buf, arr[i], strlen(arr[i])));
    }
    fclose(f);
    return 0;
}

/****************************************
 *  Test cases for translate_utils.c 
 ****************************************/

void test_translate_reg() {
    CU_ASSERT_EQUAL(translate_reg("$0"), 0);
    CU_ASSERT_EQUAL(translate_reg("$at"), 1);
    CU_ASSERT_EQUAL(translate_reg("$v0"), 2);
    CU_ASSERT_EQUAL(translate_reg("$a0"), 4);
    CU_ASSERT_EQUAL(translate_reg("$a1"), 5);
    CU_ASSERT_EQUAL(translate_reg("$a2"), 6);
    CU_ASSERT_EQUAL(translate_reg("$a3"), 7);
    CU_ASSERT_EQUAL(translate_reg("$t0"), 8);
    CU_ASSERT_EQUAL(translate_reg("$t1"), 9);
    CU_ASSERT_EQUAL(translate_reg("$t2"), 10);
    CU_ASSERT_EQUAL(translate_reg("$t3"), 11);
    CU_ASSERT_EQUAL(translate_reg("$s0"), 16);
    CU_ASSERT_EQUAL(translate_reg("$s1"), 17);
    CU_ASSERT_EQUAL(translate_reg("$3"), -1);
    CU_ASSERT_EQUAL(translate_reg("asdf"), -1);
    CU_ASSERT_EQUAL(translate_reg("hey there"), -1);
}

void test_translate_num() {
    long int output;

    CU_ASSERT_EQUAL(translate_num(&output, "35", -1000, 1000), 0);
    CU_ASSERT_EQUAL(output, 35);
    CU_ASSERT_EQUAL(translate_num(&output, "145634236", 0, 9000000000), 0);
    CU_ASSERT_EQUAL(output, 145634236);
    CU_ASSERT_EQUAL(translate_num(&output, "0xC0FFEE", -9000000000, 9000000000), 0);
    CU_ASSERT_EQUAL(output, 12648430);
    CU_ASSERT_EQUAL(translate_num(&output, "72", -16, 72), 0);
    CU_ASSERT_EQUAL(output, 72);
    CU_ASSERT_EQUAL(translate_num(&output, "72", -16, 71), -1);
    CU_ASSERT_EQUAL(translate_num(&output, "72", 72, 150), 0);
    CU_ASSERT_EQUAL(output, 72);
    CU_ASSERT_EQUAL(translate_num(&output, "72", 73, 150), -1);
    CU_ASSERT_EQUAL(translate_num(&output, "35x", -100, 100), -1);
}

/****************************************
 *  Test cases for tables.c 
 ****************************************/

void test_table_1() {
    int retval;

    SymbolTable* tbl = create_table(SYMTBL_UNIQUE_NAME);
    CU_ASSERT_PTR_NOT_NULL(tbl);

    retval = add_to_table(tbl, "abc", 8);
    CU_ASSERT_EQUAL(retval, 0);
    retval = add_to_table(tbl, "efg", 12);
    CU_ASSERT_EQUAL(retval, 0);
    retval = add_to_table(tbl, "q45", 16);
    CU_ASSERT_EQUAL(retval, 0);
    retval = add_to_table(tbl, "q45", 24); 
    CU_ASSERT_EQUAL(retval, -1); 
    retval = add_to_table(tbl, "bob", 14); 
    CU_ASSERT_EQUAL(retval, -1); 

    retval = get_addr_for_symbol(tbl, "abc");
    CU_ASSERT_EQUAL(retval, 8); 
    retval = get_addr_for_symbol(tbl, "q45");
    CU_ASSERT_EQUAL(retval, 16); 
    retval = get_addr_for_symbol(tbl, "ef");
    CU_ASSERT_EQUAL(retval, -1);
    
    free_table(tbl);

    char* arr[] = { "Error: name 'q45' already exists in table.",
                    "Error: address is not a multiple of 4." };
    check_lines_equal(arr, 2);

    SymbolTable* tbl2 = create_table(SYMTBL_NON_UNIQUE);
    CU_ASSERT_PTR_NOT_NULL(tbl2);

    retval = add_to_table(tbl2, "q45", 16);
    CU_ASSERT_EQUAL(retval, 0);
    retval = add_to_table(tbl2, "q45", 24); 
    CU_ASSERT_EQUAL(retval, 0);

    free_table(tbl2);

}

// something is wrong with resizing the table?
void test_table_2() { //tests resizing
    int retval, max = 100;

    SymbolTable* tbl = create_table(SYMTBL_UNIQUE_NAME);
    CU_ASSERT_PTR_NOT_NULL(tbl);

    char buf[10];
    for (int i = 0; i < max; i++) {
        sprintf(buf, "%d", i);
        retval = add_to_table(tbl, buf, 4 * i);
        CU_ASSERT_EQUAL(retval, 0);
    }
    // printf("hi3\n");

    for (int i = 0; i < max; i++) {
        sprintf(buf, "%d", i);
        retval = get_addr_for_symbol(tbl, buf);
        CU_ASSERT_EQUAL(retval, 4 * i);
    }
    free_table(tbl);
}

/****************************************
 *  Add your test cases here
 ****************************************/
void test_r_type() {
    int retval;
    const char* filename = "output.txt";
    FILE* output = fopen(filename, "w");
    char* addu = "addu";
    char* or = "or";
    char* slt = "slt";
    char* sltu = "sltu";
    // char* jr = "jr";
    char* args[3];
    args[0] = "$t0";
    args[1] = "$s0";
    args[2] = "$zero";

    char** ptr = args;

    // $zero, $at, $v0, $a0 - $a3, $t0 - $t3, $s0 - $s3, $sp, and $ra.
    /* Test Addu with several inputs: add things to a file first...*/
    retval = translate_inst(output, addu, ptr, 5, 0, NULL, NULL);
    CU_ASSERT_EQUAL(-1, retval);
    retval = translate_inst(output, addu, ptr, 3, 0, NULL, NULL);
    CU_ASSERT_EQUAL(0, retval);
    args[0] = "hello_World";
    retval = translate_inst(output, addu, ptr, 3, 0, NULL, NULL);
    CU_ASSERT_EQUAL(retval, -1);
    args[0] = "$t1"; args[1] = "$s1"; args[2] = "$t1";
    retval = translate_inst(output, addu, ptr, 3, 0, NULL, NULL);
    CU_ASSERT_EQUAL(retval, 0);
    args[0] = "$s1"; args[1] = "$s0"; args[2] = "$0";    
    retval = translate_inst(output, addu, ptr, 3, 0, NULL, NULL);
    CU_ASSERT_EQUAL(retval, 0);
    args[0] = "$t0"; args[1] = "$t3"; args[2] = "$t2"; // changed    
    retval = translate_inst(output, addu, ptr, 3, 0, NULL, NULL);
    CU_ASSERT_EQUAL(retval, 0);
    args[0] = "$s3"; args[1] = "$zero"; args[2] = "$t1";    
    retval = translate_inst(output, addu, ptr, 3, 0, NULL, NULL);
    CU_ASSERT_EQUAL(retval, 0);


    /* or tests */
    args[0] = "$v0"; args[1] = "$a0"; args[2] = "$a1";    
    retval = translate_inst(output, or, ptr, 3, 0, NULL, NULL);
    CU_ASSERT_EQUAL(retval, 0);

    args[0] = "$v0"; args[1] = "$a2"; args[2] = "$t0";    
    retval = translate_inst(output, or, ptr, 3, 0, NULL, NULL);
    CU_ASSERT_EQUAL(retval, 0);

    args[0] = "$t1"; args[1] = "$a0"; args[2] = "$s3";    
    retval = translate_inst(output, or, ptr, 3, 0, NULL, NULL);
    CU_ASSERT_EQUAL(retval, 0);

    // $zero, $at, $v0, $a0 - $a3, $t0 - $t3, $s0 - $s3, $sp, and $ra.
    /* slt tests */

    args[0] = "$v0"; args[1] = "$a1"; args[2] = "$a2";    
    retval = translate_inst(output, slt, ptr, 3, 0, NULL, NULL);
    CU_ASSERT_EQUAL(retval, 0);

    args[0] = "$v0"; args[1] = "$a3"; args[2] = "$t1";    
    retval = translate_inst(output, slt, ptr, 3, 0, NULL, NULL);
    CU_ASSERT_EQUAL(retval, 0);

    args[0] = "$t0"; args[1] = "$a0"; args[2] = "$zero";    
    retval = translate_inst(output, slt, ptr, 3, 0, NULL, NULL);
    CU_ASSERT_EQUAL(retval, 0);

    /* sltu tests */
    args[0] = "$t0"; args[1] = "$a1"; args[2] = "$v0";    
    retval = translate_inst(output, sltu, ptr, 3, 0, NULL, NULL);
    CU_ASSERT_EQUAL(retval, 0);

    args[0] = "$t1"; args[1] = "$zero"; args[2] = "$v0";    
    retval = translate_inst(output, sltu, ptr, 3, 0, NULL, NULL);
    CU_ASSERT_EQUAL(retval, 0);

    args[0] = "$v0"; args[1] = "$a3"; args[2] = "$t6";    
    retval = translate_inst(output, sltu, ptr, 3, 0, NULL, NULL);
    CU_ASSERT_EQUAL(retval, -1);

    // // jr tests
    // args[0] = "$v1"; args[1] = "$a2"; args[2] = "$a1";    
    // retval = translate_inst(output, addu, ptr, 3, 0, NULL, NULL);
    // CU_ASSERT_EQUAL(retval, 0);

    // args[0] = "$a1"; args[1] = "$a2"; args[2] = "$a3";    
    // retval = translate_inst(output, addu, ptr, 3, 0, NULL, NULL);
    // CU_ASSERT_EQUAL(retval, 0);

    // args[0] = "$t1"; args[1] = "$t2"; args[2] = "$t3";    
    // retval = translate_inst(output, addu, ptr, 3, 0, NULL, NULL);
    // CU_ASSERT_EQUAL(retval, 0);
    // $zero, $at, $v0, $a0 - $a3, $t0 - $t3, $s0 - $s3, $sp, and $ra.


    fclose(output);

    /* Test that the output is the same as in MARS: addu */
    fopen(filename, "r");
    char res[2000];
    CU_ASSERT_PTR_NOT_NULL(fgets(res, sizeof(res), output));
    CU_ASSERT(!strncmp(res,"02004021", strlen("02004021")));

    CU_ASSERT_PTR_NOT_NULL(fgets(res, sizeof(res), output));
    CU_ASSERT(!strncmp(res,"02294821", strlen("02294821")));

    CU_ASSERT_PTR_NOT_NULL(fgets(res, sizeof(res), output));
    CU_ASSERT(!strncmp(res,"02008821", strlen("02008821")));
    
    CU_ASSERT_PTR_NOT_NULL(fgets(res, sizeof(res), output));
    CU_ASSERT(!strncmp(res,"016a4021", strlen("016a4021")));
    
    CU_ASSERT_PTR_NOT_NULL(fgets(res, sizeof(res), output));
    CU_ASSERT(!strncmp(res,"00099821", strlen("00099821")));
    
    // CU_ASSERT_PTR_NOT_NULL(fgets(res, sizeof(res), output));

    /* Tests that the output is the same as in MARS: or*/
    CU_ASSERT_PTR_NOT_NULL(fgets(res, sizeof(res), output));
    CU_ASSERT(!strncmp(res,"00851025", strlen("00851025")));

    CU_ASSERT_PTR_NOT_NULL(fgets(res, sizeof(res), output));
    CU_ASSERT(!strncmp(res,"00c81025", strlen("00c81025")));

    CU_ASSERT_PTR_NOT_NULL(fgets(res, sizeof(res), output));
    CU_ASSERT(!strncmp(res,"00934825", strlen("00934825")));
    

    /* Tests that the output is the same as in MARS: slt*/
    CU_ASSERT_PTR_NOT_NULL(fgets(res, sizeof(res), output));
    CU_ASSERT(!strncmp(res,"00a6102a", strlen("00a6102a")));

    CU_ASSERT_PTR_NOT_NULL(fgets(res, sizeof(res), output));
    CU_ASSERT(!strncmp(res,"00e9102a", strlen("00e9102a")));

    CU_ASSERT_PTR_NOT_NULL(fgets(res, sizeof(res), output));
    CU_ASSERT(!strncmp(res,"0080402a", strlen("0080402a")));

    /* Tests that the output is the same as in MARS: sltu*/
    CU_ASSERT_PTR_NOT_NULL(fgets(res, sizeof(res), output));
    CU_ASSERT(!strncmp(res,"00a2402b", strlen("00a2402b")));

    CU_ASSERT_PTR_NOT_NULL(fgets(res, sizeof(res), output));
    CU_ASSERT(!strncmp(res,"0002482b", strlen("0002482b")));

    // CU_ASSERT_PTR_NOT_NULL(fgets(res, sizeof(res), output));
    // CU_ASSERT(!strncmp(res,"00e7182b", strlen("00e7182b")));

    CU_ASSERT_PTR_NULL(fgets(res, sizeof(res), output));

    /* Tests that the output is the same as in MARS: jr*/
    // none yet // what type is jr... hmmm
    CU_ASSERT(feof(output));



    fclose(output);
    unlink(filename);

}

void test_ori() {
    int retval;
    const char* filename = "ori.txt";
    FILE* output = fopen(filename, "w");
    char* ori = "ori";
    char* args[3];
    args[0] = "$t0";
    args[1] = "$s0";
    args[2] = "$t1";

    char** ptr = args;

    // $zero, $at, $v0, $a0 - $a3, $t0 - $t3, $s0 - $s3, $sp, and $ra.
    /* Test ori with several inputs: add things to a file first...*/
    retval = translate_inst(output, ori, ptr, 5, 0, NULL, NULL);
    CU_ASSERT_EQUAL(-1, retval);
    retval = translate_inst(output, ori, ptr, 3, 0, NULL, NULL );
    CU_ASSERT_EQUAL(-1, retval);
    args[0] = "$t0"; args[1] = "$s0"; args[2] = "10";
    retval = translate_inst(output, ori, ptr, 3, 0, NULL, NULL);
    CU_ASSERT_EQUAL(0, retval);
    // args[0] = "$t1"; args[1] = "$s3"; args[2] = "-15";
    // retval = translate_inst(output, ori, ptr, 3, NULL, NULL);
    // CU_ASSERT_EQUAL(-1, retval);
    args[0] = "$t1"; args[1] = "$s3"; args[2] = "15";
    retval = translate_inst(output, ori, ptr, 3,  0,NULL, NULL);
    CU_ASSERT_EQUAL(0, retval);

     args[0] = "$s1"; args[1] = "$0"; args[2] = "25";
    retval = translate_inst(output, ori, ptr, 3, 0, NULL, NULL);
    CU_ASSERT_EQUAL(0, retval);


    fclose(output);

    fopen(filename, "r");
    char res[2000];
    // 4e08000a
    // 4e69000f
    // 4c110019

    CU_ASSERT_PTR_NOT_NULL(fgets(res, sizeof(res), output));
    CU_ASSERT(!strncmp(res,"3608000a", strlen("3608000a")));

    CU_ASSERT_PTR_NOT_NULL(fgets(res, sizeof(res), output));
    CU_ASSERT(!strncmp(res,"3669000f", strlen("3669000f")));    
    
    CU_ASSERT_PTR_NOT_NULL(fgets(res, sizeof(res), output));
    CU_ASSERT(!strncmp(res,"34110019", strlen("34110019")));

    CU_ASSERT_PTR_NULL(fgets(res, sizeof(res), output));


    CU_ASSERT(feof(output));
    fclose(output);
    unlink(filename);
}



void test_lui() {
    int retval;
    const char* filename = "lui.txt";
    FILE* output = fopen(filename, "w");
    char* lui = "lui";
    char* args[3];
    args[0] = "$t0";
    args[1] = "$1";

    char** ptr = args;
    retval = translate_inst(output, lui, ptr, 5, 0, NULL, NULL);
    CU_ASSERT_EQUAL(-1, retval);
    retval = translate_inst(output, lui, ptr, 2, 0, NULL, NULL);
    args[0] = "$t0"; args[1] = "5";
    retval = translate_inst(output, lui, ptr, 2, 0, NULL, NULL);
    CU_ASSERT_EQUAL(0, retval);

    args[0] = "$s1"; args[1] = "27";
    retval = translate_inst(output, lui, ptr, 2, 0, NULL, NULL);
    CU_ASSERT_EQUAL(0, retval);

    args[0] = "$v0"; args[1] = "-6";
    retval = translate_inst(output, lui, ptr, 2, 0, NULL, NULL);

    CU_ASSERT_EQUAL(-1, retval);


    fclose(output);

    fopen(filename, "r");
    char res[2000];
    CU_ASSERT_PTR_NOT_NULL(fgets(res, sizeof(res), output));
    CU_ASSERT(!strncmp(res,"3c080005", strlen("3c080005")));

    CU_ASSERT_PTR_NOT_NULL(fgets(res, sizeof(res), output));
    CU_ASSERT(!strncmp(res,"3c11001b", strlen("3c11001b")));    
    

    CU_ASSERT_PTR_NULL(fgets(res, sizeof(res), output));

    CU_ASSERT(feof(output));
    fclose(output);
    unlink(filename);



}


// void test_jr() {
//     CU_ASSERT_EQUAL(-1, 0);
// }


void test_shift() {
    const char* filename = "sll.txt";
    FILE* output = fopen(filename, "w");
    char* sll= "sll";
    char* args[3];
    args[0] = "$t0";
    args[1] = "$t1";
    args[2] = "$3";
    char** ptr = args;

    int retval;
    retval = translate_inst(output, sll, ptr, 3, 0, NULL, NULL);
    CU_ASSERT_EQUAL(-1, retval);

    args[0] = "$s1"; args[1] = "$v0";  args[2] = "4";
    retval = translate_inst(output, sll, ptr, 3, 0, NULL, NULL);
    CU_ASSERT_EQUAL(0, retval);

    args[0] = "$t2"; args[1] = "$t3"; args[2] = "-3";
    retval = translate_inst(output, sll, ptr, 3, 0, NULL, NULL);
    CU_ASSERT_EQUAL(-1, retval);

    args[0] = "$t1"; args[1] = "$t0"; args[2] = "27";
    retval = translate_inst(output, sll, ptr, 3, 0, NULL, NULL);
    CU_ASSERT_EQUAL(0, retval);
    retval = -1;
    // sltu
    args[0] = "$t2"; args[1] = "$t3"; args[2] = "3";
    retval = translate_inst(output, sll, ptr, 3, 0, NULL, NULL);
    CU_ASSERT_EQUAL(0, retval);

    fclose(output);

    fopen(filename, "r");
    char res[2000];
    CU_ASSERT_PTR_NOT_NULL(fgets(res, sizeof(res), output));
    CU_ASSERT(!strncmp(res,"00028900", strlen("00028900")));

    CU_ASSERT_PTR_NOT_NULL(fgets(res, sizeof(res), output));
    CU_ASSERT(!strncmp(res,"00084ec0", strlen("00084ec0")));    
    
    CU_ASSERT_PTR_NOT_NULL(fgets(res, sizeof(res), output));
    CU_ASSERT(!strncmp(res,"000b50c0", strlen("000b50c0")));

    CU_ASSERT_PTR_NULL(fgets(res, sizeof(res), output));

    CU_ASSERT(feof(output));
    fclose(output);
    unlink(filename);
}

void test_Mem() {
 // lb, lbu, lw, sb, sw
    const char* filename = "mem.txt";
    FILE* output = fopen(filename, "w");

    char* lb = "lb";
    char* lbu = "lbu";
    char* lw = "lw";
    char* sb = "sb";
    char* sw = "sw";

    char* args[3];
    args[0] = "$t0";
    args[1] = "$5";
    args[2] = "$s3";
    char** ptr = args;
    int retval;

    retval = translate_inst(output, lb, ptr, 3, 0, NULL, NULL);
    CU_ASSERT_EQUAL(-1, retval);

    args[0] = "$t2"; args[1] = "3"; args[2] = "$s0";
    retval = translate_inst(output, lb, ptr, 3, 0, NULL, NULL);
    CU_ASSERT_EQUAL(0, retval);

    // lbu $rt, offset($rs)
    args[0] = "$t1"; args[1] = "1"; args[2] = "$t0";
    retval = translate_inst(output, lbu, ptr, 3, 0, NULL, NULL);
    CU_ASSERT_EQUAL(0, retval);


    
    args[0] = "$t2"; args[1] = "2"; args[2] = "$t3";
    retval = translate_inst(output, lbu, ptr, 3, 0, NULL, NULL);
    CU_ASSERT_EQUAL(0, retval);

    // lw $rt, offset($rs)
    args[0] = "$t1"; args[1] = "10"; args[2] = "$v0";
    retval = translate_inst(output, lw, ptr, 3, 0, NULL, NULL);
    CU_ASSERT_EQUAL(0, retval);

    
    args[0] = "$t3"; args[1] = "30"; args[2] = "$t3";
    retval = translate_inst(output, lw, ptr, 3, 0, NULL, NULL);
    CU_ASSERT_EQUAL(0, retval);
    // sb $rt, offset($rs)
    args[0] = "$s1"; args[1] = "21"; args[2] = "$t1";
    retval = translate_inst(output, sb, ptr, 3, 0, NULL, NULL);
    CU_ASSERT_EQUAL(0, retval);

    
    args[0] = "$v0"; args[1] = "13"; args[2] = "$t3";
    retval = translate_inst(output, sb, ptr, 3, 0, NULL, NULL);
    CU_ASSERT_EQUAL(0, retval);

    // sw $rt, offset($rs)
    args[0] = "$t2"; args[1] = "4"; args[2] = "$0";
    retval = translate_inst(output, sw, ptr, 3, 0, NULL, NULL);
    CU_ASSERT_EQUAL(0, retval);

    
    args[0] = "$t2"; args[1] = "5"; args[2] = "$t1";
    retval = translate_inst(output, sw, ptr, 3, 0, NULL, NULL);
    CU_ASSERT_EQUAL(0, retval);


    args[0] = "$t1"; args[1] = "-5"; args[2] = "$t0";
    retval = translate_inst(output, sw, ptr, 3, 0, NULL, NULL);
    CU_ASSERT_EQUAL(0, retval);

    fclose(output);
    fopen(filename, "r");
    char res[20000];
    CU_ASSERT_PTR_NOT_NULL(fgets(res, sizeof(res), output));
    CU_ASSERT(!strncmp(res,"820a0003", strlen("820a0003")));

    CU_ASSERT_PTR_NOT_NULL(fgets(res, sizeof(res), output));
    CU_ASSERT(!strncmp(res,"91090001", strlen("91090001")));    
    
    CU_ASSERT_PTR_NOT_NULL(fgets(res, sizeof(res), output));
    CU_ASSERT(!strncmp(res,"916a0002", strlen("916a0002")));

    CU_ASSERT_PTR_NOT_NULL(fgets(res, sizeof(res), output));
    CU_ASSERT(!strncmp(res,"8c49000a", strlen("8c49000a")));

    CU_ASSERT_PTR_NOT_NULL(fgets(res, sizeof(res), output));
    CU_ASSERT(!strncmp(res,"8d6b001e", strlen("8d6b001e")));    
    
    CU_ASSERT_PTR_NOT_NULL(fgets(res, sizeof(res), output));
    CU_ASSERT(!strncmp(res,"a1310015", strlen("a1310015")));

    CU_ASSERT_PTR_NOT_NULL(fgets(res, sizeof(res), output));
    CU_ASSERT(!strncmp(res,"a162000d", strlen("a162000d")));

    CU_ASSERT_PTR_NOT_NULL(fgets(res, sizeof(res), output));
    CU_ASSERT(!strncmp(res,"ac0a0004", strlen("ac0a0004")));    
    
    CU_ASSERT_PTR_NOT_NULL(fgets(res, sizeof(res), output));
    CU_ASSERT(!strncmp(res,"ad2a0005", strlen("ad2a0005")));

    
    CU_ASSERT_PTR_NOT_NULL(fgets(res, sizeof(res), output));
    CU_ASSERT(!strncmp(res,"ad09fffb", strlen("ad09fffb")));

    CU_ASSERT_PTR_NULL(fgets(res, sizeof(res), output));

    CU_ASSERT(feof(output));
    fclose(output);
    unlink(filename);

}

void test_div_mul() {

/*
MARS input/ testing for:
mult $s1, $s2
mult $t1, $v0

div $t1, $t0
div $v0, $s3

*/

    const char* filename = "divmul.txt";
    FILE* output = fopen(filename, "w");
    char* multF = "mult";
    char* divF = "div";
    char* args[3];
    args[0] = "$s1";
    args[1] = "$s2";
    char** ptr = args;

    int retval;
    retval = translate_inst(output, multF, ptr, 2, 0, NULL, NULL);
    CU_ASSERT_EQUAL(0, retval);

    args[0] = "$t1"; args[1] = "$v0"; 
    retval = translate_inst(output, multF, ptr, 2, 0, NULL, NULL);
    CU_ASSERT_EQUAL(0, retval);

    args[0] = "$t1"; args[1] = "$t0"; 
    retval = translate_inst(output, divF, ptr, 2, 0, NULL, NULL);
    CU_ASSERT_EQUAL(0, retval);

    args[0] = "$s1"; args[1] = "$s3"; 
    retval = translate_inst(output, divF, ptr, 2, 0, NULL, NULL);
    CU_ASSERT_EQUAL(0, retval);
    fclose(output);

    fopen(filename, "r");
    char res[2000];
    CU_ASSERT_PTR_NOT_NULL(fgets(res, sizeof(res), output));
    CU_ASSERT(!strncmp(res,"02320018", strlen("02320018")));

    CU_ASSERT_PTR_NOT_NULL(fgets(res, sizeof(res), output));
    CU_ASSERT(!strncmp(res,"01220018", strlen("01220018")));

    CU_ASSERT_PTR_NOT_NULL(fgets(res, sizeof(res), output));
    CU_ASSERT(!strncmp(res,"0128001a", strlen("0128001a")));

    CU_ASSERT_PTR_NOT_NULL(fgets(res, sizeof(res), output));
    CU_ASSERT(!strncmp(res,"0233001a", strlen("0233001a")));

    CU_ASSERT_PTR_NULL(fgets(res, sizeof(res), output));

    CU_ASSERT(feof(output));
    fclose(output);
    unlink(filename);



}

void test_mfhi_mflo() {
    /*
    mfhi $0 // 
    mfhi $t1
    mfhi $a3

    mflo $v0
    mflo $a1
    */

    const char* filename = "mfhilo.txt";
    FILE* output = fopen(filename, "w");
    char* mfhi = "mfhi";
    char* mflo = "mflo";
    char* args[3];
    args[0] = "$s0";
    char** ptr = args;

    int retval;
    retval = translate_inst(output, mfhi, ptr, 1, 0, NULL, NULL);
    CU_ASSERT_EQUAL(0, retval);

    retval = translate_inst(output, mfhi, ptr, 3, 0, NULL, NULL);
    CU_ASSERT_EQUAL(-1, retval);

    args[0] = "$a3";
    retval = translate_inst(output, mfhi, ptr, 1, 0, NULL, NULL);
    CU_ASSERT_EQUAL(0, retval);

    args[0] = "$t1"; 
    retval = translate_inst(output, mfhi, ptr, 1, 0, NULL, NULL);
    CU_ASSERT_EQUAL(0, retval);

    args[0] = "$v0"; 
    retval = translate_inst(output, mflo, ptr, 1, 0, NULL, NULL);
    CU_ASSERT_EQUAL(0, retval);

    args[0] = "$a1"; 
    retval = translate_inst(output, mflo, ptr, 1, 0, NULL, NULL);
    CU_ASSERT_EQUAL(0, retval);
    fclose(output);

    fopen(filename, "r");
    char res[2000];
    CU_ASSERT_PTR_NOT_NULL(fgets(res, sizeof(res), output));
    CU_ASSERT(!strncmp(res,"00008010", strlen("00008010")));

    CU_ASSERT_PTR_NOT_NULL(fgets(res, sizeof(res), output));
    CU_ASSERT(!strncmp(res,"00003810", strlen("00003810")));

    CU_ASSERT_PTR_NOT_NULL(fgets(res, sizeof(res), output));
    CU_ASSERT(!strncmp(res,"00004810", strlen("00004810")));

    CU_ASSERT_PTR_NOT_NULL(fgets(res, sizeof(res), output));
    CU_ASSERT(!strncmp(res,"00001012", strlen("00001012")));

    CU_ASSERT_PTR_NOT_NULL(fgets(res, sizeof(res), output));
    CU_ASSERT(!strncmp(res,"00002812", strlen("00002812")));

    CU_ASSERT_PTR_NULL(fgets(res, sizeof(res), output));

    CU_ASSERT(feof(output));
    fclose(output);
    unlink(filename);

}


// void test_branch() {
    
//     CU_ASSERT_EQUAL(-1, 0);

// }

// void test_jump() {
//     CU_ASSERT_EQUAL(-1, 0);

// }

void test_addiu() {
    const char* filename = "addiu.txt";
    FILE* output = fopen(filename, "w");
    char* addiu = "addiu";
    char* args[3];
    args[0] = "$t0";
    args[1] = "$t1";
    args[2] = "$3";
    char** ptr = args;

    int retval;
    retval = translate_inst(output, addiu, ptr, 3, 0, NULL, NULL);
    CU_ASSERT_EQUAL(-1, retval);

    args[0] = "$s1"; args[1] = "$v0";  args[2] = "4";
    retval = translate_inst(output, addiu, ptr, 3, 0, NULL, NULL);
    CU_ASSERT_EQUAL(0, retval);

    args[0] = "$t0"; args[1] = "$t1";  args[2] = "-56";
    retval = translate_inst(output, addiu, ptr, 3, 0, NULL, NULL);
    CU_ASSERT_EQUAL(0, retval);
    fclose(output);



    fopen(filename, "r");
    char res[2000];
    CU_ASSERT_PTR_NOT_NULL(fgets(res, sizeof(res), output));
    CU_ASSERT(!strncmp(res,"24510004", strlen("24510004")));

     CU_ASSERT_PTR_NOT_NULL(fgets(res, sizeof(res), output));
    CU_ASSERT(!strncmp(res,"2528ffc8", strlen("2528ffc8")));

    CU_ASSERT_PTR_NULL(fgets(res, sizeof(res), output));



    CU_ASSERT(feof(output));
    fclose(output);
    unlink(filename);

}

/////
int main(int argc, char** argv) {
    CU_pSuite pSuite1 = NULL, pSuite2 = NULL;
    CU_pSuite part3Suite = NULL;

    if (CUE_SUCCESS != CU_initialize_registry()) {
        return CU_get_error();
    }

    /* Suite 1 */
    pSuite1 = CU_add_suite("Testing translate_utils.c", NULL, NULL);
    if (!pSuite1) {
        goto exit;
    }
    if (!CU_add_test(pSuite1, "test_translate_reg", test_translate_reg)) {
        goto exit;
    }
    if (!CU_add_test(pSuite1, "test_translate_num", test_translate_num)) {
        goto exit;
    }

    /* Suite 2 */
    pSuite2 = CU_add_suite("Testing tables.c", init_log_file, NULL);
    if (!pSuite2) {
        goto exit;
    }
    if (!CU_add_test(pSuite2, "test_table_1", test_table_1)) {
        goto exit;
    }
    if (!CU_add_test(pSuite2, "test_table_2", test_table_2)) {
        goto exit;
    }

    /* Test Part 3*/
    part3Suite = CU_add_suite("Testing Part 3", NULL, NULL);
    if (!part3Suite) {
        goto exit;
    }
    if (!CU_add_test(part3Suite, "test_r_type", test_r_type)) {
        goto exit;
    }
    if (!CU_add_test(part3Suite, "test_ori", test_ori)) {
        goto exit;
    }    
    if (!CU_add_test(part3Suite, "test_lui", test_lui)) {
        goto exit;
    }    
    // if (!CU_add_test(part3Suite, "test_jr", test_jr)) {
        // goto exit;
    // }    
    if (!CU_add_test(part3Suite, "test_shift", test_shift)) {
        goto exit;
    }    
    if (!CU_add_test(part3Suite, "test_Mem", test_Mem)) {
        goto exit;
    }
    // if (!CU_add_test(part3Suite, "test_branch", test_branch)) {
    //     goto exit;
    // }    
    // if (!CU_add_test(part3Suite, "test_jump", test_jump)) {
    //     goto exit;
    // }    
    if (!CU_add_test(part3Suite, "test_addiu", test_addiu)) {
        goto exit;
    }
    if (!CU_add_test(part3Suite, "test_div_mul", test_div_mul)) {
        goto exit;
    }
    if (!CU_add_test(part3Suite, "test_mfhi_mflo", test_mfhi_mflo)) {
        goto exit;
    }   
        

    CU_basic_set_mode(CU_BRM_VERBOSE);
    CU_basic_run_tests();

exit:
    CU_cleanup_registry();
    return CU_get_error();;
}