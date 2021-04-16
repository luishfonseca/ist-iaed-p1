/*
 * File:			main.c
 * Author:			Luís, 99266
 * Description:	Implementation of Kanban for IAED 2021 Project 1.
 */


/******************************************************************************
 * INCLUDES                                                                   *
 ******************************************************************************/

#include <stdio.h>
#include <string.h>
#include <ctype.h>

/* Include constant strings and magic numbers */
#include "constants.h"


/******************************************************************************
 * STRUCTS                                                                    *
 ******************************************************************************/

/*
 * TASK
 * Represents a task in the kanban.
 * - FIELDS:
 *   - description string.
 *   - user that owns the task.
 *   - activity that the task is in.
 *   - expected duration of the task.
 *   - moment task was started.
 */
typedef struct {
	char description[TASK_DESCRIPTION_SZ];
	char user[USER_SZ];
	char activity[ACTIVITY_SZ];
	int duration;
	unsigned int start;
} Task;

/*
 * USER LIST
 * Keeps track of all users in the kanban.
 * - FIELDS:
 *   - user[]: list of all user strings in the kanban.
 *   - amount: amount of users in the list.
 */
typedef struct {
	char user[AMT_USERS][USER_SZ];
	int amount;
} UserList;

/*
 * ACTIVITY LIST
 * Keeps track of all activities in the kanban.
 * - FIELDS:
 *   - activity[]: list of all activity strings in the kanban.
 *   - amount: amount of activities in the list.
 */
typedef struct {
	char activity[AMT_ACTIVITIES][ACTIVITY_SZ];
	int amount;
} ActivityList;

/*
 * TASK LIST
 * Keeps track of all tasks in the kanban.
 * - FIELDS:
 *   - task[]: list of all tasks in the kanban.
 *   - ordered_by_description[]: vector of task indices ordered by description.
 *   - amount: amount of tasks in the list.
 *   - ordered_by_start[]: vector of task indices ordered by start time.
 *   - first_at_current_time: index of first task to be started after time was advanced.
 *   - amount_started: amount of task in the list that have been started.
 */
typedef struct {
	Task task[AMT_TASKS];
	int ordered_by_description[AMT_TASKS];
	int amount;
	int ordered_by_start[AMT_TASKS];
	int first_at_current_time;
	int amount_started;
} TaskList;

/*
 * KANBAN
 * Keeps track of the global state of the kanban.
 * - FIELDS:
 *   - now: current time.
 *   - users: user list.
 *   - activities: activity list.
 *   - tasks: task list.
 */
typedef struct {
	unsigned int now;
	UserList users;
	ActivityList activities;
	TaskList tasks;
} Kanban;


/******************************************************************************
 * FUNCTION PROTOTYPES                                                        *
 ******************************************************************************/

void setup(Kanban *k);
int select(Kanban *k, char cmd_code, int has_args);

int new_task(TaskList *l);
int list_tasks(TaskList *l, int has_args);
int advance_time(Kanban *k);
int handle_users(UserList *l, int has_args);
int new_user(UserList *l);
int list_users(UserList *l);
int move_task(Kanban *k);
int display_activity(Kanban *k);
int handle_activities(ActivityList *l, int has_args);
int new_activity(ActivityList *l);
int list_activities(ActivityList *l);

int is_new_task_valid(TaskList *l, Task *t);
int is_id_valid(TaskList *l, int id);
int is_time_valid(int time);
int is_new_user_valid(UserList *l, char user[]);
int is_move_valid(Kanban *k, int id, char user[], char activity[]);
int is_activity_valid(ActivityList *l, char activity[]);
int is_new_activity_valid(ActivityList *l, char activity[]);

int is_task_description_duplicate(TaskList *l, Task *t);
int is_existing_user(UserList *l, char user[]);
int is_existing_activity(ActivityList *l, char activity[]);
int str_has_lowercase(char s[]);

void print_task(TaskList *l, int id);
void print_activity(Kanban *k, char activity[], int order[], int sz);

void binary_insert(TaskList *l, int order[], int id, int start, int end);
void append_user(UserList *l, char new_user[]);
void append_activity(ActivityList *l, char new_activity[]);
void append_task(TaskList *l, Task *new_task);


/******************************************************************************
 * META FUNCTIONS                                                             *
 ******************************************************************************/

/*
 * MAIN FUNCTION
 * Setups the Kanban and runs the loop until the user requests to stop.
 *
 * ARGS: none.
 * RETURN (int):
 *     - exit code, EXIT_OK on success.
 */
int main()
{
	int has_args, status = KEEP_GOING;
	char cmd_code;

	Kanban kanban;

	setup(&kanban);

	while (status == KEEP_GOING) {
		cmd_code = getchar();

		if ('\n' != cmd_code) {
			has_args = ('\n' != getchar());
			status = select(&kanban, cmd_code, has_args);
		}
	}

	return EXIT_OK;
}

/*
 * SETUP FUNCTION
 * Setups the Kanban.
 *
 * ARGS:
 *     - Kanban *k: pointer to Kanban.
 * RETURN (void).
 */
void setup(Kanban *k)
{
	k->now = 0;

	k->users.amount = 0;
	k->activities.amount = 0;
	k->tasks.amount = 0;
	k->tasks.amount_started = 0;
	k->tasks.first_at_current_time = 0;

	append_activity(&k->activities, STR_TO_DO);
	append_activity(&k->activities, STR_IN_PROGRESS);
	append_activity(&k->activities, STR_DONE);
}

/*
 * SELECTION FUNCTION
 * Based on the command picks the approprite command handling function.
 *
 * ARGS:
 *     - Kanban *k: pointer to Kanban.
 *     - char cmd_code: command character, the first character in user input.
 *     - char has_args: true if the user input has further arguments.
 * RETURN (int):
 *     - continues the infinite loop if KEEP_GOING.
 */
int select(Kanban *k, char cmd_code, int has_args)
{
	switch (cmd_code) {
		case 'q':
			return STOP;
		case 't':
			return new_task(&k->tasks);
		case 'l':
			return list_tasks(&k->tasks, has_args);
		case 'n':
			return advance_time(k);
		case 'u':
			return handle_users(&k->users, has_args);
		case 'm':
			return move_task(k);
		case 'd':
			return display_activity(k);
		case 'a':
			return handle_activities(&k->activities, has_args);
		default:
			return KEEP_GOING;
	}
}


/******************************************************************************
 * COMMAND HANDLING FUNCTIONS                                                 *
 ******************************************************************************/

/*
 * NEW TASK HANDLING
 * Related command: t <duration> <descritption>
 * Adds a new task to the kanban.
 *
 * ARGS:
 *     - TaskList *l: pointer to the Kanban's task list.
 * RETURN (int):
 *     - continues the infinite loop if KEEP_GOING.
 */
int new_task(TaskList *l)
{
	Task t;

	scanf(STR_MATCH_NEW_TASK, &t.duration, t.description);
	strcpy(t.activity, STR_TO_DO);
	t.start = 0;

	if (is_new_task_valid(l, &t)) {
		append_task(l, &t);
		binary_insert(l, l->ordered_by_description, l->amount,
					  0, l->amount - 2);

		printf(STR_SUCCESS_NEW_TASK, l->amount);
	}

	return KEEP_GOING;
}

/*
 * LIST TASKS HANDLING
 * Related command: l [<id> <id> ...]
 * Lists tasks in kanban.
 *
 * ARGS:
 *     - TaskList *l: pointer to the Kanban's task list.
 * RETURN (int):
 *     - continues the infinite loop if KEEP_GOING.
 */
int list_tasks(TaskList *l, int has_args)
{
	int i, id;

	if (has_args) {
		while (scanf(STR_MATCH_SINGLE_TASK_ID, &id) != 0) {
			if (is_id_valid(l, id))
				print_task(l, id);
		}
	} else {
		for (i = 0; i < l->amount; i++) {
			id = l->ordered_by_description[i] + 1;
			if (is_id_valid(l, id))
				print_task(l, id);
		}
	}

	return KEEP_GOING;
}

/*
 * ADVANCE TIME HANDLING
 * Related command: n <duration>
 * Advance the kanban's current time.
 *
 * ARGS:
 *     - Kanban *k: pointer to Kanban.
 * RETURN (int):
 *     - continues the infinite loop if KEEP_GOING.
 */
int advance_time(Kanban *k)
{
	int time;
	scanf(STR_MATCH_TIME_INCREMENT, &time);

	if (is_time_valid(time)) {
		k->now += time;
		k->tasks.first_at_current_time = k->tasks.amount_started;
		printf(STR_SUCCESS_ADVANCE_TIME, k->now);
	}

	return KEEP_GOING;
}

/*
 * USER HANDLING
 * Related command: u [<user>]
 * Handles the user command.
 *
 * ARGS:
 *     - UserList *l: pointer to the Kanban's user list.
 *     - char has_args: true if the user input has further arguments.
 * RETURN (int):
 *     - continues the infinite loop if KEEP_GOING.
 */
int handle_users(UserList *l, int has_args)
{
	if (has_args)
		return new_user(l);
	else
		return list_users(l);
}

/*
 * NEW USER HANDLING
 * Related command: u <user>
 * Adds new user to kanban.
 *
 * ARGS:
 *     - UserList *l: pointer to the Kanban's user list.
 * RETURN (int):
 *     - continues the infinite loop if KEEP_GOING.
 */
int new_user(UserList *l)
{
	char user[USER_SZ];
	scanf(STR_MATCH_NEW_USER, user);

	if (is_new_user_valid(l, user))
		append_user(l, user);

	return KEEP_GOING;
}

/*
 * LIST USERS HANDLING
 * Related command: u
 * Lists users in kanban.
 *
 * ARGS:
 *     - UserList *l: pointer to the Kanban's user list.
 * RETURN (int):
 *     - continues the infinite loop if KEEP_GOING.
 */
int list_users(UserList *l)
{
	int i;

	for (i = 0; i < l->amount; i++)
		printf(STR_SUCCESS_LIST_USERS, l->user[i]);

	return KEEP_GOING;
}

/*
 * MOVE TASK HANDLING
 * Related command: m <id> <user> <activity>
 * Moves task between activities.
 *
 * ARGS:
 *     - Kanban *k: pointer to Kanban.
 * RETURN (int):
 *     - continues the infinite loop if KEEP_GOING.
 */
int move_task(Kanban *k)
{
	int id, real_duration, slack;
	char user[USER_SZ], activity[ACTIVITY_SZ];
	scanf(STR_MATCH_TASK_MOVE, &id, user, activity);

	if (is_move_valid(k, id, user, activity)) {
		if (strcmp(k->tasks.task[id - 1].activity, STR_TO_DO) == EQUAL) {
			k->tasks.task[id - 1].start = k->now;
			binary_insert(&k->tasks, k->tasks.ordered_by_start, id,
						  k->tasks.first_at_current_time,
						  k->tasks.amount_started - 1);
			k->tasks.amount_started++;
		}
		strcpy(k->tasks.task[id - 1].user, user);
		strcpy(k->tasks.task[id - 1].activity, activity);

		if (strcmp(activity, STR_DONE) == EQUAL) {
			real_duration = k->now - k->tasks.task[id - 1].start;
			slack = real_duration - k->tasks.task[id - 1].duration;
			printf(STR_SUCCESS_MOVE_TASK_TO_DONE, real_duration, slack);
		}
	}

	return KEEP_GOING;
}

/*
 * DISPLAY ACTIVITY HANDLING
 * Related command: d <activity>
 * List all tasks in activity.
 *
 * ARGS:
 *     - Kanban *k: pointer to Kanban.
 * RETURN (int):
 *     - continues the infinite loop if KEEP_GOING.
 */
int display_activity(Kanban *k)
{
	char activity[ACTIVITY_SZ];
	scanf(STR_MATCH_ACTIVITY, activity);

	if (is_activity_valid(&k->activities, activity)) {
		if (strcmp(activity, STR_TO_DO) == EQUAL)
			print_activity(k, activity,
						   k->tasks.ordered_by_description, k->tasks.amount);
		else
			print_activity(k, activity,
						   k->tasks.ordered_by_start, k->tasks.amount_started);
	}

	return KEEP_GOING;
}

/*
 * ACTIVITY HANDLING
 * Related command: a [<activity>]
 * Handles the activity command.
 *
 * ARGS:
 *     - ActivityList *l: pointer to the Kanban's acitivity list.
 *     - char has_args: true if the user input has further arguments.
 * RETURN (int):
 *     - continues the infinite loop if KEEP_GOING.
 */
int handle_activities(ActivityList *l, int has_args)
{
	if (has_args)
		return new_activity(l);
	else
		return list_activities(l);
}

/*
 * NEW ACTIVITY HANDLING
 * Related command: a <activity>
 * Adds new activity to kanban.
 *
 * ARGS:
 *     - ActivityList *l: pointer to the Kanban's activity list.
 * RETURN (int):
 *     - continues the infinite loop if KEEP_GOING.
 */
int new_activity(ActivityList *l)
{
	char activity[ACTIVITY_SZ];
	scanf(STR_MATCH_ACTIVITY, activity);

	if (is_new_activity_valid(l, activity))
		append_activity(l, activity);

	return KEEP_GOING;
}

/*
 * LIST ACTIVITIES HANDLING
 * Related command: a
 * Lists activities in kanban.
 *
 * ARGS:
 *     - ActivityList *l: pointer to the Kanban's activity list.
 * RETURN (int):
 *     - continues the infinite loop if KEEP_GOING.
 */
int list_activities(ActivityList *l)
{
	int i;

	for(i = 0; i < l->amount; i++)
		printf(STR_SUCCESS_LIST_ACTIVITIES, l->activity[i]);

	return KEEP_GOING;
}


/******************************************************************************
 * ERROR CHECKING FUNCTIONS                                                   *
 ******************************************************************************/

/*
 * CHECK NEW TASK
 * Checks for the following errors related to the new task command:
 *     - too many tasks.
 *     - duplicate description.
 *     - invalid duration.
 *
 * ARGS:
 *     - TaskList *l: pointer to the Kanban's task list.
 *     - Task *t: pointer to the new task that will be checked.
 * RETURN (int):
 *     - returns 1 if there are no errors, 0 otherwise.
 */
int is_new_task_valid(TaskList *l, Task *t)
{
	if (l->amount >= AMT_TASKS)
		printf(STR_FAIL_NEW_TASK_TOO_MANY_TASKS);
	else if (is_task_description_duplicate(l, t))
		printf(STR_FAIL_NEW_TASK_DUPLICATE_DESCRIPTION);
	else if (t->duration <= 0)
		printf(STR_FAIL_NEW_TASK_INVALID_DURATION);
	else
		return 1;

	return 0;
}

/*
 * CHECK ID
 * Checks for the following errors related to the list tasks command:
 *     - <id>: no such task.
 *
 * ARGS:
 *     - TaskList *l: pointer to the Kanban's task list.
 *     - int id: id to be checked.
 * RETURN (int):
 *     - returns 1 if there are no errors, 0 otherwise.
 */
int is_id_valid(TaskList *l, int id)
{
	if (id < 1 || id > l->amount)
		printf(STR_FAIL_LIST_TASKS_NO_SUCH_TASK, id);
	else
		return 1;

	return 0;
}

/*
 * CHECK TIME
 * Checks for the following errors related to the advance time command:
 *     - invalid time.
 *
 * ARGS:
 *     - int time: time integer to be checked.
 * RETURN (int):
 *     - returns 1 if there are no errors, 0 otherwise.
 */
int is_time_valid(int time)
{
	if (time < 0)
		printf(STR_FAIL_ADVANCE_TIME_INVALID_TIME);
	else
		return 1;

	return 0;
}

/*
 * CHECK NEW USER
 * Checks for the following errors related to the new user command:
 *     - user already exists.
 *     - too many users.
 *
 * ARGS:
 *     - UserList *l: pointer to the Kanban's user list.
 *     - char user[]: user string to be checked.
 * RETURN (int):
 *     - returns 1 if there are no errors, 0 otherwise.
 */
int is_new_user_valid(UserList *l, char user[])
{
	if (is_existing_user(l, user))
		printf(STR_FAIL_NEW_USER_USER_ALREADY_EXISTS);
	else if (l->amount >= AMT_USERS)
		printf(STR_FAIL_NEW_USER_TOO_MANY_USERS);
	else
		return 1;

	return 0;
}

/*
 * CHECK MOVE
 * Checks for the following errors related to the move task command:
 *     - no such task.
 *     - same activity (doesn't print a message).
 *     - task already started.
 *     - no such user.
 *     - no such activity.
 *
 * ARGS:
 *     - Kanban *k: pointer to Kanban.
 *     - int id: id of task that will be moved.
 *     - char user[]: user string to be checked.
 *     - char activity[]: activity string to be checked.
 * RETURN (int):
 *     - returns 1 if there are no errors, 0 otherwise.
 */
int is_move_valid(Kanban *k, int id, char user[], char activity[])
{
	if (id < 1 || id > k->tasks.amount)
		printf(STR_FAIL_MOVE_TASK_NO_SUCH_TASK);
	else if (strcmp(k->tasks.task[id - 1].activity, activity) == EQUAL)
		return 0;
	else if (strcmp(activity, STR_TO_DO) == EQUAL)
		printf(STR_FAIL_MOVE_TASK_TASK_ALREADY_STARTED);
	else if (!is_existing_user(&k->users, user))
		printf(STR_FAIL_MOVE_TASK_NO_SUCH_USER);
	else if (!is_existing_activity(&k->activities, activity))
		printf(STR_FAIL_MOVE_TASK_NO_SUCH_ACTIVITY);
	else
		return 1;

	return 0;
}

/*
 * CHECK ACTIVITY
 * Checks for the following errors related to the display activity command:
 *     - no such activity.
 *
 * ARGS:
 *     - ActivityList *l: pointer to the Kanban's activity list.
 *     - char activity[]: activity string to be checked.
 * RETURN (int):
 *     - returns 1 if there are no errors, 0 otherwise.
 */
int is_activity_valid(ActivityList *l, char activity[])
{
	if (!is_existing_activity(l, activity))
		printf(STR_FAIL_DISPLAY_ACTIVITY_NO_SUCH_ACTIVITY);
	else
		return 1;

	return 0;
}

/*
 * CHECK NEW ACTIVITY
 * Checks for the following errors related to the new activity command:
 *     - duplicate activity.
 *     - invalid description.
 *     - too many activities.
 *
 * ARGS:
 *     - ActivityList *l: pointer to the Kanban's activity list.
 *     - char activity[]: activity string to be checked.
 * RETURN (int):
 *     - returns 1 if there are no errors, 0 otherwise.
 */
int is_new_activity_valid(ActivityList *l, char activity[])
{
	if (is_existing_activity(l, activity))
		printf(STR_FAIL_NEW_ACTIVITY_DUPLICATE_ACTIVITY);
	else if (str_has_lowercase(activity))
		printf(STR_FAIL_NEW_ACTIVITY_INVALID_DESCRIPTION);
	else if (l->amount >= AMT_ACTIVITIES)
		printf(STR_FAIL_NEW_ACTIVITY_TOO_MANY_ACTIVITIES);
	else
		return 1;

	return 0;
}


/******************************************************************************
 * AUXILIARY ERROR CHECKING FUNCTIONS                                         *
 ******************************************************************************/

/*
 * CHECK FOR TASK DESCRIPTION DUPLICATES
 * Checks if there's another task with the same description in the kanban.
 *
 * ARGS:
 *     - TaskList *l: pointer to the Kanban's taks list.
 *     - Task *t: pointer to the task whose description will be checked.
 * RETURN (int):
 *     - returns 1 if task descripion is duplicate, 0 otherwise.
 */
int is_task_description_duplicate(TaskList *l, Task *t)
{
	int i;

	for (i = 0; i < l->amount; i++) {
		if(strcmp(t->description, l->task[i].description) == EQUAL)
			return 1;
	}

	return 0;
}

/*
 * CHECK IF USER EXISTS
 * Check if user is already in the kanban.
 *
 * ARGS:
 *     - UserList *l: pointer to the Kanban's user list.
 *     - char user[]: user string to be checked.
 * RETURN (int):
 *     - returns 1 if user is already in the list, 0 otherwise.
 */
int is_existing_user(UserList *l, char user[])
{
	int i;

	for (i = 0; i < l->amount; i++) {
		if (strcmp(user, l->user[i]) == EQUAL)
			return 1;
	}

	return 0;
}

/*
 * CHECK IF ACTIVITY EXISTS
 * Check if activity is already in the kanban.
 *
 * ARGS:
 *     - ActivityList *l: pointer to the Kanban's activity list.
 *     - char activity[]: activity string to be checked.
 * RETURN (int):
 *     - returns 1 if activity is already in the list, 0 otherwise.
 */
int is_existing_activity(ActivityList *l, char activity[])
{
	int i;

	for (i = 0; i < l->amount; i++) {
		if (strcmp(activity, l->activity[i]) == EQUAL)
			return 1;
	}

	return 0;
}

/*
 * CHECK LOWERCASE
 * Check if string contains lowercase characters.
 *
 * ARGS:
 *     - char s[]: string to be checked.
 * RETURN (int):
 *     - returns 1 if string has any lowercase, 0 otherwise.
 */
int str_has_lowercase(char s[])
{
	int i, sz;

	sz = strlen(s);
	for (i = 0; i < sz; i++) {
		if (islower(s[i]))
			return 1;
	}

	return 0;
}


/******************************************************************************
 * PRINTING FUNCTIONS                                                         *
 ******************************************************************************/

/*
 * PRINT TASK
 * Print the task with the given id.
 *
 * ARGS:
 *     - TaskList *l: pointer to the Kanban's task list.
 *     - int id: id of the task to be printed.
 * RETURN (void).
 */
void print_task(TaskList *l, int id)
{
	printf(STR_SUCCESS_LIST_TASKS, id,
		   l->task[id - 1].activity,
		   l->task[id - 1].duration,
		   l->task[id - 1].description);
}

/*
 * PRINT ACTIVITY
 * Print all tasks in an activity.
 *
 * ARGS:
 *     - Kanban *k: pointer to Kanban.
 *     - char activity[]: string of the activity to be printed.
 *     - int order[]: order in wich the tasks in the activity will be printed.
 *     - int sz: size of order vector.
 * RETURN (void).
 */
void print_activity(Kanban *k, char activity[], int order[], int sz)
{
	int i;
	Task t;

	for (i = 0; i < sz; i++) {
		t = k->tasks.task[order[i]];

		if (strcmp(t.activity, activity) == EQUAL)
			printf(STR_SUCCESS_DISPLAY_ACTIVITY,
				   order[i] + 1, t.start, t.description);
	}
}


/******************************************************************************
 * VECTOR MANIPULATION FUNCTIONS                                              *
 ******************************************************************************/

/*
 * TASK ORDER INSERTION
 * Insert the index of the task with a given id into the order vector.
 *
 * ARGS:
 *     - TaskList *l: pointer to the Kanban's task list.
 *     - int order[]: order vector where the index will be inserted.
 *     - ind id: id of the task whose index will be inserted.
 *     - ind start, end: domain of the binary search.
 * RETURN (void).
 */
void binary_insert(TaskList *l, int order[], int id, int start, int end)
{
	int mid, sz;
	sz = end;

	while (end >= start) {
		mid = (start + end) / 2;

		if (strcmp(l->task[order[mid]].description,
				   l->task[id - 1].description) > 0)
			end = mid - 1;
		else
			start = mid + 1;
	}

	sz -= start - 1;
	memmove(&order[start + 1], &order[start], sizeof(int) * sz);
	order[start] = id - 1;
}

/*
 * APPEND USER
 * Add user to the end of the user list.
 *
 * ARGS:
 *     - UserList *l: pointer to the Kanban's user list.
 *     - char new_user[]: user string to append.
 * RETURN (void).
 */
void append_user(UserList *l, char new_user[])
{
	strcpy(l->user[(l->amount)++], new_user);
}

/*
 * APPEND ACTIVITY
 * Add activity to the end of the activity list.
 *
 * ARGS:
 *     - ActivityList *l: pointer to the Kanban's activity list.
 *     - char new_activity[]: activity string to append.
 * RETURN (void).
 */
void append_activity(ActivityList *l, char new_activity[])
{
	strcpy(l->activity[(l->amount)++], new_activity);
}

/*
 * APPEND TASK
 * Add task to the end of the task list.
 *
 * ARGS:
 *     - TaskList *l: pointer to the Kanban's task list.
 *     - Task *new_task: pointer to the task that will be appended.
 * RETURN (void).
 */
void append_task(TaskList *l, Task *new_task)
{
	memcpy(&l->task[(l->amount)++], new_task, sizeof(Task));
}
