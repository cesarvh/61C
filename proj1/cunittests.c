#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <CUnit/Basic.h>
#include "beargit.h"
#include "util.h"

/* printf/fprintf calls in this tester will NOT go to file. */

#undef printf
#undef fprintf

/* The suite initialization function.
 * You'll probably want to delete any leftover files in .beargit from previous
 * tests, along with the .beargit directory itself.
 *
 * You'll most likely be able to share this across suites.
 */
int init_suite(void)
{
    // preps to run tests by deleting the .beargit directory if it exists
    fs_force_rm_beargit_dir();
    unlink("TEST_STDOUT");
    unlink("TEST_STDERR");
    return 0;
}

/* You can also delete leftover files after a test suite runs, but there's
 * no need to duplicate code between this and init_suite 
 */
int clean_suite(void)
{
    return 0;
}

/* Simple test of fread().
 * Reads the data previously written by testFPRINTF()
 * and checks whether the expected characters are present.
 * Must be run after testFPRINTF().
 */
void simple_sample_test(void)
{
    // This is a very basic test. Your tests should likely do more than this.
    // We suggest checking the outputs of printfs/fprintfs to both stdout
    // and stderr. To make this convenient for you, the tester replaces
    // printf and fprintf with copies that write data to a file for you
    // to access. To access all output written to stdout, you can read 
    // from the "TEST_STDOUT" file. To access all output written to stderr,
    // you can read from the "TEST_STDERR" file.
    int retval;
    retval = beargit_init();
    CU_ASSERT(0==retval);
    retval = beargit_add("asdf.txt");
    CU_ASSERT(0==retval);
}

struct commit {
  char msg[MSG_SIZE];
  struct commit* next;
};


void free_commit_list(struct commit** commit_list) {
  if (*commit_list) {
    free_commit_list(&((*commit_list)->next));
    free(*commit_list);
  }

  *commit_list = NULL;
}

void run_commit(struct commit** commit_list, const char* msg) {
    int retval = beargit_commit(msg);
    CU_ASSERT(0==retval);

    struct commit* new_commit = (struct commit*)malloc(sizeof(struct commit));
    new_commit->next = *commit_list;
    strcpy(new_commit->msg, msg);
    *commit_list = new_commit;
}

void simple_log_test(void)
{
    struct commit* commit_list = NULL;
    int retval;
    retval = beargit_init();
    CU_ASSERT(0==retval);
    FILE* asdf = fopen("asdf.txt", "w");
    fclose(asdf);
    retval = beargit_add("asdf.txt");
    CU_ASSERT(0==retval);
    run_commit(&commit_list, "THIS IS BEAR TERRITORY!1");
    run_commit(&commit_list, "THIS IS BEAR TERRITORY!2");
    run_commit(&commit_list, "THIS IS BEAR TERRITORY!3");

    retval = beargit_log(10);
    CU_ASSERT(0==retval);

    struct commit* cur_commit = commit_list;

    const int LINE_SIZE = 512;
    char line[LINE_SIZE];

    FILE* fstdout = fopen("TEST_STDOUT", "r");
    CU_ASSERT_PTR_NOT_NULL(fstdout);

    while (cur_commit != NULL) {
      char refline[LINE_SIZE];

      // First line is commit -- don't check the ID.
      CU_ASSERT_PTR_NOT_NULL(fgets(line, LINE_SIZE, fstdout));
      CU_ASSERT(!strncmp(line,"commit", strlen("commit")));

      // Second line is msg
      sprintf(refline, "   %s\n", cur_commit->msg);
      CU_ASSERT_PTR_NOT_NULL(fgets(line, LINE_SIZE, fstdout));
      CU_ASSERT_STRING_EQUAL(line, refline);

      // Third line is empty
      CU_ASSERT_PTR_NOT_NULL(fgets(line, LINE_SIZE, fstdout));
      CU_ASSERT(!strcmp(line,"\n"));

      cur_commit = cur_commit->next;
    }

    CU_ASSERT_PTR_NULL(fgets(line, LINE_SIZE, fstdout));

    // It's the end of output
    CU_ASSERT(feof(fstdout));
    fclose(fstdout);

    free_commit_list(&commit_list);
}

void merge_case() {
  struct commit* commit_list = NULL;
  int retval;
  retval = beargit_init();
  CU_ASSERT(0 == retval);

  FILE* a = fopen("a.txt", "w");
  FILE* b = fopen("b.txt", "w");
  FILE* c = fopen("c.txt", "w");
  FILE* d = fopen("d.txt", "w");
  fclose(a); 
  fclose(b);
  fclose(c);
  fclose(d);

  retval =beargit_add("a.txt");
  CU_ASSERT(0 == retval);
  run_commit(&commit_list, "THIS IS BEAR TERRITORY! a");
  retval = beargit_add("b.txt");
  CU_ASSERT(0 == retval);
  run_commit(&commit_list, "THIS IS BEAR TERRITORY! b");
  retval = beargit_add("c.txt");
  CU_ASSERT(0 == retval);
  run_commit(&commit_list, "THIS IS BEAR TERRITORY! c");
  retval = beargit_add("d.txt");
  CU_ASSERT(0 == retval);
  run_commit(&commit_list, "THIS IS BEAR TERRITORY! d");

  retval = beargit_checkout("new", 1);
  CU_ASSERT(0 == retval);
  FILE* e = fopen("e.txt", "w");
  fclose(e);
  retval = beargit_add("e.txt");
  CU_ASSERT(0 == retval);
  run_commit(&commit_list, "THIS IS BEAR TERRITORY! e");

  retval = beargit_merge("new");
  CU_ASSERT(0 == retval);

  FILE* fstdout = fopen("TEST_STDOUT", "r");
  char res[512];

    CU_ASSERT_PTR_NOT_NULL(fgets(res,sizeof(res), fstdout));
    CU_ASSERT(!strncmp(res,"a.txt conflicted copy created", strlen("a.txt conflicted copy created")));
    CU_ASSERT_PTR_NOT_NULL(fgets(res,sizeof(res), fstdout));
    CU_ASSERT(!strncmp(res,"b.txt conflicted copy created", strlen("b.txt conflicted copy created")));    CU_ASSERT_PTR_NOT_NULL(fgets(res,sizeof(res), fstdout));
    CU_ASSERT(!strncmp(res,"c.txt conflicted copy created", strlen("c.txt conflicted copy created")));    CU_ASSERT_PTR_NOT_NULL(fgets(res,sizeof(res), fstdout));
    CU_ASSERT(!strncmp(res,"d.txt conflicted copy created", strlen("d.txt conflicted copy created")));

    CU_ASSERT_PTR_NULL(fgets(res, sizeof(res), fstdout));

    // It's the end of output
    CU_ASSERT(feof(fstdout));
    fclose(fstdout);
    free_commit_list(&commit_list);

}

void case_1_checkout(void)
{
    struct commit* commit_list = NULL;
    int retval;

    retval = beargit_init();
    CU_ASSERT(0==retval);

    FILE* a = fopen("a.txt", "w");
    fclose(a);

    retval = beargit_add("a.txt");
    CU_ASSERT(0==retval);

    run_commit(&commit_list, "THIS IS BEAR TERRITORY!1");

    beargit_checkout("ciel", 1);     

    FILE* c = fopen("c.txt", "w");
    fclose(c);

    beargit_add("c.txt");

    run_commit(&commit_list, "THIS IS BEAR TERRITORY!2");

    beargit_checkout("master", 0);
    // At this point in the test, we are back in the master branch,
    // which should only have a.txt and b.txt. If we get a null pointer back,
    // that means c.txt does not exist here, which is what we want.
    CU_ASSERT_PTR_NULL(fopen("c.txt", "r"));
    
    free_commit_list(&commit_list);
}

/* The main() function for setting up and running the tests.
 * Returns a CUE_SUCCESS on successful running, another
 * CUnit error code on failure.
 */
int cunittester()
{
   CU_pSuite pSuite = NULL;
   CU_pSuite pSuite2 = NULL;
   CU_pSuite merge_suite = NULL;
   CU_pSuite checkout_suite = NULL;


   /* initialize the CUnit test registry */
   if (CUE_SUCCESS != CU_initialize_registry())
      return CU_get_error();

   /* add a suite to the registry */
   pSuite = CU_add_suite("Suite_1", init_suite, clean_suite);
   if (NULL == pSuite) {
      CU_cleanup_registry();
      return CU_get_error();
   }

   /* Add tests to the Suite #1 */
   if (NULL == CU_add_test(pSuite, "Simple Test #1", simple_sample_test))
   {
      CU_cleanup_registry();
      return CU_get_error();
   }

   pSuite2 = CU_add_suite("Suite_2", init_suite, clean_suite);
   if (NULL == pSuite2) {
      CU_cleanup_registry();
      return CU_get_error();
   }

   /* Add tests to the Suite #2 */
   if (NULL == CU_add_test(pSuite2, "Log output test", simple_log_test))
   {
      CU_cleanup_registry();
      return CU_get_error();
   }

   /* add a suite to the registry */
   merge_suite = CU_add_suite("merge_suite", init_suite, clean_suite);
   if (NULL == merge_suite) {
      CU_cleanup_registry();
      return CU_get_error();
   }

   /* Add tests to the Checkout Suite */
   if (NULL == CU_add_test(merge_suite, "Case 1 Test", merge_case))
   {
      CU_cleanup_registry();
      return CU_get_error();
   }
   /* add a suite to the registry */
   checkout_suite = CU_add_suite("checkout_suite", init_suite, clean_suite);
   if (NULL == checkout_suite) {
      CU_cleanup_registry();
      return CU_get_error();
   }

   /* Add tests to the Checkout Suite */
   if (NULL == CU_add_test(checkout_suite, "Case 1 Test", case_1_checkout))
   {
      CU_cleanup_registry();
      return CU_get_error();
   }
   /* Run all tests using the CUnit Basic interface */
   CU_basic_set_mode(CU_BRM_VERBOSE);
   CU_basic_run_tests();
   CU_cleanup_registry();
   return CU_get_error();



}

