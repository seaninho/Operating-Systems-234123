#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/slab.h>
#include <asm/uaccess.h>
#define LOG_SIZE 100

// Description:
//    Imposes the restrictions for the process with PID= ​pid ​ with respect to the given
//    proc_restriciton_level and the ​restricitions_list.​ The restriction level of this process should
//    be set to ​ proc_restriction_level​. In addition, from this point on, the log should record (at
//    most) the last ​100 ​forbidden activities that this process has performed.
// Parameters:
//    @pid - The process for whom we would like to set the restrictions
//    @proc_restriction_level - The process restriction level. From now on, every invocation of a
//      system call which is configured in ​restrictions_list ​ that has a
//      strictly greater ​restriction_theshold​ than ​proc_restriction_level
//      should fail and will be recorded as a forbidden activity.
//    @restrictions_list - A configuration list in which every item contains a system call
//      number and its desired ​restriction_theshold​.
//      Every system call that is not in this list should not be affected.
//    @list_size - The number of elements in ​restricions_list ​. You may assume that
//      this number always match the actual list size.
//Return values:
//    ● On success: return 0.
//    ● On failure: return -1
//        o If pid<0 errno should contain ​ESRCH
//        o If no such process exists errno should contain ​ESRCH
//        o If proc_restriction_level<0 or proc_restriction_level>2 errno should contain
//          EINVAL
//        o If size<0 errno should contain ​EINVAL
//        o On memory allocation failure errno should contain ​ENOMEM
//        o On memory copy failure errno should contain ​ENOMEM
//        o On any other failure errno should contain ​EINVAL
int​ ​sys_sc_restrict(​pid_t​ pid ,​int​ proc_restriction_level, scr* restrictions_list, ​int​ list_size) {
  struct task_struct* p;
  // Invalid pid
  if (validate_pid(pid, p) == -1) {
    return -ESRCH;
  }
  // Invalid restriction level
  if (proc_restriction_level < 0 || proc_restriction_level > 2) {
    return -EINVAL;
  }
  // Invalid size
  if (list_size < 0){
    return -EINVAL;
  }

  p->log_forbidden_activity = kmalloc(LOG_SIZE*sizeof(fai), GFP_KERNEL);
  // Allocation failure
  if (!(p->log_forbidden_activity)){
	return -ENOMEM;
  }

  // *** TODO: CHECK!!! *** //

  copy_from_user(p->restrictions_list, restrictions_list, list_size*sizeof(scr));
  // Failure in copying
  if (!(p->restrictions_list)) {
	return -ENOMEM;
  }

  // Failure in copying
  if (copy_from_user(p->restrictions_list, restrictions_list, list_size*sizeof(scr)) > 0) {
    return -ENOMEM;
  }

  // ******* //

  p->proc_restriction_level = proc_restriction_level;
  p->restriction_list_size = list_size;
  p->violations = 0;
  p->feature = ON;

  // resetting the log array
  int i = 0;
  for ( ; i<LOG_SIZE ; i++) {
	p->log_forbidden_activity[i].syscall_num = -1 ;
	p->log_forbidden_activity[i].syscall_restriction_threshold = -1 ;
	p->log_forbidden_activity[i].proc_restriction_level = -1 ;
	p->log_forbidden_activity[i].time = -1 ;
  }

  return 0;
}

// Description:
//    Sets a new restriction level for the process with PID= ​pid​. This method only sets a new
//    restriction level regardless of the process’s restrictions list.
//      ● If this method was called in parallel to the ​sc_restrict ​method, then the one that
//        was performed last should takes effect.
//      ● If this method was called more than once than its last invocation takes effect.
// Parameters:
//    @pid - The process for whom we would like to set the restrictions
//    @proc_restriction_level - The process new restriction level
//Return values:
//    ● On success: return 0.
//    ● On failure: return -1
//        o If pid<0 errno should contain ​ESRCH
//        o If no such process exists errno should contain ​ESRCH
//        o If proc_restriction_level<0 or proc_restriction_level>2 errno should contain
//          EINVAL
//        o On any other failure errno should contain ​EINVAL
int​ sys_set_proc_restriction(pid_t​ pid ,​int​ proc_restriction_level) {
  struct task_struct* p;
  // Invalid pid
  if (validate_pid(pid, p) == -1) {
    return -ESRCH;
  }
  // Invalid restriction level
  if (proc_restriction_level < 0 || proc_restriction_level > 2) {
    return -EINVAL;
  }

  p -> proc_restriction_level = proc_restriction_level;
  return 0;
}

// Description:
//    Returns in ​user_mem​ the last ​size ​records of the forbidden activities of the process with
//    PID = ​pid.
//    ** Reading data from the log does not remove or change the log​. ** ​
// Parameters:
//    @pid The - process for whom we would like to set the restrictions
//    @size The - number of records we wish to read from the log
//    @user_mem - An address in the user space into which the method should write the result
//Return values:
//    ● On success: return 0.
//    ● On failure: return -1
//        o If pid<0 errno should contain ​ESRCH
//        o If no such process exists errno should contain ​ESRCH
//        o If size> actual number of records in the log, errno should contain ​EINVAL​.
//        o If size<0 errno should contain ​EINVAL
//        o On memory copy failure errno should contain ​ENOMEM
//        o On any other failure errno should contain ​EINVAL
int sys_get_process_log(​pid_t pid, ​int size, fai* user_mem) {
  struct task_struct* p;
  // Invalid pid
  if (validate_pid(pid, p) == -1) {
    return -ESRCH;
  }
  // record == forbidden activity
  int num_of_records = p->violations;
  // Invalid number of records
  if (size < 0 || size > num_of_records) {
    return -EINVAL;
  }

  // *** TODO: CHECK!!! *** //

  copy_to_user(user_mem, *(p->log_forbidden_activity + num_of_records - size), size*sizeof(fai));
  // Failure in copying
  if (!()){
	return -ENOMEM;
  }

  // Failure in copying
  if (copy_to_user(user_mem, *(p->log_forbidden_activity + num_of_records - size), size*sizeof(fai)) > 0) {
    return -ENOMEM;
  }

  // ******* //

}

int validate_pid(pid_t pid, task_struct* p) {
  // Invalid PID
  if (pid < 0) {
    return -1;
  }
  struct task_struct* tmp;
  tmp = find_task_by_pid(pid);
  // If p is NULL, there is no process with this pid.
  if (tmp == NULL) {
    return -1;
  }
  p = tmp;
  return 0;
}
