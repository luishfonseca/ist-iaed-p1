/*
 * File:			constants.h
 * Author:			Luís, 99266
 * Description:	Header file containing all strings and "magic numbers".
*/

/* OK return exit code */
#define EXIT_OK 0

/* Keep program running or stop it */
#define KEEP_GOING 1
#define STOP 0

/* String comparisons */
#define EQUAL 0

/* Maximum amount of tasks stored. */
#define AMT_TASKS 10000
/* Maximum size for the descripion string of a task. */
#define TASK_DESCRIPTION_SZ 51

/* Maximum amount of users stored. */
#define AMT_USERS 50
/* Maximum size for the user string. */
#define USER_SZ 21

/* Maximum amount of activities stored. */
#define AMT_ACTIVITIES 10
/* Maxium size for the activity string. */
#define ACTIVITY_SZ 21

/* Names of the default activities. */
#define STR_TO_DO "TO DO"
#define STR_IN_PROGRESS "IN PROGRESS"
#define STR_DONE "DONE"

/* Success and failure messages for adding a new task. */
#define STR_SUCCESS_NEW_TASK "task %d\n"
#define STR_FAIL_NEW_TASK_TOO_MANY_TASKS "too many tasks\n"
#define STR_FAIL_NEW_TASK_DUPLICATE_DESCRIPTION "duplicate description\n"
#define STR_FAIL_NEW_TASK_INVALID_DURATION "invalid duration\n"

/* Success and failure messages for listing tasks. */
#define STR_SUCCESS_LIST_TASKS "%d %s #%d %s\n"
#define STR_FAIL_LIST_TASKS_NO_SUCH_TASK "%d: no such task\n"

/* Success and failure messages for advancing time. */
#define STR_SUCCESS_ADVANCE_TIME "%u\n"
#define STR_FAIL_ADVANCE_TIME_INVALID_TIME "invalid time\n"

/* Success and failure messages for adding a new user. */
#define STR_FAIL_NEW_USER_USER_ALREADY_EXISTS "user already exists\n"
#define STR_FAIL_NEW_USER_TOO_MANY_USERS "too many users\n"

/* Success message for listing users. */
#define STR_SUCCESS_LIST_USERS "%s\n"

/* Success and failure messages for moving a task. */
#define STR_SUCCESS_MOVE_TASK_TO_DONE "duration=%d slack=%d\n"
#define STR_FAIL_MOVE_TASK_NO_SUCH_TASK "no such task\n"
#define STR_FAIL_MOVE_TASK_TASK_ALREADY_STARTED "task already started\n"
#define STR_FAIL_MOVE_TASK_NO_SUCH_USER "no such user\n"
#define STR_FAIL_MOVE_TASK_NO_SUCH_ACTIVITY "no such activity\n"

/* Success and failure messages for displaying an activity. */
#define STR_SUCCESS_DISPLAY_ACTIVITY "%d %u %s\n"
#define STR_FAIL_DISPLAY_ACTIVITY_NO_SUCH_ACTIVITY "no such activity\n"

/* Success and failure messages for adding a new activity. */
#define STR_SUCCESS_LIST_ACTIVITIES "%s\n"
#define STR_FAIL_NEW_ACTIVITY_DUPLICATE_ACTIVITY "duplicate activity\n"
#define STR_FAIL_NEW_ACTIVITY_INVALID_DESCRIPTION "invalid description\n"
#define STR_FAIL_NEW_ACTIVITY_TOO_MANY_ACTIVITIES "too many activities\n"

/* Match strings for scanf */
#define STR_MATCH_NEW_TASK "%d%*[ ]%[^\n]"
#define STR_MATCH_SINGLE_TASK_ID "%d"
#define STR_MATCH_TIME_INCREMENT "%d"
#define STR_MATCH_NEW_USER "%s"
#define STR_MATCH_TASK_MOVE "%d%s%*[ ]%[^\n]"
#define STR_MATCH_ACTIVITY "%[^\n]"
