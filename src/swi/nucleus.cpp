#include <cstdint>
#include <spdlog/spdlog.h>
#include <swilib/nucleus.h>

/* swilib/nucleus.h */
int NU_Create_Task(NU_TASK *task, CHAR *name, void (*task_entry)(int, void *), unsigned long argc, void *argv, void *stack_address, unsigned long stack_size, OPTION priority, unsigned long time_slice, OPTION preempt, OPTION auto_start) {
	spdlog::debug("{}: not implemented!", __func__);
	return -1;
}

int NU_Delete_Task(NU_TASK *task) {
	spdlog::debug("{}: not implemented!", __func__);
	return -1;
}

int NU_Reset_Task(NU_TASK *task, unsigned long argc, void *argv) {
	spdlog::debug("{}: not implemented!", __func__);
	return -1;
}

int NU_Terminate_Task(NU_TASK *task) {
	spdlog::debug("{}: not implemented!", __func__);
	return -1;
}

int NU_Resume_Task(NU_TASK *task) {
	spdlog::debug("{}: not implemented!", __func__);
	return -1;
}

int NU_Suspend_Task(NU_TASK *task) {
	spdlog::debug("{}: not implemented!", __func__);
	return -1;
}

void NU_Relinquish(void) {
	spdlog::debug("{}: not implemented!", __func__);
}

void NU_Sleep(unsigned long ticks) {
	uint64_t ms = (uint64_t) ticks * 4615LL / 1000LL;
	std::this_thread::sleep_for(std::chrono::milliseconds(ms));
}

OPTION NU_Change_Priority(NU_TASK *task, OPTION new_priority) {
	spdlog::debug("{}: not implemented!", __func__);
	return 0;
}

OPTION NU_Change_Preemption(OPTION preempt) {
	spdlog::debug("{}: not implemented!", __func__);
	return 0;
}

unsigned long NU_Change_Time_Slice(NU_TASK *task, unsigned long time_slice) {
	spdlog::debug("{}: not implemented!", __func__);
	return 0;
}

unsigned long NU_Check_Stack(void) {
	spdlog::debug("{}: not implemented!", __func__);
	return 0;
}

NU_TASK *NU_Current_Task_Pointer(void) {
	spdlog::debug("{}: not implemented!", __func__);
	return NULL;
}

unsigned long NU_Established_Tasks(void) {
	spdlog::debug("{}: not implemented!", __func__);
	return 0;
}

int NU_Task_Information(NU_TASK *task, CHAR *name, DATA_ELEMENT *status, unsigned long *scheduled_count, OPTION *priority, OPTION *preempt, unsigned long *time_slice, void **stack_base, unsigned long *stack_size, unsigned long *minimum_stack) {
	spdlog::debug("{}: not implemented!", __func__);
	return -1;
}

unsigned long NU_Task_Pointers(NU_TASK **pointer_list, unsigned long maximum_pointers) {
	spdlog::debug("{}: not implemented!", __func__);
	return 0;
}

int NU_Create_Mailbox(NU_MAILBOX *mailbox, CHAR *name, OPTION suspend_type) {
	spdlog::debug("{}: not implemented!", __func__);
	return -1;
}

int NU_Delete_Mailbox(NU_MAILBOX *mailbox) {
	spdlog::debug("{}: not implemented!", __func__);
	return -1;
}

int NU_Reset_Mailbox(NU_MAILBOX *mailbox) {
	spdlog::debug("{}: not implemented!", __func__);
	return -1;
}

int NU_Send_To_Mailbox(NU_MAILBOX *mailbox, void *message, unsigned long suspend) {
	spdlog::debug("{}: not implemented!", __func__);
	return -1;
}

int NU_Broadcast_To_Mailbox(NU_MAILBOX *mailbox, void *message, unsigned long suspend) {
	spdlog::debug("{}: not implemented!", __func__);
	return -1;
}

int NU_Receive_From_Mailbox(NU_MAILBOX *mailbox, void *message, unsigned long suspend) {
	spdlog::debug("{}: not implemented!", __func__);
	return -1;
}

unsigned long NU_Established_Mailboxes(void) {
	spdlog::debug("{}: not implemented!", __func__);
	return 0;
}

int NU_Mailbox_Information(NU_MAILBOX *mailbox, CHAR *name, OPTION *suspend_type, OPTION *message_present, unsigned long *tasks_waiting, NU_TASK **first_task) {
	spdlog::debug("{}: not implemented!", __func__);
	return -1;
}

unsigned long NU_Mailbox_Pointers(NU_MAILBOX **pointer_list, unsigned long maximum_pointers) {
	spdlog::debug("{}: not implemented!", __func__);
	return 0;
}

int NU_Create_Queue(NU_QUEUE *queue, CHAR *name, void *start_address, unsigned long queue_size, OPTION message_type, unsigned long message_size, OPTION suspend_type) {
	spdlog::debug("{}: not implemented!", __func__);
	return -1;
}

int NU_Delete_Queue(NU_QUEUE *queue) {
	spdlog::debug("{}: not implemented!", __func__);
	return -1;
}

int NU_Reset_Queue(NU_QUEUE *queue) {
	spdlog::debug("{}: not implemented!", __func__);
	return -1;
}

int NU_Send_To_Front_Of_Queue(NU_QUEUE *queue, void *message, unsigned long size, unsigned long suspend) {
	spdlog::debug("{}: not implemented!", __func__);
	return -1;
}

int NU_Send_To_Queue(NU_QUEUE *queue, void *message, unsigned long size, unsigned long suspend) {
	spdlog::debug("{}: not implemented!", __func__);
	return -1;
}

int NU_Broadcast_To_Queue(NU_QUEUE *queue, void *message, unsigned long size, unsigned long suspend) {
	spdlog::debug("{}: not implemented!", __func__);
	return -1;
}

int NU_Receive_From_Queue(NU_QUEUE *queue, void *message, unsigned long size, unsigned long *actual_size, unsigned long suspend) {
	spdlog::debug("{}: not implemented!", __func__);
	return -1;
}

unsigned long NU_Established_Queues(void) {
	spdlog::debug("{}: not implemented!", __func__);
	return 0;
}

int NU_Queue_Information(NU_QUEUE *queue, CHAR *name, void **start_address, unsigned long *queue_size, unsigned long *available, unsigned long *messages, OPTION *message_type, unsigned long *message_size, OPTION *suspend_type, unsigned long *tasks_waiting, NU_TASK **first_task) {
	spdlog::debug("{}: not implemented!", __func__);
	return -1;
}

unsigned long NU_Queue_Pointers(NU_QUEUE **pointer_list, unsigned long maximum_pointers) {
	spdlog::debug("{}: not implemented!", __func__);
	return 0;
}

int NU_Create_Pipe(NU_PIPE *pipe, CHAR *name, void *start_address, unsigned long pipe_size, OPTION message_type, unsigned long message_size, OPTION suspend_type) {
	spdlog::debug("{}: not implemented!", __func__);
	return -1;
}

int NU_Delete_Pipe(NU_PIPE *pipe) {
	spdlog::debug("{}: not implemented!", __func__);
	return -1;
}

int NU_Reset_Pipe(NU_PIPE *pipe) {
	spdlog::debug("{}: not implemented!", __func__);
	return -1;
}

int NU_Send_To_Front_Of_Pipe(NU_PIPE *pipe, void *message, unsigned long size, unsigned long suspend) {
	spdlog::debug("{}: not implemented!", __func__);
	return -1;
}

int NU_Send_To_Pipe(NU_PIPE *pipe, void *message, unsigned long size, unsigned long suspend) {
	spdlog::debug("{}: not implemented!", __func__);
	return -1;
}

int NU_Broadcast_To_Pipe(NU_PIPE *pipe, void *message, unsigned long size, unsigned long suspend) {
	spdlog::debug("{}: not implemented!", __func__);
	return -1;
}

int NU_Receive_From_Pipe(NU_PIPE *pipe, void *message, unsigned long size, unsigned long *actual_size, unsigned long suspend) {
	spdlog::debug("{}: not implemented!", __func__);
	return -1;
}

unsigned long NU_Established_Pipes(void) {
	spdlog::debug("{}: not implemented!", __func__);
	return 0;
}

int NU_Pipe_Information(NU_PIPE *pipe, CHAR *name, void **start_address, unsigned long *pipe_size, unsigned long *available, unsigned long *messages, OPTION *message_type, unsigned long *message_size, OPTION *suspend_type, unsigned long *tasks_waiting, NU_TASK **first_task) {
	spdlog::debug("{}: not implemented!", __func__);
	return -1;
}

unsigned long NU_Pipe_Pointers(NU_PIPE **pointer_list, unsigned long maximum_pointers) {
	spdlog::debug("{}: not implemented!", __func__);
	return 0;
}

int NU_Create_Semaphore(NU_SEMAPHORE *semaphore, CHAR *name, unsigned long initial_count, OPTION suspend_type) {
	spdlog::debug("{}: not implemented!", __func__);
	return -1;
}

int NU_Delete_Semaphore(NU_SEMAPHORE *semaphore) {
	spdlog::debug("{}: not implemented!", __func__);
	return -1;
}

int NU_Reset_Semaphore(NU_SEMAPHORE *semaphore, unsigned long initial_count) {
	spdlog::debug("{}: not implemented!", __func__);
	return -1;
}

int NU_Obtain_Semaphore(NU_SEMAPHORE *semaphore, unsigned long suspend) {
	spdlog::debug("{}: not implemented!", __func__);
	return -1;
}

int NU_Release_Semaphore(NU_SEMAPHORE *semaphore) {
	spdlog::debug("{}: not implemented!", __func__);
	return -1;
}

unsigned long NU_Established_Semaphores(void) {
	spdlog::debug("{}: not implemented!", __func__);
	return 0;
}

int NU_Semaphore_Information(NU_SEMAPHORE *semaphore, CHAR *name, unsigned long *current_count, OPTION *suspend_type, unsigned long *tasks_waiting, NU_TASK **first_task) {
	spdlog::debug("{}: not implemented!", __func__);
	return -1;
}

unsigned long NU_Semaphore_Pointers(NU_SEMAPHORE **pointer_list, unsigned long maximum_pointers) {
	spdlog::debug("{}: not implemented!", __func__);
	return 0;
}

int NU_Create_Event_Group(NU_EVENT_GROUP *group, CHAR *name) {
	spdlog::debug("{}: not implemented!", __func__);
	return -1;
}

int NU_Delete_Event_Group(NU_EVENT_GROUP *group) {
	spdlog::debug("{}: not implemented!", __func__);
	return -1;
}

int NU_Set_Events(NU_EVENT_GROUP *group, unsigned long events, OPTION operation) {
	spdlog::debug("{}: not implemented!", __func__);
	return -1;
}

int NU_Retrieve_Events(NU_EVENT_GROUP *group, unsigned long requested_flags, OPTION operation, unsigned long *retrieved_flags, unsigned long suspend) {
	spdlog::debug("{}: not implemented!", __func__);
	return -1;
}

unsigned long NU_Established_Event_Groups(void) {
	spdlog::debug("{}: not implemented!", __func__);
	return 0;
}

int NU_Event_Group_Information(NU_EVENT_GROUP *group, CHAR *name, unsigned long *event_flags, unsigned long *tasks_waiting, NU_TASK **first_task) {
	spdlog::debug("{}: not implemented!", __func__);
	return -1;
}

unsigned long NU_Event_Group_Pointers(NU_EVENT_GROUP **pointer_list, unsigned long maximum_pointers) {
	spdlog::debug("{}: not implemented!", __func__);
	return 0;
}

unsigned long NU_Control_Signals(unsigned long signal_enable_mask) {
	spdlog::debug("{}: not implemented!", __func__);
	return 0;
}

unsigned long NU_Receive_Signals(void) {
	spdlog::debug("{}: not implemented!", __func__);
	return 0;
}

STATUS NU_Register_Signal_Handler(void (*signal_handler)(unsigned long)) {
	spdlog::debug("{}: not implemented!", __func__);
	return 0;
}

STATUS NU_Send_Signals(NU_TASK *task, unsigned long signals) {
	spdlog::debug("{}: not implemented!", __func__);
	return 0;
}

int NU_Create_Partition_Pool(NU_PARTITION_POOL *pool, CHAR *name, void *start_address, unsigned long pool_size, unsigned long partition_size, OPTION suspend_type) {
	spdlog::debug("{}: not implemented!", __func__);
	return -1;
}

int NU_Delete_Partition_Pool(NU_PARTITION_POOL *pool) {
	spdlog::debug("{}: not implemented!", __func__);
	return -1;
}

int NU_Allocate_Partition(NU_PARTITION_POOL *pool, void **return_pointer, unsigned long suspend) {
	spdlog::debug("{}: not implemented!", __func__);
	return -1;
}

int NU_Deallocate_Partition(void *partition) {
	spdlog::debug("{}: not implemented!", __func__);
	return -1;
}

unsigned long NU_Established_Partition_Pools(void) {
	spdlog::debug("{}: not implemented!", __func__);
	return 0;
}

int NU_Partition_Pool_Information(NU_PARTITION_POOL *pool, CHAR *name, void **start_address, unsigned long *pool_size, unsigned long *partition_size, unsigned long *available, unsigned long *allocated, OPTION *suspend_type, unsigned long *tasks_waiting, NU_TASK **first_task) {
	spdlog::debug("{}: not implemented!", __func__);
	return -1;
}

unsigned long NU_Partition_Pool_Pointers(NU_PARTITION_POOL **pointer_list, unsigned long maximum_pointers) {
	spdlog::debug("{}: not implemented!", __func__);
	return 0;
}

int NU_Create_Memory_Pool(NU_MEMORY_POOL *pool, CHAR *name, void *start_address, unsigned long pool_size, unsigned long min_allocation, OPTION suspend_type) {
	spdlog::debug("{}: not implemented!", __func__);
	return -1;
}

int NU_Delete_Memory_Pool(NU_MEMORY_POOL *pool) {
	spdlog::debug("{}: not implemented!", __func__);
	return -1;
}

int NU_Allocate_Memory(NU_MEMORY_POOL *pool, void **return_pointer, unsigned long size, unsigned long suspend) {
	spdlog::debug("{}: not implemented!", __func__);
	return -1;
}

int NU_Deallocate_Memory(void *memory) {
	spdlog::debug("{}: not implemented!", __func__);
	return -1;
}

unsigned long NU_Established_Memory_Pools(void) {
	spdlog::debug("{}: not implemented!", __func__);
	return 0;
}

int NU_Memory_Pool_Information(NU_MEMORY_POOL *pool, CHAR *name, void **start_address, unsigned long *pool_size, unsigned long *min_allocation, unsigned long *available, OPTION *suspend_type, unsigned long *tasks_waiting, NU_TASK **first_task) {
	spdlog::debug("{}: not implemented!", __func__);
	return -1;
}

unsigned long NU_Memory_Pool_Pointers(NU_MEMORY_POOL **pointer_list, unsigned long maximum_pointers) {
	spdlog::debug("{}: not implemented!", __func__);
	return 0;
}

int NU_Control_Interrupts(INT new_level) {
	spdlog::debug("{}: not implemented!", __func__);
	return -1;
}

int NU_Local_Control_Interrupts(INT new_level) {
	spdlog::debug("{}: not implemented!", __func__);
	return -1;
}

void *NU_Setup_Vector(INT vector, void *new_vector) {
	spdlog::debug("{}: not implemented!", __func__);
	return NULL;
}

int NU_Register_LISR(INT vector, void (*new_lisr)(INT), void (**old_lisr)(INT)) {
	spdlog::debug("{}: not implemented!", __func__);
	return -1;
}

int NU_Activate_HISR(NU_HISR *hisr) {
	spdlog::debug("{}: not implemented!", __func__);
	return -1;
}

int NU_Create_HISR(NU_HISR *hisr, CHAR *name, void (*hisr_entry)(void), OPTION priority, void *stack_address, unsigned long stack_size) {
	spdlog::debug("{}: not implemented!", __func__);
	return -1;
}

int NU_Delete_HISR(NU_HISR *hisr) {
	spdlog::debug("{}: not implemented!", __func__);
	return -1;
}

NU_HISR *NU_Current_HISR_Pointer(void) {
	spdlog::debug("{}: not implemented!", __func__);
	return NULL;
}

unsigned long NU_Established_HISRs(void) {
	spdlog::debug("{}: not implemented!", __func__);
	return 0;
}

int NU_HISR_Information(NU_HISR *hisr, CHAR *name, unsigned long *scheduled_count, DATA_ELEMENT *priority, void **stack_base, unsigned long *stack_size, unsigned long *minimum_stack) {
	spdlog::debug("{}: not implemented!", __func__);
	return -1;
}

unsigned long NU_HISR_Pointers(NU_HISR **pointer_list, unsigned long maximum_pointers) {
	spdlog::debug("{}: not implemented!", __func__);
	return 0;
}

void NU_Protect(NU_PROTECT *protect_struct) {
	spdlog::debug("{}: not implemented!", __func__);
}

int NU_Create_Timer(NU_TIMER *timer, CHAR *name, void (*expiration_routine)(unsigned long), unsigned long id, unsigned long initial_time, unsigned long reschedule_time, OPTION enable) {
	spdlog::debug("{}: not implemented!", __func__);
	return -1;
}

int NU_Delete_Timer(NU_TIMER *timer) {
	spdlog::debug("{}: not implemented!", __func__);
	return -1;
}

int NU_Reset_Timer(NU_TIMER *timer, void (*expiration_routine)(unsigned long), unsigned long initial_time, unsigned long reschedule_timer, OPTION enable) {
	spdlog::debug("{}: not implemented!", __func__);
	return -1;
}

int NU_Control_Timer(NU_TIMER *timer, OPTION enable) {
	spdlog::debug("{}: not implemented!", __func__);
	return -1;
}

unsigned long NU_Established_Timers(void) {
	spdlog::debug("{}: not implemented!", __func__);
	return 0;
}

int NU_Timer_Information(NU_TIMER *timer, CHAR *name, OPTION *enable, unsigned long *expirations, unsigned long *id, unsigned long *initial_time, unsigned long *reschedule_time) {
	spdlog::debug("{}: not implemented!", __func__);
	return -1;
}

unsigned long NU_Timer_Pointers(NU_TIMER **pointer_list, unsigned long maximum_pointers) {
	spdlog::debug("{}: not implemented!", __func__);
	return 0;
}

void NU_Set_Clock(unsigned long new_value) {
	spdlog::debug("{}: not implemented!", __func__);
}

unsigned long NU_Retrieve_Clock(void) {
	spdlog::debug("{}: not implemented!", __func__);
	return 0;
}

char *NU_Release_Information(void) {
	spdlog::debug("{}: not implemented!", __func__);
	return NULL;
}

char *NU_License_Information(void) {
	spdlog::debug("{}: not implemented!", __func__);
	return NULL;
}

void NU_Make_History_Entry(unsigned long param1, unsigned long param2, unsigned long param3) {
	spdlog::debug("{}: not implemented!", __func__);
}

int NU_Retrieve_History_Entry(DATA_ELEMENT *id, unsigned long *param1, unsigned long *param2, unsigned long *param3, unsigned long *time, NU_TASK **task, NU_HISR **hisr) {
	spdlog::debug("{}: not implemented!", __func__);
	return -1;
}

int NU_Create_Driver(NU_DRIVER *driver, CHAR *name, void (*driver_entry)(NU_DRIVER *, NU_DRIVER_REQUEST *)) {
	spdlog::debug("{}: not implemented!", __func__);
	return -1;
}

int NU_Delete_Driver(NU_DRIVER *driver) {
	spdlog::debug("{}: not implemented!", __func__);
	return -1;
}

int NU_Request_Driver(NU_DRIVER *driver, NU_DRIVER_REQUEST *request) {
	spdlog::debug("{}: not implemented!", __func__);
	return -1;
}

int NU_Resume_Driver(NU_TASK *task) {
	spdlog::debug("{}: not implemented!", __func__);
	return -1;
}

int NU_Suspend_Driver(void (*terminate_routine)(void *), void *information, unsigned long timeout) {
	spdlog::debug("{}: not implemented!", __func__);
	return -1;
}

unsigned long NU_Established_Drivers(void) {
	spdlog::debug("{}: not implemented!", __func__);
	return 0;
}

unsigned long NU_Driver_Pointers(NU_DRIVER **pointer_list, unsigned long maximum_pointers) {
	spdlog::debug("{}: not implemented!", __func__);
	return 0;
}

unsigned long NU_IS_SUPERVISOR_MODE() {
	spdlog::debug("{}: not implemented!", __func__);
	return 0;
}

void NU_SUPERVISOR_MODE(unsigned long nu_svc_usr_var) {
	spdlog::debug("{}: not implemented!", __func__);
}

void NU_USER_MODE(unsigned long nu_svc_usr_var) {
	spdlog::debug("{}: not implemented!", __func__);
}

void *NU_SUPERV_USER_VARIABLES(void) {
	// spdlog::debug("{}: not implemented!", __func__);
	return NULL;
}
