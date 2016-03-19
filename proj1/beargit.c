#include <stdio.h>
#include <string.h>

#include <unistd.h>
#include <sys/stat.h>

#include "beargit.h"
#include "util.h"

/* Implementation Notes:
 *
 * - Functions return 0 if successful, 1 if there is an error.
 * - All error conditions in the function description need to be implemented
 *   and written to stderr. We catch some additional errors for you in main.c.
 * - Output to stdout needs to be exactly as specified in the function description.
 * - Only edit this file (beargit.c)
 * - Here are some of the helper functions from util.h:
 *   * fs_mkdir(dirname): create directory <dirname>
 *   * fs_rm(filename): delete file <filename>
 *   * fs_mv(src,dst): move file <src> to <dst>, overwriting <dst> if it exists
 *   * fs_cp(src,dst): copy file <src> to <dst>, overwriting <dst> if it exists
 *   * write_string_to_file(filename,str): write <str> to filename (overwriting contents)
 *   * read_string_from_file(filename,str,size): read a string of at most <size> (incl.
 *     NULL character) from file <filename> and store it into <str>. Note that <str>
 *     needs to be large enough to hold that string.
 *  - You NEED to test your code. The autograder we provide does not contain the
 *    full set of tests that we will run on your code. See "Step 5" in the project spec.
 */

/* beargit init
 *
 * - Create .beargit directory
 * - Create empty .beargit/.index file
 * - Create .beargit/.prev file containing 0..0 commit id
 *
 * Output (to stdout):
 * - None if successful
 */

 int beargit_init(void) {
    fs_mkdir(".beargit");

    FILE* findex = fopen(".beargit/.index", "w");
    fclose(findex);

    FILE* fbranches = fopen(".beargit/.branches", "w");
    fprintf(fbranches, "%s\n", "master");
    fclose(fbranches);

    write_string_to_file(".beargit/.prev", "0000000000000000000000000000000000000000");
    write_string_to_file(".beargit/.current_branch", "master");

    return 0;
 }



/* beargit add <filename>
 *
 * - Append filename to list in .beargit/.index if it isn't in there yet
 *
 * Possible errors (to stderr):
 * >> ERROR:  File <filename> has already been added.
 *
 * Output (to stdout):
 * - None if successful
 */

 int beargit_add(const char* filename) {
    FILE* findex = fopen(".beargit/.index", "r");
    FILE *fnewindex = fopen(".beargit/.newindex", "w");

    char line[FILENAME_SIZE];
    while(fgets(line, sizeof(line), findex)) {
        strtok(line, "\n");
        if (strcmp(line, filename) == 0) {
            fprintf(stderr, "ERROR:  File %s has already been added.\n", filename);
            fclose(findex);
            fclose(fnewindex);
            fs_rm(".beargit/.newindex");
            return 3;
        }

        fprintf(fnewindex, "%s\n", line);
    }

    fprintf(fnewindex, "%s\n", filename);
    fclose(findex);
    fclose(fnewindex);

    fs_mv(".beargit/.newindex", ".beargit/.index");

    return 0;
 }

/* beargit status
 *
 * See "Step 1" in the project spec.
 *
 */

 int beargit_status() {
  /* COMPLETE THE REST */
    int num_Files = 0; /* Starts the counter for number of files */
    FILE* findex = fopen(".beargit/.index", "r");
    char line[FILENAME_SIZE];

    fprintf(stdout, "Tracked files:\n\n"); /* Header */ 
    // fprintf("\n");
    /* Here we print the files... Increase counter for each file*/
    while (fgets(line, sizeof(line), findex)) {
        fprintf(stdout, "%s", line);
        num_Files += 1;
    }
    fprintf(stdout, "\n"); /* Empty Space */

    fprintf(stdout, "There are %d files total.\n", num_Files); /* This is the f inal line */
    fclose(findex);
    return 0;
 }



/* beargit rm <filename>
 *
 * See "Step 2" in the project spec.
 *
 */
 int beargit_rm(const char* filename) {
    /* COMPLETE THE REST */
    FILE* findex = fopen(".beargit/.index", "r");
    FILE *fnewindex = fopen(".beargit/.newindex", "w");

    int linecount = 0;
    char line[FILENAME_SIZE];
    int ok = 0;
    while (fgets(line, sizeof(line), findex)) {
        strtok(line, "\n");
        if (strcmp(filename, line) != 0) {
            fprintf(fnewindex, "%s\n", line);
        } 

        if (strcmp(filename, line) == 0) {
            ok = 1;
        }


    }

    if (!ok) {
        fprintf(stderr, "ERROR:  File %s not tracked.\n", filename);
    }


    fclose(findex);
    fclose(fnewindex);
    fs_mv(".beargit/.newindex", ".beargit/.index");


    return 0;
 }

/* beargit commit -m <msg>
 *
 * See "Step 3" in the project spec.
 *
 */
 const char* go_bears = "THIS IS BEAR TERRITORY!";
 const char* no_bears = "ERROR:  Message must contain 'THIS IS BEAR TERRITORY!' yeeee";
 int is_commit_msg_ok(const char* msg) {
  /* COMPLETE THE REST */
    int m = 0;
    // char* message = &msg[0];
    while (msg[m] != go_bears[0]) {
        m++;
        if (msg[m] == '\0') {
            return 0;
        }
    }

    int ok = 0;
    while (!ok) {
        int b;
        int sum = 0;
        if (msg[m] == '\0') {
            ok = 1;
            break;
        }
        for (b = 0; go_bears[b] != '\0' || msg[m] != '\0';b++) {
            if (msg[m] == go_bears[b]) {
                sum++;
            }
            else {
                break;
            }
            if (sum == 23) {
                ok = 1;
                return 1;
            }
            m++;
        }
        m++;

    }
    return 0;
 }


/* Use next_commit_id to fill in the rest of the commit ID.
 *
 * Hints:
 * You will need a destination string buffer to hold your next_commit_id, before you copy it back to commit_id
 * You will need to use a function we have provided for you. (cryptohash)
 */
 const char* head_error = "ERROR:  Need to be on HEAD of a branch to commit.";

 void next_commit_id(char* commit_id) {
    /* Reads the current branch and then uses cryptohash to create a spankin-new hash code*/

    char* final = commit_id;

    char commit_id_branch[COMMIT_ID_BRANCH_BYTES];
    read_string_from_file(".beargit/.current_branch", commit_id_branch, COMMIT_ID_BRANCH_BYTES);




    cryptohash(strcat(commit_id, commit_id_branch), final);
    commit_id = final;


 } 

 int beargit_commit(const char* msg) {
    if (!is_commit_msg_ok(msg)) {
        fprintf(stderr, "ERROR:  Message must contain \"%s\"\n", go_bears);
        return 1;
    }

    char commit_id[COMMIT_ID_SIZE];
    char commit_check[COMMIT_ID_BRANCH_BYTES];
    read_string_from_file(".beargit/.prev", commit_id, COMMIT_ID_SIZE);
    read_string_from_file(".beargit/.current_branch", commit_check, COMMIT_ID_BRANCH_BYTES);

    if (strlen(commit_check) == 0) {
        fprintf(stderr, "%s\n", head_error);
        return 1;

    }    next_commit_id(commit_id);


    /* Initialize character arrays to be able to print into them using sprintf*/
    char new_dir[100];
    char new_prev[100];
    char new_index[100];
    char new_msg[100];
    // char copy_dir[55 + FILENAME_SIZE];

    /* Creates the strings needed to create the new folders */
    sprintf(new_dir, "%s%s", ".beargit/", commit_id);
    sprintf(new_prev, "%s%s%s", ".beargit/", commit_id, "/.prev");
    sprintf(new_index, "%s%s%s", ".beargit/", commit_id, "/.index");
    sprintf(new_msg, "%s%s%s", ".beargit/", commit_id, "/.msg");
    /* Creates <ID> dir and then opens up files to copy onto them */
    fs_mkdir(new_dir);

    /* Begins process of copying .prev et al */
    fs_cp(".beargit/.index", new_index);
    fs_cp(".beargit/.prev", new_prev);

    write_string_to_file(new_msg ,msg);
    write_string_to_file(".beargit/.prev", commit_id);


    /* Copying of files from current directory to the .beargit/<ID> directory*/
    FILE* findex = fopen(".beargit/.index", "r"); 
    char curr_index_file[FILENAME_SIZE];
    while (fgets(curr_index_file, sizeof(curr_index_file), findex)) {
        strtok(curr_index_file, "\n");

        char copy_dir[55 + FILENAME_SIZE];
        sprintf(copy_dir, "%s%s/%s", ".beargit/", commit_id, curr_index_file);

        fs_cp(curr_index_file, copy_dir);
    }
    fclose(findex);


    return 0;
 }
/* Implementation Notes:
 *
 * - Functions return 0 if successful, 1 if there is an error.
 * - All error conditions in the function description need to be implemented
 *   and written to stderr. We catch some additional errors for you in main.c.
 * - Output to stdout needs to be exactly as specified in the function description.
 * - Only edit this file (beargit.c)
 * - Here are some of the helper functions from util.h:
 *   * fs_mkdir(dirname): create directory <dirname>
 *   * fs_rm(filename): delete file <filename>
 *   * fs_mv(src,dst): move file <src> to <dst>, overwriting <dst> if it exists
 *   * fs_cp(src,dst): copy file <src> to <dst>, overwriting <dst> if it exists
 *   * write_string_to_file(filename,str): write <str> to filename (overwriting contents)
 *   * read_string_from_file(filename,str,size): read a string of at most <size> (incl.
 *     NULL character) from file <filename> and store it into <str>. Note that <str>
 *     needs to be large enough to hold that string.
 *  - You NEED to test your code. The autograder we provide does not contain the
 *    full set of tests that we will run on your code. See "Step 5" in the project spec.
 */





/* beargit log
 *
 * See "Step 4" in the project spec.
 *
 */

 int beargit_log(int limit) {
    // char 
    char* last_commit = "0000000000000000000000000000000000000000";
    char previous_commt[COMMIT_ID_SIZE];
    char curr_msg[MSG_SIZE];

    char msg_file[COMMIT_ID_SIZE + 100];
    char prev_file[COMMIT_ID_SIZE + 50];
 
    char curr_folder[COMMIT_ID_SIZE + 20];
    read_string_from_file(".beargit/.prev",previous_commt, COMMIT_ID_SIZE);
    int i = 0;
    sprintf(curr_folder, ".beargit/%s", previous_commt);

    while (strcmp(previous_commt, last_commit)  && i != limit) {

        sprintf(msg_file, ".beargit/%s/.msg", previous_commt);
        read_string_from_file(msg_file, curr_msg, MSG_SIZE);

        fprintf(stdout, "commit %s\n", previous_commt);
        fprintf(stdout, "   %s\n\n", curr_msg);
        // printf("\n");
        sprintf(prev_file, ".beargit/%s/.prev", previous_commt);
        sprintf(curr_folder, ".beargit/%s", previous_commt);

        read_string_from_file(prev_file ,previous_commt,COMMIT_ID_SIZE);
        i += 1;

    }
    if (!fs_check_dir_exists(curr_folder) && i == 0) {
        fprintf(stderr, "%s\n", "ERROR:  There are no commits.");
        return 1;
    }

    return 0;
 }


// This helper function returns the branch number for a specific branch, or
// returns -1 if the branch does not exist.
 int get_branch_number(const char* branch_name) {
    FILE* fbranches = fopen(".beargit/.branches", "r");

    int branch_index = -1;
    int counter = 0;
    char line[FILENAME_SIZE];
    while(fgets(line, sizeof(line), fbranches)) {
        strtok(line, "\n");
        if (strcmp(line, branch_name) == 0) {
            branch_index = counter;
        }
        counter++;
    }

    fclose(fbranches);

    return branch_index;
 }

/* beargit branch
 *
 * See "Step 5" in the project spec.
 * Not much changes: every commit still 1 predecessor
 *    Pointers to specific commits
 *    ONLY commit when at head of the branch
 *    In another commit? You may go there.
 * .beargit/.branches -> contains a line for each branch
 * .beargit/.current_branch -> current branch if we are at 
 *       the head of some branch X. Empty if not on a head.
 *  .beargit/.branch_<name> -> one for every branch
 *        copy of PREV file that belongs to thebranch head (the 
 *        head commit of the branch)
 * Beargit branch prints all the branches and puts a star 
 *      in front of the current one. Do you remember beargit 
 *      status? This is almost the same: you need to read the 
 *      entire .branches file line by line and output it. 
 *      However, you also need to check each line against the 
 *      string in .current_branch. If they are the same, you 
 *      need to print a * in front of it.
 * Note that we require you to print branches in the order 
 *      of creation, from oldest to latest. Also note that if 
 *      you have checked out a commit previously (in contrast 
 *      to a branch), you are detached from the HEAD and don't 
 *      have to print a star in front of any branch. This is 
 *      even true if the commit you checked out is actually the 
 *      HEAD of a branch.
 * If there are commits, you should produce the output 
 *      indicated in the "Output to stdout" section above and 
 *      return 0.
 */

 int beargit_branch() {
  /* COMPLETE THE REST */
    FILE* fbranches = fopen(".beargit/.branches", "r");

    char line[BRANCHNAME_SIZE];
    char current_branch[BRANCHNAME_SIZE];
    read_string_from_file(".beargit/.current_branch", current_branch, BRANCHNAME_SIZE);
    strtok(current_branch, "\n");

    while(fgets(line, sizeof(line), fbranches)) {
        strtok(line, "\n");
        if (strlen(current_branch) == 0) {
            fprintf(stdout, "   %s\n", line );
        } 
        if (strlen(current_branch) != 0) {
            if (strcmp(line, current_branch) == 0) {
                fprintf(stdout, "*  %s\n", line);
            } else {
                fprintf(stdout, "   %s\n", line );
            }
        }
    }
    fclose(fbranches);


    return 0;
 }


/* beargit checkout
 *
 * See "Step 6" in the project spec.
 * 
 * Does the actual checking out 
 √ Going through the index of the current index file, delete all those files (in the current directory; i.e., the directory   
    where we ran beargit).
 * Copy the index from the commit that is being checked out to the .beargit directory, and use it to copy all that commit's 
    tracked files from the commit's directory into the current directory.
 * Write the ID of the commit that is being checked out into .prev.
 √ In the special case that the new commit is the 00.0 commit, there are no files to copy and there is no index. Instead empty 
     the index (but still write the ID into .prev and delete the current index files). You may wonder how we could ever check 
     out the 00.0 commit, since it is not a valid commit ID; the answer is that if you check out a branch whose HEAD is the 
     000 commit, that checkout is expected to work (while 00.0 would not be recognized as a commit ID).

 */
int checkout_commit(const char* commit_id) {
    /* COMPLETE THE REST */
    // int special = 0;
    char special_checker[COMMIT_ID_SIZE + 20];
    sprintf(special_checker, ".beargit/%s", commit_id);
    int special = fs_check_dir_exists(special_checker);
    // (strcmp(commit_id, "0000000000000000000000000000000000000000") == 0);
    /* Clearrrrrrring*/
    if (!special) {
        fs_rm(".beargit/.index");
        FILE *nindex = fopen(".beargit/.index", "w");
        write_string_to_file(".beargit/.prev", commit_id);
        // write_string_to_file(".beargit/.index", ""); //ERROR M8
        fclose(nindex);
        return 0;

    }
    FILE *findex = fopen(".beargit/.index", "r");
    char curr_file[FILENAME_SIZE];
    while (fgets(curr_file, sizeof(curr_file), findex)) {
        strtok(curr_file, "\n");
        fs_rm(curr_file);
    }
    fclose(findex);

    /* Special Case*/


    /* Copy the index from commit being checked out -->*/
    char commit_directory[100];
    char index_directory[100];

    sprintf(commit_directory, ".beargit/%s/", commit_id);
    sprintf(index_directory, "%s.index", commit_directory);

    fs_cp(index_directory, ".beargit/.index");


    /* copy all the tracked files into the curr directory*/
    char file_dir[80];
    findex = fopen(".beargit/.index", "r");
    
    while (fgets(curr_file, sizeof(curr_file), findex)) {
        strtok(curr_file, "\n");

        if (strlen(curr_file) != 0) {
            strcpy(file_dir, commit_directory);
            fs_cp(strcat(file_dir, curr_file), curr_file);

        }
    }
    fclose(findex);

    
    /* Write the ID of the commit that is being checked out into .prev ayyyyyye*/
    write_string_to_file(".beargit/.prev", commit_id);
    return 0;
}

int is_it_a_commit_id(const char* commit_id) {
  /* COMPLETE THE REST */
  char string_to_check[COMMIT_ID_SIZE + BRANCHNAME_SIZE + 10];
  sprintf(string_to_check, ".beargit/%s", commit_id);
  if (!fs_check_dir_exists(string_to_check)) {
    return 0;
  }
  return 1;

 
}

// ERROR 1: "current_branch" string literal instead of the variable
// ERROR 2: char* branch_file = ".beargit/.branch_";
int beargit_checkout(const char* arg, int new_branch) {
    // Get the current branch

    if (strlen(arg) == 0) {
        return 1;
    }
    // int branch_exists = (get_branch_number(branch_name) >= 0);
    char current_branch[BRANCHNAME_SIZE + 50];
    read_string_from_file(".beargit/.current_branch", current_branch, BRANCHNAME_SIZE);
    // If not detached, leave the current branch by storing the current HEAD into that branch's file...
    if (strlen(current_branch)) {
        char current_branch_file[BRANCHNAME_SIZE+50];
        sprintf(current_branch_file, ".beargit/.branch_%s", current_branch);
        fs_cp(".beargit/.prev", current_branch_file);
    }

    // Check whether the argument is a commit ID. If yes, we just change to detached mode
    // without actually having to change into any other branch.
    
    if (is_it_a_commit_id(arg)) {
        char commit_dir[FILENAME_SIZE + 10] = ".beargit/";
        strcat(commit_dir, arg);
    // ...and setting the current branch to none (i.e., detached).
        write_string_to_file(".beargit/.current_branch", "");

        return checkout_commit(arg);
    } 

    // Read branches file (giving us the HEAD commit id for that branch).
    int branch_exists = (get_branch_number(arg) >= 0);
    // printf("bus error or nah 1 %d\n" branch_exists);

    // Check for errors.
    if (!(!branch_exists || !new_branch)) {
        fprintf(stderr, "ERROR:  A branch named %s already exists.\n", arg);
        return 1;
    } else if (!branch_exists && !is_it_a_commit_id(arg) && !new_branch) {
        fprintf(stderr, "ERROR:  No branch or commit %s exists.\n", arg);
        return 1;
    }

    // Just a better name, since we now know the argument is a branch name.
    char branch_name[strlen(arg)];
    char branch_file[BRANCHNAME_SIZE + 20];
    sprintf(branch_name, "%s", arg);
    sprintf(branch_file, "%s%s", ".beargit/.branch_", branch_name);



    // Update the branch file if new branch is created (now it can't go wrong anymore)
    if (new_branch) {
        FILE* fbranches = fopen(".beargit/.branches", "a");
        fprintf(fbranches, "%s\n", branch_name);
        fclose(fbranches);
        fs_cp(".beargit/.prev", branch_file);
    }

    write_string_to_file(".beargit/.current_branch", branch_name);

    // Read the head commit ID of this branch.
    char branch_head_commit_id[COMMIT_ID_SIZE];
    read_string_from_file(branch_file, branch_head_commit_id, COMMIT_ID_SIZE);

    // Check out the actual commit.
    return checkout_commit(branch_head_commit_id);
}

/* beargit reset
 *
 * See "Step 7" in the project spec.
 *
 */
int beargit_reset(const char* commit_id, const char* filename) {
    if (!is_it_a_commit_id(commit_id)) {
        fprintf(stderr, "ERROR:  Commit %s does not exist.\n", commit_id);
        return 1;
    }
    char reset_directory[COMMIT_ID_SIZE + 20];
    sprintf(reset_directory, ".beargit/%s/.index", commit_id);


    // Check if the file is in the commit directory
    /* COMPLETE THIS PART */
    FILE* rindex = fopen(reset_directory, "r");
    char line[FILENAME_SIZE];
    int found = 0;
    while (fgets(line, sizeof(line), rindex))  {
        strtok(line, "\n");
        if (strcmp(line, filename) == 0) {
            found += 1;
        }
    }
    if (found == 0) {
        fprintf(stdout, "ERROR:  %s is not in the index of commit %s.\n", filename, commit_id);
        return 1;
    }
    fclose(rindex);

    // Copy the file to the current working directory
    /* COMPLETE THIS PART */
    char src[FILENAME_SIZE + COMMIT_ID_SIZE + 10];
    sprintf(src, ".beargit/%s/%s", commit_id, filename);
    fs_cp(src ,filename);

    // Add the file if it wasn't already there
    /* COMPLETE THIS PART */
    int in_cindex = 0;
    FILE* cindex = fopen(".beargit/.index", "r");
    while (fgets(line, sizeof(line), cindex)) {
        strtok(line, "\n");
        if (strcmp(line, filename)) {
            in_cindex += 1;
        }
    }
    fclose(cindex);

    if (in_cindex == 0) {
        FILE* mindex = fopen(".beargit/.index", "a");
        fprintf(mindex, "%s\n", filename);
        fclose(mindex);
    }

    return 0;
 }

/* beargit merge
 *
 * See "Step 8" in the project spec.
 * 
 */

 int beargit_merge(const char* arg) {
    // Get the commit_id or throw an error
    char commit_id[COMMIT_ID_SIZE]; // so this already co
    if (!is_it_a_commit_id(arg)) {
        if (get_branch_number(arg) == -1) {
            fprintf(stderr, "ERROR:  No branch or commit %s exists.\n", arg);
            return 1;
        }
        char branch_file[FILENAME_SIZE];
        snprintf(branch_file, FILENAME_SIZE, ".beargit/.branch_%s", arg);
        read_string_from_file(branch_file, commit_id, COMMIT_ID_SIZE);
    } else {
        snprintf(commit_id, COMMIT_ID_SIZE, "%s", arg);
    }

    // commit_id already has the head of the branch/commit id. 
    // since we already have the desired one, we can just iterate trough index 
    // yay

    // Iterate through each line of the commit_id index and determine how you
    // should copy the index file over
    /* COMPLETE THE REST */
    char dest_dir[FILENAME_SIZE + COMMIT_ID_SIZE + 10];
    char src_dir[FILENAME_SIZE+ COMMIT_ID_SIZE + 10];
    char commit_line[FILENAME_SIZE + 20];
    char wd_line[FILENAME_SIZE + 20];

    char directory[COMMIT_ID_SIZE + 20];

    sprintf(directory, ".beargit/%s/.index", commit_id);
    // sprintf(src_dir, ".beargit/%s/", commit_id);
    FILE* commit_index = fopen(directory,"r");
            int exists = 0;

    while (fgets(commit_line, sizeof(commit_line), commit_index)) {
        exists = 0;
        // printf("%d\n", exists );
        strtok(commit_line, "\n");

        FILE* wd_index = fopen(".beargit/.index", "r");
        while (fgets(wd_line, sizeof(wd_line), wd_index)) {
            strtok(wd_line, "\n");
            if (strcmp(commit_line, wd_line) == 0) {

                exists = 1;
            }
        }

        fclose(wd_index);

        sprintf(src_dir, ".beargit/%s/%s", commit_id, commit_line);
        if (!exists) {
            sprintf(dest_dir, "%s", commit_line);
            fprintf(stdout, "%s added\n", commit_line );
            fs_cp(src_dir, dest_dir);
            FILE* altindex = fopen(".beargit/.index", "a");
            fprintf(altindex, "%s\n", commit_line);
            fclose(altindex);

        } else if (exists) {

            sprintf(dest_dir, "%s.%s", commit_line, commit_id);
            fprintf(stdout, "%s conflicted copy created\n", commit_line);

            fs_cp(src_dir, dest_dir);
        }
        // fclose(wd_index);


    }
    
    fclose(commit_index);



    return 0;
}











